/*
 *
 *  Copyright (C) 2007-2019, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class to extract pixel data and meta information from JPEG file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2djpgs.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"
#include "dcmtk/dcmdata/dcerror.h"

I2DJpegSource::I2DJpegSource() : m_jpegFileMap(), jpegFile(),
  m_disableProgrTs(OFFalse), m_disableExtSeqTs(OFFalse), m_insistOnJFIF(OFFalse),
  m_keepAPPn(OFFalse), m_lossyCompressed(OFTrue)
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Plugin instantiated");
}


OFString I2DJpegSource::inputFormat() const
{
  return "JPEG";
}

OFCondition I2DJpegSource::openFile(const OFString &filename)
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Opening JPEG file: " << filename);
  OFCondition cond;
  if (filename.empty())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "No JPEG filename specified");

  // Try to open JPEG file
  if ((jpegFile.fopen(filename.c_str(), "rb")) == OFFalse)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to open JPEG file");

  return cond;
}


void I2DJpegSource::setExtSeqSupport(const OFBool enabled)
{
  m_disableExtSeqTs = !enabled;
}


void I2DJpegSource::setProgrSupport(const OFBool enabled)
{
  m_disableProgrTs = !enabled;
}


void I2DJpegSource::setInsistOnJFIF(const OFBool enabled)
{
  m_insistOnJFIF = enabled;
}


void I2DJpegSource::setKeepAPPn(const OFBool enabled)
{
  m_keepAPPn = enabled;
}


OFCondition I2DJpegSource::readPixelData(Uint16& rows,
                                         Uint16& cols,
                                         Uint16& samplesPerPixel,
                                         OFString& photoMetrInt,
                                         Uint16& bitsAlloc,
                                         Uint16& bitsStored,
                                         Uint16& highBit,
                                         Uint16& pixelRepr,
                                         Uint16& planConf,
                                         Uint16& pixAspectH,
                                         Uint16& pixAspectV,
                                         char*&  pixData,
                                         Uint32& length,
                                         E_TransferSyntax &ts)
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Importing JPEG pixel data");
  OFCondition cond = openFile(m_imageFile);
  // return error if file is not open
  if (cond.bad())
  {
    closeFile();
    return cond;
  }
  // Create "map" with byte positions of all JPEG markers in JPEG file
  cond = createJPEGFileMap();
  if (cond.bad())
  {
    clearMap();
    closeFile();
    return cond;
  }

  // Check for image data in file (look for SOF marker)
  E_JPGMARKER jpegEncoding;
  OFListIterator(JPEGFileMapEntry*) entry = m_jpegFileMap.begin();
  while (entry != m_jpegFileMap.end())
  {
    if ( isSOFMarker((*entry)->marker) )
    {
      jpegEncoding = OFstatic_cast(E_JPGMARKER, (*entry)->marker);
      break;
    }
    entry++;
  }
  if (entry == m_jpegFileMap.end())
  {
    closeFile();
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "No image data found in JPEG file");
  }

  // Reject arithmetic and hierarchical (differential) encodings (not supported)
  cond = isJPEGEncodingSupported(jpegEncoding);
  if (cond.bad())
  {
    closeFile();
    return cond;
  }

  // Get transfer syntax associated with the given JPEG encoding
  ts = associatedTS(jpegEncoding);

  // Extract width, height, samples per pixel, bits per sample
  Uint16 width, height, spp, bps;
  cond = getSOFImageParameters(**entry, width, height, spp, bps);
  if (cond.bad())
  {
    closeFile();
    return cond;
  }

  // Return error if invalid image attributes were found
  if (height == 0 || width == 0)
  {
    closeFile();
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Invalid image size (width or height = 0)");
  }

  // Examine JFIF information (version, horizontal and vertical aspect ratio, aspect ratio units.
  Uint16 jfifVersion, unit;
  Uint16 aspectH = 1;
  Uint16 aspectV = 1;
  entry = m_jpegFileMap.begin();
  while (entry != m_jpegFileMap.end())
  {
    if ( (*entry)->marker == E_JPGMARKER_APP0 )
      break;
    entry++;
  }
  if ( entry == m_jpegFileMap.end())
  {
    if (!m_insistOnJFIF)
    {
      DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Ignoring missing JFIF header");
    }
    else
    {
      closeFile();
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "No JFIF information found in JPEG file");
    }
  }
  else
    cond = getJFIFImageParameters(**entry, jfifVersion, aspectH, aspectV, unit);
  if (cond.bad())
  {
    if (!m_insistOnJFIF)
    {
      DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Ignoring errors while evaluating JFIF data");
    }
    else
    {
      closeFile();
      return cond;
    }
  }
  pixAspectH = aspectH;
  pixAspectV = aspectV;

  // Collect information needed for image pixel module
  rows = height;
  cols = width;
  samplesPerPixel = spp;
  bitsAlloc = bps;
  // Some output formats do not allow 12 bit at all, so it is more safe to
  // use 16 bit for Bits Allocated (and therefore for Bits Stored, see below)
  if (bitsAlloc == 12)
  {
    bitsAlloc = 16;
  }
  bitsStored = bitsAlloc;
  highBit = OFstatic_cast(Uint16, bitsStored - 1);
  if (samplesPerPixel == 1)
    photoMetrInt = "MONOCHROME2";
  else if (samplesPerPixel == 3)
    photoMetrInt = "YBR_FULL_422";
  else
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "For JPEG data, Samples per Pixel must be 1 or 3");
  // Planar Configuration and Pixel Representation is always 0 for JPEG data
  planConf = 0;
  pixelRepr = 0;

  Uint32 tLength = 0;
  char* tPixelData = NULL;
  // Keep all APPx info (however, JFIF is always removed)
  if (m_keepAPPn)
    cond = copyJPEGStream(tPixelData, tLength);
  // Cut off all APPx information from JPEG and get raw JPEG bit stream into memory
  else
    cond = extractRawJPEGStream(tPixelData, tLength);
  if (cond.bad())
  {
    closeFile();
      return cond;
  }
  length = tLength;
  pixData = tPixelData;
  return cond;
}


OFCondition I2DJpegSource::getLossyComprInfo(OFBool& srcEncodingLossy,
                                             OFString& srcLossyComprMethod) const
{
  if (m_lossyCompressed)
  {
    srcEncodingLossy = OFTrue;
    srcLossyComprMethod = "ISO_10918_1"; // Defined term for JPEG Lossy Compression
  }
  else
  {
    // (will never get here, no lossless input supported so far)
    srcEncodingLossy = OFFalse;
  }
  return EC_Normal;
}


OFCondition I2DJpegSource::getSOFImageParameters( const JPEGFileMapEntry& entry,
                                                  Uint16& imageWidth,
                                                  Uint16& imageHeight,
                                                  Uint16& samplesPerPixel,
                                                  Uint16& bitsPerSample)
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Examining JPEG SOF image parameters");
  if ( (entry.marker < E_JPGMARKER_SOF0) || (entry.marker > E_JPGMARKER_SOF15) )
    return EC_IllegalCall;
  Uint16 length;
  Uint16 image_height, image_width;
  Uint8 data_precision, num_components;
  int result;

  // seek to the given SOFn marker

  jpegFile.fseek(entry.bytePos, SEEK_SET);
  result = read2Bytes(length);  /* usual parameter length count */
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");

  // read values

  result = read1Byte(data_precision);
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");

  result = read2Bytes(image_height);
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");

  result = read2Bytes(image_width);
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");

  result = read1Byte(num_components);
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");

  imageWidth = image_width;
  imageHeight = image_height;
  samplesPerPixel = num_components;
  bitsPerSample = data_precision;

  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Dumping JPEG SOF image parameters:");
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Image Width: " << image_width);
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Image Height: " << image_height);
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Number of Components: " << OFstatic_cast(unsigned int, num_components));
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Data Precision: " << OFstatic_cast(unsigned int, data_precision));

  if (length != OFstatic_cast(unsigned int, 8 + num_components * 3))
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Bogus SOF marker length");

  return EC_Normal;
}


// ignores thumbnail data
OFCondition I2DJpegSource::getJFIFImageParameters( const JPEGFileMapEntry& entry,
                                                   Uint16& jfifVersion,
                                                   Uint16& pixelAspectH,
                                                   Uint16& pixelAspectV,
                                                   Uint16& unit)
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Examining JFIF information");
  if (entry.marker != E_JPGMARKER_APP0)
    return EC_IllegalCall;
  Uint16 jv, pah, pav, unt;

  // go to specified byte position and read on to value field
  Uint16 length;
  jpegFile.fseek(entry.bytePos, SEEK_SET);
  int result = read2Bytes(length);  /* usual parameter length count */
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");

  // read and check the 5 byte "JFIF" marker value (X'4A', X'46', X'49', X'46', X'00')
  Uint16 twoBytes;
  result = read2Bytes(twoBytes);
  if ( (result == EOF) || (twoBytes != 0x4a46) )
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Invalid JFIF marker or JFIF marker not found");
  result = read2Bytes(twoBytes);
  if ( (result == EOF) || (twoBytes != 0x4946) )
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Invalid JFIF marker or JFIF marker not found");
  Uint8 oneByte;
  result = read1Byte(oneByte);
  if ( (result == EOF)|| (oneByte != 0x00) )
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Invalid JFIF marker or JFIF marker not found");

  // read JFIF version
  result = read2Bytes(twoBytes);
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
  jv = twoBytes;

  // read pixel aspect ratio unit
  result = read1Byte(oneByte);
  if ( (result == EOF) || (oneByte > 2) )
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
  unt = oneByte;

  // read horizontal aspect ratio (XDensity)
  result = read2Bytes(twoBytes);
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
  pah = twoBytes;

  // read vertical aspect ratio (YDensity)
  result = read2Bytes(twoBytes);
 if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
  pav = twoBytes;

  // prepare return values and return
  jfifVersion = jv;
  pixelAspectH = pah;
  pixelAspectV = pav;
  unit = unt;

  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Dumping some JFIF image parameters:");
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   JFIF version: " << jfifVersion);
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Horizontal Pixel Aspect Ratio " << pixelAspectH);
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Vertical Pixel Aspect Ratio: " << pixelAspectV);
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Units: " << unit);

  return EC_Normal;
}


OFCondition I2DJpegSource::copyJPEGStream(char*& pixelData,
                                          Uint32& pixLength)
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Copying JPEG data from JPEG file");
  /* Calculate length of total stream as found in the file
   * Therefore, look at byte positions from SOI and EOI marker */

  offile_off_t bytePosJFIF = 0;
  offile_off_t bytePosAfterJFIF = 0;
  int marker = 0;

  // determine file size
  offile_off_t result = jpegFile.fseek(0, SEEK_END);
  if (result != 0)
    return EC_IllegalParameter;
  offile_off_t filesize = jpegFile.ftell();

  // Only pixel data up to 2^32 bytes is supported (DICOM) and maximum size for "new" operator = size_t

  if ( ( OFstatic_cast(unsigned long, filesize) > OFstatic_cast(unsigned long, 4294967294UL) ) ||
       ( OFstatic_cast(unsigned long, filesize) > OFstatic_cast(unsigned long, -1) ) )
  {
    DCMDATA_LIBI2D_ERROR("I2DJpegSource: JPEG file length longer than 2^32 bytes (or larger than size_t capacity), aborting");
    return EC_MemoryExhausted;
  }

  // get position of JFIF section and SOI marker
  OFListIterator(JPEGFileMapEntry*) entry = m_jpegFileMap.begin();
  while (entry != m_jpegFileMap.end())
  {
    marker = (*entry)->marker;
    if ( marker == E_JPGMARKER_APP0 )
    {
      DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Skipping application segment APP0");
      bytePosJFIF = (*entry)->bytePos - 1; // include first byte of marker (FF)
      entry++;
      bytePosAfterJFIF = (*entry)->bytePos - 1; // include first byte (FF) of marker after APPn
      break;
    }
    entry++;
  }

  // Go to starting position (SOI marker) of JPEG stream data
  jpegFile.fseek(0, SEEK_SET);

  // Allocate buffer for raw JPEG data
  pixLength = OFstatic_cast(Uint32, filesize - (bytePosAfterJFIF - bytePosJFIF));
  pixelData = new char[pixLength];
  char *currBufferPos = pixelData;

  // exclude JFIF if present
  if (bytePosJFIF != 0)
  {
    // read from SOI to JFIF
    result = jpegFile.fread (currBufferPos, 1, 2);
    if (result != 2)
      return EC_IllegalCall;
    currBufferPos += 2;
    // read from end of JFIF to end of file
    jpegFile.fseek(bytePosAfterJFIF - 1, SEEK_SET); // -1 because offsets start with 0
    result = jpegFile.fread (currBufferPos, 1, OFstatic_cast(size_t, filesize - bytePosAfterJFIF + 1));
    if (result != filesize - bytePosAfterJFIF + 1)
      return EC_IllegalCall;
  }
  else // otherwise copy everything starting with SOI marker
  {
    result = jpegFile.fread (currBufferPos, 1, OFstatic_cast(size_t, filesize));
    if (result != filesize)
      return EC_IllegalCall;
  }
  return EC_Normal;
}


// expects valid JPEG stream (especially exactly one SOI and one EOI marker)
OFCondition I2DJpegSource::extractRawJPEGStream(char*& pixelData,
                                                Uint32& pixLength)
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Extracting JPEG data from JPEG file");
  OFCondition cond;
  int marker = 0;
  Uint16 length;

  /* Calculate length of total stream as found in the file
   * Therefore, look at byte positions from SOI and EOI marker and
   * and exclude all APPn markers in calculation
   */

  offile_off_t bytePosSOI = 0;
  offile_off_t bytePosEOI = 0;
  offile_off_t totalAPPSize = 0;
  OFList<offile_off_t> appPosAndLengths;

  OFListIterator(JPEGFileMapEntry*) entry = m_jpegFileMap.begin();
  while (entry != m_jpegFileMap.end())
  {
    marker = (*entry)->marker;

    if (marker == E_JPGMARKER_SOI)
    {
      bytePosSOI = (*entry)->bytePos;
      // Subtract one byte for the preceding 0xFF of the SOI-Marker
      bytePosSOI -= 1;
    }
    else if (marker == E_JPGMARKER_EOI)
    {
      bytePosEOI = (*entry)->bytePos;
      // No need to subtract / add bytes
      break;
    }
    else if (marker >= E_JPGMARKER_APP0 && marker <= E_JPGMARKER_APP15)
    {
      DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Skipping application segment APP" << (marker - E_JPGMARKER_APP0));
      jpegFile.fseek((*entry)->bytePos - jpegFile.ftell(), SEEK_CUR);
      int result = read2Bytes( length);
      if (result == EOF)
      {
        jpegFile.fclose();
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
      }
      // remember pos and length of APP data so we don't need a second "scan" for that
      appPosAndLengths.push_back( (*entry)->bytePos - 1 ); // -1 for FF of marker
      appPosAndLengths.push_back( length );
      // add length of marker value to total APP size
      totalAPPSize += length;
      // add the marker length itself to total APP size
      totalAPPSize += 2;
    }
    // Advance to next segment
    entry++;
  }

  if ( (entry == m_jpegFileMap.end()) || (bytePosSOI == 0) || (bytePosEOI == 0)) // at least end marker was not found
    return EC_IllegalCall;

  offile_off_t rawStreamSize = bytePosEOI - bytePosSOI - totalAPPSize;
  // Start position n and endpos. m results in a total amount of m-n+1 bytes
  rawStreamSize++;

  // Allocate buffer for raw JPEG data
  // Only pixel data up to 2^32 bytes is supported (DICOM)
  if ( ( OFstatic_cast(unsigned long, rawStreamSize) > OFstatic_cast(unsigned long, 4294967294UL) ) ||
       ( OFstatic_cast(unsigned long, rawStreamSize) > OFstatic_cast(unsigned long, -1) ) )
  {
    DCMDATA_LIBI2D_ERROR("I2DJpegSource: Raw JPEG stream length longer than 2^32 bytes (or larger than size_t capacity), aborting");
    return EC_MemoryExhausted;
  }
  pixelData = new char[OFstatic_cast(size_t, rawStreamSize)];
  // keep track of current write position in memory buffer
  char *currBufferPos = pixelData;

  // Go to starting position (SOI marker) of JPEG stream data
  jpegFile.fseek(bytePosSOI-1, SEEK_SET);

  /* Copy everything but leave out APP segments
   */
  OFBool finished = OFFalse;
  offile_off_t endOfBlock = 0; offile_off_t startOfNextBlock = 0;
  while (!finished)
  {
    // determine position of the next block to be read
    if ( appPosAndLengths.size() != 0)
    {
      // we have some APP blocks left
      endOfBlock = appPosAndLengths.front() - 1;
      appPosAndLengths.pop_front();
      startOfNextBlock = endOfBlock + appPosAndLengths.front() + 2; // +2 for marker
      appPosAndLengths.pop_front();
    }
    else // we can read to the end
    {
      endOfBlock = bytePosEOI;
      finished = OFTrue;
    }
    // read block
    offile_off_t blockSize = endOfBlock - jpegFile.ftell();
    if (blockSize < 0)
    {
        DCMDATA_LIBI2D_ERROR("Length field in JPEG data bigger than remaining file");
        cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Length field in JPEG data bigger than remaining file");
    }
    if (cond.good())
    {
        size_t result = jpegFile.fread (currBufferPos, 1, OFstatic_cast(size_t, blockSize));
        if (result != OFstatic_cast(size_t, blockSize))
            cond = EC_IllegalCall;
        else if (!finished)
        {
            jpegFile.fseek(startOfNextBlock, SEEK_SET);
            currBufferPos += blockSize;
        }
    }
  }
  // update result variable
  pixLength = OFstatic_cast(Uint32, rawStreamSize);
  if (cond.bad())
  {
    delete[] pixelData;
  }
  return cond;
}


OFCondition I2DJpegSource::createJPEGFileMap()
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Examing JPEG file and creating map of JPEG markers");
  // clear any old data
  clearMap();
  E_JPGMARKER marker;
  JPEGFileMapEntry *entry = NULL;
  OFBool lastWasSOSMarker = OFFalse;
  OFCondition cond;

  /* Expect SOI at start of file */
  E_JPGMARKER first = E_JPGMARKER();
  cond = firstMarker(first);
  if (cond.bad())
    return cond;
  entry = new JPEGFileMapEntry();
  entry->bytePos = jpegFile.ftell();
  entry->marker = first;
  m_jpegFileMap.push_back(entry);

  if (first != E_JPGMARKER_SOI)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "I2DJpegSource: SOI marker not found at beginning of JPEG stream");

  /* Scan miscellaneous markers until we reach EOI */
  while (cond.good())
  {
    cond = nextMarker(lastWasSOSMarker, marker);
    if (cond.good())
    {
      entry = new JPEGFileMapEntry();
      entry->bytePos = jpegFile.ftell();
      entry->marker = marker;
      m_jpegFileMap.push_back(entry);
      if (marker == E_JPGMARKER_SOS)
      {
        // FIXME: reset this to OFFalse after the next marker?
        lastWasSOSMarker = OFTrue;
      }
      else if (marker == E_JPGMARKER_EOI)
      {
        // End of file reached
        cond = EC_Normal;
        break;
      }
      else if ( isSOFMarker(marker) && m_keepAPPn )
      {
        cond = EC_Normal;
        break;
      }
      if ( !isRSTMarker(marker) ) // RST marker does not have a length that could be used for skipping
        skipVariable();
    }
  } /* end loop */
  debugDumpJPEGFileMap();
  return cond;
}


OFBool I2DJpegSource::isRSTMarker(const E_JPGMARKER& marker)
{
  return ((marker >= E_JPGMARKER_RST0) && (marker <= E_JPGMARKER_RST7));
}


OFBool I2DJpegSource::isSOFMarker(const E_JPGMARKER& marker)
{
  return ( (marker >= E_JPGMARKER_SOF0) && (marker <= E_JPGMARKER_SOF15)
           && (marker != E_JPGMARKER_DHT) &&  (marker != E_JPGMARKER_DAC));
}


OFString I2DJpegSource::jpegMarkerToString(const E_JPGMARKER& marker)
{
  switch (marker)
  {
    case(E_JPGMARKER_SOF0)  :    return "SOF0: Baseline DCT"; break;
    case(E_JPGMARKER_SOF1)  :    return "SOF1: Extended sequential DCT"; break;
    case(E_JPGMARKER_SOF2)  :    return "SOF2: Progressive DCT"; break;
    case(E_JPGMARKER_SOF3)  :    return "SOF3: Lossless (sequential)"; break;
    case(E_JPGMARKER_SOF5)  :    return "SOF5: Differential sequential DCT"; break;
    case(E_JPGMARKER_SOF6)  :    return "SOF6: Differential progressive DCT"; break;
    case(E_JPGMARKER_SOF7)  :    return "SOF7: Differential lossless (sequential)"; break;
    case(E_JPGMARKER_JPG)   :    return "JPG: JPEG extension"; break;
    case(E_JPGMARKER_SOF9)  :    return "SOF9: Extended sequential DCT"; break;
    case(E_JPGMARKER_SOF10) :    return "SOF10: Progressive DCT"; break;
    case(E_JPGMARKER_SOF11) :    return "SOF11: Lossless (sequential)"; break;
    case(E_JPGMARKER_SOF13) :    return "SOF13: Differential sequential DCT"; break;
    case(E_JPGMARKER_SOF14) :    return "SOF14: Differential progressive DCT"; break;
    case(E_JPGMARKER_SOF15) :    return "SOF15: Differential lossless (sequential)"; break;
    case(E_JPGMARKER_DHT)   :    return "DHT: Huffman table(s)"; break;
    case(E_JPGMARKER_DAC)   :    return "DAC: Arithmetic coding conditioning(s)"; break;
    case(E_JPGMARKER_SOI)   :    return "SOI: Start of image"; break;
    case(E_JPGMARKER_EOI)   :    return "EOI: End of image"; break;
    case(E_JPGMARKER_SOS)   :    return "SOS: Start of scan"; break;
    case(E_JPGMARKER_DQT)   :    return "DQT: Quantization table(s)"; break;
    case(E_JPGMARKER_DNL)   :    return "DNL: Number of lines"; break;
    case(E_JPGMARKER_DRI)   :    return "DRI: Define restart interval"; break;
    case(E_JPGMARKER_DHP)   :    return "DHP: Hierarchical progression"; break;
    case(E_JPGMARKER_EXP)   :    return "EXP: Expand reference component(s)"; break;
    case(E_JPGMARKER_COM)   :    return "COM: Comment"; break;
    case(E_JPGMARKER_TEM)   :    return "TEM: For temporary private use in arithmetic coding"; break;

    default:
      if ((marker >= E_JPGMARKER_RST0) && (marker <= E_JPGMARKER_RST7))    {return "RSTn: Restart interval termination"; break;}
      if ((marker >= E_JPGMARKER_APP0) && (marker <= E_JPGMARKER_APP15))   {return "APPn: Application segment"; break;}
      if ((marker >= E_JPGMARKER_JPGN0) && (marker <= E_JPGMARKER_JPGN13)) {return "JPGn: JPEG extension"; break;}
      if ((marker >= E_JPGMARKER_RES0) && (marker <= E_JPGMARKER_RESN))    {return "RESn"; break;}
  }
  OFString msg = "UNKNOWN MARKER :";
  char hexNo[10];
  sprintf(hexNo,"%x2", marker);
  msg += hexNo;
  return msg;
}


/* Read one byte, testing for EOF */
int I2DJpegSource::read1Byte(Uint8& result)
{
  int c;
  c = jpegFile.fgetc();
  if (c == EOF)
    return EOF;
  result = OFstatic_cast(Uint8, c);
  return 0;
}


/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
int I2DJpegSource::read2Bytes(Uint16& result)
{
  int c1, c2;
  c1 = jpegFile.fgetc();
  if (c1 == EOF)
    return EOF;
  c2 = jpegFile.fgetc();
  if (c2 == EOF)
    return EOF;
  result = OFstatic_cast(Uint16, ((OFstatic_cast(Uint16, c1)) << 8) + OFstatic_cast(Uint16, c2));
  return 0;
}



/*
 * Find the next JPEG marker and return its marker code.
 * We expect at least one FF byte, possibly more if the compressor used FFs
 * to pad the file.
 * There could also be non-FF garbage between markers.  The treatment of such
 * garbage is unspecified; we choose to skip over it but emit a warning msg.
 */

OFCondition I2DJpegSource::nextMarker(const OFBool& lastWasSOSMarker,
                                      E_JPGMARKER& result)
{
  Uint8 c;
  int discarded_bytes = 0;
  int oneByte;

  do {
    /* Find 0xFF byte; count and skip any non-FFs. */
    oneByte = read1Byte(c);
    if (oneByte == EOF)
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");

    while (c != 0xFF)
    {
      if (!lastWasSOSMarker)
        discarded_bytes++;
      oneByte = read1Byte(c);
      if (oneByte == EOF)
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
    }
    /* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
     * are legal as pad bytes, so don't count them in discarded_bytes.
     */
    do {
      oneByte = read1Byte(c);
      if (oneByte == EOF)
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
    } while (c == 0xFF);
  } while (lastWasSOSMarker && c == 0x00);

  if (discarded_bytes != 0) {
    DCMDATA_LIBI2D_WARN("garbage data found in JPEG file");
  }
  result = OFstatic_cast(E_JPGMARKER, c);
  return EC_Normal;
}


/*
 * Read the initial marker, which should be SOI.
 * For a JFIF file, the first two bytes of the file should be literally
 * 0xFF M_SOI.
 */

OFCondition I2DJpegSource::firstMarker(E_JPGMARKER& result)
{
  Uint8 c1, c2;

  c1 = OFstatic_cast(Uint8, jpegFile.fgetc());
  c2 = OFstatic_cast(Uint8, jpegFile.fgetc());
  if (c1 != 0xFF || c2 != E_JPGMARKER_SOI) {
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Not a JPEG file");
  }
  result = OFstatic_cast(E_JPGMARKER, c2);
  return EC_Normal;
}

/*
 * Most types of marker are followed by a variable-length parameter segment.
 * This routine skips over the parameters for any marker we don't otherwise
 * want to process.
 * Note that we MUST skip the parameter segment explicitly in order not to
 * be fooled by 0xFF bytes that might appear within the parameter segment;
 * such bytes do NOT introduce new markers.
 */

OFCondition I2DJpegSource::skipVariable()
/* Skip over an unknown or uninteresting variable-length marker */
{
  Uint16 length;

  /* Get the marker parameter length count */
  int result = read2Bytes(length);
  if (result == EOF)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Premature EOF in JPEG file");
  /* Length includes itself, so must be at least 2 */
  if (length < 2)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Erroneous JPEG marker length");
  length = OFstatic_cast(Uint16, length - 2);
  /* Skip over the remaining bytes */
  jpegFile.fseek(length, SEEK_CUR);
  return EC_Normal;
}


OFCondition I2DJpegSource::isJPEGEncodingSupported(const E_JPGMARKER& jpegEncoding) const
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Checking whether JPEG encoding is supported");
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Encoding: " << jpegMarkerToString(jpegEncoding));
  switch (jpegEncoding)
  {
    case E_JPGMARKER_SOF0: // Baseline
      return EC_Normal;
    case E_JPGMARKER_SOF1: // Extended sequential
      if (!m_disableExtSeqTs)
        return EC_Normal;
      else
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to convert: Extended sequential JPEG coding found but support disabled");
    case E_JPGMARKER_SOF2: // Progressive
      if (!m_disableProgrTs)
        return EC_Normal;
      else
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to convert: Progressive JPEG coding found but disabled");
    // SOF3: Lossless, SOF5-7: Hierarchical (differential), SOF9-15: Arithmetic coding, all other
    default:
      OFString errMsg("JPEG data with encoding: '");
      errMsg += jpegMarkerToString(jpegEncoding);
      errMsg += "' not supported";
      return makeOFCondition(OFM_dcmdata, 18, OF_error, errMsg.c_str());
  }
  return EC_Normal;
}


E_TransferSyntax I2DJpegSource::associatedTS(const E_JPGMARKER& jpegEncoding)
{
  switch (jpegEncoding)
  {
    case E_JPGMARKER_SOF0: // Baseline
      return EXS_JPEGProcess1;
    case E_JPGMARKER_SOF1: // Extended Sequential
      return EXS_JPEGProcess2_4;
    case E_JPGMARKER_SOF2: // Progressive
      return EXS_JPEGProcess10_12;
    default:
      return EXS_Unknown;
  }
}


void I2DJpegSource::debugDumpJPEGFileMap() const
{
  if (!DCM_dcmdataLibi2dLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    return;

  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Dumping JPEG marker file map:");
  if (m_keepAPPn)
    DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Keep APPn option enabled, any markers after SOFn marker will not be dumped");
  OFListConstIterator(JPEGFileMapEntry*) it= m_jpegFileMap.begin();
  while (it != m_jpegFileMap.end())
  {
    DCMDATA_LIBI2D_DEBUG("I2DJpegSource:   Byte Position: 0x"
        << STD_NAMESPACE hex << STD_NAMESPACE setw(8) << STD_NAMESPACE setfill('0')
        /* need to cast bytePos to unsigned long to keep VC6 happy */
        << OFstatic_cast(unsigned long, (*it)->bytePos) <<" | Marker: " << jpegMarkerToString( (*it)->marker));
    it++;
  }
}

void I2DJpegSource::clearMap()
{
  // free memory of each map entry and remove them all from list
  JPEGFileMapEntry *entry = NULL;
  while (m_jpegFileMap.size() != 0)
  {
    entry = m_jpegFileMap.front();
    m_jpegFileMap.pop_front();
    delete entry; entry = NULL;
  }
}

// closes underlying JPEG file
void I2DJpegSource::closeFile()
{
  jpegFile.fclose();
}

// close file and free dynamically allocated memory
I2DJpegSource::~I2DJpegSource()
{
  DCMDATA_LIBI2D_DEBUG("I2DJpegSource: Closing JPEG file and cleaning up memory");
  closeFile();
  clearMap();
}

/*
 *
 *  Copyright (C) 2007-2022, OFFIS e.V.
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
 *  Purpose: Implements utility for converting standard image formats to DICOM
 *
 */


#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/libi2d/i2d.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcfilefo.h"  /* for DcmFileFormat */
#include "dcmtk/dcmdata/dcdeftag.h"  /* for DCM_ defines */
#include "dcmtk/dcmdata/dcuid.h"     /* for SITE_SERIES_UID_ROOT */
#include "dcmtk/dcmdata/dcpixseq.h"  /* for DcmPixelSequence */
#include "dcmtk/dcmdata/dcpath.h"    /* for override keys */
#include "dcmtk/dcmdata/dcmxml/xml2dcm.h"   /* for DcmXMLParseHelper */

OFLogger DCM_dcmdataLibi2dLogger = OFLog::getLogger("dcmtk.dcmdata.libi2d");


Image2Dcm::Image2Dcm()
: m_overrideKeys()
, m_templateFile("")
, m_templateFileIsXML(OFFalse)
, m_XMLvalidation(OFFalse)
, m_XMLnamespaceCheck(OFFalse)
, m_readStudyLevel(OFFalse)
, m_readSeriesLevel(OFFalse)
, m_studySeriesFile()
, m_incInstNoFromFile(OFFalse)
, m_disableAttribChecks(OFFalse)
, m_inventMissingType2Attribs(OFTrue)
, m_inventMissingType1Attribs(OFFalse)
, m_rows(0)
, m_cols(0)
, m_samplesPerPixel(0)
, m_bitsAllocated(0)
, m_bitsStored(0)
, m_highBit(0)
, m_pixelRepresentation(0)
, m_planarConfiguration(0)
, m_pixelAspectRatioH(0)
, m_pixelAspectRatioV(0)
, m_frameLength(0)
, m_photometricInterpretation()
, m_compressionRatio(1.0)
, m_conversionFlags(0)
, m_output_buffer(NULL)
, m_offsetList()
, m_pixelSequence(NULL)
, m_offsetTable(NULL)
{
}


Image2Dcm::~Image2Dcm()
{
}


OFCondition Image2Dcm::convertFirstFrame(
  I2DImgSource *inputPlug,
  I2DOutputPlug *outPlug,
  size_t numberOfFrames,
  DcmDataset*& resultDset,
  E_TransferSyntax& proposedTS)
{
  if (!inputPlug || !outPlug || (numberOfFrames < 1))
    return EC_IllegalParameter;

  OFCondition cond;
  resultDset = NULL;
  OFunique_ptr<DcmDataset> tempDataset;
  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Starting conversion of file: " << inputPlug->getImageFile());

  // If specified, copy DICOM template file to export file
  if (!m_templateFile.empty())
  {
    DcmFileFormat dcmff;

#ifdef WITH_LIBXML
    if (m_templateFileIsXML)
    {
      DcmXMLParseHelper parser;
      E_TransferSyntax xfer;
      cond = parser.readXmlFile(m_templateFile.c_str(), dcmff, xfer, OFFalse /* ignore metaheader */, m_XMLnamespaceCheck, m_XMLvalidation, OFTrue /* stop on errors */);
    }
    else
    {
      cond = dcmff.loadFile(m_templateFile.c_str());
    }
#else
    cond = dcmff.loadFile(m_templateFile.c_str());
#endif
    if (cond.bad())
      return cond;

    // remove problematic attributes from dataset
    cleanupTemplate(dcmff.getDataset());
    // copy from input file
    tempDataset.reset(new DcmDataset(*(dcmff.getDataset())));
  }
  else // otherwise, start with an empty DICOM file
    tempDataset.reset(new DcmDataset());
  if (!tempDataset.get())
    return EC_MemoryExhausted;

  // Read patient and study or series information if desired and write to export file
  if (m_readStudyLevel || m_readSeriesLevel)
  {
    cond = applyStudyOrSeriesFromFile(tempDataset.get());
    if (cond.bad())
    {
      return cond;
    }
  }

  // Increment instance number
  if (m_incInstNoFromFile)
  {
    cond = incrementInstanceNumber(tempDataset.get());
    if (cond.bad())
    {
      return cond;
    }
  }

  // Generate and insert UIDs as necessary
  generateUIDs(tempDataset.get());

  // Read and insert pixel data
  m_compressionRatio = 1.0;
  cond = readAndInsertPixelDataFirstFrame(inputPlug, numberOfFrames, tempDataset.get(), proposedTS, m_compressionRatio);
  if (cond.bad())
  {
    return cond;
  }

  // Insert SOP Class specific attributes (and values)
  cond = outPlug->convert(*tempDataset);
  if (cond.bad())
  {
    return cond;
  }

  // Insert SOP Class specific attributes (and values)
  cond = outPlug->insertMultiFrameAttributes(tempDataset.get(), numberOfFrames);
  if (cond.bad())
  {
    return cond;
  }

  // At last, apply override keys on dataset
  cond = applyOverrideKeys(tempDataset.get());
  if (cond.bad())
  {
    return cond;
  }

  // Do some very basic attribute checking (e. g. existence (type 2) and values (type 1))
  if (!m_disableAttribChecks)
  {
    OFString err;
    err = isValid(*tempDataset);
    err += outPlug->isValid(*tempDataset);
    if (!err.empty())
    {
      return makeOFCondition(OFM_dcmdata, 18, OF_error, err.c_str());
    }
  }

  resultDset = tempDataset.release();
  return EC_Normal;
}

OFCondition Image2Dcm::updateLossyCompressionInfo(
  I2DImgSource *inputPlug,
  size_t numberOfFrames,
  DcmDataset *dset)
{
  // Insert Lossy Image Compression and Lossy Image Compression Method attributes if necessary
  OFBool srcIsLossy = OFFalse;
  OFString comprMethod;
  OFCondition cond;
  if (inputPlug->getLossyComprInfo(srcIsLossy, comprMethod).good())
  {
    if (srcIsLossy)
    {
      double compressionRatio = m_compressionRatio;
      if (numberOfFrames > 0) compressionRatio = m_compressionRatio / OFstatic_cast(double, numberOfFrames);
      cond = dset->putAndInsertOFStringArray(DCM_LossyImageCompression, "01");
      if (cond.good() && !comprMethod.empty())
        cond = dset->putAndInsertOFStringArray(DCM_LossyImageCompressionMethod, comprMethod);
      if (cond.good())
      {
        char buf[64];
        OFStandard::ftoa(buf, sizeof(buf), compressionRatio, OFStandard::ftoa_uppercase, 0, 5);
        cond = dset->putAndInsertOFStringArray(DCM_LossyImageCompressionRatio, buf);
      }
      if (cond.bad()) return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write attribute Lossy Image Compression and/or Lossy Image Compression Method to result dataset");
    }
  }
  else
    DCMDATA_LIBI2D_DEBUG("Image2Dcm: No information regarding lossy compression available");
  return EC_Normal;
}

OFCondition Image2Dcm::convertNextFrame(
  I2DImgSource *inputPlug,
  size_t frameNumber)
{
  if (!inputPlug || (frameNumber < 2))
    return EC_IllegalParameter;

  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Starting conversion of file: " << inputPlug->getImageFile());

  // Read and insert pixel data
  return readAndInsertPixelDataNextFrame(inputPlug, frameNumber);
}


void Image2Dcm::setConversionFlags(size_t conversionFlags)
{
  m_conversionFlags = conversionFlags;
}


void Image2Dcm::cleanupTemplate(DcmDataset *targetDset)
{
  if (!targetDset)
    return;
  // Remove any existing image pixel module attribute
  targetDset->findAndDeleteElement(DCM_PixelDataProviderURL);
  targetDset->findAndDeleteElement(DCM_PhotometricInterpretation);
  targetDset->findAndDeleteElement(DCM_SamplesPerPixel);
  targetDset->findAndDeleteElement(DCM_Rows);
  targetDset->findAndDeleteElement(DCM_Columns);
  targetDset->findAndDeleteElement(DCM_BitsAllocated);
  targetDset->findAndDeleteElement(DCM_BitsStored);
  targetDset->findAndDeleteElement(DCM_HighBit);
  targetDset->findAndDeleteElement(DCM_PixelRepresentation);
  targetDset->findAndDeleteElement(DCM_PixelData);
  targetDset->findAndDeleteElement(DCM_PlanarConfiguration);
  targetDset->findAndDeleteElement(DCM_PixelAspectRatio);
  targetDset->findAndDeleteElement(DCM_SmallestImagePixelValue);
  targetDset->findAndDeleteElement(DCM_LargestImagePixelValue);
  targetDset->findAndDeleteElement(DCM_RedPaletteColorLookupTableDescriptor);
  targetDset->findAndDeleteElement(DCM_GreenPaletteColorLookupTableDescriptor);
  targetDset->findAndDeleteElement(DCM_BluePaletteColorLookupTableDescriptor);
  targetDset->findAndDeleteElement(DCM_RedPaletteColorLookupTableData);
  targetDset->findAndDeleteElement(DCM_GreenPaletteColorLookupTableData);
  targetDset->findAndDeleteElement(DCM_BluePaletteColorLookupTableData);
  targetDset->findAndDeleteElement(DCM_ICCProfile);
  // Remove SOP Class / Instance information
  targetDset->findAndDeleteElement(DCM_SOPClassUID);
  targetDset->findAndDeleteElement(DCM_SOPInstanceUID);

}


OFCondition Image2Dcm::applyStudyOrSeriesFromFile(DcmDataset *targetDset)
{
  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Applying study and/or series information from file");
  if ( (!m_readSeriesLevel && !m_readStudyLevel) || m_studySeriesFile.empty() )
    return EC_IllegalCall;

  DcmFileFormat dcmff;
  OFString errMsg;
  OFCondition cond;

  // Open DICOM file to read patient/study/series information from
  cond = dcmff.loadFile(m_studySeriesFile.c_str());
  if (cond.bad())
  {
    errMsg = "Error: Unable to open study / series file "; errMsg += m_studySeriesFile;
    return makeOFCondition(OFM_dcmdata, 18, OF_error, errMsg.c_str());
  }

  DcmDataset *srcDset = NULL;
  srcDset = dcmff.getDataset();
  if (srcDset == NULL)
    return EC_IllegalCall;

  // Determine the specific character set of the target dataset
  OFString value;
  OFString targetCharSet;
  (void) targetDset->findAndGetOFString(DCM_SpecificCharacterSet, targetCharSet);

  // Determine the specific character set of the target dataset of the Study/Series file
  (void) srcDset->findAndGetOFString(DCM_SpecificCharacterSet, value);

  if (targetCharSet == value)
  {
    // case 1: character sets are identical (or both empty), nothing to do.
  }
  else if (value.empty())
  {
    // case 2: target dataset uses an extended character set,
    // while the study/series file uses ASCII. Nothing to do.
  }
  else if (targetCharSet.empty())
  {
    // case 3: the target dataset uses ASCII, while the
    // study/series file uses an extended character set.
    // Use the character set from the study/series file.
    cond = targetDset->putAndInsertOFStringArray(DCM_SpecificCharacterSet, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Specific Character Set to file");
  }
  else
  {
    // case 4: target dataset and study/series files use different
    // extended character sets. We will try to convert the study/series file
    // to the character set used in the target dataset and fail if this
    // does not succeed.
    DCMDATA_LIBI2D_DEBUG("Image2Dcm: target charset: " << targetCharSet << ", study/series file charset: " << value);
    cond = dcmff.convertCharacterSet(targetCharSet, m_conversionFlags);
    if (cond.bad())
    {
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Character set mismatch between template and study/series file");
    }
  }

  // Patient level attributes (type 2 - if value cannot be read, insert empty value
  value.clear();
  srcDset->findAndGetOFString(DCM_PatientName, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientName, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Patient's Name to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_PatientID, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Patient ID to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_PatientSex, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientSex, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Patient's Sex to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_PatientBirthDate, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientBirthDate, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Patient's Birth Date to file");
  value.clear();

  // Study level attributes (type 2 except Study Instance UID)
  cond = srcDset->findAndGetOFString(DCM_StudyInstanceUID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read Study Instance UID (type 1) from file");
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyInstanceUID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Study Instance UID to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_StudyDate, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyDate, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Study Date to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_StudyTime, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyTime, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Study Time to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_ReferringPhysicianName, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_ReferringPhysicianName, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Referring Physician's Name to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_StudyID, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Study ID to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_AccessionNumber, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_AccessionNumber, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to trite Accession Number to file");
  value.clear();

  // Series Level attributes (type 2 except Series Instance UID which is type 1)
  if (m_readSeriesLevel)
  {
    // General Series Module attributes
    cond = srcDset->findAndGetOFString(DCM_SeriesInstanceUID, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read Series Instance UID (type 1) from file");
    cond = targetDset->putAndInsertOFStringArray(DCM_SeriesInstanceUID, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Series Instance UID to file");
    value.clear();

    srcDset->findAndGetOFString(DCM_SeriesNumber, value);
    cond = targetDset->putAndInsertOFStringArray(DCM_SeriesNumber, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Series Number to file");
    value.clear();

    // General Equipment Module attributes
    srcDset->findAndGetOFString(DCM_Manufacturer, value);
    cond = targetDset->putAndInsertOFStringArray(DCM_Manufacturer, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Manufacturer to file");
    value.clear();
  }

  // We need to copy Instance Number if we are supposed to increase it
  if (m_incInstNoFromFile)
  {
    cond = srcDset->findAndGetOFString(DCM_InstanceNumber, value);
    if (cond.bad())
      value = "1";
    cond = targetDset->putAndInsertOFStringArray(DCM_InstanceNumber, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Instance Number to file");
  }

  return EC_Normal;
}


OFCondition Image2Dcm::incrementInstanceNumber(DcmDataset *targetDset)
{
  // Read and increment Instance Number if desired
  if (m_incInstNoFromFile)
  {
    DCMDATA_LIBI2D_DEBUG("Image2Dcm: Trying to read and increment instance number");
    Sint32 instanceNumber;
    if ( targetDset->findAndGetSint32(DCM_InstanceNumber, instanceNumber).good() )
    {
      instanceNumber++;
      char buf[100];
      sprintf(buf, "%ld", OFstatic_cast(long, instanceNumber));
      OFCondition cond = targetDset->putAndInsertOFStringArray(DCM_InstanceNumber, buf);
      if (cond.bad())
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Instance Number to dataset");
    }
    else
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read Instance Number from dataset");
  }
  return EC_Normal;
}


OFCondition Image2Dcm::generateUIDs(DcmDataset *dset)
{
  OFString value;
  OFCondition cond;

  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Generate and insert new UIDs if necessary");
  // Generate and write Series Instance UID if not already present
  if (!m_readSeriesLevel)
  {
    cond = dset->findAndGetOFString(DCM_SeriesInstanceUID, value);
    if (cond.bad() || value.empty())
    {
      char newUID[100];
      dcmGenerateUniqueIdentifier(newUID, SITE_SERIES_UID_ROOT);
      cond = dset->putAndInsertOFStringArray(DCM_SeriesInstanceUID, newUID);
      if (cond.bad())
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Series Instance UID to file");
    }
    value.clear();
  }

  // Generate and write Study Instance UID if not already present
  if (!m_readStudyLevel)
  {
    cond = dset->findAndGetOFString(DCM_StudyInstanceUID, value);
    if (cond.bad() || value.empty())
    {
      char newUID[100];
      dcmGenerateUniqueIdentifier(newUID, SITE_STUDY_UID_ROOT);
      cond = dset->putAndInsertOFStringArray(DCM_StudyInstanceUID, newUID);
      if (cond.bad())
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write Study Instance UID to file");
    }
    value.clear();
  }

  // Generate SOP Instance UID if not already present
  cond = dset->findAndGetOFString(DCM_SOPInstanceUID, value);
  if (cond.bad() || value.empty())
  {
    char newUID[100];
    dcmGenerateUniqueIdentifier(newUID, SITE_INSTANCE_UID_ROOT);
    cond = dset->putAndInsertOFStringArray(DCM_SOPInstanceUID, newUID);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to write SOP Instance UID to file");
  }

  return EC_Normal;
}


OFCondition Image2Dcm::insertEncapsulatedPixelDataFirstFrame(
  DcmDataset* dset,
  char *pixData,
  Uint32 length,
  E_TransferSyntax outputTS)
{
  OFCondition cond;

  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Storing imported pixel data to DICOM file");

  // create initial pixel sequence
  delete m_pixelSequence;
  m_pixelSequence = new DcmPixelSequence(DCM_PixelSequenceTag);

  // insert empty offset table into sequence
  delete m_offsetTable;
  m_offsetTable = new DcmPixelItem(DCM_PixelItemTag);
  cond = m_pixelSequence->insert(m_offsetTable);
  if (cond.bad())
  {
    delete m_offsetTable; m_offsetTable = NULL;
    delete m_pixelSequence; m_pixelSequence = NULL;
    return cond;
  }

  // store compressed frame into pixel sequence
  cond = m_pixelSequence->storeCompressedFrame(m_offsetList, OFreinterpret_cast(Uint8*,pixData), length, 0);
  if (cond.bad())
  {
    delete m_pixelSequence; m_pixelSequence = NULL;
    return cond;
  }

  // insert pixel data attribute incorporating pixel sequence into dataset
  DcmPixelData *pixelData = new DcmPixelData(DCM_PixelData);

  /* tell pixel data element that this is the original presentation of the pixel data
   * pixel data and how it compressed
   */
  pixelData->putOriginalRepresentation(outputTS, NULL, m_pixelSequence);
  cond = dset->insert(pixelData);
  if (cond.bad())
  {
    delete m_pixelSequence; m_pixelSequence = NULL; // also deletes contained pixel sequence
    return cond;
  }

  return EC_Normal;
}


OFCondition Image2Dcm::insertEncapsulatedPixelDataNextFrame(
  char *pixData,
  Uint32 length)
{
  if (m_pixelSequence == NULL || m_offsetTable == NULL) return EC_IllegalCall;

  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Storing imported pixel data to DICOM file");

  // store compressed frame into pixel sequence
  return m_pixelSequence->storeCompressedFrame(m_offsetList, OFreinterpret_cast(Uint8*,pixData), length, 0);
}


OFCondition Image2Dcm::readAndInsertPixelDataFirstFrame(
  I2DImgSource* imgSource,
  size_t numberOfFrames,
  DcmDataset* dset,
  E_TransferSyntax& outputTS,
  double& compressionRatio)
{
  m_pixelAspectRatioH =1;
  m_pixelAspectRatioV = 1;
  outputTS = EXS_Unknown;
  char* pixData = NULL;

  OFCondition cond = imgSource->readPixelData(m_rows, m_cols,
    m_samplesPerPixel, m_photometricInterpretation, m_bitsAllocated, m_bitsStored, m_highBit, m_pixelRepresentation,
    m_planarConfiguration, m_pixelAspectRatioH, m_pixelAspectRatioV, pixData, m_frameLength, outputTS);

  if (cond.bad())
    return cond;

  // compute compression ratio for this frame
  compressionRatio = 1.0;
  double uncompressedSize = m_cols * m_rows;
  uncompressedSize = uncompressedSize * m_bitsStored * m_samplesPerPixel / 8.0;
  if (m_frameLength > 0) compressionRatio = uncompressedSize / m_frameLength;

  DcmXfer transport(outputTS);
  if (transport.isEncapsulated())
  {
    m_offsetList.clear();
    insertEncapsulatedPixelDataFirstFrame(dset, pixData, m_frameLength, outputTS);
    delete[] pixData;
  }
  else
  {
    /* Not encapsulated */
    DcmPixelData *pixelData = new DcmPixelData(DCM_PixelData);
    if (pixelData == NULL)
    {
      delete[] pixData;
      return EC_MemoryExhausted;
    }

    cond = dset->insert(pixelData);
    if (cond.bad())
    {
      delete[] pixData;
      delete pixelData;
      return cond;
    }

    DCMDATA_LIBI2D_DEBUG("Image2Dcm: frame size=" << m_frameLength << ", number of frames=" << numberOfFrames << ", length of pixel data array=" << ((m_frameLength * numberOfFrames + 1)/2)*2);
    Uint16 *array = NULL;
    size_t arraySize = (m_frameLength * numberOfFrames + 1)/2;
    cond = pixelData->createUint16Array(OFstatic_cast(Uint32, arraySize), array);
    if (cond.bad())
    {
      delete[] pixData;
      return cond;
    }
    m_output_buffer = OFreinterpret_cast(char *, array);
    memcpy(array, pixData, m_frameLength);
    delete[] pixData;
  }

  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Inserting Image Pixel module information");

  cond = dset->putAndInsertUint16(DCM_SamplesPerPixel, m_samplesPerPixel);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertOFStringArray(DCM_PhotometricInterpretation, m_photometricInterpretation);
  if (cond.bad())
    return cond;

  // Should only be written if Samples per Pixel > 1
  if (m_samplesPerPixel > 1)
  {
    cond = dset->putAndInsertUint16(DCM_PlanarConfiguration, m_planarConfiguration);
    if (cond.bad())
      return cond;
  }

  cond = dset->putAndInsertUint16(DCM_Rows, m_rows);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_Columns, m_cols);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_BitsAllocated, m_bitsAllocated);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_BitsStored, m_bitsStored);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_HighBit, m_highBit);
  if (cond.bad())
    return cond;

  if ( m_pixelAspectRatioH != m_pixelAspectRatioV )
  {
    char buf[200];
    int err = sprintf(buf, "%u\\%u", m_pixelAspectRatioV, m_pixelAspectRatioH);
    if (err == -1) return EC_IllegalCall;
    cond = dset->putAndInsertOFStringArray(DCM_PixelAspectRatio, buf);
    if (cond.bad())
      return cond;
  }

  return dset->putAndInsertUint16(DCM_PixelRepresentation, m_pixelRepresentation);
}


OFCondition Image2Dcm::readAndInsertPixelDataNextFrame(
  I2DImgSource* imageSource,
  size_t frameNumber)
{

  Uint16 next_pixelAspectRatioH = 1;
  Uint16 next_pixelAspectRatioV = 1;
  Uint16 next_rows = 0;
  Uint16 next_cols = 0;
  Uint16 next_samplesPerPixel = 0;
  Uint16 next_bitsAllocated = 0;
  Uint16 next_bitsStored = 0;
  Uint16 next_highBit = 0;
  Uint16 next_pixelRepresentation = 0;
  Uint16 next_planarConfiguration = 0;
  Uint32 next_frameLength = 0;
  OFString next_photometricInterpretation;

  char *pixData = NULL;
  E_TransferSyntax outputTS;

  OFCondition cond = imageSource->readPixelData(next_rows, next_cols,
    next_samplesPerPixel, next_photometricInterpretation, next_bitsAllocated, next_bitsStored, next_highBit, next_pixelRepresentation,
    next_planarConfiguration, next_pixelAspectRatioH, next_pixelAspectRatioV, pixData, next_frameLength, outputTS);

  if (cond.bad())
    return cond;

  // create a transfer syntax object for the output transfer syntax
  DcmXfer transport(outputTS);

  // check consistency between current frame and first frame
  if (next_rows != m_rows)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of Rows not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_cols != m_cols)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of Columns not equal for all frames of the multi-frame image");
    return cond;
  }

  if (next_samplesPerPixel != m_samplesPerPixel)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of SamplesPerPixel not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_photometricInterpretation != m_photometricInterpretation)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of PhotometricInterpretation not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_bitsAllocated != m_bitsAllocated)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of BitsAllocated not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_bitsStored != m_bitsStored)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of BitsStored not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_highBit != m_highBit)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of HighBit not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_pixelRepresentation != m_pixelRepresentation)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of PixelRepresentation not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_planarConfiguration != m_planarConfiguration)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of PlanarConfiguration not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_pixelAspectRatioH != m_pixelAspectRatioH)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of horizontal PixelAspectRatio not equal for all frames of the multi-frame image");
    return cond;
  }
  if (next_pixelAspectRatioV != m_pixelAspectRatioV)
  {
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: value of vertical PixelAspectRatio not equal for all frames of the multi-frame image");
    return cond;
  }
  if ((!transport.isEncapsulated()) && (next_frameLength != m_frameLength))
  {
    // in the case of uncompressed images, all frames must have exactly the same size.
    // for compressed images, where we store the compressed bitstream as a pixel item, this does not matter.
    cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: frame size not equal for all frames of the multi-frame image");
    return cond;
  }

  // compute compression ratio for this frame
  double compressionRatio = 1.0;
  double uncompressedSize = next_cols * next_rows;
  uncompressedSize = uncompressedSize * next_bitsStored * next_samplesPerPixel / 8.0;
  if (next_frameLength > 0) compressionRatio = uncompressedSize / next_frameLength;

  // accumulate compression ratio in m_compressionRatio.
  // We will divide this by the number of frames later.
  m_compressionRatio += compressionRatio;

  if (transport.isEncapsulated())
  {
    cond = insertEncapsulatedPixelDataNextFrame(pixData, next_frameLength);
    delete[] pixData;
    return cond;
  }
  else
  {
    /* Not encapsulated */
    if (m_output_buffer == NULL)
    {
      cond = makeOFCondition(OFM_dcmdata, 18, OF_error, "Image2Dcm: output buffer not allocated");
      delete[] pixData;
      return cond;
    }

    char *array = m_output_buffer + m_frameLength * (frameNumber - 1);
    memcpy(array, pixData, m_frameLength);
    delete[] pixData;
  }

  return EC_Normal;
}


OFString Image2Dcm::isValid(DcmDataset& dset) const
{
  DCMDATA_LIBI2D_DEBUG("Image2Dcm: Checking validity of DICOM output dataset");
  OFString dummy, err; OFCondition cond;
  // General Patient module attributes
  err += checkAndInventType2Attrib(DCM_PatientName, &dset);
  err += checkAndInventType2Attrib(DCM_PatientSex, &dset);
  err += checkAndInventType2Attrib(DCM_PatientBirthDate, &dset);
  err += checkAndInventType2Attrib(DCM_PatientID, &dset);

  // General Study module attributes
  err += checkAndInventType1Attrib(DCM_StudyInstanceUID, &dset);
  err += checkAndInventType2Attrib(DCM_StudyDate, &dset);
  err += checkAndInventType2Attrib(DCM_StudyTime, &dset);
  err += checkAndInventType2Attrib(DCM_ReferringPhysicianName, &dset);
  err += checkAndInventType2Attrib(DCM_StudyID, &dset);
  err += checkAndInventType2Attrib(DCM_AccessionNumber, &dset);

  // General Series module attributes
  err += checkAndInventType1Attrib(DCM_SeriesInstanceUID, &dset);
  err += checkAndInventType2Attrib(DCM_SeriesNumber, &dset);
  err += checkAndInventType2Attrib(DCM_InstanceNumber, &dset);

  // General Image module attributes
  /* Patient Orientation is of type 2C and must be written if not
     Image Orientation (Patient) (0020,0037) and Image Position (Patient)
     are required for the IOD. The current output IODs (SC, new SC, VLP)
     therefore need Patient Orientation. Make sure any new output plugin
     takes care about this attribute
   */
  err += checkAndInventType2Attrib(DCM_PatientOrientation, &dset);

  // Image Pixel Module
  err += checkAndInventType1Attrib(DCM_Rows, &dset);
  err += checkAndInventType1Attrib(DCM_Columns, &dset);
  err += checkAndInventType1Attrib(DCM_SamplesPerPixel, &dset);
  err += checkAndInventType1Attrib(DCM_PhotometricInterpretation, &dset);
  err += checkAndInventType1Attrib(DCM_BitsAllocated, &dset);
  err += checkAndInventType1Attrib(DCM_BitsStored, &dset);
  err += checkAndInventType1Attrib(DCM_HighBit, &dset);
  err += checkAndInventType1Attrib(DCM_PixelRepresentation, &dset);
  err += checkAndInventType1Attrib(DCM_SOPInstanceUID, &dset);

  return err;
}



void Image2Dcm::setSeriesFrom(const OFString& file)
{
  m_readSeriesLevel = OFTrue;
  m_studySeriesFile = file;
}


void Image2Dcm::setStudyFrom(const OFString& file)
{
  m_readStudyLevel = OFTrue;
  m_studySeriesFile = file;
}


void Image2Dcm::setValidityChecking(OFBool doChecks,
                                    OFBool insertMissingType2,
                                    OFBool inventMissingType1)
{
  m_disableAttribChecks = !doChecks;
  m_inventMissingType2Attribs = insertMissingType2;
  m_inventMissingType1Attribs = inventMissingType1;
}


void Image2Dcm::setTemplateFile(const OFString& file)
{
  m_templateFile = file;
}


void Image2Dcm::setTemplateFileIsXML(OFBool isXML)
{
  m_templateFileIsXML = isXML;
}


void Image2Dcm::setXMLvalidation(OFBool enabled)
{
  m_XMLvalidation = enabled;
}


void Image2Dcm::setXMLnamespaceCheck(OFBool enabled)
{
  m_XMLnamespaceCheck = enabled;
}


void Image2Dcm::setIncrementInstanceNumber(OFBool incInstNo)
{
  m_incInstNoFromFile = incInstNo;
}


void Image2Dcm::setOverrideKeys(const OFList<OFString>& ovkeys)
{
  OFListConstIterator(OFString) it = ovkeys.begin();
  OFListConstIterator(OFString) end = ovkeys.end();
  while (it != end)
  {
    m_overrideKeys.push_back(*it);
    it++;
  }
}


OFCondition Image2Dcm::applyOverrideKeys(DcmDataset *outputDset)

{
  /* replace specific keys by those in overrideKeys, copied from findscu */
  OFListConstIterator(OFString) path = m_overrideKeys.begin();
  OFListConstIterator(OFString) endOfList = m_overrideKeys.end();
  OFCondition cond;
  DcmPathProcessor proc;
  while (path != endOfList)
  {
    cond = proc.applyPathWithValue(outputDset, *path);
    if (cond.bad()) {
      OFString err;
      err += "Bad override key/path: "; err += *path; err += ": "; err += cond.text();
      return makeOFCondition(OFM_dcmdata, 18, OF_error, err.c_str());
    }
    path++;
  }
  return cond;
}


OFString Image2Dcm::checkAndInventType1Attrib(const DcmTagKey& key,
                                              DcmDataset* targetDset,
                                              const OFString& defaultValue) const
{
  OFBool exists = targetDset->tagExists(key);
  if (!exists)
  {
    OFString err = "Image2Dcm: Missing type 1 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
    return err;
  }
  DcmElement *elem;
  OFCondition cond = targetDset->findAndGetElement(key, elem);
  if (cond.bad() || !elem || (elem->getLength() == 0))
  {
    if (!m_inventMissingType1Attribs)
    {
      OFString err = "Image2Dcm: Empty value for type 1 attribute: ";
      err += DcmTag(key).getTagName();
      err += "\n";
      return err;
    }
    // holds element to insert in item
    elem = NULL;
    DcmTag tag(key); OFBool wasError = OFFalse;
    // if DICOM element could be created, insert in to item and modify to value
    if ( DcmItem::newDicomElement(elem, tag).good())
    {
        if (targetDset->insert(elem, OFTrue).good())
        {
          if (elem->putString(defaultValue.c_str()).good())
          {
            DCMDATA_LIBI2D_DEBUG("Image2Dcm: Inserting missing type 1 attribute " << tag.getTagName()
                << " with value " << defaultValue);
          } else wasError = OFTrue;
        } else wasError = OFTrue;
    } else wasError = OFTrue;
    if (wasError)
    {
      OFString err = "Unable to insert type 1 attribute "; err += tag.getTagName(); err += " with value "; err += defaultValue; err += "\n";
      return err;
    }
  }
  return "";
}


OFString Image2Dcm::checkAndInventType2Attrib(const DcmTagKey& key,
                                              DcmDataset* targetDset) const
{
  OFString err;
  OFBool exists = targetDset->tagExists(key);
  if (!exists)
  {
    if (m_inventMissingType2Attribs)
    {
      DcmTag tag(key);
      DCMDATA_LIBI2D_DEBUG("Image2Dcm: Inserting missing type 2 attribute: " << tag.getTagName());
      targetDset->insertEmptyElement(tag);
    }
    else
    {
      err = "Image2Dcm: Missing type 2 attribute: "; err += DcmTag(key).getTagName(); err += "\n";
      return err;
    }
  }
  return err;
}


OFCondition Image2Dcm::updateOffsetTable()
{
  OFCondition result = EC_Normal;
  if (m_offsetTable) result = m_offsetTable->createOffsetTable(m_offsetList);
  return result;
}

/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmPixelItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:16 $
 *  CVS/RCS Revision: $Revision: 1.43 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcwcache.h"    /* for class DcmWriteCache */


// ********************************


DcmPixelItem::DcmPixelItem(const DcmTag &tag,
                           const Uint32 len)
  : DcmOtherByteOtherWord(tag, len)
{
    setTagVR(EVR_pixelItem);
}


DcmPixelItem::DcmPixelItem(const DcmPixelItem &old)
  : DcmOtherByteOtherWord(old)
{
}


OFCondition DcmPixelItem::copyFrom(const DcmObject &rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmPixelItem &, rhs);
  }
  return EC_Normal;
}


DcmPixelItem::~DcmPixelItem()
{
}


// ********************************


OFCondition DcmPixelItem::writeTagAndLength(DcmOutputStream &outStream,
                                            const E_TransferSyntax oxfer,
                                            Uint32 &writtenBytes) const
{
    OFCondition l_error = outStream.status();
    if (l_error.good())
    {
        /* write tag information */
        l_error = writeTag(outStream, getTag(), oxfer);
        writtenBytes = 4;
        /* prepare to write the value field */
        Uint32 valueLength = getLengthField();
        DcmXfer outXfer(oxfer);
        /* check byte-ordering */
        const E_ByteOrder oByteOrder = outXfer.getByteOrder();
        if (oByteOrder == EBO_unknown)
        {
            return EC_IllegalCall;
        }
        swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
        // availability of four bytes space in output buffer
        // has been checked by caller.
        writtenBytes += OFstatic_cast(Uint32, outStream.write(&valueLength, 4));
    } else
        writtenBytes = 0;
    return l_error;
}


void DcmPixelItem::print(STD_NAMESPACE ostream &out,
                         const size_t flags,
                         const int level,
                         const char *pixelFileName,
                         size_t *pixelCounter)
{
    /* call inherited method */
    printPixel(out, flags, level, pixelFileName, pixelCounter);
}


OFCondition DcmPixelItem::createOffsetTable(const DcmOffsetList &offsetList)
{
    OFCondition result = EC_Normal;

    unsigned long numEntries = offsetList.size();
    if (numEntries > 0)
    {
        Uint32 current = 0;
        Uint32 *array = new Uint32[numEntries];
        if (array)
        {
            DCMDATA_DEBUG("DcmPixelItem: creating offset table with " << numEntries << " entries");
            OFListConstIterator(Uint32) first = offsetList.begin();
            OFListConstIterator(Uint32) last = offsetList.end();
            unsigned long idx = 0;
            while ((first != last) && result.good())
            {
                // check for odd offset values, should never happen at this point
                if (current & 1)
                {
                    DCMDATA_WARN("DcmPixelItem: odd frame size (" << current << ") found for frame #"
                        << (idx + 1) << ", cannot create offset table");
                    result = EC_InvalidBasicOffsetTable;
                } else {
                    array[idx++] = current;
                    current += *first;
                    ++first;
                }
            }
            if (result.good())
            {
                result = swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, array, numEntries * sizeof(Uint32), sizeof(Uint32));
                if (result.good())
                    result = putUint8Array(OFreinterpret_cast(Uint8 *, array), numEntries * sizeof(Uint32));
            }
            delete[] array;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DcmPixelItem::writeXML(STD_NAMESPACE ostream&out,
                                   const size_t flags)
{
    /* XML start tag for "item" */
    out << "<pixel-item";
    /* value length in bytes = 0..max */
    out << " len=\"" << getLengthField() << "\"";
    /* value loaded = no (or absent)*/
    if (!valueLoaded())
        out << " loaded=\"no\"";
    /* pixel item contains binary data */
    if (!(flags & DCMTypes::XF_writeBinaryData))
        out << " binary=\"hidden\"";
    else if (flags & DCMTypes::XF_encodeBase64)
        out << " binary=\"base64\"";
    else
        out << " binary=\"yes\"";
    out << ">";
    /* write element value (if loaded) */
    if (valueLoaded() && (flags & DCMTypes::XF_writeBinaryData))
    {
        /* encode binary data as Base64 */
        if (flags & DCMTypes::XF_encodeBase64)
        {
            /* pixel items always contain 8 bit data, therefore, byte swapping not required */
            OFStandard::encodeBase64(out, OFstatic_cast(Uint8 *, getValue()), OFstatic_cast(size_t, getLengthField()));
        } else {
            /* get and check 8 bit data */
            Uint8 *byteValues = NULL;
            if (getUint8Array(byteValues).good() && (byteValues != NULL))
            {
                const unsigned long count = getLengthField();
                out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0');
                /* print byte values in hex mode */
                out << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(byteValues++));
                for (unsigned long i = 1; i < count; i++)
                    out << "\\" << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(byteValues++));
                /* reset i/o manipulators */
                out << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
            }
        }
    }
    /* XML end tag for "item" */
    out << "</pixel-item>" << OFendl;
    /* always report success */
    return EC_Normal;
}


OFCondition DcmPixelItem::writeSignatureFormat(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
  if (dcmEnableOldSignatureFormat.get())
  {
      /* Old signature format as created by DCMTK releases previous to 3.5.4.
       * This is non-conformant because it includes the item length in pixel items.
       */
      return DcmOtherByteOtherWord::writeSignatureFormat(outStream, oxfer, enctype, wcache);
  }
  else
  {
      DcmWriteCache wcache2;

      /* In case the transfer state is not initialized, this is an illegal call */
      if (getTransferState() == ERW_notInitialized)
          errorFlag = EC_IllegalCall;
      else
      {
          /* if this is not an illegal call, we need to do something. First */
          /* of all, check the error state of the stream that was passed */
          /* only do something if the error state of the stream is ok */
          errorFlag = outStream.status();
          if (errorFlag.good())
          {
              /* create an object that represents the transfer syntax */
              DcmXfer outXfer(oxfer);

              /* pointer to element value if value resides in memory or old-style
               * write behaviour is active (i.e. everything loaded into memory upon write
               */
              Uint8 *value = NULL;
              OFBool accessPossible = OFFalse;

              /* check that we actually do have access to the element's value.
               * If the element is unaccessable (which would mean that the value resides
               * in file and access to the file fails), write an empty element with
               * zero length.
               */
              if (getLengthField() > 0)
              {
                if (valueLoaded())
                {
                  /* get this element's value. Mind the byte ordering (little */
                  /* or big endian) of the transfer syntax which shall be used */
                  value = OFstatic_cast(Uint8 *, getValue(outXfer.getByteOrder()));
                  if (value) accessPossible = OFTrue;
                }
                else
                {
                  /* Use local cache object if needed. This may cause those bytes
                   * that are read but not written because the output stream stalls to
                   * be read again, and the file from being re-opened next time.
                   * Therefore, this case should be avoided.
                   */
                  if (!wcache) wcache = &wcache2;

                  /* initialize cache object. This is safe even if the object was already initialized */
                  wcache->init(this, getLengthField(), getTransferredBytes(), outXfer.getByteOrder());

                  /* access first block of element content */
                  errorFlag = wcache->fillBuffer(*this);

                  /* check that everything worked and the buffer is non-empty now */
                  accessPossible = errorFlag.good() && (! wcache->bufferIsEmpty());
                }
              }

              /* if this element's transfer state is ERW_init (i.e. it has not yet been written to */
              /* the stream) and if the outstream provides enough space for tag and length information */
              /* write tag and length information to it, do something */
              if (getTransferState() == ERW_init)
              {
                  /* first compare with DCM_TagInfoLength (12). If there is not enough space
                   * in the buffer, check if the buffer is still sufficient for the requirements
                   * of this element, which may need only 8 instead of 12 bytes.
                   */
                  if (outStream.avail() >= 4)
                  {
                      /* if there is no value, Length (member variable) shall be set to 0 */
                      if (! accessPossible) setLengthField(0);

                      /* write tag and length information (and possibly also data type information) to the stream, */
                      /* mind the transfer syntax and remember the amount of bytes that have been written */
                      errorFlag = writeTag(outStream, getTag(), oxfer);

                      /* if the writing was successful, set this element's transfer */
                      /* state to ERW_inWork and the amount of transferred bytes to 0 */
                      if (errorFlag.good())
                      {
                          setTransferState(ERW_inWork);
                          setTransferredBytes(0);
                      }
                  } else
                      errorFlag = EC_StreamNotifyClient;
              }
              /* if there is a value that has to be written to the stream */
              /* and if this element's transfer state is ERW_inWork */
              if (accessPossible && getTransferState() == ERW_inWork)
              {

                  Uint32 len = 0;
                  if (valueLoaded())
                  {
                      /* write as many bytes as possible to the stream starting at value[getTransferredBytes()] */
                      /* (note that the bytes value[0] to value[getTransferredBytes()-1] have already been */
                      /* written to the stream) */
                      len = OFstatic_cast(Uint32, outStream.write(&value[getTransferredBytes()], getLengthField() - getTransferredBytes()));

                      /* increase the amount of bytes which have been transfered correspondingly */
                      incTransferredBytes(len);

                      /* see if there is something fishy with the stream */
                      errorFlag = outStream.status();
                  }
                  else
                  {
                      Uint32 buflen = 0;
                      OFBool done = getTransferredBytes() == getLengthField();
                      while (! done)
                      {
                        // re-fill buffer from file if empty
                        errorFlag = wcache->fillBuffer(*this);
                        buflen = wcache->contentLength();

                        if (errorFlag.good())
                        {
                          // write as many bytes from cache buffer to stream as possible
                          len = wcache->writeBuffer(outStream);

                          /* increase the amount of bytes which have been transfered correspondingly */
                          incTransferredBytes(len);

                          /* see if there is something fishy with the stream */
                          errorFlag = outStream.status();
                        }

                        // stop writing if something went wrong, we were unable to send all of the buffer content
                        // (which indicates that the output stream needs to be flushed, or everything was sent out.
                        done = errorFlag.bad() || (len < buflen) || (getTransferredBytes() == getLengthField());
                      }
                  }

                  /* if the amount of transferred bytes equals the length of the element's value, the */
                  /* entire value has been written to the stream. Thus, this element's transfer state */
                  /* has to be set to ERW_ready. If this is not the case but the error flag still shows */
                  /* an ok value, there was no more space in the stream and a corresponding return value */
                  /* has to be set. (Isn't the "else if" part superfluous?!?) */
                  if (getLengthField() == getTransferredBytes()) setTransferState(ERW_ready);
                  else if (errorFlag.good()) errorFlag = EC_StreamNotifyClient;
              }
          }
      }
  }

  /* return result value */
  return errorFlag;
}


/*
** CVS/RCS Log:
** $Log: dcpxitem.cc,v $
** Revision 1.43  2010-10-20 16:44:16  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.42  2010-10-14 13:14:08  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.41  2010-05-27 16:54:27  joergr
** Added debug message with details on created basic offset table (if any).
**
** Revision 1.40  2009-11-13 13:11:21  joergr
** Fixed minor issues in log output.
**
** Revision 1.39  2009-11-04 09:58:10  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.38  2009-09-15 15:02:43  joergr
** Enhanced implementation of writeXML() by writing hex numbers directly to the
** output stream instead of creating a temporary string first.
**
** Revision 1.37  2009-05-04 14:29:02  meichel
** Fixed bug in DcmPixelItem::writeSignatureFormat() that caused pixel data
**   to be removed from the dataset when a digital signature was generated
**   for a compressed image.
**
** Revision 1.36  2009-02-04 17:57:19  joergr
** Fixes various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.35  2009-02-04 10:18:57  joergr
** Fixed issue with compressed frames of odd length (possibly wrong values in
** basic offset table).
**
** Revision 1.34  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.33  2007-11-29 14:30:21  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.32  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.31  2007/06/26 16:24:23  joergr
** Added new variant of encodeBase64() method that outputs directly to a stream
** (avoids using a memory buffer for large binary data).
**
** Revision 1.30  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.29  2005/12/08 15:41:27  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.28  2005/11/24 12:50:59  meichel
** Fixed bug in code that prepares a byte stream that is fed into the MAC
**   algorithm when creating or verifying a digital signature. The previous
**   implementation was non-conformant when signatures included compressed
**   (encapsulated) pixel data because the item length was included in the byte
**   stream, while it should not. The global variable dcmEnableOldSignatureFormat
**   and a corresponding command line option in dcmsign allow to re-enable the old
**   implementation.
**
** Revision 1.27  2004/02/04 16:42:42  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.26  2003/06/12 18:21:46  joergr
** Modified code to use const_iterators where appropriate (required for STL).
**
** Revision 1.25  2002/12/06 13:16:59  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.24  2002/11/27 12:06:51  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.23  2002/08/27 16:55:55  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.22  2002/05/24 14:51:51  meichel
** Moved helper methods that are useful for different compression techniques
**   from module dcmjpeg to module dcmdata
**
** Revision 1.21  2002/05/14 08:21:52  joergr
** Added support for Base64 (MIME) encoded binary data.
**
** Revision 1.20  2002/04/25 10:25:49  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.19  2002/04/16 13:43:20  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.18  2001/11/16 15:55:04  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.17  2001/09/25 17:19:53  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.16  2001/06/01 15:49:08  meichel
** Updated copyright header
**
** Revision 1.15  2000/04/14 15:55:06  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.14  2000/03/08 16:26:40  meichel
** Updated copyright header.
**
** Revision 1.13  2000/03/03 14:05:35  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.12  2000/02/23 15:12:00  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.11  2000/02/10 10:52:22  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.10  2000/02/03 16:31:26  joergr
** Fixed bug: encapsulated data (pixel items) have never been loaded using
** method 'loadAllDataIntoMemory'. Therefore, encapsulated pixel data was
** never printed with 'dcmdump'.
** Corrected bug that caused wrong calculation of group length for sequence
** of items (e.g. encapsulated pixel data).
**
** Revision 1.9  2000/02/01 10:12:09  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.8  1999/03/31 09:25:37  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:19  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/07/07 07:52:29  andreas
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.5  1997/07/03 15:10:03  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.4  1997/05/22 16:57:16  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.3  1996/01/05 13:27:41  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/

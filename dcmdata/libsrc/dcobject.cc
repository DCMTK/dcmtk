/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: 
 *  This file contains the interface to routines which provide
 *  DICOM object encoding/decoding, search and lookup facilities.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-08-20 12:18:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcobject.cc,v $
 *  CVS/RCS Revision: $Revision: 1.36 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstream.h"
#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcswap.h"
#include "dcdebug.h"

/*
** Should automatic correction be applied to input data (e.g. stripping
** of padding blanks, removal of blanks in UIDs, etc).
*/
OFGlobal<OFBool> dcmEnableAutomaticInputDataCorrection(OFTrue);

/*
** Handling of illegal odd-length attributes: If flag is true, odd lengths
** are respected (i.e. an odd number of bytes is read from the input stream.)
** After successful reading, padding to even number of bytes is enforced
** by adding a zero pad byte if dcmEnableAutomaticInputDataCorrection is true.
** Otherwise the odd number of bytes remains as read.
**
** If flag is false, old (pre DCMTK 3.5.2) behaviour applies: The length field
** implicitly incremented and an even number of bytes is read from the stream.
*/
OFGlobal<OFBool> dcmAcceptOddAttributeLength(OFTrue);


// ****** public methods **********************************


DcmObject::DcmObject(const DcmTag &tag, const Uint32 len)
  : Tag(tag),    
    Length(len),
    fTransferState(ERW_init),
    errorFlag(EC_Normal),
    fTransferredBytes(0)
{
}


// ********************************


DcmObject::DcmObject( const DcmObject& obj )
  : Tag(obj.Tag),
    Length(obj.Length),
    fTransferState(obj.fTransferState),
    errorFlag(obj.errorFlag),
    fTransferredBytes(obj.fTransferredBytes)
{
}


// ********************************


DcmObject::~DcmObject()
{
}


// ********************************

DcmObject &DcmObject::operator=(const DcmObject &obj)
{
  Tag = obj.Tag;
  Length = obj.Length;
  errorFlag = obj.errorFlag;
  fTransferState = obj.fTransferState;
  fTransferredBytes = obj.fTransferredBytes;
  return *this;
}

// ********************************

void DcmObject::transferInit()
{
  fTransferState = ERW_init;
  fTransferredBytes = 0;
}


// ********************************


void DcmObject::transferEnd()
{
  fTransferState = ERW_notInitialized;
}


// ********************************

DcmObject * DcmObject::nextInContainer(const DcmObject * /*obj*/)
{
    return NULL;
}

// ********************************

OFCondition DcmObject::nextObject(DcmStack & /*stack*/,
                                  const OFBool /*intoSub*/)
{
    return EC_TagNotFound;
}

// ********************************

OFCondition DcmObject::search( const DcmTagKey &/*tag*/,
                               DcmStack &/*resultStack*/,
                               E_SearchMode /*mode*/,
                               OFBool /*searchIntoSub*/ )
{
  return EC_TagNotFound;
}

// ********************************

OFCondition DcmObject::searchErrors( DcmStack &resultStack )
{
  if ( errorFlag != EC_Normal )
    resultStack.push( this );
  return errorFlag;
}

// ********************************

OFCondition DcmObject::writeXML(ostream & /*out*/,
                                const size_t /*flags*/)
{
    return EC_IllegalCall;
}

// ***********************************************************
// ****** protected methods **********************************
// ***********************************************************


void DcmObject::printInfoLine(ostream & out, const OFBool showFullData,
                              const int level, const char *info )
{
  printInfoLine(out, showFullData, level, Tag, Length, info );
}


// ********************************


void DcmObject::printInfoLine(ostream & out, const OFBool showFullData,
                              const int level, DcmTag &tag, 
                              const Uint32 length,
                              const char *info)
{
    DcmVR vr(tag.getVR());

    char output[100+DCM_OptPrintLineLength];
    for (int i=1; i<level; i++) out << "    ";

    if (strlen(info) <= 38)
    {
        sprintf(output, "(%4.4x,%4.4x) %-5.5s %-38s #%6lu,%3lu",
                tag.getGTag(), tag.getETag(), vr.getVRName(), info,
                (unsigned long)length, getVM());
    }
    else 
    {
        sprintf(output, "(%4.4x,%4.4x) %-5.5s ", tag.getGTag(), tag.getETag(), vr.getVRName());
        out << output;
        if (!showFullData && DCM_OptPrintLineLength+10 <= strlen(info))
        {
          strncpy(output, info, (size_t)DCM_OptPrintLineLength);
          strcpy(output+DCM_OptPrintLineLength, "...");
          out << output;
        } else out << info;
        sprintf(output, " #%6lu,%3lu", (unsigned long)length, getVM());
    }
    out << output << "  " << tag.getTagName() << endl;
}


// ********************************


OFCondition DcmObject::writeTag(DcmStream & outStream, const DcmTag & tag, 
                                const E_TransferSyntax oxfer)
    /*
     * This function writes the tag information which was passed to the stream. When
     * writing information, the transfer syntax which was passed is accounted for.
     *
     * Parameters:
     *   outStream - [out] The stream that the information will be written to.
     *   tag       - [in] The tag which shall be written.
     *   oxfer     - [in] The transfer syntax which shall be used.
     */
{
  /* create an object which represents the transfer syntax */
  DcmXfer outXfer(oxfer);

  /* determine the byte ordering */
  const E_ByteOrder outByteOrder = outXfer.getByteOrder();

  /* if the byte ordering is unknown, this is an illegal call (return error) */
  if (outByteOrder == EBO_unknown)
      return EC_IllegalCall;

  /* determine the group number, mind the transfer syntax and */
  /* write the group number value (2 bytes) to the stream */
  Uint16 groupTag = tag.getGTag();
  swapIfNecessary(outByteOrder, gLocalByteOrder, &groupTag, 2, 2);
  outStream.WriteBytes(&groupTag, 2);

  /* determine the element number, mind the transfer syntax and */
  /* write the element number value (2 bytes) to the stream */
  Uint16 elementTag = tag.getETag();    // 2 Byte Laenge; 
  swapIfNecessary(outByteOrder, gLocalByteOrder, &elementTag, 2, 2);
  outStream.WriteBytes(&elementTag, 2);

  /* if the stream reports an error return this error, else return ok */
  if (outStream.GetError() != EC_Normal)
    return outStream.GetError();
  else
    return EC_Normal;
}


OFCondition DcmObject::writeTagAndLength(DcmStream & outStream, 
                                         const E_TransferSyntax oxfer,  
                                         Uint32 & writtenBytes) const
    /*
     * This function writes this DICOM object's tag and length information to the stream. When
     * writing information, the transfer syntax which was passed is accounted for. If the transfer
     * syntax shows an explicit value representation, the data type of this object is also written
     * to the stream. In general, this function follows the rules which are specified in the DICOM
     * standard (see DICOM standard (year 2000) part 5, section 7) (or the corresponding section
     * in a later version of the standard) concerning the encoding of a data set which shall be
     * transmitted.
     *
     * Parameters:
     *   outStream    - [out] The stream that the information will be written to.
     *   oxfer        - [in] The transfer syntax which shall be used.
     *   writtenBytes - [out] Contains in the end the amount of bytes which have been written to the stream.
     */
{
  /* check the error status of the stream. If it is not ok, nothing can be done */
  OFCondition l_error = outStream.GetError();
  if (l_error != EC_Normal)
    writtenBytes = 0;
  else
  {
      /* if the stream is ok, we need to do something */

      /* write the tag information (a total of 4 bytes, group number and element */
      /* number) to the stream. Mind the transfer syntax's byte ordering. */
      l_error = this -> writeTag(outStream, Tag, oxfer);
      writtenBytes = 4;

      /* create an object which represents the transfer syntax */
      DcmXfer oxferSyn(oxfer);

      /* determine the byte ordering */
      const E_ByteOrder oByteOrder = oxferSyn.getByteOrder();

      /* if the byte ordering is unknown, this is an illegal call (return error) */
      if (oByteOrder == EBO_unknown)
          return EC_IllegalCall;

      /* if the transfer syntax is one with explicit value representation */
      /* this value's data type also has to be written to the stream. Do so */
      /* and also write the length information to the stream. */
      if (oxferSyn.isExplicitVR())
        {
          /* Create an object that represents this object's data type */
          DcmVR myvr(getVR());
          
          /* getValidEVR() will convert datatype "UN" to "OB" if generation of "UN" is disabled */
          DcmEVR vr = myvr.getValidEVR();

          /* get name of data type */
          const char *vrname = myvr.getValidVRName();

          /* write data type name to the stream (a total of 2 bytes) */
          outStream.WriteBytes(vrname, 2);
          writtenBytes += 2;

          /* create another data type object on the basis of the above created object */
          DcmVR outvr(vr);

          /* in case we are dealing with a transfer syntax with explicit VR (see if above) */
          /* and the actual VR uses extended length encoding (see DICOM standard (year 2000) */
          /* part 5, section 7.1.2) (or the corresponding section in a later version of the */
          /* standard) we have to add 2 reserved bytes (set to a value of 00H) to the data */
          /* type field and the actual length field is 4 bytes wide. Write the corresponding */
          /* information to the stream. */
          if (outvr.usesExtendedLengthEncoding())
          {
            Uint16 reserved = 0;
            outStream.WriteBytes(&reserved, 2);                                 // write 2 reserved bytes to stream
            Uint32 valueLength = Length;                                        // determine length
            swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);   // mind transfer syntax
            outStream.WriteBytes(&valueLength, 4);                              // write length, 4 bytes wide
            writtenBytes += 6;                                                  // remember that 6 bytes were written in total
          }
          /* in case that we are dealing with a transfer syntax with explicit VR (see if above) and */
          /* the actual VR does not use extended length encoding (see DICOM standard (year 2000) */
          /* part 5, section 7.1.2) (or the corresponding section in a later version of the standard) */
          /* we do not have to add reserved bytes to the data type field and the actual length field */
          /* is 2 bytes wide. Write the corresponding information to the stream. */
          else
          {
            Uint16 valueLength = (Uint16)Length;                                // determine length
            swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 2, 2);   // mind transfer syntax
            outStream.WriteBytes(&valueLength, 2);                              // write length, 2 bytes wide
            writtenBytes += 2;                                                  // remember that 2 bytes were written in total
          }
        }
        /* if the transfer syntax is one with implicit value representation this value's data type */
        /* does not have to be written to the stream. Only the length information has to be written */
        /* to the stream. According to the DICOM standard the length field is in this case always 4 */
        /* byte wide. (see DICOM standard (year 2000) part 5, section 7.1.2) (or the corresponding */
        /* section in a later version of the standard) */
        else
        {
          Uint32 valueLength = Length;                                          // determine length
          swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);     // mind transfer syntax
          outStream.WriteBytes(&valueLength, 4);                                // write length, 4 bytes wide
          writtenBytes += 4;                                                    // remember that 4 bytes were written in total
        }
    }

  /* return result */
  return l_error;
}

OFBool DcmObject::isSignable() const
{ 
  return Tag.isSignable();
}

OFBool DcmObject::containsUnknownVR() const
{ 
  return Tag.isUnknownVR();
}

/*
 * CVS/RCS Log:
 * $Log: dcobject.cc,v $
 * Revision 1.36  2002-08-20 12:18:48  meichel
 * Changed parameter list of loadFile and saveFile methods in class
 *   DcmFileFormat. Removed loadFile and saveFile from class DcmObject.
 *
 * Revision 1.35  2002/07/08 14:44:40  meichel
 * Improved dcmdata behaviour when reading odd tag length. Depending on the
 *   global boolean flag dcmAcceptOddAttributeLength, the parser now either accepts
 *   odd length attributes or implements the old behaviour, i.e. assumes a real
 *   length larger by one.
 *
 * Revision 1.34  2002/04/25 10:17:19  joergr
 * Added support for XML output of DICOM objects.
 *
 * Revision 1.33  2002/04/16 13:43:19  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 * Revision 1.32  2002/04/11 12:27:10  joergr
 * Added new methods for loading and saving DICOM files.
 *
 * Revision 1.31  2001/11/16 15:55:03  meichel
 * Adapted digital signature code to final text of supplement 41.
 *
 * Revision 1.30  2001/11/01 14:55:41  wilkens
 * Added lots of comments.
 *
 * Revision 1.29  2001/09/25 17:19:52  meichel
 * Adapted dcmdata to class OFCondition
 *
 * Revision 1.28  2001/06/01 15:49:06  meichel
 * Updated copyright header
 *
 * Revision 1.27  2000/04/14 16:10:09  meichel
 * Global flag dcmEnableAutomaticInputDataCorrection now derived from OFGlobal
 *   and, thus, safe for use in multi-thread applications.
 *
 * Revision 1.26  2000/03/08 16:26:38  meichel
 * Updated copyright header.
 *
 * Revision 1.25  2000/03/07 15:41:00  joergr
 * Added explicit type casts to make Sun CC 2.0.1 happy.
 *
 * Revision 1.24  2000/02/10 10:52:20  joergr
 * Added new feature to dcmdump (enhanced print method of dcmdata): write
 * pixel data/item value fields to raw files.
 *
 * Revision 1.23  2000/02/01 10:12:09  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.22  1999/03/31 09:25:34  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */

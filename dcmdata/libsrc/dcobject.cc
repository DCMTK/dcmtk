/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-03-07 15:41:00 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcobject.cc,v $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include <iostream.h>
#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcswap.h"
#include "dcdebug.h"

/*
** Should automatic correction be applied to input data (e.g. stripping
** of padding blanks, removal of blanks in UIDs, etc).
*/
OFBool dcmEnableAutomaticInputDataCorrection = OFTrue;



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

E_Condition DcmObject::nextObject(DcmStack & /*stack*/,
                                  const OFBool /*intoSub*/)
{
    return EC_TagNotFound;
}

// ********************************

E_Condition DcmObject::search( const DcmTagKey &/*tag*/,
                               DcmStack &/*resultStack*/,
                               E_SearchMode /*mode*/,
                               OFBool /*searchIntoSub*/ )
{
  return EC_TagNotFound;
}


// ********************************


E_Condition DcmObject::searchErrors( DcmStack &resultStack )
{
  if ( errorFlag != EC_Normal )
    resultStack.push( this );
  return errorFlag;
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
                              const int level, const DcmTag &tag, 
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

E_Condition DcmObject::writeTag(DcmStream & outStream, const DcmTag & tag, 
                                const E_TransferSyntax oxfer)
{
  DcmXfer outXfer(oxfer);
  const E_ByteOrder outByteOrder = outXfer.getByteOrder();
  if (outByteOrder == EBO_unknown)
      return EC_IllegalCall;

  Uint16 groupTag = tag.getGTag();              // 2 Byte Laenge; 
  swapIfNecessary(outByteOrder, gLocalByteOrder, &groupTag, 2, 2);
  outStream.WriteBytes(&groupTag, 2);
        
  Uint16 elementTag = tag.getETag();    // 2 Byte Laenge; 
  swapIfNecessary(outByteOrder, gLocalByteOrder, &elementTag, 2, 2);
  outStream.WriteBytes(&elementTag, 2);
  if (outStream.GetError() != EC_Normal)
    return outStream.GetError();
  else
    return EC_Normal;
}


E_Condition DcmObject::writeTagAndLength(DcmStream & outStream, 
                                         const E_TransferSyntax oxfer,  
                                         Uint32 & writtenBytes) 
{
  E_Condition l_error = outStream.GetError();
  if (l_error != EC_Normal)
    writtenBytes = 0;
  else
    {
      l_error = this -> writeTag(outStream, Tag, oxfer);
      writtenBytes = 4;

      DcmXfer oxferSyn(oxfer);
      const E_ByteOrder oByteOrder = oxferSyn.getByteOrder();
      if (oByteOrder == EBO_unknown)
          return EC_IllegalCall;

      if (oxferSyn.isExplicitVR())
        {
          // Umwandlung in gueltige VR
          DcmVR myvr(getVR()); // what VR should it be
          // getValidEVR() will convert UN to OB if generation of UN disabled
          DcmEVR vr = myvr.getValidEVR();
          // convert to a valid string
          const char *vrname = myvr.getValidVRName();
          outStream.WriteBytes(vrname, 2);    // 2 Bytes of VR name 
          writtenBytes += 2;
          DcmVR outvr(vr);

          if (outvr.usesExtendedLengthEncoding()) {
              Uint16 reserved = 0;
              outStream.WriteBytes(&reserved, 2); // 2 Byte Laenge
              Uint32 valueLength = Length;
              swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
              outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
              writtenBytes += 6;
          } else {
              Uint16 valueLength = (Uint16)Length;
              swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 2, 2);
              outStream.WriteBytes(&valueLength, 2); // 2 Byte Laenge
              writtenBytes += 2;
            }

        } else {
          // is the implicit vr transfer syntax
          Uint32 valueLength = Length;
          swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
          outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
          writtenBytes += 4;
        }
    }
  return l_error;
}


/*
 * CVS/RCS Log:
 * $Log: dcobject.cc,v $
 * Revision 1.25  2000-03-07 15:41:00  joergr
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

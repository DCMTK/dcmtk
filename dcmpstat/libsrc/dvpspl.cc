/*
 *
 *  Copyright (C) 1999-2010, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSPresentationLUT
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:32 $
 *  CVS/RCS Revision: $Revision: 1.31 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmpstat/dvpspl.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmnet/dimse.h"

/* --------------- class DVPSPresentationLUT --------------- */

DVPSPresentationLUT::DVPSPresentationLUT()
: presentationLUT(DVPSP_identity)
, presentationLUTDescriptor(DCM_LUTDescriptor)
, presentationLUTExplanation(DCM_LUTExplanation)
, presentationLUTData(DCM_LUTData)
, sOPInstanceUID(DCM_SOPInstanceUID)
{
}

DVPSPresentationLUT::DVPSPresentationLUT(const DVPSPresentationLUT& copy)
: presentationLUT(copy.presentationLUT)
, presentationLUTDescriptor(copy.presentationLUTDescriptor)
, presentationLUTExplanation(copy.presentationLUTExplanation)
, presentationLUTData(copy.presentationLUTData)
, sOPInstanceUID(copy.sOPInstanceUID)
{
}

DVPSPresentationLUT::~DVPSPresentationLUT()
{
}

void DVPSPresentationLUT::clear()
{
  presentationLUT = DVPSP_identity;
  presentationLUTDescriptor.clear();
  presentationLUTExplanation.clear();
  presentationLUTData.clear();
  sOPInstanceUID.clear();
}

OFCondition DVPSPresentationLUT::read(DcmItem &dset, OFBool withSOPInstance)
{
  DcmSequenceOfItems *seq;
  DcmItem *item;
  OFCondition result = EC_Normal;
  DcmStack stack;
  OFString aString;

  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
  
  READ_FROM_DATASET(DcmCodeString, presentationLUTShape)
  if (withSOPInstance) { READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID) }
  else sOPInstanceUID.clear();
  
  /* read Presentation LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_PresentationLUTSequence, stack, ESM_fromHere, OFFalse))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTDescriptor.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           presentationLUTDescriptor = *((DcmUnsignedShort *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           presentationLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse))
         {
           presentationLUTData = *((DcmUnsignedShort *)(stack.top()));
         }
      } else {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("found Presentation LUT SQ with number of items != 1");
      }
    }
  }


  /* Now perform basic sanity checks */

  if (presentationLUTShape.getLength() == 0)
  {
    presentationLUT = DVPSP_table;

    if (presentationLUTDescriptor.getLength() == 0)
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("presentationLUTShape and presentationLUTDescriptor absent or empty");
    }
    else if (presentationLUTDescriptor.getVM() != 3)
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("presentationLUTDescriptor present but VM != 3 in presentation state");
    }
    if (presentationLUTData.getLength() == 0)
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("presentationLUTShape and presentationLUTData absent or empty");
    }
  } else {
    if (presentationLUTShape.getVM() != 1)
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("presentationLUTShape present but VM != 1");
    } else {
      // check presentation LUT shape
      aString.clear();
      presentationLUTShape.getOFString(aString,0);
      if (aString=="IDENTITY") presentationLUT = DVPSP_identity;
      else if (aString=="INVERSE") presentationLUT = DVPSP_inverse;
      else if (aString=="LIN OD") presentationLUT = DVPSP_lin_od;
      else
      {
        result=EC_IllegalCall;
        DCMPSTAT_INFO("unknown presentationLUTShape keyword: " << aString);
      }
    }
  }

  if (withSOPInstance)
  {
    if (sOPInstanceUID.getLength() == 0)
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("sOPInstanceUID absent in Presentation LUT Content Sequence");
    }
    else if (sOPInstanceUID.getVM() != 1)
    {
      result=EC_IllegalCall;
      DCMPSTAT_INFO("sOPInstanceUID VM != 1 in Presentation LUT Content Sequence");
    }
  }
  
  return result;
}

OFCondition DVPSPresentationLUT::write(DcmItem &dset, OFBool withSOPInstance)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;
  DcmSequenceOfItems *dseq=NULL;
  DcmItem *ditem=NULL;
  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
  
  if (presentationLUT==DVPSP_table)
  {
    if (result == EC_Normal)
    {
      ditem = new DcmItem();
      if (ditem)
      {
        dseq = new DcmSequenceOfItems(DCM_PresentationLUTSequence);
        if (dseq)
        {
          delem = new DcmUnsignedShort(presentationLUTDescriptor);
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
          delem = new DcmUnsignedShort(presentationLUTData);
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
          if (presentationLUTExplanation.getLength() >0)
          {
            delem = new DcmLongString(presentationLUTExplanation);
            if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;
          }
          if (result==EC_Normal)
          {
            dseq->insert(ditem);
            dset.insert(dseq, OFTrue /*replaceOld*/);
          } else {
            // out of memory during creation of sequence contents.
            delete dseq;
            delete ditem;
            result = EC_MemoryExhausted;
          }
        } else {
          // could allocate item but not sequence. Bail out.
          delete ditem;
          result = EC_MemoryExhausted;
        }
      }
      else result = EC_MemoryExhausted;
    }
  } else {
    if (presentationLUT==DVPSP_inverse) presentationLUTShape.putString("INVERSE");
    else if (presentationLUT==DVPSP_lin_od) presentationLUTShape.putString("LIN OD");
    else presentationLUTShape.putString("IDENTITY");
    ADD_TO_DATASET(DcmCodeString, presentationLUTShape)
  }
  if (withSOPInstance) { ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID) }

  return result;
}


OFBool DVPSPresentationLUT::haveTable()
{
  if ((presentationLUTDescriptor.getVM()==3)&&(presentationLUTData.getLength() > 0)) return OFTrue;
  else return OFFalse;
}

const char *DVPSPresentationLUT::getSOPInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == sOPInstanceUID.getString(c)) return c; else return NULL;
}

const char *DVPSPresentationLUT::getCurrentExplanation()
{
  const char *value = NULL;
  switch (presentationLUT)
  {
    case DVPSP_identity:
      value = "Identity Presentation LUT Shape";
      break;
    case DVPSP_inverse:
      value = "Inverse Presentation LUT Shape";
      break;
    case DVPSP_lin_od:
      value = "Linear Optical Density Presentation LUT Shape";
      break;
    case DVPSP_table:
      value = getLUTExplanation();
      if (value==NULL) value = "Unnamed Presentation LUT";
      break;
  }
  return value;
}

const char *DVPSPresentationLUT::getLUTExplanation()
{
  char *value = NULL;
  if (EC_Normal != presentationLUTExplanation.getString(value)) return NULL;
  return value;
}

OFCondition DVPSPresentationLUT::setLUT(
    DcmUnsignedShort& lutDescriptor,
    DcmUnsignedShort& lutData,
    DcmLongString& lutExplanation)
{
  if ((lutDescriptor.getVM()==3)&&(lutData.getLength() > 0))
  {
    presentationLUTDescriptor = lutDescriptor;
    presentationLUTData = lutData;
    presentationLUTExplanation = lutExplanation;
    presentationLUT = DVPSP_table;
  } else return EC_IllegalCall;
  return EC_Normal;
}

OFCondition DVPSPresentationLUT::setType(DVPSPresentationLUTType newType)
{
  if ((newType == DVPSP_table)&&(! haveTable())) return EC_IllegalCall;
  presentationLUT = newType;
  return EC_Normal;
}


OFCondition DVPSPresentationLUT::setSOPInstanceUID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) return EC_IllegalCall;
  return sOPInstanceUID.putString(value);
}


OFBool DVPSPresentationLUT::isLegalPrintPresentationLUT()
{
  OFBool result = OFFalse;
  Uint16 val=0;
  switch (presentationLUT)
  {
    case DVPSP_table:
      if (EC_Normal == presentationLUTDescriptor.getUint16(val,2))
      {
        if ((val>=10)&&(val<=16)) result = OFTrue;
      }
      break;
    case DVPSP_inverse:
      break;
    case DVPSP_identity:
    case DVPSP_lin_od:
      result = OFTrue;
      break;
  }
  return result;
}
  
OFBool DVPSPresentationLUT::matchesImageDepth(OFBool is12bit)
{
  Uint16 numEntries=0;
  Uint16 firstMapped=0;
  OFBool result = OFFalse;
  switch (presentationLUT)
  {
    case DVPSP_table:

      if ((EC_Normal == presentationLUTDescriptor.getUint16(numEntries,0)) &&
         (EC_Normal == presentationLUTDescriptor.getUint16(firstMapped,1)))
      {
      	if ((firstMapped == 0)&&((is12bit && (numEntries == 4096))||((!is12bit) && (numEntries == 256)))) result = OFTrue;
      }
      break;
    case DVPSP_inverse:
      break;
    case DVPSP_identity:
    case DVPSP_lin_od:
      result = OFTrue;
      break;
  }
  return result;
}

DVPSPrintPresentationLUTAlignment DVPSPresentationLUT::getAlignment()
{
  if (presentationLUT == DVPSP_table)
  {
    Uint16 numberOfEntries = 0;
    Uint16 firstEntryMapped = 0xFFFF;
    if (EC_Normal != presentationLUTDescriptor.getUint16(numberOfEntries, 0)) numberOfEntries = 0;
    if (EC_Normal != presentationLUTDescriptor.getUint16(firstEntryMapped, 1)) firstEntryMapped = 0xFFFF;
    if ((numberOfEntries == 256)&&(firstEntryMapped == 0)) return DVPSK_table8;
    if ((numberOfEntries == 4096)&&(firstEntryMapped == 0)) return DVPSK_table12;
    return DVPSK_other;
  }
  return DVPSK_shape;
}


OFBool DVPSPresentationLUT::printSCPCreate(
  DcmDataset *rqDataset, 
  T_DIMSE_Message& rsp, 
  DcmDataset *& rspDataset, 
  OFBool matchRequired,
  OFBool supports12Bit)
{
  OFBool result = OFTrue;
  DcmStack stack;
  
  if ((rqDataset==NULL)||(EC_Normal != read(*rqDataset, OFFalse)))
  {
    DCMPSTAT_INFO("cannot create Presentation LUT: attribute list error.");
    rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
    result = OFFalse;
  }

  // read() has cleared sOPInstanceUID; assign UID now.
  if (EC_Normal != setSOPInstanceUID(rsp.msg.NCreateRSP.AffectedSOPInstanceUID))
  {
    rsp.msg.NCreateRSP.DimseStatus = STATUS_N_ProcessingFailure;
    result = OFFalse;
  }

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    OFBool intoSub = OFTrue;
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, intoSub))
    {
      intoSub = OFFalse;
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag.getElement() == 0x0000) /* group length */ ;
      else if (currentTag == DCM_PresentationLUTShape) /* OK */ ;
      else if (currentTag == DCM_PresentationLUTSequence) /* OK */ ;
      else
      {
        DCMPSTAT_INFO("cannot create Presentation LUT: unsupported attribute received:" << OFendl
            << DcmObject::PrintHelper(*stack.top(), DCMTypes::PF_shortenLongTagValues));
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }
  
  // if match between LUT and pixel data required, enforce rule
  if (result && matchRequired)
  {
    OFBool matches = OFTrue;
    switch (getAlignment())
    {
      case DVPSK_shape:
      case DVPSK_table8:
        break; // always OK
      case DVPSK_table12:
        // is OK if printer supports 12 bit
        matches = supports12Bit;
        break;
      case DVPSK_other: // never fits
        matches = OFFalse;
        break;  
    }
    if (!matches)
    {
      DCMPSTAT_INFO("cannot create Presentation LUT: Mismatch between LUT entries and image pixel depth.");
      rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
      result = OFFalse;
    }
  }

  // if n-create was successful, create response dataset
  if (result)
  {
    rspDataset = new DcmDataset;    
    if (rspDataset)
    {
      if (EC_Normal == write(*rspDataset, OFFalse))
      {
        rsp.msg.NCreateRSP.DataSetType = DIMSE_DATASET_PRESENT;
      } else {
      	delete rspDataset;
      	rspDataset = NULL;
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_ProcessingFailure;
        result = OFFalse;
      }     
    } else {
      rsp.msg.NCreateRSP.DimseStatus = STATUS_N_ProcessingFailure;
      result = OFFalse;
    }
  }
  return result;
}

/*
 *  $Log: dvpspl.cc,v $
 *  Revision 1.31  2010-10-14 13:14:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.30  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.29  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.28  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.27  2005/12/08 15:46:38  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.26  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.25  2003/08/27 14:59:08  meichel
 *  Moved all methods of class DVPSPresentationLUT that depend on module dcmimgle
 *    into a separate implementation file
 *
 *  Revision 1.24  2003/03/12 17:34:22  meichel
 *  Updated DcmObject::print() flags
 *
 *  Revision 1.23  2001/11/28 13:56:58  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.22  2001/09/26 15:36:29  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.21  2001/06/01 15:50:34  meichel
 *  Updated copyright header
 *
 *  Revision 1.20  2001/05/25 10:07:57  meichel
 *  Corrected some DIMSE error status codes for Print SCP
 *
 *  Revision 1.19  2000/09/06 08:55:38  meichel
 *  Updated Print SCP to accept and silently ignore group length attributes.
 *
 *  Revision 1.18  2000/07/11 14:53:06  joergr
 *  Corrected rendering of presentation LUT shape LIN OD.
 *
 *  Revision 1.17  2000/07/07 14:15:14  joergr
 *  Added support for LIN OD presentation LUT shape.
 *
 *  Revision 1.16  2000/07/07 13:39:50  joergr
 *  Added support for LIN OD presentation LUT shape.
 *
 *  Revision 1.15  2000/06/09 10:15:36  joergr
 *  Added support for rendering inverse presentation LUT into print bitmaps.
 *
 *  Revision 1.14  2000/06/08 10:44:36  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.13  2000/06/07 14:27:13  joergr
 *  Added support for rendering "hardcopy" and "softcopy" presentation LUTs.
 *
 *  Revision 1.12  2000/06/07 13:17:07  meichel
 *  now using DIMSE status constants and log facilities defined in dcmnet
 *
 *  Revision 1.11  2000/06/02 16:01:03  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.10  2000/05/31 12:58:15  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.9  2000/03/08 16:29:07  meichel
 *  Updated copyright header.
 *
 *  Revision 1.8  2000/03/03 14:14:02  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.7  1999/11/24 15:15:05  joergr
 *  Replaced call of method invertTable() by mirrorTable() to invert a
 *  presentation LUT.
 *
 *  Revision 1.6  1999/10/20 10:55:19  joergr
 *  Enhanced method invertTable to distinguish between copy of LUT data and
 *  original (referenced) LUT data.
 *
 *  Revision 1.5  1999/10/07 17:22:00  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.4  1999/09/24 15:24:07  meichel
 *  Print spooler (dcmprtsv) now logs diagnostic messages in log files
 *    when operating in spool mode.
 *
 *  Revision 1.3  1999/09/24 13:22:07  joergr
 *  Corrected bug writing inverse Presentation LUT Shape.
 *
 *  Revision 1.2  1999/09/10 07:32:43  thiel
 *  Added Presentation LUT Shape LIN OD
 *
 *  Revision 1.1  1999/07/30 13:34:57  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */

/*
 *
 *  Copyright (C) 1999-2024, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrobow.h"
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
  
  READ_FROM_DATASET(DcmCodeString, EVR_CS, presentationLUTShape)
  if (withSOPInstance) { READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, sOPInstanceUID) }
  else sOPInstanceUID.clear();
  
  /* read Presentation LUT Sequence */
  if (result==EC_Normal)
  {
    stack.clear();
    if (EC_Normal == dset.search(DCM_PresentationLUTSequence, stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_SQ))
    {
      seq=(DcmSequenceOfItems *)stack.top();
      if (seq->card() ==1)
      {
         item = seq->getItem(0);
         stack.clear();

         // LUTDescriptor can be US or SS
         if ((EC_Normal == item->search((DcmTagKey &)presentationLUTDescriptor.getTag(),
           stack, ESM_fromHere, OFFalse)) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_SS))
         {
           // We explicitly use DcmElement::operator=(), which works for US and SS
           DcmElement *pLUTDescriptor = &presentationLUTDescriptor;
           pLUTDescriptor->operator=(* OFstatic_cast(DcmElement *, stack.top()));
         }

         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTExplanation.getTag(), 
           stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_LO))
         {
           presentationLUTExplanation = *((DcmLongString *)(stack.top()));
         }
         stack.clear();
         if (EC_Normal == item->search((DcmTagKey &)presentationLUTData.getTag(), 
           stack, ESM_fromHere, OFFalse) && (stack.top()->ident() == EVR_US || stack.top()->ident() == EVR_OW))
         {
           // we deliberately call DcmElement::operator=() here, which will work for both DcmUnsignedShort and DcmOtherByteOtherWord parameters
           DcmElement *pldata = &presentationLUTData;
           pldata->operator=(*(DcmElement *)(stack.top()));
         }
      } else {
        result=EC_TagNotFound;
        DCMPSTAT_WARN("found Presentation LUT SQ with number of items != 1");
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
      DCMPSTAT_WARN("presentationLUTShape and presentationLUTDescriptor absent or empty");
    }
    else if (presentationLUTDescriptor.getVM() != 3)
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("presentationLUTDescriptor present but VM != 3 in presentation state");
    }
    if (presentationLUTData.getLength() == 0)
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("presentationLUTShape and presentationLUTData absent or empty");
    }
  } else {
    if (presentationLUTShape.getVM() != 1)
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("presentationLUTShape present but VM != 1");
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
        DCMPSTAT_WARN("unknown presentationLUTShape keyword: " << aString);
      }
    }
  }

  if (withSOPInstance)
  {
    if (sOPInstanceUID.getLength() == 0)
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("sOPInstanceUID absent in Presentation LUT Content Sequence");
    }
    else if (sOPInstanceUID.getVM() != 1)
    {
      result=EC_IllegalCall;
      DCMPSTAT_WARN("sOPInstanceUID VM != 1 in Presentation LUT Content Sequence");
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
          // we clone presentationLUTDescriptor in order to retain the VR (US or SS)
          delem = OFstatic_cast(DcmElement *, presentationLUTDescriptor.clone());
          if (delem) ditem->insert(delem, OFTrue /*replaceOld*/); else result=EC_MemoryExhausted;

          // we write LUTData as OW in order to avoid the 64 kByte limit for US
          delem = new DcmOtherByteOtherWord(DCM_LUTData);
          delem->operator=(presentationLUTData);
          OFstatic_cast(DcmOtherByteOtherWord *, delem)->setVR(EVR_OW);
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
    DCMPSTAT_WARN("cannot create Presentation LUT: attribute list error.");
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
        DCMPSTAT_WARN("cannot create Presentation LUT: unsupported attribute received:" << OFendl
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
      DCMPSTAT_WARN("cannot create Presentation LUT: Mismatch between LUT entries and image pixel depth.");
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

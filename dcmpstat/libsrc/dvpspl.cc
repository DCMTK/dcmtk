/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSPresentationLUT
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-24 13:22:07 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpspl.h"
#include "dcmimage.h"    /* for class DiLookupTable, DicomImage */

/* --------------- a few macros avoiding copy/paste --------------- */

#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
}

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

/* --------------- class DVPSPresentationLUT --------------- */

DVPSPresentationLUT::DVPSPresentationLUT()
: presentationLUT(DVPSP_identity)
, presentationLUTDescriptor(DCM_LUTDescriptor)
, presentationLUTExplanation(DCM_LUTExplanation)
, presentationLUTData(DCM_LUTData)
{
}

DVPSPresentationLUT::DVPSPresentationLUT(const DVPSPresentationLUT& copy)
: presentationLUT(copy.presentationLUT)
, presentationLUTDescriptor(copy.presentationLUTDescriptor)
, presentationLUTExplanation(copy.presentationLUTExplanation)
, presentationLUTData(copy.presentationLUTData)
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
}

E_Condition DVPSPresentationLUT::read(DcmItem &dset)
{
  DcmSequenceOfItems *seq;
  DcmItem *item;
  E_Condition result = EC_Normal;
  DcmStack stack;
  OFString aString;

  DcmCodeString presentationLUTShape(DCM_PresentationLUTShape);
  
  READ_FROM_DATASET(DcmCodeString, presentationLUTShape)

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
#ifdef DEBUG
        cerr << "Error: found Presentation LUT SQ with number of items != 1" << endl;
#endif
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
#ifdef DEBUG
      cerr << "Error: presentationLUTShape and presentationLUTDescriptor absent or empty" << endl;
#endif
    }
    else if (presentationLUTDescriptor.getVM() != 3)
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: presentationLUTDescriptor present but VM != 3 in presentation state" << endl;
#endif
    }
    if (presentationLUTData.getLength() == 0)
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: presentationLUTShape and presentationLUTData absent or empty" << endl;
#endif
    }
  } else {
    if (presentationLUTShape.getVM() != 1)
    {
      result=EC_IllegalCall;
#ifdef DEBUG
      cerr << "Error: presentationLUTShape present but VM != 1" << endl;
#endif
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
#ifdef DEBUG
        cerr << "Error: unknown presentationLUTShape keyword: " << aString << endl;
#endif
      }
    }
  }

  return result;
}

E_Condition DVPSPresentationLUT::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          delem = new DcmUnsignedShort(presentationLUTData);
          if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          if (presentationLUTExplanation.getLength() >0)
          {
            delem = new DcmLongString(presentationLUTExplanation);
            if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;
          }
          if (result==EC_Normal)
          {
            dseq->insert(ditem);
            dset.insert(dseq);
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

  return result;
}


OFBool DVPSPresentationLUT::isInverse()
{
  OFBool result = OFFalse;
  switch (presentationLUT)
  {
    case DVPSP_identity:
    case DVPSP_lin_od:
      break;
    case DVPSP_inverse:
      result = OFTrue;
      break;
    case DVPSP_table:
      if ((presentationLUTDescriptor.getVM()==3)&&(presentationLUTData.getLength() > 0))
      {
        DiLookupTable *lut = new DiLookupTable(presentationLUTData, presentationLUTDescriptor);
        if (lut && (lut->getFirstValue() > lut->getLastValue())) result = OFTrue;
        delete lut;
      }
      break;
  }
  return result;
}

OFBool DVPSPresentationLUT::haveTable()
{
  if ((presentationLUTDescriptor.getVM()==3)&&(presentationLUTData.getLength() > 0)) return OFTrue;
  else return OFFalse;
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
      value = "Lineare Optical Density Presentation LUT Shape";
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

E_Condition DVPSPresentationLUT::setLUT(
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

E_Condition DVPSPresentationLUT::setType(DVPSPresentationLUTType newType)
{
  if ((newType == DVPSP_table)&&(! haveTable())) return EC_IllegalCall;
  presentationLUT = newType;
  return EC_Normal;
}

E_Condition DVPSPresentationLUT::invert()
{
  E_Condition status = EC_Normal;
  switch (presentationLUT)
  {
      case DVPSP_identity:
          presentationLUT = DVPSP_inverse;
          break;
      case DVPSP_inverse:
          presentationLUT = DVPSP_identity;
          break;
      case DVPSP_table:
          status = EC_IllegalCall;
          if (haveTable())
          {
              DiLookupTable *lut = new DiLookupTable(presentationLUTData, presentationLUTDescriptor);
              if (lut && (lut->invertTable())) status = EC_Normal;
              delete lut;
          }
          break;
      case DVPSP_lin_od:
          status = EC_IllegalCall;
          break;
          
  }
  return status;
}

OFBool DVPSPresentationLUT::activate(DicomImage *image)
{
  if (image==NULL) return OFFalse;

  int result=0;
  switch (presentationLUT)
  {   
    case DVPSP_identity:
      result = image->setPresentationLutShape(ESP_Identity);
#ifdef DEBUG
      if (!result) cerr << "warning: unable to set identity presentation LUT shape, ignoring." << endl;
#endif
      break;
    case DVPSP_inverse:
      result = image->setPresentationLutShape(ESP_Inverse);
#ifdef DEBUG
      if (!result) cerr << "warning: unable to set inverse presentation LUT shape, ignoring." << endl;
#endif
      break;
      
    case DVPSP_lin_od:
    //  make no sense at the moment
    //  result = image->setPresentationLutShape(ESP_Lin_od);
#ifdef DEBUG
      if (!result) cerr << "warning: unable to set lineare optical density presentation LUT shape, ignoring." << endl;
#endif
      break;
    case DVPSP_table:
      result = image->setPresentationLut(presentationLUTData, presentationLUTDescriptor,
        &presentationLUTExplanation);
#ifdef DEBUG
      if (!result) cerr << "warning: unable to set identity presentation LUT shape, ignoring." << endl;
#endif
      break;
  }
  if (result) return OFTrue; else return OFFalse;
}

/*
 *  $Log: dvpspl.cc,v $
 *  Revision 1.3  1999-09-24 13:22:07  joergr
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


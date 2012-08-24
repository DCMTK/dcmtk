/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpspl.h"
#include "dcmtk/dcmimgle/dcmimage.h"    /* for class DiLookupTable, DicomImage */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmnet/dimse.h"

/* --------------- class DVPSPresentationLUT --------------- */

OFBool DVPSPresentationLUT::compareDiLookupTable(DiLookupTable *lut)
{
  if ((presentationLUT == DVPSP_table) && lut 
     && (0 == lut->compareLUT(presentationLUTData, presentationLUTDescriptor))) return OFTrue;
  return OFFalse;
}

DiLookupTable *DVPSPresentationLUT::createDiLookupTable()
{
  DiLookupTable *result = NULL;
  if (presentationLUT == DVPSP_table) result = new DiLookupTable(presentationLUTData, presentationLUTDescriptor);
  return result;
}  

OFCondition DVPSPresentationLUT::invert()
{
  OFCondition status = EC_Normal;
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
              if (lut && (lut->mirrorTable(0x2))) status = EC_Normal;       // flag = 0x2: mirror only original LUT data
              delete lut;
          }
          break;
      case DVPSP_lin_od:
          status = EC_IllegalCall;
          break;
          
  }
  return status;
}

OFBool DVPSPresentationLUT::activate(DicomImage *image, OFBool printLUT)
{
  if (image==NULL) return OFFalse;

  int result=0;
  switch (presentationLUT)
  {   
    case DVPSP_identity:
      if (printLUT)
      {
        // in DICOM print, IDENTITY should not invert a MONOCHROME1 image
        result = image->setPresentationLutShape(ESP_Default);
      }
      else 
      {
      	result = image->setPresentationLutShape(ESP_Identity);
      }
      if (!result)
        DCMPSTAT_WARN("unable to set identity presentation LUT shape, ignoring.");
      break;
    case DVPSP_inverse:
      if (!printLUT)
        result = image->setPresentationLutShape(ESP_Inverse);
      if (!result)
        DCMPSTAT_WARN("unable to set inverse presentation LUT shape, ignoring.");
      break;      
    case DVPSP_lin_od:
      result = image->setPresentationLutShape(ESP_LinOD);
      if (!result)
        DCMPSTAT_WARN("unable to set linear optical density presentation LUT shape, ignoring.");
      break;
    case DVPSP_table:
      if (printLUT)
        result = image->setVoiLut(presentationLUTData, presentationLUTDescriptor, &presentationLUTExplanation);
      else
        result = image->setPresentationLut(presentationLUTData, presentationLUTDescriptor, &presentationLUTExplanation);
      if (!result)
        DCMPSTAT_WARN("unable to set presentation LUT, ignoring.");
      break;
  }
  if (result) return OFTrue; else return OFFalse;
}

OFBool DVPSPresentationLUT::activateInverseLUT(DicomImage *image)
{
  int result = 0;
  if ((image != NULL) && (presentationLUT == DVPSP_table))
  {
      result = image->setInversePresentationLut(presentationLUTData, presentationLUTDescriptor);
      if (!result)
        DCMPSTAT_WARN("unable to set inverse presentation LUT, ignoring.");
  }
  if (result) return OFTrue; else return OFFalse;
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

/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 *    classes: DVPSAnnotationContent
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dvpsab.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */


/* --------------- class DVPSAnnotationContent --------------- */

DVPSAnnotationContent::DVPSAnnotationContent()
: sOPInstanceUID(DCM_SOPInstanceUID)
, annotationPosition(DCM_AnnotationPosition)
, textString(DCM_TextString)
{
}

DVPSAnnotationContent::DVPSAnnotationContent(const DVPSAnnotationContent& copy)
: sOPInstanceUID(copy.sOPInstanceUID)
, annotationPosition(copy.annotationPosition)
, textString(copy.textString)
{
}

DVPSAnnotationContent::~DVPSAnnotationContent()
{
}

void DVPSAnnotationContent::clear()
{
  sOPInstanceUID.clear();
  annotationPosition.clear();
  textString.clear();
  return;
}

OFCondition DVPSAnnotationContent::setContent(
    const char *instanceuid,
    const char *text,
    Uint16 position)
{
  OFCondition result = EC_Normal;
  if (instanceuid && text)
  {
    clear();
    result = sOPInstanceUID.putString(instanceuid);
    if (EC_Normal == result) result = textString.putString(text);
    if (EC_Normal == result) result = annotationPosition.putUint16(position,0);
  } else result = EC_IllegalCall;
  return result;
}

OFCondition DVPSAnnotationContent::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, sOPInstanceUID)
  READ_FROM_DATASET(DcmUnsignedShort, EVR_US, annotationPosition)
  READ_FROM_DATASET(DcmLongString, EVR_LO, textString)
  
  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_WARN("SOPInstanceUID missing or incorrect in Stored Print Annotation");
    }
    if ((annotationPosition.getLength() == 0)||(annotationPosition.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_WARN("AnnotationPosition missing or incorrect in Stored Print Annotation");
    }
    if ((textString.getLength() == 0)||(textString.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_WARN("TextString missing or incorrect in Stored Print Annotation");
    }
  }

  return result;
}

OFCondition DVPSAnnotationContent::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;

  /* before writing anything, check that we are able to write a correct item */
  if (sOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_WARN("cannot write Stored Print Annotation: SOPInstanceUID empty");
  }
  if (annotationPosition.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_WARN("cannot write Stored Print Annotation: AnnotationPosition empty");
  }
  if (textString.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_WARN("cannot write Stored Print Annotation: TextString empty");
  }

  ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  ADD_TO_DATASET(DcmUnsignedShort, annotationPosition)
  ADD_TO_DATASET(DcmLongString, textString)

  return result;
}

OFCondition DVPSAnnotationContent::prepareBasicAnnotationBox(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;

  ADD_TO_DATASET(DcmUnsignedShort, annotationPosition)
  ADD_TO_DATASET(DcmLongString, textString)
  return result;
}

OFCondition DVPSAnnotationContent::setSOPInstanceUID(const char *value)
{
  if ((value==NULL)||(strlen(value)==0)) 
  {
  	sOPInstanceUID.clear();
  	return EC_Normal;
  }
  return sOPInstanceUID.putString(value);
}

const char *DVPSAnnotationContent::getSOPInstanceUID()
{
  char *c = NULL;
  if (EC_Normal == sOPInstanceUID.getString(c)) return c; else return NULL;
}

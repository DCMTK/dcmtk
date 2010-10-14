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
 *    classes: DVPSAnnotationContent
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:31 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
  
  READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  READ_FROM_DATASET(DcmUnsignedShort, annotationPosition)
  READ_FROM_DATASET(DcmLongString, textString)
  
  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("SOPInstanceUID missing or incorrect in Stored Print Annotation");
    }
    if ((annotationPosition.getLength() == 0)||(annotationPosition.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("AnnotationPosition missing or incorrect in Stored Print Annotation");
    }
    if ((textString.getLength() == 0)||(textString.getVM() != 1))
    {
        result=EC_TagNotFound;
        DCMPSTAT_INFO("TextString missing or incorrect in Stored Print Annotation");
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
    DCMPSTAT_INFO("cannot write Stored Print Annotation: SOPInstanceUID empty");
  }
  if (annotationPosition.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Annotation: AnnotationPosition empty");
  }
  if (textString.getLength() == 0)
  {
    result=EC_TagNotFound;
    DCMPSTAT_INFO("cannot write Stored Print Annotation: TextString empty");
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

/*
 *  $Log: dvpsab.cc,v $
 *  Revision 1.13  2010-10-14 13:14:31  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.12  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.11  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.10  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.9  2005/12/08 15:46:16  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2001/09/26 15:36:22  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.7  2001/06/07 14:31:34  joergr
 *  Removed unused variable (reported by gcc 2.5.8 on NeXTSTEP).
 *
 *  Revision 1.6  2001/06/01 15:50:26  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/06/02 16:00:56  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.4  2000/05/31 13:02:35  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.3  2000/03/08 16:29:01  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  2000/03/03 14:13:57  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.1  1999/10/19 14:48:27  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *
 */


/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *    classes: DVPSAnnotationContent
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-05-31 13:02:35 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsab.h"
#include "dvpsdef.h"     /* for constants and macros */


/* --------------- class DVPSAnnotationContent --------------- */

DVPSAnnotationContent::DVPSAnnotationContent()
: sOPInstanceUID(DCM_SOPInstanceUID)
, annotationPosition(DCM_AnnotationPosition)
, textString(DCM_TextString)
, logstream(&CERR)
{
}

DVPSAnnotationContent::DVPSAnnotationContent(const DVPSAnnotationContent& copy)
: sOPInstanceUID(copy.sOPInstanceUID)
, annotationPosition(copy.annotationPosition)
, textString(copy.textString)
, logstream(copy.logstream)
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

E_Condition DVPSAnnotationContent::setContent(
    const char *instanceuid,
    const char *text,
    Uint16 position)
{
  E_Condition result = EC_Normal;
  if (instanceuid && text)
  {
    clear();
    result = sOPInstanceUID.putString(instanceuid);
    if (EC_Normal == result) result = textString.putString(text);
    if (EC_Normal == result) result = annotationPosition.putUint16(position,0);
  } else result = EC_IllegalCall;
  return result;
}

E_Condition DVPSAnnotationContent::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;
  OFString aString;
  
  READ_FROM_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  READ_FROM_DATASET(DcmUnsignedShort, annotationPosition)
  READ_FROM_DATASET(DcmLongString, textString)
  
  /* Now perform basic sanity checks */

  if (result==EC_Normal)
  {
    if ((sOPInstanceUID.getLength() == 0)||(sOPInstanceUID.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: SOPInstanceUID missing or incorrect in Stored Print Annotation" << endl;
#endif
    }
    if ((annotationPosition.getLength() == 0)||(annotationPosition.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: AnnotationPosition missing or incorrect in Stored Print Annotation" << endl;
#endif
    }
    if ((textString.getLength() == 0)||(textString.getVM() != 1))
    {
        result=EC_TagNotFound;
#ifdef DEBUG
        *logstream << "Error: TextString missing or incorrect in Stored Print Annotation" << endl;
#endif
    }
  }

  return result;
}

E_Condition DVPSAnnotationContent::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;

  /* before writing anything, check that we are able to write a correct item */
  if (sOPInstanceUID.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Annotation: SOPInstanceUID empty" << endl;
#endif
  }
  if (annotationPosition.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Annotation: AnnotationPosition empty" << endl;
#endif
  }
  if (textString.getLength() == 0)
  {
    result=EC_TagNotFound;
#ifdef DEBUG
    *logstream << "Error: cannot write Stored Print Annotation: TextString empty" << endl;
#endif
  }

  ADD_TO_DATASET(DcmUniqueIdentifier, sOPInstanceUID)
  ADD_TO_DATASET(DcmUnsignedShort, annotationPosition)
  ADD_TO_DATASET(DcmLongString, textString)

  return result;
}

E_Condition DVPSAnnotationContent::prepareBasicAnnotationBox(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;

  ADD_TO_DATASET(DcmUnsignedShort, annotationPosition)
  ADD_TO_DATASET(DcmLongString, textString)
  return result;
}

E_Condition DVPSAnnotationContent::setSOPInstanceUID(const char *value)
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
 *  Revision 1.4  2000-05-31 13:02:35  meichel
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


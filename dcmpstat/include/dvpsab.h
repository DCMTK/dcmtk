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
 *  Update Date:      $Date: 2000-03-08 16:28:48 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSAB_H__
#define __DVPSAB_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dctk.h"
#include "dvpstyp.h"     /* for enum types */

/** the representation of a Annotation Content SQ item for Stored Print
 */  

class DVPSAnnotationContent
{
public:
 /// default constructor
  DVPSAnnotationContent();
  
  /// copy constructor
  DVPSAnnotationContent(const DVPSAnnotationContent& copy);

  /** clone method.
   *  @return a pointer to a new DVPSAnnotationContent object containing
   *  a copy of this object.
   */
  DVPSAnnotationContent *clone() { return new DVPSAnnotationContent(*this); }

  /// destructor
  virtual ~DVPSAnnotationContent();

  /** resets the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** reads an annotation content item from a DICOM dataset.
   *  The DICOM elements of the referenced item are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the item of the AnnotationContentSequence from which the data is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the annotation box managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the AnnotationContentSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
  
  /** sets the content of this annotation box object.
   *  @param instanceuid SOP instance UID of this annotation
   *  @param text annotation text string
   *  @param position annotation box position
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setContent(
    const char *instanceuid,
    const char *text,
    Uint16 position);
    
  /** gets the current SOP Instance UID.
   *  @return SOP Instance UID, may be NULL.
   */
  const char *getSOPInstanceUID();

  /** sets the SOP instance UID (which is returned by the Print SCP).
   *  @param value new attribute value, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition setSOPInstanceUID(const char *value);

  /** writes the attributes managed by this objects that are part of a 
   *  basic annotation box N-SET request into the DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition prepareBasicAnnotationBox(DcmItem &dset);

  /** sets a new log stream
   *  @param o new log stream, must not be NULL
   */
  void setLog(ostream *o)
  {
    if (o) logstream = o;
  }
 
private:
  /// private undefined assignment operator
  DVPSAnnotationContent& operator=(const DVPSAnnotationContent&);

  /// Module=Annotation_List, VR=UI, VM=1, Type 1(c) 
  DcmUniqueIdentifier      sOPInstanceUID;
  /// Module=Annotation_List, VR=US, VM=1, Type 1(c)
  DcmUnsignedShort         annotationPosition;
  /// Module=Annotation_List, VR=LO, VM=1, Type 1(c)
  DcmLongString            textString;

  /** output stream for error messages, never NULL
   */
  ostream *logstream;

};

#endif

/*
 *  $Log: dvpsab.h,v $
 *  Revision 1.2  2000-03-08 16:28:48  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1999/10/19 14:46:05  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *
 */


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
 *  Update Date:      $Date: 2010-10-14 13:16:35 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSAB_H
#define DVPSAB_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrus.h"     /* for class DcmUnsignedShort */
#include "dcmtk/dcmdata/dcvrlo.h"     /* for class DcmLongString */
#include "dcmtk/dcmdata/dcvrui.h"     /* for class DcmUniqueIdentifier */
#include "dcmtk/dcmpstat/dvpstyp.h"   /* for enum types */

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
  OFCondition read(DcmItem &dset);
  
  /** writes the annotation box managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the the item of the AnnotationContentSequence to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);
  
  /** sets the content of this annotation box object.
   *  @param instanceuid SOP instance UID of this annotation
   *  @param text annotation text string
   *  @param position annotation box position
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setContent(
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
  OFCondition setSOPInstanceUID(const char *value);

  /** writes the attributes managed by this objects that are part of a 
   *  basic annotation box N-SET request into the DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition prepareBasicAnnotationBox(DcmItem &dset);

private:
  /// private undefined assignment operator
  DVPSAnnotationContent& operator=(const DVPSAnnotationContent&);

  /// Module=Annotation_List, VR=UI, VM=1, Type 1(c) 
  DcmUniqueIdentifier      sOPInstanceUID;
  /// Module=Annotation_List, VR=US, VM=1, Type 1(c)
  DcmUnsignedShort         annotationPosition;
  /// Module=Annotation_List, VR=LO, VM=1, Type 1(c)
  DcmLongString            textString;

};

#endif

/*
 *  $Log: dvpsab.h,v $
 *  Revision 1.10  2010-10-14 13:16:35  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.9  2010-10-07 14:31:35  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.8  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.7  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.6  2005-12-08 16:03:32  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2001/09/26 15:36:08  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:11  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/06/02 16:00:42  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/03/08 16:28:48  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1999/10/19 14:46:05  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *
 */

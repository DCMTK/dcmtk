/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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

#ifndef DVPSAB_H
#define DVPSAB_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"   /* for enum types */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmdata/dcvrus.h"     /* for class DcmUnsignedShort */
#include "dcmtk/dcmdata/dcvrlo.h"     /* for class DcmLongString */
#include "dcmtk/dcmdata/dcvrui.h"     /* for class DcmUniqueIdentifier */

/** the representation of a Annotation Content SQ item for Stored Print
 */  

class DCMTK_DCMPSTAT_EXPORT DVPSAnnotationContent
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

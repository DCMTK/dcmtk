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
 *    classes: DVPSAnnotationContent_PList
 *
 */

#ifndef DVPSABL_H
#define DVPSABL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"     /* for enum types */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmdata/dcitem.h"

class DVPSAnnotationContent;

/** the list of Annotations contained in a stored print object.
 *  This class manages the data structures comprising one complete
 *  Annotation Content Sequence in a Stored Print object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSAnnotationContent_PList
{
public:

  /// default constructor
  DVPSAnnotationContent_PList();
  
  /// copy constructor
  DVPSAnnotationContent_PList(const DVPSAnnotationContent_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSAnnotationContent_PList object containing
   *  a deep copy of this object.
   */
  DVPSAnnotationContent_PList *clone() { return new DVPSAnnotationContent_PList(*this); }

  /// destructor
  virtual ~DVPSAnnotationContent_PList();

  /** reads a list of annotations (AnnotationContentSequence) from a DICOM dataset.
   *  The DICOM elements of the annotations are copied from the dataset to this object.
   *  The completeness of all items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the sequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);
  
  /** writes the list of annotations managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the AnnotationContentSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** gets the number of annotations in this list.
   *  @return the number of annotations.
   */
  size_t size() const { return list_.size(); }
  
  /** creates a new annotation object and sets the content of this annotation object.
   *  @param instanceuid SOP instance UID of this annotation
   *  @param text annotation text string
   *  @param position annotation box position
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addAnnotationBox(
    const char *instanceuid,
    const char *text,
    Uint16 position);
  
  /** deletes one of the registered annotations.
   *  @param idx index, must be < size()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition deleteAnnotation(size_t idx);
  
  /** deletes multiple of the registered annotations, starting with the first one.
   *  @param number number of annotations to delete, must be <= size()
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition deleteMultipleAnnotations(size_t number);

  /** sets the SOP instance UID for the given annotation.
   *  @param idx index, must be < getNumberOfAnnotations()
   *  @param value new attribute value, must not be NULL.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition setAnnotationSOPInstanceUID(size_t idx, const char *value);

  /** gets the current SOP Instance UID for the given registered annotation.
   *  @param idx index, must be < getNumberOfAnnotations()
   *  @return SOP Instance UID, may be NULL.
   */
  const char *getSOPInstanceUID(size_t idx);

  /** writes the attributes managed by the referenced object that are part of a 
   *  basic annotation box N-SET request into the DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param idx index, must be < getNumberOfAnnotations()
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition prepareBasicAnnotationBox(size_t idx, DcmItem &dset);

  /** removes all UIDs from the annotation boxes managed by this object
   */
  void clearAnnotationSOPInstanceUIDs();

private:

  /** private undefined assignment operator
   */
  DVPSAnnotationContent_PList& operator=(const DVPSAnnotationContent_PList&);
  
  /** returns a pointer to the annotation with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < size()
   *  @return pointer to annotation object or NULL
   */
  DVPSAnnotationContent *getAnnotationBox(size_t idx); 

  /** the list maintained by this object
   */
  OFList<DVPSAnnotationContent *> list_;

};


#endif

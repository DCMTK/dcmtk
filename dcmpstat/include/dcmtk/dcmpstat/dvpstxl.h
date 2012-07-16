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
 *    classes: DVPSTextObject_PList
 *
 */

#ifndef DVPSTXL_H
#define DVPSTXL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dpdefine.h"

class DVPSTextObject;


/** the list of text objects contained in a presentation state (internal use only).
 *  This class manages the data structures comprising one complete
 *  Text Object Sequence which is contained in one item
 *  of the Graphic Annotation Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSTextObject_PList
{
public:
  /// default constructor
  DVPSTextObject_PList();

  /// copy constructor
  DVPSTextObject_PList(const DVPSTextObject_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSTextObject_PList object containing
   *  a deep copy of this object.
   */
  DVPSTextObject_PList *clone() { return new DVPSTextObject_PList(*this); }

  /// destructor
  virtual ~DVPSTextObject_PList();

  /** reads a list of text objects from a DICOM dataset.
   *  The DICOM elements of the Text Object Sequence are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the TextObjectSequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the list of text objects managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the TextObjectSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** get number of text objects in this list.
   *  @return the number of text objects.
   */
  size_t size() const { return list_.size(); }

  /** returns a pointer to the text object with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < size()
   *  @return pointer to text object or NULL
   */
  DVPSTextObject *getTextObject(size_t idx);

  /** adds the given text object to
   *  the list of text objects managed by this object.
   *  @param text text object to be inserted.
   */
  void addTextObject(DVPSTextObject *text);

  /** returns a pointer to the text object with the given
   *  index (or NULL if it does not exist) and removes it from the list.
   *  @param idx index, must be < size()
   *  @return pointer to text object or NULL
   */
  DVPSTextObject *removeTextObject(size_t idx);

private:
  /** private undefined assignment operator
   */
  DVPSTextObject_PList& operator=(const DVPSTextObject_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSTextObject *> list_;

};

#endif

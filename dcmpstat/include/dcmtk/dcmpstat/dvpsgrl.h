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
 *    classes: DVPSGraphicObject_PList
 *
 */

#ifndef DVPSGRL_H
#define DVPSGRL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dpdefine.h"

class DVPSGraphicObject;

/** the list of graphic objects contained in a presentation state (internal use only).
 *  This class manages the data structures comprising one complete
 *  Graphic Object Sequence which is contained in one item
 *  of the Graphic Annotation Sequence in a Presentation State object.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSGraphicObject_PList
{
public:
  /// default constructor
  DVPSGraphicObject_PList();

  /// copy constructor
  DVPSGraphicObject_PList(const DVPSGraphicObject_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicObject_PList object containing
   *  a deep copy of this object.
   */
  DVPSGraphicObject_PList *clone() { return new DVPSGraphicObject_PList(*this); }

  /// destructor
  virtual ~DVPSGraphicObject_PList();

  /** reads a list of graphic objects from a DICOM dataset.
   *  The DICOM elements of the Graphic Object Sequence are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the GraphicObjectSequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition read(DcmItem &dset);

  /** writes the list of graphic objects managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the GraphicObjectSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  /** get number of graphic objects in this list.
   *  @return the number of graphic objects.
   */
  size_t size() const { return list_.size(); }

  /** returns a pointer to the graphic object with the given
   *  index or NULL if it does not exist.
   *  @param idx index, must be < size()
   *  @return pointer to graphic object or NULL
   */
  DVPSGraphicObject *getGraphicObject(size_t idx);

  /** adds the given graphic object to
   *  the list of graphic objects managed by this object.
   *  @param text graphic object to be inserted.
   */
  void addGraphicObject(DVPSGraphicObject *graphic);

  /** returns a pointer to the graphic object with the given
   *  index (or NULL if it does not exist) and removes it from the list.
   *  @param idx index, must be < size()
   *  @return pointer to graphic object or NULL
   */
  DVPSGraphicObject *removeGraphicObject(size_t idx);

private:

  /// private undefined assignment operator
  DVPSGraphicObject_PList& operator=(const DVPSGraphicObject_PList&);

  /** the list maintained by this object
   */
  OFList<DVPSGraphicObject *> list_;
};

#endif

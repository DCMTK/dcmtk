/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *    classes: DVPSGraphicObject_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:29 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSGRL_H__
#define __DVPSGRL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"

class DVPSGraphicObject;

/** the list of graphic objects contained in a presentation state (internal use only).
 *  This class manages the data structures comprising one complete
 *  Graphic Object Sequence which is contained in one item
 *  of the Graphic Annotation Sequence in a Presentation State object.
 */

class DVPSGraphicObject_PList: private OFList<DVPSGraphicObject *>
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
  E_Condition read(DcmItem &dset);

  /** writes the list of graphic objects managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the GraphicObjectSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
  
  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();

  
  /** get number of graphic objects in this list.
   *  @return the number of graphic objects.
   */
  size_t size() const { return OFList<DVPSGraphicObject *>::size(); }  
  
};

#endif

/*
 *  $Log: dvpsgrl.h,v $
 *  Revision 1.1  1998-11-27 14:50:29  meichel
 *  Initial Release.
 *
 *
 */


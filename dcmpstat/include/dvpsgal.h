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
 *    classes: DVPSGraphicAnnotation_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:26 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSGAL_H__
#define __DVPSGAL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"

class DVPSGraphicAnnotation;

/** the list of graphic annotations contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the complete
 *  Graphic Annotation Sequence in a Presentation State object.
 */

class DVPSGraphicAnnotation_PList: private OFList<DVPSGraphicAnnotation *>
{
public:
  /// default constructor
  DVPSGraphicAnnotation_PList();
  
  /// copy constructor
  DVPSGraphicAnnotation_PList(const DVPSGraphicAnnotation_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicAnnotation_PList object containing
   *  a deep copy of this object.
   */
  DVPSGraphicAnnotation_PList *clone() { return new DVPSGraphicAnnotation_PList(*this); }

  /// destructor
  virtual ~DVPSGraphicAnnotation_PList();

  /** reads a list of graphic annotations from a DICOM dataset.
   *  The DICOM elements of the Graphic Annotation Sequence are copied
   *  from the dataset to this object.
   *  The completeness of the item (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the dataset from which the GraphicAnnotationSequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);

  /** writes the list of graphic annotations managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the dataset to which the GraphicAnnotationSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);
  
  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
};

#endif

/*
 *  $Log: dvpsgal.h,v $
 *  Revision 1.1  1998-11-27 14:50:26  meichel
 *  Initial Release.
 *
 *
 */


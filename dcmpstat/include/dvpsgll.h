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
 *    classes: DVPSGraphicLayer_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:28 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSGLL_H__
#define __DVPSGLL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"

class DVPSGraphicLayer;

/** the list of graphic layers contained in a presentation state (internal use only).
 *  This class manages the data structures comprising the complete
 *  Graphic Layer Sequence in a Presentation State object.
 */

class DVPSGraphicLayer_PList: private OFList<DVPSGraphicLayer *>
{
public:
  /// default constructor
  DVPSGraphicLayer_PList();
  
  /// copy constructor
  DVPSGraphicLayer_PList(const DVPSGraphicLayer_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSGraphicLayer_PList object containing
   *  a deep copy of this object.
   */
  DVPSGraphicLayer_PList *clone() { return new DVPSGraphicLayer_PList(*this); }

  /// destructor
  virtual ~DVPSGraphicLayer_PList();

  /** reads the Graphic Layer Sequence from a DICOM dataset.
   *  The completeness of the sequence items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the overlays are read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the Graphic Layer Sequence managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the sequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
  
  /** adds a new graphic layer to the list. The recommended display value
   *  of the new graphic layer remains undefined and can be set later.
   *  @param gLayer name of the graphic layer. Must be unique within the sequence,
   *   i.e. no other graphic layer with the same name may exist, otherwise an error
   *   code is returned. 
   *  @param gLayerOrder graphic layer order of the new layer.
   *  @param gLayerDescription (optional) description of the new layer. Default: description absent.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition addGraphicLayer(const char *gLayer, 
    const Sint32 gLayerOrder,
    const char *gLayerDescription=NULL);
};

#endif

/*
 *  $Log: dvpsgll.h,v $
 *  Revision 1.1  1998-11-27 14:50:28  meichel
 *  Initial Release.
 *
 *
 */


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
 *    classes: DVPSOverlayCurveActivationLayer
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:28:48 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSAL_H__
#define __DVPSAL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"

/** a curve or overlay activation layer in a presentation state (internal use only).
 *  This class manages the data structures comprising a single curve
 *  activation layer or overlay activation layer
 *  (one instance of the Curve Activation Layer Module or 
 *  Overlay Activation Layer Module repeating elements)
 *  contained in a Presentation State object.
 */

class DVPSOverlayCurveActivationLayer
{
public:
  /// default constructor
  DVPSOverlayCurveActivationLayer();
  
  /// copy constructor
  DVPSOverlayCurveActivationLayer(const DVPSOverlayCurveActivationLayer& copy);

  /** clone method.
   *  @return a pointer to a new DVPSOverlayCurveActivationLayer object containing
   *  a copy of this object.
   */
  DVPSOverlayCurveActivationLayer *clone() { return new DVPSOverlayCurveActivationLayer(*this); }

  /// destructor
  virtual ~DVPSOverlayCurveActivationLayer();

  /** reads the activation layer for the specified repeating group from a DICOM dataset.
   *  The DICOM elements of the Overlay/Curve Activation Layer module are copied
   *  from the dataset to this object. 
   *  The completeness of the module is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the activation layer is to be read
   *  @param ovGroup the the repeating group to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset, Uint16 ovGroup);
  
  /** writes the activation layer managed by this object to a DICOM dataset.
   *  Copies of the DICOM elements managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the activation layer is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

  /** set activation layer name of this activation.
   *  @param aLayer a pointer to the activation layer name, which is copied into this object.
   */
  void setActivationLayer(const char *aLayer);

  /** set repeating group of this activation.
   *  @param rGroup the repeating group
   */
  void setRepeatingGroup(Uint16 rGroup);

  /** get activation layer name.
   *  @return a pointer to the activation layer name
   */
  const char *getActivationLayer();

  /** get repeating group.
   *  @return the repeating group of this activation.
   */
  Uint16 getRepeatingGroup();

  /** compare repeating group.
   *  @param rGroup the repeating group to compare
   *  @return OFTrue if the activation matches the passed repeating group, OFFalse otherwise.
   */
  OFBool isRepeatingGroup(Uint16 rGroup);
  
private:
  /// the repeating group managed by this object
  Uint16                   repeatingGroup;
  /// VR=CS, VM=1, Type 2c
  DcmCodeString            activationLayer;
};

#endif

/*
 *  $Log: dvpsal.h,v $
 *  Revision 1.2  2000-03-08 16:28:48  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1998/11/27 14:50:24  meichel
 *  Initial Release.
 *
 *
 */


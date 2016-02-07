/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Image Pixel Module
 *
 */

#ifndef MODIMAGEPIXEL_H
#define MODIMAGEPIXEL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the General Image Module:
 *
 *  Samples Per Pixel: (US, 1, 1)
 *  Photometric Interpretation (CS, 1, 1)
 *  Rows: (US, 1, 1)
 *  Columns: (US, 1, 1)
 *  Bits Allocated: (US, 1, 1)
 *  Bits Stored: (US, 1, 1)
 *  High Bit: (US, 1, 1)
 *  Pixel Representation: (US, 1, 1)
 *  Pixel Data is left out for now
 *  Planar Configuration: (US, 1, 1C)
 *  Pixel Aspect Ratio: (IS,  2, 1C)
 *  ICC Profile: (OB, 1, 3)
 */
class DCMTK_DCMIOD_EXPORT IODImagePixelModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODImagePixelModule(OFshared_ptr<DcmItem> item,
                      OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODImagePixelModule();

  /** Destructor
   */
  virtual ~IODImagePixelModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module
   *  @return Name of the module ("ImagePixelModule")
   */
  virtual OFString getName() const;

  /** Get Samples per Pixel
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSamplesPerPixel(Uint16 &value,
                                         const signed long pos = 0);

  /** Get Photometric Interpretation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
  */
  virtual OFCondition getPhotometricInterpretation(OFString&value,
                                                  const signed long pos = 0);

  /** Get Rows
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRows(Uint16& value,
                              const signed long pos = 0);

  /** Get Columns
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getColumns(Uint16& value,
                                  const signed long pos = 0);
   /** Get Bits Allocated
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getBitsAllocated(Uint16& value,
                                        const signed long pos = 0);

   /** Get Bits Stored
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getBitsStored(Uint16& value,
                                  const signed long pos = 0);

  /** Get High Bit
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getHighBit(Uint16& value,
                                 const signed long pos = 0);

  /** Get Pixel Representation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPixelRepresentation(Uint16& value,
                                             const signed long pos = 0);

  /** Get Planar Configuration
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
  */
  virtual OFCondition getPlanarConfiguration(Uint16& value,
                                             const signed long pos = 0);

  /** Get Pixel Aspect Ratio
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
  */
  virtual OFCondition getPixelAspectRatio(Uint16& value,
                                          const signed long pos = 0);

  /** Get ICC Profile
   *  @param  values Reference to variable in which the values should be stored
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getICCProfile(OFVector<Uint8>& values);

  /** set Samples per Pixel
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value' for conformance with VR (US) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSamplesPerPixel(const Uint16 value,
                                         const OFBool checkValue = OFTrue);

  /** Set Photometric Interpretation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPhotometricInterpretation(const OFString& value,
                                                   const OFBool checkValue = OFTrue);

  /** Set Rows
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRows(const Uint16 value,
                              const OFBool checkValue = OFTrue);

  /** Set Columns
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setColumns(const Uint16 value,
                                 const OFBool checkValue = OFTrue);
  /** Set Bits Allocated
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBitsAllocated(const Uint16 value,
                                       const OFBool checkValue = OFTrue);

  /** Set Bits Stored
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setBitsStored(const Uint16 value,
                                    const OFBool checkValue = OFTrue);

  /** Set High Bit
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setHighBit(const Uint16 value,
                                 const OFBool checkValue = OFTrue);

  /** Set Pixel Representation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value' for conformance with VR (US) and consistency
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPixelRepresentation(const Uint16 value,
                                             const OFBool checkValue = OFTrue);

  /** Set Planar Configuration
   *  @param  value Reference to variable in which the value should be stored
   *  @param  checkValue Check 'value' for conformance with VR (US) and consistency
   *    with other attributes if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPlanarConfiguration(const Uint16 value,
                                             const OFBool checkValue = OFTrue);

  /** Set Pixel Aspect Ratio
   *  @param  verticalPixelSize The vertical pixel size (no unit)
   *  @param  horizontalPixelSize The horizontal pixel size (no unit)
   *  @param  checkValue Check 'value' for conformance with VR (IS)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPixelAspectRatio(const OFString& verticalPixelSize,
                                          const OFString& horizontalPixelSize,
                                          const OFBool checkValue = OFTrue);

  /** Set ICC Profile
   *  @param  values Reference to variable in which the values should be stored
   *  @param  length Length of array provided in values parameter
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setICCProfile(const Uint8* values,
                                    const size_t length);
private:

  /// This module's name ("ImagePixelModule")
  static const OFString m_ModuleName;

};

#endif // MODIMAGEPIXEL_H

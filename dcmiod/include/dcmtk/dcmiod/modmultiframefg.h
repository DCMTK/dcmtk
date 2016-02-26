/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
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
 *  Purpose: Class for managing the Multi-frame Functional Groups Module
 *
 */

#ifndef MODMULTIFRAMEFG_H
#define MODMULTIFRAMEFG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class managing the data from the Multi-Frame Functional Group Module. The
 *  actual functional groups are not included and can be interfaced by the
 *  FGInterface class in module "dcmfg".
 */
class DCMTK_DCMIOD_EXPORT IODMultiFrameFGModule : public IODModule
{

public:

  // Forward declaration
  class ConcatenationInfo;

  /** Constructor
   *  @param  data The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODMultiFrameFGModule(OFshared_ptr<DcmItem> data,
                        OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODMultiFrameFGModule();

  /** Virtual destructor
   */
  virtual ~IODMultiFrameFGModule();

  /** Get name of the module ("MultiframeFunctionalGroupsModule")
   *  @return Name of the module ("MultiframeFunctionalGroupsModule")
   */
  virtual OFString getName() const;

  /** Reset attribute rules to their default values
   */
  virtual void resetRules();

  // -- getters --

  /** Get Instance Number
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getInstanceNumber(Sint32 &value,
                                        const unsigned int pos = 0);

   /** Get Content Date
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getContentDate(OFString &value,
                                     const signed long pos = 0);

   /** Get Content Time
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getContentTime(OFString &value,
                                     const signed long pos = 0);

   /** Get Number of Frames
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getNumberOfFrames(Sint32 &value,
                                        const unsigned int pos = 0);

   /** Get Representative Frame Number
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getRepresentativeFrameNumber(Uint16 &value,
                                                   const unsigned int pos = 0);

  /** Get Concatenation Information
   *  @return Reference to the concatenation information
   */
  virtual ConcatenationInfo& getConcatenationInfo();

   // -- setters --

  /** Set Instance Number
   *  @param  value Value to be stored
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setInstanceNumber(const OFString& value,
                                        const OFBool checkValue = OFTrue);

  /** Set Content Date
   *  @param  value Value to be stored
   *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setContentDate(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Set Content Time
   *  @param  value Value to be stored
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setContentTime(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Set Number of Frames
   *  @param  value Value to be stored
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setNumberOfFrames(const Uint16 value);

  /** Set Representative Frame Number
   *  @param  value Value to be stored
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRepresentativeFrameNumber(const Uint16 value);

  /** Set concatenation information
   *  @param concatenationInfo The concatenation info to set
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition setConcatenationInfo(const ConcatenationInfo& concatenationInfo);

  /** Helper class encapsulating concatenation information, including
   *  Concatenation Frame Offset Number,
   *  Concatenation UID,
   *  SOP Instance UID of Concatenation Source,
   *  In-concatenation Number,
   *  In-concatenation Total Number
   */
  class DCMTK_DCMIOD_EXPORT ConcatenationInfo : public IODComponent
  {

  public:

    /** Constructor
     *  @param  data The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set where this classes rules are added to. If NULL, the
     *          class creates an empty rule set.
     */
    ConcatenationInfo(OFshared_ptr<DcmItem> data,
                      OFshared_ptr<IODRules> rules);

    /** Constructor
     *  @param  parent The parent component of this class (if applicable, might be NULL)
     */
    ConcatenationInfo(IODComponent* parent = NULL);

    /** Virtual destructor
     */
    virtual ~ConcatenationInfo();

    /** Resets attribute rules to their default values
     */
    virtual void resetRules();

    /** Get name of this component ("ConcatenationInfo")
     */
    virtual OFString getName() const;

    // -- getters --

    /** Get Concatenation Frame Offset Number
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getConcatenationFrameOffsetNumber(Uint32& value,
                                                  const unsigned int pos);

    /** Get Concatenation UID
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getConcatenationUID(OFString& value,
                                    const signed long pos);

    /** Get SOP Instance UID of Concatenation Source
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getSOPInstanceUIDOfConcatenationSource(OFString& value,
                                                       const signed long pos);

    /** Get Concatenation Number
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getInConcatenationNumber(Uint16& value,
                                         const unsigned int pos);

    /** Get Concatenation Total Number
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition getInConcatenationTotalNumber(Uint16& value,
                                              const unsigned int pos);

    // -- setters --

    /** Set Concatenation Frame Offset Number
     *  @param  value Value to be stored
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setConcatenationFrameOffsetNumber(const Uint32 value);

    /** Set Concatenation UID
     *  @param  value Value to be stored
     *  @param  checkValue If OFTrue, the value is checked for conformance
     *          (VR = UID and VM = 1).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setConcatenationUID(const OFString &value,
                                            const OFBool checkValue = OFTrue);

    /** Set SOP Instance UID of Concatenation Source
     *  @param  value Value to be stored
     *  @param  checkValue If OFTrue, the value is checked for conformance
     *          (VR = UID and VM = 1).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSOPInstanceUIDOfConcatenationSource(const OFString &value,
                                                               const OFBool checkValue = OFTrue);

    /** Set In-concatenation Number
     *  @param  value Value to be stored
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInConcatenationNumber(const Uint16 value);

    /** Set In-concatenation Total Number
     *  @param  value Value to be stored
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInConcatenationTotalNumber(const Uint16 value);

  }; // ConcatenationInfoClass


private:

  /// Concatenation Information
  ConcatenationInfo m_ConcatenationInfo;

  /// Module name ("MultiframeFunctionalGroupsModule"
  static const OFString m_ModuleName;
};

#endif // MODMULTIFRAMEFG_H

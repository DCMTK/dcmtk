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
 *  Purpose: Class for managing the Synchronization Module
 *
 */

#ifndef MODSYNCHRONIZATION_H
#define MODSYNCHRONIZATION_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofoption.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Synchronization Module:
 *
 *  Synchronization Frame of Reference UID: (UI, 1, 1)
 *  Synchronization Trigger: (CS, 1, 1)
 *  Trigger Source or Type: (LO, 1, 3)
 *  Synchronization Channel: (US, 2, 1C)
 *  Acquisition Time Synchronized: (CS, 1, 1)
 *  Time Source: (SH, 1, 3)
 *  Time Distribution Protocol: (CS, 1, 3)
 *  NTP Source Address (LO, 1, 3)
 */
class DCMTK_DCMIOD_EXPORT IODSynchronizationModule : public IODModule
{

public:

    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    IODSynchronizationModule(OFshared_ptr<DcmItem> item,
                             OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODSynchronizationModule();

    /** Destructor
     */
    virtual ~IODSynchronizationModule();

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("SynchronizationModule")
     */
    virtual OFString getName() const;

    /** Get Synchronization Frame of Reference UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSynchronizationFrameOfReferenceUID(OFString& value,
                                                              const signed long pos = 0) const;

    /** Get Synchronization Trigger
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSynchronizationTrigger(OFString& value,
                                                  const signed long pos = 0) const;

    /** Get Trigger Source or Type
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTriggerSourceOrType(OFString& value,
                                               const signed long pos = 0) const;

    /** Get Synchronization Channel
    *  @param  value Reference to variable in which the value should be stored
    *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSynchronizationChannel(OFVector<Uint16>& value) const;

    /** Get Acquisition Time Synchronized
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     */
    virtual OFCondition getAcquisitionTimeSynchronized(OFString& value,
                                                       const signed long pos = 0) const;

    /** Get Time Source
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     */
    virtual OFCondition getTimeSource(OFString& value,
                                      const signed long pos = 0) const;

    /** Get Time Distribution Protocol
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     */
    virtual OFCondition getTimeDistributionProtocol(OFString& value,
                                                    const signed long pos = 0) const;

    /** Get NTP Source Address
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     */
    virtual OFCondition getNTPSourceAddress(OFString& value,
                                            const signed long pos = 0) const;

    /** Set Synchronization Frame of Reference UID
     *  @param  value Value to be set
     *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSynchronizationFrameofReferenceUID(const OFString &value,
                                                              const OFBool checkValue = OFTrue);

    /** Set Synchronization Trigger
     *  @param  value Value to be set
     *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSynchronizationTrigger(const OFString &value,
                                                  const OFBool checkValue = OFTrue);

    /** Set Trigger Source or Type
     *  @param  value Value to be set
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTriggerSourceOrType(const OFString &value,
                                               const OFBool checkValue = OFTrue);

    /** Set Synchronization Channel
     *  @param  value Value to be set
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSynchronizationChannel(const OFPair<Uint16, Uint16> &value,
                                                  const OFBool checkValue = OFTrue);

    /** Set Acquisition Time Synchronized
     *  @param  value Value to be set
     *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAcquisitionTimeSynchronized(const OFString &value,
                                                       const OFBool checkValue = OFTrue);

    /** Set Time Source
     *  @param  value Value to be set
     *  @param  checkValue Check 'value' for conformance with VR (SH) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTimeSource(const OFString &value,
                                      const OFBool checkValue = OFTrue);

    /** Set Time Distribution Protocol
     *  @param  value Value to be set
     *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTimeDistributionProtocol(const OFString &value,
                                                    const OFBool checkValue = OFTrue);

    /** Set NTP Source Address
     *  @param  value Value to be set
     *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
     *          if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setNTPSourceAddress(const OFString &value,
                                            const OFBool checkValue = OFTrue);

private:

    /// Module name "SynchronizationModule"
    static const OFString m_ModuleName;

};

#endif // MODSYNCHRONIZATION_H

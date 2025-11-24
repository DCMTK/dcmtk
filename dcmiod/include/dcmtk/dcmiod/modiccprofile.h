/*
 *
 *  Copyright (C) 2024-2025, Open Connections GmbH
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
 *  Purpose: Class for managing the ICC Profile Module
 *
 */

#ifndef MODICCPROFILE_H
#define MODICCPROFILE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftypes.h"

/** Class representing the ICC Profile Module:
 *
 * ICC Profile (0028,2000): (OB, 1, 1)
 * Color Space (0028,2002): (CS, 3, 1)
 */
class DCMTK_DCMIOD_EXPORT IODICCProfileModule : public IODModule
{

public:

    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    IODICCProfileModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODICCProfileModule();

    /** Destructor
     */
    virtual ~IODICCProfileModule();

    /** Clear all attributes from the data that are handled by this module.
     *  An attribute is considered belonging to the module if there are rules
     *  marked as belonging to this module via the rule's module name.
     */
    virtual void clearData();

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("ICCProfileModule")
     */
    virtual OFString getName() const;

    /** Read attributes from given item into this class
     *  @param source  The source to read from
     *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
     *         old data is overwritten (or amended)
     *  @result EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& source, const OFBool clearOldData = OFTrue);

    /** Write attributes from this class into given item
     *  @param  destination The item to write to
     *  @result EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition write(DcmItem& destination);

    /** Get the ICC Color Profile data
     *  @param  value Returns a reference to the icc profile data
     *  @param  numBytes Number of bytes in the icc profile data returned in value parameter
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getICCProfile(const Uint8*& value, Uint32& numBytes);

    /** Get Color Space
     *  @param  value Reference to variable in which a copy of the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getColorSpace(OFString& value, const signed long pos = 0) const;

    /** Set ICC Profile data
     *  @param  value The value of ICC Profile (will be copied)
     *  @param  numBytes Number of bytes in the icc profile data (value parameter)
     *  @param  checkValue Check value for conformance with VR (OB) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition
    setICCProfile(const Uint8* value, const unsigned long numBytes, const OFBool checkValue = OFTrue);

    /** Set default ICC Profile data (SRGB)
     *  @param  setColorSpaceDescription If OFTrue, Color Space Description (0028,2004)
     *          is set to "SRGB" when setting the default profile. If not, this attribute
     *          is not touched by this method (i.e. by default it remains unset).
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setDefaultProfile(const OFBool& setColorSpaceDescription = OFTrue);

    /** Set Color Space
     *  @param  value The value of Color Space
     *  @param  checkValue Check value for conformance with VR (CS) and VM (1) if enabled
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setColorSpace(const OFString& value, const OFBool checkValue = OFTrue);

private:

    /// The module's name ("ICCProfileModule")
    static const OFString m_ModuleName;

};

#endif // MODICCPROFILE_H

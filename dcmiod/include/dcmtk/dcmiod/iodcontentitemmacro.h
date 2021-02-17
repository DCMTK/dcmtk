/*
 *
 *  Copyright (C) 2016-2019, Open Connections GmbH
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
 *  Module: dcmpmap
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the Content Item Macro
 *
 */

#ifndef IODCONTENTITEMMACRO_H
#define IODCONTENTITEMMACRO_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/ofstd/ofmem.h"

/** Class representing the Content Item Macro:
 *
 *  ValueType: (CS, 1, 1)
 *  ConceptNameCodeSequence: (SQ, 1, 1)
 *  DateTime: (DT, 1, 1C)
 *  Date: (DA, 1, 1C)
 *  Time: (TM, 1, 1C)
 *  PersonName: (PN, 1, 1C)
 *  UID: (UI, 1, 1C)
 *  TextValue: (UT, 1, 1C)
 *  ConceptCodeSequence: (SQ, 1, 1C)
 *  NumericValue: (DS, 1-n, 1C)
 *  FloatingPointValue: (FD, 1-n, 1C)
 *  RationalNumeratorValue: (SL, 1-n, 1C)
 *  RationalDenominatorValue: (UL, 1-n, 1C)
 *  MeasurementUnitsCodeSequence: (SQ, 1, 1C)
 *  ReferencedSOPSequence: (SQ, 1, 1C)
 */
class DCMTK_DCMIOD_EXPORT ContentItemMacro : public IODComponent
{
public:
    enum ValueType
    {
        // Value is empty and not provided
        VT_EMPTY,
        // Value is provided but not known to the implementation
        VT_UNKNOWN,
        VT_DATE,
        VT_TIME,
        VT_DATETIME,
        VT_PNAME,
        VT_UIDREF,
        VT_TEXT,
        VT_CODE,
        VT_NUMERIC,
        VT_COMPOSITE,
        VT_IMAGE
    };

    /** Class representing an item within the Content Item Macro's Referenced
     *  SOP Sequence
     */
    class DCMTK_DCMIOD_EXPORT ReferencedSOPSequenceItem : public IODComponent
    {
    public:
        /** Constructor
         *  @param  item The item to be used for data storage. If NULL, the
         *          class creates an empty data container.
         *  @param  rules The rule set for this class. If NULL, the class creates
         *          one from scratch and adds its values.
         *  @param  parent The parent of the IOD component (NULL if none or unknown)
         */
        ReferencedSOPSequenceItem(OFshared_ptr<DcmItem> item,
                                  OFshared_ptr<IODRules> rules,
                                  IODComponent* parent = NULL);

        /** Constructor
         *  @param  parent The parent of the IOD component (NULL if none or unknown)
         */
        ReferencedSOPSequenceItem(IODComponent* parent = NULL);

        /** Copy Constructor, performs deep copy
         *  @param  rhs The parent of the IOD component (NULL if none or unknown)
         */
        ReferencedSOPSequenceItem(const ReferencedSOPSequenceItem& rhs);

        /** Virtual Destructor
         */
        virtual ~ReferencedSOPSequenceItem();

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

        /** Resets rules to their original values
         */
        virtual void resetRules();

        /** Get name of macro
         *  @return Name of the component ("ReferencedSOPSequenceItem")
         */
        virtual OFString getName() const;

        /** Get SOPInstanceReferenceMacro
         *  @return a reference to the included SOPInstanceReferenceMacro
         */
        virtual SOPInstanceReferenceMacro& getSOPInstanceReferenceMacro();

        /** Get ReferencedFrameNumber
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getReferencedFrameNumber(OFString& value, const signed long pos = 0) const;

        /** Get ReferencedSegmentNumber
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getReferencedSegmentNumber(Uint16& value, const unsigned long pos = 0) const;

        /** Set ReferencedFrameNumber
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1-n) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setReferencedFrameNumber(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set ReferencedSegmentNumber
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (US) and VM (1-n) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setReferencedSegmentNumber(const Uint16 value, const OFBool checkValue = OFTrue);

    protected:
        /// The name of this item ("ReferencedSOPSequenceItem")
        static const OFString m_ComponentName;

        /// SOP Instance Reference Macro
        SOPInstanceReferenceMacro m_SOPInstanceReferenceMacro;
    };

    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    ContentItemMacro(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    ContentItemMacro();

    /** Destructor
     */
    virtual ~ContentItemMacro();

    ContentItemMacro(const ContentItemMacro& rhs);

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module ("ContentItemMacro")
     *  @return Name of the module ("ContentItemMacro")
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

    /** Get ValueType
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getValueType(OFString& value, const signed long pos = 0) const;

    /** Get ValueType
     *  @param  value Reference to variable in which the value should be stored
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getValueType(ValueType& value) const;

    /** Get reference to the ConceptNameCodeSequence
     *  @return a reference to the ConceptNameCodeSequence
     */
    virtual CodeSequenceMacro* getConceptNameCodeSequence();

    /** Get a reference to the entire ConceptNameCodeSequence, including items
     *  exceeding the value multiplicity restriction of "1"
     *  @return a reference to the entire ConceptNameCodeSequence
     */
    virtual OFVector<CodeSequenceMacro*>& getEntireConceptNameCodeSequence();

    /** Get DateTime
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDateTime(OFString& value, const signed long pos = 0) const;

    /** Get Date
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDate(OFString& value, const signed long pos = 0) const;

    /** Get Time
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTime(OFString& value, const signed long pos = 0) const;

    /** Get PersonName
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPersonName(OFString& value, const signed long pos = 0) const;

    /** Get UID
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUID(OFString& value, const signed long pos = 0) const;

    /** Get TextValue
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTextValue(OFString& value, const signed long pos = 0) const;

    /** Get reference to the ConceptCodeSequence
     *  @return a reference to the ConceptCodeSequence
     */
    virtual CodeSequenceMacro* getConceptCodeSequence();

    /** Get a reference to the entire ConceptCodeSequence, including items
     *  exceeding the value multiplicity restriction of "1"
     *  @return a reference to the entire ConceptCodeSequence
     */
    virtual OFVector<CodeSequenceMacro*>& getEntireConceptCodeSequence();

    /** Get NumericValue
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getNumericValue(OFString& value, const signed long pos = 0) const;

    /** Get FloatingPointValue
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFloatingPointValue(Float64& value, const unsigned long pos = 0) const;

    /** Get RationalNumeratorValue
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRationalNumeratorValue(Sint32& value, const unsigned long pos = 0) const;

    /** Get RationalDenominatorValue
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRationalDenominatorValue(Uint32& value, const unsigned long pos = 0) const;

    /** Get reference to the MeasurementUnitsCodeSequence
     *  @return a reference to the MeasurementUnitsCodeSequence
     */
    virtual CodeSequenceMacro* getMeasurementUnitsCodeSequence();

    /** Get a reference to the entire MeasurementUnitsCodeSequence, including items
     *  exceeding the value multiplicity restriction of "1"
     *  @return a reference to the entire MeasurementUnitsCodeSequence
     */
    virtual OFVector<CodeSequenceMacro*>& getEntireMeasurementUnitsCodeSequence();

    /** Get reference to the ReferencedSOPSequence
     *  @return a reference to the ReferencedSOPSequence
     */
    virtual ReferencedSOPSequenceItem* getReferencedSOPSequence();

    /** Get a reference to the entire ReferencedSOPSequence, including items
     *  exceeding the value multiplicity restriction of "1"
     *  @return a reference to the entire ReferencedSOPSequence
     */
    virtual OFVector<ReferencedSOPSequenceItem*>& getEntireReferencedSOPSequence();

    /** Set ValueType
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setValueType(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set ValueType
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value'. Does nothing, here for consistency with
     *          other set() functions.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setValueType(const ValueType value, const OFBool checkValue = OFTrue);

    /** Set DateTime
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (DT) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDateTime(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Date
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDate(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Time
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTime(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set PersonName
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPersonName(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set UID
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setUID(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set TextValue
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (UT) and VM (1) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTextValue(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set NumericValue
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1-n) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setNumericValue(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set FloatingPointValue
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  pos Index of the value to be set (starting from 0)
     *  @param  checkValue Check 'value' for conformance with VR (FD) and VM (1-n) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition
    setFloatingPointValue(const Float64 value, const unsigned long pos = 0, const OFBool checkValue = OFTrue);

    /** Set RationalNumeratorValue
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  pos Index of the value to be set (starting from 0)
     *  @param  checkValue Check 'value' for conformance with VR (SL) and VM (1-n) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition
    setRationalNumeratorValue(const Sint32 value, const unsigned long pos = 0, const OFBool checkValue = OFTrue);

    /** Set RationalDenominatorValue
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  pos Index of the value to be set (starting from 0)
     *  @param  checkValue Check 'value' for conformance with VR (UL) and VM (1-n) if enabled
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition
    setRationalDenominatorValue(const Uint32 value, const unsigned long pos = 0, const OFBool checkValue = OFTrue);

    virtual OFString toString();

protected:
    /// The name of this module ("ContentItemMacro")
    static const OFString m_ModuleName;

    /// ConceptNameCodeSequence
    OFVector<CodeSequenceMacro*> m_ConceptNameCodeSequence;

    /// ConceptCodeSequence
    OFVector<CodeSequenceMacro*> m_ConceptCodeSequence;

    /// MeasurementUnitsCodeSequence
    OFVector<CodeSequenceMacro*> m_MeasurementUnitsCodeSequence;

    /// ReferencedSOPSequence
    OFVector<ReferencedSOPSequenceItem*> m_ReferencedSOPSequence;
};

#endif // IODCONTENTITEMMACRO_H

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
 *  Purpose: Collection of classes representing DICOM IOD macros
 *
 */

#ifndef IODMACRO_H
#define IODMACRO_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"  // For VR classes, i.e. DcmCodeString etc.
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing a Code Sequence Macro
 */
class DCMTK_DCMIOD_EXPORT CodeSequenceMacro : public IODComponent
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  CodeSequenceMacro(OFshared_ptr<DcmItem> item,
                    OFshared_ptr<IODRules> rules,
                    IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  CodeSequenceMacro(IODComponent* parent = NULL);

  /** Copy Constructor, performs deep copy
   *  @param  rhs The parent of the IOD component (NULL if none or unknown)
   */
  CodeSequenceMacro(const CodeSequenceMacro& rhs);


  /** Convenience constructor to set initial values
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   *  @param  codeValue The code value
   *  @param  codingSchemeDesignator The coding scheme designator
   *  @param  codeMeaning The code meaning
   *  @param  codingSchemeVersion The coding scheme version (might be empty if
   *          coding scheme is unique)
   */
  CodeSequenceMacro(OFshared_ptr<DcmItem> item,
                    OFshared_ptr<IODRules> rules,
                    IODComponent* parent,
                    const OFString& codeValue,
                    const OFString& codingSchemeDesignator,
                    const OFString& codeMeaning,
                    const OFString& codingSchemeVersion = "");

  /** Convenience constructor to set initial values
   *  @param  codeValue The code value
   *  @param  codingSchemeDesignator The coding scheme designator
   *  @param  codeMeaning The code meaning
   *  @param  codingSchemeVersion The coding scheme version (might be empty if
   *          coding scheme is unique)
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  CodeSequenceMacro(const OFString& codeValue,
                    const OFString& codingSchemeDesignator,
                    const OFString& codeMeaning,
                    const OFString& codingSchemeVersion = "",
                    IODComponent* parent = NULL);

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of macro
   *  @return Name of the component ("CodeSequenceMacro")
   */
  virtual OFString getName() const;

  /** Virtual Destructor
   */
  virtual ~CodeSequenceMacro();

   /** Get Code Value
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getCodeValue(OFString &value,
                                   const signed long pos = 0);

   /** Get Coding Scheme Designator
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos   Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getCodingSchemeDesignator(OFString &value,
                                                const signed long pos = 0);

   /** Get Coding Scheme Version
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return  EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getCodingSchemeVersion(OFString &value,
                                             const signed long pos = 0);

   /** Get Code Meaning
    *  @param  value  Reference to variable in which the value should be stored
    *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getCodeMeaning(OFString &value,
                                     const signed long pos = 0);
  /** Set Code Value
   *  @param  value The value to set
   *  @param  checkValue If OFTrue, VM and VR of value are checked
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setCodeValue(const OFString &value,
                                   const OFBool checkValue = OFTrue);

  /** Set Coding Scheme Designator
   *  @param  value The value to set
   *  @param  checkValue If OFTrue, VM and VR of value are checked
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setCodingSchemeDesignator(const OFString &value,
                                                const OFBool checkValue = OFTrue);

  /** Set Coding Scheme Version
   *  @param  value The value to set
   *  @param  checkValue If OFTrue, VM and VR of value are checked
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setCodingSchemeVersion(const OFString &value,
                                             const OFBool checkValue = OFTrue);

  /** Set Code Meaning
   *  @param  value The value to set
   *  @param  checkValue If OFTrue, VM and VR of value are checked
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setCodeMeaning(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Set all values in this class conveniently
   *  @param  value Code Value to set
   *  @param  scheme Coding Scheme Designator to set
   *  @param  meaning Code Meaning to set
   *  @param  schemeVersion The Coding Scheme Designator version to set
   *  (optional)
   *  @param  checkValue If OFTrue, VM and VR of values is checked
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition set(const OFString &value,
                          const OFString &scheme,
                          const OFString &meaning,
                          const OFString &schemeVersion = "",
                          const OFBool checkValue = OFTrue);

};


/** Code with Modifier(s). Represents the combination of a Code Sequence Macro
 *  that is amended by a Modifier Code Sequence with one or more items. The
 *  VM and requirement type of the Modifier Code Sequence can be configured, as
 *  well as the tag of the sequence holding the modifiers (default: Modifier
 *  Code Sequence). Overall, the class reads and write the following structure
 *  from an item:
 *
 *  Coding Scheme Designator, 1, 1C
 *  Coding Scheme Version, 1, 1C
 *  Code Value, 1, 1C
 *  Code Meaning, 1, 1C
 *  Modifier Code Sequence, VM, requirement type
 *  > Code Sequence Macro
 *
 *  Note that the Code Sequence Macro does not support Long Code Value and
 *  URN Code Value, so that Coding Scheme Designator and Code Value are
 *  handled as type 1 (and not 1C as denoted now in the standard and above).
 */
class DCMTK_DCMIOD_EXPORT CodeWithModifiers : public CodeSequenceMacro
{

public:

  /** Constructor
   *  @param  modifierType Denotes type of Modifier Code Sequence (i.e. 1, 1C,
   *          2, 2C or 3), default is 3
   *  @param  modifierVM Denotes how many items are allowed in the Modifier Code
   *          Sequence
   *  @param  modifierSeq Tag of the sequence that holds the modifier codes.
   *          The default is the Modifier Code Sequence.
   */
  CodeWithModifiers(const OFString& modifierType = "3",
                    const OFString& modifierVM = "1-n",
                    const DcmTagKey& modifierSeq = DCM_ModifierCodeSequence);

  /** Copy constructor, performs deep copy.
   *  @param  rhs The component to be copied from
   */
  CodeWithModifiers(const CodeWithModifiers& rhs);

  /** Assignment operator, performs deep copy.
   *  @param  rhs The component to be assigned from
   */
  CodeWithModifiers& operator=(const CodeWithModifiers& rhs);

  /** Virtual Destructor, frees memory
   */
  virtual ~CodeWithModifiers();

  /** Clear all attributes from the data that are handled by this component.
   *  An attribute is considered belonging to the module if there are rules
   *  marked as belonging to this module via the rule's module name.
   */
  void clearData();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get rules handled by this module
   *  @return The rules
   */
  OFshared_ptr<IODRules> getRules()
  {
    return m_Rules;
  }

  /** Get name of component
   *  @return Name of the component
   */
  virtual OFString getName() const;

  /** Get modifier code denoted by index
   *  @param  index Index of modifier code to get (first modifier = 0)
   *  @return Code if modifier with index exists, NULL otherwise
   */
  virtual CodeSequenceMacro* getModifier(const size_t index = 0);

  /** Adds modifier code
   *  @param  modifier The code to be added
   *  @return EC_Normal if adding was successful, error otherwise
   */
  virtual OFCondition addModifier(const CodeSequenceMacro& modifier);

  /** Read attributes from given item into this class
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
   *         old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced. Only
   *                the returned error code will indicate error or OK. Per default,
   *                logging output is produced (OFFalse).
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Comparison operator for IOD Components
   *  @param  rhs The right hand side of the comparison
   *  @return 0, if the given object is equal to this object, other value otherwise
   */
  virtual int compare(const IODComponent& rhs) const;

private:

  /// Items of Modifier Code Sequence
  OFVector<CodeSequenceMacro*> m_Modifiers;

  /// Type 1,2,3,1C or 2C
  OFString m_ModifierType;

  /// 1, 1-n, 2-2n, ...
  OFString m_ModifierVM;

  /// The sequence tag key that contains the modifier codes
  DcmTagKey m_CodeModifierSeq;
};


/** Class implementing the SOP Instance Reference Macro
 */
class DCMTK_DCMIOD_EXPORT SOPInstanceReferenceMacro : public IODComponent
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  SOPInstanceReferenceMacro(OFshared_ptr<DcmItem> item,
                            OFshared_ptr<IODRules> rules,
                            IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  SOPInstanceReferenceMacro(IODComponent* parent = NULL);

  /** Virtual Destructor
   */
  virtual ~SOPInstanceReferenceMacro();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of macro
   *  @return Name of the module ("SOPInstanceReferenceMacro")
   */
  virtual OFString getName() const;

   /** Get Referenced SOP Class UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos   Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
   virtual OFCondition getReferencedSOPClassUID(OFString &value,
                                                const signed long pos = 0);
   /** Get Referenced SOP Instance UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos   Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
   virtual OFCondition getReferencedSOPInstanceUID(OFString &value,
                                                   const signed long pos = 0);
  /** Set Referenced SOP Class UID
   *  @param  value The value to set
   *  @param  checkValue If OFTrue, the value is checked regarding VM and VR
   *  @return EC_Normal, if successful, error otherwise
   */
  virtual OFCondition setReferencedSOPClassUID(const OFString& value,
                                               const OFBool checkValue = OFTrue);
  /** Set Referenced SOP Instance UID
   *  @param  value The value to set
   *  @param  checkValue If OFTrue, the value is checked regarding VM and VR
   *  @return EC_Normal, if successful, error otherwise
   */
  virtual OFCondition setReferencedSOPInstanceUID(const OFString& value,
                                                  const OFBool checkValue = OFTrue);
};


/** Class representing the Series and Instance Reference Macro
 */
class DCMTK_DCMIOD_EXPORT IODSeriesAndInstanceReferenceMacro : public IODComponent
{

public:

  // Forward declaration
  class ReferencedSeriesItem;

  /** Constructor
   *  @param  data The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  IODSeriesAndInstanceReferenceMacro(OFshared_ptr<DcmItem> data,
                                     OFshared_ptr<IODRules> rules,
                                     IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent component of this class (if applicable, might
   *          be NULL)
   */
  IODSeriesAndInstanceReferenceMacro(IODComponent* parent = NULL);

  /** Virtual Destructor
   */
  virtual ~IODSeriesAndInstanceReferenceMacro();

  /** Read Series and Instance Reference Macro from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is deleted before
   *          reading (default)
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write Series and Instance Reference Macro to given item
   *  @param  destination The item to write to
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Get name of module ("SeriesAndInstanceReferenceMacro")
   *  @return Name of the module ("SeriesAndInstanceReferenceMacro")
   */
  virtual OFString getName() const;

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Clear (removes) all attributes handled by the modules of this component.
   *  Rules are not reset.
   */
  virtual void clearData();

  /** Return reference to list of Referenced Series items
   *  @return Reference to list of Reference Series Items
   */
  OFVector<ReferencedSeriesItem*>& getReferencedSeriesItems();

private:

  /// Vector with all items of the Referenced Series Sequence
  OFVector<ReferencedSeriesItem*> m_ReferencedSeriesItems;

  /// Name of this component ("SeriesAndInstanceReferenceMacro")
  static const OFString m_ComponentName;

};


/** Class representing Items from the Referenced Series Sequence:
 *
 * [Referenced Series Sequence: (SQ, VM 1-n, Type 1C)]
 * > Series Instance UID: (UI, 1, 1)
 * > Referenced Instance Sequence: (SQ, 1-n, 1)
 * >> SOP Instance Reference Macro
 *
 */
class DCMTK_DCMIOD_EXPORT IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem : public IODComponent
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  ReferencedSeriesItem(OFshared_ptr<DcmItem> item,
                       OFshared_ptr<IODRules> rules,
                       IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent component of this class (if applicable, might
   *          be NULL)
   */
  ReferencedSeriesItem(IODComponent* parent = NULL);

  /** Destructor
   */
  virtual ~ReferencedSeriesItem();

  /** Clear (removes) all attributes handled by the modules of this component.
   *  Rules are not reset.
   */
  virtual void clearData();

  /** Read Referenced Series Sequence item data from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared before reading
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write this Referenced Series Sequence item data to given item
   *  @param  destination The item to write to
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Resets rules to their original values.
   */
  virtual void resetRules();

  /** Get name of module ("SeriesAndInstanceReferenceMacro")
   *  @return Name of the module ("SeriesAndInstanceReferenceMacro")
   */
  virtual OFString getName() const;

  /** Get Series Instance UID
   *  @param  value  Reference to variable in which the value should be stored
   *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getSeriesInstanceUID(OFString &value,
                                           const signed long pos = 0) const;

  /** Set Series Instance UID
  *  @param  value Value to be set (single value only) or "" for no value
  *  @param  checkValue Check 'value' for conformance with VR and VM if enabled
  *  @return status EC_Normal if successful, an error code otherwise
  */
  virtual OFCondition setSeriesInstanceUID(const OFString& value,
                                           const OFBool checkValue = OFTrue);

  virtual OFCondition addReference(const OFString& sopClassUID,
                                   const OFString& sopInstanceUID);

  /** Get content of the Referenced Instance Sequence
   *  @return Reference to the Referenced Instance Sequence content
  */
  virtual OFVector<SOPInstanceReferenceMacro*>& getReferencedInstanceItems();

private:

  /// The name of this component ("SeriesAndInstanceReferenceMacro")
  static const OFString m_ComponentName;

  /// Vector containing the data of all items of the Referenced Instance Sequence
  OFVector<SOPInstanceReferenceMacro*> m_ReferencedInstanceSequence;
};


/** Class representing the Image SOP Instance Reference Macro
 *  TODO: Change implementation to use IODComponent class inherited anyway
 *  from SOPInstanceReferenceMacro
 */
class DCMTK_DCMIOD_EXPORT ImageSOPInstanceReferenceMacro : public SOPInstanceReferenceMacro
{

public:

  /** Constructor
   */
  ImageSOPInstanceReferenceMacro();

  /** Virtual Destructor
   */
  virtual ~ImageSOPInstanceReferenceMacro();

  /** Creates an ImageSOPInstanceReferenceMacro object without frame/segment
   *  reference from required information.
   *  @param  sopClassUID The SOP Class UID of the reference
   *  @param  sopInstanceUID The SOP Instance UID of the reference
   *  @param  result Returns the resulting object if successful, NULL otherwise
   *  @return EC_Normal if creation was successful, error code otherwise
   */
  static OFCondition create(const OFString& sopClassUID,
                            const OFString& sopInstanceUID,
                            ImageSOPInstanceReferenceMacro*& result);

  /** Creates an ImageSOPInstanceReferenceMacro object with frame or segment
   *  references from required information.
   *  @param  sopClassUID The SOP Class UID of the reference
   *  @param  sopInstanceUID The SOP Instance UID of the reference
   *  @param  refFramesOrSegments Reference to specific frames of an image or
   *          segments of a Segmentation object. The decision (image or
   *          segmentation) is based on the SOP Class; in case it is the
   *          Segmentation Storage SOP Class, the parameter is interpreted
   *          as segment references, otherwise as frame references. If this
   *          parameter is provided empty, then no frame/segment reference is
   *          set at all.
   *  @param  result Returns the resulting object if successful, NULL otherwise
   *  @return EC_Normal if creation was successful, error code otherwise
   */
  static OFCondition create(const OFString& sopClassUID,
                            const OFString& sopInstanceUID,
                            const OFVector<Uint16>& refFramesOrSegments,
                            ImageSOPInstanceReferenceMacro*& result);

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type
   *  @param  rhs The right hand side of the comparison
   *  @return 0 If the object values are equal.
   *          -1 if either the value of the first component that does not match
   *          is lower in the rhs object, or all compared components match
   *          but the rhs component is shorter. Also returned if rhs cannot be
   *          casted to DcmAttributeTag.
   *          1 if either the value of the first component that does not match
   *          is greater in the rhs object, or all compared components match
   *          but the rhs component is longer.
   */
  virtual int compare(const IODComponent& rhs) const;

  /** Clear data
   */
  virtual void clear();

  /** Read Image SOP Instance Reference Macro from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue (default), old data is cleared before reading
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write Image SOP Instance Reference Macrom to given item
   *  @param  item The item to write to
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

   /** Get Referenced Frame Number
    *  @param  values Reference to variable in which the value should be stored
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getReferencedFrameNumber(OFVector<Uint16> &values);

   /** Get Referenced Segment Number
    *  @param  values Reference to variable in which the value should be stored
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getReferencedSegmentNumber(OFVector<Uint16> &values);

  /** Set Referenced Frame Number
   *  @param  values The frame numbers that shoule be referenced
   *  @param  checkValue  If OFTrue (default) the given values will be checked
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition setReferencedFrameNumber(const OFVector<Uint16>& values,
                                               const OFBool checkValue = OFTrue);
  /** Add a Referenced Frame Number
   *  @param  value The frame number to add
   *  @param  checkValue If OFTrue, consistency checks are performed (as possible)
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition addReferencedFrameNumber(const Uint16& value,
                                               const OFBool checkValue = OFTrue);
  /** Set the Referenced Segment Numbers
   *  @param  values The segment numbers to add
   *  @param  checkValue  If OFTrue, consistency checks are performed (as possible)
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition setReferencedSegmentNumber(const OFVector<Uint16>& values,
                                                 const OFBool checkValue = OFTrue);

  /** Add a Referenced Segment Number
   *  @param  value The segment number to add
   *  @param  checkValue If OFTrue, consistency checks are performed (as possible)
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition addReferencedSegmentNumber(const Uint16& value,
                                                 const OFBool checkValue = OFTrue);

private:

  // DICOM attributes.
  // The comments for each attribute describe "Name: (VR, VM, Type)".
  // See DICOM standard for further reference.

  /// Referenced Frame Number: (IS, 1-n, 1C)
  DcmIntegerString ReferencedFrameNumber;

  /// Referenced Segment Number: (US, 1-n, 1C)
  DcmUnsignedShort ReferencedSegmentNumber;

};


/** Class representing an item within the the Primary Anatomic Structure Macro
 */
class DCMTK_DCMIOD_EXPORT PrimaryAnatomicStructureMacroItem
{

public:

  /** Constructor
   */
  PrimaryAnatomicStructureMacroItem();

  /** Copy constructor
   *  @param  rhs The item to copy from
   */
  PrimaryAnatomicStructureMacroItem(const PrimaryAnatomicStructureMacroItem& rhs);

  /** Virtual destructor
   */
  virtual ~PrimaryAnatomicStructureMacroItem();

  /** Clear (removes) all attributes handled by the modules of this component.
   */
  virtual void clearData();

  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced.
   *          Only the returned error code will indicate error or OK. Per
   *          default, logging output is produced.
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Return the Anatomic Structure Code
   *  @return Reference to the anatomic structure code
   */
  virtual CodeSequenceMacro& getAnatomicStructure();

  /** Return the Anatomic Structure Modifier Codes
   *  @return The Anatomic Structure Modifier Codes
   */
  virtual OFVector<CodeSequenceMacro*>& getAnatomicStructureModifier();

  /** Reads Primary Anatomic Region Sequence Item from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared first, otherwise it is
   *          kept where not overwritten
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write Anatomic Region Sequence Item to given item
   * @param  item The item to write to
   * @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  /** Assignment operator (performs deep copy)
   *  @param  rhs The item to copy from
   *  @return result
   */
  PrimaryAnatomicStructureMacroItem& operator=(const PrimaryAnatomicStructureMacroItem &rhs);

private:

  /// Primary Anatomic Structure Sequence Item
  CodeSequenceMacro m_AnatomicStructure;

  /// Primary Anatomic Structure Structure Modifier Sequence (SQ, 1-n, 3)
  OFVector<CodeSequenceMacro*> m_AnatomicStructureModifier;

};


/** Class representing the Primary Anatomic Structure Macro
 */
class DCMTK_DCMIOD_EXPORT PrimaryAnatomicStructureMacro
{

public:

  /** Constructor
   */
  PrimaryAnatomicStructureMacro();

  /** Copy constructor
   *  @param  rhs The macro to copy from
   */
  PrimaryAnatomicStructureMacro(const PrimaryAnatomicStructureMacro& rhs);

  /** Virtual destructor
   */
  virtual ~PrimaryAnatomicStructureMacro();

  /** Clear (removes) all attributes handled by the modules of this component.
   */
  virtual void clearData();

  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced.
   *          Only the returned error code will indicate error or OK. Per
   *          default, logging output is produced.
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Return Primary Anatomic Structure items
   *  @return Reference to items of this macro
   */
  virtual OFVector<PrimaryAnatomicStructureMacroItem*>& getPrimaryAnatomicStructure();

  /** Reads Primary Anatomic Region Sequence (i.e.\ this macro) from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared first, otherwise it is
   *          kept where not overwritten
   *  @return EC_Normal if reading was fine, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write Anatomic Region Sequence (i.e.\ this macro) to given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was fine, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  /** Assignment operator
   *  @param  rhs The macro to copy from
   *  @result Reference to "this" object
   */
  PrimaryAnatomicStructureMacro& operator=(const PrimaryAnatomicStructureMacro &rhs);

private:

  /// Primary Anatomic Structure Sequence (SQ, 0-1, 3)
  OFVector<PrimaryAnatomicStructureMacroItem*> m_PrimaryAnatomicStructure;
};


/** Class representing the General Anatomy Mandatory or Optional Macro
 */
class DCMTK_DCMIOD_EXPORT GeneralAnatomyMacro
{

public:

  /** Constructor
   *  @param type Type of Anatomic Region Sequence. Permitted values: If 1,
   * the class represents the "General Anatomy Mandatory Macro", if
   * type 2 then it behaves like the "General Anatomy Required Macro",
   * and type 3 like the "General Anatomy Optional Macro". Other values
   * are interpreted as type 3 (optional).
   */
  GeneralAnatomyMacro(const OFString& type);

  /** Copy constructor
   *  @param  rhs The macro to copy from
   */
  GeneralAnatomyMacro(const GeneralAnatomyMacro& rhs);

  /** Virtual destructor
   */
  virtual ~GeneralAnatomyMacro();

  /** Clear (removes) all attributes handled by the modules of this component.
   */
  virtual void clearData();

  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced.
   *          Only the returned error code will indicate error or OK. Per
   *          default, logging output is produced (OFFalse).
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Return Anatomic Region
   *  @return Reference to Anatomic Region Code
   */
  virtual CodeSequenceMacro& getAnatomicRegion();

  /** Return Anatomic Region Modifier Codes
   *  @return Reference to Anatomic Region Modifier codes
   */
  virtual OFVector<CodeSequenceMacro*>& getAnatomicRegionModifier();

  /** Return anatomic structure
   *  @return Reference to anatomic structure macro
   */
  virtual PrimaryAnatomicStructureMacro& getPrimaryAnatomicStructure();

  /** Reads Anatomic Region Sequence and Primary Anatomic Structure Macro from
   *  given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared before reading,
   *          otherwise it is kept/overwritten.
   *  @return EC_Normal if no error, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write Anatomic Region Sequence and Primary Anatomic Structure Macro to
   *  given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  /** Assignment operator (performs deep copy)
   *  @param  rhs The macro to copy from
   *  @return Reference to "this" object
   */
  GeneralAnatomyMacro& operator=(const GeneralAnatomyMacro &rhs);

private:

  /// Type (1,2,3) of Anatomic Region Sequence.
  OFString m_Type; // TODO: Make enum?

  /// Anatomic Region Sequence (SQ, 1, 1) (Code Sequence Macro within item of
  /// Anatomic Region Sequence))
  CodeSequenceMacro m_AnatomicRegion;

  /// Anatomic Region Modifier Macro (within item of Anatomic Region Sequence)
  OFVector<CodeSequenceMacro*> m_AnatomicRegionModifier;

  /// Primary Anatomic Structure Macro (on the same level as Anatomic
  /// Region Sequence)
  PrimaryAnatomicStructureMacro m_PrimaryAnatomicStructure;
};


/** Class representing the Algorithm Identification Macro
 */
class DCMTK_DCMIOD_EXPORT AlgorithmIdentificationMacro
{

public:

  /** Constructor
   */
  AlgorithmIdentificationMacro();

  /** Virtual destructor
   */
  virtual ~AlgorithmIdentificationMacro();

  /** Clear (removes) all attributes handled by the modules of this component.
   */
  virtual void clearData();

  /** Perform consistency checks
   *  @param  quiet If OFTrue, not error / warning messages will be produced.
   *          Only the returned error code will indicate error or OK. Per
   *          default, logging output is produced (OFFalse).
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Return Algorithm Family Code
   *  @return Reference to algorithm family code
   */
  virtual CodeSequenceMacro& getAlgorithmFamilyCode();

  /** Get Algorithm Name Code
   *  @return Algorithm Name Code
   */
  virtual CodeSequenceMacro& getAlgorithmNameCode();

  /** Get Algorithm Name
   *  @param  value Reference variable to store the value to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getAlgorithmName(OFString& value,
                                       const signed long pos = 0);

  /** Get Algorithm Version
   *  @param  value Reference variable to store the value to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getAlgorithmVersion(OFString& value,
                                          const signed long pos = 0);

  /** Get Algorithm Parameters
   *  @param  value Reference variable to store the value to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getAlgorithmParameters(OFString& value,
                                             const signed long pos = 0);
  /** Get Algorithm Source
   *  @param  value Reference variable to store the value to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getAlgorithmSource(OFString& value,
                                         const signed long pos = 0);

  /** Set Algorithm Name
   *  @param  value The value to be set
   *  @param  checkValue If OFTrue, the given value is checked
   *  @return EC_Normal, if value could be set, error otherwise
   */
  virtual OFCondition setAlgorithmName(const OFString& value,
                                       const OFBool checkValue = OFTrue);

  /** Set Algorithm Version
   *  @param  value The value to be set
   *  @param  checkValue If OFTrue, the given value is checked
   *  @return EC_Normal, if value could be set, error otherwise
   */
  virtual OFCondition setAlgorithmVersion(const OFString& value,
                                          const OFBool checkValue = OFTrue);

  /** Set Algorithm Parameters
   *  @param  value The value to be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal, if value could be set, error otherwise
   */
  virtual OFCondition setAlgorithmParameters(const OFString& value,
                                             const OFBool checkValue = OFTrue);

  /** Set Algorithm Source
   *  @param  value The value to be set
   *  @param  checkValue If OFTrue, the given value is checked
   *  @return EC_Normal, if value could be set, error otherwise
   */
  virtual OFCondition setAlgorithmSource(const OFString& value,
                                         const OFBool checkValue = OFTrue);

  /** Reads this macro from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared before reading,
   *          otherwise it is kept/overwritten
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write this macro to given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

private:

  /// Code Sequence Macro from Algorithm Family Code Sequence (single item),
  /// (SQ, VM 1, Type 1)
  CodeSequenceMacro m_AlgorithmFamilyCode;

  /// Code Sequence Macro from Algorithm Name Code Sequence (single item)
  /// (SQ, VM 1, Type 3)
  CodeSequenceMacro m_AlgorithmNameCode;

  /// Algorithm Name: (LO, VM 1, Typ 1)
  DcmLongString m_AlgorithmName;

  /// Algorithm Version: (LO, VM 1, Typ 1)
  DcmLongString m_AlgorithmVersion;

  /// Algorithm Parameters: (LT, VM 1, Typ 3)
  DcmLongText m_AlgorithmParameters;

  /// Algorithm Source: (LO, VM 1, Typ 3)
  DcmLongString m_AlgorithmSource;
};


/** Content Identification Macro
 */
class DCMTK_DCMIOD_EXPORT ContentIdentificationMacro
{

public:

  /** Class representing an Alternate Content Description item
   */
  class DCMTK_DCMIOD_EXPORT AlternateContentDescriptionItem
  {
    public:

      /** Constructor
       */
      AlternateContentDescriptionItem();

      /** Virtual destructor
       */
      virtual ~AlternateContentDescriptionItem();

      /** Clear (removes) all attributes handled by the modules of this component.
       */
      virtual void clearData();

      /** Get Content Description
       *  @param  value Reference variable to store the value to
       *  @param  pos Index of the value to get (0..vm-1), -1 for all components
       *  @return EC_Normal if successful, error otherwise
       */
      virtual OFCondition getContentDescription(OFString& value,
                                                const signed long pos = 0);
      /** Get Language Code
       *  @return Reference to the language code
       */
      virtual CodeSequenceMacro& getLanguageCode();

      /** Set Content Description
       *  @param  value The value to set
       *  @param  checkValue If OFTrue, consistency check is performed
       *  @result EC_Normal if setting was successful, error otherwise
       */
      virtual OFCondition setContentDescription(const OFString& value,
                                                const OFBool checkValue = OFTrue);

      /** Read Alternate Content Description Sequence item from given item
       *  @param  source The item to read from
       *  @param  clearOldData If OFTrue, old data is cleared before reading,
       *          otherwise it is kept/overwritten
       *  @result EC_Normal, if reading was successful, error otherwise
       */
      virtual OFCondition read(DcmItem& source,
                               const OFBool clearOldData = OFTrue);

      /** Write alternate content description item to given item
       *  @param  item The item to write to
       *  @result EC_Normal, if writing was successful, error otherwise
       */
      virtual OFCondition write(DcmItem& item);

    private:

      /// Content Description: (LO, VM 1, Type 1)
      DcmLongString m_ContentDescription;

      /// Item of Language Code Sequence: (SQ, VM 1, Type 1)
      CodeSequenceMacro m_LanguageCode;
  };

  /** Constructor
   */
  ContentIdentificationMacro();

  /** Constructor initializing basic data
   *  @param  instanceNumber Instance Number
   *  @param  contentLabel Content Label
   *  @param  contentDescription Content Description, may be empty
   *  @param  contentCreatorName Content Creator's Name, may be empty
   */
  ContentIdentificationMacro(const OFString& instanceNumber,
                             const OFString& contentLabel,
                             const OFString& contentDescription,
                             const OFString& contentCreatorName);

  /** Copy constructor
   *  @param  rhs The macro to copy from (deep copy)
   */
  ContentIdentificationMacro(const ContentIdentificationMacro& rhs);

  /** Create Content Identification Macro with minimally required data.
   *  @param  instanceNumber Instance Number
   *  @param  contentLabel Content Label
   *  @param  contentDescription Content Description, may be empty
   *  @param  contentCreatorName Content Creator's Name, may be empty
   *  @param  result Returns created macro if successful, NULL otherwise
   *  @return EC_Normal if creation was successful, error otherwise
   */
  static OFCondition create(const OFString& instanceNumber,
                            const OFString& contentLabel,
                            const OFString& contentDescription,
                            const OFString& contentCreatorName,
                            ContentIdentificationMacro*& result);

  /** Assignment operator (deep copy)
   *  @param  rhs The macro to copy from
   *  @return Reference to "this" object
   */
  ContentIdentificationMacro& operator=(const ContentIdentificationMacro& rhs);

  /** Virtual destructor
   */
  virtual ~ContentIdentificationMacro();

  /** Get rules for this data structure
   *  @return Reference to the rules
   */
  virtual IODRules& getIODRules();

  /** Clear (removes) all attributes handled by the modules of this component.
   */
  virtual void clearData();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of macro
   *  @return Name of the module ("ContentIdentificationMacro")
   */
  virtual OFString getName() const;


  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced.
   *                Only the returned error code will indicate error or OK. Per
   *                default, logging output is produced (OFFalse).
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Get Instance Number
   *  @param  value Variable to store the result to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getInstanceNumber(OFString& value,
                                        const signed long pos = 0) const;

  /** Get Content Label
   *  @param  value Variable to store the result to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getContentLabel(OFString& value,
                                      const signed long pos = 0) const;

  /** Get Content Description
   *  @param  value Variable to store the result to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getContentDescription(OFString& value,
                                            const signed long pos = 0) const;

  /** Get Alternate Content Description items
   *  @return Reference to the items
   */
  virtual OFVector<AlternateContentDescriptionItem*>& getAlternateContentDescription();

  /** Get Content Creator Name
   *  @param  value Variable to store the result to
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getContentCreatorName(OFString& value,
                                            const signed long pos = 0) const;

  /** Get Content Creator Identification Code
   *  @return Reference to code
   */
  virtual CodeSequenceMacro& getContentCreatorIdentificationCode();

  /** Set Instance Number
   *  @param  value Value to be set
   *  @param  checkValue If OFTrue, value is checked for validity
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setInstanceNumber(const OFString& value,
                                        const OFBool checkValue = OFTrue);

  /** Set Content Label
   *  @param  value Value to be set
   *  @param  checkValue If OFTrue, value is checked for validity
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setContentLabel(const OFString& value,
                                      const OFBool checkValue = OFTrue);

  /** Set Content Description
   *  @param  value Value to be set
   *  @param  checkValue If OFTrue, value is checked for validity
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setContentDescription(const OFString& value,
                                            const OFBool checkValue = OFTrue);

  /** Set Content Creator Name
   *  @param  value Value to be set
   *  @param  checkValue If OFTrue, value is checked for validity
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setContentCreatorName(const OFString& value,
                                            const OFBool checkValue = OFTrue);

  /** Reads Content Identification Macro from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared before reading, otherwise
   *          it is kept/overwritten
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write Content Identification Macro to given item
   *  @param  item The item to write to
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

private:

  // Instance Number: (IS, VM 1, Type 1)
  DcmIntegerString m_InstanceNumber;

  /// Content Label: (CS, VM 1, Type 1)
  DcmCodeString m_ContentLabel;

  /// Content Description: (LO, VM 1, Type 1)
  DcmLongString m_ContentDescription;

  // Alternate Content Description Sequence (VM 1-n, Type 3)
  OFVector<AlternateContentDescriptionItem*> m_AlternateContentDescription;

  /// Content Creator's Name: (LO, VM 1, Type 2)
  DcmPersonName m_ContentCreatorName;

  /// Content Creator's Identification Code Sequence
  CodeSequenceMacro m_ContentCreatorIdentificationCode;

  /// IOD rules for this data structure
  IODRules m_IODRules;
};


/** Class representing the HL7 V2 Hierarchic Designator Macro
 */
class HL7HierarchicDesignatorMacro : public IODComponent
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  HL7HierarchicDesignatorMacro(OFshared_ptr<DcmItem> item,
                               OFshared_ptr<IODRules> rules,
                               IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  HL7HierarchicDesignatorMacro(IODComponent* parent = NULL);

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of macro
   *  @return Name of the module ("HL7HierarchicDesignatorMacro")
   */
  virtual OFString getName() const;

  /** Get Local Namespace Entity ID
   *  @param  value Variable to store the result to
   *  @param  pos The index (0..VM) of the value to get
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getLocalNamespaceEntityID(OFString& value,
                                                const signed long pos = 0) const;

  /** Get Universal Entity ID
   *  @param  value Variable to store the result to
   *  @param  pos The index (0..VM) of the value to get
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getUniversalEntityID(OFString& value,
                                           const signed long pos = 0) const;

  /** Get Universal Entity ID Type
   *  @param  value Variable to store the result to
   *  @param  pos The index (0..VM) of the value to get
   *  @return EC_Normal if value could be get, error otherwise
   */
  virtual OFCondition getUniversalEntityIDType(OFString& value,
                                               const signed long pos = 0) const;

  /** Set Local Namespace Entity ID
   *  @param  value Value to be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setLocalNamespaceEntityID(const OFString& value,
                                            const OFBool checkValue = OFTrue);

  /** Set Universal Entity ID
   *  @param  value Value to be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setUniversalEntityID(const OFString& value,
                                           const OFBool checkValue = OFTrue);

  /** Set Universal Entity ID Type
   *  @param  value Value to be set
   *  @param  checkValue If OFTrue, value is checked for validity
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition setUniversalEntityIDType(const OFString& value,
                                               const OFBool checkValue = OFTrue);
};

/** Class representing the Mandatory View and Slice Progression Direction Macro
 */

class MandatoryViewAndSliceProgressionDirectionMacro : public IODComponent
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *  class creates an empty data container.
   *  @param  rules The rule set where this classes rules are added to. If NULL,
   *          the class creates an empty rule set.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  MandatoryViewAndSliceProgressionDirectionMacro(OFshared_ptr<DcmItem> item,
                                                 OFshared_ptr<IODRules> rules,
                                                 IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  MandatoryViewAndSliceProgressionDirectionMacro(IODComponent* parent = NULL);

  /** Clear (removes) all attributes handled by the modules of this component.
   */
  virtual void clearData();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of macro
   *  @return Name of the module ("MandatoryViewAndSliceProgressionDirectionMacro")
   */
  virtual OFString getName() const;

  /** Read Mandatory View and Slice Progression Direction Macro from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue (default), old data is cleared
   *  before reading
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write Mandatory View and Slice Progression Direction Macro to given item
   *  @param  item The item to write to
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  /** Get View Code
   *  @return Reference to code
   */
  virtual CodeSequenceMacro& getViewCode();

  /** Get View Modifier Code
   *  @return Reference to code
   */
  virtual OFVector<CodeSequenceMacro*>& getViewModifierCode();


protected:

  /// View Code Sequence (SQ, VM 1, type 1)
  CodeSequenceMacro m_ViewCodeSequence;

  /// View Modifier Code Sequence (SQ, VM 1-n, type 2C)
  OFVector<CodeSequenceMacro*> m_ViewModifierCode;
};

#endif // IODMACRO_H

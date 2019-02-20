/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRBasicCodedEntry, DSRCodedEntryValue
 *
 */


#ifndef DSRCODVL_H
#define DSRCODVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"

#include "dcmtk/ofstd/ofexbl.h"


/*----------------------*
 *  class declarations  *
 *----------------------*/

/** Class for storing the "Basic Coded Entry Attributes".
 *  This class should be used to define code constants since it is a lightweight structure
 *  that does not carry any overhead and also does not perform any unnecessary checks on the
 *  data passed to it.  Furthermore, the individual values cannot be modified after an instance
 *  of this class has been constructed.  Therefore, the members can be accessed publicly.
 */
class DCMTK_DCMSR_EXPORT DSRBasicCodedEntry
{
  public:

    /** constructor.
     *  To be used when the code to be set consists of three values (code value, coding scheme
     *  designator and code meaning).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH/UC/UR, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, conditional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  codeValueType           type of 'codeValue' (short, long or URN) used to map
     *                                  the value to the correct DICOM value representation
     *                                  (VR).  The default value is the one most often used
     *                                  (DSRTypes::CVT_Short).
     */
    DSRBasicCodedEntry(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codeMeaning,
                       const DSRTypes::E_CodeValueType codeValueType = DSRTypes::CVT_Short);

    /** constructor.
     *  To be used when the code to be set consists of four values (code value, coding scheme
     *  designator, coding scheme version and code meaning).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH/UC/UR, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, conditional)
     *  @param  codingSchemeVersion     version of the coding scheme.  May be used to identify
     *                                  the version of a coding scheme if necessary to resolve
     *                                  ambiguity in the 'codeValue' or 'codeMeaning'.  (VR=SH,
     *                                  optional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  codeValueType           type of 'codeValue' (short, long or URN) used to map
     *                                  the value to the correct DICOM value representation
     *                                  (VR).  The default value is the one most often used
     *                                  (DSRTypes::CVT_Short).
     */
    DSRBasicCodedEntry(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codingSchemeVersion,
                       const OFString &codeMeaning,
                       const DSRTypes::E_CodeValueType codeValueType = DSRTypes::CVT_Short);

  // --- public but constant members

    /// type of 'CodeValue': short (SH), long (UC) or Uniform Resource Name (UR)
    const DSRTypes::E_CodeValueType CodeValueType;
    /// Code Value (VR=SH/UC/UR, type 1)
    const OFString CodeValue;
    /// Coding Scheme Designator (VR=SH, type 1C)
    const OFString CodingSchemeDesignator;
    /// Coding Scheme Version (VR=SH, type 1C)
    const OFString CodingSchemeVersion;
    /// Code Meaning (VR=LO, type 1)
    const OFString CodeMeaning;
};


/** Class for handling coded entry values, i.e.\ unambiguous machine-readable codes.
 *  This class supports both the "Basic Coded Entry Attributes" and the "Enhanced Encoding
 *  Mode".  There are also some basic checks that make sure that a given code is valid.
 */
class DCMTK_DCMSR_EXPORT DSRCodedEntryValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default constructor
     */
    DSRCodedEntryValue();

    /** constructor.
     *  To be used for code constants defined by an instance of the DSRBasicCodedEntry class.
     ** @param  basicCodedEntry  code to be set, defined by its "Basic Coded Entry Attributes"
     *  @param  check            if enabled, check code for validity before setting it.
     *                           See checkCode() for details.  Empty values are never accepted.
     */
    DSRCodedEntryValue(const DSRBasicCodedEntry &basicCodedEntry,
                       const OFBool check = OFFalse);

    /** constructor.
     *  To be used when the code to be set consists of three values (code value, coding scheme
     *  designator and code meaning).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH/UC/UR, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, conditional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  codeValueType           type of 'codeValue' (short, long or URN) used to map
     *                                  the value to the correct DICOM value representation
     *                                  (VR).  By default, the type is determined automatically
     *                                  (see determineCodeValueType()).
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are never
     *                                  accepted.
     */
    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codeMeaning,
                       const DSRTypes::E_CodeValueType codeValueType = DSRTypes::CVT_auto,
                       const OFExplicitBool check = OFTrue);

    /** constructor.
     *  To be used when the code to be set consists of four values (code value, coding scheme
     *  designator, coding scheme version and code meaning).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH/UC/UR, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, conditional)
     *  @param  codingSchemeVersion     version of the coding scheme.  May be used to identify
     *                                  the version of a coding scheme if necessary to resolve
     *                                  ambiguity in the 'codeValue' or 'codeMeaning'.  (VR=SH,
     *                                  optional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  codeValueType           type of 'codeValue' (short, long or URN) used to map
     *                                  the value to the correct DICOM value representation
     *                                  (VR).  By default, the type is determined automatically
     *                                  (see determineCodeValueType()).
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are only
     *                                  accepted for non-mandatory attributes.
     */
    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codingSchemeVersion,
                       const OFString &codeMeaning,
                       const DSRTypes::E_CodeValueType codeValueType = DSRTypes::CVT_auto,
                       const OFBool check = OFTrue);

    /** copy constructor
     ** @param  codedEntryValue  code to be copied (not checked !)
     */
    DSRCodedEntryValue(const DSRCodedEntryValue &codedEntryValue);

    /** destructor
     */
    virtual ~DSRCodedEntryValue();

    /** assignment operator
     ** @param  codedEntryValue  code to be copied (not checked !)
     ** @return reference to this code after 'codedEntryValue' has been copied
     */
    DSRCodedEntryValue &operator=(const DSRCodedEntryValue &codedEntryValue);

    /** comparison operator "equal".
     *  Two codes are equal if the code value, the coding scheme designator and the (optional)
     *  coding scheme version are equal.  The code meaning or attributes from the "Enhanced
     *  Encoding Mode" are not used for this check.
     ** @param  codedEntryValue  code that should be compared to the current one
     ** @return OFTrue if both codes are equal, OFFalse otherwise
     */
    OFBool operator==(const DSRCodedEntryValue &codedEntryValue) const;

    /** comparison operator "not equal".
     *  Two codes are not equal if either the code value or the coding scheme designator
     *  or the (optional) coding scheme version are not equal.  The code meaning is not
     *  used for this check.
     ** @param  codedEntryValue  code that should be compared to the current one
     ** @return OFTrue if both codes are not equal, OFFalse otherwise
     */
    OFBool operator!=(const DSRCodedEntryValue &codedEntryValue) const;

    /** comparison operator "equal".
     *  Two codes are equal if the code value, the coding scheme designator and the (optional)
     *  coding scheme version are equal.  The code meaning is not used for this check.
     ** @param  basicCodedEntry  code that should be compared to the current one
     ** @return OFTrue if both codes are equal, OFFalse otherwise
     */
    OFBool operator==(const DSRBasicCodedEntry &basicCodedEntry) const;

    /** comparison operator "not equal".
     *  Two codes are not equal if either the code value or the coding scheme designator
     *  or the (optional) coding scheme version are not equal.  The code meaning is not
     *  used for this check.
     ** @param  basicCodedEntry  code that should be compared to the current one
     ** @return OFTrue if both codes are not equal, OFFalse otherwise
     */
    OFBool operator!=(const DSRBasicCodedEntry &basicCodedEntry) const;

    /** clear all internal variables.
     *  Since an empty code is invalid the code becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current code is valid.  This check only covers the "Basic Coded Entry
     *  Attributes".  An empty code is not valid.  See checkCode() for details.
     ** @return OFTrue if code is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current code is empty, i.e.\ whether all four components of the code
     *  ("Basic Coded Entry Attributes") are empty.
     ** @return OFTrue if code is empty, OFFalse otherwise
     */
    virtual OFBool isEmpty() const;

    /** check whether the current code is complete, i.e.\ whether the three (two for URN code
     *  value) mandatory components of the code are non-empty.  This is just a basic check
     *  that might be useful for "validating" input data.  See isValid() for a more
     *  sophisticated way of checking the current code.
     ** @return OFTrue if code is complete, OFFalse otherwise
     */
    virtual OFBool isComplete() const;

    /** print code.
     *  The output of a typical code triple looks like this: (1234,99_OFFIS_DCMTK,"Code
     *  Meaning").  The optional coding scheme version is printed in square brackets directly
     *  after the coding scheme designator, e.g.: (cm,UCUM[1.4],"centimeter").
     *  Please note that only the "Basic Coded Entry Attributes" are supported by this method.
     ** @param  stream          output stream to which the code should be printed
     *  @param  printCodeValue  flag indicating whether the code value and coding scheme
     *                          designator should be printed (default) or not.  If OFFalse,
     *                          the output looks like this: (,,"Code Meaning")
     *  @param  flags           flag used to customize the output (see DSRTypes::PF_xxx)
     */
    void print(STD_NAMESPACE ostream &stream,
               const OFBool printCodeValue = OFTrue,
               const size_t flags = 0) const;

    /** read code sequence from dataset.
     *  The number of items within the code sequence is checked.  If error/warning output are
     *  enabled, a warning message is printed if the sequence is empty or contains more items
     *  than specified in the 'vm ' parameter (1 by default, which applies to most use cases).
     *  However, this method always reads the first item from the given sequence.  If another
     *  item should be read (e.g. a modifier), the method readSequenceItem() should be used.
     ** @param  dataset  DICOM dataset from which the code sequence should be read
     *  @param  tagKey   DICOM tag specifying the attribute (= sequence) that should be read
     *  @param  type     value type of the sequence (valid value: "1", "2", something else).
     *                   This parameter is used for checking purpose, any difference is
     *                   reported.
     *  @param  flags    optional flag used to customize the reading process (see
     *                   DSRTypes::RF_xxx)
     *  @param  vm       value multiplicity to be checked, interpreted as cardinality (number
     *                   of items).  See DcmElement::checkVM() for a list of valid values.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readSequence(DcmItem &dataset,
                             const DcmTagKey &tagKey,
                             const OFString &type,
                             const size_t flags = 0,
                             const OFString &vm = "1");

    /** read code from sequence item
     ** @param  item    DICOM sequence item from which the code should be read
     *  @param  tagKey  DICOM tag specifying the sequence in which this item is contained
     *  @param  flags   flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readSequenceItem(DcmItem &item,
                                 const DcmTagKey &tagKey,
                                 const size_t flags = 0);

    /** write code sequence to dataset
     ** @param  dataset  DICOM dataset to which the code sequence should be written
     *  @param  tagKey   DICOM tag specifying the attribute (= sequence) that should be
     *                   written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeSequence(DcmItem &dataset,
                              const DcmTagKey &tagKey) const;

    /** write code to sequence item
     ** @param  item    DICOM sequence item to which the code should be written
     *  @param  tagKey  DICOM tag specifying the sequence in which this item is contained
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeSequenceItem(DcmItem &item,
                                  const DcmTagKey &tagKey);

    /** read code from XML document.
     *  Please note that only the "Basic Coded Entry Attributes" are supported by this method.
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const DSRXMLDocument &doc,
                        DSRXMLCursor cursor,
                        const size_t flags);

    /** write code in XML format.
     *  Please note that only the "Basic Coded Entry Attributes" are supported by this method.
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(STD_NAMESPACE ostream &stream,
                         const size_t flags) const;

    /** render code in HTML/XHTML format.
     *  Please note that only the "Basic Coded Entry Attributes" are supported by this method.
     ** @param  stream      output stream to which the HTML/XHTML document is written
     *  @param  flags       flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  fullCode    optional flag indicating whether to render the full code tuple
     *                      or the code value/meaning only
     *  @param  valueFirst  optional flag indicating whether to render the code value or
     *                      meaning first (outside the brackets)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTML(STD_NAMESPACE ostream &stream,
                           const size_t flags,
                           const OFBool fullCode = OFTrue,
                           const OFBool valueFirst = OFFalse) const;

    /** get reference to the code
     ** @return reference to the code
     */
    inline const DSRCodedEntryValue &getValue() const
    {
        return *this;
    }

    /** get copy of the code
     ** @param  codedEntryValue  reference to variable in which the code should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRCodedEntryValue &codedEntryValue) const;

    /** get code value.
     *  This is an identifier of the code that is unambiguous within the coding scheme.
     ** @return current code value (might be invalid or an empty string)
     */
    inline const OFString &getCodeValue() const
    {
        return CodeValue;
    }

    /** get type of code value, i.e.\ short, long or Uniform Resource Name (URN).
     *  The type is used to map the value to the correct DICOM value representation (VR).
     ** @return type of code value.  Should never be DSRTypes::CVT_auto for non-empty value.
     */
    inline DSRTypes::E_CodeValueType getCodeValueType() const
    {
        return CodeValueType;
    }

    /** get coding scheme designator.
     *  This is an identifier of the coding scheme in which the code for a term is defined.
     *  Designators beginning with "99" and the designator "L" are defined to be private or
     *  local coding schemes.  More details on the coding scheme might be retrieved via the
     *  DSRDocument::getCodingSchemeIdentification() method.
     ** @return current coding scheme designator (might be invalid or an empty string)
     */
    inline const OFString &getCodingSchemeDesignator() const
    {
        return CodingSchemeDesignator;
    }

    /** get coding scheme version.
     *  Optional - May be used to identify the version of a coding scheme if necessary to
     *  resolve ambiguity in the code value or code meaning.  Coding scheme version is not
     *  required for backward-compatible revisions of a coding scheme.
     ** @return current coding scheme version (might be invalid or an empty string)
     */
    inline const OFString &getCodingSchemeVersion() const
    {
        return CodingSchemeVersion;
    }

    /** get code meaning.
     *  Human-readable translation of the code value.  Can be used for display when code
     *  dictionary is not available.
     ** @return current code meaning (might be invalid or an empty string)
     */
    inline const OFString &getCodeMeaning() const
    {
        return CodeMeaning;
    }

    /** get context identifier.
     *  Optional - Identifier of the context group defined by a mapping resource from which
     *  the code was selected, or to which the code has been added as a private context group
     *  extension.  This attribute is part of the "Enhanced Encoding Mode".
     ** @return current context identifier (might be invalid or an empty string)
     */
    inline const OFString &getContextIdentifier() const
    {
        return ContextIdentifier;
    }

    /** get context UID.
     *  Optional - Uniquely identifies the context group.  This attribute is part of the
     *  "Enhanced Encoding Mode".
     ** @return current context UID (might be invalid or an empty string)
     */
    inline const OFString &getContextUID() const
    {
        return ContextUID;
    }

    /** get mapping resource.
     *  Optional - Message/Terminology mapping resource that specifies the context group with
     *  the given identifier.  This attribute is part of the "Enhanced Encoding Mode".
     ** @return current mapping resource (might be invalid or an empty string)
     */
    inline const OFString &getMappingResource() const
    {
        return MappingResource;
    }

    /** get context group version.
     *  Optional - Version of the context group.  This attribute is part of the "Enhanced
     *  Encoding Mode".
     ** @return current context group version (might be invalid or an empty string)
     */
    inline const OFString &getContextGroupVersion() const
    {
        return ContextGroupVersion;
    }

    /** get context group local version.
     *  Optional - Implementation-specific version of a context group that contains private
     *  extensions.  This attribute is part of the "Enhanced Encoding Mode".
     ** @return current context group local version (might be invalid or an empty string)
     */
    inline const OFString &getContextGroupLocalVersion() const
    {
        return ContextGroupLocalVersion;
    }

    /** get context group extension creator UID.
     *  Optional - Identifies the person or organization who created the extension to the
     *  context group.  This attribute is part of the "Enhanced Encoding Mode".
     ** @return current context group extension creator UID (might be invalid or an empty
     *    string)
     */
    inline const OFString &getContextGroupExtensionCreatorUID() const
    {
        return ContextGroupExtensionCreatorUID;
    }

    /** set code.
     *  Before setting the code, it is usually checked.  If the code is invalid, the current
     *  code is not replaced and remains unchanged.
     ** @param  codedEntryValue  code to be set
     *  @param  check            if enabled, check code for validity before setting it.
     *                           See checkCode() for details.  Empty values are only accepted
     *                           for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRCodedEntryValue &codedEntryValue,
                         const OFBool check = OFTrue);

    /** set code.
     *  To be used for code constants defined by an instance of the DSRBasicCodedEntry class.
     ** @param  basicCodedEntry  code to be set, defined by its "Basic Coded Entry Attributes"
     *  @param  check            if enabled, check code for validity before setting it.
     *                           See checkCode() for details.  Empty values are never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCode(const DSRBasicCodedEntry &basicCodedEntry,
                        const OFBool check = OFFalse);

    /** set code.
     *  To be used when the code to be set consists of three values (code value, coding scheme
     *  designator and code meaning).  The attributes from the "Enhanced Encoding Mode" are set
     *  by one of the setEnhancedEncodingMode() methods.
     *  Before setting the code, it is usually checked.  If the code is invalid, the current
     *  code is not replaced and remains unchanged.  Additional information on the coding
     *  scheme can be provided via the DSRDocument::getCodingSchemeIdentification() method
     *  (highly recommended for private coding schemes).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH/UC/UR, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, conditional)
     *                                  Designators beginning with "99" and the designator
     *                                  "L" are defined to be private or local coding schemes.
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  codeValueType           type of 'codeValue' (short, long or URN) used to map
     *                                  the value to the correct DICOM value representation
     *                                  (VR).  By default, the type is determined automatically
     *                                  (see determineCodeValueType()).
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are only
     *                                  accepted for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCode(const OFString &codeValue,
                        const OFString &codingSchemeDesignator,
                        const OFString &codeMeaning,
                        const DSRTypes::E_CodeValueType codeValueType = DSRTypes::CVT_auto,
                        const OFExplicitBool check = OFTrue);

    /** set code.
     *  To be used when the code to be set consists of four values (code value, coding scheme
     *  designator, coding scheme version and code meaning).  The attributes from the "Enhanced
     *  Encoding Mode" are set by one of the setEnhancedEncodingMode() methods.
     *  Before setting the code, it is usually checked.  If the code is invalid, the current
     *  code is not replaced and remains unchanged.  Additional information on the coding
     *  scheme can be provided via the DSRDocument::getCodingSchemeIdentification() method
     *  (highly recommended for private coding schemes).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, conditional)
     *                                  Designators beginning with "99" and the designator
     *                                  "L" are defined to be private or local coding schemes.
     *  @param  codingSchemeVersion     version of the coding scheme.  May be used to identify
     *                                  the version of a coding scheme if necessary to resolve
     *                                  ambiguity in the 'codeValue' or 'codeMeaning.  (VR=SH,
     *                                  optional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  codeValueType           type of 'codeValue' (short, long or URN) used to map
     *                                  the value to the correct DICOM value representation
     *                                  (VR).  By default, the type is determined automatically
     *                                  (see determineCodeValueType()).
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are only
     *                                  accepted for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCode(const OFString &codeValue,
                        const OFString &codingSchemeDesignator,
                        const OFString &codingSchemeVersion,
                        const OFString &codeMeaning,
                        const DSRTypes::E_CodeValueType codeValueType = DSRTypes::CVT_auto,
                        const OFBool check = OFTrue);

    /** check whether the "Enhanced Encoding Mode" is used for this code.
     *  Currently, the only check that is performed is that either the context identifier or
     *  the context UID have a non-empty value.
     ** @return OFTrue if the "Enhanced Encoding Mode" is used, OFFalse otherwise
     */
    OFBool usesEnhancedEncodingMode() const;

    /** remove the "Enhanced Encoding Mode" from this code.
     *  Internally, all elements that belong to this mode are cleared.
     */
    void removeEnhancedEncodingMode();

    /** specify the "Enhanced Encoding Mode" for this code.
     *  This method should be used for private context groups, which are not identified by a
     *  context identifier and mapping resource.
     ** @param  contextUID  uniquely identifies the context group.  (VR=UI, mandatory)
     *  @param  check       if enabled, the given value is checked for validity (conformance
     *                      with corresponding VR and VM) before setting it.  An empty value
     *                      is never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setEnhancedEncodingMode(const OFString &contextUID,
                                        const OFBool check = OFTrue);

    /** specify the "Enhanced Encoding Mode" for this code.
     *  This method should be used for codes from or extensions to non-private context groups,
     *  e.g. from the DICOM Content Mapping Resource (DCMR).
     ** @param  contextIdentifier    identifier of the context group defined by
     *                               'mappingResource' from which the code was selected, or to
     *                               which the code has been added as a private context group
     *                               extension.  (VR=CS, mandatory)
     *  @param  mappingResource      message/terminology mapping resource that specifies the
     *                               context group with the given identifier.  (VR=CS,
     *                               mandatory)
     *  @param  contextGroupVersion  version of the context group.  (VR=DT, mandatory)
     *  @param  contextUID           uniquely identifies the context group.  (VR=UI, optional)
     *  @param  localVersion         implementation-specific version of a context group that
     *                               contains private extensions.  (VR=DT, conditional)
     *  @param  extensionCreatorUID  identifies the person or organization who created the
     *                               extension to the context group.  (VR=UI, conditional)
     *                               Should be specified if 'localVersion' is non-empty.
     *  @param  check                if enabled, the given values are checked for validity
     *                               (conformance with corresponding VR and VM) before setting
     *                               them.  Empty values are only accepted for non-mandatory
     *                               attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setEnhancedEncodingMode(const OFString &contextIdentifier,
                                        const OFString &mappingResource,
                                        const OFString &contextGroupVersion,
                                        const OFString &contextUID = "",
                                        const OFString &localVersion = "",
                                        const OFString &extensionCreatorUID = "",
                                        const OFBool check = OFTrue);

    /** check the currently stored code for validity.
     *  See below checkCode() method for details.
     ** @return status, EC_Normal if current value is valid, an error code otherwise
     */
    OFCondition checkCurrentValue() const;


  protected:

    /** get pointer to this code
     ** @return pointer to this code (never NULL)
     */
    inline DSRCodedEntryValue *getValuePtr()
    {
        return this;
    }

    /** read code from dataset.
     *  This method also supports the attributes from the "Enhanced Encoding Mode".
     ** @param  dataset     DICOM dataset from which the code should be read
     *  @param  moduleName  module name (sequence) from which the item is read.  If NULL,
     *                      the default value "SR document" is used for output messages.
     *  @param  flags       flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readItem(DcmItem &dataset,
                         const char *moduleName,
                         const size_t flags);

    /** write code to dataset.
     *  This method also supports the attributes from the "Enhanced Encoding Mode".
     ** @param  dataset  DICOM dataset to which the code should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeItem(DcmItem &dataset) const;

  // --- static helper functions ---

    /** check the specified code for validity.
     *  Currently, the only checks performed are that the three (two for URN code value)
     *  mandatory string values are non-empty and that all four values conform to the
     *  corresponding VR and VM.  Later on, it might also be checked whether the specified
     *  code really belongs to the coding
     *  scheme, etc.  This requires the presence of the relevant code dictionaries, though.
     ** @param  codeValue               code value to be checked
     *  @param  codingSchemeDesignator  coding scheme designator to be checked
     *  @param  codingSchemeVersion     coding scheme version to be checked (might be empty)
     *  @param  codeMeaning             code meaning to be checked
     *  @param  codeValueType           type of 'codeValue' (short, long or URN).
     *                                  Never use DSRTypes::CVT_auto for this function.
     ** @return status, EC_Normal if code is valid, an error code otherwise
     */
    static OFCondition checkCode(const OFString &codeValue,
                                 const OFString &codingSchemeDesignator,
                                 const OFString &codingSchemeVersion,
                                 const OFString &codeMeaning,
                                 const DSRTypes::E_CodeValueType codeValueType);

    /** try to determine the type of the given code value.
     *  Please note that the check that is currently performed is very simple.  So, the user
     *  is advised to passed the correct type (short, long or URN) to the appropriate method
     *  and should not rely on this automatic detection.  Specifically, URN values are only
     *  detected by the prefix "urn:" (i.e. no URL allowed), and the maximum length of a
     *  short code value is determined based on the number of bytes (not characters, since
     *  the character set is unknown to this function - and to this class).
     ** @param  codeValue  code value to be checked
     ** @return automatically determined type that should be used for the given code value
     */
    static DSRTypes::E_CodeValueType determineCodeValueType(const OFString &codeValue);


  private:

    // -- Basic Coded Entry Attributes --

    /// type of 'CodeValue': short (SH), long (UC) or Uniform Resource Name (UR)
    DSRTypes::E_CodeValueType CodeValueType;
    /// Code Value (VR=SH/UC/UR, type 1)
    OFString CodeValue;
    /// Coding Scheme Designator (VR=SH, type 1C)
    OFString CodingSchemeDesignator;
    /// Coding Scheme Version (VR=SH, type 1C)
    OFString CodingSchemeVersion;
    /// Code Meaning (VR=LO, type 1)
    OFString CodeMeaning;

    /// tbd: Equivalent Code Sequence (VR=SQ, type 3) not yet supported

    // -- Enhanced Encoding Mode --

    /// Context Identifier (VR=CS, type 3)
    OFString ContextIdentifier;
    /// Context UID (VR=UI, type 2)
    OFString ContextUID;
    /// Mapping Resource (VR=CS, type 1C)
    OFString MappingResource;
    /// Mapping Resource UID (VR=UI, type 3)
     // - tbd: optional attribute not yet supported
    /// Mapping Resource Name (VR=LO, type 3)
     // - tbd: optional attribute not yet supported
    /// Context Group Version (VR=DT, type 1C)
    OFString ContextGroupVersion;
    /// Context Group Local Version (VR=DT, type 1C)
    OFString ContextGroupLocalVersion;
    /// Context Group Extension Creator UID (VR=UI, type 1C)
    OFString ContextGroupExtensionCreatorUID;
};


/** output stream operator for coded entry values.
 *  Internally, the DSRCodedEntryValue::print() method is used, i.e. the output looks
 *  like this: (1234,99_OFFIS_DCMTK,"Code Meaning") or (cm,UCUM[1.4],"centimeter")
 *  @param  stream           output stream to which the coded entry value is printed
 *  @param  codedEntryValue  coded entry value to be printed
 *  @return reference to output stream
 */
DCMTK_DCMSR_EXPORT STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream,
                                                     const DSRCodedEntryValue& codedEntryValue);


#endif

/*
 *
 *  Copyright (C) 2000-2012, OFFIS e.V.
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
 *    classes: DSRCodedEntryValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2012-06-11 08:53:01 $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRCODVL_H
#define DSRCODVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"

#include "dcmtk/ofstd/ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for coded entry values
 */
class DCMTK_DCMSR_EXPORT DSRCodedEntryValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default contructor
     */
    DSRCodedEntryValue();

    /** constructor
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, mandatory)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are never
     *                                  accepted.
     */
    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codeMeaning,
                       const OFBool check = OFTrue);

    /** constructor
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, mandatory)
     *  @param  codingSchemeVersion     version of the coding scheme.  May be used to identify
     *                                  the version of a coding scheme if necessary to resolve
     *                                  ambiguity in the 'codeValue' or 'codeMeaning.  (VR=SH,
     *                                  optional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are only
     *                                  accepted for non-mandatory attributes.
     */
    DSRCodedEntryValue(const OFString &codeValue,
                       const OFString &codingSchemeDesignator,
                       const OFString &codingSchemeVersion,
                       const OFString &codeMeaning,
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

    /** comparison operator.
     *  Two codes are equal if the code value, coding scheme designator and the (optional)
     *  coding scheme version are equal.  The code meaning is not relevant for this check.
     ** @param  codedEntryValue  code which should be compared to the current one
     ** @return OFTrue if both codes are equal, OFFalse otherwise
     */
    OFBool operator==(const DSRCodedEntryValue &codedEntryValue) const;

    /** clear all internal variables.
     *  Since an empty code is invalid the code becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current code is valid.  This check only covers the "Basic Coded Entry
     *  Attributes".  See checkCode() for details.
     ** @return OFTrue if code is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the current code is empty, i.e.\ whether all four components of the code
     *  ("Basic Coded Entry Attributes") are empty.
     ** @return OFTrue if code is empty, OFFalse otherwise
     */
    virtual OFBool isEmpty() const;

    /** print code.
     *  The output of a typical code triple looks like this: (1234,99_OFFIS_DCMTK,"Code
     *  Meaning").  The optional coding scheme version is printed in square brackets directly
     *  after the coding scheme designator, e.g.: (cm,UCUM[1.4],"centimeter").
     *  Please note that only the "Basic Coded Entry Attributes" are supported by this method.
     ** @param  stream          output stream to which the code should be printed
     *  @param  printCodeValue  flag indicating whether the code value and coding scheme
     *                          designator should be printed (default) or not.  If OFFalse the
     *                          output looks like this: (,,"Code Meaning")
     *  @param  printInvalid    flag indicating whether the text "invalid code" should be
     *                          printed for invalid codes or not (default)
     */
    void print(STD_NAMESPACE ostream &stream,
               const OFBool printCodeValue = OFTrue,
               const OFBool printInvalid = OFFalse) const;

    /** read code sequence from dataset.
     *  The number of items within the code sequence is checked.  If error/warning output are
     *  enabled, a warning message is printed if the sequence is empty or contains more than
     *  one item.  The latter is a limitation of the current implementation, knowing that
     *  there are code sequences for which more than one item is allowed.
     ** @param  dataset  DICOM dataset from which the code sequence should be read
     *  @param  tagKey   DICOM tag specifying the attribute (= sequence) which should be read
     *  @param  type     value type of the sequence (valid value: "1", "2", something else).
     *                   This parameter is used for checking purpose, any difference is
     *                   reported.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readSequence(DcmItem &dataset,
                             const DcmTagKey &tagKey,
                             const OFString &type);

    /** write code sequence to dataset
     ** @param  dataset  DICOM dataset to which the code sequence should be written
     *  @param  tagKey   DICOM tag specifying the attribute (= sequence) which should be
     *                   written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeSequence(DcmItem &dataset,
                              const DcmTagKey &tagKey) const;

    /** read code from XML document.
     *  Please note that only the "Basic Coded Entry Attributes" are supported by this method.
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const DSRXMLDocument &doc,
                        DSRXMLCursor cursor);

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
     *  code is not replaced and remains unchanged.  The attributes from the "Enhanced
     *  Encoding Mode" are set by one of the setEnhancedEncodingMode() methods.
     ** @param  codedEntryValue  code to be set
     *  @param  check            if enabled, check code for validity before setting it.
     *                           See checkCode() for details.  Empty values are only accepted
     *                           for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRCodedEntryValue &codedEntryValue,
                         const OFBool check = OFTrue);

    /** set code.
     *  Before setting the code, it is usually checked.  If the code is invalid, the current
     *  code is not replaced and remains unchanged.  Additional information on the coding
     *  scheme can be provided via the DSRDocument::getCodingSchemeIdentification() method
     *  (highly recommended for private coding schemes).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, mandatory)
     *                                  Designators beginning with "99" and the designator
     *                                  "L" are defined to be private or local coding schemes.
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are only
     *                                  accepted for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCode(const OFString &codeValue,
                        const OFString &codingSchemeDesignator,
                        const OFString &codeMeaning,
                        const OFBool check = OFTrue);

    /** set code.
     *  Before setting the code, it is usually checked.  If the code is invalid, the current
     *  code is not replaced and remains unchanged.  Additional information on the coding
     *  scheme can be provided via the DSRDocument::getCodingSchemeIdentification() method
     *  (highly recommended for private coding schemes).
     ** @param  codeValue               identifier of the code to be set that is unambiguous
     *                                  within the coding scheme.  (VR=SH, mandatory)
     *  @param  codingSchemeDesignator  identifier of the coding scheme in which the code for
     *                                  a term is defined.  (VR=SH, mandatory)
     *                                  Designators beginning with "99" and the designator
     *                                  "L" are defined to be private or local coding schemes.
     *  @param  codingSchemeVersion     version of the coding scheme.  May be used to identify
     *                                  the version of a coding scheme if necessary to resolve
     *                                  ambiguity in the 'codeValue' or 'codeMeaning.  (VR=SH,
     *                                  optional)
     *  @param  codeMeaning             human-readable translation of the 'codeValue'.  Can be
     *                                  used for display when code dictionary is not available.
     *                                  (VR=LO, mandatory)
     *  @param  check                   if enabled, check code for validity before setting it.
     *                                  See checkCode() for details.  Empty values are only
     *                                  accepted for non-mandatory attributes.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCode(const OFString &codeValue,
                        const OFString &codingSchemeDesignator,
                        const OFString &codingSchemeVersion,
                        const OFString &codeMeaning,
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
     *  context identifier and mapping resource.  Before setting the code, it is usually
     *  checked.  If the code is invalid, the current code is not replaced and remains
     *  unchanged.
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
     *  @param  moduleName  optional module name (sequence) from which the item is read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readItem(DcmItem &dataset,
                         const char *moduleName = NULL);

    /** write code to dataset.
     *  This method also supports the attributes from the "Enhanced Encoding Mode".
     ** @param  dataset    DICOM dataset to which the code should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeItem(DcmItem &dataset) const;

    /** check the specified code for validity.
     *  Currently, the only checks performed are that the three mandatory string values are
     *  non-empty and that all four values conform to the corresponding VR and VM.  Later on,
     *  it might also be checked whether the specified code really belongs to the coding
     *  scheme, etc.  This requires the presence of the relevant code dictionaries, though.
     ** @param  codeValue               code value to be checked
     *  @param  codingSchemeDesignator  coding scheme designator to be checked
     *  @param  codingSchemeVersion     coding scheme version to be checked (might be empty)
     *  @param  codeMeaning             code meaning to be checked
     ** @return status, EC_Normal if code is valid, an error code otherwise
     */
    OFCondition checkCode(const OFString &codeValue,
                          const OFString &codingSchemeDesignator,
                          const OFString &codingSchemeVersion,
                          const OFString &codeMeaning) const;


  private:

    // -- Basic Coded Entry Attributes --

    /// Code Value (VR=SH, type 1)
    OFString CodeValue;
    /// Coding Scheme Designator (VR=SH, type 1)
    OFString CodingSchemeDesignator;
    /// Coding Scheme Version (VR=SH, 1C)
    OFString CodingSchemeVersion;
    /// Code Meaning (VR=LO, type 1)
    OFString CodeMeaning;

    // -- Enhanced Encoding Mode --

    /// Context Identifier (VR=CS, type 3)
    OFString ContextIdentifier;
    /// Context UID (VR=UI, type 2)
    OFString ContextUID;
    /// Mapping Resource (VR=CS, type 1C)
    OFString MappingResource;
    /// Context Group Version (VR=DT, type 1C)
    OFString ContextGroupVersion;
    /// Context Group Local Version (VR=DT, type 1C)
    OFString ContextGroupLocalVersion;
    /// Context Group Extension Creator UID (VR=UI, type 1C)
    OFString ContextGroupExtensionCreatorUID;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcodvl.h,v $
 *  Revision 1.25  2012-06-11 08:53:01  joergr
 *  Added optional "check" parameter to "set" methods and enhanced documentation.
 *
 *  Revision 1.24  2012-01-06 09:13:04  uli
 *  Make it possible to build dcmsr as a DLL.
 *
 *  Revision 1.23  2011-12-14 15:45:09  joergr
 *  Added intitial support for the "Enhanced Encoding Mode", which comprises some
 *  optional attributes that further describe a code value.
 *
 *  Revision 1.22  2011-08-02 06:26:31  joergr
 *  Fixed typos and/or minor formatting issues.
 *
 *  Revision 1.21  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.20  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.19  2007-11-15 16:33:30  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.18  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.17  2006/05/11 09:18:21  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.16  2005/12/08 16:04:52  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.15  2004/11/22 16:39:09  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.14  2003/12/16 15:56:53  joergr
 *  Added note on coding scheme identfication and private coding schemes.
 *
 *  Revision 1.13  2003/12/11 17:14:28  joergr
 *  Adapted documentation of print() method to new output format of CodingScheme
 *  Version (square brackets instead of comma to separate from CodingScheme).
 *
 *  Revision 1.12  2003/09/10 13:16:13  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdenticationSequence as
 *  required by CP 324.
 *
 *  Revision 1.11  2003/08/07 17:31:00  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.10  2003/08/07 12:20:48  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.9  2001/09/26 13:04:04  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:50:59  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/11/09 20:32:07  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.6  2000/11/06 11:14:55  joergr
 *  Updated comments/formatting.
 *
 *  Revision 1.5  2000/11/01 16:12:55  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.4  2000/10/23 15:08:33  joergr
 *  Added/updated doc++ comments.
 *
 *  Revision 1.3  2000/10/19 16:02:13  joergr
 *  Renamed some set methods.
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.2  2000/10/18 17:00:29  joergr
 *  Added methods allowing direct access to certain content item values.
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */

/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContextGroup
 *
 */


#ifndef DSRCTXGR_H
#define DSRCTXGR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcodvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing common information on a context group.
 *  This class is used as a base class only.
 */
class DCMTK_DCMSR_EXPORT DSRContextGroup
{

  public:

    /** conversion operator that returns the currently selected coded entry value
     *  (might be empty if none is selected)
     */
    operator const DSRCodedEntryValue &() const
    {
        return getSelectedValue();
    }

    /** clear list of extended coded entries and forget about the currently selected
     *  coded entry
     */
    virtual void clear();

    /** check whether context group is extensible
     ** @return OFTrue if extensible, OFFalse otherwise
     */
    inline OFBool isExtensible() const
    {
        return ExtensibleMode;
    }

    /** change mode specifying whether the context group is extensible or non-extensible
     ** @param  mode  set context group type to extensible if OFTrue (default)
     */
    inline void setExtensible(const OFBool mode = OFTrue)
    {
        ExtensibleMode = mode;
    }

    /** get context identifier
     ** @return identifier of the context group
     */
    inline const OFString &getIdentifier() const
    {
        return Identifier;
    }

    /** get mapping resource
     ** @return mapping resource that defines the context group
     */
    inline const OFString &getMappingResource() const
    {
        return MappingResource;
    }

    /** get context group version (optional)
     ** @return version of the context group (might be empty)
     */
    inline const OFString &getVersion() const
    {
        return Version;
    }

    /** get context group UID (optional)
     ** @return unique identifier of the context group (might be empty)
     */
    inline const OFString &getUID() const
    {
        return UID;
    }

    /** check whether a coded entry is selected as the current value
     ** @return OFTrue if a value is selected, OFFalse otherwise
     */
    virtual OFBool hasSelectedValue() const;

    /** get currently selected coded entry value (optional)
     ** @return current value (might be empty if none is selected)
     */
    inline const DSRCodedEntryValue &getSelectedValue() const
    {
        return SelectedValue;
    }

    /** select a given coded entry as the current value
     ** @param  codedEntryValue      coded entry to be selected as the current value
     *  @param  check                if enabled, check 'codedEntryValue' for validity and
     *                               appropriateness before selecting it as the current
     *                               value.  See checkSelectedValue() for details.
     *  @param  definedContextGroup  flag indicating whether this context group is used as
     *                               a baseline (default) or defined context group.  The
     *                               first means that the list of coded entries in this
     *                               context group is just a suggestion.  The latter
     *                               means that one of the coded entries should be used.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const DSRCodedEntryValue &codedEntryValue,
                                    const OFBool check = OFTrue,
                                    const OFBool definedContextGroup = OFFalse);

    /** check whether a given coded entry is contained in this context group.
     *  Internally, the method findCodedEntry() is used for this purpose.
     ** @param  codedEntryValue  coded entry to be checked
     ** @return OFTrue if coded entry is part of this context group, OFFalse otherwise
     */
    virtual OFBool hasCodedEntry(const DSRCodedEntryValue &codedEntryValue) const;

    /** search for a given coded entry in this context group.  If found, the type of the
     *  entry (standard or extension) is returned:
     *  - SR_EC_CodedEntryInStandardContextGroup
     *  - SR_EC_CodedEntryIsExtensionOfContextGroup
     *
     ** @param  codedEntryValue  coded entry to be searched for
     ** @return result of the search process, SR_EC_CodedEntryNotInContextGroup if coded
     *          entry could not be found.  Use OFCondition::good() or OFCondition::bad()
     *          if not interested in the type of the entry.
     */
    virtual OFCondition findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const;

    /** check whether this context group has any extended coded entries.
     *  Extended coded entries are those non-standard codes added by addCodedEntry().
     ** @return OFTrue if there are any extended coded entries, OFFalse otherwise
     */
    virtual OFBool hasExtendedCodedEntries() const;

    /** add a coded entry to this context group as an extension, i.e.\ as a non-standard
     *  code.  Adding a new coded entry by this method only works for extensible context
     *  groups.  Please note that the 'codedEntryValue' is not checked for validity.
     ** @param  codedEntryValue  coded entry to be added to this context group
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition addCodedEntry(const DSRCodedEntryValue &codedEntryValue);

    /** search for a given coded entry in this context group and, if found, replace its
     *  value with the one found in the context group.  This method can, therefore, be
     *  used to lookup the "official" code meaning or to set the "enhanced encoding mode".
     ** @param  codedEntryValue       coded entry to be searched for.  Should be a valid
     *                                code, i.e. at least the three mandatory components
     *                                have to be non-empty. Also used to store the retrieved
     *                                coded entry.
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return result of the search process, see findCodedEntry() for details
     */
    virtual OFCondition lookupCodedEntry(DSRCodedEntryValue &codedEntryValue,
                                         const OFBool enhancedEncodingMode = OFFalse) const;

    /** print contents of this context group, i.e.\ a general header, the currently
     *  selected coded entry value (if any) and all coded entries that are contained
     *  in this context group (both standard and extended ones).
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void print(STD_NAMESPACE ostream &stream) const;

    /** check whether the currently selected coded entry is valid.
     *  Internally, the method hasCodedEntry() is used if 'definedContextGroup' is OFTrue.
     ** @param  definedContextGroup  flag indicating whether this context group is used as
     *                               a baseline (default) or defined context group.  The
     *                               first means that the list of coded entries in this
     *                               context group is just a suggestion.  The latter
     *                               means that one of the coded entries should be used.
     ** @return status, EC_Normal if either no coded entry is selected or the selected one
     *          is both valid and allowed for this context group, an error code otherwise
     */
    virtual OFCondition checkSelectedValue(const OFBool definedContextGroup = OFFalse) const;


  protected:

    /** constructor
     ** @param  contextIdentifier    identifier of the context group
     *  @param  mappingResource      mapping resource that defines the context group
     *  @param  contextGroupVersion  version of the context group (optional)
     *  @param  contextGroupUID      unique identifier of the context group (optional)
     *  @param  selectedValue        coded entry to be selected as the current value
     *                               (optional)
     */
    DSRContextGroup(const OFString &contextIdentifier,
                    const OFString &mappingResource,
                    const OFString &contextGroupVersion = "",
                    const OFString &contextGroupUID = "",
                    const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** destructor
     */
    virtual ~DSRContextGroup();

    /** print header with some general information on this context group
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printHeader(STD_NAMESPACE ostream &stream) const;

    /** print details on coded entries that are contained in this context group.
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printCodes(STD_NAMESPACE ostream &stream) const;

    /** search for a given coded entry in this context group
     ** @param  searchForCodedEntry   coded entry to be searched for
     *  @param  foundCodedEntry       pointer to variable that will store the coded entry
     *                                (if found and pointer is not NULL)
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry
     *                                (not used for this class, only in derived classes)
     ** @return result of the search process, also defines the type of the entry
     */
    virtual OFCondition findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                       DSRCodedEntryValue *foundCodedEntry,
                                       const OFBool enhancedEncodingMode = OFFalse) const;

    /** check whether a given coded entry is valid.
     *  Internally, the method hasCodedEntry() is used if 'definedContextGroup' is OFTrue.
     ** @param  codedEntryValue      coded entry to be checked
     *  @param  definedContextGroup  flag indicating whether this context group is used as
     *                               a baseline (default) or defined context group.  The
     *                               first means that the list of coded entries in this
     *                               context group is just a suggestion.  The latter
     *                               means that one of the coded entries should be used.
     ** @return status, EC_Normal if either no coded entry is selected or the selected one
     *          is both valid and allowed for this context group, an error code otherwise
     */
    virtual OFCondition checkCodedEntry(const DSRCodedEntryValue &codedEntryValue,
                                        const OFBool definedContextGroup) const;


  private:

    /// type used for storing and managing coded entries
    typedef OFList<DSRCodedEntryValue> CodeList;

    /// context identifier (VR=CS, mandatory)
    const OFString Identifier;
    /// mapping resource (VR=CS, mandatory)
    const OFString MappingResource;
    /// context group version (VR=DT, optional)
    const OFString Version;
    /// context group UID (VR=UI, optional)
    const OFString UID;

    /// coded entry selected as the current value (optional, might be empty)
    DSRCodedEntryValue SelectedValue;
    /// mode indicating whether context group is extensible (default: false)
    OFBool ExtensibleMode;
    /// extended coded entries (e.g. private or implementation-specific ones)
    CodeList ExtendedCodes;

 // --- declaration of default constructor and assignment operator

    DSRContextGroup();
    DSRContextGroup &operator=(const DSRContextGroup &);
};


#endif

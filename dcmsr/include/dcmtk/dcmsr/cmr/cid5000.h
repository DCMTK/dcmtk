/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID5000_Languages
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_CID5000_H
#define CMR_CID5000_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 5000 - Languages.
 */
class DCMTK_CMR_EXPORT CID5000_Languages
  : public DSRContextGroup
{

  public:

    /** available languages (in alphabetical order).
     *  There are also variants that include the country.
     *  Please note that this list is everything but complete.
     */
    enum EnumType
    {
        /// (eng,IETF4646,"English")
        English,
        /// (en-CA,IETF4646","English (CA)")
        English_CA,
        /// (en-GB,IETF4646","English (GB)")
        English_GB,
        /// (en-US,IETF4646","English (US)")
        English_US,
        /// (fra,IETF4646","French")
        French,
        /// (fr-CA,IETF4646","French (CA)")
        French_CA,
        /// (fr-CH,IETF4646","French (CH)")
        French_CH,
        /// (fr-FR,IETF4646","French (FR)")
        French_FR,
        /// (deu,IETF4646","German")
        German,
        /// (de-AT,IETF4646","German (AT)")
        German_AT,
        /// (de-CH,IETF4646","German (CH)")
        German_CH,
        /// (de-DE,IETF4646","German (DE)")
        German_DE
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. should be based on RFC 4646.
     */
    CID5000_Languages(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         language mapped to a coded entry that is selected as
     *                                the current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID5000_Languages(const EnumType selectedValue,
                      const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value
     ** @param  selectedValue         language mapped to a coded entry that is selected as
     *                                the current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const EnumType selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** initialize this context group explicitly.  Internally, the list of standard coded
     *  entries is created and initialized by calling getCodes().
     ** @note This function can be called multiple times but in case of multi-threaded
     *        applications should be called at least once before any instance of this class
     *        is actually used.  For single-threaded applications, there is no need to call
     *        it since the initialization is done implicitly.
     */
    static void initialize();

    /** cleanup this context group, i.e.\ delete the internal list of standard coded entries.
     *  Usually, there is no need to call this method, but it might be useful in order to
     *  explicitly free the associated memory, e.g. when checking for memory leaks.  The list
     *  will be recreated automatically when needed (or when initialize() is called).
     */
    static void cleanup();

    /** map a given language to the associated coded entry
     ** @param  value                 language that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given type
     */
    static DSRCodedEntryValue getCodedEntry(const EnumType value,
                                            const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce methods from base class

     using DSRContextGroup::selectValue;
     using DSRContextGroup::findCodedEntry;


  protected:

    /// type used for storing and managing the coded entries
    typedef OFMap<EnumType, DSRBasicCodedEntry> CodeList;

    /** print details on coded entries that are contained in this context group.
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printCodes(STD_NAMESPACE ostream &stream) const;

    /** search for a given coded entry in this context group
     ** @param  searchForCodedEntry   coded entry to be searched for
     *  @param  foundCodedEntry       pointer to variable that will store the coded entry
     *                                (if found and pointer is not NULL)
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return result of the search process, also defines the type of the entry
     */
    virtual OFCondition findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                       DSRCodedEntryValue *foundCodedEntry,
                                       const OFBool enhancedEncodingMode = OFFalse) const;

  // --- static helper function ---

    /** get list of standard coded entries that are managed by this context group.
     *  Internally, the singleton pattern is used, so the list is initialized only once.
     *  Please note that this function is not thread-safe.  Therefore, the initialize()
     *  function can be called before any instance of this class is actually used.
     ** @return reference to list of coded entries managed by this context group
     */
    static CodeList &getCodes();

    /** set the "Enhanced Encoding Mode" for a given coded entry
     ** @param  codedEntryValue  coded entry for which the enhanced encoding mode is set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue);


  private:

    /// pointer to internal code list (use a static variable for singleton pattern)
    static CodeList *Codes;
};

#endif

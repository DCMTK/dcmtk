/*
 *
 *  Copyright (C) 2015-2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID11_RouteOfAdministration
 *
 *  Generated automatically from DICOM PS 3.16-2022b
 *  File created on 2022-04-12 13:01:08 by J. Riesmeier
 *
 */


#ifndef CMR_CID11_H
#define CMR_CID11_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 11 - Route of Administration.
 *  (type: extensible, version: 20200117)
 */
class DCMTK_CMR_EXPORT CID11_RouteOfAdministration
  : public DSRContextGroup
{

  public:

    /** types of route of administration
     */
    enum EnumType
    {
        /// (47625008,SCT,"Intravenous route")
        IntravenousRoute,
        /// (58100008,SCT,"Intra-arterial route")
        IntraArterialRoute,
        /// (78421000,SCT,"Intramuscular route")
        IntramuscularRoute,
        /// (34206005,SCT,"Subcutaneous route")
        SubcutaneousRoute,
        /// (372464004,SCT,"Intracutaneous route")
        IntracutaneousRoute,
        /// (38239002,SCT,"Intraperitoneal route")
        IntraperitonealRoute,
        /// (60213007,SCT,"Intramedullary route")
        IntramedullaryRoute,
        /// (72607000,SCT,"Intrathecal route")
        IntrathecalRoute,
        /// (12130007,SCT,"Intra-articular route")
        IntraArticularRoute,
        /// (C38244,NCIt,"Intraepithelial route")
        IntraepithelialRoute,
        /// (6064005,SCT,"Topical route")
        TopicalRoute,
        /// (26643006,SCT,"Oral route")
        OralRoute,
        /// (C38306,NCIt,"Transluminal route")
        TransluminalRoute,
        /// (37737002,SCT,"Intraluminal route")
        IntraluminalRoute,
        /// (C38213,NCIt,"Extraluminal route")
        ExtraluminalRoute,
        /// (446406008,SCT,"By inhalation")
        ByInhalation,
        /// (37161004,SCT,"Per rectum")
        PerRectum,
        /// (16857009,SCT,"Vaginal route")
        VaginalRoute,
        /// (372463005,SCT,"Intracoronary route")
        IntracoronaryRoute,
        /// (372460008,SCT,"Intracardiac route")
        IntracardiacRoute,
        /// (420287000,SCT,"Intraventricular route - cardiac")
        IntraventricularRouteCardiac,
        /// (127070,DCM,"Retro-orbital route")
        RetroOrbitalRoute,
        /// (46713006,SCT,"Nasal route")
        NasalRoute,
        /// (372464004,SCT,"Intradermal route")
        IntradermalRoute,
        /// (447122006,SCT,"Intratumor route")
        IntratumorRoute,
        /// (445769006,SCT,"Intracorpus cavernosum route")
        IntracorpusCavernosumRoute
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID11_RouteOfAdministration(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID11_RouteOfAdministration(const EnumType selectedValue,
                                const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
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

    /** map a given type to the associated coded entry
     ** @param  value                 type that should be mapped to a coded entry
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


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID11_RouteOfAdministration CMR_CID11;


#endif

/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *    classes: CID11_RouteOfAdministration
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-08-10 16:46:51 by J. Riesmeier
 *
 */


#ifndef CMR_CID11_H
#define CMR_CID11_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 11 - Route of Administration.
 *  (type: extensible, version: 20100608)
 */
class DCMTK_CMR_EXPORT CID11_RouteOfAdministration
  : public DSRContextGroup
{

  public:

    /** types of route of administration
     */
    enum EnumType
    {
        /// (G-D101,SRT,"Intravenous route")
        IntravenousRoute,
        /// (G-D102,SRT,"Intra-arterial route")
        IntraArterialRoute,
        /// (G-D103,SRT,"Intramuscular route")
        IntramuscularRoute,
        /// (G-D104,SRT,"Subcutaneous route")
        SubcutaneousRoute,
        /// (G-D105,SRT,"Intracutaneous route")
        IntracutaneousRoute,
        /// (G-D106,SRT,"Intraperitoneal route")
        IntraperitonealRoute,
        /// (G-D107,SRT,"Intramedullary route")
        IntramedullaryRoute,
        /// (G-D108,SRT,"Intrathecal route")
        IntrathecalRoute,
        /// (G-D109,SRT,"Intra-articular route")
        IntraArticularRoute,
        /// (G-D111,SRT,"Intraepithelial route")
        IntraepithelialRoute,
        /// (G-D112,SRT,"Topical route")
        TopicalRoute,
        /// (G-D140,SRT,"Oral route")
        OralRoute,
        /// (G-D142,SRT,"Transluminal route")
        TransluminalRoute,
        /// (G-D144,SRT,"Intraluminal route")
        IntraluminalRoute,
        /// (G-D146,SRT,"Extraluminal route")
        ExtraluminalRoute,
        /// (G-D150,SRT,"By inhalation")
        ByInhalation,
        /// (G-D160,SRT,"Per rectum")
        PerRectum,
        /// (G-D164,SRT,"Vaginal route")
        VaginalRoute,
        /// (G-D17C,SRT,"Intracoronary route")
        IntracoronaryRoute,
        /// (G-D173,SRT,"Intracardiac route")
        IntracardiacRoute,
        /// (R-F2C86,SRT,"Intraventricular route - cardiac")
        IntraventricularRouteCardiac
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

    /** search for a given coded entry in this context group.  If found, the type of the
     *  entry (standard or extension) is returned.
     *  See DSRContextGroup::findCodedEntry() for details.
     ** @param  codedEntryValue  coded entry to be searched for
     ** @return result of the search process.  Use OFCondition::good() or OFCondition::bad()
     *          to check whether the coded entry has been found or not.
     */
   virtual OFCondition findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const;

  // --- static helper functions ---

    /** initialize this context group explicitly.  Internally, the list of standard coded
     *  entries is created and initialized by calling getCodes().
     ** @note This function can be called multiple times but in case of multi-threaded
     *        applications should be called at least once before any instance of this class
     *        is actually used.  For single-threaded applications, there is no need to call
     *        it since the initialization is done implicitly.
     */
    static void initialize();

    /** map a given type to the associated coded entry
     ** @param  value                 type that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given type
     */
    static DSRCodedEntryValue getCodedEntry(const EnumType value,
                                            const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

     using DSRContextGroup::selectValue;


  protected:

    /// type used for storing and managing the coded entries
    typedef OFMap<EnumType, DSRBasicCodedEntry> CodeList;

    /** print details on coded entries that are contained in this context group.
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printCodes(STD_NAMESPACE ostream &stream) const;

  // --- static helper function ---

    /** get list of standard coded entries that are managed by this context group.
     *  Internally, the singleton pattern is used, so the list is initialized only once.
     *  Please note that this function is not thread-safe.  Therefore, the initialize()
     *  function can be called before any instance of this class is actually used.
     ** @return reference to list of coded entries managed by this context group
     */
    static CodeList &getCodes();
};

#endif

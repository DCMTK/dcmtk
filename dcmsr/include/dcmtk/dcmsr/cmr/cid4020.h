/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID4020_PETRadionuclide
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:22 by J. Riesmeier
 *
 */


#ifndef CMR_CID4020_H
#define CMR_CID4020_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 4020 - PET Radionuclide.
 *  (type: extensible, version: 20160119)
 */
class DCMTK_CMR_EXPORT CID4020_PETRadionuclide
  : public DSRContextGroup
{

  public:

    /** types of PET radionuclide
     */
    enum EnumType
    {
        /// (C-105A1,SRT,"^11^Carbon")
        _11_Carbon,
        /// (C-107A1,SRT,"^13^Nitrogen")
        _13_Nitrogen,
        /// (C-1018C,SRT,"^14^Oxygen")
        _14_Oxygen,
        /// (C-B1038,SRT,"^15^Oxygen")
        _15_Oxygen,
        /// (C-111A1,SRT,"^18^Fluorine")
        _18_Fluorine,
        /// (C-155A1,SRT,"^22^Sodium")
        _22_Sodium,
        /// (C-135A4,SRT,"^38^Potassium")
        _38_Potassium,
        /// (126605,DCM,"^43^Scandium")
        _43_Scandium,
        /// (126600,DCM,"^44^Scandium")
        _44_Scandium,
        /// (C-166A2,SRT,"^45^Titanium")
        _45_Titanium,
        /// (126601,DCM,"^51^Manganese")
        _51_Manganese,
        /// (C-130A1,SRT,"^52^Iron")
        _52_Iron,
        /// (C-149A1,SRT,"^52^Manganese")
        _52_Manganese,
        /// (126607,DCM,"^52m^Manganese")
        _52m_Manganese,
        /// (C-127A4,SRT,"^60^Copper")
        _60_Copper,
        /// (C-127A1,SRT,"^61^Copper")
        _61_Copper,
        /// (C-127A5,SRT,"^62^Copper")
        _62_Copper,
        /// (C-141A1,SRT,"^62^Zinc")
        _62_Zinc,
        /// (C-127A2,SRT,"^64^Copper")
        _64_Copper,
        /// (C-131A1,SRT,"^66^Gallium")
        _66_Gallium,
        /// (C-131A3,SRT,"^68^Gallium")
        _68_Gallium,
        /// (C-128A2,SRT,"^68^Germanium")
        _68_Germanium,
        /// (126602,DCM,"^70^Arsenic")
        _70_Arsenic,
        /// (C-115A2,SRT,"^72^Arsenic")
        _72_Arsenic,
        /// (C-116A2,SRT,"^73^Selenium")
        _73_Selenium,
        /// (C-113A1,SRT,"^75^Bromine")
        _75_Bromine,
        /// (C-113A2,SRT,"^76^Bromine")
        _76_Bromine,
        /// (C-113A3,SRT,"^77^Bromine")
        _77_Bromine,
        /// (C-159A2,SRT,"^82^Rubidium")
        _82_Rubidium,
        /// (C-162A3,SRT,"^86^Yttrium")
        _86_Yttrium,
        /// (C-168A4,SRT,"^89^Zirconium")
        _89_Zirconium,
        /// (126603,DCM,"^90^Niobium")
        _90_Niobium,
        /// (C-162A7,SRT,"^90^Yttrium")
        _90_Yttrium,
        /// (C-163AA,SRT,"^94m^Technetium")
        _94m_Technetium,
        /// (C-114A5,SRT,"^124^Iodine")
        _124_Iodine,
        /// (126606,DCM,"^152^Terbium")
        _152_Terbium
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID4020_PETRadionuclide(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID4020_PETRadionuclide(const EnumType selectedValue,
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
typedef CID4020_PETRadionuclide CMR_CID4020;


#endif

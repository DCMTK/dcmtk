/*
 *
 *  Copyright (C) 2015-2021, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID7452_OrganizationalRoles
 *
 *  Generated automatically from DICOM PS 3.16-2021a
 *  File created on 2021-02-03 21:06:29 by J. Riesmeier
 *
 */


#ifndef CMR_CID7452_H
#define CMR_CID7452_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 7452 - Organizational Roles.
 *  (type: extensible, version: 20170626)
 */
class DCMTK_CMR_EXPORT CID7452_OrganizationalRoles
  : public DSRContextGroup
{

  public:

    /** types of organizational roles
     */
    enum EnumType
    {
        /// (158965000,SCT,"Medical Practitioner")
        MedicalPractitioner,
        /// (309343006,SCT,"Physician")
        Physician,
        /// (128670,DCM,"Head of Radiology")
        HeadOfRadiology,
        /// (128671,DCM,"Chair of Protocol Committee")
        ChairOfProtocolCommittee,
        /// (128676,DCM,"Representative of Protocol Committee")
        RepresentativeOfProtocolCommittee,
        /// (128677,DCM,"Representative of Ethics Committee")
        RepresentativeOfEthicsCommittee,
        /// (128675,DCM,"Head of Cardiology")
        HeadOfCardiology,
        /// (128673,DCM,"Administrator of Radiology Department")
        AdministratorOfRadiologyDepartment,
        /// (106292003,SCT,"Nurse")
        Nurse,
        /// (159016003,SCT,"Radiologic Technologist")
        RadiologicTechnologist,
        /// (128674,DCM,"Lead Radiologic Technologist")
        LeadRadiologicTechnologist,
        /// (3430008,SCT,"Radiation Therapist")
        RadiationTherapist,
        /// (159016003,SCT,"Radiographer")
        Radiographer,
        /// (C1144859,UMLS,"Intern")
        Intern,
        /// (405277009,SCT,"Resident")
        Resident,
        /// (158971006,SCT,"Registrar")
        Registrar,
        /// (121088,DCM,"Fellow")
        Fellow,
        /// (405279007,SCT,"Attending")
        Attending,
        /// (309390008,SCT,"Consultant")
        Consultant,
        /// (C1441532,UMLS,"Consulting Physician")
        ConsultingPhysician,
        /// (415506007,SCT,"Scrub nurse")
        ScrubNurse,
        /// (304292004,SCT,"Surgeon")
        Surgeon,
        /// (121092,DCM,"Sonologist")
        Sonologist,
        /// (C1954848,UMLS,"Sonographer")
        Sonographer,
        /// (C2985483,UMLS,"Radiation Physicist")
        RadiationPhysicist,
        /// (C1708969,UMLS,"Medical Physicist")
        MedicalPhysicist
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID7452_OrganizationalRoles(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID7452_OrganizationalRoles(const EnumType selectedValue,
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
typedef CID7452_OrganizationalRoles CMR_CID7452;


#endif

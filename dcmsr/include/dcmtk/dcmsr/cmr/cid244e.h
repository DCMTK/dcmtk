/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID244e_Laterality
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_CID244E_H
#define CMR_CID244E_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid244.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** DCMR Context Group with enhanced functionality:
 *  CID 244 - Laterality.
 */
class DCMTK_CMR_EXPORT CID244e_Laterality
  : public CID244_Laterality
{

  public:

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID244e_Laterality(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID244e_Laterality(const EnumType selectedValue,
                       const OFBool enhancedEncodingMode = OFFalse);

    /** constructor
     ** @param  selectedValue         enumerated value for Image Laterality (0020,0062) mapped
     *                                to a coded entry that is selected as the current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID244e_Laterality(const OFString &selectedValue,
                       const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its associated enumerated value as the current value
     ** @param  selectedValue         enumerated value for Image Laterality (0020,0062) mapped
     *                                to a coded entry that is selected as the current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const OFString &selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** map an enumerated value for Image Laterality (0020,0062) to the associated coded entry
     ** @param  enumeratedValue       enumerated value that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given enumerated value, or an empty
     *          value if none was found
     */
    static DSRCodedEntryValue mapImageLaterality(const OFString &enumeratedValue,
                                                 const OFBool enhancedEncodingMode = OFFalse);

    /** map an enumerated value for Image Laterality (0020,0062) to the associated coded entry
     ** @param  enumeratedValue       enumerated value that should be mapped to a coded entry
     *  @param  codedEntryValue       reference to variable where the resulting coded entry is
     *                                stored.  This variable is not cleared in case of error.
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition mapImageLaterality(const OFString &enumeratedValue,
                                          DSRCodedEntryValue &codedEntryValue,
                                          const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

    using CID244_Laterality::selectValue;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID244e_Laterality CMR_CID244e;


#endif

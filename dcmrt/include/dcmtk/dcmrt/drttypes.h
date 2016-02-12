/*
 *
 *  Copyright (c) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTTypes
 *
 *  Generated manually based on dsrtypes.h
 *  File created on 2008-12-05
 *  Last modified on 2016-02-12 by Riesmeier
 *
 */


#ifndef DRTTYPES_H
#define DRTTYPES_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/oflog/oflog.h"

#include "dcmtk/dcmdata/dctk.h"

#include "dcmtk/ofstd/ofdefine.h"


#ifdef dcmrt_EXPORTS
#define DCMTK_DCMRT_EXPORT DCMTK_DECL_EXPORT
#else
#define DCMTK_DCMRT_EXPORT DCMTK_DECL_IMPORT
#endif


// include this file in doxygen documentation

/** @file drttypes.h
 *  @brief type definitions, constants and helper functions for the dcmrt module
 */


/*-----------------------*
 *  contant definitions  *
 *-----------------------*/

/** @name specific error conditions for module dcmrt.
 *  These error codes can be used in addition to the general purpose
 *  codes defined in module dcmdata.
 */
//@{

/// error: a value is invalid according to the standard
extern DCMTK_DCMRT_EXPORT const OFConditionConst RT_EC_InvalidValue;

/// error: a value is not supported by this implementation
extern DCMTK_DCMRT_EXPORT const OFConditionConst RT_EC_UnsupportedValue;

/// error: the object is invalid, see isValid() method in IOD class
extern DCMTK_DCMRT_EXPORT const OFConditionConst RT_EC_InvalidObject;

//@}


// global definitions for logging mechanism provided by the oflog module

extern DCMTK_DCMRT_EXPORT OFLogger DCM_dcmrtLogger;

#define DCMRT_TRACE(msg) OFLOG_TRACE(DCM_dcmrtLogger, msg)
#define DCMRT_DEBUG(msg) OFLOG_DEBUG(DCM_dcmrtLogger, msg)
#define DCMRT_INFO(msg)  OFLOG_INFO(DCM_dcmrtLogger, msg)
#define DCMRT_WARN(msg)  OFLOG_WARN(DCM_dcmrtLogger, msg)
#define DCMRT_ERROR(msg) OFLOG_ERROR(DCM_dcmrtLogger, msg)
#define DCMRT_FATAL(msg) OFLOG_FATAL(DCM_dcmrtLogger, msg)


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** General purpose class hiding global functions, constants and types from the
 *  global namespace.  Some of them might be moved to 'ofstd' or 'dcmdata' later on.
 *  All functions and constants are static and can, therefore, be accessed without
 *  creating an instance of this class.
 */
class DCMTK_DCMRT_EXPORT DRTTypes
{

  public:

  // --- destructor ---

    /** destructor. (only needed to avoid compiler warnings)
     */
    virtual ~DRTTypes();


  // --- DICOM data structure access functions ---

    /** add given element to the dataset.
     *  The element is only added if 'result' is EC_Normal and the 'element' pointer is not NULL.
     *  @param  result      reference to status variable (checked before adding and updated afterwards!)
     *  @param  dataset     reference to DICOM dataset to which the element should be added
     *  @param  element     pointer to DICOM element which should be added. deleted if not inserted.
     *  @param  vm          value multiplicity (according to the data dictionary) to be checked for.
     *                      (See DcmElement::checkVM() for a list of valid values.)
     *                      Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type        value type (valid value: "1", "2" or something else which is not checked)
     *  @param  moduleName  optional module name to be printed (default: "RT object" if NULL)
     *  @return current value of 'result', EC_Normal if successful, an error code otherwise
     */
    static OFCondition addElementToDataset(OFCondition &result,
                                           DcmItem &dataset,
                                           DcmElement *element,
                                           const OFString &vm,
                                           const OFString &type,
                                           const char *moduleName = NULL);

    /** get element from dataset
     *  @param  dataset  reference to DICOM dataset from which the element should be retrieved.
     *                   (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  element  reference to DICOM element which should be retrieved.  The return value
     *                   is also stored in this parameter.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition getElementFromDataset(DcmItem &dataset,
                                             DcmElement &element);

    /** get string value from element
     *  @param  element      reference to DICOM element from which the string value should be retrieved
     *  @param  stringValue  reference to character string where the result should be stored
     *  @param  pos          index of the value to get (0..vm-1), -1 for all components
     *  @return reference character string if successful, empty string otherwise
     */
    static OFCondition getStringValueFromElement(const DcmElement &element,
                                                 OFString &stringValue,
                                                 const signed long pos = 0);

    /** get string value from dataset
     *  @param  dataset      reference to DICOM dataset from which the string should be retrieved.
     *                       (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  tagKey       DICOM tag specifying the attribute from which the string should be retrieved
     *  @param  stringValue  reference to character string in which the result should be stored.
     *                       (This parameter is automatically cleared if the tag could not be found.)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition getStringValueFromDataset(DcmItem &dataset,
                                                 const DcmTagKey &tagKey,
                                                 OFString &stringValue);

    /** put string value to dataset
     *  @param  dataset      reference to DICOM dataset to which the string should be put.
     *  @param  tag          DICOM tag specifying the attribute to which the string should be put
     *  @param  stringValue  character string specifying the value to be set
     *  @param  allowEmpty   allow empty element to be inserted if OFTrue.
     *                       Do not insert empty element otherwise.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition putStringValueToDataset(DcmItem &dataset,
                                               const DcmTag &tag,
                                               const OFString &stringValue,
                                               const OFBool allowEmpty = OFTrue);

    /** check element value for correct value multiplicity and type
     *  @param  element     DICOM element to be checked
     *  @param  vm          value multiplicity (according to the data dictionary) to be checked for.
     *                      (See DcmElement::checkVM() for a list of valid values.)
     *                      Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type        value type (valid value: "1", "1C", "2" or something else which is not checked)
     *  @param  searchCond  optional flag indicating the status of a previous 'search' function call
     *  @param  moduleName  optional module name to be printed (default: "RT object" if NULL)
     *  @return OFTrue if element value is correct, OFFalse otherwise
     */
    static OFBool checkElementValue(DcmElement &element,
                                    const OFString &vm,
                                    const OFString &type,
                                    const OFCondition &searchCond = EC_Normal,
                                    const char *moduleName = NULL);

    /** get element from dataset and check it for correct value multiplicity and type.
     *  This functions calls the above one to check the element value.
     *  @param  dataset     reference to DICOM dataset from which the element should be retrieved.
     *                      (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  element     DICOM element used to store the value
     *  @param  vm          value multiplicity (according to the data dictionary) to be checked for.
     *                      (See DcmElement::checkVM() for a list of valid values.)
     *                      Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type        value type (valid value: "1", "1C", "2" or something else which is not checked)
     *  @param  moduleName  optional module name to be printed (default: "RT object" if NULL)
     *  @return status, EC_Normal if element could be retrieved and value is correct, an error code otherwise
     */
    static OFCondition getAndCheckElementFromDataset(DcmItem &dataset,
                                                     DcmElement &element,
                                                     const OFString &vm,
                                                     const OFString &type,
                                                     const char *moduleName = NULL);

    /** get string value from dataset and check it for correct value multiplicity and type.
     *  This functions calls the above one to check the element value.
     *  @param  dataset      reference to DICOM dataset from which the element should be retrieved.
     *                       (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  tagKey       DICOM tag specifying the attribute from which the string should be retrieved
     *  @param  stringValue  reference to character string in which the result should be stored.
     *                       (This parameter is automatically cleared if the tag could not be found.)
     *  @param  vm           value multiplicity (according to the data dictionary) to be checked for.
     *                       (See DcmElement::checkVM() for a list of valid values.)
     *                       Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type         value type (valid value: "1", "1C", "2" or something else which is not checked)
     *  @param  moduleName   optional module name to be printed (default: "RT object" if NULL)
     *  @return status, EC_Normal if element could be retrieved and value is correct, an error code otherwise
     */
    static OFCondition getAndCheckStringValueFromDataset(DcmItem &dataset,
                                                         const DcmTagKey &tagKey,
                                                         OFString &stringValue,
                                                         const OFString &vm,
                                                         const OFString &type,
                                                         const char *moduleName = NULL);
};


#endif

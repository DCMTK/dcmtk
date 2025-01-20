/*
 *
 *  Copyright (C) 2015-2025, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class TID1500_MeasurementReport
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_TID1500_H
#define CMR_TID1500_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrrtpl.h"
#include "dcmtk/dcmsr/dsrstpl.h"

#include "dcmtk/dcmsr/cmr/define.h"
#include "dcmtk/dcmsr/cmr/tid1001.h"
#include "dcmtk/dcmsr/cmr/tid1204.h"
#include "dcmtk/dcmsr/cmr/tid1411.h"
#include "dcmtk/dcmsr/cmr/tid1501.h"
#include "dcmtk/dcmsr/cmr/tid1600.h"
#include "dcmtk/dcmsr/cmr/cid100.h"
#include "dcmtk/dcmsr/cmr/cid218e.h"
#include "dcmtk/dcmsr/cmr/cid6147.h"
#include "dcmtk/dcmsr/cmr/cid7021.h"
#include "dcmtk/dcmsr/cmr/cid7181.h"
#include "dcmtk/dcmsr/cmr/cid7464.h"
#include "dcmtk/dcmsr/cmr/cid7551.h"


// include this file in doxygen documentation

/** @file tid1500.h
 *  @brief Interface class for TID 1500 in module dcmsr/cmr
 */


/*------------------------*
 *  constant definitions  *
 *------------------------*/

/** @name specific error conditions for TID 1500 (and included templates) in module dcmsr/cmr
 */
///@{

/// error: there is no measurement report to add content items to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoMeasurementReport;
/// error: there is no measurement group to add entries to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoMeasurementGroup;
/// error: the given segmentation object does not conform to the template constraints
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_InvalidSegmentationObject;
/// error: the given DICOM object is not a real world value mapping object
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_InvalidRealWorldValueMappingObject;

///@}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 1500 - Measurement Report (and included templates 1204, 1001, 1600, 1411, 1501).
 *  All added content items are annotated with a text in the format "TID 1500 - Row [n]".
 ** @note Please note that currently only the mandatory and some optional/conditional
 *        content items and included templates are supported.
 */
class DCMTK_CMR_EXPORT TID1500_MeasurementReport
  : public DSRRootTemplate
{

  public:

    // type definitions
    typedef TID1411_VolumetricROIMeasurements<CID218e_QuantitativeImageFeature,
                                              CID7181_AbstractMultiDimensionalImageModelComponentUnit,
                                              CID6147_ResponseCriteria,
                                              CID7464_GeneralRegionOfInterestMeasurementModifier>
            TID1411_Measurements;

    typedef TID1501_MeasurementGroup<CID218e_QuantitativeImageFeature,
                                     CID7181_AbstractMultiDimensionalImageModelComponentUnit,
                                     CID6147_ResponseCriteria,
                                     CID7464_GeneralRegionOfInterestMeasurementModifier,
                                     CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurement>
            TID1501_Measurements;

    /** (default) constructor.
     *  Also creates an initial, almost empty measurement report by calling
     *  createNewMeasurementReport(), but only if a non-empty 'title' is passed.
     ** @param  title  optional document title to be set (from CID 7021 - Measurement
     *                 Report Document Title), i.e.\ the concept name of the root node
     *  @param  check  if enabled, check value for validity before setting it
     */
    TID1500_MeasurementReport(const CID7021_MeasurementReportDocumentTitle &title = CID7021_MeasurementReportDocumentTitle(),
                              const OFBool check = OFTrue);

    /** clear internal member variables.
     *  Also see notes on the clear() method of the base class.
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  That means, check whether the base class is valid, the mandatory included
     *  template TID 1001 is valid, and whether hasImagingMeasurements() or
     *  hasQualitativeEvaluations() return true.  In addition, each of the included
     *  templates TID 1204, 1411, 1501 and 1600 should either be empty or valid.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether there are any 'Procedure reported' content items (TID 1500 - Row 4)
     *  in this measurement report.  This content item is optional, i.e. might be absent.
     ** @return OFTrue if at least one procedure reported is present, OFFalse otherwise
     */
    OFBool hasProcedureReported() const;

    /** check whether there is an included 'Image Library' template (TID 1500 - Row 8)
     *  in this measurement report.  Initially, the optional sub-template TID 1600 is
     *  created (without content items) and included by the constructor of this class.
     *  After clear() has been called or no document title is passed to the constructor,
     *  it can be created again by calling createNewMeasurementReport().
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective sub-template has
     *                         any content (child nodes).  By default, the presence of
     *                         the "included template" content item is checked only.
     ** @return OFTrue if image library is present, OFFalse otherwise
     */
    OFBool hasImageLibrary(const OFBool checkChildren = OFFalse) const;

    /** check whether there is an 'Imaging Measurements' content item (TID 1500 - Row 6)
     *  in this measurement report.  Initially, this conditional content item is created
     *  by the constructor of this class.  After clear() has been called or no document
     *  title is passed to the constructor, it can be created again by calling
     *  createNewMeasurementReport().
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective sub-template has
     *                         any content (child nodes).  By default, the presence of
     *                         the "included template" content item is checked only.
     ** @return OFTrue if imaging measurements are present, OFFalse otherwise
     */
    OFBool hasImagingMeasurements(const OFBool checkChildren = OFFalse) const;

    /** check whether there is an included 'Volumetric ROI Measurements' template
     *  (TID 1500 - Row 8) in this measurement report.  Initially, the optional
     *  sub-template TID 1511 is created and included by the constructor of this class
     *  After clear() has been called or no document title is passed to the constructor,
     *  it can be created again by calling createNewMeasurementReport().
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective sub-template has
     *                         any content (child nodes).  By default, the presence of
     *                         the "included template" content item is checked only.
     ** @return OFTrue if volumetric ROI measurements are present, OFFalse otherwise
     */
    OFBool hasVolumetricROIMeasurements(const OFBool checkChildren = OFFalse) const;

    /** check whether there is an included 'Measurement Group' template (TID 1500 -
     *  Row 9) in this measurement report.  Initially, the optional sub-template TID
     *  1501 is created and included by the constructor of this class.  After clear() has
     *  been called or no document title is passed to the constructor, it can be created
     *  again by calling createNewMeasurementReport().
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective sub-template has
     *                         any content (child nodes).  By default, the presence of
     *                         the "included template" content item is checked only.
     ** @return OFTrue if volumetric ROI measurements are present, OFFalse otherwise
     */
    OFBool hasIndividualMeasurements(const OFBool checkChildren = OFFalse) const;

    /** check whether there is an 'Qualitative Evaluations' content item (TID 1500 -
     *  Row 12) in this measurement report
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective content item has
     *                         child nodes.  By default, the presence of the higher-level
     *                         CONTAINER is checked only.
     ** @return OFTrue if qualitative evaluations are present, OFFalse otherwise
     */
    OFBool hasQualitativeEvaluations(const OFBool checkChildren = OFFalse) const;

    /** get language of this report as defined by TID 1204 (Language of Content Item and
     *  Descendants).  This included template is optional, i.e. might be empty and,
     *  therefore, does not contribute any content items to the tree.
     ** @return reference to internally managed SR template
     */
    inline TID1204_LanguageOfContentItemAndDescendants &getLanguage() const
    {
        return *OFstatic_cast(TID1204_LanguageOfContentItemAndDescendants *, Language.get());
    }

    /** get observation context of this report as defined by TID 1001 (Observation
     *  Context).  This included template is mandatory, i.e. should not be empty.
     ** @return reference to internally managed SR template
     */
    inline TID1001_ObservationContext &getObservationContext() const
    {
        return *OFstatic_cast(TID1001_ObservationContext *, ObservationContext.get());
    }

    /** get image library of this report as defined by TID 1600 (Image Library).
     *  This included template is optional, i.e. might be empty and, therefore, does not
     *  contribute any content items to the tree.
     ** @return reference to internally managed SR template
     */
    inline TID1600_ImageLibrary &getImageLibrary() const
    {
        return *OFstatic_cast(TID1600_ImageLibrary *, ImageLibrary.get());
    }

    /** get volumetric ROI measurements of this report as defined by TID 1411 (Volumetric
     *  ROI Measurements), i.e.\ the current instance of TID 1500 - Row 8.
     *  This included template is optional, i.e. might be empty and, therefore, does not
     *  add any content items to the tree.
     *  Further instances can be added by calling addVolumetricROIMeasurements().
     ** @return reference to internally managed SR template (current instance)
     */
    inline TID1411_Measurements &getVolumetricROIMeasurements() const
    {
        return *OFstatic_cast(TID1411_Measurements *, VolumetricROIMeasurements.get());
    }

    /** get individual measurements of this report as defined by TID 1501 (Measurement
     *  Group), i.e.\ the current instance of TID 1500 - Row 9.
     *  This included template is optional, i.e. might be empty and, therefore, does not
     *  contribute any content items to the tree.
     *  Further instances can be added by calling addIndividualMeasurements().
     ** @return reference to internally managed SR template (current instance)
     */
    inline TID1501_Measurements &getIndividualMeasurements() const
    {
        return *OFstatic_cast(TID1501_Measurements *, MeasurementGroup.get());
    }

    /** get document title of this report, i.e.\ the concept name of the root node
     ** @param  titleCode  coded entry that specifies the document title of this report
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getDocumentTitle(DSRCodedEntryValue &titleCode);

    /** create a new measurement report.
     *  Clear the report and create the mandatory and other supported content items of
     *  this template, i.e.\ TID 1500 - Row 1 to 6 and 8 to 9.
     ** @param  title  document title to be set (from CID 7021 - Measurement Report
     *                 Document Title), i.e.\ the concept name of the root node
     *  @param  check  if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createNewMeasurementReport(const CID7021_MeasurementReportDocumentTitle &title,
                                           const OFBool check = OFTrue);

    /** set language of this report as defined by TID 1204 (Language of Content Item and
     *  Descendants)
     ** @param  language  language of the report, being a language that is primarily
     *                    used for human communication (from CID 5000 - Language)
     *  @param  country   coded entry that describes the country-specific variant of
     *                    'language' (optional, from CID 5001 - Country)
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setLanguage(const CID5000_Language &language,
                            const CID5001_Country &country = CID5001_Country(),
                            const OFBool check = OFTrue);

    /** add the imaging procedure whose results are reported (TID 1500 - Row 4).
     *  There should be at least a single instance of the associated content item.
     ** @param  procedure  coded entry that describes the imaging procedure to be added
     *                     (from CID 100 - Quantitative Diagnostic Imaging Procedure)
     *  @param  check      if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addProcedureReported(const CID100_QuantitativeDiagnosticImagingProcedure &procedure,
                                     const OFBool check = OFTrue);

    /** create another instance of TID 1411 (Volumetric ROI Measurements) and add it as
     *  an included template to this report (TID 1500 - Row 8).  A first instance of
     *  TID 1411 is created and added by calling createNewMeasurementReport().  Access
     *  to the current instance is available through getVolumetricROIMeasurements().
     ** @param  checkEmpty  by default, it is checked whether the current instance of
     *                      TID 1411 is empty, and thus no new instance is created.
     *                      Setting this parameter to OFFalse disables this check and
     *                      always creates and adds a new instance of this sub-template.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addVolumetricROIMeasurements(const OFBool checkEmpty = OFTrue);

    /** create another instance of TID 1501 (Measurement Group) and add it as an included
     *  template to this report (TID 1500 - Row 9).  A first instance of TID 1501 is
     *  created and added by calling createNewMeasurementReport().  Access to the current
     *  instance is available through getIndividualMeasurements().
     *  Please note that a new instance of TID 1501 is only added if the current one is
     *  not empty!
     ** @param  checkEmpty  by default, it is checked whether the current instance of
     *                      TID 1501 is empty, and thus no new instance is created.
     *                      Setting this parameter to OFFalse disables this check and
     *                      always creates and adds a new instance of this sub-template.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addIndividualMeasurements(const OFBool checkEmpty = OFTrue);

    /** add a qualitative evaluation related to the entire subject of the report as a
     *  coded entry (TID 1500 - Row 13).  The higher-level CONTAINER (Row 12) is created
     *  automatically (if none is present).
     ** @param  conceptName  coded entry to be set as the concept name
     *  @param  codeValue    coded entry to be set as the value of the new content item
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                         const DSRCodedEntryValue &codeValue,
                                         const OFBool check = OFTrue);

    /** add a qualitative evaluation related to the entire subject of the report in
     *  text form (TID 1500 - Row 14).  The higher-level CONTAINER (Row 12) is created
     *  automatically (if none is present).
     ** @param  conceptName  coded entry to be set as the concept name
     *  @param  stringValue  character string to be set as the value of the content item
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                         const OFString &stringValue,
                                         const OFBool check = OFTrue);


  protected:

    /** create the mandatory and other supported content items of this template,
     *  i.e.\ TID 1500 - Row 1 to 6 and 8.  It is expected that the tree is currently
     *  empty.
     ** @param  title  coded entry that specifies the document title to be set
     *  @param  check  if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createMeasurementReport(const CID7021_MeasurementReportDocumentTitle &title,
                                        const OFBool check);

    /** create the 'Qualitative Evaluations' content item (TID 1500 - Row 12) if not
     *  existing yet
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createQualitativeEvaluations();


  private:

    // shared pointer to included template "Language of Content Item and Descendants" (TID 1204)
    DSRSharedSubTemplate Language;
    // shared pointer to included template "Observation Context" (TID 1001)
    DSRSharedSubTemplate ObservationContext;
    // shared pointer to included template "Image Library" (TID 1600)
    DSRSharedSubTemplate ImageLibrary;
    // shared pointer to included template "Volumetric ROI Measurements" (TID 1411)
    DSRSharedSubTemplate VolumetricROIMeasurements;
    // shared pointer to included template "Measurement Group" (TID 1501)
    DSRSharedSubTemplate MeasurementGroup;
};


/*--------------------*
 *  type definitions  *
 *--------------------*/

// define short names for the SR template classes
typedef TID1500_MeasurementReport CMR_TID1500;
typedef TID1500_MeasurementReport::TID1411_Measurements CMR_TID1411_in_TID1500;
typedef TID1500_MeasurementReport::TID1411_Measurements::TID1419_Measurement CMR_TID1419_in_TID1411_in_TID1500;
typedef TID1500_MeasurementReport::TID1501_Measurements CMR_TID1501_in_TID1500;
typedef TID1500_MeasurementReport::TID1501_Measurements::TID300_Measurement CMR_TID300_in_TID1501_in_TID1500;


#endif

/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
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
#include "dcmtk/dcmsr/cmr/tid1600.h"
#include "dcmtk/dcmsr/cmr/cid100.h"
#include "dcmtk/dcmsr/cmr/cid6147.h"
#include "dcmtk/dcmsr/cmr/cid7021.h"
#include "dcmtk/dcmsr/cmr/cid7181.h"
#include "dcmtk/dcmsr/cmr/cid7464.h"
#include "dcmtk/dcmsr/cmr/cid7469.h"


// include this file in doxygen documentation

/** @file tid1500.h
 *  @brief Interface class and error constants for TID 1500 in module dcmsr/cmr
 */


/*------------------------*
 *  constant definitions  *
 *------------------------*/

/** @name specific error conditions for TID 1500 in module dcmsr/cmr
 */
//@{

/// error: there is no measurement report to add content items to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoMeasurementReport;

//@}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 1500 - Measurement Report (and included templates 1204, 1001, 1600, 1411).
 *  All added content items are annotated with a text in the format "TID 1500 - Row [n]".
 ** @note Please note that currently only the mandatory (and some optional/conditional)
 *        content items and included templates are supported.
 */
class DCMTK_CMR_EXPORT TID1500_MeasurementReport
  : public DSRRootTemplate
{

  public:

    // type definition
    typedef TID1411_VolumetricROIMeasurements<CID7469_GenericIntensityAndSizeMeasurements,
                                              CID7181_AbstractMultiDimensionalImageModelComponentUnits,
                                              CID6147_ResponseCriteria,
                                              CID7464_GeneralRegionOfInterestMeasurementModifiers>
            TID1411_Measurements;

    /** (default) constructor.
     *  Also creates an initial, almost empty measurement report by calling
     *  createNewMeasurementReport(), but only if a non-empty 'title' is passed.
     ** @param  title  optional document title to be set (from CID 7021 - Measurement
     *                 Report Document Titles), i.e.\ the concept name of the root node
     */
    TID1500_MeasurementReport(const CID7021_MeasurementReportDocumentTitles &title = CID7021_MeasurementReportDocumentTitles());

    /** clear internal member variables.
     *  Also see notes on the clear() method of the base class.
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  That means, check whether the base class is valid, the mandatory included
     *  templates TID 1204, 1001 and 1600 are valid, and whether hasProcedureReported()
     *  as well as hasImagingMeasurements() or hasQualitativeEvaluations() return true.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether there are any 'Procedure reported' content items (TID 1500 - Row 4)
     *  in this measurement report.  This content item is mandatory, i.e. one or more
     *  instances of the associated content item should be present.
     ** @return OFTrue if at least one procedure reported is present, OFFalse otherwise
     */
    OFBool hasProcedureReported() const;

    /** check whether there is an included 'Volumetric ROI Measurements' template
     *  (TID 1500 - Row 8) in this measurement report.  Initially, this optional
     *  sub-template is created and included by the constructor of this class.  After
     *  clear() has been called or no document title is passed to the constructor, it
     *  can be created again by calling createNewMeasurementReport().
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective sub-template has
     *                         any content (child nodes).  By default, the presence of
     *                         the "included template" content item is checked only.
     ** @return OFTrue if volumetric ROI measurements are present, OFFalse otherwise
     */
    OFBool hasVolumetricROIMeasurements(const OFBool checkChildren = OFFalse) const;

    /** check whether there is an 'Imaging Measurements' content item (TID 1500 - Row 6)
     *  in this measurement report.  Initially, this conditional content item is created
     *  by the constructor of this class.  After clear() has been called or no document
     *  title is passed to the constructor, it can be created again by calling
     *  createNewMeasurementReport().
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective content item has
     *                         child nodes.  By default, the presence of the higher-level
     *                         CONTAINER is checked only.
     ** @return OFTrue if imaging measurements are present, OFFalse otherwise
     */
    OFBool hasImagingMeasurements(const OFBool checkChildren = OFFalse) const;

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
     *  Descendants).  This included template is mandatory, i.e. should not be empty.
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
     *  This included template is mandatory, i.e. should not be empty.
     ** @return reference to internally managed SR template
     */
    inline TID1600_ImageLibrary &getImageLibrary() const
    {
        return *OFstatic_cast(TID1600_ImageLibrary *, ImageLibrary.get());
    }

    /** get volumetric ROI measurements of this report as defined by TID 1411 (Volumetric
     *  ROI Measurements), i.e.\ the current instance of TID 1500 - Row 8.
     *  This included template is optional, i.e. might be empty (but not absent).
     *  Further instances can be added by calling addVolumetricROIMeasurements().
     ** @return reference to internally managed SR template (current instance)
     */
    inline TID1411_Measurements &getVolumetricROIMeasurements() const
    {
        return *OFstatic_cast(TID1411_Measurements *, VolumetricROIMeasurements.get());
    }

    /** get document title of this report, i.e.\ the concept name of the root node
     ** @param  titleCode  coded entry that specifies the document title of this report
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getDocumentTitle(DSRCodedEntryValue &titleCode);

    /** create a new measurement report.
     *  Clear the report and create the mandatory (and other supported) content items of
     *  this template, i.e.\ TID 1500 - Row 1 to 6 and 8.
     ** @param  title  document title to be set (from CID 7021 - Measurement Report
     *                 Document Titles), i.e.\ the concept name of the root node
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createNewMeasurementReport(const CID7021_MeasurementReportDocumentTitles &title);

    /** set language of this report as defined by TID 1204 (Language of Content Item and
     *  Descendants)
     ** @param  language  language of the report, being a language that is primarily
     *                    used for human communication (from CID 5000 - Languages)
     *  @param  country   coded entry that describes the country-specific variant of
     *                    'language' (optional, from CID 5001 - Countries)
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setLanguage(const CID5000_Languages &language,
                            const CID5001_Countries &country = CID5001_Countries(),
                            const OFBool check = OFTrue);

    /** add the imaging procedure whose results are reported (TID 1500 - Row 4).
     *  There should be at least a single instance of the associated content item.
     ** @param  procedure  coded entry that describes the imaging procedure to be added
     *                     (from CID 100 - Quantitative Diagnostic Imaging Procedures)
     *  @param  check      if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addProcedureReported(const CID100_QuantitativeDiagnosticImagingProcedures &procedure,
                                     const OFBool check = OFTrue);

    /** create another instance of TID 1411 (Volumetric ROI Measurements) and add it as
     *  an included template to this report (TID 1500 - Row 8).  A first instance of
     *  TID 1411 is created and added by calling createNewMeasurementReport().  Access
     *  to the current instance is available through getVolumetricROIMeasurements().
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addVolumetricROIMeasurements();

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

    /** create the mandatory (and other supported) content items of this template,
     *  i.e.\ TID 1500 - Row 1 to 6 and 8.  It is expected that the tree is currently
     *  empty.
     ** @param  title  coded entry that specifies the document title to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createMeasurementReport(const CID7021_MeasurementReportDocumentTitles &title);

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
};


/*--------------------*
 *  type definitions  *
 *--------------------*/

// define short names for the SR template classes
typedef TID1500_MeasurementReport CMR_TID1500;
typedef TID1500_MeasurementReport::TID1411_Measurements CMR_TID1411_in_TID1500;


#endif

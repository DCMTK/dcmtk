/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DVInterface
 *
 */


#ifndef DVIFACE_H
#define DVIFACE_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */


#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmpstat/dvpscf.h"     /* for class DVConfiguration */
#include "dcmtk/dcmpstat/dvpstat.h"    /* for class DVPresentationState */
#include "dcmtk/dcmqrdb/dcmqridx.h"    /* for struct IdxRecord */
#include "dcmtk/ofstd/ofstring.h"      /* for class OFString */
#include "dcmtk/dcmpstat/dvcache.h"    /* for index file caching */


class DicomImage;
class DiDisplayFunction;
class DVPSStoredPrint;
class DVPSPrintMessageHandler;
class DSRDocument;
class DVSignatureHandler;

/** Interface class for the Softcopy Presentation State viewer.
 *  This class manages the database facilities, allows to start and stop
 *  network transactions and gives access to images and presentation states.
 */
class DCMTK_DCMPSTAT_EXPORT DVInterface: public DVConfiguration
{

 public:

   /** constructor.
    *  @param config_file filename (path) of the config file to be used
    *     by the interface object. The caller should make sure that the config file
    *     really exists because the constructor cannot return an error status.
    *     If a non-existing filename (or NULL) is passed, an empty configuration file
    *     is assumed.
    *  @param useLog (optional) flag specifying whether a general log file should be used
    *     (config entry GENERAL\APPLICATION\LOGFILE).
    */
    DVInterface(const char *config_file = NULL, OFBool useLog = OFFalse);

    /** destructor.
     */
    virtual ~DVInterface();

    /* load images, presentation states and structured reports */

    /** loads an image which is contained in the database
     *  and creates a default presentation state for the image.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID study instance UID of the image
     *  @param seriesUID series instance UID of the image
     *  @param instanceUID SOP instance UID of the image
     *  @param changeStatus if true the image file is marked 'reviewed' (not new)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadImage(const char *studyUID, const char *seriesUID, const char *instanceUID, OFBool changeStatus = OFFalse);

    /** loads an image (which need not be contained in the database)
     *  and creates a default presentation state for the image.
     *  This method does not acquire a database lock.
     *  @param filename path and filename of the image to be loaded
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadImage(const char *filename);

    /** loads an image which referenced by the current presentation
     *  state and needs to be contained in the database.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param idx index of the image to be loaded (< getNumberOfImageReferences())
     *  @param changeStatus if true the image file is marked 'reviewed' (not new)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadReferencedImage(size_t idx, OFBool changeStatus = OFFalse);

    /** loads a presentation state which is contained in the database.
     *  The first image referenced in presentation state is also looked up in the
     *  database, loaded, and attached to the presentation state.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID study instance UID of the presentation state
     *  @param seriesUID series instance UID of the presentation state
     *  @param instanceUID SOP instance UID of the presentation state
     *  @param changeStatus if true the pstate and (first) image file is marked 'reviewed' (not new)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadPState(const char *studyUID, const char *seriesUID, const char *instanceUID, OFBool changeStatus = OFFalse);

    /** loads a presentation state and an image (which need not be contained in the database)
     *  and attaches the image to the presentation state (if specified, otherwise the current
     *  image will be used).
     *  This method does not acquire a database lock.
     *  @param pstName path and filename of the presentation state to be loaded
     *  @param imgName path and filename of the image to be loaded
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadPState(const char *pstName, const char *imgName = NULL);

    /** loads a structured report which is contained in the database.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID study instance UID of the structured report
     *  @param seriesUID series instance UID of the structured report
     *  @param instanceUID SOP instance UID of the structured report
     *  @param changeStatus if true the structured report file is marked 'reviewed' (not new)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadStructuredReport(const char *studyUID, const char *seriesUID, const char *instanceUID, OFBool changeStatus = OFFalse);

    /** loads a structured report (which need not be contained in the database).
     *  This method does not acquire a database lock.
     *  @param filename path and filename of the structured report to be loaded
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadStructuredReport(const char *filename);

    /** loads a structured reporting "template".
     *  This "template" is just a DICOM Structured Reporting file which resides in a
     *  special sub-folder and is referenced by the configuration file.  This meachanism
     *  should facilitate the creation of new reports since one does not have to start
     *  with a completely empty report.
     *  Please note that the current structured report is replaced by the specified
     *  "template".  New study/series/instance UIDs are generated automatically for the
     *  new report.
     *  This method does not acquire a database lock.
     *  @param reportID report identifier, as returned by getReportID().
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadSRTemplate(const char *reportID);

    /** saves the current presentation state in the same directory
     *  in which the database index file resides. The filename is generated automatically.
     *  A new SOP Instance UID is assigned whenever a presentation state is saved.
     *  After successfully storing the presentation state, the database index is updated
     *  to include the new object.
     *  This method releases under any circumstances the database lock if it exists.
     *  @param replaceSOPInstanceUID flag indicating whether the
     *    SOP Instance UID should be replaced by a new UID.
     *    If true, a new UID is always generated. If false, a new
     *    UID is generated only if no UID has been assigned before.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition savePState(OFBool replaceSOPInstanceUID);

    /** saves the current presentation state in a file with the given path and filename.
     *  A new SOP Instance UID is assigned whenever a presentation state is saved.
     *  This method does not acquire a database lock and does not register
     *  the saved presentation state in the database.
     *  @param filename path and filename under which the presentation state is to be saved
     *  @param replaceSOPInstanceUID flag indicating whether the
     *    SOP Instance UID should be replaced by a new UID.
     *    If true, a new UID is always generated. If false, a new
     *    UID is generated only if no UID has been assigned before.
     *  @param explicitVR selects the transfer syntax to be written. True (the default) selects
     *    Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition savePState(const char *filename, OFBool replaceSOPInstanceUID, OFBool explicitVR=OFTrue);

    /** saves the DICOM image that is currently attached to the presentation state
     *  in a file with the given path and filename.
     *  This method does not acquire a database lock and does not register
     *  the saved presentation state in the database.
     *  @param filename path and filename under which the image is to be saved
     *  @param explicitVR selects the transfer syntax to be written. True (the default) selects
     *    Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveCurrentImage(const char *filename, OFBool explicitVR=OFTrue);

    /** saves the current structured report in the same directory in which the database index
     *  file resides.  The filename is generated automatically.  A new SOP Instance UID is not
     *  assigned automatically unless the method getStructuredReport().createRevisedVersion()
     *  is called (see documentation in file dcmsr/dsrdoc.h).  This is not required for the
     *  first version of a document (i.e. directly after program start or calling the method
     *  getStructuredReport().createNewDocument()).
     *  After successfully storing the structured report, the database index is updated
     *  to include the new object.
     *  This method releases under any circumstances the database lock if it exists.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveStructuredReport();

    /** saves the current structured report in a file with the given path and filename.
     *  A new SOP Instance UID is not assigned automatically unless the method
     *  getStructuredReport().createRevisedVersion() is called (see documentation in file
     *  dcmsr/dsrdoc.h).  This is not required for the first version of a document (i.e.
     *  directly after program start or calling the method getStructuredReport().createNewDocument()).
     *  This method does not acquire a database lock and does not register the saved structured
     *  report in the database.
     *  @param filename path and filename under which the structured report is to be saved
     *  @param explicitVR selects the transfer syntax to be written. True (the default) selects
     *    Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveStructuredReport(const char *filename, OFBool explicitVR=OFTrue);

    /** adds an image which is contained in the database
     *  to the list of referenced images of the current presentation state.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID study instance UID of the image
     *  @param seriesUID series instance UID of the image
     *  @param instanceUID SOP instance UID of the image
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition addImageReferenceToPState(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** gets the number of image references contained in the current presentation state.
     *  @return number of image references, 0 if an error occurred.
     */
    size_t getNumberOfImageReferences();

    /** returns a reference to the current presentation state.
     *  This reference will become invalid when the DVInterface object is deleted,
     *  a different image or presentation state is loaded
     *  (using loadPState or loadImage) or when resetPresentationState() is called.
     *  @return reference to the current presentation state
     */
    DVPresentationState& getCurrentPState()
    {
      return *pState;
    }

    /** returns a reference to the current structured report.
     *  This reference will become invalid when the DVInterface object is deleted or
     *  a different structured report is loaded (using loadStructuredReport).
     *  @return reference to the current structured report
     */
    DSRDocument& getCurrentReport()
    {
      return *pReport;
    }

    /** returns a reference to the print handler.
     *  This reference remains valid as long as the DVInterface object exists.
     *  @return reference to the current print handler
     */
    DVPSStoredPrint& getPrintHandler()
    {
      return *pPrint;
    }

    /** resets the presentation state object to the status it had immediately after the
     *  last successful operation of "loadImage" or "loadPState". A state can also explicitly
     *  specified as such a "reset state" by using the method saveCurrentPStateForReset().
     *  Attention: The last reference returned by getCurrentPState() becomes invalid
     *  when this method is called.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition resetPresentationState();

    /** saves the current state of the presentation state object to be used for
     *  resetPresentationState(). This is e.g. useful after registration of additional images
     *  directly after a new images has been loaded.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveCurrentPStateForReset();

    /** removes any shared or exclusive lock on the database.
     *  This method should be called when a database transaction
     *  (i.e. reading all studies, series, instances etc.) is finished.
     *  As long as a lock exists on the database, no other application
     *  (i.e. the network receiver) can add new images to the database.
     *  This method also clears the index cache.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition releaseDatabase();

    /** searches in the database for a DICOM instance with the given
     *  study, series and instance UIDs and returns its pathname if found.
     *  If the given instance is not found in the database, NULL is returned.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID the DICOM study instance UID
     *  @param seriesUID the DICOM series instance UID
     *  @param instanceUID the DICOM SOP instance UID
     *  @return filename (path) if found, NULL otherwise
     */
    const char *getFilename(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** returns the number of studies currently contained in the database.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The number reported (returned) by this method remains valid as long
     *  as the database lock remains active and no function modifying the database is called.
     *  Functions that modify the database are: Storing new presentation states,
     *  deleting data, modifying the 'reviewed' status flag of IODs.
     *  @return number of studies in the database.
     */
    Uint32 getNumberOfStudies();

    /** selects the study with the given index in the database.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  Implicitly the first series and first instance within this study is selected, too.
     *  @param idx index to be selected, must be < getNumberOfStudies()
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectStudy(Uint32 idx);

    /** selects the study with the given UID in the database.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  Implicitly the first series and first instance within this study is selected, too.
     *  @param studyUID the DICOM study instance UID
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectStudy(const char *studyUID);

    /** returns the review status of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  Implicitly the first instance within this series is selected, too.
     *  @return study review status
     */
    DVIFhierarchyStatus getStudyStatus() ;

    /** returns the Study Instance UID of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Study Instance UID or NULL if absent or not selected.
     */
    const char *getStudyUID();

    /** returns the Study Description of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Study Description or NULL if absent or not selected.
     */
    const char *getStudyDescription();

    /** returns the Study Date of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Study Date or NULL if absent or not selected.
     */
    const char *getStudyDate();

    /** returns the Study Time of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Study Time or NULL if absent or not selected.
     */
    const char *getStudyTime();

    /** returns the Referring Physicians Name of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Referring Physicians Name or NULL if absent or not selected.
     */
    const char *getReferringPhysiciansName();

    /** returns the Accession Number of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Accession Number or NULL if absent or not selected.
     */
    const char *getAccessionNumber();

    /** returns the Name Of Physicians Reading Study of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Name Of Physicians Reading Study or NULL if absent or not selected.
     */
    const char *getNameOfPhysiciansReadingStudy();

    /** returns the Patient Name of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Patient Name or NULL if absent or not selected.
     */
    const char *getPatientName();

    /** returns the Patient ID of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Patient ID or NULL if absent or not selected.
     */
    const char *getPatientID();

    /** returns the Patient Birth Date of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Patient Birth Date or NULL if absent or not selected.
     */
    const char *getPatientBirthDate();

    /** returns the Patient Sex of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Patient Sex or NULL if absent or not selected.
     */
    const char *getPatientSex();

    /** returns the Patient Birth Time of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Patient Birth Time or NULL if absent or not selected.
     */
    const char *getPatientBirthTime();

    /** returns the Other Patient Names of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Other Patient Names or NULL if absent or not selected.
     */
    const char *getOtherPatientNames();

    /** returns the Other Patient ID of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Other Patient ID or NULL if absent or not selected.
     */
    const char *getOtherPatientID();

    /** returns the Patient Ethnic Group of the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  Note: Since the database uses the Study Root model, patient data appears
     *  on the study level.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Patient Ethnic Group or NULL if absent or not selected.
     */
    const char *getEthnicGroup();

    /** returns the number of series within the currently selected study.
     *  May be called only if a valid study selection exists - see selectStudy().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  See the comments for getNumberOfStudies() about the validity period
     *  of the returned number.
     *  @return number of series in the current study.
     */
    Uint32 getNumberOfSeries();

    /** selects the series with the given index within the currently selected study.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  @param idx index to be selected, must be < getNumberOfSeries()
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectSeries(Uint32 idx);

    /** selects the series with the given UID within the currently selected study.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  @param seriesUID series instance UID of the image
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectSeries(const char *seriesUID);

    /** returns the Series Instance UID of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Instance UID or NULL if absent or not selected.
     */
    const char *getSeriesUID();

    /** returns the review status of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return series review status
     */
    DVIFhierarchyStatus getSeriesStatus();

    /** returns the type of all instances within the currently selected series.
     *  DICOM series always contain a single modality only, if not the modality of
     *  the first examined instance (which is no image) is used (e.g. PresentationState).
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return instance type
     */
    DVPSInstanceType getSeriesType();

    /** returns the type of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return instance type
     */
    DVPSInstanceType getInstanceType();

    /** returns the Series Number of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Number or NULL if absent or not selected.
     */
    const char *getSeriesNumber();

    /** returns the Series Date of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Date or NULL if absent or not selected.
     */
    const char *getSeriesDate();

    /** returns the Series Time of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Time or NULL if absent or not selected.
     */
    const char *getSeriesTime();

    /** returns the Series Description of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Description or NULL if absent or not selected.
     */
    const char *getSeriesDescription();

    /** returns the Series Performing Physicians Name of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Performing Physicians Name or NULL if absent or not selected.
     */
    const char *getSeriesPerformingPhysiciansName();

    /** returns the Series Protocol Name of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Protocol Name or NULL if absent or not selected.
     */
    const char *getSeriesProtocolName();

    /** returns the Series Operators Name of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Series Operators Name or NULL if absent or not selected.
     */
    const char *getSeriesOperatorsName();

    /** returns the Modality of the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Modality or NULL if absent or not selected.
     */
    const char *getModality();

    /** returns the number of instances (IODs) within the currently selected series.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  See the comments for getNumberOfStudies() about the validity period
     *  of the returned number.
     *  @return number of instances in the current series.
     */
    Uint32 getNumberOfInstances();

    /** selects the instance with the given index within the currently selected series.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  @param idx index to be selected, must be < getNumberOfInstances()
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectInstance(Uint32 idx);

    /** selects the instance with the given UID within the currently selected series.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  @param instanceUID SOP instance UID of the instance
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectInstance(const char *instanceUID);

    /** selects the instance with the given UID and SOP class over all studies and series.
     *  Please note that in worst case all studies, series and instances are examined.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  @param instanceUID SOP instance UID of the instance
     *  @param sopClassUID SOP class UID of the instance (might be NULL to be not compared)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectInstance(const char *instanceUID, const char *sopClassUID);

    /** selects the instance with the given UIDs.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The selection remains valid until the database lock is removed or the database
     *  is modified (see comments for getNumberOfStudies).
     *  @param studyUID study instance UID of the instance
     *  @param seriesUID series instance UID of the instance
     *  @param instanceUID SOP instance UID of the instance
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectInstance(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** returns the SOP class UID of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return SOP Instance UID or NULL if absent or not selected.
     */
    const char *getSOPClassUID();

    /** returns the SOP Instance UID of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return SOP Instance UID or NULL if absent or not selected.
     */
    const char *getInstanceUID();

    /** returns the Image Number of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Image Number or NULL if absent or not selected.
     */
    const char *getImageNumber();

    /** returns the Filename of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Filename or NULL if absent or not selected.
     */
    const char *getFilename();

    /** returns the review status of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return instance review status
     */
    DVIFhierarchyStatus getInstanceStatus() ;

    /** returns the escription of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Instance Description or NULL if absent or not selected.
     */
    const char *getInstanceDescription();

    /** returns the Presentation Label of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Presentation Label or NULL if absent or not selected.
     */
    const char *getPresentationLabel();


    /* methods modifying the database */

    /** modifies the review flag for one instance in the database, which is set to
     *  'reviewed' state (DVIF_objectIsNotNew). The status of the corresponding series
     *  and study is updated automatically.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The database is modified - any study, series or instance selection
     *  and the number of studies, series and instances reported will become invalid since
     *  other processes may modify the database before the exclusive lock is granted to this method.
     *  @param studyUID study instance UID of the reviewed instance
     *  @param seriesUID series instance UID of the reviewed instance
     *  @param instanceUID SOP instance UID of the reviewed instance
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition instanceReviewed(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** deletes the given instance from the database. If the corresponding DICOM file
     *  resides in the same directory as the index file, it is also removed.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The database is modified - any study, series or instance selection
     *  and the number of studies, series and instances reported will become invalid since
     *  other processes may modify the database before the exclusive lock is granted to this method.
     *  @param studyUID study instance UID of the instance to be deleted
     *  @param seriesUID series instance UID of the instance to be deleted
     *  @param instanceUID SOP instance UID of the instance to be deleted
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition deleteInstance(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** deletes the given series from the database. Any of the corresponding DICOM files
     *  residing in the same directory as the index file are also removed.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The database is modified - any study, series or instance selection
     *  and the number of studies, series and instances reported will become invalid since
     *  other processes may modify the database before the exclusive lock is granted to this method.
     *  @param studyUID study instance UID of the series to be deleted
     *  @param seriesUID series instance UID of the series to be deleted
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition deleteSeries(const char *studyUID, const char *seriesUID);

    /** deletes the given study from the database. Any of the corresponding DICOM files
     *  residing in the same directory as the index file are also removed.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The database is modified - any study, series or instance selection
     *  and the number of studies, series and instances reported will become invalid since
     *  other processes may modify the database before the exclusive lock is granted to this method.
     *  @param studyUID study instance UID of the study to be deleted
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition deleteStudy(const char *studyUID);

    /* here follow the Network interface methods */

    /** starts the network receiver processes (Storage Service Class SCPs).
     *  The receiver processes will wait for incoming DICOM associations, receive images
     *  and presentation states. Data will be stored in file and registered in the
     *  database index file. Attention: Successful return of this method is no guarantee
     *  that the network receiver has successfully started, because certain errors
     *  (i.e. incorrect settings in the config file) will only be noted in the network
     *  receiver process when running. On Unix platform, successful return of this method
     *  means that the fork() used to start the receiver was successful.
     *  On Win32 platforms, it means that the CreateProcess() call was successful.
     *  @return EC_Normal if the receiver processes could be started, an error code otherwise.
     */
    OFCondition startReceiver();

    /** terminates the network receiver process (Storage Service Class SCP).
     *  This method attempts to terminate the network receiver process by
     *  requesting a DICOM association with it and delivering a special "shutdown" command.
     *  If for some reason the network receiver cannot be found (i.e. because it has
     *  terminated abnormally), a TCP/IP timeout (several seconds) may occur before this method returns.
     *  @return EC_Normal if the receiver process could be terminated, an error code otherwise.
     */
    OFCondition terminateReceiver();

    /** starts the query/retrieve server process (Query/Retrieve Service Class SCP).
     *  The query/retrieve process will wait for incoming DICOM associations, serve queries and
     *  send the requested instances to the specified destination.  Data will be taken from the
     *  same local database used to store received instances and created presentation states,
     *  stored print objects and hardcopy grayscale images.
     *  The configuration file which is required for the query/retrieve process will be created
     *  automatically from the 'global' configuration file each time this method is called, unless
     *  this automatical creation is disabled (entry: [QUERYRETIUEVE] AutoCreateConfigFile = false).
     *  Attention: Successful return of this method is no guarantee that the query/retrieve server
     *  has successfully started, because certain errors (i.e. incorrect settings in the config file)
     *  will only be noted in the query/retrieve process when running. On Unix platform, successful
     *  return of this method means that the fork() used to start the receiver was successful.
     *  On Win32 platforms, it means that the CreateProcess() call was successful.
     *  @return EC_Normal if the query/retrieve process could be started, an error code otherwise.
     */
    OFCondition startQueryRetrieveServer();

    /** terminates the query/retrieve server process (Query/Retrieve Service Class SCP).
     *  This method attempts to terminate the query/retrieve process by requesting a
     *  DICOM association with it and delivering a special "shutdown" command.
     *  If for some reason the query/retrieve server cannot be found (i.e. because it has
     *  terminated abnormally), a TCP/IP timeout (several seconds) may occur before this
     *  method returns.
     *  @return EC_Normal if the query/retrieve process could be terminated,
     *     an error code otherwise.
     */
    OFCondition terminateQueryRetrieveServer();

    /** tests whether the database has been modified in any way since the last
     *  call to this method. Any write access to the database (adding, deleting, changing)
     *  is reported. This method works by modifying and checking the "modification date/time"
     *  of the database index file. This method is not affected by database locks and can be
     *  called at any time.
     *  Always returns OFTrue for the first call after construction of the interface.
     *  Also returns OFTrue if something goes wrong (i.e. if the method cannot make sure
     *  that nothing has changed).
     *  @return OFTrue if the database has been modified since the last call to this method.
     */
    OFBool newInstancesReceived();

    /** sends a complete study over network to a different DICOM peer.
     *  A separate application or process is launched to handle the send operation.
     *  This call returns when the send operation has successfully been launched.
     *  No information about the status or success of the transfer itself is being made
     *  available.
     *  @param targetID symbolic identifier of the send target, must be one of the
     *     strings returned by getTargetID().
     *  @param studyUID Study Instance UID of the study to be sent. Must be a study
     *     contained in the database.
     *  @return EC_Normal when the send process has successfully been launched,
     *     an error condition otherwise.
     */
    OFCondition sendStudy(const char *targetID, const char *studyUID)
    {
      return sendIOD(targetID, studyUID, NULL, NULL);
    }

    /** sends a complete series over network to a different DICOM peer.
     *  A separate application or process is launched to handle the send operation.
     *  This call returns when the send operation has successfully been launched.
     *  No information about the status or success of the transfer itself is being made
     *  available.
     *  @param targetID symbolic identifier of the send target, must be one of the
     *     strings returned by getTargetID().
     *  @param studyUID Study Instance UID of the series to be sent. Must be a series
     *     contained in the database.
     *  @param seriesUID Series Instance UID of the series to be sent. Must be a series
     *     contained in the database.
     *  @return EC_Normal when the send process has successfully been launched,
     *     an error condition otherwise.
     */
    OFCondition sendSeries(const char *targetID, const char *studyUID, const char *seriesUID)
    {
      return sendIOD(targetID, studyUID, seriesUID, NULL);
    }

    /** sends a single instance (image or presentation state)
     *  over network to a different DICOM peer.
     *  A separate application or process is launched to handle the send operation.
     *  This call returns when the send operation has successfully been launched.
     *  No information about the status or success of the transfer itself is being made
     *  available.
     *  @param targetID symbolic identifier of the send target, must be one of the
     *     strings returned by getTargetID().
     *  @param studyUID Study Instance UID of the IOD to be sent. Must be an IOD
     *     contained in the database.
     *  @param seriesUID Series Instance UID of the IOD to be sent. Must be an IOD
     *     contained in the database.
     *  @param instanceUID SOP Instance UID of the IOD to be sent. Must be an IOD
     *     contained in the database.
     *  @return EC_Normal when the send process has successfully been launched,
     *     an error condition otherwise.
     */
    OFCondition sendIOD(const char *targetID, const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** creates a dump of the contents of a DICOM file and displays it on-screen.
     *  A separate application or process is launched to handle the dump and display.
     *  This call returns when the dump operation has successfully been launched.
     *  No information about the status or success of the process itself is being made
     *  available.
     *  This method does not acquire a database lock.
     *  @param filename path of file to be displayed.
     *  @return EC_Normal when the process has successfully been launched,
     *     an error condition otherwise.
     */
    OFCondition dumpIOD(const char *filename);

    /** creates a dump of the contents of a DICOM file and displays it on-screen.
     *  A separate application or process is launched to handle the dump and display.
     *  This call returns when the dump operation has successfully been launched.
     *  No information about the status or success of the process itself is being made
     *  available.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID Study Instance UID of the IOD to be dumped. Must be an IOD
     *     contained in the database.
     *  @param seriesUID Series Instance UID of the IOD to be dumped. Must be an IOD
     *     contained in the database.
     *  @param instanceUID SOP Instance UID of the IOD to be dumped. Must be an IOD
     *     contained in the database.
     *  @return EC_Normal when the process has successfully been launched,
     *     an error condition otherwise.
     */
    OFCondition dumpIOD(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** checks the contents of a DICOM file and displays an evaluation report on the screen.
     *  A separate application or process is launched to handle the evaluation and display.
     *  This call returns when the check operation has successfully been launched.
     *  No information about the status or success of the process itself is being made
     *  available.
     *  This method does not acquire a database lock.
     *  @param filename path of file to be checked.
     *  @return EC_Normal when the process has successfully been launched,
     *     an error condition otherwise.
     */
    OFCondition checkIOD(const char *filename);

    /** checks the contents of a DICOM file and displays an evaluation report on the screen.
     *  A separate application or process is launched to handle the evaluation and display.
     *  This call returns when the check operation has successfully been launched.
     *  No information about the status or success of the process itself is being made
     *  available.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID Study Instance UID of the IOD to be checked. Must be an IOD
     *     contained in the database.
     *  @param seriesUID Series Instance UID of the IOD to be checked. Must be an IOD
     *     contained in the database.
     *  @param instanceUID SOP Instance UID of the IOD to be checked. Must be an IOD
     *     contained in the database.
     *  @return EC_Normal when the process has successfully been launched,
     *     an error condition otherwise.
     */
    OFCondition checkIOD(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** saves a monochrome bitmap as a DICOM Secondary Capture image.
     *  The bitmap must use one byte per pixel, left to right, top to bottom
     *  order of the pixels. 0 is interpreted as black, 255 as white.
     *  @param filename the file name or path under which the image is saved.
     *  @param pixelData a pointer to the image data. Must contain at least
     *    width*height bytes of data.
     *  @param width the width of the image, must be <= 0xFFFF
     *  @param height the height of the image, must be <= 0xFFFF
     *  @param aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @param explicitVR selects the transfer syntax to be written.
     *    True selects Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @param instanceUID optional parameter containing the SOP Instance UID to be written.
     *    This parameter should be omitted unless the SOP Instance UID needs to be controlled
     *    externally.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveDICOMImage(
      const char *filename,
      const void *pixelData,
      unsigned long width,
      unsigned long height,
      double aspectRatio=1.0,
      OFBool explicitVR=OFTrue,
      const char *instanceUID=NULL);

    /** saves a monochrome bitmap as a DICOM Secondary Capture image
     *  in the same directory in which the database index file resides.
     *  The filename is generated automatically.
     *  When the image is stored successfully, the database index is updated
     *  to include the new object.
     *  This method releases under any circumstances the database lock if it exists.
     *  @param pixelData a pointer to the image data. Must contain at least
     *    width*height bytes of data.
     *  @param width the width of the image, must be <= 0xFFFF
     *  @param height the height of the image, must be <= 0xFFFF
     *  @param aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveDICOMImage(
      const void *pixelData,
      unsigned long width,
      unsigned long height,
      double aspectRatio=1.0);

    /** saves a monochrome bitmap as a DICOM Hardcopy Grayscale image.
     *  The bitmap must use 16 bits per pixel, left to right, top to bottom
     *  order of the pixels. It is assumed that only values 0..4095 are used.
     *  @param filename the file name or path under which the image is saved.
     *  @param pixelData a pointer to the image data. Must contain at least
     *    width*height*2 bytes of data.
     *  @param width the width of the image, must be <= 0xFFFF
     *  @param height the height of the image, must be <= 0xFFFF
     *  @param aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @param explicitVR selects the transfer syntax to be written.
     *    True selects Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @param instanceUID optional parameter containing the SOP Instance UID to be written.
     *    This parameter should be omitted unless the SOP Instance UID needs to be controlled
     *    externally.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveHardcopyGrayscaleImage(
      const char *filename,
      const void *pixelData,
      unsigned long width,
      unsigned long height,
      double aspectRatio=1.0,
      OFBool explicitVR=OFTrue,
      const char *instanceUID=NULL);

    /** saves a monochrome bitmap as a DICOM Hardcopy Grayscale image
     *  in the same directory in which the database index file resides.
     *  The filename is generated automatically.
     *  When the image is stored successfully, the database index is updated
     *  to include the new object.
     *  This method releases under any circumstances the database lock if it exists.
     *  @param pixelData a pointer to the image data. Must contain at least
     *    width*height*2 bytes of data.
     *  @param width the width of the image, must be <= 0xFFFF
     *  @param height the height of the image, must be <= 0xFFFF
     *  @param aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveHardcopyGrayscaleImage(
      const void *pixelData,
      unsigned long width,
      unsigned long height,
      double aspectRatio=1.0);

    /** saves a DICOM object into a file in the same directory in which the
     *  database index file resides. The object must contain a SOP Class
     *  UID and SOP Instance UID. The filename is generated automatically.
     *  When the image is stored successfully, the database index is
     *  updated to include the new object. This method releases under any
     *  circumstances the database lock if it exists.
     *  @param fileformat the complete DICOM file object to be written
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveFileFormatToDB(DcmFileFormat &fileformat);

    /** loads a Stored Print object which is contained in the database into memory.
     *  Attention: The current print job (Stored Print object) will be deleted by doing this.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID study instance UID of the Stored Print object
     *  @param seriesUID series instance UID of the Stored Print object
     *  @param instanceUID SOP instance UID of the Stored Print object
     *  @param changeStatus if true the stored print object is marked 'reviewed' (not new)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadStoredPrint(const char *studyUID, const char *seriesUID, const char *instanceUID, OFBool changeStatus = OFFalse);

    /** loads a Stored Print object (which need not be contained in the database) into memory.
     *  Attention: The current print job (Stored Print object) will be deleted by doing this.
     *  This method does not acquire a database lock.
     *  @param filename path and filename of the Stored Print object to be loaded
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition loadStoredPrint(const char *filename);

    /** saves the current print job as a Stored Print object.
     *  @param filename the file name or path under which the image is saved.
     *  @param writeRequestedImageSize if false, the Requested Image Size attributes are not written,
     *    e. g. because they are not supported by the target printer.
     *  @param explicitVR selects the transfer syntax to be written.
     *    True selects Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @param instanceUID optional parameter containing the SOP Instance UID to be written.
     *    This parameter should be omitted unless the SOP Instance UID needs to be controlled
     *    externally.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveStoredPrint(
      const char *filename,
      OFBool writeRequestedImageSize,
      OFBool explicitVR=OFTrue,
      const char *instanceUID=NULL);

    /** saves the current print job as a Stored Print object
     *  in the same directory in which the database index file resides.
     *  The filename is generated automatically.
     *  When the image is stored successfully, the database index is updated
     *  to include the new object.
     *  This method releases under any circumstances the database lock if it exists.
     *  @param writeRequestedImageSize if false, the Requested Image Size attributes are not written,
     *    e. g. because they are not supported by the target printer.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition saveStoredPrint(OFBool writeRequestedImageSize);

    /** gets the number of Hardcopy Grayscaleimages currently registered by the stored print object.
     *  @return number of images.
     */
    size_t getNumberOfPrintPreviews();

    /** loads a Hardcopy Grayscale image registered by the stored print object and creates a preview.
     *  The preview bitmap is implicitly scaled to fit into the rectangle specified by
     *  setMaxPrintPreviewWidthHeight().
     *  @param idx index of the image, must be < getNumberOfPrintPreviews()
     *  @param printLUT OFTrue if presentation LUT should be interpreted as a print presentation LUT
     *    (default, in this case there is no implicit scaling of the input width of the LUT and,
     *    therefore, the VOI transformation - which is absent for print - is used),
     *    OFFalse otherwise (softcopy interpretation of a presentation LUT)
     *  @param changeStatus if true the hardcopy grayscale image file is marked 'reviewed' (not new)
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition loadPrintPreview(size_t idx, OFBool printLUT = OFTrue, OFBool changeStatus = OFFalse);

    /** removes a currently loaded Hardcopy Grayscale image from memory.
     */
    void unloadPrintPreview();

    /** gets number of bytes used for the print preview bitmap.
     *  (depends on width, height and depth)
     *  @return number of bytes used for the preview bitmap
     */
    unsigned long getPrintPreviewSize();

    /** sets the maximum print preview bitmap width and height.
     *  Larger images are scaled down (according to the pixel aspect ratio) to fit into
     *  the specified rectangle.
     *  Attention: If the values differ from the the previous ones the currently loaded
     *  hardcopy grayscale image (preview) is automatically detroyed and has to be re-loaded.
     *  @param width maximum width of preview bitmap (in pixels)
     *  @param height maximum height of preview bitmap (in pixels)
     */
    void setMaxPrintPreviewWidthHeight(unsigned long width, unsigned long height);

    /** gets width and height of print preview bitmap.
     *  The return values depend on the current maximum preview bitmap width/height values!
     *  @param width upon success, the bitmap width (in pixels) is returned in this parameter
     *  @param height upon success, the bitmap height (in pixels) is returned in this parameter
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getPrintPreviewWidthHeight(unsigned long &width, unsigned long &height);

    /** writes the bitmap data of the print preview image into the given buffer.
     *  The storage area must be allocated and deleted from the calling method.
     *  @param bitmap pointer to storage area where the pixel data is copied to (array of 8 bit values)
     *  @param size specifies size of the storage area in bytes
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getPrintPreviewBitmap(void *bitmap, unsigned long size);

    /** stores the current presentation state in a temporary place
     *  and creates a new presentation state that corresponds with an
     *  image displayed "without" presentation state.
     *  If called twice, an error code is returned.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition disablePState();

    /** restores the stored presentation state (see disablePresentationState)
     *  and deletes the temporary presentation state.
     *  If no stored presentation state exists, returns an error.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition enablePState();

    /** returns number of presentation states referencing the currently selected image.
     *  If no instance is currently selected or the selected instance is a presentation
     *  state, returns an error.
     *  @return number of presentation states, 0 if none available or an error occurred
     */
    Uint32 getNumberOfPStates();

    /** selects and loads specified presentation state referencing the currently selected
     *  image.
     *  @param idx index to be selected, must be < getNumberOfPStates()
     *  @param changeStatus if true the presentation state is marked 'reviewed' (not new)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition selectPState(Uint32 idx, OFBool changeStatus = OFFalse);

    /** returns description of specified presentation state referencing the currently
     *  selected image.
     *  @param idx index to be selected, must be < getNumberOfPStates()
     *  @return presentation state description or NULL idx is invalid
     */
    const char *getPStateDescription(Uint32 idx);

    /** returns label of specified presentation state referencing the currently
     *  selected image.
     *  @param idx index to be selected, must be < getNumberOfPStates()
     *  @return presentation state label or NULL idx is invalid
     */
    const char *getPStateLabel(Uint32 idx);

    /** checks whether display correction is possible (in principle),
     *  i.e. a valid monitor characteristics description exists
     *  and current system is a low-cost system (without built-in
     *  calibration).
     *  @param transform display transform to be checked (default: GSDF)
     *  @return OFTrue if display transform is possible, OFFalse otherwise
     */
    OFBool isDisplayTransformPossible(DVPSDisplayTransform transform = DVPSD_GSDF);

    /** sets ambient light value for the display transformation.
     *  @param value ambient light value to be set
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition setAmbientLightValue(double value);

    /** returns ambient light value for the display transformation.
     *  @param value returned ambient light value
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition getAmbientLightValue(double &value);

    /* print related methods */

    /** selects the current printer. Also adjusts the destination AE title and the
     *  printer name attribute within the Stored Print object.
     *  @param targetID one of the printer target IDs returned by getTargetID().
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition setCurrentPrinter(const char *targetID);

    /** gets the current printer's target ID.
     *  @return printer target ID, can be NULL if no printer is defined
     *   in the configuration file.
     */
    const char *getCurrentPrinter();

    /** sets the (optional) print medium type.
     *  @param value new attribute value, may be NULL.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition setPrinterMediumType(const char *value);

    /** gets the (optional) print medium type.
     *  @return medium type, may be NULL.
     */
    const char *getPrinterMediumType();

    /** sets the (optional) printer film destination.
     *  @param value new attribute value, may be NULL.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition setPrinterFilmDestination(const char *value);

    /** gets the (optional) printer film destination.
     *  @return printer film destination, may be NULL or empty string.
     */
    const char *getPrinterFilmDestination();

    /** sets the (optional) printer film session label.
     *  @param value new attribute value, may be NULL.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition setPrinterFilmSessionLabel(const char *value);

    /** gets the (optional) printer film session label.
     *  @return printer film session label, may be NULL or empty string.
     */
    const char *getPrinterFilmSessionLabel();

    /** sets the (optional) print priority.
     *  @param value new attribute value, may be NULL.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition setPrinterPriority(const char *value);

    /** gets the (optional) print priority.
     *  @return print priority, may be NULL or empty string.
     */
    const char *getPrinterPriority();

    /** sets the (optional) print session owner ID.
     *  @param value new attribute value, may be NULL.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition setPrinterOwnerID(const char *value);

    /** gets the (optional) print session owner ID.
     *  @return print session owner ID, may be NULL or empty string.
     */
    const char *getPrinterOwnerID();

    /** sets the (optional) print number of copies.
     *  @param value new attribute value, may be 0.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition setPrinterNumberOfCopies(unsigned long value);

    /** gets the (optional) print number of copies.
     *  @return print number of copies, 0 if absent.
     */
    unsigned long getPrinterNumberOfCopies();

    /** resets the settings for basic film session (everything that
     *  is not managed by the Stored Print object) to initial state.
     *  Affects medium type, film destination, film session label,
     *  priority, owner ID, and number of copies.
     */
    void clearFilmSessionSettings();

    /** sets the LUT with the given identifier
     *  in the Presentation State as current Presentation LUT.
     *  @param lutID LUT identifier, as returned by getLUTID().
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition selectDisplayPresentationLUT(const char *lutID);

    /** if the Presentation State contains an active
     *  Presentation LUT that was set with selectDisplayPresentationLUT(),
     *  return the corresponding LUT identifier.
     *  @return lutID if found, NULL or empty string otherwise.
     */
    const char *getDisplayPresentationLUTID();

    /** sets the LUT with the given identifier in the Stored Print object
     *  as current Presentation LUT. This LUT overrides the settings made
     *  for the separate image boxes, it can be deactivated using the method
     *  DVPSStoredPrint::setDefaultPresentationLUT().
     *  @param lutID LUT identifier, as returned by getLUTID().
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition selectPrintPresentationLUT(const char *lutID);

    /** if the Stored Print object contains an active
     *  Presentation LUT that was set with selectPrintPresentationLUT(),
     *  return the corresponding LUT identifier.
     *  @return lutID if found, NULL or empty string otherwise.
     */
    const char *getPrintPresentationLUTID();

    /** start spooling of print job with current settings.
     *  @param deletePrintedImages if true, delete printed images from queue.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition spoolPrintJob(OFBool deletePrintedImages=OFTrue);

    /** starts the print spooler process.
     *  The print spooler will wait for print jobs created with spoolPrintJob()
     *  and communicate them to the printer using the DICOM Print Management Service Class.
     *  Attention: Successful return of this method is no guarantee
     *  that the spooler has successfully started, because certain errors
     *  (i.e. incorrect settings in the config file) will only be noted in the spooler
     *  process when running. On Unix platform, successful return of this method
     *  means that the fork() used to start the spooler was successful.
     *  On Win32 platforms, it means that the CreateProcess() call was successful.
     *  @return EC_Normal if the spooler process could be started, an error code otherwise.
     */
    OFCondition startPrintSpooler();

    /** terminates the print spooler process. This method creates a "dummy"
     *  print job that request the print spooler to shutdown as soon as all other pending
     *  print jobs are finished.
     *  @return EC_Normal if the spooler process dummy print job could be written,
     *    an error code otherwise.
     */
    OFCondition terminatePrintSpooler();

    /** starts the print server process (Basic Grayscale Print Management SCP).
     *  The print server process will wait for incoming DICOM associations, handle the
     *  DICOM print protcol, store data in file and register stored print and grayscale
     *  image objects in the database index file.
     *  Attention: Successful return of this method is no guarantee that the print
     *  server has successfully started, because certain errors (i.e. incorrect settings
     *  in the config file) will only be noted in the print server process when running.
     *  On Unix platform, successful return of this method means that the fork() used to
     *  start the server was successful.
     *  On Win32 platforms, it means that the CreateProcess() call was successful.
     *  @return EC_Normal if the server process could be started, an error code otherwise.
     */
    OFCondition startPrintServer();

    /** terminates the print server process (Basic Grayscale Print Management SCP).
     *  This method attempts to terminate the print server process by requesting a DICOM
     *  association with it and delivering a special "shutdown" command.
     *  If for some reason the print server cannot be found (i.e. because it has terminated
     *  abnormally), a TCP/IP timeout (several seconds) may occur before this method returns.
     *  @return EC_Normal if the server process could be terminated, an error code otherwise.
     */
    OFCondition terminatePrintServer();

    /** adds an existing DICOM image (should be Hardcopy Grayscale)
     *  that is already present in the image database to the current print image queue
     *  without rendering it again.
     *  The "requested image size" option is not used - the bitmap is treated as if the
     *  presentation mode was "SCALE TO FIT".
     *  @param studyUID study instance UID of the image, as reported by getStudyUID()
     *  @param seriesUID series instance UID of the image, as reported by getSeriesUID()
     *  @param instanceUID SOP instance UID of the image, as reported by getInstanceUID()
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition addToPrintHardcopyFromDB(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** requests the spooler process to print an old print job that is stored
     *  in the database as a "stored print" object. The Stored Print that is printed
     *  does not contain all parameters of a print job. The following parameters are taken from the
     *  current settings in this object: Target printer, medium type,
     *  illumination and reflected ambient light.
     *  @param studyUID study instance UID of the Stored Print, as reported by getStudyUID()
     *  @param seriesUID series instance UID of the Stored Print, as reported by getSeriesUID()
     *  @param instanceUID SOP instance UID of the Stored Print, as reported by getInstanceUID()
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition spoolStoredPrintFromDB(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** Initiates the creation of a DICOM Basic Film Session SOP Instance in the printer.
     *  This method stores all Basic Film Session related attributes that are managed by this object
     *  in a DICOM dataset and passes the result to the embedded Stored Print object which manages
     *  the further communication.
     *  @param printHandler print communication handler, association must be open.
     *  @param plutInSession true if printer expects referenced presentation LUT sequence, illumination
     *    and reflected ambient light in basic film session, false if it expects them in basic film box.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition printSCUcreateBasicFilmSession(DVPSPrintMessageHandler& printHandler, OFBool plutInSession);

    /* annotation interface */

    /** gets the current setting of the annotation activity annotation flag.
     *  @return OFTrue if annotation is on, OFFalse otherwise.
     */
    OFBool isActiveAnnotation() { return activateAnnotation; }

    /** gets the current setting of the Prepend Date/Time annotation flag.
     *  @return OFTrue if Prepend Date/Time is on, OFFalse otherwise.
     */
    OFBool getPrependDateTime() { return prependDateTime; }

    /** gets the current setting of the Prepend Printer Name annotation flag.
     *  @return OFTrue if Prepend Printer Name is on, OFFalse otherwise.
     */
    OFBool getPrependPrinterName() { return prependPrinterName; }

    /** gets the current setting of the Prepend Lighting annotation flag.
     *  @return OFTrue if Prepend Lighting is on, OFFalse otherwise.
     */
    OFBool getPrependLighting() { return prependLighting; }

    /** gets the current annotation text.
     *  @return annotation text, may be NULL or empty string.
     */
    const char *getAnnotationText() { return annotationText.c_str(); }

    /** switches annotation printing on/off
     *  @param value OFTrue if annotation is switched on, OFFalse otherwise.
     */
    void setActiveAnnotation(OFBool value) { activateAnnotation=value; }

    /** sets the Prepend Date/Time annotation flag.
     *  @param value OFTrue if Prepend Date/Time is switched on, OFFalse otherwise.
     */
    void setPrependDateTime(OFBool value) { prependDateTime=value; }

    /** sets the Prepend Printer Name annotation flag.
     *  @param value OFTrue if Prepend Printer Name is switched on, OFFalse otherwise.
     */
    void setPrependPrinterName(OFBool value) { prependPrinterName=value; }

    /** sets the Prepend Lighting annotation flag.
     *  @param value OFTrue if Prepend Lighting is switched on, OFFalse otherwise.
     */
    void setPrependLighting(OFBool value) { prependLighting=value; }

    /** sets the current annotation text.
     *  @param value new text, may be NULL.
     */
    void setAnnotationText(const char *value);

    /* user management interface */

    /** verifies the given password for the given user ID. This method tries
     *  to load and decrypt the private key for the given user with the
     *  given password. If this fails, the password verification fails,
     *  otherwise the password verification succeeds.
     *  This method requires that DCMTK be configured and compiled with
     *  the WITH_OPENSSL flag, otherwise always returns false.
     *  @param userID symbolic user ID for given user, as returned by
     *    DVConfiguration::getUserID()
     *  @param passwd for user as entered in some GUI control
     *  @return true if password verification succeeds, false otherwise.
     */
    OFBool verifyUserPassword(const char *userID, const char *passwd);

    /** verifies and digitally signs the current structured report.
     *  If the user ID is known (i.e. specified in the configuration file) the current
     *  structured report is verified (a verifying observer is added). If the 'mode'
     *  parameter is set accordingly and the password is correct (see verifyUserPassword)
     *  the report is also digitally signed.
     *  Please note that a document can be verified/signed more than once, but only completed
     *  documents can be verified. After signing the report no modifications should be performed
     *  before the object is stored in the database or a file. Otherwise the digital signature
     *  would be corrupted. Therefore, the SOP instance UID should be generated before calling
     *  this method.
     *  To digitally sign the report it is required that DCMTK is configured and compiled
     *  with the WITH_OPENSSL flag, otherwise only verification is available (returns an
     *  error code if mode differs from DVPSY_verify).
     *  @param userID symbolic user ID for given user, as returned by
     *    DVConfiguration::getUserID()
     *  @param passwd for user as entered in some GUI control
     *  @param mode flag specifying whether to verify only, verify and sign or verify and
     *    sign and finalize the document. The difference between the second and the third mode
     *    is that "finalize" always signs the entire document whereas the other mode only signs
     *    the marked items (if any, all items otherwise) and leaves out certain header attributes
     *    (e.g. the SOP instance UID and the verifying observer sequence).
     *  @return status, EC_Normal if successful, an error code otherwise.
     */
    OFCondition verifyAndSignStructuredReport(const char *userID, const char *passwd, DVPSVerifyAndSignMode mode);


    /* digital signatures */

    /** returns a string containing a complete HTML page with the
     *  signature validation results for the last object of the
     *  given type. Never returns NULL.
     *  @param objtype object type
     *  @return pointer to string with HTML page
     */
    const char *getCurrentSignatureValidationHTML(DVPSObjectType objtype) const;

    /** returns a string containing a complete HTML page with the
     *  signature validation overview. Never returns NULL.
     *  @return pointer to string with HTML page
     */
    const char *getCurrentSignatureValidationOverview() const;

    /** returns the status flag for the current object of given type.
     *  @param objtype object type
     *  @return digital signature status for object
     */
    DVPSSignatureStatus getCurrentSignatureStatus(DVPSObjectType objtype) const;

    /** returns the combined status flag for the current image and presentation state.
     *  @return digital signature status for image and presentation state
     */
    DVPSSignatureStatus getCombinedImagePStateSignatureStatus() const;

    /** returns number of correct signatures for given object type.
     *  @param objtype object type
     *  @return number of digital signatures
     */
    unsigned long getNumberOfCorrectSignatures(DVPSObjectType objtype) const;

    /** returns number of untrustworthy signatures for given object type.
     *  @param objtype object type
     *  @return number of digital signatures
     */
    unsigned long getNumberOfUntrustworthySignatures(DVPSObjectType objtype) const;

    /** returns number of corrupt signatures for given object type.
     *  @param objtype object type
     *  @return number of digital signatures
     */
    unsigned long getNumberOfCorruptSignatures(DVPSObjectType objtype) const;

    /** disables internal settings for image and presentation state.
     *  Called when a new SR object is loaded and the current
     *  image/presentation state are hidden consequently.
     */
    void disableImageAndPState();

private:

    /** private undefined copy constructor
     */
    DVInterface(const DVInterface&);

    /** private undefined assignment operator
     */
    DVInterface& operator=(const DVInterface&);

    /** helper function that exchanges the current presentation state and image
     *  by the pointers passed and frees the old ones.
     *  @param newState new presentation state, must not be NULL
     *  @param image image file
     *  @param state presentation state if newState was not created from image.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition exchangeImageAndPState(DVPresentationState *newState, DcmFileFormat *image, DcmFileFormat *state=NULL);

    /** creates a database handle if none exists yet (this method may
     *  be called multiple times without interference) and puts a shared lock
     *  on the database.
     *  The lock will remain until explicitly released with releaseDatabase();
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition lockDatabase();

    /** creates an exlusive lock on the database if none exists.
     *  The lock will remain until explicitly released with releaseDatabase()
     *  or unlockExclusive().
     *  This method also clears the index cache.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition lockExclusive();

    /** removes an exlusive lock on the database if any.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition unlockExclusive();

    /** creates a new pair of filenames for print job creation.
     *  @param printer printer identifier
     *  @param tempname temporary name is returned here
     *  @param jobname print job name is returned here
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition createPrintJobFilenames(const char *printer, OFString& tempname, OFString& jobname);

    /** creates the query/retrieve server configuration file.
     *  @param filename path to the configuration file (incl. filename)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition createQueryRetrieveServerConfigFile(const char *filename);

    /** starts an external application and passes a filename.
     *  @param application pathname of the application to be started
     *  @param filename filename to be passed as single parameter.
     *  @return EC_Normal if fork or CreateProcess succeeds, an error
     *    condition otherwise.
     */
    OFCondition startExternalApplication(const char *application, const char *filename);

    /* member variables */

    /** pointer to the current print handler object
     */
    DVPSStoredPrint *pPrint;

    /** pointer to the current presentation state object
     */
    DVPresentationState *pState;

    /** pointer to the current structured reporting object (should never be NULL)
     */
    DSRDocument *pReport;

    /** pointer to the handler object for digital signatures
     */
    DVSignatureHandler *pSignatureHandler;

    /** pointer to the stored presentation state object (if any)
     */
    DVPresentationState *pStoredPState;

    /** pointer to the current DICOM image attached to the presentation state
     */
    DcmFileFormat *pDicomImage;

    /** pointer to the current DICOM dataset containing the loaded presentation state.
     *  Is NULL when the presentation state has been created "on the fly" from image.
     */
    DcmFileFormat *pDicomPState;

    /** pointer to the current hardcopy grayscale image (bitmap information only)
     */
    DicomImage *pHardcopyImage;

    /** a unique string generated for each instance of this class.
     *  Used to identify print jobs generated from this instance.
     */
    OFString printJobIdentifier;

    /** a counter used for generating print job names.
     */
    unsigned long printJobCounter;

    /** string containing the path name of the config file as passed to the ctor.
     */
    OFString configPath;

    /** string containing the path name of the database index file
     *  after a database lock has been acquired for the first time
     */
    OFString databaseIndexFile;

    /** initialized with construction time of the interface object
     *  minus one day. Used to check modifications of the database index file.
     */
    unsigned long referenceTime;

    /** list of display function object
     */
    DiDisplayFunction *displayFunction[DVPSD_max];

    /** handle to access database/index file
     */
    DcmQueryRetrieveIndexDatabaseHandle *pHandle;

    /** locking mode (OFFalse => shared, OFTrue => exclusive)
     */
    OFBool lockingMode;

    /** hierarchical cache structure to optimize index file access
     */
    DVStudyCache idxCache;

    /** record structure of index items (last read entry)
     */
    IdxRecord idxRec;

    /** position of last read index record (for optimization purposes)
     */
    int idxRecPos;

    /** OFTrue if current image is already stored in database, OFFalse otherwise
     */
    OFBool imageInDatabase;

    /* private methods for database */

    /** creates index cache to optimize reading of index file
     */
    OFBool createIndexCache();

    /** creates cache of referencing pstates for the current image
     */
    OFBool createPStateCache();

    /** clears index cache (includes pstate cache)
     */
    void clearIndexCache();

    /** clears specified index record (and makes pos invalid)
     */
    void clearIndexRecord(IdxRecord &record,
                          int &recpos);

    /** reads specified index record (comparing pos with oldpos to avoid redundant
     *  loading)
     */
    OFBool readIndexRecord(const int pos,
                           IdxRecord &record,
                           int *oldpos = NULL);

    /** updates (hierarchical) status cache (propagates status information from instances
     *  to series and from series to studies)
     */
    void updateStatusCache();

    /** returns pointer to study struct specified by given UIDs or to current study
     */
    DVStudyCache::ItemStruct *getStudyStruct(const char *studyUID = NULL,
                                             const char *seriesUID = NULL);

    /** returns pointer to series struct specified by given UIDs or to current series
     */
    DVSeriesCache::ItemStruct *getSeriesStruct(const char *studyUID = NULL,
                                               const char *seriesUID = NULL,
                                               const char *instanceUID = NULL);

    /** returns pointer to instance struct specified by given UIDs or to current instance
     */
    DVInstanceCache::ItemStruct *getInstanceStruct(const char *studyUID = NULL,
                                                   const char *seriesUID = NULL,
                                                   const char *instanceUID = NULL);

    /** returns index of specified study within study description record
     */
    int findStudyIdx(StudyDescRecord *study,
                     const char *uid);

    /** conditionally deletes given image file (only if file resides in index.dat directory)
     */
    int deleteImageFile(const char *filename);

    /** resets index file modification time to reference time (yesterday)
     */
    void resetDatabaseReferenceTime();


    /** modifies the review flag for one instance in the database (see method with three UID
     *  parameters)
     */
    OFCondition instanceReviewed(int pos);

    /** minimum width of print bitmap (used for implicit scaling)
     */
    unsigned long minimumPrintBitmapWidth;

    /** minimum height of print bitmap (used for implicit scaling)
     */
    unsigned long minimumPrintBitmapHeight;

    /** maximum width of print bitmap (used for implicit scaling)
     */
    unsigned long maximumPrintBitmapWidth;

    /** maximum height of print bitmap (used for implicit scaling)
     */
    unsigned long maximumPrintBitmapHeight;

    /** maximum width of print preview bitmap
     */
    unsigned long maximumPrintPreviewWidth;

    /** maximum height of print preview bitmap
     */
    unsigned long maximumPrintPreviewHeight;

    /** maximum width of (optional) preview image
     */
    unsigned long maximumPreviewImageWidth;

    /** maximum height of (optional) preview image
     */
    unsigned long maximumPreviewImageHeight;

    /** target ID of current printer, empty if no printer exists in config file
     */
    OFString currentPrinter;

     /** config file identifier of LUT currently selected as Display Presentation LUT
     */
    OFString displayCurrentLUTID;

     /** config file identifier of LUT currently selected as Print Presentation LUT
     */
    OFString printCurrentLUTID;

    /** printer medium type identifier, may be empty. VR=CS, VM=1
     */
    OFString printerMediumType;

    /** printer film destination identifier, may be empty. VR=CS, VM=1
     */
    OFString printerFilmDestination;

    /** printer film session label, may be empty. VR=LO, VM=1
     */
    OFString printerFilmSessionLabel;

    /** printer number of copies
     */
    unsigned long printerNumberOfCopies;

    /** printer print priority, may be empty. VR=CS, VM=1,
     *  enumerated values: HIGH, MED, LOW
     */
    OFString printerPriority;

    /** printer film session owner ID, may be empty. VR=SH, VM=1
     */
    OFString printerOwnerID;

    /** true if annotation should be created when spooling print job
     */
    OFBool activateAnnotation;

    /** true if date and time should be prepended to annotation text
     */
    OFBool prependDateTime;

    /** true if printer name should be prepended to annotation text
     */
    OFBool prependPrinterName;

    /** true if reflected ambient light and illumination should be prepended to annotation text
     */
    OFBool prependLighting;

    /** annotation text (if any)
     */
    OFString annotationText;
};


#endif

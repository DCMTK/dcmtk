/*
 *
 *  Copyright (C) 1998-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DVInterface
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:13:54 $
 *  CVS/RCS Revision: $Revision: 1.58 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DVIFACE_H
#define DVIFACE_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include <iostream.h>   /* for ostream */

#include "dctk.h"
#include "dvpscf.h"     /* for class DVConfiguration */
#include "dvpstat.h"    /* for class DVPresentationState */
#include "dbpriv.h"     /* for struct IdxRecord */
#include "dbstore.h"    /* for filedeletion */
#include "ofstring.h"   /* for class OFString */
#include "imagedb.h"    /* for DB_UpperMaxBytesPerStudy */
#include "dvcache.h"    /* for index file caching */

/* max study count for DB handle creation */
#define PSTAT_MAXSTUDYCOUNT 200
/* study size for DB handle creation */
#define PSTAT_STUDYSIZE DB_UpperMaxBytesPerStudy
/* private SOP Class UID used to shutdown the Presentation State network receiver */
#define PSTAT_PRIVATESOPCLASSUID "1.2.276.0.7230010.3.4.1915765545.18030.917282194.0"
/* filename suffixes for print jobs */
#define PRINTJOB_SUFFIX      ".job"
#define PRINTJOB_DONE_SUFFIX ".old"
#define PRINTJOB_TEMP_SUFFIX ".tmp"

class DVPSConfig;
class DiDisplayFunction;
class DVPSStoredPrint;
class DVPSPrintMessageHandler;


/** Interface class for the Softcopy Presentation State viewer.
 *  This class manages the database facilities, allows to start and stop
 *  network transactions and gives access to images and presentation states.
 */
class DVInterface: public DVConfiguration
{

 public:

   /** constructor.
    *  @param config_file filename (path) of the config file to be used
    *     by the interface object. The caller should make sure that the config file
    *     really exists because the constructor cannot return an error status.
    *     If a non-existing filename (or NULL) is passed, an empty configuration file
    *     is assumed.
    */
    DVInterface(const char *config_file=NULL);

    /** destructor.
     */
    virtual ~DVInterface();

    /* load images and presentation states */

    /** loads an image which is contained in the database
     *  and creates a default presentation state for the image.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID study instance UID of the image
     *  @param seriesUID series instance UID of the image
     *  @param instanceUID SOP instance UID of the image
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition loadImage(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** loads an image (which need not be contained in the database)
     *  and creates a default presentation state for the image.
     *  This method does not acquire a database lock.
     *  @param filename path and filename of the image to be loaded
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition loadImage(const char *filename);

    /** loads a presentation state which is contained in the database.
     *  The first image referenced in presentation state is also looked up in the
     *  database, loaded, and attached to the presentation state.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @param studyUID study instance UID of the presentation state
     *  @param seriesUID series instance UID of the presentation state
     *  @param instanceUID SOP instance UID of the presentation state
     *  @param changeStatus if true the pstate and image file is marked 'reviewed' (not new)
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition loadPState(const char *studyUID, const char *seriesUID, const char *instanceUID, OFBool changeStatus = OFFalse);

    /** loads a presentation state and an image (which need not be contained in the database)
     *  and attaches the image to the presentation state (if specified, otherwise the current
     *  image will be used).
     *  This method does not acquire a database lock.
     *  @param pstName path and filename of the presentation state to be loaded
     *  @param imgName path and filename of the image to be loaded
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition loadPState(const char *pstName, const char *imgName = NULL);

    /** saves the current presentation state in the same directory
     *  in which the database index file resides. The filename is generated automatically.
     *  A new SOP Instance UID is assigned whenever a presentation state is saved.
     *  After successfully storing the presentation state, the database index is updated
     *  to include the new object.
     *  This method releases under any circumstances the database lock if it exists.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition savePState();

    /** saves the current presentation state in a file with the given path and filename.
     *  A new SOP Instance UID is assigned whenever a presentation state is saved.
     *  This method does not acquire a database lock and does not register
     *  the saved presentation state in the database.
     *  @param filename path and filename under which the presentation state is to be saved
     *  @param explicitVR selects the transfer syntax to be written. True (the default) selects
     *    Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition savePState(const char *filename, OFBool explicitVR=OFTrue);

    /** saves the DICOM image that is currently attached to the presentation state
     *  in a file with the given path and filename.
     *  This method does not acquire a database lock and does not register
     *  the saved presentation state in the database.
     *  @param filename path and filename under which the image is to be saved
     *  @param explicitVR selects the transfer syntax to be written. True (the default) selects
     *    Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition saveCurrentImage(const char *filename, OFBool explicitVR=OFTrue);

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

    /** returns a reference to the print handler.
     *  This reference remains valid as long as the DVInterface object exists.
     *  @return reference to the current print handler
     */
    DVPSStoredPrint& getPrintHandler()
    {
      return *pPrint;
    }

    /** resets the presentation state object to the status
     *  it had immediately after the last successful operation of "loadImage" or "loadPState".
     *  Attention: The last reference returned by getCurrentPState() becomes invalid
     *  when this method is called.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition resetPresentationState();

    /** removes any shared or exclusive lock on the database.
     *  This method should be called when a database transaction
     *  (i.e. reading all studies, series, instances etc.) is finished.
     *  As long as a lock exists on the database, no other application
     *  (i.e. the network receiver) can add new images to the database.
     *  This method also clears the index cache.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition releaseDatabase();

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
    E_Condition selectStudy(Uint32 idx);

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
    E_Condition selectSeries(Uint32 idx);

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

// --- <BEGIN> ONLY FOR COMPATIBILITY REASONS <BEGIN> ---

    /** checks if the current series consists (only) of Presentation States.
     *  Since DICOM series always contain a single modality only, a series is
     *  either completely a presentation state series or completely different.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return OFTrue if series contains presentation states, OFFalse otherwise.
     */
    OFBool isPresentationStateSeries();

    /** checks if the currently selected instance is a presentation state.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return OFTrue if current instance is presentation state, OFFalse otherwise
     */
    OFBool isPresentationState();

// --- <END> ONLY FOR COMPATIBILITY REASONS <END> ---

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
    E_Condition selectInstance(Uint32 idx);

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

    /** returns the Presentation Description of the currently selected instance.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return Presentation Description or NULL if absent or not selected.
     */
    const char *getPresentationDescription();

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
    E_Condition instanceReviewed(const char *studyUID, const char *seriesUID, const char *instanceUID);

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
    E_Condition deleteInstance(const char *studyUID, const char *seriesUID, const char *instanceUID);

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
    E_Condition deleteSeries(const char *studyUID, const char *seriesUID);

    /** deletes the given study from the database. Any of the corresponding DICOM files
     *  residing in the same directory as the index file are also removed.
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  The database is modified - any study, series or instance selection
     *  and the number of studies, series and instances reported will become invalid since
     *  other processes may modify the database before the exclusive lock is granted to this method.
     *  @param studyUID study instance UID of the study to be deleted
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition deleteStudy(const char *studyUID);

    /* here follow the Network interface methods */

    /** starts the network receiver process (Storage Service Class SCP).
     *  The receiver process will wait for incoming DICOM associations, receive images
     *  and presentation states. Data will be stored in file and registered in the
     *  database index file. Attention: Successful return of this method is no guarantee
     *  that the network receiver has successfully started, because certain errors
     *  (i.e. incorrect settings in the config file) will only be noted in the network
     *  receiver process when running. On Unix platform, successful return of this method
     *  means that the fork() used to start the receiver was successful.
     *  On Win32 platforms, it means that the CreateProcess() call was successful.
     *  @return EC_Normal if the receiver process could be started, an error code otherwise.
     */
    E_Condition startReceiver();

    /** terminates the network receiver process (Storage Service Class SCP).
     *  This method attempts to terminate the network receiver process by
     *  requesting a DICOM association with it and delivering a special "shutdown" command.
     *  If for some reason the network receiver cannot be found (i.e. because it has
     *  terminated abnormally), a TCP/IP timeout (several seconds) may occur before this method returns.
     */
    E_Condition terminateReceiver();

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
    E_Condition sendStudy(const char *targetID, const char *studyUID)
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
    E_Condition sendSeries(const char *targetID, const char *studyUID, const char *seriesUID)
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
    E_Condition sendIOD(const char *targetID, const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** creates a dump of the contents of a DICOM files and displays it on-screen.
     *  A separate application or process is launched to handle the dump and display.
     *  This call returns when the dump operation has successfully been launched.
     *  No information about the status or success of the process itself is being made
     *  available.
     *  This method does not acquire a database lock.
     *  @param filename path of file to be displayed.
     *  @return EC_Normal when the process has successfully been launched,
     *     an error condition otherwise.
     */
    E_Condition dumpIOD(const char *filename);

    /** creates a dump of the contents of a DICOM files and displays it on-screen.
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
    E_Condition dumpIOD(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** saves a monochrome bitmap as a DICOM Secondary Capture image.
     *  The bitmap must use one byte per pixel, left to right, top to bottom
     *  order of the pixels. 0 is interpreted as black, 255 as white.
     *  @param filename the file name or path under which the image is saved.
     *  @param pixelData a pointer to the image data. Must contain at least
     *    width*height bytes of data.
     *  @param width the width of the image, must be <= 0xFFFF
     *  @param height the height of the image, must be <= 0xFFFF
     *  @aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @param explicitVR selects the transfer syntax to be written.
     *    True selects Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @param instanceUID optional parameter containing the SOP Instance UID to be written.
     *    This parameter should be omitted unless the SOP Instance UID needs to be controlled
     *    externally.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition saveDICOMImage(
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
     *  @aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition saveDICOMImage(
      const void *pixelData,
      unsigned long width,
      unsigned long height,
      double aspectRatio=1.0);

    /** saves a monochrome bitmap as a DICOM Grayscale Hardcopy image.
     *  The bitmap must use 16 bits per pixel, left to right, top to bottom
     *  order of the pixels. It is assumed that only values 0..4095 are used.
     *  @param filename the file name or path under which the image is saved.
     *  @param pixelData a pointer to the image data. Must contain at least
     *    width*height*2 bytes of data.
     *  @param width the width of the image, must be <= 0xFFFF
     *  @param height the height of the image, must be <= 0xFFFF
     *  @aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @param explicitVR selects the transfer syntax to be written.
     *    True selects Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @param instanceUID optional parameter containing the SOP Instance UID to be written.
     *    This parameter should be omitted unless the SOP Instance UID needs to be controlled
     *    externally.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition saveGrayscaleHardcopyImage(
      const char *filename,
      const void *pixelData,
      unsigned long width,
      unsigned long height,
      double aspectRatio=1.0,
      OFBool explicitVR=OFTrue,
      const char *instanceUID=NULL);

    /** saves a monochrome bitmap as a DICOM Grayscale Hardcopy image
     *  in the same directory in which the database index file resides.
     *  The filename is generated automatically.
     *  When the image is stored successfully, the database index is updated
     *  to include the new object.
     *  This method releases under any circumstances the database lock if it exists.
     *  @param pixelData a pointer to the image data. Must contain at least
     *    width*height*2 bytes of data.
     *  @param width the width of the image, must be <= 0xFFFF
     *  @param height the height of the image, must be <= 0xFFFF
     *  @aspectRatio the pixel aspect ratio as width/height. If omitted, a pixel
     *    aspect ratio of 1/1 is assumed.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition saveGrayscaleHardcopyImage(
      const void *pixelData,
      unsigned long width,
      unsigned long height,
      double aspectRatio=1.0);

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
    E_Condition saveStoredPrint(
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
    E_Condition saveStoredPrint(OFBool writeRequestedImageSize);

    /** stores the current presentation state in a temporary place
     *  and creates a new presentation state that corresponds with an
     *  image displayed "without" presentation state.
     *  If called twice, an error code is returned.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition disablePState();

    /** restores the stored presentation state (see disablePresentationState)
     *  and deletes the temporary presentation state.
     *  If no stored presentation state exists, returns an error.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition enablePState();

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
    E_Condition selectPState(Uint32 idx, OFBool changeStatus = OFFalse);

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

// --- <BEGIN> ONLY FOR COMPATIBILITY REASONS <BEGIN> ---

    /** checks whether Barten correction is possible (in principle),
     *  i.e. a valid monitor characteristics description exists
     *  and current system is a low-cost system (without built-in
     *  calibration).
     *  @return OFTrue if Barten transform is possible, OFFalse otherwise
     */
    OFBool isBartenTransformPossible();

// --- <END> ONLY FOR COMPATIBILITY REASONS <END> ---

    /** sets ambient light value for the display transformation.
     *  @param value ambient light value to be set
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition setAmbientLightValue(double value);

    /** returns ambient light value for the display transformation.
     *  @param value returned ambient light value
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition getAmbientLightValue(double &value);

    /* print related methods */

    /** selects the current printer. No further
     *  adjustments are made within the Stored Print object.
     *  @param targetID one of the printer target IDs returned by getTargetID().
     *  @return EC_Normal if successful, an error code otherwise.
     */
    E_Condition setCurrentPrinter(const char *targetID);

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
    E_Condition setPrinterMediumType(const char *value);

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
    E_Condition setPrinterFilmDestination(const char *value);

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
    E_Condition setPrinterFilmSessionLabel(const char *value);

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
    E_Condition setPrinterPriority(const char *value);

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
    E_Condition setPrinterOwnerID(const char *value);

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
    E_Condition setPrinterNumberOfCopies(unsigned long value);

    /** gets the (optional) print number of copies.
     *  @return print number of copies, 0 if absent.
     */
    unsigned long getPrinterNumberOfCopies();

    /* ----- for compatibility reasons only - use methods in class DVPSStoredPrint instead ---- */

    /** sets the illumination to be used
     *  with the print Presentation LUT SOP Class.
     *  @param value new attribute value, in cd/m2.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    E_Condition setPrintIllumination(Uint16 value);

    /** gets the current illumination setting
     *  used with the print Presentation LUT SOP Class.
     *  @return illumination in cd/m2
     */
    Uint16 getPrintIllumination();

    /** sets the reflected ambient light to be used
     *  with the print Presentation LUT SOP Class.
     *  @param value new attribute value, in cd/m2.
     *    The caller is responsible for making sure
     *    that the value is valid for the selected printer.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    E_Condition setPrintReflectedAmbientLight(Uint16 value);

    /** gets the current reflected ambient light setting
     *  used with the print Presentation LUT SOP Class.
     *  @return reflected ambient light in cd/m2
     */
    Uint16 getPrintReflectedAmbientLight();

    /* ----- end compatibility reasons only ---- */

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
    E_Condition selectDisplayPresentationLUT(const char *lutID);

    /** if the Presentation State contains an active
     *  Presentation LUT that was set with selectPrintPresentationLUT(),
     *  return the corresponding LUT identifier.
     *  @return lutID if found, NULL or empty string otherwise.
     */
    const char *getDisplayPresentationLUTID();

    /** start spooling of print job with current settings.
     *  @param deletePrintedImages if true, delete printed images from queue.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    E_Condition spoolPrintJob(OFBool deletePrintedImages=OFTrue);

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
    E_Condition startPrintSpooler();

    /** terminates the print spooler process. This method creates a "dummy"
     *  print job that request the print spooler to shutdown as soon as all other pending
     *  print jobs are finished.
     *  @return EC_Normal if the spooler process dummy print job could be written,
     *    an error code otherwise.
     */
    E_Condition terminatePrintSpooler();

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
    E_Condition addToPrintHardcopyFromDB(const char *studyUID, const char *seriesUID, const char *instanceUID);

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
    E_Condition spoolStoredPrintFromDB(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** Initiates the creation of a DICOM Basic Film Session SOP Instance in the printer.
     *  This method stores all Basic Film Session related attributes that are managed by this object
     *  in a DICOM dataset and passes the result to the embedded Stored Print object which manages
     *  the further communication.
     *  @param printHandler print communication handler, association must be open.
     *  @param plutInSession true if printer expects referenced presentation LUT sequence, illumination
     *    and reflected ambient light in basic film session, false if it expects them in basic film box.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition printSCUcreateBasicFilmSession(DVPSPrintMessageHandler& printHandler, OFBool plutInSession);

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
    
    /** sets a new log stream
     *  @param o new log stream, must not be NULL
     */
    void setLog(ostream *o);

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
     *  @image image file
     *  @state presentation state if newState was not created from image.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition exchangeImageAndPState(DVPresentationState *newState, DcmFileFormat *image, DcmFileFormat *state=NULL);

    /** creates a database handle if none exists yet (this method may
     *  be called multiple times without interference) and puts a shared lock
     *  on the database.
     *  The lock will remain until explicitly released with releaseDatabase();
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition lockDatabase();

    /** creates an exlusive lock on the database if none exists.
     *  The lock will remain until explicitly released with releaseDatabase()
     *  or unlockExclusive().
     *  This method also clears the index cache.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition lockExclusive();

    /** removes an exlusive lock on the database if any.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition unlockExclusive();

    /** creates a new pair of filenames for print job creation.
     *  @param printer printer identifier
     *  @param tempname temporary name is returned here
     *  @param jobname print job name is returned here
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition createPrintJobFilenames(const char *printer, OFString& tempname, OFString& jobname);

    /* member variables */

    /** pointer to the current print handler object
     */
    DVPSStoredPrint *pPrint;

    /** pointer to the current presentation state object
     */
    DVPresentationState *pState;

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
    DB_Private_Handle *pHandle;

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
    E_Condition instanceReviewed(int pos);

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


/*
 *  CVS/RCS Log:
 *  $Log: dviface.h,v $
 *  Revision 1.58  2000-03-03 14:13:54  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.57  1999/11/03 13:05:31  meichel
 *  Added support for transmitting annotations in the film session label.
 *    Added support for dump tool launched from DVInterface.
 *
 *  Revision 1.56  1999/10/20 10:47:13  joergr
 *  Added support for a down-scaled preview image of the current DICOM image
 *  (e.g. useful for online-windowing or print preview).
 *
 *  Revision 1.55  1999/10/19 14:46:01  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.54  1999/10/07 17:21:45  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.53  1999/09/27 10:41:52  meichel
 *  Print interface now copies current printer name, avoids JNI problems.
 *
 *  Revision 1.52  1999/09/24 15:24:28  meichel
 *  Added support for CP 173 (Presentation LUT clarifications)
 *
 *  Revision 1.51  1999/09/23 17:37:12  meichel
 *  Added support for Basic Film Session options to dcmpstat print code.
 *
 *  Revision 1.50  1999/09/17 14:33:55  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.49  1999/09/15 17:43:25  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.48  1999/09/13 15:19:08  meichel
 *  Added implementations for a number of further print API methods.
 *
 *  Revision 1.47  1999/09/10 12:46:44  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.46  1999/09/10 09:37:29  joergr
 *  Re-added automatically removed method declarations (maybe a problem with
 *  CVS's merge algorithm).
 *
 *  Revision 1.43  1999/09/08 17:03:01  joergr
 *  Added support for new instance types in database (grayscale hardcopy and
 *  stored print).
 *
 *  Revision 1.42  1999/09/08 16:42:01  meichel
 *  Moved configuration file evaluation to separate class.
 *
 *  Revision 1.41  1999/09/01 16:14:39  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.40  1999/08/31 16:54:44  meichel
 *  Added new sample application that allows to create simple print jobs.
 *
 *  Revision 1.39  1999/08/31 14:02:05  meichel
 *  Added print related config file methods
 *
 *  Revision 1.38  1999/08/27 15:57:55  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.37  1999/08/25 16:47:20  joergr
 *  Moved method 'saveFileFormat()' to public part of the interface class.
 *
 *  Revision 1.36  1999/05/05 14:25:26  joergr
 *  Added optional parameter to method loadPState (from database) to change
 *  instance reviewed flag for pstate and image.
 *
 *  Revision 1.35  1999/04/29 15:25:37  joergr
 *  Added PresentationLabel to index file.
 *
 *  Revision 1.34  1999/04/27 11:23:16  joergr
 *  Enhanced savePState() method: now image file is also added to index file
 *  and stored in image directory (if not already there).
 *  Added new entry to index file: Presentation Description.
 *
 *  Revision 1.33  1999/03/03 13:24:32  joergr
 *  Added methods to get and set ambient light value (re: Barten transformation).
 *  Moved method 'isBartenTransformPossible()' from presentation state class to
 *  interface class.
 *
 *  Revision 1.32  1999/03/02 12:52:51  joergr
 *  Added parameter to selectPState() specifying whether to change the review
 *  status of the loaded presentation state.
 *
 *  Revision 1.31  1999/02/25 18:38:58  joergr
 *  Added some comments.
 *  Renamed methods enable/disablePState().
 *
 *  Revision 1.30  1999/02/24 20:17:48  joergr
 *  Added methods to get a list of presentation states referencing the
 *  currently selected image.
 *  Added support for exchanging current presentation state (load from file)
 *  without deleting the current image.
 *
 *  Revision 1.29  1999/02/22 14:20:20  joergr
 *  Added deletion of image files (depending on directory where the file is
 *  stored).
 *  Modified comments for getGUIConfig... methods to indicate that the
 *  specified key must be upper case.
 *  Reset reference time for file modification checking after the index file
 *  has been changed internally (delete and change status methods).
 *
 *  Revision 1.28  1999/02/19 18:58:11  joergr
 *  Added methods to disable and (re-)enable PresentationStates.
 *  Added (private) helper methods to reduce redundant lines of code.
 *
 *  Revision 1.27  1999/02/19 09:44:17  joergr
 *  Added comments to new database routines and related member variables.
 *
 *  Revision 1.26  1999/02/18 18:46:19  joergr
 *  Re-implemented methods to access index file (delete methods are still
 *  missing).
 *  Removed parameter 'deletefile' from delete methods. This parameter is
 *  not necessary because the decision whether a images file is deleted only
 *  depends on the directory where the file is stored (see comments).
 *
 *  Revision 1.25  1999/02/18 11:07:26  meichel
 *  Added new parameter explicitVR to interface methods savePState,
 *    saveDICOMImage.  Allows to choose between explicit VR and implicit VR
 *    little endian format.  Added new method saveCurrentImage that allows to
 *    save the current image to file.
 *
 *  Revision 1.24  1999/02/17 10:05:30  meichel
 *  Moved creation of Display Function object from DVPresentationState to
 *    DVInterface to avoid unnecessary re-reads.
 *
 *  Revision 1.23  1999/02/16 16:36:10  meichel
 *  Added method newInstancesReceived() to DVInterface class.
 *
 *  Revision 1.22  1999/02/12 10:02:46  vorwerk
 *  added cache , changed deletemethods.
 *
 *  Revision 1.21  1999/02/09 15:58:07  meichel
 *  Implemented methods that save images and presentation states in the DB.
 *
 *  Revision 1.20  1999/02/08 10:52:33  meichel
 *  Updated documentation of dviface.h in Doc++ style.
 *    Removed dummy parameter from constructor.
 *
 *  Revision 1.19  1999/02/05 17:45:35  meichel
 *  Added config file entry for monitor characteristics file.  Monitor charac-
 *    teristics are passed to dcmimage if present to activate Barten transform.
 *
 *  Revision 1.18  1999/02/05 11:38:01  vorwerk
 *  parameter in stripidxarray added.
 *
 *  Revision 1.17  1999/01/29 16:01:05  meichel
 *  Reworked index file handle acquisition and locking code.
 *
 *  Revision 1.16  1999/01/28 15:30:53  vorwerk
 *  New database lock method added.
 *
 *  Revision 1.15  1999/01/27 15:28:34  vorwerk
 *  new method idxfiletest added to handle with indexfiles of length zero.
 *
 *  Revision 1.14  1999/01/27 14:59:28  meichel
 *  Implemented DICOM network receive application "dcmpsrcv" which receives
 *    images and presentation states and stores them in the local database.
 *
 *  Revision 1.13  1999/01/25 18:18:23  meichel
 *  Defined private SOP class UID for network receiver
 *    shutdown function. Cleanup up some code.
 *
 *  Revision 1.12  1999/01/25 16:55:17  vorwerk
 *  function stripidxarray added.
 *
 *  Revision 1.11  1999/01/25 13:05:55  meichel
 *  Implemented DVInterface::startReceiver()
 *    and several config file related methods.
 *
 *  Revision 1.10  1999/01/20 19:25:25  meichel
 *  Implemented sendIOD method which creates a separate process for trans-
 *    mitting images from the local database to a remote communication peer.
 *
 *  Revision 1.9  1999/01/19 15:14:48  vorwerk
 *  Methods for acesseing additional attributes added.
 *
 *  Revision 1.8  1999/01/15 17:27:14  meichel
 *  added DVInterface method resetPresentationState() which allows to reset a
 *    presentation state to the initial state (after loading).
 *
 *  Revision 1.7  1999/01/14 17:50:30  meichel
 *  added new method saveDICOMImage() to class DVInterface.
 *    Allows to store a bitmap as a DICOM image.
 *
 *  Revision 1.6  1999/01/04 13:27:16  vorwerk
 *  line inserted
 *
 *  Revision 1.5  1999/01/04 13:02:26  vorwerk
 *  getSeriesPerformingPhysicainsName() changed in getSeriesPerformingPhysiciansName()
 *
 *  Revision 1.4  1998/12/22 17:57:03  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.3  1998/12/22 15:50:06  vorwerk
 *  - constructor now with parameter for directory of index.dat
 *  - changed const methods in non-const-methods
 *  - added methods for attributes of DICOM-documents
 *  - added private method for operations on index.dat
 *
 *  Revision 1.2  1998/11/30 11:26:59  joergr
 *  Added CVS/RCS log at end of file.
 *
 *
 */

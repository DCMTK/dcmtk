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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-22 14:20:20 $
 *  CVS/RCS Revision: $Revision: 1.29 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

 
#ifndef DVIFACE_H
#define DVIFACE_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */
#include "dctk.h"
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
/* default AETitle for the Presentation State viewer */
#define PSTAT_AETITLE "DCMPSTAT"
/* default path for database folder */
#define PSTAT_DBFOLDER "."
/* private SOP Class UID used to shutdown the Presentation State network receiver */
#define PSTAT_PRIVATESOPCLASSUID "1.2.276.0.7230010.3.4.1915765545.18030.917282194.0"

class DVPSConfig;
class DiDisplayFunction;


/** Interface class for the Softcopy Presentation State viewer.
 *  This class manages the database facilities, allows to start and stop
 *  network transactions and gives access to images and presentation states.
 */
class DVInterface
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
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition loadPState(const char *studyUID, const char *seriesUID, const char *instanceUID);

    /** loads a presentation state and an image (which need not be contained in the database)
     *  and attaches the image to the presentation state.
     *  This method does not acquire a database lock.
     *  @param pstName path and filename of the presentation state to be loaded
     *  @param imgName path and filename of the image to be loaded
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition loadPState(const char *pstName, const char *imgName);
    
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
    DVPresentationState& getCurrentPState();
    
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

    /** checks if the current series consists (only) of Presentation States.
     *  Since DICOM series always contain a single modality only, a series is
     *  either completely a presentation state series or completely different.
     *  May be called only if a valid series selection exists - see selectSeries().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return OFTrue if series contains presentation states, OFFalse otherwise.
     */
    OFBool isPresentationStateSeries();
    
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
    
    /** checks if the currently selected instance is a presentation state.
     *  May be called only if a valid instance selection exists - see selectInstance().
     *  This method acquires a database lock which must be explicitly freed by the user.
     *  @return OFTrue if current instance is presentation state, OFFalse otherwise
     */
    OFBool isPresentationState();
    
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
 
    /* here follow the Config interface methods */

    /** returns the number of communication partners (send targets)
     *  in the configuration file. 
     *  @return number of communication partners
     */
    Uint32 getNumberOfTargets();

    /** returns the target identifier of the communication partner
     *  with the given index. The target identifier is unique within the configuration file 
     *  @param idx index, must be < getNumberOfTargets()
     *  @return target identifier if found, NULL otherwise.
     */
    const char *getTargetID(Uint32 idx);
    
    /** returns the DESCRIPTION entry for the communication partner with the given
     *  index from the configuration file. 
     *  @param idx index, must be < getNumberOfTargets()
     *  @return entry if present in the config file, NULL otherwise.
     */
    const char *getTargetDescription(Uint32 idx);
    
    /** returns the DESCRIPTION entry for the communication partner with the given
     *  target ID from the configuration file. 
     *  @param targetID communication target ID, must be one of the target 
     *    identifiers returned by getTargetID().
     *  @return entry if present in the config file, NULL otherwise.
     */
    const char *getTargetDescription(const char *targetID);

    /** returns the HOSTNAME entry for the communication partner with the given
     *  target ID from the configuration file. 
     *  @param targetID communication target ID, must be one of the target 
     *    identifiers returned by getTargetID().
     *  @return entry if present in the config file, NULL otherwise.
     */
    const char *getTargetHostname(const char *targetID);

    /** returns the PORT entry for the communication partner with the given
     *  target ID from the configuration file. 
     *  @param targetID communication target ID, must be one of the target 
     *    identifiers returned by getTargetID().
     *  @return entry if present and parsable in the config file, 0 otherwise.
     */
    unsigned short getTargetPort(const char *targetID);

    /** returns the HOSTNAME entry for the communication partner with the given
     *  target ID from the configuration file. 
     *  @param targetID communication target ID, must be one of the target 
     *    identifiers returned by getTargetID().
     *  @return entry if present in the config file, NULL otherwise.
     */
    const char *getTargetAETitle(const char *targetID);

    /** returns the MAXPDU entry for the communication partner with the given
     *  target ID from the configuration file. 
     *  @param targetID communication target ID, must be one of the target 
     *    identifiers returned by getTargetID().
     *  @return entry if present and parsable in the config file, 0 otherwise.
     */
    unsigned long getTargetMaxPDU(const char *targetID);

    /** returns the IMPLICITONLY entry for the communication partner with the given
     *  target ID from the configuration file. 
     *  @param targetID communication target ID, must be one of the target 
     *    identifiers returned by getTargetID().
     *  @return entry if present in the config file, OFFalse otherwise.
     */
    OFBool getTargetImplicitOnly(const char *targetID);

    /** returns the DISABLENEWVRS entry for the communication partner with the given
     *  target ID from the configuration file. 
     *  @param targetID communication target ID, must be one of the target 
     *    identifiers returned by getTargetID().
     *  @return entry if present in the config file, OFFalse otherwise.
     */
    OFBool getTargetDisableNewVRs(const char *targetID);

    /** returns the AETitle with which this application should identify itself.
     *  The AETitle is taken from the section GENERAL/NETWORK in the
     *  config file. If absent, a default value is returned.
     *  @return AETitle for this application. Never returns NULL.
     */
    const char *getNetworkAETitle();

    /** returns the IMPLICITONLY entry for the network receiver
     *  from the section GENERAL/NETWORK in the config file.
     *  @return entry if present in the config file, OFFalse otherwise.
     */
    OFBool getNetworkImplicitVROnly();

    /** returns the DISABLENEWVRS entry for the network receiver
     *  from the section GENERAL/NETWORK in the config file.
     *  @return entry if present in the config file, OFFalse otherwise.
     */
    OFBool getNetworkDisableNewVRs();

    /** returns the BITPRESERVINGMODE entry for the network receiver
     *  from the section GENERAL/NETWORK in the config file.
     *  @return entry if present in the config file, OFFalse otherwise.
     */
    OFBool getNetworkBitPreserving();

    /** returns the PORT entry for the network receiver
     *  from the section GENERAL/NETWORK in the config file.
     *  @return entry if present and parsable in the config file, 0 otherwise.
     */
    unsigned short getNetworkPort();

    /** returns the MAXPDU entry for the network receiver
     *  from the section GENERAL/NETWORK in the config file.
     *  @return entry if present and parsable in the config file, 0 otherwise.
     */
    unsigned long getNetworkMaxPDU();
     
    /** returns the database folder to be used for sending/receiving/browsing.
     *  Value is taken from the section GENERAL/DATABASE/DIRECTORY
     *  in the config file. If absent, a default value is returned.
     *  @return database folder path. Never returns NULL.
     */
    const char *getDatabaseFolder();

    /** returns the filename (path) of the DICOM Store SCU application used
     *  for sending images, as configured in section
     *  GENERAL/NETWORK/SENDER in the config file.
     *  @return send application path name or NULL if absent.
     */
    const char *getSenderName();

    /** returns the filename (path) of the DICOM Store SCP application used
     *  for receiving images, as configured in section
     *  GENERAL/NETWORK/RECEIVER in the config file.
     *  @return receive application path name or NULL if absent.
     */
    const char *getReceiverName();
   
    /** returns the filename (path) of the monitor characteristics file
     *  used to implement that Barten transform, as configured in section
     *  GENERAL/MONITOR/CHARACTERISTICS in the config file.
     *  @return monitor characteristics path name or NULL if absent.
     */
    const char *getMonitorCharacteristicsFile();
    
    /** returns the value of configuration file entry key=value
     *  in the section GENERAL/GUI of the config file.
     *  Specified key must be upper case.
     *  If the entry is absent, NULL is returned.
     *  @param key the entry key
     *  @return entry value or NULL.
     */
    const char *getGUIConfigEntry(const char *key);

    /** returns the value of configuration file entry key=value
     *  in the section GENERAL/GUI of the config file.
     *  Specified key must be upper case.
     *  If the entry is absent or cannot be parsed, the default dfl
     *  is returned.
     *  @param key the entry key
     *  @param dfl the default to be used if the value is absent or incorrect
     *  @return entry value or default.
     */
    OFBool getGUIConfigEntryBool(const char *key, OFBool dfl);
    
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

    /** helper function that inserts a new element into a DICOM dataset.
     *  A new DICOM element of the type determined by the tag is created.
     *  The string value (if any) is assigned and the element is inserted
     *  into the dataset. Only tags corresponding to string VRs may be passed.
     *  @param item the dataset into which the new element is inserted
     *  @param tag the tag key of the new DICOM element, must have string VR.
     *  @param value the value to be inserted. If omitted, an empty element is created.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    static E_Condition putStringValue(DcmItem *item, DcmTagKey tag, const char *value=NULL);

    /** helper function that inserts a new element into a DICOM dataset.
     *  A new DICOM element of type "US" is created, the value is assigned 
     *  and the element is inserted into the dataset. 
     *  @param item the dataset into which the new element is inserted
     *  @param tag the tag key of the new DICOM element, must have "US" VR.
     *  @param value the value to be inserted.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    static E_Condition putUint16Value(DcmItem *item, DcmTagKey tag, Uint16 value);

    /** stores the current presentation state in a temporary place
     *  and creates a new presentation state that corresponds with an
     *  image displayed "without" presentation state.
     *  If called twice, an error code is returned.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition disablePresentationState();

    /** restores the stored presentation state (see disablePresentationState)
     *  and deletes the temporary presentation state.
     *  If no stored presentation state exists, returns an error.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition enablePresentationState();
    
private:

    /** private undefined copy constructor
     */
    DVInterface(const DVInterface&);
    
    /** private undefined assignment operator
     */
    DVInterface& operator=(const DVInterface&);

    /** helper function which loads a DICOM file and returns a
     *  pointer to a DcmFileFormat object if loading succeeds.
     *  @param filename name of DICOM file to be loaded
     *  @param fileformat pointer to DcmFileFormat object passed back here
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition loadFileFormat(const char *filename,
                               DcmFileFormat *&fileformat);

    /** helper function which saves a DICOM object to file.
     *  @param filename name of DICOM file to be created
     *  @param fileformat DICOM object to be saved
     *  @param explicitVR selects the transfer syntax to be written. 
     *    True selects Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition saveFileFormat(const char *filename,
                               DcmFileFormat *fileformat, OFBool explicitVR);

    /** helper function that exchanges the current presentation state and image
     *  by the pointers passed and frees the old ones.
     *  @param newState new presentation state, must not be NULL
     *  @image image file
     *  @state presentation state if newState was not created from image.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    E_Condition exchangeImageAndPState(DVPresentationState *newState, DcmFileFormat *image, DcmFileFormat *state=NULL);
 
    /** returns the entry with the given keys.
     *  @param l2_key level 2 key
     *  @param l1_key level 1 key
     *  @param l0_key level 0 key (entry name)
     *  @return value assigned to the key if present, NULL otherwise.
     */
    const char *getConfigEntry(const char *l2_key, const char *l1_key, const char *l0_key);

    /** returns the entry with the given keys as bool.
     *  @param l2_key level 2 key
     *  @param l1_key level 1 key
     *  @param l0_key level 0 key (entry name)
     *  @param deflt default to be returned if entry is not present or syntax error.
     *  @return value assigned to the key if present, default otherwise.
     */
    OFBool getConfigBoolEntry(const char *l2_key, const char *l1_key, const char *l0_key, OFBool deflt);

    /** helper function that cleans up pending processes under Unix.
     *  No function if used on Windows.
     */ 
    static void cleanChildren();

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
    
    /* member variables */
    
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
    
    /** pointer to the configuration file data if the configuration file was found.
     *  NULL otherwise.
     */
    DVPSConfig *pConfig;

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
    
    /** display function object
     */
    DiDisplayFunction *displayFunction;

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

    /* private methods for database */
    
    /** creates index cache to optimize reading of index file
     */
    OFBool createIndexCache();

    /** clears index cache
     */
    void clearIndexCache();

    /** clears specified index record
     */
    void clearIndexRecord(IdxRecord &record,
                          int &recpos);

    /** reads specified index record
     */
    OFBool readIndexRecord(const int pos,
                           IdxRecord &record,
                           int *oldpos = NULL);

    /** updates (hierarchical) status cache
     */
    void updateStatusCache();
    
    /** returns pointer to study struct specified by given UIDs
     */
    DVStudyCache::ItemStruct *getStudyStruct(const char *studyUID,
                                             const char *seriesUID = NULL);

    /** returns pointer to series struct specified by given UIDs
     */
    DVSeriesCache::ItemStruct *getSeriesStruct(const char *studyUID,
                                               const char *seriesUID,
                                               const char *instanceUID = NULL);

    /** returns index of specified study within study description record
     */
    int findStudyIdx(StudyDescRecord *study,
                     const char *uid);

    /** conditionally deletes given image file (only if file resides in index.dat directory)
     */
    int deleteImageFile(const char *filename);

    /** reset index file modification time to reference time (yesterday)
     */
    void resetDatabaseReferenceTime();
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dviface.h,v $
 *  Revision 1.29  1999-02-22 14:20:20  joergr
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

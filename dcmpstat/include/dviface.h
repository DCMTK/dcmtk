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
 *  Last Update:      $Author: vorwerk $
 *  Update Date:      $Date: 1999-01-04 13:02:26 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

 
#ifndef DVIFACE_H
#define DVIFACE_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"
#include "dvpstat.h"  /* for class DVPresentationState */
#include "dbpriv.h"   /* for struct IdxRecord */

class DVInterface
{
 
 public:
 
   /** constructor.
    *  @param indexfolder a string defining the directory for the index.dat file.
    *    The directory must exist.
    */
    DVInterface(const char *indexfolder);

    /** destructor.
     */
    virtual ~DVInterface();
    
    /* load images and presentation states */
    
    /* this method loads an image, creates a default presentation state for the image.
       The options defining how a default pstate is created can be modified with separate methods
       before executing loadImage().
     */
    E_Condition loadImage(const char *studyUID, const char *seriesUID, const char *instanceUID);
    E_Condition loadImage(const char *filename);
    
    /* this method loads a presentation state and the first image referenced by the presentation
       state.
     */
    E_Condition loadPState(const char *studyUID, const char *seriesUID, const char *instanceUID);
    E_Condition loadPState(const char *pstName, const char *imgName = NULL);
    
    /* this method saves the current presentation state as a new presentation state instance. */
    E_Condition savePState();
    E_Condition savePState(const char *filename);
    
    /* resets the interface object to the initial state
       which is identical to the state after the default constructor.
     */
    void clear();
    
    /* returns a reference to the current presentation state. */
    DVPresentationState& getCurrentPState();
    
    /* here follow the Browser interface methods */
    
  
  /** creates an exclusive lock on the database.
       The methods defined below may only be called during a database lock.
       Other processes (i.e. Store SCPs) will be unable to store new images
       in the database as long as it is locked. */
    E_Condition lockDatabase();
    
  /** removes the exclusive database lock. */
    E_Condition unlockDatabase();
  /** returns the number of studies in the database. */  
    Uint32 getNumberOfStudies();
  /** select a study from the range given by the method getNumberOfStudies() */
    E_Condition selectStudy(Uint32 idx);
  /** returns the study UID of a study which is selected by the method 
       selectStudy */
    const char *getStudyUID();
  /** returns the study description of a study which is selected by the method 
       selectStudy */
    const char *getStudyDescription();
  /** returns the DVIFhierarchyStatus of a study which is selected by the 
       method  selectStudy */
    DVIFhierarchyStatus getStudyStatus() ;
    
    /* attributes, which can be examined */
  /** returns the study date of a study which is selected by the method 
       selectStudy */
    const char *getStudyDate();
  /** returns the study time of a study which is selected by the method 
       selectStudy */
    const char *getStudyTime();
  /** returns the referring physicians name of a study which is selected 
       by the method  selectStudy */
    const char *getReferringPhysiciansName();
  /** returns the accession number of a study which is selected by 
       the method  selectStudy */
    const char *getAccessionNumber();
  /** returns the DVIFhierarchyStatus of a study which is selected by 
    the method  selectStudy */
    const char *getNameOfPhysiciansReadingStudy();
 

    /* number of series within the current study */
    Uint32 getNumberOfSeries();
    E_Condition selectSeries(Uint32 idx);
    const char *getSeriesUID();
    DVIFhierarchyStatus getSeriesStatus();


    /* returns OFTrue if this series contains presentation stated, OFFalse otherwise.
       A series can never contain both images and presentation states.
     */
    OFBool isPresentationStateSeries();
    

  /** returns the series number of a series which is selected by 
       the methods  selectStudy and selectSeries */
    const char *getSeriesNumber();
  /** returns the series date of a series which is selected by the methods  
       selectStudy and selectSeries */
    const char *getSeriesDate();
  /** returns the series date of a series which is selected by the methods 
       selectStudy and selectSeries */
    const char *getSeriesTime();
  /** returns the series description of a series which is selected by the 
    methods  selectStudy and selectSeries */
    const char *getSeriesDescription();
  /** returns the series performing physicians name of a series which is 
    selected by the methods  selectStudy and selectSeries */
    const char *getSeriesPerformingPhysiciansName();
  /** returns the series protocol name of a series which is selected by the 
    methods  selectStudy and selectSeries */
    const char *getSeriesProtocolName();
  /** returns series operators name of a series which is selected by the 
    methods selectSeries and selectStudy */
    const char *getSeriesOperatorsName();
    /* number of instances (images or presentation states) within the current study */

  /** returns the number of studies in the database. */  
    Uint32 getNumberOfInstances();

  /** select a study from the range given by the method getNumberOfStudies() */
    E_Condition selectInstance(Uint32 idx);
  /** returns the instance UID of an instance which is selected by the methods
        selectStudy, selectSeries and selectInstance */
    
    const char *getInstanceUID();
  /** returns the modality of an instance which is selected by the methods
        selectStudy, selectSeries and selectInstance */
  
    const char *getModality();
  /** returns the DVIFhierarchyStatus of an instance which is selected by 
       the methods selectStudy, selectSeries and selectInstance */

    DVIFhierarchyStatus getInstanceStatus() ;
    
  /** returns OFTrue if the instance is a presentation state, OFFalse otherwise. */
    OFBool isPresentationState();
    
  /** instance has been reviewed, change status in DB from isNew to isNotNew if necessary */
    E_Condition instanceReviewed(const char *studyUID, const char *seriesUID, const char *instanceUID);
  /** delete instance indentified by studyUID, seriesUID and instanceUID */
    E_Condition deleteInstance(const char *studyUID, const char *seriesUID, const char *instanceUID);
  /** delete series indentified by studyUID and seriesUID */  
    E_Condition deleteSeries(const char *studyUID, const char *seriesUID);
  /** delete study indentified by studyUID*/
    E_Condition deleteStudy(const char *studyUID);
    
    /* here follow the Network interface methods */
    E_Condition sendIOD(const char *targetID, const char *studyUID, const char *seriesUID, const char *instanceUID);
    
    /* here follow the Config interface methods */
    
    /* returns the number of communication partners (send targets).
       This information is read from the config file. */
    Uint32 getNumberOfTargets();
    
    /* returns the target identifier of a communication partner.
       The target identifier is unique within the configuration file 
       idx must be < getNumberOfTargets(). */
    const char *getTargetID(Uint32 idx);
    
    /* returns the target description of a communication partner.
       idx must be < getNumberOfTargets(). */
    const char *getTargetDescription(Uint32 idx);
    
    /* returns the target description of a communication partner.
       targetID must be one of the target identifiers returned
       by getTargetID().
     */
    const char *getTargetDescription(const char *targetID);
    



protected:

    E_Condition loadFileFormat(const char *filename,
                               DcmFileFormat *&fileformat);

    E_Condition saveFileFormat(const char *filename,
                               DcmFileFormat *fileformat);

private:
    /** private undefined copy constructor
     */
    DVInterface(const DVInterface&);
    
    /** private undefined assignment operator
     */
    DVInterface& operator=(const DVInterface&);

    /* member variables */
    DVPresentationState pState;

    /* member variables for database */
    char selectedStudy[65]; /* allow for trailing '\0' */
    char selectedSeries[65];
    char selectedInstance[65];

    Uint32 SeriesNumber, StudyNumber;  
    DB_Private_Handle *phandle;  
    StudyDescRecord *pStudyDesc;
    char IndexName[255];
    long MaxStudyCount;
    long StudySize;
    DB_Handle *handle;
    IdxRecord idxRec;

    /* private methods for database */
  /** method getAnInstance:
       Scans database for an Instance, 
       counts Series and Instances if count is true,
       find an indexed Series or Instance if sel is true,
       compute DVIFhierarchystatus for Study, Series or Instance 
       if dvistatus flag is set
       */
    OFBool getAnInstance(
			 OFBool dvistatus, 
			 OFBool count, 
			 OFBool sel,
			 IdxRecord *idxRec, 
			 const char *StudyUID, 
			 const char *SeriesUID=NULL, 
			 Uint32 selser=0,
			 Uint32 *colser=0 ,
			 OFBool *isNew=NULL, 
			 Uint32 *idxCounter=NULL,
			 const char *InstanceUID=NULL 
		       ) ;

  
};
#endif


/*
 *  $Log: dviface.h,v $
 *  Revision 1.5  1999-01-04 13:02:26  vorwerk
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

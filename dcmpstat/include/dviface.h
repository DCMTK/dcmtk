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
 *  Update Date:      $Date: 1998-11-30 11:26:59 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dctk.h"
#include "dvpstat.h"


class DVInterface
{
 
 public:

    DVInterface();
    DVInterface(const DVInterface& obj);
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
    
    /* This enum describes the status of one entry in the database hierarchy.
       An entry can describe a study, a series or an instance.
       A study or series is new exactly if all subobjects (series and instances) are new.
       A study or series contains new subobjecs as long as any subobject
       (series or instance) has the status objectIsNew.
       Instances can never have the status DVIF_objectContainsNewSubobjects.
     */
    enum DVIFhierarchyStatus
    {
      DVIF_objectIsNotNew,
      DVIF_objectIsNew,
      DVIF_objectContainsNewSubobjects
    };
    
    /* creates an exclusive lock on the database.
       The methods defined below may only be called during a database lock.
       Other processes (i.e. Store SCPs) will be unable to store new images
       in the database as long as it is locked. */
    E_Condition lockDatabase();
    
    /* removes the exclusive database lock. */
    E_Condition unlockDatabase();
    
    Uint32 getNumberOfStudies();
    E_Condition selectStudy(Uint32 idx);
    const char *getStudyUID();
    const char *getStudyDescription();
    DVIFhierarchyStatus getStudyStatus() const;
    
    /* number of series within the current study */
    Uint32 getNumberOfSeries();
    E_Condition selectSeries(Uint32 idx);
    const char *getSeriesUID();
    DVIFhierarchyStatus getSeriesStatus() const;
    /* returns OFTrue if this series contains presentation stated, OFFalse otherwise.
       A series can never contain both images and presentation states.
     */
    OFBool isPresentationStateSeries();
    
    /* number of instances (images or presentation states) within the current study */
    Uint32 getNumberOfInstances();
    E_Condition selectInstance(Uint32 idx);
    const char *getInstanceUID();
    const char *getModality();
    DVIFhierarchyStatus getInstanceStatus() const;
    
    /* returns OFTrue if the instance is a presentation state, OFFalse otherwise. */
    OFBool isPresentationState();
    
    /* instance has been reviewed, change status in DB from isNew to isNotNew if necessary */
    E_Condition instanceReviewed(const char *studyUID, const char *seriesUID, const char *instanceUID);
    
    E_Condition deleteInstance(const char *studyUID, const char *seriesUID, const char *instanceUID);
    E_Condition deleteSeries(const char *studyUID, const char *seriesUID);
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

    /* member variables */
    DVPresentationState pState;
};


/*
 *  $Log: dviface.h,v $
 *  Revision 1.2  1998-11-30 11:26:59  joergr
 *  Added CVS/RCS log at end of file.
 *
 *
 */

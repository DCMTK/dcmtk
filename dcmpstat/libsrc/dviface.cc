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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVPresentationState
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-18 18:48:01 $
 *  CVS/RCS Revision: $Revision: 1.36 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */



#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dviface.h"
#include "ofstring.h"    /* for class OFString */
#include "dvpsconf.h"    /* for class DVPSConfig */
#include "ofbmanip.h"    /* for OFBitmanipTemplate */
#include "oflist.h"      /* for class OFList */
#include "didispfn.h"    /* for DiDisplayFunction */
#include <stdio.h>

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* for fork */
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>    /* for waitpid */
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>    /* for wait3 */
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h> /* for wait3 */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>      /* for execl, fork */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat, fstat */
#endif
#ifdef HAVE_SYS_UTIME_H
#include <sys/utime.h>   /* for utime */
#endif
#ifdef HAVE_UTIME_H
#include <utime.h>   /* for utime */
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
END_EXTERN_C

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>     /* for CreateProcess */
#endif

#define DEFAULT_MAXPDU 16384

/* keywords for configuration file */
#define L2_COMMUNICATION     "COMMUNICATION"
#define L2_GENERAL           "GENERAL"
#define L1_NETWORK           "NETWORK"
#define L1_DATABASE          "DATABASE"
#define L1_GUI               "GUI"
#define L1_MONITOR           "MONITOR"
#define L0_DIRECTORY         "DIRECTORY"
#define L0_HOSTNAME          "HOSTNAME"
#define L0_PORT              "PORT"
#define L0_DESCRIPTION       "DESCRIPTION"
#define L0_AETITLE           "AETITLE"
#define L0_IMPLICITONLY      "IMPLICITONLY"
#define L0_DISABLENEWVRS     "DISABLENEWVRS"
#define L0_MAXPDU            "MAXPDU"
#define L0_SENDER            "SENDER"
#define L0_RECEIVER          "RECEIVER"
#define L0_BITPRESERVINGMODE "BITPRESERVINGMODE"
#define L0_CHARACTERISTICS   "CHARACTERISTICS"


DVInterface::DVInterface(const char *config_file)
: pState(NULL)
, pDicomImage(NULL)
, pDicomPState(NULL)
, pConfig(NULL)
, configPath()
, databaseIndexFile()
, referenceTime(0)
, displayFunction(NULL)
, pHandle(NULL)
, lockingMode(OFFalse)
, idxCache()
, idxRec()
, idxRecPos(-1)
{
    clearIndexRecord();
    if (config_file)
    {
        FILE *cfgfile = fopen(config_file, "rb");
        if (cfgfile)
        {
            pConfig = new DVPSConfig(cfgfile);
            fclose(cfgfile);
        }
    }
    if (config_file) configPath = config_file;

      /* initialize Barten transform */
    const char * displayFunctionFile = getMonitorCharacteristicsFile();
    if (displayFunctionFile && (strlen(displayFunctionFile) > 0))
    {
        DiDisplayFunction *df = new DiDisplayFunction(displayFunctionFile);
        if (df && (df->isValid())) displayFunction = df;
        else
        {
            if (df) delete df;
#ifdef DEBUG
            cerr << "warning: unable to load monitor characterics file '" << displayFunctionFile.c_str() << "', ignoring." << endl;
#endif
        }        
    }

    pState = new DVPresentationState(displayFunction);
  
    /* initialize reference time with "yesterday" */
    referenceTime = (unsigned long)time(NULL);
    if (referenceTime >= 86400) referenceTime -= 86400; // subtract one day
}


DVInterface::~DVInterface()
{
    if (pDicomImage) delete pDicomImage;
    if (pDicomPState) delete pDicomPState;
    if (pState) delete pState;
    if (displayFunction) delete displayFunction;
    if (pConfig) delete pConfig;
    if (pHandle) releaseDatabase();
    // refresh database index file access time
    if (databaseIndexFile.length() > 0) utime(databaseIndexFile.c_str(), NULL);
}


E_Condition DVInterface::loadImage(const char *studyUID,
                                   const char *seriesUID,
                                   const char *instanceUID)
{
    if (lockDatabase())
    {
        const char *filename = getFilename(studyUID, seriesUID, instanceUID);
        if (filename != NULL)
            return loadImage(filename);
    }
    return EC_IllegalCall;
}


E_Condition DVInterface::loadImage(const char *imgName)
{
    E_Condition status = EC_IllegalCall;
    DcmFileFormat *image = NULL;
    DVPresentationState *newState = new DVPresentationState(displayFunction);
    if (newState==NULL) return EC_MemoryExhausted;
    if ((status = loadFileFormat(imgName, image)) == EC_Normal)
    {
        if (image)
        {
            DcmDataset *dataset = image->getDataset();
            if (dataset)
            {
              if (EC_Normal == (status = newState->createFromImage(*dataset)))
              {
                status = newState->attachImage(image, OFFalse);
              }
              if (EC_Normal == status) exchangeImageAndPState(newState, image);
            } else status = EC_CorruptedData;
            if (status != EC_Normal)
            {
              delete newState;
              delete image;
            }
        } else status = EC_IllegalCall;
    }
    return status;
}


E_Condition DVInterface::loadPState(const char *studyUID,
                                    const char *seriesUID,
                                    const char *instanceUID)
{
    // determine the filename of the presentation state
    E_Condition status = lockDatabase();
    if (status != EC_Normal) return status;
    const char *filename = getFilename(studyUID, seriesUID, instanceUID);
    if (status != EC_Normal) return status;
    if (filename==NULL) return EC_IllegalCall;
    
    // load the presentation state
    DcmFileFormat *pstate = NULL;
    DVPresentationState *newState = new DVPresentationState(displayFunction);
    if (newState==NULL) return EC_MemoryExhausted;
    
    if ((EC_Normal == (status = loadFileFormat(filename, pstate)))&&(pstate))
    {
        DcmDataset *dataset = pstate->getDataset();
        if (dataset) status = newState->read(*dataset); else status = EC_CorruptedData;
    } 
    if (status!=EC_Normal)
    {
        delete pstate;
        delete newState;
        return status;
    }
    
    // access the first image reference in the presentation state
    OFString ofstudyUID, ofseriesUID, ofsopclassUID, ofinstanceUID, offrames;
    status = newState->getImageReference(0, ofstudyUID, ofseriesUID, ofsopclassUID, ofinstanceUID, offrames);
    
    // determine the filename of the referenced image
    const char *imagefilename = NULL;
    if (EC_Normal == status)
    { 
        imagefilename = getFilename(ofstudyUID.c_str(), ofseriesUID.c_str(), ofinstanceUID.c_str());
    }
    
    // load the image file and attach it
    DcmFileFormat *fimage = NULL; 
    if (imagefilename && (status == EC_Normal))
    {
        if ((EC_Normal == (status = loadFileFormat(imagefilename, fimage)))&&(fimage))
        {
            DcmDataset *dataset = pstate->getDataset();
            if (dataset)
            {
                status = newState->attachImage(fimage, OFFalse);
                if (EC_Normal == status) exchangeImageAndPState(newState, fimage, pstate);
            } else status = EC_CorruptedData;
        } 
    }
    
    if (status!=EC_Normal)
    {
        delete fimage;
        delete pstate;
        delete newState;
    }
    
    return status;
}


E_Condition DVInterface::loadPState(const char *pstName,
                                    const char *imgName)
{
    E_Condition status = EC_IllegalCall;
    DcmFileFormat *pstate = NULL;
    DcmFileFormat *image = NULL;
    DVPresentationState *newState = new DVPresentationState(displayFunction);
    if (newState==NULL) return EC_MemoryExhausted;
    if (((status = loadFileFormat(pstName, pstate)) == EC_Normal) &&
        ((status = loadFileFormat(imgName, image)) == EC_Normal))
    {
        if ((pstate)&&(image))
        {
            DcmDataset *dataset = pstate->getDataset();
            if (dataset)
            {
              if (EC_Normal == (status = newState->read(*dataset))) 
                status = newState->attachImage(image, OFFalse);
              if (EC_Normal == status) exchangeImageAndPState(newState, image, pstate);
            } else status = EC_CorruptedData;
            if (status != EC_Normal)
            {
              delete newState;
              delete image;
              delete pstate;
            }
        } else status = EC_IllegalCall;
    }
    return status;
}


E_Condition DVInterface::savePState()
{
    // release database lock since we are using the DB module directly
    releaseDatabase();
    
    if (pState==NULL) return EC_IllegalCall;
    
    const char *instanceUID = pState->createInstanceUID();
    if (instanceUID==NULL) return EC_IllegalCall;
    
    DB_Status dbStatus;
    dbStatus.status = STATUS_Success;
    dbStatus.statusDetail = NULL;
    DB_Handle *handle = NULL;
    char imageFileName[MAXPATHLEN+1];
    if (DB_createHandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &handle) != DB_NORMAL) return EC_IllegalCall;
    
    E_Condition result=EC_Normal;
    if (DB_NORMAL == DB_makeNewStoreFileName(handle, UID_GrayscaleSoftcopyPresentationStateStorage, instanceUID, imageFileName))
    {
        // now store presentation state as filename
        result = savePState(imageFileName);
        if (EC_Normal==result)
        {
            if (DB_NORMAL != DB_storeRequest(handle, UID_GrayscaleSoftcopyPresentationStateStorage, 
                instanceUID, imageFileName, &dbStatus))
            {
                result = EC_IllegalCall;
#ifdef DEBUG
                cerr << "unable to register presentation state '" << imageFileName << "' in database." << endl;
                COND_DumpConditions();
#endif         
            }
        }
    }
    DB_destroyHandle(&handle);
    COND_PopCondition(OFTrue); // clear condition stack
    return result;
}


E_Condition DVInterface::savePState(const char *filename, OFBool explicitVR)
{
    if (pState==NULL) return EC_IllegalCall;
    if (filename==NULL) return EC_IllegalCall;
    
    E_Condition status = EC_IllegalCall;
    DcmDataset *dataset = new DcmDataset();
    if (dataset != NULL)
    {
        if ((status = pState->write(*dataset)) == EC_Normal)
        {
          DcmFileFormat *fileformat = new DcmFileFormat(dataset);
          if (fileformat != NULL) 
          {
            status = saveFileFormat(filename, fileformat, explicitVR);
            
            // replace the stored data for resetPresentationState()
            if (pDicomPState) delete pDicomPState;
            pDicomPState = fileformat;
          } else {
            status = EC_MemoryExhausted;
            delete dataset;
          }
        } else delete dataset;
    } else status = EC_MemoryExhausted;
    return status;
}


E_Condition DVInterface::saveCurrentImage(const char *filename, OFBool explicitVR)
{
    if (filename==NULL) return EC_IllegalCall;
    if (pDicomImage==NULL) return EC_IllegalCall;
    return saveFileFormat(filename, pDicomImage, explicitVR);
}


DVPresentationState &DVInterface::getCurrentPState()
{
    return *pState;
}


E_Condition DVInterface::exchangeImageAndPState(DVPresentationState *newState, DcmFileFormat *image, DcmFileFormat *state)
{
    if (newState==NULL) return EC_IllegalCall;
    if (image==NULL) return EC_IllegalCall;
    if (pState) delete pState;
    if (pDicomImage) delete pDicomImage;
    if (pDicomPState) delete pDicomPState;
    pState = newState;
    pDicomImage = image;
    pDicomPState = state;
    return EC_Normal;
}


E_Condition DVInterface::resetPresentationState()
{
    DVPresentationState *newState = new DVPresentationState(displayFunction);
    if (newState==NULL) return EC_MemoryExhausted;        

    E_Condition status = EC_Normal;
    if ((pDicomImage)&&(pDicomPState))
    {
        // both image and presentation state are present
        DcmDataset *dataset = pDicomPState->getDataset();
        if (dataset)
        {
            if (EC_Normal == (status = newState->read(*dataset))) status = newState->attachImage(pDicomImage, OFFalse);
            if (EC_Normal == status)
            {
                if (pState) delete pState;
                pState = newState;
            } 
        } else status = EC_IllegalCall;
    } 
    else if (pDicomImage)
    {
        // only image is present
        DcmDataset *dataset = pDicomImage->getDataset();
        if (dataset)
        {
            if (EC_Normal == (status = newState->createFromImage(*dataset))) status = newState->attachImage(pDicomImage, OFFalse);
            if (EC_Normal == status)
            {
                if (pState) delete pState;
                pState = newState;
            } 
        } else status = EC_IllegalCall;
    }
    if (EC_Normal != status) delete newState;
    return status;
}


const char *DVInterface::getFilename(const char *studyUID,
                                     const char *seriesUID,
                                     const char *instanceUID)
{
    if (createIndexCache())
    {
        if (idxCache.isElem(studyUID))
        {
            DVStudyCache::ItemStruct *study = idxCache.getItem();
            if (study->List.isElem(seriesUID))
            {
                DVSeriesCache::ItemStruct *series = study->List.getItem();
                if (series != NULL)
                {
                    if (series->List.isElem(instanceUID))
                        return series->List.getFilename();
                }
            }
        }
    }
    return NULL;
}


E_Condition DVInterface::lockDatabase()
{
    if (pHandle) return EC_Normal; // may be called multiple times  

    DB_Handle *handle = NULL;
    if (DB_createHandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &handle) == DB_NORMAL)
    {
        pHandle = (DB_Private_Handle *) handle;
        lockingMode = OFFalse;
        if (DB_lock(pHandle, OFFalse) == DB_NORMAL)
            return EC_Normal;
    }
    return EC_IllegalCall;
}


E_Condition DVInterface::lockExclusive()
{
    if (pHandle && lockingMode) return EC_Normal;

    E_Condition result = EC_Normal;
    if (pHandle == NULL)
        result = lockDatabase();
    if (EC_Normal == result)
    {
        // we now have a shared lock.
        DB_unlock(pHandle);
        if (DB_lock(pHandle, OFTrue) == DB_NORMAL)
            lockingMode = OFTrue;
        else
            result = EC_IllegalCall;
    }
    return result;
}


E_Condition DVInterface::unlockExclusive()
{
    if (pHandle && lockingMode)
    {
        lockingMode=OFFalse;
        DB_unlock(pHandle);
        clearIndexCache();
        if (DB_lock(pHandle, OFFalse) != DB_NORMAL)
            return EC_IllegalCall; 
    }
    return EC_Normal;
}


E_Condition DVInterface::releaseDatabase()
{
    if (pHandle == NULL) return EC_Normal;
  
    if (DB_unlock(pHandle) == DB_NORMAL)
    {
        DB_destroyHandle((DB_Handle **)(&pHandle));
        pHandle=NULL;
        clearIndexCache();
        return EC_Normal;
    }
    return EC_IllegalCall;
}


OFBool DVInterface::newInstancesReceived()
{
  if (databaseIndexFile.length() == 0)
  {
    if (pHandle == NULL)
    {
      lockDatabase(); // derives databaseIndexFile
      releaseDatabase();
    }
  }
  
  if (databaseIndexFile.length() > 0)
  {
    struct stat stat_buf;
    if (0== stat(databaseIndexFile.c_str(), &stat_buf))
    {
      if (((unsigned long)stat_buf.st_mtime) == referenceTime) return OFFalse;
    }
    
    // set index file modification time to "yesterday" to make sure
    // we notice any change even if different processes have minor
    // date/time differences (i.e. over NFS)
    struct utimbuf utime_buf;
    utime_buf.actime  = (time_t) referenceTime; 
    utime_buf.modtime = (time_t) referenceTime;
    if (0 != utime(databaseIndexFile.c_str(), &utime_buf))
    {
#ifdef DEBUG
      cerr << "warning: cannot set database index file modification time" << endl;
#endif
    } else {
      if (0 == stat(databaseIndexFile.c_str(), &stat_buf))
      {
        referenceTime = (unsigned long) stat_buf.st_mtime;
      }
    }
  }
  return OFTrue; // default
}


void DVInterface::clearIndexCache()
{
    idxCache.clear();
    clearIndexRecord();
}


OFBool DVInterface::createIndexCache()
{
    if (lockDatabase() == EC_Normal)
    {
        if (idxCache.empty())
        {
cerr << "creating index cache ... " << endl;
            int counter = 0;
            DB_IdxInitLoop(pHandle, &counter);
            IdxRecord record;
            while (DB_IdxGetNext(pHandle, &counter, &record) == DB_NORMAL)
            {
                if (!idxCache.isElem(record.StudyInstanceUID))
                    idxCache.addItem(record.StudyInstanceUID);
                DVStudyCache::ItemStruct *study = idxCache.getItem();
                if (study != NULL)
                {
                    if (!study->List.isElem(record.SeriesInstanceUID))
                        study->List.addItem(record.SeriesInstanceUID);
                    DVSeriesCache::ItemStruct *series = study->List.getItem();
                    if (series != NULL)
                    {
                        if (!series->List.isElem(record.SOPInstanceUID))
                        {
                            const OFBool pstate = (record.Modality != NULL) && (strcmp(record.Modality, "PR") == 0);
                            series->List.addItem(record.SOPInstanceUID, counter, record.hstat, pstate, record.filename);
                            if (pstate)
                                series->PState = OFTrue;                // series contains PState(s)
                        }
                    }
                }
            }
            updateStatusCache();
        }
        return OFTrue;
    }
    return OFFalse;
}


void DVInterface::clearIndexRecord()
{
    OFBitmanipTemplate<Uint8>::zeroMem((Uint8 *)&idxRec, sizeof(idxRec));
    idxRecPos = -1;
}


OFBool DVInterface::readIndexRecord(const int pos,
                                    IdxRecord &record,
                                    int *oldpos)
{
    if (lockDatabase() == EC_Normal)
    {
        if ((oldpos != NULL) && (pos == *oldpos))                      // record already read
            return OFTrue;
        if (DB_IdxRead(pHandle, pos, &record) == DB_NORMAL)
        {
            if (oldpos != NULL)
                *oldpos = pos;
            return OFTrue;
        }
    }
    return OFFalse;
}


void DVInterface::updateStatusCache()
{
    idxCache.updateStatus();
}


Uint32 DVInterface::getNumberOfStudies()
{
    if (createIndexCache())
        return idxCache.getCount();
    return 0;
}


Uint32 DVInterface::getNumberOfSeries()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
        return study->List.getCount();
    return 0;
}


Uint32 DVInterface::getNumberOfInstances()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
    {
        DVSeriesCache::ItemStruct *series = study->List.getItem();
        if (series != NULL)
            return series->List.getCount();
    }
    return 0;
}


E_Condition DVInterface::selectStudy(Uint32 idx)
{
    if (createIndexCache())
    {
        if (idxCache.gotoItem(idx))
        {
            DVStudyCache::ItemStruct *study = idxCache.getItem();
            if (study->List.gotoItem(0))
            {
                DVSeriesCache::ItemStruct *series = study->List.getItem();
                if (series != NULL)
                {
                    if (series->List.gotoItem(0))
                    {
                        if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                            return EC_Normal;
                    }
                }
            }
        }
    }
    return EC_IllegalCall;
}


E_Condition DVInterface::selectSeries(Uint32 idx)
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
    {
        if (study->List.gotoItem(idx))
        {
            DVSeriesCache::ItemStruct *series = study->List.getItem();
            if (series != NULL)
            {
                if (series->List.gotoItem(0))
                {
                    if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                        return EC_Normal;
                }
            }
        }
    }
    return EC_IllegalCall;
}


E_Condition DVInterface::selectInstance(Uint32 idx)
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
    {
        DVSeriesCache::ItemStruct *series = study->List.getItem();
        if (series != NULL)
        {
            if (series->List.gotoItem(idx))
            {
                if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                    return EC_Normal;
            }
        }
    }
    return EC_Normal;
}


DVIFhierarchyStatus DVInterface::getStudyStatus()
{
    return idxCache.getStatus();
}


DVIFhierarchyStatus DVInterface::getSeriesStatus()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
        return study->List.getStatus();
    return DVIF_objectIsNew;
}


DVIFhierarchyStatus DVInterface::getInstanceStatus()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
    {
        DVSeriesCache::ItemStruct *series = study->List.getItem();
        if (series != NULL)
            return series->List.getStatus();
    }
    return DVIF_objectIsNew;
}


OFBool DVInterface::isPresentationStateSeries()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
    {
        DVSeriesCache::ItemStruct *series = study->List.getItem();
        if (series != NULL)
            return series->PState;
    }
    return OFFalse;
}


const char *DVInterface::getStudyUID()
{
    return idxCache.getUID();
}


const char *DVInterface::getSeriesUID()
{
    return idxRec.SeriesInstanceUID;
}


const char *DVInterface::getInstanceUID()
{
    return idxRec.SOPInstanceUID;
}


const char *DVInterface::getStudyDescription()
{ 
    return idxRec.StudyDescription;
}


const char *DVInterface::getStudyDate()
{
    return idxRec.StudyDate;
}

const char *DVInterface::getStudyTime()
{
    return idxRec.StudyTime;
}

const char *DVInterface::getReferringPhysiciansName()
{
    return idxRec.ReferringPhysiciansName;
}

const char *DVInterface::getAccessionNumber()
{
    return idxRec.AccessionNumber;
}

const char *DVInterface::getNameOfPhysiciansReadingStudy()
{
    return idxRec.NameOfPhysiciansReadingStudy;
}

const char *DVInterface::getPatientName()
{
    return idxRec.PatientsName;
}

const char *DVInterface::getPatientID()
{
    return idxRec.PatientID;
}

const char *DVInterface::getPatientBirthDate()
{
    return idxRec.PatientsBirthDate;
}

const char *DVInterface::getPatientSex()
{
    return idxRec.PatientsSex;
}

const char *DVInterface::getPatientBirthTime()
{
    return idxRec.PatientsBirthTime;
}

const char *DVInterface::getOtherPatientNames()
{
    return idxRec.OtherPatientNames;
}

const char *DVInterface::getOtherPatientID()
{
    return idxRec.OtherPatientIDs;
}

const char *DVInterface::getEthnicGroup()
{
    return idxRec.EthnicGroup;
}

const char *DVInterface::getSeriesDescription()
{ 
    return idxRec.SeriesDescription;
}

const char *DVInterface::getSeriesNumber()
{
    return idxRec.SeriesNumber;
}

const char *DVInterface::getSeriesDate()
{
    return idxRec.SeriesDate;
}

const char *DVInterface::getSeriesTime()
{
    return idxRec.SeriesTime;
}

const char *DVInterface::getSeriesPerformingPhysiciansName()
{
    return idxRec.PerformingPhysiciansName;
}

const char *DVInterface::getSeriesProtocolName()
{
    return idxRec.ProtocolName;
}

const char *DVInterface::getSeriesOperatorsName()
{
    return idxRec.OperatorsName;
}


const char *DVInterface::getModality()
{ 
    return idxRec.Modality;
}


const char *DVInterface::getImageNumber()
{
    return idxRec.ImageNumber;
}


OFBool DVInterface::isPresentationState()
{
    const char *str = getModality();
    return ((str != NULL) && (strcmp(str, "PR") == 0));
}


E_Condition DVInterface::instanceReviewed(const char *studyUID,
                                          const char *seriesUID,
                                          const char *instanceUID)
{
    if (createIndexCache())
    {
        if (idxCache.isElem(studyUID))
        {
            DVStudyCache::ItemStruct *study = idxCache.getItem();
            if (study->List.isElem(seriesUID))
            {
                DVSeriesCache::ItemStruct *series = study->List.getItem();
                if (series != NULL)
                {
                    if (series->List.isElem(instanceUID))
                    {
                        int pos;
                        IdxRecord record;
                        if (readIndexRecord(series->List.getPos(), record, &pos))
                        {
                            if (lockExclusive() == EC_Normal)
                            {
                                record.hstat = DVIF_objectIsNotNew;
                                lseek(pHandle->pidx, (long)(SIZEOF_STUDYDESC + pos * SIZEOF_IDXRECORD), SEEK_SET);
                                write(pHandle->pidx, (char *)&record, SIZEOF_IDXRECORD);
                                lseek(pHandle->pidx, 0L, SEEK_SET);
                            }
                            unlockExclusive();
                            return EC_Normal;
                        }
                    }
                }
            }
        }
    }
    return EC_IllegalCall;
}


E_Condition DVInterface::deleteStudy(const char *studyUID)
{
    return EC_IllegalCall;
}

 
E_Condition DVInterface::deleteSeries(const char *studyUID,
                                      const char *seriesUID)
{
    return EC_IllegalCall;
}



E_Condition DVInterface::deleteInstance(const char *studyUID,
                                        const char *seriesUID,
                                        const char *instanceUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::sendIOD(const char * targetID,
                                 const char * studyUID,
                                 const char * seriesUID,
                                 const char * instanceUID)
{
  if ((targetID==NULL)||(studyUID==NULL)) return EC_IllegalCall;
  const char *sender_application = getSenderName();
  if (sender_application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;
  
  cleanChildren(); // clean up old child processes before creating new ones
  
#ifdef HAVE_FORK
  // Unix version - call fork() and execl()
  pid_t pid = fork();
  if (pid < 0)
  {
    // fork failed - return error code
    return EC_IllegalCall;
  } else if (pid > 0)
  {
    // we are the parent process
    return EC_Normal;
  } else {
    // we are the child process
    if (execl(sender_application, sender_application, configPath.c_str(),
            targetID, studyUID, seriesUID, instanceUID, NULL) < 0)
    {
      cerr << "error: unable to execute '" << sender_application << "'" << endl;
    }
    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  // Windows version - call CreateProcess()

  // initialize startup info
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;  
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);
  char commandline[4096];
  if (seriesUID && instanceUID) sprintf(commandline, "%s %s %s %s %s %s", sender_application, configPath.c_str(),
	  targetID, studyUID, seriesUID, instanceUID);
  else if (seriesUID) sprintf(commandline, "%s %s %s %s %s", sender_application, configPath.c_str(), targetID, 
	  studyUID, seriesUID);
  else sprintf(commandline, "%s %s %s %s", sender_application, configPath.c_str(), targetID, studyUID);
  
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
  {
    return EC_Normal;
  } else {
      cerr << "error: unable to execute '" << sender_application << "'" << endl;
  }
 
#endif  
  return EC_IllegalCall; 
}


E_Condition DVInterface::startReceiver()
{
  const char *receiver_application = getReceiverName();
  if (receiver_application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;
  
  cleanChildren(); // clean up old child processes before creating new ones
  
#ifdef HAVE_FORK
  // Unix version - call fork() and execl()
  pid_t pid = fork();
  if (pid < 0)
  {
    // fork failed - return error code
    return EC_IllegalCall;
  } else if (pid > 0)
  {
    // we are the parent process
    return EC_Normal;
  } else {
    // we are the child process
    if (execl(receiver_application, receiver_application, configPath.c_str(), NULL) < 0)
    {
      cerr << "error: unable to execute '" << receiver_application << "'" << endl;
    }
    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  // Windows version - call CreateProcess()
  // initialize startup info
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;  
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);
  char commandline[4096];
  sprintf(commandline, "%s %s", receiver_application, configPath.c_str());
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
  {
    return EC_Normal;
  } else {
      cerr << "error: unable to execute '" << receiver_application << "'" << endl;
  }
#endif  
  return EC_IllegalCall; 
}

E_Condition DVInterface::terminateReceiver()
{
#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  /* we need at least version 1.1 */
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData);
#endif

  E_Condition result = EC_Normal;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc=NULL;

  CONDITION cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net);
  if (SUCCESS(cond))
  {
    cond = ASC_createAssociationParameters(&params, DEFAULT_MAXPDU);
    if (SUCCESS(cond))
    {
      ASC_setAPTitles(params, getNetworkAETitle(), getNetworkAETitle(), NULL);
      gethostname(localHost, sizeof(localHost) - 1);
      sprintf(peerHost, "localhost:%d", (int)getNetworkPort());
      ASC_setPresentationAddresses(params, localHost, peerHost);

      const char* transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax };
      cond = ASC_addPresentationContext(params, 1, PSTAT_PRIVATESOPCLASSUID,transferSyntaxes, 1);
      if (SUCCESS(cond))
      {
    
        cond = ASC_requestAssociation(net, params, &assoc); 
        if (cond==ASC_NORMAL) ASC_abortAssociation(assoc); // tear down association if necessary
        ASC_dropAssociation(assoc);
        ASC_destroyAssociation(&assoc);
      }
    } else result = EC_IllegalCall;
    ASC_dropNetwork(&net);
  } else result = EC_IllegalCall;
  COND_PopCondition(OFTrue); // clear condition stack

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif
  
  return result;
}


Uint32 DVInterface::getNumberOfTargets()
{
  Uint32 result = 0;
  if (pConfig)
  {
    pConfig->set_section(2, L2_COMMUNICATION);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while (pConfig->section_valid(1))
       {
         result++;
         pConfig->next_section(1);
       }
    }
  }
  return result;
}


const char *DVInterface::getTargetID(Uint32 idx)
{
  const char *result=NULL; 
  if (pConfig)
  {
    pConfig->set_section(2, L2_COMMUNICATION);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while ((idx>0)&&(pConfig->section_valid(1)))
       {
         idx--;
         pConfig->next_section(1);
       }
       if (pConfig->section_valid(1)) result = pConfig->get_keyword(1);
    }
  }
  return result;
}


const char *DVInterface::getTargetDescription(Uint32 idx)
{
  const char *result=NULL; 
  if (pConfig)
  {
    pConfig->set_section(2, L2_COMMUNICATION);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while ((idx>0)&&(pConfig->section_valid(1)))
       {
         idx--;
         pConfig->next_section(1);
       }
       if (pConfig->section_valid(1)) result = pConfig->get_entry(L0_DESCRIPTION);
    }
  }
  return result;
}


const char *DVInterface::getConfigEntry(const char *l2_key, const char *l1_key, const char *l0_key)
{
  const char *result=NULL; 
  if (l2_key && l1_key && l0_key && pConfig)
  {
    pConfig->select_section(l1_key, l2_key);
    if (pConfig->section_valid(1)) result = pConfig->get_entry(l0_key);
  }
  return result;
}

OFBool DVInterface::getConfigBoolEntry(const char *l2_key, const char *l1_key, const char *l0_key, OFBool deflt)
{
  OFBool result=deflt; 
  if (l2_key && l1_key && l0_key && pConfig)
  {
    pConfig->select_section(l1_key, l2_key);
    if (pConfig->section_valid(1)) 
    {
      pConfig->set_section(0,l0_key);
      result = pConfig->get_bool_value(deflt);
    }
  }
  return result;
}

const char *DVInterface::getTargetDescription(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_DESCRIPTION);
}

const char *DVInterface::getTargetHostname(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_HOSTNAME);
}

unsigned short DVInterface::getTargetPort(const char *targetID)
{
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_PORT);
  unsigned short result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%hu", &result)) result=0;
  }
  return result;
}

const char *DVInterface::getTargetAETitle(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_AETITLE);
}

unsigned long DVInterface::getTargetMaxPDU(const char *targetID)
{
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_MAXPDU);
  unsigned long result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%lu", &result)) result=0;
  }
  return result;
}

OFBool DVInterface::getTargetImplicitOnly(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_IMPLICITONLY, OFFalse);
}

OFBool DVInterface::getTargetDisableNewVRs(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_DISABLENEWVRS, OFFalse);
}

const char *DVInterface::getNetworkAETitle()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_NETWORK, L0_AETITLE);
  if (result==NULL) result = PSTAT_AETITLE;
  return result;
}


OFBool DVInterface::getNetworkImplicitVROnly()
{
  return getConfigBoolEntry(L2_GENERAL, L1_NETWORK, L0_IMPLICITONLY, OFFalse);
}

OFBool DVInterface::getNetworkDisableNewVRs()
{
  return getConfigBoolEntry(L2_GENERAL, L1_NETWORK, L0_DISABLENEWVRS, OFFalse);
}

OFBool DVInterface::getNetworkBitPreserving()
{
  return getConfigBoolEntry(L2_GENERAL, L1_NETWORK, L0_BITPRESERVINGMODE, OFFalse);
}

unsigned short DVInterface::getNetworkPort()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_NETWORK, L0_PORT);
  unsigned short result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%hu", &result)) result=0;
  }
  return result;
}

unsigned long DVInterface::getNetworkMaxPDU()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_NETWORK, L0_MAXPDU);
  unsigned long result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%lu", &result)) result=0;
  }
  return result;
}


const char *DVInterface::getDatabaseFolder()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_DATABASE, L0_DIRECTORY);
  if (result==NULL) result = PSTAT_DBFOLDER;
  return result;
}

const char *DVInterface::getSenderName()
{
  return getConfigEntry(L2_GENERAL, L1_NETWORK, L0_SENDER);
}

const char *DVInterface::getReceiverName()
{
  return getConfigEntry(L2_GENERAL, L1_NETWORK, L0_RECEIVER);
}

const char *DVInterface::getMonitorCharacteristicsFile()
{
  return getConfigEntry(L2_GENERAL, L1_MONITOR, L0_CHARACTERISTICS);
}

const char *DVInterface::getGUIConfigEntry(const char *key)
{
  return getConfigEntry(L2_GENERAL, L1_GUI, key);
}

OFBool DVInterface::getGUIConfigEntryBool(const char *key, OFBool dfl)
{
    return getConfigBoolEntry(L2_GENERAL, L1_GUI, key, dfl);
}


E_Condition DVInterface::loadFileFormat(const char *filename,
                                        DcmFileFormat *&fileformat)
{
    DcmFileStream stream(filename, DCM_ReadMode);
    if (!stream.Fail())
    {
        fileformat = new DcmFileFormat;
        if (fileformat != NULL)
        {
            E_Condition status;
            fileformat->transferInit();
            if ((status = fileformat->read(stream)) == EC_Normal)
                fileformat->transferEnd();
            return status;
        }
        return EC_MemoryExhausted;
    }
    return stream.GetError();
}


E_Condition DVInterface::saveFileFormat(const char *filename,
                                        DcmFileFormat *fileformat, OFBool explicitVR)
{
    E_TransferSyntax xfer = EXS_LittleEndianImplicit;
    if (explicitVR) xfer = EXS_LittleEndianExplicit;
    
    DcmFileStream stream(filename, DCM_WriteMode);
    if (!stream.Fail())
    {
        if (fileformat != NULL)
        {
            E_Condition status;
            fileformat->transferInit();
            status = fileformat->write(stream, xfer, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding);
            fileformat->transferEnd();
            return status;
        }
        return EC_IllegalCall;
    }
    return stream.GetError();
}


E_Condition DVInterface::putStringValue(DcmItem *item, DcmTagKey tag, const char *value)
{
    E_Condition result = EC_Normal;
    DcmTag localTag(tag);
    if (item)
    {
        DcmElement *elem = NULL;
        switch(localTag.getEVR())
        {
          case EVR_AE: elem = new DcmApplicationEntity(localTag); break;
          case EVR_AS: elem = new DcmAgeString(localTag); break;
          case EVR_CS: elem = new DcmCodeString(localTag); break;
          case EVR_DA: elem = new DcmDate(localTag); break;
          case EVR_DS: elem = new DcmDecimalString(localTag); break;
          case EVR_DT: elem = new DcmDateTime(localTag); break;
          case EVR_IS: elem = new DcmIntegerString(localTag); break;
          case EVR_TM: elem = new DcmTime(localTag); break;
          case EVR_UI: elem = new DcmUniqueIdentifier(localTag); break;
          case EVR_LO: elem = new DcmLongString(localTag); break;
          case EVR_LT: elem = new DcmLongText(localTag); break;
          case EVR_PN: elem = new DcmPersonName(localTag); break;
          case EVR_SH: elem = new DcmShortString(localTag); break;
          case EVR_ST: elem = new DcmShortText(localTag); break;
          default: result=EC_IllegalCall; break;
        }
        if (elem)
        {
          if (value) result = elem->putString(value);
        } else if (result==EC_Normal) result = EC_MemoryExhausted;
        if (EC_Normal==result) item->insert(elem, OFTrue);
    } else result = EC_IllegalCall;
    return result;
}


E_Condition DVInterface::putUint16Value(DcmItem *item, DcmTagKey tag, Uint16 value)
{
    E_Condition result = EC_Normal;
    DcmTag localTag(tag);
    if (item)
    {
        DcmElement *elem = new DcmUnsignedShort(localTag);
        if (elem) result = elem->putUint16(value); else result=EC_MemoryExhausted;
        if (EC_Normal==result) item->insert(elem, OFTrue);
    } else result = EC_IllegalCall;
    return result;
}


E_Condition DVInterface::saveDICOMImage(
  const char *filename, 
  const void *pixelData,
  unsigned long width,
  unsigned long height,
  double aspectRatio,
  OFBool explicitVR,
  const char *instanceUID)
{
    if ((width<1)||(width > 0xFFFF)) return EC_IllegalCall;
    if ((height<1)||(height > 0xFFFF)) return EC_IllegalCall;
    if (pixelData == NULL) return EC_IllegalCall;
    if (filename == NULL) return EC_IllegalCall;
    if (aspectRatio == 0.0) return EC_IllegalCall;
    
    Uint16 columns = (Uint16) width;
    Uint16 rows = (Uint16) height;
    E_Condition status = EC_Normal;
    DcmDataset *dataset = new DcmDataset();
    char newuid[70];
    
    if (dataset)
    {
      if (EC_Normal==status) status = putStringValue(dataset, DCM_PatientName);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_PatientID);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_PatientBirthDate);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_PatientSex);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_StudyDate);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_StudyTime);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_ReferringPhysiciansName);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_StudyID);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_AccessionNumber);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_SeriesNumber);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_ImageNumber);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_PatientOrientation);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_Modality, "OT");
      if (EC_Normal==status) status = putStringValue(dataset, DCM_ConversionType, "WSD");
      if (EC_Normal==status) status = putStringValue(dataset, DCM_PhotometricInterpretation, "MONOCHROME2");
      dcmGenerateUniqueIdentifer(newuid);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_SOPInstanceUID, (instanceUID ? instanceUID : newuid));
      dcmGenerateUniqueIdentifer(newuid);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_SeriesInstanceUID, newuid);
      dcmGenerateUniqueIdentifer(newuid);
      if (EC_Normal==status) status = putStringValue(dataset, DCM_StudyInstanceUID, newuid);
      if (EC_Normal==status) status = putUint16Value(dataset, DCM_SamplesPerPixel, 1);
      if (EC_Normal==status) status = putUint16Value(dataset, DCM_Rows, rows);
      if (EC_Normal==status) status = putUint16Value(dataset, DCM_Columns, columns);
      if (EC_Normal==status) status = putUint16Value(dataset, DCM_BitsAllocated, 8);
      if (EC_Normal==status) status = putUint16Value(dataset, DCM_BitsStored, 8);
      if (EC_Normal==status) status = putUint16Value(dataset, DCM_HighBit, 7);
      if (EC_Normal==status) status = putUint16Value(dataset, DCM_PixelRepresentation, 0);
      if ((EC_Normal==status)&&(aspectRatio != 1.0))
      {
        sprintf(newuid, "%ld\\%ld", 1000L, (long)(aspectRatio*1000.0));
        status = putStringValue(dataset, DCM_PixelAspectRatio, newuid);
      }
      DcmPolymorphOBOW *pxData = new DcmPolymorphOBOW(DCM_PixelData);
      if (pxData)
      {
        status = pxData->putUint8Array((Uint8 *)pixelData, (unsigned long)(width*height));
        if (EC_Normal==status) status = dataset->insert(pxData); else delete pxData;
      } else status = EC_MemoryExhausted;
 
      if (EC_Normal == status)
      {
          DcmFileFormat *fileformat = new DcmFileFormat(dataset);
          if (fileformat) 
          {
            status = saveFileFormat(filename, fileformat, explicitVR);
            delete fileformat;
          } else {
            status = EC_MemoryExhausted;
            delete dataset;
          }
      } else delete dataset;
    } else status = EC_MemoryExhausted;
    return status;
}


E_Condition DVInterface::saveDICOMImage(
  const void *pixelData,
  unsigned long width,
  unsigned long height,
  double aspectRatio)
{
  // release database lock since we are using the DB module directly
  releaseDatabase();

  char uid[100];
  dcmGenerateUniqueIdentifer(uid);
    
  DB_Status dbStatus;
  dbStatus.status = STATUS_Success;
  dbStatus.statusDetail = NULL;
  DB_Handle *handle = NULL;
  char imageFileName[MAXPATHLEN+1];
  if (DB_createHandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &handle) != DB_NORMAL) return EC_IllegalCall;
  
  E_Condition result=EC_Normal;
  if (DB_NORMAL == DB_makeNewStoreFileName(handle, UID_SecondaryCaptureImageStorage, uid, imageFileName))
  {
     // now store presentation state as filename
     result = saveDICOMImage(imageFileName, pixelData, width, height, aspectRatio, OFTrue, uid);
     if (EC_Normal==result)
     {
       if (DB_NORMAL != DB_storeRequest(handle, UID_SecondaryCaptureImageStorage, uid, imageFileName, &dbStatus))
       {
         result = EC_IllegalCall;
#ifdef DEBUG
         cerr << "unable to register secondary capture image '" << imageFileName << "' in database." << endl;
         COND_DumpConditions();
#endif         
       }
     }
  }
  DB_destroyHandle(&handle);
  COND_PopCondition(OFTrue); // clear condition stack
  return result;
}


void DVInterface::cleanChildren()
{
#ifdef HAVE_WAITPID
    int stat_loc;
#elif HAVE_WAIT3
    struct rusage rusage;
#if defined(__NeXT__)
    /* some systems need a union wait as argument to wait3 */
    union wait status;
#else
    int        status;
#endif
#endif

#if defined(HAVE_WAITPID) || defined(HAVE_WAIT3)
    int child = 1;
    int options = WNOHANG;
    while (child > 0)
    {
#ifdef HAVE_WAITPID
	child = (int)(waitpid(-1, &stat_loc, options));
#elif defined(HAVE_WAIT3)
	child = wait3(&status, options, &rusage);
#endif
        if (child < 0)
	{
	   if (errno != ECHILD) cerr << "wait for child failed: " << strerror(errno) << endl;
	}
    }
#endif
}


/*
 *  CVS/RCS Log:
 *  $Log: dviface.cc,v $
 *  Revision 1.36  1999-02-18 18:48:01  joergr
 *  Re-implemented methods to access index file (delete methods are still
 *  missing).
 *  Removed parameter 'deletefile' from delete methods. This parameter is
 *  not necessary because the decision whether a images file is deleted only
 *  depends on the directory where the file is stored (see comments).
 *
 *  Revision 1.35  1999/02/18 11:07:28  meichel
 *  Added new parameter explicitVR to interface methods savePState,
 *    saveDICOMImage.  Allows to choose between explicit VR and implicit VR
 *    little endian format.  Added new method saveCurrentImage that allows to
 *    save the current image to file.
 *
 *  Revision 1.34  1999/02/17 12:46:10  vorwerk
 *  bug fixed in strippidxarray.
 *
 *  Revision 1.33  1999/02/17 10:05:33  meichel
 *  Moved creation of Display Function object from DVPresentationState to
 *    DVInterface to avoid unnecessary re-reads.
 *
 *  Revision 1.32  1999/02/16 16:36:08  meichel
 *  Added method newInstancesReceived() to DVInterface class.
 *
 *  Revision 1.31  1999/02/12 10:04:13  vorwerk
 *  added cache , changed delete methods.
 *
 *  Revision 1.30  1999/02/10 16:01:40  meichel
 *  Fixed memory leak in dviface.cc - Config file contents were never deleted.
 *
 *  Revision 1.29  1999/02/09 15:58:10  meichel
 *  Implemented methods that save images and presentation states in the DB.
 *
 *  Revision 1.28  1999/02/08 10:52:35  meichel
 *  Updated documentation of dviface.h in Doc++ style.
 *    Removed dummy parameter from constructor.
 *
 *  Revision 1.27  1999/02/05 17:45:37  meichel
 *  Added config file entry for monitor characteristics file.  Monitor charac-
 *    teristics are passed to dcmimage if present to activate Barten transform.
 *
 *  Revision 1.26  1999/02/05 12:45:12  vorwerk
 *  actualised version: comments see previous version.
 *
 *  Revision 1.24  1999/01/29 16:01:02  meichel
 *  Reworked index file handle acquisition and locking code.
 *
 *  Revision 1.23  1999/01/29 09:51:28  vorwerk
 *  locking bug removed.
 *
 *  Revision 1.22  1999/01/28 15:27:23  vorwerk
 *  Exclusive and shared locking mechanism in all browser-methods added.
 *
 *  Revision 1.21  1999/01/27 15:31:28  vorwerk
 *  record deletion bug removed. Errorhandling for indexfiles with length zero added.
 *
 *  Revision 1.20  1999/01/27 14:59:26  meichel
 *  Implemented DICOM network receive application "dcmpsrcv" which receives
 *    images and presentation states and stores them in the local database.
 *
 *  Revision 1.19  1999/01/25 18:18:22  meichel
 *  Defined private SOP class UID for network receiver
 *    shutdown function. Cleanup up some code.
 *
 *  Revision 1.18  1999/01/25 16:48:37  vorwerk
 *  Bug in getaninstance removed. getNumberOfSeries and getNumberOfInstances results
 *  are correct now. getStudyStatus bug removed. Error handling routines added.
 *
 *  Revision 1.17  1999/01/25 13:05:57  meichel
 *  Implemented DVInterface::startReceiver()
 *    and several config file related methods.
 *
 *  Revision 1.16  1999/01/20 19:25:30  meichel
 *  Implemented sendIOD method which creates a separate process for trans-
 *    mitting images from the local database to a remote communication peer.
 *
 *  Revision 1.15  1999/01/19 15:13:41  vorwerk
 *  Additional methods for attributes in the indexfile added.
 *  Method getFilename implemented.
 *
 *  Revision 1.14  1999/01/18 17:30:48  meichel
 *  minor syntax purifications to keep VC++ happy
 *
 *  Revision 1.13  1999/01/18 16:15:17  vorwerk
 *  Bug in isPresentationstateSeries() corrected.
 *
 *  Revision 1.12  1999/01/15 17:27:17  meichel
 *  added DVInterface method resetPresentationState() which allows to reset a
 *    presentation state to the initial state (after loading).
 *
 *  Revision 1.11  1999/01/14 17:50:27  meichel
 *  added new method saveDICOMImage() to class DVInterface.
 *    Allows to store a bitmap as a DICOM image.
 *
 *  Revision 1.10  1999/01/14 16:19:46  vorwerk
 *  getNumberOfSeries bug corrected. Error handling in deletion and reviewed methods added.
 *
 *  Revision 1.8  1999/01/11 10:10:18  vorwerk
 *  error handling for getNumberofStudies and selectStudy implemented.
 *
 *  Revision 1.7  1999/01/07 16:40:04  vorwerk
 *  getSeriesDescription implemented
 *
 *  Revision 1.5  1999/01/04 13:28:11  vorwerk
 *  line inserted
 *
 *  Revision 1.4  1999/01/04 13:10:30  vorwerk
 *  getSeriesPerformingPhysicainsName() changed in getSeriesPerformingPhysiciansName()
 *
 *  Revision 1.3  1998/12/22 17:57:13  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/22 15:52:45  vorwerk
 *  - browser methods implemented
 *  - methods added for index.dat
 *
 *  Revision 1.1  1998/11/27 14:50:38  meichel
 *  Initial Release.
 *
 *
 */

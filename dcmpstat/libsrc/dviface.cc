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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DVPresentationState
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-01-29 16:01:02 $
 *  CVS/RCS Revision: $Revision: 1.24 $
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
END_EXTERN_C

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>     /* for CreateProcess */
#endif

#define DEFAULT_MAXPDU 16384

DVInterface::DVInterface(int /* dummy */, const char *config_file)
: pState(NULL)
, pDicomImage(NULL)
, pDicomPState(NULL)
, pConfig(NULL)
, configPath()
, studyidx(0)
, SeriesNumber(0)
, StudyNumber(0)
, phandle(NULL)
, lockingMode(OFFalse)
, pStudyDesc(NULL)
  //, handle(NULL)
, idxRec()
{
  strcpy(selectedStudy,"");
  strcpy(selectedSeries,"");
  strcpy(selectedInstance,"");
  pState = new DVPresentationState();
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
}


DVInterface::~DVInterface()
{
  if (pDicomImage) delete pDicomImage;
  if (pDicomPState) delete pDicomPState;
  if (pState) delete pState;
  if (phandle) releaseDatabase();
}


E_Condition DVInterface::loadImage(const char *studyUID,
                                   const char *seriesUID,
                                   const char *instanceUID)
{
  E_Condition status = lockDatabase();
  if (status != EC_Normal) return status;
  const char *filename = getFilename(studyUID, seriesUID, instanceUID);
  if (filename==NULL) return EC_IllegalCall;
  return loadImage(filename);
}


E_Condition DVInterface::loadImage(const char *imgName)
{
    E_Condition status = EC_IllegalCall;
    DcmFileFormat *image = NULL;
    DVPresentationState *newState = new DVPresentationState();
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
  DVPresentationState *newState = new DVPresentationState();
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
    DVPresentationState *newState = new DVPresentationState();
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
    return EC_IllegalCall;
}


E_Condition DVInterface::savePState(const char *filename)
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
            status = saveFileFormat(filename, fileformat);
            delete fileformat;
          } else {
            status = EC_MemoryExhausted;
            delete dataset;
          }
        } else delete dataset;
    } else status = EC_MemoryExhausted;
    return status;
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
  DVPresentationState *newState = new DVPresentationState();
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

const char *DVInterface::getFilename(const char * studyUID, const char * seriesUID, const char * instanceUID)
{
  if ((studyUID==NULL) || ((seriesUID==NULL) || (instanceUID==NULL))) return NULL;
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID, seriesUID,0,0,NULL,0,instanceUID)==OFFalse){  
  return idxRec.filename;
  } 
  else
	  return NULL;
}


E_Condition DVInterface::lockDatabase()
{
  if (phandle) return EC_Normal; // may be called multiple times
  
  DB_Handle *handle = NULL;
  if (DB_createHandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &handle) != DB_NORMAL) return EC_IllegalCall;
  phandle = (DB_Private_Handle *) handle;
  lockingMode = OFFalse;
  if (DB_lock(phandle, OFFalse) != DB_NORMAL) return EC_IllegalCall;
  return EC_Normal;
}

E_Condition DVInterface::lockExclusive()
{
  if (phandle && lockingMode) return EC_Normal;

  E_Condition result = EC_Normal;
  if (phandle==NULL) result = lockDatabase();
  if (EC_Normal == result)
  {
    // we now have a shared lock.
    DB_unlock(phandle);
    if (DB_lock(phandle, OFTrue) != DB_NORMAL) result=EC_IllegalCall; else lockingMode=OFTrue;
  }
  return result;
}

E_Condition DVInterface::unlockExclusive()
{
  E_Condition result = EC_Normal;
  if (phandle && lockingMode)
  {
    lockingMode=OFFalse;
    DB_unlock(phandle);
    if (DB_lock(phandle, OFFalse) != DB_NORMAL) result=EC_IllegalCall; 
  }
  return result;
}

E_Condition DVInterface::releaseDatabase()
{
  if (phandle == NULL) return EC_Normal;
  
  E_Condition result = EC_Normal;
  if (DB_NORMAL != DB_unlock(phandle)) result=EC_IllegalCall;
  DB_destroyHandle((DB_Handle **)(&phandle));
  phandle=NULL;
  return result;
}


Uint32 DVInterface::getNumberOfStudies()
{
  int i ;
  Uint32 j=0 ;  
  if (lockDatabase() != EC_Normal) return 0;
  if (phandle==NULL) return 0;
  if (idxfiletest()==OFTrue) return 0;
  pStudyDesc = (StudyDescRecord *)malloc (SIZEOF_STUDYDESC) ;
  DB_GetStudyDesc(phandle, pStudyDesc );
  for (i=0; i<phandle->maxStudiesAllowed; i++) {
    if (pStudyDesc[i].NumberofRegistratedImages != 0 ) {  
      j++;
    }
  }
  return j;
}

OFBool DVInterface::idxfiletest()
{
  return OFFalse;
  int curpos, result;
  curpos=lseek(phandle->pidx,0,SEEK_CUR);
  result=lseek(phandle->pidx,0,SEEK_CUR)-lseek(phandle->pidx,0,SEEK_END);
  lseek(phandle->pidx,curpos,SEEK_SET);
  if (result==0) return OFTrue;
  return OFFalse;
}


E_Condition DVInterface::selectStudy(Uint32 idx)
{

  if (idx>getNumberOfStudies()-1) return EC_IllegalCall; 
  if (pStudyDesc==NULL) return EC_IllegalCall;
  if (lockDatabase() != EC_Normal) return EC_IllegalCall;
  if (phandle==NULL) return EC_IllegalCall;

  if (idxfiletest()==OFTrue) return EC_IllegalCall;

  while ((pStudyDesc[idx].StudySize==0) && ( idx < PSTAT_MAXSTUDYCOUNT)) idx++;
  if (idx==PSTAT_MAXSTUDYCOUNT) return EC_IllegalCall;

  if ((pStudyDesc[idx].StudySize!=0) || ( idx < (unsigned)(phandle -> maxStudiesAllowed)+1))
  {
     strcpy(selectedStudy,pStudyDesc[idx].StudyInstanceUID);
  }
  else return EC_IllegalCall;
  studyidx=idx;
  return EC_Normal;
}


const char *DVInterface::getStudyUID()
{
  if (strcmp(selectedStudy,"")==0) return NULL; 
  return selectedStudy;
}


const char *DVInterface::getStudyDescription()
{ 
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy)!=EC_Normal) return NULL;
  return idxRec.StudyDescription;
}


DVIFhierarchyStatus DVInterface::getStudyStatus()
{
  OFBool isNew=OFFalse;
  Uint32 counter=0;
  if (strcmp(selectedStudy,"")==0) return DVIF_objectIsNew; 
  if(getAnInstance(OFTrue,OFTrue,OFFalse,&idxRec, selectedStudy,NULL,0,&counter,&isNew)==OFFalse)
  {
    if (isNew==OFTrue) return DVIF_objectIsNew;
    else if (counter!=getNumberOfInstances()) return DVIF_objectContainsNewSubobjects;
    else return DVIF_objectIsNotNew;
  }
  else
  {
    cerr << "Database Error" << endl;
    return DVIF_objectIsNew;
  }
}

const char *
DVInterface::getStudyDate(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy)==OFTrue) return NULL;
  return idxRec.StudyDate;
}

const char *
DVInterface::getStudyTime(){
  if (strcmp(selectedStudy,"")==0) return NULL;
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy)==OFTrue) return NULL;
  return idxRec.StudyTime;
}

const char *
DVInterface::getReferringPhysiciansName(){
  if (strcmp(selectedStudy,"")==0) return NULL; 

  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy)==OFTrue) return NULL;
  return idxRec.ReferringPhysiciansName;
}

const char *
DVInterface::getAccessionNumber(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy)==OFTrue) return NULL;
  return idxRec.AccessionNumber;
}

const char *
DVInterface::getNameOfPhysiciansReadingStudy(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.NameOfPhysiciansReadingStudy;
}

const char *DVInterface::getPatientName(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.PatientsName;
}

const char *DVInterface::getPatientID(){
if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.PatientID;
}

const char *DVInterface::getPatientBirthDate(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.PatientsBirthDate;
}

const char *DVInterface::getPatientSex(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.PatientsSex;
}

const char *DVInterface::getPatientBirthTime(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.PatientsBirthTime;
}

const char *DVInterface::getOtherPatientNames(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.OtherPatientNames;
}

const char *DVInterface::getOtherPatientID(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.OtherPatientIDs;
}

const char *DVInterface::getEthnicGroup(){
  if (strcmp(selectedStudy,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy)==OFTrue) return NULL;
  return idxRec.EthnicGroup;
}

const char *DVInterface::getSeriesDescription()
{ 
  if (strcmp(selectedSeries,"")==0) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedSeries)==OFTrue) return NULL;
  return idxRec.SeriesDescription;
}


const char *
DVInterface::getSeriesNumber(){
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFTrue) return NULL;
  return idxRec.SeriesNumber;
}

const char *
DVInterface::getSeriesDate(){
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFTrue) return NULL;
  return idxRec.SeriesDate;
}

const char *
DVInterface::getSeriesTime(){
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFTrue) return NULL;
  return idxRec.SeriesTime;
}


const char *
DVInterface::getSeriesPerformingPhysiciansName(){
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFTrue) return NULL;
  return idxRec.PerformingPhysiciansName;
}

const char *
DVInterface::getSeriesProtocolName(){
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFTrue) return NULL;
  return idxRec.ProtocolName;
}

const char *
DVInterface::getSeriesOperatorsName(){
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFTrue) return NULL;
  return idxRec.OperatorsName;
}

/*

getAnInstance:
Scans database for an Instance, 
counts Series and Instances if count is true,
find an indexed Series or Instance if sel is true,
compute DVIFhierarchystatus for Study, Series or Instance 
if dvistatus flag is set
 */

OFBool
DVInterface::getAnInstance(OFBool dvistatus, // indicates search for reviewed instances 
			   OFBool count,  // indicates, that positive compare results were counted
			   OFBool sel, // the method is used to select a series or instance 
			   IdxRecord *anIdxRec, // a Record of the indexfile
			   const char *StudyUID, // StudyUID as string 
			   const char *SeriesUID, // SeriesUID as string, if used
			   Uint32 selser, // selected Series, if used
			   Uint32 *colser , // counts instances and serieses, if used
			   OFBool *isNew,  // reviewstate of an instance, if used
			   Uint32 *idxCounter, // position of pointer in the indexfile
			   const char *InstanceUID // InstanceUID as string, if used
			   )  {
  if (lockDatabase() != EC_Normal) return OFTrue;  
  int recordCounter ; // required for IdxInitLoop, contains index of indexFile
  OFBool study,series,instance; // compareresults
  unsigned int idxlength;// length of positive results array
  int *elems; // positive results array
  Uint32 posResCounter; // index of positive results array
  if (idxfiletest()==OFTrue) return OFTrue;
  if (colser!=NULL)  (*colser)=0; // used counter, if definied 
  pStudyDesc = (StudyDescRecord *)malloc (SIZEOF_STUDYDESC) ;
  if (pStudyDesc == NULL) {
    fprintf(stderr, "DB_PrintIndexFile: out of memory\n");
    return OFTrue;
  }
  
  if (DB_GetStudyDesc(phandle, pStudyDesc )==DB_ERROR){
	  cerr << "study descriptor not available !" << endl;
  return OFTrue;
  ;
  }
  
  // number of registrated images
  idxlength=pStudyDesc[studyidx].NumberofRegistratedImages ;
  if (idxlength==0) return OFTrue;
  elems=new int[idxlength]; 
  DB_IdxInitLoop (phandle, &recordCounter); 
  free (pStudyDesc);
  pStudyDesc=NULL;
  posResCounter=0;

  while (1) {
    study=OFFalse;
    series=OFFalse;
    instance=OFFalse;

	// idxCounter defined ?
    if (idxCounter!=NULL) (*idxCounter)++;

	// end of indexfile reached ?
	if (DB_IdxGetNext (phandle, &recordCounter, anIdxRec) != DB_NORMAL)
	{
		if (isNew!=0) {
		  return OFFalse;
		}
		if (colser==NULL) {
		  return OFTrue;
		}
		else 
		{
			(*colser)=stripidxarray(elems); // removes duplicate series
			return OFFalse;
		}	
	}
    // Tests, if studyUID is definied and if it matched to the studyUID of actual loaded record 
	// in indexfile
    // if studyUID not definied, the result is true to ensure a correct match.
    if (StudyUID!=NULL){
      if (strcmp(StudyUID, (*anIdxRec).StudyInstanceUID)==0) 
		  study=OFTrue;
      else
		if (strcmp(StudyUID, "")==0) 
			study=OFTrue;
		} 
    else
	  study=OFTrue;

	// Tests, if seriesUID is definied and if it matched to the seriesUID of actual loaded record 
	// in indexfile
    // if studyUID not definied, the result is true to ensure a correct match.
    
    if (SeriesUID!=NULL){
      if (strcmp(SeriesUID, (*anIdxRec).SeriesInstanceUID)==0) 
		  series=OFTrue;
      else
		if (strcmp(SeriesUID, "")==0) 
			series=OFTrue;
		}
    else
      series=OFTrue;

	// Tests, if instanceUID is definied and if it matched to the instanceUID of actual loaded record 
	// in indexfile
    // if studyUID not definied, the result is true to ensure a correct match.
    
    if (InstanceUID!=NULL){
      if (strcmp(InstanceUID, (*anIdxRec).SOPInstanceUID)==0) 
		  instance=OFTrue;
      else 
	if (strcmp(InstanceUID, "")==0) 
		instance=OFTrue;
        }
    else
      instance=OFTrue;

	// tests, if parameters matched to the currently loaded record in the
	// indexfile

    if ((study==OFTrue) && ((series==OFTrue) && (instance==OFTrue))){
		// counts Serieses and save their position in the indexfile
		if (posResCounter<idxlength){
			elems[posResCounter]=recordCounter-1;
			posResCounter++;
			elems[posResCounter]=-2;
		}
    // one or more results are needed
    if (count==OFFalse){	
	   break ;
    }
    else
	{
		(*colser)++;
	
		if ((dvistatus==OFTrue) && ((*anIdxRec).hstat==DVIF_objectIsNew)) 
		{
			if (isNew!=NULL) 
				(*isNew)=OFTrue;
			else
			{
			    return OFFalse;
			}
		}
		else
			if (isNew!=NULL) { 
				(*isNew)=OFFalse;
				return OFFalse;
			}
		}
      if ((sel==OFTrue)&&((*colser)==(selser))){
		return OFFalse;
      }
    }
      
  } // end of while
    return OFFalse;
}


// Method to sort out duplicate Series
Uint32 DVInterface::stripidxarray(int *elemarray)
{
	int arrsize=0;
	int recordCounter;
	int idx;
	OFList<const char *> seriesuidlist;
	IdxRecord idxentry;
	for (idx=0; (elemarray[idx] > -2); idx++) arrsize++;	
	for (idx=0; idx<arrsize; idx++){
		recordCounter=elemarray[idx];
		if (DB_IdxGetNext (phandle, &recordCounter, &idxentry) != DB_NORMAL){ 
			cerr << "seek not sucessful !" << endl;
		} // fi
		seriesuidlist.push_back(idxentry.SeriesInstanceUID);
	} // for	
	arrsize=0;
	const char *lientry;
	OFListIterator(const char *) first = seriesuidlist.begin();
	OFListIterator(const char *) last = seriesuidlist.end();
	while ((seriesuidlist.size()!=0) && (first != last)){
		lientry = *first++;
		seriesuidlist.remove(lientry);
		arrsize++;
		if (seriesuidlist.size()==0) break;
	}
	return arrsize;
}

Uint32 DVInterface::getNumberOfSeries()
{
  Uint32 j;
  if (strcmp(selectedStudy,"")==0) return 0; 
  if (getAnInstance(OFFalse,OFTrue,OFFalse,&idxRec, selectedStudy, "" ,0,&j)==OFTrue) return 0;
  return j; 
}


E_Condition DVInterface::selectSeries(Uint32 idx)
{
  if (idx>getNumberOfSeries()-1) return EC_IllegalCall;
  if (getAnInstance(OFFalse,OFFalse,OFTrue,&idxRec, selectedStudy, selectedSeries,idx)==OFTrue) return EC_IllegalCall;
  strcpy(selectedSeries,idxRec.SeriesInstanceUID);
  return EC_Normal;
}



const char *DVInterface::getSeriesUID()
{
    if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
    if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFTrue) return NULL;
    return idxRec.SeriesInstanceUID;
}


DVIFhierarchyStatus DVInterface::getSeriesStatus()
{
  OFBool isNew=OFFalse;
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return DVIF_objectIsNew;   
  if (getAnInstance(OFTrue,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,&isNew)==OFFalse)
  {
    if (isNew==OFTrue) return DVIF_objectIsNew;
    else return DVIF_objectContainsNewSubobjects;
  }
  else return DVIF_objectIsNotNew;
}


OFBool DVInterface::isPresentationStateSeries()
{
  Uint32 num;
  num=getNumberOfInstances();
  for (Uint32 i=0; i<num; i++){
	  if (selectInstance(i)==EC_Normal) {
		  if ((strcmp(getSeriesUID(),selectedSeries)==0) && (strcmp(getModality(),"PR")!=0))
			  return OFFalse;
	  }
else return OFFalse;
	}
  return OFTrue;
 
}

const char *DVInterface::getModality()
{ 
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0)) return NULL; 
  if(getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries)==OFFalse) return idxRec.Modality;
  return NULL;
}


Uint32 DVInterface::getNumberOfInstances()
{
  Uint32 j;
  if (strcmp(selectedSeries,"")==0) return 0;
  if (lockDatabase() != EC_Normal) return 0;
	  
  pStudyDesc = (StudyDescRecord *)malloc (SIZEOF_STUDYDESC) ;
  if (pStudyDesc == NULL)
  {
    fprintf(stderr, "DB_PrintIndexFile: out of memory\n");
    return OFTrue;
  }
  if (DB_GetStudyDesc(phandle, pStudyDesc )==DB_ERROR)
  {
    cerr << "study descriptor not available !" << endl;
    return OFTrue;
  }
  
  j=pStudyDesc[studyidx].NumberofRegistratedImages;
  free(pStudyDesc);
  pStudyDesc=NULL;
  return j;
}


E_Condition DVInterface::selectInstance(Uint32 idx)
{
  if (idx > getNumberOfInstances()-1) return EC_IllegalCall;
  if  (getAnInstance(OFFalse,OFFalse,OFTrue,&idxRec, selectedStudy, selectedSeries,idx)==OFFalse) {
    strcpy(selectedInstance,idxRec.SOPInstanceUID);
  }
  else return EC_IllegalCall;
  return EC_Normal;
}


const char *DVInterface::getInstanceUID()
{
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0) || (strcmp(selectedInstance,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,NULL,0,selectedInstance)==OFFalse) return selectedInstance;
  else return NULL;
}


const char *DVInterface::getImageNumber()
{
  if ((strcmp(selectedSeries,"")==0) || (strcmp(selectedStudy,"")==0) || (strcmp(selectedInstance,"")==0)) return NULL; 
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,NULL,0,selectedInstance)==OFFalse) return idxRec.ImageNumber;
  else return NULL;
}




DVIFhierarchyStatus DVInterface::getInstanceStatus() 
{
  if (getAnInstance(OFTrue,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,NULL,0,selectedInstance)==OFFalse) return DVIF_objectIsNew;
  else return DVIF_objectIsNotNew;
}


OFBool DVInterface::isPresentationState()
{
  if (strcmp(getModality(),"PS")==0) return OFTrue;
  else return OFFalse;
}


E_Condition DVInterface::instanceReviewed(const char *studyUID,
                                          const char *seriesUID,
                                          const char *instanceUID)
{
  long curpos;
  if ((studyUID==NULL) || ((seriesUID==NULL) || (instanceUID==NULL))) return EC_IllegalCall;
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID, seriesUID,0,0,NULL,0,instanceUID)==OFFalse){  
    idxRec.hstat=DVIF_objectIsNotNew;
    curpos = lseek(phandle -> pidx, 0, SEEK_CUR);
    lseek(phandle -> pidx,curpos-SIZEOF_IDXRECORD, SEEK_SET);
    write (phandle -> pidx, (char *) &idxRec, SIZEOF_IDXRECORD);
    return EC_Normal;
  }
  else {
    return EC_IllegalCall;
  }
}

E_Condition DVInterface::deleteInstance(const char *studyUID,
                                        const char *seriesUID,
                                        const char *instanceUID)
{
  if ((studyUID==NULL) || ((seriesUID==NULL) || (instanceUID==NULL))) return EC_IllegalCall;
  if ( getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID, seriesUID,0,0,NULL,0,instanceUID)==OFTrue) return EC_IllegalCall;

  Uint32 counter;
  int studyIdx = 0;
  (counter)=0;
  if ((strcmp(selectedStudy,"")==0) || ((strcmp(selectedSeries,"")==0) || (strcmp(selectedInstance,"")==0))) return EC_IllegalCall;

  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,NULL,&counter,selectedInstance)==OFTrue) {
	  cerr << "getanInstance error" << endl;
	  return EC_IllegalCall;
  }
  if (DB_IdxRemove (phandle, (counter)-1)==DB_ERROR)
  {
	  cerr << "db_error" << endl;
	  return EC_IllegalCall;
  }
  pStudyDesc = (StudyDescRecord *)malloc (SIZEOF_STUDYDESC);
  DB_GetStudyDesc(phandle, pStudyDesc );
  studyIdx = DB_MatchStudyUIDInStudyDesc (pStudyDesc, (char*)selectedStudy, 
					    (int)(phandle -> maxStudiesAllowed));
  pStudyDesc[studyIdx].NumberofRegistratedImages -= 1;
  pStudyDesc[studyIdx].StudySize -= idxRec. ImageSize;
  DB_StudyDescChange (phandle, pStudyDesc);
  free(pStudyDesc);
  pStudyDesc=NULL;
  return EC_Normal;
}


E_Condition DVInterface::deleteSeries(const char *studyUID,
                                      const char *seriesUID)
{
 if ((studyUID==NULL) || (seriesUID==NULL)) return EC_IllegalCall;
 if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID, seriesUID)==OFFalse){ 
  Uint32 number=getNumberOfInstances();
  if (number==0) return EC_IllegalCall;
  for (Uint32 i=0;i<number;i++){

    if(selectInstance(0)!=EC_Normal)  
	{
    cerr << "cannot select instance" << endl;
		return EC_IllegalCall;
	}
    if (deleteInstance(selectedStudy, selectedSeries,selectedInstance)!=EC_Normal){
	cerr << "cannot delete instance" << endl;	
		return EC_IllegalCall;
	}
  }
  return EC_Normal;
 }
 else return EC_IllegalCall;
}


E_Condition DVInterface::deleteStudy(const char *studyUID)
{
	
  if (studyUID==NULL) return EC_IllegalCall;
  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID)==OFTrue) return EC_IllegalCall; 
  Uint32 number=getNumberOfSeries();
  for (Uint32 i=0;i<number;i++)
  {
    if (selectSeries(0)==EC_IllegalCall) return EC_IllegalCall;
    if (deleteSeries(selectedStudy, selectedSeries)==EC_IllegalCall) return EC_IllegalCall;
	getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID);
  }
  return EC_Normal;
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

/* keyword for configuration file */

#define L2_COMMUNICATION     "COMMUNICATION"
#define L2_GENERAL           "GENERAL"
#define L1_NETWORK           "NETWORK"
#define L1_DATABASE          "DATABASE"
#define L1_GUI               "GUI"
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
                                        DcmFileFormat *fileformat)
{
    DcmFileStream stream(filename, DCM_WriteMode);
    if (!stream.Fail())
    {
        if (fileformat != NULL)
        {
            E_Condition status;
            fileformat->transferInit();
            status = fileformat->write(stream, EXS_LittleEndianExplicit, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding);
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
  double aspectRatio)
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
      if (EC_Normal==status) status = putStringValue(dataset, DCM_SOPInstanceUID, newuid);
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
            status = saveFileFormat(filename, fileformat);
            delete fileformat;
          } else {
            status = EC_MemoryExhausted;
            delete dataset;
          }
      } else delete dataset;
    } else status = EC_MemoryExhausted;
    return status;
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
 *  Revision 1.24  1999-01-29 16:01:02  meichel
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

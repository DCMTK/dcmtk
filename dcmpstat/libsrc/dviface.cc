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
 *  Update Date:      $Date: 1999-01-18 17:30:48 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */



#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dviface.h"
#include "ofstring.h"    /* for class OFString */
#include "dvpsconf.h"    /* for class DVPSConfig */

DVInterface::DVInterface(const char *indexfolder, const char *config_file)
: pState(NULL)
, pDicomImage(NULL)
, pDicomPState(NULL)
, pConfig(NULL)
, SeriesNumber(0)
, StudyNumber(0)
, phandle(NULL)
, pStudyDesc(NULL)
, MaxStudyCount(200)
, StudySize(1000000)
, handle(NULL)
, idxRec()
{
  strcpy(IndexName,indexfolder);
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
}


DVInterface::~DVInterface()
{
  if (pDicomImage) delete pDicomImage;
  if (pDicomPState) delete pDicomPState;
  if (pState) delete pState;
}


E_Condition DVInterface::loadImage(const char *studyUID,
                                   const char *seriesUID,
                                   const char *instanceUID)
{
  E_Condition status = lockDatabase();
  if (status != EC_Normal) return status;
  const char *filename = getFilename(studyUID, seriesUID, instanceUID);
  status = unlockDatabase();
  if (status != EC_Normal) return status;
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
  status = unlockDatabase();
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
    status = lockDatabase();
    if (EC_Normal == status)
    { 
      imagefilename = getFilename(ofstudyUID.c_str(), ofseriesUID.c_str(), ofinstanceUID.c_str());
      status = unlockDatabase();
    }
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

const char *DVInterface::getFilename(const char * /*studyUID*/, const char * /*seriesUID*/, const char * /*instanceUID*/)
{
  // UNIMPLEMENTED
  return NULL;
}

E_Condition DVInterface::lockDatabase()
{
  DB_createHandle(IndexName, 
		  MaxStudyCount,
		  StudySize,
		  &handle);
  phandle = (DB_Private_Handle *) handle;
  if (DB_lock(phandle, OFTrue)==DB_ERROR) return EC_IllegalCall;
  return EC_Normal;
}


E_Condition DVInterface::unlockDatabase()
{
 if (DB_unlock(phandle)==DB_ERROR) return EC_IllegalCall;
// DB_destroyHandle(&handle); 
 return EC_Normal;
}



Uint32 DVInterface::getNumberOfStudies()
{
  int i ;
  Uint32 j=0 ;
  pStudyDesc = (StudyDescRecord *)malloc (SIZEOF_STUDYDESC) ;
  if (phandle==NULL) return 0;
  DB_GetStudyDesc(phandle, pStudyDesc );
  for (i=0; i<phandle->maxStudiesAllowed; i++) {
    if (pStudyDesc[i].NumberofRegistratedImages != 0 ) {  
      j++;
    }
  }
  return j;
}


E_Condition DVInterface::selectStudy(Uint32 idx)
{
  if (pStudyDesc==NULL) return EC_IllegalCall;
 if (phandle==NULL) return EC_IllegalCall;
 if ((pStudyDesc[idx].StudySize!=0) || ( idx < (unsigned)(phandle -> maxStudiesAllowed)+1)){
    strcpy(selectedStudy,pStudyDesc[idx].StudyInstanceUID);
	}
  else 
    return EC_IllegalCall;
  return EC_Normal;
}


const char *DVInterface::getStudyUID()
{
  if (selectedStudy==NULL) return NULL; 
  return selectedStudy;
}


const char *DVInterface::getStudyDescription()
{ 
  if (selectedStudy==NULL) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy);
  return idxRec.StudyDescription;
}


DVIFhierarchyStatus DVInterface::getStudyStatus()
{
  OFBool isNew=OFFalse;
  if (selectedStudy==NULL) return DVIF_objectIsNew; 
  if(getAnInstance(OFTrue,OFTrue,OFFalse,&idxRec, selectedStudy,NULL,0,0,&isNew)==OFFalse)
    if (isNew==OFTrue) return DVIF_objectIsNew;
    else
      return DVIF_objectContainsNewSubobjects;
  else
    return DVIF_objectIsNotNew;
}

const char *
DVInterface::getStudyDate(){
  if (selectedStudy==NULL) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy);
  return idxRec.StudyDate;
}

const char *
DVInterface::getStudyTime(){
  if (selectedStudy==NULL) return NULL;
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy);
  return idxRec.StudyTime;
}

const char *
DVInterface::getReferringPhysiciansName(){
  if (selectedStudy==NULL) return NULL; 

  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy);
  return idxRec.ReferringPhysiciansName;
}

const char *
DVInterface::getAccessionNumber(){
  if (selectedStudy==NULL) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedStudy);
  return idxRec.AccessionNumber;
}

const char *
DVInterface::getNameOfPhysiciansReadingStudy(){
  if (selectedStudy==NULL) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy);
  return idxRec.NameOfPhysiciansReadingStudy;
}


const char *DVInterface::getSeriesDescription()
{ 
  if (selectedSeries==NULL) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec,selectedSeries);
  return idxRec.SeriesDescription;
}


const char *
DVInterface::getSeriesNumber(){
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
  return idxRec.SeriesNumber;
}

const char *
DVInterface::getSeriesDate(){
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
  return idxRec.SeriesDate;
}

const char *
DVInterface::getSeriesTime(){
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
  return idxRec.SeriesTime;
}


const char *
DVInterface::getSeriesPerformingPhysiciansName(){
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
  return idxRec.PerformingPhysiciansName;
}

const char *
DVInterface::getSeriesProtocolName(){
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
  return idxRec.ProtocolName;
}

const char *
DVInterface::getSeriesOperatorsName(){
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
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
DVInterface::getAnInstance(OFBool dvistatus, 
			   OFBool count,  
			   OFBool sel, 
			   IdxRecord *anIdxRec, 
			   const char *StudyUID, 
			   const char *SeriesUID, 
			   Uint32 selser, 
			   Uint32 *colser ,
			   OFBool *isNew, 
			   Uint32 *idxCounter,
			   const char *InstanceUID 
			   )  {
  
  int i ;
  int j1 ;
  
  if (colser!=NULL)  (*colser)=0; 
 
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
  for (i=0; i<phandle->maxStudiesAllowed; i++) {
    
    if (pStudyDesc[i].NumberofRegistratedImages != 0 ) {

    }
  }
  DB_IdxInitLoop (phandle, &j1) ;
   OFBool study,series,instance;
       free (pStudyDesc) ;
    pStudyDesc=NULL;

  while (1) {
    study=OFFalse;
    series=OFFalse;
    instance=OFFalse;    
    if (idxCounter!=NULL) (*idxCounter)++;
	if (DB_IdxGetNext (phandle, &j1, anIdxRec) != DB_NORMAL){ 
	

	   return OFTrue;
	} // fi
    if (StudyUID!=NULL){
      if (strcmp(StudyUID, (*anIdxRec).StudyInstanceUID)==0) study=OFTrue;
      else
	if (strcmp(StudyUID, "")==0) study=OFTrue;
    } 
    else
      study=OFTrue;

    if (SeriesUID!=NULL){
      if (strcmp(SeriesUID, (*anIdxRec).SeriesInstanceUID)==0) series=OFTrue;
      else
	if (strcmp(SeriesUID, "")==0) series=OFTrue;
      
    }
    else
      series=OFTrue;

    if (InstanceUID!=NULL){
      if (strcmp(InstanceUID, (*anIdxRec).SOPInstanceUID)==0) instance=OFTrue;
      else 
	if (strcmp(InstanceUID, "")==0) instance=OFTrue;
      
    }
    else
      instance=OFTrue;
  
    if ((study==OFTrue) && ((series==OFTrue) && (instance==OFTrue))){
      if (count==OFFalse){
	
	break ;
      }
      else{
	(*colser)++;
	if ((dvistatus==OFTrue) && ((*anIdxRec).hstat==DVIF_objectIsNew)) 
        {
          if (isNew!=NULL) (*isNew)=OFTrue;
          else
           return OFFalse;
        }
	else
	  if (isNew!=NULL)  (*isNew)=OFFalse;
      }
      if ((sel==OFTrue)&&((*colser)==(selser))){
	return OFFalse;
      }
    }
      
  } // end of while
  
  return OFFalse;
}


Uint32 DVInterface::getNumberOfSeries()
{
  Uint32 j;
  if (strcmp(selectedStudy,"")==0){
	  return 0; 
  }
  getAnInstance(OFFalse,OFTrue,OFFalse,&idxRec, selectedStudy, "" ,0,&j);
  return j; 
}


E_Condition DVInterface::selectSeries(Uint32 idx)
{
  if  (getAnInstance(OFFalse,OFFalse,OFTrue,&idxRec, selectedStudy, selectedSeries,idx)==OFFalse){ 
    strcpy(selectedSeries,idxRec.SeriesInstanceUID);
  }
  else 
    return EC_IllegalCall;
  return EC_Normal;
}



const char *DVInterface::getSeriesUID()
{
    if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
    getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
    return idxRec.SeriesInstanceUID;
}


DVIFhierarchyStatus DVInterface::getSeriesStatus()
{
  OFBool isNew=OFFalse;
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return DVIF_objectIsNew;   if (getAnInstance(OFTrue,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,&isNew)==OFFalse)
    if (isNew==OFTrue) return DVIF_objectIsNew;
    else
      return DVIF_objectContainsNewSubobjects;
  else
    return DVIF_objectIsNotNew;
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


Uint32 DVInterface::getNumberOfInstances()
{
  Uint32 j;
  if (strcmp(selectedSeries,"")==0) return 0;  
  getAnInstance(OFFalse,OFTrue,OFFalse,&idxRec, selectedStudy, selectedSeries,0,&j);
  
  return j;
}


E_Condition DVInterface::selectInstance(Uint32 idx)
{
  if  (getAnInstance(OFFalse,OFFalse,OFTrue,&idxRec, selectedStudy, selectedSeries,idx)==OFFalse) 
    strcpy(selectedInstance,idxRec.SOPInstanceUID);
  else 
    return EC_IllegalCall;
  return EC_Normal;
}


const char *DVInterface::getInstanceUID()
{
  if ((selectedSeries==NULL) || (selectedStudy==NULL) || (selectedInstance==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,NULL,0,selectedInstance);
  return selectedInstance;
}


const char *DVInterface::getModality()
{ 
  if ((selectedSeries==NULL) || (selectedStudy==NULL)) return NULL; 
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries);
  return idxRec.Modality;
  
}


DVIFhierarchyStatus DVInterface::getInstanceStatus() 
{
if (getAnInstance(OFTrue,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,NULL,0,selectedInstance)==OFFalse)
  return DVIF_objectIsNew;
else 
  return DVIF_objectIsNotNew;
 
}


OFBool DVInterface::isPresentationState()
{
  if (strcmp(getModality(),"PS")==0) return OFTrue;
  else
    return OFFalse;
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
  else
	  return EC_IllegalCall;
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
  if ((selectedStudy==NULL) || ((selectedSeries==NULL) || (selectedInstance==NULL))) return EC_IllegalCall;

  if (getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, selectedStudy, selectedSeries,0,0,NULL,&counter,selectedInstance)==OFTrue) return EC_IllegalCall;

  if (DB_IdxRemove (phandle, (counter)-1)==DB_ERROR) 
	  return EC_IllegalCall;
 pStudyDesc = (StudyDescRecord *)malloc (SIZEOF_STUDYDESC) ;
 DB_GetStudyDesc(phandle, pStudyDesc );

 studyIdx = DB_MatchStudyUIDInStudyDesc (pStudyDesc, (char*)selectedStudy, 
					    (int)(phandle -> maxStudiesAllowed)) ;
 
 pStudyDesc[studyIdx].NumberofRegistratedImages -= 1 ;
 pStudyDesc[studyIdx].StudySize -= 2;
 DB_StudyDescChange (phandle, pStudyDesc);
 free (pStudyDesc) ;
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
    if(selectInstance(i)!=EC_Normal) return EC_IllegalCall;
    if (deleteInstance(selectedStudy, selectedSeries,selectedInstance)!=EC_Normal) 
		return EC_IllegalCall;
getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID, seriesUID);
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
  for (Uint32 i=0;i<number;i++){
    if (selectSeries(i)==EC_IllegalCall) return EC_IllegalCall;
    if (deleteSeries(selectedStudy, selectedSeries)==EC_IllegalCall) return EC_IllegalCall;
	getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID);
}
   return EC_Normal;
}
 

E_Condition DVInterface::sendIOD(const char * /*targetID*/,
                                 const char * /*studyUID*/,
                                 const char * /*seriesUID*/,
                                 const char * /*instanceUID*/)
{
    return EC_IllegalCall;
}


/* keyword for configuration file */

#define L2_COMMUNICATION "COMMUNICATION"
#define L0_HOSTNAME      "HOSTNAME"
#define L0_PORT          "PORT"
#define L0_DESCRIPTION   "DESCRIPTION"
#define L0_AETITLE       "AETITLE"

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


const char *DVInterface::getTargetDescription(const char *targetID)
{
  if (targetID==NULL) return NULL;
  const char *result=NULL; 
  if (pConfig)
  {
    pConfig->select_section(targetID, L2_COMMUNICATION);
    if (pConfig->section_valid(1)) result = pConfig->get_entry(L0_DESCRIPTION);
  }
  return result;
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


/*
 *  CVS/RCS Log:
 *  $Log: dviface.cc,v $
 *  Revision 1.14  1999-01-18 17:30:48  meichel
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

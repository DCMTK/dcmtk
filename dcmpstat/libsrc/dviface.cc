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
 *  Last Update:      $Author: vorwerk $
 *  Update Date:      $Date: 1999-01-12 17:00:29 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */



#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dviface.h"


DVInterface::DVInterface(const char *indexfolder)
: pState()
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
}


DVInterface::~DVInterface()
{
}


E_Condition DVInterface::loadImage(const char *studyUID,
                                   const char *seriesUID,
                                   const char *instanceUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::loadImage(const char *imgName)
{
    E_Condition status = EC_IllegalCall;
    DcmFileFormat *image = NULL;
    if ((status = loadFileFormat(imgName, image)) == EC_Normal)
    {
        if (image != NULL)
        {
            DcmDataset *dataset = image->getDataset();
            if (dataset != NULL)
            {
                if ((status = pState.createFromImage(*dataset)) == EC_Normal)
                    status = pState.attachImage(image);
            } else
                status = EC_CorruptedData;
        } else
            status = EC_IllegalCall;
    }
    return status;
}


E_Condition DVInterface::loadPState(const char *studyUID,
                                    const char *seriesUID,
                                    const char *instanceUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::loadPState(const char *pstName,
                                    const char *imgName)
{
    E_Condition status = EC_IllegalCall;
    DcmFileFormat *pstate = NULL;
    DcmFileFormat *image = NULL;
    if (((status = loadFileFormat(pstName, pstate)) == EC_Normal) &&
        ((status = loadFileFormat(imgName, image)) == EC_Normal))
    {
        if ((pstate != NULL) && (image != NULL))
        {
            DcmDataset *dataset = pstate->getDataset();
            if (dataset != NULL)
            {
                if ((status = pState.read(*dataset)) == EC_Normal)
                    status = pState.attachImage(image);
            } else
                status = EC_MemoryExhausted;
        } else
            status = EC_IllegalCall;
    }
    return status;
}


E_Condition DVInterface::savePState()
{
    return EC_IllegalCall;
}


E_Condition DVInterface::savePState(const char *filename)
{
    E_Condition status = EC_IllegalCall;
    DcmDataset *dataset = new DcmDataset();
    if (dataset != NULL)
    {
        if ((status = pState.write(*dataset)) == EC_Normal)
        {
        	DcmFileFormat *fileformat = new DcmFileFormat(dataset);
        	if (fileformat != NULL)
                status = saveFileFormat(filename, fileformat);
            else
                status = EC_MemoryExhausted;
        }
    } else
        status = EC_MemoryExhausted;
    return status;
}


void DVInterface::clear()
{
    pState.clear();
}


DVPresentationState &DVInterface::getCurrentPState()
{
    return pState;
}


E_Condition DVInterface::lockDatabase()
{
 DB_createHandle(
		  IndexName, 
		  MaxStudyCount,
		  StudySize,
		  &handle
		  );
  phandle = (DB_Private_Handle *) handle ; 

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
	cout << "selectedStudy" << selectedStudy << endl;
	}
  else 
    return EC_IllegalCall;
  strcpy(selectedStudy,"");
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
	//	cerr << "Images in Study: " << i << endl;
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
		//cerr << "No instance found in this db !" << endl;
	

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
		  cout << "Instance found !" << endl;
	return OFFalse;
      }
    }
      
  } // end of while
  
  return OFFalse;
}


Uint32 DVInterface::getNumberOfSeries()
{
  Uint32 j;
  if (strcmp(selectedStudy,"")==0) return 0; 
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
			  return OFTrue;
	  }
	}
  return OFFalse;
 
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
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID, seriesUID,0,0,NULL,0,instanceUID);  
  idxRec.hstat=DVIF_objectIsNotNew;
  curpos = lseek(phandle -> pidx, 0, SEEK_CUR);
  lseek(phandle -> pidx,curpos-SIZEOF_IDXRECORD, SEEK_SET);
  write (phandle -> pidx, (char *) &idxRec, SIZEOF_IDXRECORD);  
  return EC_Normal;
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
 
  DB_IdxRemove (phandle, (counter)-1);
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
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID, seriesUID);   
  Uint32 number=getNumberOfInstances();
  for (Uint32 i=0;i<number;i++){

    selectInstance(i);

    deleteInstance(selectedStudy, selectedSeries,selectedInstance);

  }
  return EC_Normal;
}


E_Condition DVInterface::deleteStudy(const char *studyUID)
{
if (studyUID==NULL) return EC_IllegalCall;
  getAnInstance(OFFalse,OFFalse,OFFalse,&idxRec, studyUID);  
  Uint32 number=getNumberOfSeries();
  for (Uint32 i=0;i<number;i++){
    selectSeries(i);
    deleteSeries(selectedStudy, selectedSeries);
}
   return EC_Normal;
}
 

E_Condition DVInterface::sendIOD(const char *targetID,
                                 const char *studyUID,
                                 const char *seriesUID,
                                 const char *instanceUID)
{
    return EC_IllegalCall;
}


Uint32 DVInterface::getNumberOfTargets()
{
    return 0;
}


const char *DVInterface::getTargetID(Uint32 idx)
{
    return NULL;
}


const char *DVInterface::getTargetDescription(Uint32 idx)
{
    return NULL;
}


const char *DVInterface::getTargetDescription(const char *targetID)
{
    return NULL;
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


/*
 *  CVS/RCS Log:
 *  $Log: dviface.cc,v $
 *  Revision 1.9  1999-01-12 17:00:29  vorwerk
 *  bug fixed in method getAnInstance
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

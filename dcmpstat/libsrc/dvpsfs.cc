/*
 *
 *  Copyright (C) 1998-2002, OFFIS
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
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSFilmSession
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-11-27 15:48:09 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsfs.h"
#include "dvpsdef.h"     /* for constants and macros */
#include "ofstring.h"
#include "dvpshlp.h"
#include "dvpscf.h"
#include "dvpspl.h"
#include "dvpspll.h"
#include "dvpssp.h"
#include "dvpsspl.h"

#define INCLUDE_CSTRING
#include "ofstdinc.h"

/* --------------- class DVPSFilmSession --------------- */

DVPSFilmSession::DVPSFilmSession(Uint16 illumin, Uint16 reflection)
: sopInstanceUID(NULL)
, numberOfCopies(DCM_NumberOfCopies)
, printPriority(DCM_PrintPriority)
, mediumType(DCM_MediumType)
, filmDestination(DCM_FilmDestination)
, filmSessionLabel(DCM_FilmSessionLabel)
, ownerID(DCM_OwnerID)
, illumination(DCM_Illumination)
, reflectedAmbientLight(DCM_ReflectedAmbientLight)
, referencedPresentationLUTInstanceUID(DCM_ReferencedSOPInstanceUID)
, referencedPresentationLUTAlignment(DVPSK_other)
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
  illumination.putUint16(illumin, 0);
  reflectedAmbientLight.putUint16(reflection, 0);
}

DVPSFilmSession::DVPSFilmSession(const DVPSFilmSession& copy)
: sopInstanceUID(copy.sopInstanceUID)
, numberOfCopies(copy.numberOfCopies)
, printPriority(copy.printPriority)
, mediumType(copy.mediumType)
, filmDestination(copy.filmDestination)
, filmSessionLabel(copy.filmSessionLabel)
, ownerID(copy.ownerID)
, illumination(copy.illumination)
, reflectedAmbientLight(copy.reflectedAmbientLight)
, referencedPresentationLUTInstanceUID(copy.referencedPresentationLUTInstanceUID)
, referencedPresentationLUTAlignment(copy.referencedPresentationLUTAlignment)
, logstream(copy.logstream)
, verboseMode(copy.verboseMode)
, debugMode(copy.debugMode)
{
}

DVPSFilmSession::~DVPSFilmSession()
{
}

OFBool DVPSFilmSession::isInstance(const char *uid)
{
  if (uid && (sopInstanceUID == uid)) return OFTrue;
  return OFFalse;
}

OFBool DVPSFilmSession::printSCPCreate(
    DVConfiguration& cfg, 
    const char *cfgname, 
    DcmDataset *rqDataset, 
    T_DIMSE_Message& rsp, 
    DcmDataset *& rspDataset, 
    const char *peerae,
    OFBool presentationLUTnegotiated,
    DVPSPresentationLUT_PList& globalPresentationLUTList)
{
  OFBool result = OFTrue;
  DcmStack stack;
  sopInstanceUID = rsp.msg.NCreateRSP.AffectedSOPInstanceUID;
  
  // numberOfCopies
  if (result)
  {
    READ_FROM_PDATASET(DcmIntegerString, numberOfCopies)
    if (numberOfCopies.getLength() == 0) numberOfCopies.putString(DEFAULT_numberOfCopies);
    else
    {
      Sint32 numCopies=0;
      char *numCopiesString=NULL;
      if ((EC_Normal != numberOfCopies.getSint32(numCopies)) || (numCopies < 1))
      {
        numberOfCopies.getString(numCopiesString);
        if (numCopiesString==NULL) numCopiesString = (char *)"";
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot create Basic Film Session: illegal number of copies: '" << numCopiesString << "'" << endl; 
          logstream->unlockCerr();
        }
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }

  // printPriority
  if (result)
  {
    READ_FROM_PDATASET(DcmCodeString, printPriority)
    if (printPriority.getLength() == 0) printPriority.putString(DEFAULT_priority);
    else
    {
      OFString aString;
      printPriority.getOFString(aString, 0, OFTrue);
      if ((aString != "HIGH")&&(aString != "MED")&&(aString != "LOW"))
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot create Basic Film Session: illegal print priority: '" << aString.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }
  
  // mediumType
  if (result)
  {
    Uint32 numMediumTypes = cfg.getTargetPrinterNumberOfMediumTypes(cfgname);  	
    READ_FROM_PDATASET(DcmCodeString, mediumType)
    if (mediumType.getLength() == 0) 
    {
      if (numMediumTypes > 0)
      {
      	OFString aString;
      	cfg.getTargetPrinterMediumType(cfgname, 0, aString);      	
        mediumType.putString(aString.c_str());
      } else {
        mediumType.putString(DEFAULT_mediumType);
      }
    } else {
      // check whether we can accept the proposed medium type
      OFString theMedium;
      OFString aString;
      OFBool found = OFFalse;
      mediumType.getOFString(theMedium, 0, OFTrue);
      for (Uint32 i=0; i<numMediumTypes; i++)
      {
      	cfg.getTargetPrinterMediumType(cfgname, i, aString);
      	if (theMedium == aString) 
      	{
      	  found = OFTrue;
      	  break;
      	}	
      }
      if (! found)
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot create Basic Film Session: illegal medium type: '" << theMedium.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }

  // filmDestination
  if (result)
  {
    Uint32 numFilmDestination = cfg.getTargetPrinterNumberOfFilmDestinations(cfgname);  	
    READ_FROM_PDATASET(DcmCodeString, filmDestination)
    if (filmDestination.getLength() == 0) 
    {
      if (numFilmDestination > 0)
      {
      	OFString aString;
      	cfg.getTargetPrinterFilmDestination(cfgname, 0, aString);      	
        filmDestination.putString(aString.c_str());
      } else {
        filmDestination.putString(DEFAULT_filmDestination);
      }
    } else {
      // check whether we can accept the proposed medium type
      OFString theDestination;
      OFString aString;
      OFBool found = OFFalse;
      filmDestination.getOFString(theDestination, 0, OFTrue);
      for (Uint32 i=0; i<numFilmDestination; i++)
      {
      	cfg.getTargetPrinterFilmDestination(cfgname, i, aString);
      	if (theDestination == aString) 
      	{
      	  found = OFTrue;
      	  break;
      	}	
      }
      if (! found)
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot create Basic Film Session: illegal film destination: '" << theDestination.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      }
    }
  }

  // filmSessionLabel
  if (result)
  {
    READ_FROM_PDATASET(DcmLongString, filmSessionLabel)
    if (filmSessionLabel.getLength() == 0) 
    {
      OFString labelString("print job ");
      OFString aString;
      if (peerae)
      {
      	labelString += "for \"";
      	labelString += peerae;
      	labelString += "\" ";
      }
      labelString += "created ";
      DVPSHelper::currentDate(aString);
      labelString += aString;
      labelString += " ";
      DVPSHelper::currentTime(aString);
      labelString += aString;      
      filmSessionLabel.putString(labelString.c_str());
    }
  }

  // ownerID
  if (result)
  {
    READ_FROM_PDATASET(DcmShortString, ownerID)
    if (ownerID.getLength() == 0) 
    {
      if (peerae) ownerID.putString(peerae); else ownerID.putString(DEFAULT_ownerID);
    }
  }

  if (presentationLUTnegotiated)
  {

    // illumination
    if (result)
    {
      READ_FROM_PDATASET(DcmUnsignedShort, illumination)
      if (illumination.getLength() == 0) illumination.putUint16(DEFAULT_illumination, 0);
      // we don't check illumination set by the user (for now)
    }

    // reflectedAmbientLight
    if (result)
    {
      READ_FROM_PDATASET(DcmUnsignedShort, reflectedAmbientLight)
      if (reflectedAmbientLight.getLength() == 0) illumination.putUint16(DEFAULT_reflectedAmbientLight, 0);
      // we don't check reflected ambient light set by the user (for now)
    }

    // referenced presentation LUT sequence
    if (result)
    {
      stack.clear();

      if (rqDataset && (EC_Normal == rqDataset->search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse)))
      {
        DcmSequenceOfItems *seq=(DcmSequenceOfItems *)stack.top();
        if (seq->card() ==1)
        {
           OFString aString;
           DcmItem *item = seq->getItem(0);
           stack.clear();
           READ_FROM_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)           
           if (referencedPresentationLUTInstanceUID.getLength() > 0)
           {
             referencedPresentationLUTInstanceUID.getOFString(aString,0);
             DVPSPresentationLUT *currentPLUT = globalPresentationLUTList.findPresentationLUT(aString.c_str());
             if (NULL == currentPLUT)
             {
               if (verboseMode)
               {
                 logstream->lockCerr() << "cannot create Basic Film Session: presentation LUT reference cannot be resolved" << endl;
                 logstream->unlockCerr();
               }
               rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
               result = OFFalse;
             } else {
               // check referenced SOP class UID
               DcmUniqueIdentifier refClassUID(DCM_ReferencedSOPClassUID);
               stack.clear();
               READ_FROM_DATASET2(DcmUniqueIdentifier, refClassUID)
               if (refClassUID.getLength() > 0)
               {     
                  aString.clear();
                  refClassUID.getOFString(aString,0, OFTrue);
                  if (aString != UID_PresentationLUTSOPClass)
                  {
                    if (verboseMode)
                    {
                      ostream &mycerr = logstream->lockCerr();
                      mycerr << "cannot create Basic Film Session: referenced SOP class UID in referenced presentation LUT sequence incorrect:" << endl;
                      (stack.top())->print(mycerr, OFFalse);
                      logstream->unlockCerr();            
                    }
                    rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                    result = OFFalse;
                  } else {
                    // referenced presentation LUT sequence is OK
                    referencedPresentationLUTAlignment = currentPLUT->getAlignment();
                  }
               } else {
                  if (verboseMode)
                  {
                    logstream->lockCerr() << "cannot create Basic Film Session: no referenced SOP class UID in referenced presentation LUT sequence" << endl;
                    logstream->unlockCerr();
                  }
                  rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                  result = OFFalse;
               }
             }
           } else {
             if (verboseMode)
             {
               logstream->lockCerr() << "cannot create Basic Film Session: no referenced SOP instance UID in referenced presentation LUT sequence" << endl;
               logstream->unlockCerr();
             }
             rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
             result = OFFalse;
           }
        } else {
          if (verboseMode)
          {
            logstream->lockCerr() << "cannot create Basic Film Session: referenced presentation LUT sequence number of items != 1" << endl;
            logstream->unlockCerr();
          }
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }    
    
  } /* if presentationLUTnegotiated */

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    OFBool intoSub = OFTrue;
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, intoSub))
    {
      intoSub = OFFalse;
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag.getElement() == 0x0000) /* group length */ ;
      else if (currentTag == DCM_NumberOfCopies) /* OK */ ;
      else if (currentTag == DCM_PrintPriority) /* OK */ ;
      else if (currentTag == DCM_MediumType) /* OK */ ;
      else if (currentTag == DCM_FilmDestination) /* OK */ ;
      else if (currentTag == DCM_FilmSessionLabel) /* OK */ ;
      else if (currentTag == DCM_OwnerID) /* OK */ ;
      else if (currentTag == DCM_MemoryAllocation)
      {
        if (verboseMode)
        {
          ostream &mycerr = logstream->lockCerr();
          mycerr << "warning while creating Basic Film Session: memory allocation not supported" << endl;
          (stack.top())->print(mycerr, OFFalse);
          logstream->unlockCerr();
        }
      	if (rsp.msg.NCreateRSP.DimseStatus == 0) rsp.msg.NCreateRSP.DimseStatus = STATUS_N_PRINT_BFS_Warn_MemoryAllocation;
      }
      else if (currentTag == DCM_Illumination)
      {
        if (! presentationLUTnegotiated)
        {
          if (verboseMode)
          {
            ostream &mycerr = logstream->lockCerr();
            mycerr << "cannot create Basic Film Session: illumination received:" << endl;
            (stack.top())->print(mycerr, OFFalse);
            logstream->unlockCerr();
          }
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;          
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReflectedAmbientLight)
      {
        if (! presentationLUTnegotiated)
        {
          if (verboseMode)
          {
            ostream &mycerr = logstream->lockCerr();
            mycerr << "cannot create Basic Film Session: reflected ambient light received:" << endl;
            (stack.top())->print(mycerr, OFFalse);
            logstream->unlockCerr();
          }
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;          
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReferencedPresentationLUTSequence)
      {
        if (! presentationLUTnegotiated)
        {
          if (verboseMode)
          {
            ostream &mycerr = logstream->lockCerr();
            mycerr << "cannot create Basic Film Session: referenced presentation LUT sequence received:" << endl;
            (stack.top())->print(mycerr, OFFalse);
            logstream->unlockCerr();
          }
          rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;          
          result = OFFalse;
        }
      }
      else
      {
        if (verboseMode)
        {
          ostream &mycerr = logstream->lockCerr();
          mycerr << "cannot create Basic Film Session: unsupported attribute received:" << endl;
          (stack.top())->print(mycerr, OFFalse);
          logstream->unlockCerr();
        }
      	rsp.msg.NCreateRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }
  
  // if n-create was successful, create response dataset
  if (result)
  {
    rspDataset = new DcmDataset;    
    if (rspDataset)
    {
      OFCondition writeresult = EC_Normal;
      DcmElement *delem = NULL;
      
      ADD_TO_PDATASET(DcmIntegerString, numberOfCopies)
      ADD_TO_PDATASET(DcmCodeString, printPriority)
      ADD_TO_PDATASET(DcmCodeString, mediumType)
      ADD_TO_PDATASET(DcmCodeString, filmDestination)
      ADD_TO_PDATASET(DcmLongString, filmSessionLabel)
      ADD_TO_PDATASET(DcmShortString, ownerID)

      if (presentationLUTnegotiated)
      {
        if (referencedPresentationLUTInstanceUID.getLength() == 0)
        {
          referencedPresentationLUTInstanceUID.putString(WELLKNOWN_IDENTITY_PLUT_UID);
          if (NULL == globalPresentationLUTList.findPresentationLUT(WELLKNOWN_IDENTITY_PLUT_UID))
          {
            DVPSPresentationLUT *wellknownlut = new DVPSPresentationLUT();
            if (wellknownlut)
            {
              writeresult = wellknownlut->setType(DVPSP_identity);
              if (EC_Normal == writeresult) writeresult = wellknownlut->setSOPInstanceUID(WELLKNOWN_IDENTITY_PLUT_UID);
              if (EC_Normal == writeresult) globalPresentationLUTList.insert(wellknownlut);              
            } else writeresult = EC_MemoryExhausted;
          }
        }
        if (EC_Normal == writeresult) writeresult = addPresentationLUTReference(*rspDataset);
      }

      if (EC_Normal == writeresult)
      {
        rsp.msg.NCreateRSP.DataSetType = DIMSE_DATASET_PRESENT;
      } else {
      	delete rspDataset;
      	rspDataset = NULL;
        rsp.msg.NCreateRSP.DimseStatus = STATUS_N_ProcessingFailure;
        result = OFFalse;
      }     
    } else {
      rsp.msg.NCreateRSP.DimseStatus = STATUS_N_ProcessingFailure;
      result = OFFalse;
    }
  }
  return result;
}


OFBool DVPSFilmSession::printSCPSet(
    DVConfiguration& cfg, 
    const char *cfgname, 
    DcmDataset *rqDataset, 
    T_DIMSE_Message& rsp, 
    DcmDataset *& rspDataset,
    OFBool presentationLUTnegotiated,
    DVPSPresentationLUT_PList& globalPresentationLUTList,
    DVPSStoredPrint_PList& basicFilmBoxList)
{
  OFBool result = OFTrue;
  DcmStack stack;
  OFCondition writeresult = EC_Normal;
  DcmElement *delem = NULL;
  OFBool overrideFilmBoxPLUTSettings = OFFalse;
  
  rspDataset = new DcmDataset;    
  if ((rqDataset == NULL)||(rspDataset == NULL))
  {
    rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
    result = OFFalse;
  }
  
  // numberOfCopies
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)numberOfCopies.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      numberOfCopies = *((DcmIntegerString *)(stack.top()));
      Sint32 numCopies=0;
      char *numCopiesString=NULL;
      if ((EC_Normal != numberOfCopies.getSint32(numCopies)) || (numCopies < 1))
      {
        numberOfCopies.getString(numCopiesString);
        if (numCopiesString==NULL) numCopiesString = (char *)"";
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Film Session: illegal number of copies: '" << numCopiesString << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmIntegerString, numberOfCopies)
      }
    }
  }

  // printPriority
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)printPriority.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      printPriority = *((DcmCodeString *)(stack.top()));
      OFString aString;
      printPriority.getOFString(aString, 0, OFTrue);
      if ((aString != "HIGH")&&(aString != "MED")&&(aString != "LOW"))
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Film Session: illegal print priority: '" << aString.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmCodeString, printPriority)
      }
    }
  }
  
  // mediumType
  if (result)
  {
    Uint32 numMediumTypes = cfg.getTargetPrinterNumberOfMediumTypes(cfgname);  	
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)mediumType.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      mediumType = *((DcmCodeString *)(stack.top()));
      OFString theMedium;
      OFString aString;
      OFBool found = OFFalse;
      mediumType.getOFString(theMedium, 0, OFTrue);
      for (Uint32 i=0; i<numMediumTypes; i++)
      {
      	cfg.getTargetPrinterMediumType(cfgname, i, aString);
      	if (theMedium == aString) 
      	{
      	  found = OFTrue;
      	  break;
      	}	
      }
      if (! found)
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Film Session: illegal medium type: '" << theMedium.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmCodeString, mediumType)
      }
    }
  }

  // filmDestination
  if (result)
  {
    Uint32 numFilmDestination = cfg.getTargetPrinterNumberOfFilmDestinations(cfgname);  	
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)filmDestination.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      filmDestination = *((DcmCodeString *)(stack.top()));
      OFString theDestination;
      OFString aString;
      OFBool found = OFFalse;
      filmDestination.getOFString(theDestination, 0, OFTrue);
      for (Uint32 i=0; i<numFilmDestination; i++)
      {
      	cfg.getTargetPrinterFilmDestination(cfgname, i, aString);
      	if (theDestination == aString) 
      	{
      	  found = OFTrue;
      	  break;
      	}	
      }
      if (! found)
      {
        if (verboseMode)
        {
          logstream->lockCerr() << "cannot update Basic Film Session: illegal film destination: '" << theDestination.c_str() << "'" << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        result = OFFalse;
      } else {
        ADD_TO_PDATASET(DcmCodeString, filmDestination)
      }
    }
  }

  // filmSessionLabel
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)filmSessionLabel.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      filmSessionLabel = *((DcmLongString *)(stack.top()));
      ADD_TO_PDATASET(DcmLongString, filmSessionLabel)
    }
  }

  // ownerID
  if (result)
  {
    stack.clear();
    if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)ownerID.getTag(), stack, ESM_fromHere, OFFalse)))
    {
      ownerID = *((DcmShortString *)(stack.top()));
      ADD_TO_PDATASET(DcmShortString, ownerID)
    }
  }

  if (presentationLUTnegotiated)
  {

    // illumination
    if (result)
    {
      stack.clear();
      if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)illumination.getTag(), stack, ESM_fromHere, OFFalse)))
      {
        illumination = *((DcmUnsignedShort *)(stack.top()));
        // we don't check illumination set by the user (for now)
        ADD_TO_PDATASET(DcmUnsignedShort, illumination)
      }
    }

    // reflectedAmbientLight
    if (result)
    {
      stack.clear();
      if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)reflectedAmbientLight.getTag(), stack, ESM_fromHere, OFFalse)))
      {
        reflectedAmbientLight = *((DcmUnsignedShort *)(stack.top()));
        // we don't check reflected ambient light set by the user (for now)
        ADD_TO_PDATASET(DcmUnsignedShort, reflectedAmbientLight)
      }
    }

    // referenced presentation LUT sequence
    if (result)
    {
      stack.clear();

      if (rqDataset && (EC_Normal == rqDataset->search(DCM_ReferencedPresentationLUTSequence, stack, ESM_fromHere, OFFalse)))
      {
        DcmSequenceOfItems *seq=(DcmSequenceOfItems *)stack.top();
        if (seq->card() ==1)
        {
           OFString aString;
           DcmItem *item = seq->getItem(0);
           stack.clear();
           READ_FROM_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)
           if (referencedPresentationLUTInstanceUID.getLength() > 0)
           {
             referencedPresentationLUTInstanceUID.getOFString(aString,0);
             DVPSPresentationLUT *currentPLUT = globalPresentationLUTList.findPresentationLUT(aString.c_str());
             if (NULL == currentPLUT)
             {
               if (verboseMode)
               {
                 logstream->lockCerr() << "cannot update Basic Film Session: presentation LUT reference cannot be resolved" << endl;
                 logstream->unlockCerr();
               }
               rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
               result = OFFalse;
             } else {
               // check referenced SOP class UID
               DcmUniqueIdentifier refClassUID(DCM_ReferencedSOPClassUID);
               stack.clear();
               READ_FROM_DATASET2(DcmUniqueIdentifier, refClassUID)
               if (refClassUID.getLength() > 0)
               {     
                  aString.clear();
                  refClassUID.getOFString(aString,0, OFTrue);
                  if (aString != UID_PresentationLUTSOPClass)
                  {
                    if (verboseMode)
                    {
                      ostream &mycerr = logstream->lockCerr();
                      mycerr << "cannot update Basic Film Session: referenced SOP class UID in referenced presentation LUT sequence incorrect:" << endl;
                      (stack.top())->print(mycerr, OFFalse);
                      logstream->unlockCerr();
                    }
                    rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                    result = OFFalse;
                  } else {
                    referencedPresentationLUTAlignment = currentPLUT->getAlignment();
                    if ((cfg.getTargetPrinterPresentationLUTMatchRequired(cfgname)) &&
                        (! basicFilmBoxList.matchesPresentationLUT(referencedPresentationLUTAlignment)))
                    {
                      if (verboseMode)
                      {
                        logstream->lockCerr() << "cannot update Basic Film Session: referenced presentation LUT number of entries does not match image bit depth." << endl;
                        logstream->unlockCerr();
                      }
                      rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                      result = OFFalse;
                    } else {
                      // referenced presentation LUT sequence is OK                      
                      overrideFilmBoxPLUTSettings = OFTrue;
                      DcmSequenceOfItems *newSeq = new DcmSequenceOfItems(*seq);
                      if (newSeq) rspDataset->insert(newSeq, OFTrue /*replaceOld*/);
                      else 
                      {
                        writeresult = EC_MemoryExhausted;
                      }
                    }
                  }
               } else {
                  if (verboseMode)
                  {
                    logstream->lockCerr() << "cannot update Basic Film Session: no referenced SOP class UID in referenced presentation LUT sequence" << endl;
                    logstream->unlockCerr();
                  }
                  rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
                  result = OFFalse;
               }
             }
           } else {
             if (verboseMode)
             {
               logstream->lockCerr() << "cannot update Basic Film Session: no referenced SOP instance UID in referenced presentation LUT sequence" << endl;
               logstream->unlockCerr();
             }
             rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
             result = OFFalse;
           }
        } else {
          if (verboseMode)
          {
            logstream->lockCerr() << "cannot update Basic Film Session: referenced presentation LUT sequence number of items != 1" << endl;
            logstream->unlockCerr();
          }
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
          result = OFFalse;
        }
      }
    }    
    
  } /* if presentationLUTnegotiated */

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    stack.clear();
    OFBool intoSub = OFTrue;
    while (EC_Normal == rqDataset->nextObject(stack, intoSub))
    {
      intoSub = OFFalse;
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag.getElement() == 0x0000) /* group length */ ;
      else if (currentTag == DCM_NumberOfCopies) /* OK */ ;
      else if (currentTag == DCM_PrintPriority) /* OK */ ;
      else if (currentTag == DCM_MediumType) /* OK */ ;
      else if (currentTag == DCM_FilmDestination) /* OK */ ;
      else if (currentTag == DCM_FilmSessionLabel) /* OK */ ;
      else if (currentTag == DCM_OwnerID) /* OK */ ;
      else if (currentTag == DCM_MemoryAllocation)
      {
        if (verboseMode)
        {
          ostream &mycerr = logstream->lockCerr();
          mycerr << "warning while updating Basic Film Session: memory allocation not supported" << endl;
          (stack.top())->print(mycerr, OFFalse);
          logstream->unlockCerr();
        }
      	if (rsp.msg.NSetRSP.DimseStatus == 0) rsp.msg.NSetRSP.DimseStatus = STATUS_N_PRINT_BFS_Warn_MemoryAllocation;
      }
      else if (currentTag == DCM_Illumination)
      {
        if (! presentationLUTnegotiated)
        {
          if (verboseMode)
          {
            ostream &mycerr = logstream->lockCerr();
            mycerr << "cannot update Basic Film Session: illumination received:" << endl;
            (stack.top())->print(mycerr, OFFalse);
            logstream->unlockCerr();
          }
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;          
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReflectedAmbientLight)
      {
        if (! presentationLUTnegotiated)
        {
          if (verboseMode)
          {
            ostream &mycerr = logstream->lockCerr();
            mycerr << "cannot update Basic Film Session: reflected ambient light received:" << endl;
            (stack.top())->print(mycerr, OFFalse);
            logstream->unlockCerr();            
          }
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;          
          result = OFFalse;
        }
      }
      else if (currentTag == DCM_ReferencedPresentationLUTSequence)
      {
        if (! presentationLUTnegotiated)
        {
          if (verboseMode)
          {
            ostream &mycerr = logstream->lockCerr();
            mycerr << "cannot update Basic Film Session: referenced presentation LUT sequence received:" << endl;
            (stack.top())->print(mycerr, OFFalse);
            logstream->unlockCerr();            
          }
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;          
          result = OFFalse;
        }
      }
      else
      {
        if (verboseMode)
        {
          ostream &mycerr = logstream->lockCerr();
          mycerr << "cannot update Basic Film Session: unsupported attribute received:" << endl;
          (stack.top())->print(mycerr, OFFalse);
          logstream->unlockCerr();
        }
      	rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchAttribute;
        result = OFFalse;
      }
    }
  }
  
  // if n-set was successful, send back response dataset
  if (result && (EC_Normal == writeresult))
  {
    rsp.msg.NSetRSP.DataSetType = DIMSE_DATASET_PRESENT;
    if (overrideFilmBoxPLUTSettings)
    {
      basicFilmBoxList.overridePresentationLUTSettings(illumination, reflectedAmbientLight, 
        referencedPresentationLUTInstanceUID, referencedPresentationLUTAlignment);
    }
  } else {
    delete rspDataset;
    rspDataset = NULL;
    if (rsp.msg.NSetRSP.DimseStatus == 0) rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
    result = OFFalse;
  }
  return result;
}

void DVPSFilmSession::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
}

OFCondition DVPSFilmSession::addPresentationLUTReference(DcmItem& dset)
{
  DcmElement *delem=NULL;
  OFCondition result = EC_Normal;

  ADD_TO_DATASET(DcmUnsignedShort, illumination)
  ADD_TO_DATASET(DcmUnsignedShort, reflectedAmbientLight)

  if (referencedPresentationLUTInstanceUID.getLength() > 0)
  {
    DcmUniqueIdentifier refsopclassuid(DCM_ReferencedSOPClassUID);
    if (result==EC_Normal) result = refsopclassuid.putString(UID_PresentationLUTSOPClass);
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(DCM_ReferencedPresentationLUTSequence);
    DcmItem *ditem = new DcmItem();
    if ((result == EC_Normal) && ditem && dseq)
    {
       ADD_TO_DATASET2(DcmUniqueIdentifier, refsopclassuid)
       ADD_TO_DATASET2(DcmUniqueIdentifier, referencedPresentationLUTInstanceUID)
       if (result==EC_Normal)
       {
         dseq->insert(ditem);
         dset.insert(dseq, OFTrue /*replaceOld*/);
       } else {
        delete dseq;
        delete ditem;
       }
    } else {
      delete dseq;
      delete ditem;
      result = EC_MemoryExhausted;
    }
  }
  return result;
}

void DVPSFilmSession::copyPresentationLUTSettings(DVPSStoredPrint& sp)
{
  sp.overridePresentationLUTSettings(illumination, reflectedAmbientLight, 
     referencedPresentationLUTInstanceUID, referencedPresentationLUTAlignment);
}

/*
 *  $Log: dvpsfs.cc,v $
 *  Revision 1.11  2002-11-27 15:48:09  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.10  2001/11/28 13:56:53  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.9  2001/09/26 15:36:24  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.8  2001/06/01 15:50:30  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2001/05/25 10:07:56  meichel
 *  Corrected some DIMSE error status codes for Print SCP
 *
 *  Revision 1.6  2000/09/06 08:55:36  meichel
 *  Updated Print SCP to accept and silently ignore group length attributes.
 *
 *  Revision 1.5  2000/06/08 10:44:34  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.4  2000/06/07 13:17:06  meichel
 *  now using DIMSE status constants and log facilities defined in dcmnet
 *
 *  Revision 1.3  2000/06/02 16:00:59  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/06/02 12:45:05  joergr
 *  Removed const type specifier to avoid compiler warnings reported by MSVC.
 *
 *  Revision 1.1  2000/05/31 12:58:11  meichel
 *  Added initial Print SCP support
 *
 *
 */


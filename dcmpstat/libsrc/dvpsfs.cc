/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Update Date:      $Date: 2000-06-02 16:00:59 $
 *  CVS/RCS Revision: $Revision: 1.3 $
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

#include <string.h>

/* --------------- class DVPSFilmSession --------------- */

DVPSFilmSession::DVPSFilmSession()
: sopInstanceUID(NULL)
, numberOfCopies(DCM_NumberOfCopies)
, printPriority(DCM_PrintPriority)
, mediumType(DCM_MediumType)
, filmDestination(DCM_FilmDestination)
, filmSessionLabel(DCM_FilmSessionLabel)
, ownerID(DCM_OwnerID)
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSFilmSession::DVPSFilmSession(const DVPSFilmSession& copy)
: sopInstanceUID(copy.sopInstanceUID)
, numberOfCopies(copy.numberOfCopies)
, printPriority(copy.printPriority)
, mediumType(copy.mediumType)
, filmDestination(copy.filmDestination)
, filmSessionLabel(copy.filmSessionLabel)
, ownerID(copy.ownerID)
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

OFBool DVPSFilmSession::printSCPCreate(DVConfiguration& cfg, const char *cfgname, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset, const char *peerae)
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
        rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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
        rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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
        rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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
        rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, OFFalse))
    {
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag == DCM_NumberOfCopies) /* OK */ ;
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
      	if (rsp.msg.NCreateRSP.DimseStatus == 0) rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_BFS_Warn_MemoryAllocation;
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
      	rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_AttributeListError;
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
      E_Condition writeresult = EC_Normal;
      DcmElement *delem = NULL;
      
      ADD_TO_PDATASET(DcmIntegerString, numberOfCopies)
      ADD_TO_PDATASET(DcmCodeString, printPriority)
      ADD_TO_PDATASET(DcmCodeString, mediumType)
      ADD_TO_PDATASET(DcmCodeString, filmDestination)
      ADD_TO_PDATASET(DcmLongString, filmSessionLabel)
      ADD_TO_PDATASET(DcmShortString, ownerID)

      if (EC_Normal == writeresult)
      {
        rsp.msg.NCreateRSP.DataSetType = DIMSE_DATASET_PRESENT;
      } else {
      	delete rspDataset;
      	rspDataset = NULL;
        rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_ProcessingFailure;
        result = OFFalse;
      }     
    } else {
      rsp.msg.NCreateRSP.DimseStatus = DIMSE_N_ProcessingFailure;
      result = OFFalse;
    }
  }
  return result;
}


OFBool DVPSFilmSession::printSCPSet(DVConfiguration& cfg, const char *cfgname, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset)
{
  OFBool result = OFTrue;
  DcmStack stack;
  E_Condition writeresult = EC_Normal;
  DcmElement *delem = NULL;

  rspDataset = new DcmDataset;    
  if ((rqDataset == NULL)||(rspDataset == NULL))
  {
    rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
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
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_InvalidAttributeValue;
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

  // browse through rqDataset and check for unsupported attributes
  if (result && rqDataset)
  {
    stack.clear();
    while (EC_Normal == rqDataset->nextObject(stack, OFFalse))
    {
      const DcmTagKey& currentTag = (stack.top())->getTag();
      if (currentTag == DCM_NumberOfCopies) /* OK */ ;
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
      	if (rsp.msg.NSetRSP.DimseStatus == 0) rsp.msg.NSetRSP.DimseStatus = DIMSE_N_BFS_Warn_MemoryAllocation;
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
      	rsp.msg.NSetRSP.DimseStatus = DIMSE_N_AttributeListError;
        result = OFFalse;
      }
    }
  }
  
  // if n-set was successful, send back response dataset
  if (result && (EC_Normal == writeresult))
  {
    rsp.msg.NSetRSP.DataSetType = DIMSE_DATASET_PRESENT;
  } else {
    delete rspDataset;
    rspDataset = NULL;
    if (rsp.msg.NSetRSP.DimseStatus == 0) rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
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

/*
 *  $Log: dvpsfs.cc,v $
 *  Revision 1.3  2000-06-02 16:00:59  meichel
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


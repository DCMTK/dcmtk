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
 *    classes: DVPSStoredPrint_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-06-02 16:01:07 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsspl.h"
#include "dvpssp.h"      /* for DVPSStoredPrint */
#include "dvpsib.h"      /* for DVPSImageBoxContent */
#include "dviface.h"
#include "dvpsdef.h"

DVPSStoredPrint_PList::DVPSStoredPrint_PList()
: OFList<DVPSStoredPrint *>()
, logstream(&ofConsole)
, verboseMode(OFFalse)
, debugMode(OFFalse)
{
}

DVPSStoredPrint_PList::DVPSStoredPrint_PList(const DVPSStoredPrint_PList &arg)
: OFList<DVPSStoredPrint *>()
, logstream(arg.logstream)
, verboseMode(arg.verboseMode)
, debugMode(arg.debugMode)
{
  OFListIterator(DVPSStoredPrint *) first = arg.begin();
  OFListIterator(DVPSStoredPrint *) last = arg.end();
  while (first != last)
  {
    push_back((*first)->clone());
    ++first;
  }
}

DVPSStoredPrint_PList::~DVPSStoredPrint_PList()
{
  clear();
}

void DVPSStoredPrint_PList::clear()
{
  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  while (first != last)
  {
    delete (*first);
    first = erase(first);
  }
}

void DVPSStoredPrint_PList::printSCPBasicFilmBoxSet(
    DVConfiguration& cfg,
    const char *cfgname,
    T_DIMSE_Message& rq,
    DcmDataset *rqDataset,
    T_DIMSE_Message& rsp,
    DcmDataset *& rspDataset,
    OFBool presentationLUTnegotiated,
    DVPSPresentationLUT_PList& globalPresentationLUTList)
{
  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  OFBool found = OFFalse;
  while ((first != last) && (!found))
  {
    if ((*first)->isFilmBoxInstance(rq.msg.NSetRQ.RequestedSOPInstanceUID)) found = OFTrue;
    else ++first;
  }

  if (found)
  {
    DVPSStoredPrint *newSP = new DVPSStoredPrint(*(*first));
    if (newSP)
    {
      if (newSP->printSCPSet(cfg, cfgname, rqDataset, rsp, rspDataset, presentationLUTnegotiated, globalPresentationLUTList))
      {
        // N-SET successful, replace entry in list
        delete (*first);
        erase(first);
        push_back(newSP);
      } else delete newSP;
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "error: cannot update film box, out of memory." << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
    }
  } else {
    // film box does not exist or wrong instance UID
    if (verboseMode)
    {
      logstream->lockCerr() << "error: cannot update film box, object not found." << endl;
      logstream->unlockCerr();
    }
    rsp.msg.NSetRSP.DimseStatus = DIMSE_N_NoSuchObjectInstance;
  }
}


void DVPSStoredPrint_PList::printSCPBasicGrayscaleImageBoxSet(
    DVInterface& cfg,
    const char *cfgname,
    T_DIMSE_Message& rq,
    DcmDataset *rqDataset,
    T_DIMSE_Message& rsp,
    DcmDataset *& rspDataset)
{
  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  DVPSStoredPrint *sp = NULL;
  DVPSImageBoxContent *newib = NULL;
  while ((first != last) && (sp == NULL))
  {
    newib = (*first)->duplicateImageBox(rq.msg.NSetRQ.RequestedSOPInstanceUID);
    if (newib) sp = *first; else ++first;
  }

  if (newib && sp)
  {
    DcmFileFormat imageFile;
    DcmDataset *imageDataset = imageFile.getDataset();

    if (newib->printSCPSet(cfg, cfgname, rqDataset, rsp, rspDataset, *imageDataset, sp->getReferencedPresentationLUTAlignment()))
    {
      if (EC_Normal == sp->writeHardcopyImageAttributes(*imageDataset))
      {
        // check for image position clash
        if (sp->haveImagePositionClash(rq.msg.NSetRQ.RequestedSOPInstanceUID, newib->getImageBoxPosition()))
        {
          delete rspDataset;
          rspDataset = NULL;
          if (verboseMode)
          {
            logstream->lockCerr() << "error: cannot update basic grayscale image box, image position collision." << endl;
            logstream->unlockCerr();
          }
          rsp.msg.NSetRSP.DimseStatus = DIMSE_N_BFS_BFB_Fail_PositionCollision;
        } else {
          if (EC_Normal == cfg.saveFileFormatToDB(imageFile))
          {
            sp->replaceImageBox(newib);
          } else {
            delete rspDataset;
            rspDataset = NULL;
            rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
          }
        }
      } else {
        delete rspDataset;
        rspDataset = NULL;
        if (verboseMode)
        {
          logstream->lockCerr() << "error: cannot update basic grayscale image box, out of memory." << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NSetRSP.DimseStatus = DIMSE_N_ProcessingFailure;
      }
    }
  } else {
    // image box does not exist or wrong instance UID
    if (verboseMode)
    {
      logstream->lockCerr() << "error: cannot update basic grayscale image box, object not found." << endl;
      logstream->unlockCerr();
    }
    rsp.msg.NSetRSP.DimseStatus = DIMSE_N_NoSuchObjectInstance;
  }
}


void DVPSStoredPrint_PList::printSCPBasicFilmBoxAction(
    DVInterface& cfg,
    const char *cfgname,
    T_DIMSE_Message& rq,
    T_DIMSE_Message& rsp,
    DVPSPresentationLUT_PList& globalPresentationLUTList)
{
  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  OFBool found = OFFalse;
  while ((first != last) && (!found))
  {
    if ((*first)->isFilmBoxInstance(rq.msg.NActionRQ.RequestedSOPInstanceUID)) found = OFTrue;
    else ++first;
  }
  if (found)
  {
    DcmFileFormat spFile;
    DcmDataset *spDataset = spFile.getDataset();
    DVPSStoredPrint *sp = *first;
    OFBool writeRequestedImageSize = cfg.getTargetPrinterSupportsRequestedImageSize(cfgname);
    sp->updatePresentationLUTList(globalPresentationLUTList);
    sp->clearInstanceUID();

    if (EC_Normal == sp->write(*spDataset, writeRequestedImageSize, OFFalse, OFFalse, OFTrue))
    {
      if (EC_Normal == cfg.saveFileFormatToDB(spFile))
      {
        // N-ACTION successful.
      } else {
        rsp.msg.NActionRSP.DimseStatus = DIMSE_N_ProcessingFailure;
      }
    } else {
      if (verboseMode)
      {
        logstream->lockCerr() << "error: cannot print basic film box, out of memory." << endl;
        logstream->unlockCerr();
      }
      rsp.msg.NActionRSP.DimseStatus = DIMSE_N_ProcessingFailure;
    }

  } else {
    // film box does not exist or wrong instance UID
    if (verboseMode)
    {
      logstream->lockCerr() << "error: cannot print film box, object not found." << endl;
      logstream->unlockCerr();
    }
    rsp.msg.NActionRSP.DimseStatus = DIMSE_N_NoSuchObjectInstance;
  }
}


void DVPSStoredPrint_PList::printSCPBasicFilmSessionAction(
    DVInterface& cfg,
    const char *cfgname,
    T_DIMSE_Message& rsp,
    DVPSPresentationLUT_PList& globalPresentationLUTList)
{
  if (size() > 0)
  {
    OFBool writeRequestedImageSize = cfg.getTargetPrinterSupportsRequestedImageSize(cfgname);
    OFListIterator(DVPSStoredPrint *) first = begin();
    OFListIterator(DVPSStoredPrint *) last = end();
    while (first != last)
    {
      DcmFileFormat spFile;
      DcmDataset *spDataset = spFile.getDataset();
      (*first)->updatePresentationLUTList(globalPresentationLUTList);
      (*first)->clearInstanceUID();

      if (EC_Normal == (*first)->write(*spDataset, writeRequestedImageSize, OFFalse, OFFalse, OFTrue))
      {
        if (EC_Normal == cfg.saveFileFormatToDB(spFile))
        {
          // success for this film box
        } else {
          rsp.msg.NActionRSP.DimseStatus = DIMSE_N_ProcessingFailure;
        }
      } else {
        if (verboseMode)
        {
          logstream->lockCerr() << "error: cannot print basic film session, out of memory." << endl;
          logstream->unlockCerr();
        }
        rsp.msg.NActionRSP.DimseStatus = DIMSE_N_ProcessingFailure;
      }
      ++first;
    }
  } else {
    // no film boxes to print
    if (verboseMode)
    {
      logstream->lockCerr() << "error: cannot print film session, no film box." << endl;
      logstream->unlockCerr();
    }
    rsp.msg.NActionRSP.DimseStatus = DIMSE_N_BFS_Fail_NoFilmBox;
  }
}

void DVPSStoredPrint_PList::printSCPBasicFilmBoxDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{

  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  OFBool found = OFFalse;
  while ((first != last) && (!found))
  {
    if ((*first)->isFilmBoxInstance(rq.msg.NDeleteRQ.RequestedSOPInstanceUID)) found = OFTrue;
    else ++first;
  }

  if (found)
  {
    delete (*first);
    erase(first);
  } else {
    // film box does not exist or wrong instance UID
    if (verboseMode)
    {
      logstream->lockCerr() << "error: cannot delete film box with instance UID '" << rq.msg.NDeleteRQ.RequestedSOPInstanceUID << "': object does not exist." << endl;
      logstream->unlockCerr();
    }
    rsp.msg.NDeleteRSP.DimseStatus = DIMSE_N_NoSuchObjectInstance;
  }
}

OFBool DVPSStoredPrint_PList::haveFilmBoxInstance(const char *uid)
{
  if (uid==NULL) return OFFalse;

  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  while (first != last)
  {
    if ((*first)->isFilmBoxInstance(uid)) return OFTrue;
    else ++first;
  }
  return OFFalse;
}

OFBool DVPSStoredPrint_PList::usesPresentationLUT(const char *uid)
{
  if (uid==NULL) return OFFalse;

  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  while (first != last)
  {
    if ((*first)->usesPresentationLUT(uid)) return OFTrue;
    else ++first;
  }
  return OFFalse;
}

void DVPSStoredPrint_PList::setLog(OFConsole *stream, OFBool verbMode, OFBool dbgMode)
{
  if (stream) logstream = stream; else logstream = &ofConsole;
  verboseMode = verbMode;
  debugMode = dbgMode;
  OFListIterator(DVPSStoredPrint *) first = begin();
  OFListIterator(DVPSStoredPrint *) last = end();
  while (first != last)
  {
    (*first)->setLog(logstream, verbMode, dbgMode);
    ++first;
  }	
}


/*
 *  $Log: dvpsspl.cc,v $
 *  Revision 1.2  2000-06-02 16:01:07  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.1  2000/05/31 12:58:12  meichel
 *  Added initial Print SCP support
 *
 *
 */


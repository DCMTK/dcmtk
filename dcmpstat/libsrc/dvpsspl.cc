/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSStoredPrint_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpsspl.h"
#include "dcmtk/dcmpstat/dvpssp.h"      /* for DVPSStoredPrint */
#include "dcmtk/dcmpstat/dvpsib.h"      /* for DVPSImageBoxContent */
#include "dcmtk/dcmpstat/dviface.h"
#include "dcmtk/dcmpstat/dvpsdef.h"

#include "dcmtk/dcmpstat/dvpsov.h"      /* for DVPSOverlay, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgl.h"      /* for DVPSGraphicLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsrs.h"      /* for DVPSReferencedSeries, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsal.h"      /* for DVPSOverlayCurveActivationLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsga.h"      /* for DVPSGraphicAnnotation, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpscu.h"      /* for DVPSCurve, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsvl.h"      /* for DVPSVOILUT, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsvw.h"      /* for DVPSVOIWindow, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsda.h"      /* for DVPSDisplayedArea, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpssv.h"      /* for DVPSSoftcopyVOI, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsab.h"      /* for DVPSAnnotationContent, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpstx.h"      /* for DVPSTextObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgr.h"      /* for DVPSGraphicObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsri.h"      /* for DVPSReferencedImage, needed by MSVC5 with STL */

DVPSStoredPrint_PList::DVPSStoredPrint_PList()
: list_()
{
}

DVPSStoredPrint_PList::DVPSStoredPrint_PList(const DVPSStoredPrint_PList &arg)
: list_()
{
  OFListConstIterator(DVPSStoredPrint *) first = arg.list_.begin();
  OFListConstIterator(DVPSStoredPrint *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSStoredPrint_PList::~DVPSStoredPrint_PList()
{
  clear();
}

void DVPSStoredPrint_PList::clear()
{
  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
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
  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();
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
        list_.erase(first);
        list_.push_back(newSP);
      } else delete newSP;
    } else {
      DCMPSTAT_INFO("cannot update film box, out of memory.");
      rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
    }
  } else {
    // film box does not exist or wrong instance UID
    DCMPSTAT_INFO("cannot update film box, object not found.");
    rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchObjectInstance;
  }
}


void DVPSStoredPrint_PList::printSCPBasicGrayscaleImageBoxSet(
    DVInterface& cfg,
    const char *cfgname,
    T_DIMSE_Message& rq,
    DcmDataset *rqDataset,
    T_DIMSE_Message& rsp,
    DcmDataset *& rspDataset,
    OFBool presentationLUTnegotiated)
{
  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();
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

    if (newib->printSCPSet(cfg, cfgname, rqDataset, rsp, rspDataset, *imageDataset, 
        sp->getReferencedPresentationLUTAlignment(), presentationLUTnegotiated))
    {
      if (EC_Normal == sp->writeHardcopyImageAttributes(*imageDataset))
      {
        // check for image position clash
        if (sp->haveImagePositionClash(rq.msg.NSetRQ.RequestedSOPInstanceUID, newib->getImageBoxPosition()))
        {
          delete rspDataset;
          rspDataset = NULL;
          DCMPSTAT_INFO("cannot update basic grayscale image box, image position collision.");
          rsp.msg.NSetRSP.DimseStatus = STATUS_N_InvalidAttributeValue;
        } else {
          if (EC_Normal == cfg.saveFileFormatToDB(imageFile))
          {
            sp->replaceImageBox(newib);
          } else {
            delete rspDataset;
            rspDataset = NULL;
            rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
          }
        }
      } else {
        delete rspDataset;
        rspDataset = NULL;
        DCMPSTAT_INFO("cannot update basic grayscale image box, out of memory.");
        rsp.msg.NSetRSP.DimseStatus = STATUS_N_ProcessingFailure;
      }
    }
  } else {
    // image box does not exist or wrong instance UID
    DCMPSTAT_INFO("cannot update basic grayscale image box, object not found.");
    rsp.msg.NSetRSP.DimseStatus = STATUS_N_NoSuchObjectInstance;
  }
}


void DVPSStoredPrint_PList::printSCPBasicFilmBoxAction(
    DVInterface& cfg,
    const char *cfgname,
    T_DIMSE_Message& rq,
    T_DIMSE_Message& rsp,
    DVPSPresentationLUT_PList& globalPresentationLUTList)
{
  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();
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

    if ((*first)->emptyPageWarning())
    {
      DCMPSTAT_INFO("empty page, will not be stored in database");
      if (STATUS_Success == rsp.msg.NActionRSP.DimseStatus) rsp.msg.NActionRSP.DimseStatus = STATUS_N_PRINT_BFB_Warn_EmptyPage;
    } else {
      if (EC_Normal == sp->write(*spDataset, writeRequestedImageSize, OFFalse, OFFalse, OFTrue))
      {
        if (EC_Normal == cfg.saveFileFormatToDB(spFile))
        {
          // N-ACTION successful.
        } else {
          rsp.msg.NActionRSP.DimseStatus = STATUS_N_ProcessingFailure;
        }
      } else {
        DCMPSTAT_INFO("cannot print basic film box, out of memory.");
        rsp.msg.NActionRSP.DimseStatus = STATUS_N_ProcessingFailure;
      }
    }
  } else {
    // film box does not exist or wrong instance UID
    DCMPSTAT_INFO("cannot print film box, object not found.");
    rsp.msg.NActionRSP.DimseStatus = STATUS_N_NoSuchObjectInstance;
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
    OFListIterator(DVPSStoredPrint *) first = list_.begin();
    OFListIterator(DVPSStoredPrint *) last = list_.end();
    while (first != last)
    {
      DcmFileFormat spFile;
      DcmDataset *spDataset = spFile.getDataset();
      (*first)->updatePresentationLUTList(globalPresentationLUTList);
      (*first)->clearInstanceUID();

      if ((*first)->emptyPageWarning())
      {
        DCMPSTAT_INFO("empty page, will not be stored in database");
        if (STATUS_Success == rsp.msg.NActionRSP.DimseStatus) rsp.msg.NActionRSP.DimseStatus = STATUS_N_PRINT_BFS_Warn_EmptyPage;
      } else {
         if (EC_Normal == (*first)->write(*spDataset, writeRequestedImageSize, OFFalse, OFFalse, OFTrue))
         {
           if (EC_Normal == cfg.saveFileFormatToDB(spFile))
           {
             // success for this film box
           } else {
             rsp.msg.NActionRSP.DimseStatus = STATUS_N_ProcessingFailure;
           }
         } else {
           DCMPSTAT_INFO("cannot print basic film session, out of memory.");
           rsp.msg.NActionRSP.DimseStatus = STATUS_N_ProcessingFailure;
         }
      }
      ++first;
    }
  } else {
    // no film boxes to print
    DCMPSTAT_INFO("cannot print film session, no film box.");
    rsp.msg.NActionRSP.DimseStatus = STATUS_N_PRINT_BFS_Fail_NoFilmBox;
  }
}

void DVPSStoredPrint_PList::printSCPBasicFilmBoxDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp)
{

  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();
  OFBool found = OFFalse;
  while ((first != last) && (!found))
  {
    if ((*first)->isFilmBoxInstance(rq.msg.NDeleteRQ.RequestedSOPInstanceUID)) found = OFTrue;
    else ++first;
  }

  if (found)
  {
    delete (*first);
    list_.erase(first);
  } else {
    // film box does not exist or wrong instance UID
    DCMPSTAT_INFO("cannot delete film box with instance UID '" << rq.msg.NDeleteRQ.RequestedSOPInstanceUID << "': object does not exist.");
    rsp.msg.NDeleteRSP.DimseStatus = STATUS_N_NoSuchObjectInstance;
  }
}

OFBool DVPSStoredPrint_PList::haveFilmBoxInstance(const char *uid)
{
  if (uid==NULL) return OFFalse;

  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();
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

  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();
  while (first != last)
  {
    if ((*first)->usesPresentationLUT(uid)) return OFTrue;
    else ++first;
  }
  return OFFalse;
}

OFBool DVPSStoredPrint_PList::matchesPresentationLUT(DVPSPrintPresentationLUTAlignment align) const
{
  OFBool result = OFTrue;
  OFListConstIterator(DVPSStoredPrint *) first = list_.begin();
  OFListConstIterator(DVPSStoredPrint *) last = list_.end();  
  while (first != last)
  {
    result = result && (*first)->matchesPresentationLUT(align);
    ++first;
  }  
  return result;
}

void DVPSStoredPrint_PList::overridePresentationLUTSettings(
      DcmUnsignedShort& newIllumination,
      DcmUnsignedShort& newReflectedAmbientLight,
      DcmUniqueIdentifier& newReferencedPLUT,
      DVPSPrintPresentationLUTAlignment newAlignment)
{
  OFListIterator(DVPSStoredPrint *) first = list_.begin();
  OFListIterator(DVPSStoredPrint *) last = list_.end();  
  while (first != last)
  {
    (*first)->overridePresentationLUTSettings(newIllumination, newReflectedAmbientLight, newReferencedPLUT, newAlignment);
    ++first;
  }  
}

/*
 *  $Log: dvpsspl.cc,v $
 *  Revision 1.14  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.13  2009-11-24 14:12:59  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.12  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.11  2005/12/08 15:46:48  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.9  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.8  2003/06/04 12:30:29  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.7  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.6  2001/06/01 15:50:38  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2001/05/25 10:07:59  meichel
 *  Corrected some DIMSE error status codes for Print SCP
 *
 *  Revision 1.4  2000/06/08 10:44:38  meichel
 *  Implemented Referenced Presentation LUT Sequence on Basic Film Session level.
 *    Empty film boxes (pages) are not written to file anymore.
 *
 *  Revision 1.3  2000/06/07 13:17:09  meichel
 *  now using DIMSE status constants and log facilities defined in dcmnet
 *
 *  Revision 1.2  2000/06/02 16:01:07  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.1  2000/05/31 12:58:12  meichel
 *  Added initial Print SCP support
 *
 *
 */


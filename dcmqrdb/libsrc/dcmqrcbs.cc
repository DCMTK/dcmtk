/*
 *
 *  Copyright (C) 1993-2009, OFFIS
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQueryRetrieveStoreContext
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-08-21 09:53:52 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrcbs.h"

#include "dcmtk/dcmqrdb/dcmqrcnf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmqrdb/dcmqropt.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmqrdb/dcmqrdbs.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"


void DcmQueryRetrieveStoreContext::updateDisplay(T_DIMSE_StoreProgress * progress)
{
  if (options_.verbose_)
  {
    switch (progress->state)
    {
      case DIMSE_StoreBegin:
        printf("RECV:");
        break;
      case DIMSE_StoreEnd:
        printf("\n");
        break;
      default:
        putchar('.');
        break;
    }
    fflush(stdout);
  }
}


void DcmQueryRetrieveStoreContext::saveImageToDB(
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    const char *imageFileName,
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **stDetail)
{
    OFCondition dbcond = EC_Normal;
    DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);

    /* Store image */
    if (options_.ignoreStoreData_) {
        rsp->DimseStatus = STATUS_Success;
        *stDetail = NULL;
        return; /* nothing else to do */
    }

    if (status == STATUS_Success)
    {
        dbcond = dbHandle.storeRequest(
            req->AffectedSOPClassUID, req->AffectedSOPInstanceUID,
            imageFileName, &dbStatus);
        if (dbcond.bad())
        {
            DcmQueryRetrieveOptions::errmsg("storeSCP: Database: storeRequest Failed (%s)",
               DU_cstoreStatusString(dbStatus.status()));
            DimseCondition::dump(dbcond);
        }
        status = dbStatus.status();
    }

    rsp->DimseStatus = status;
    *stDetail = dbStatus.extractStatusDetail();
}

void DcmQueryRetrieveStoreContext::writeToFile(
    DcmFileFormat *ff,
    const char* fname,
    T_DIMSE_C_StoreRSP *rsp)
{
    E_TransferSyntax xfer = options_.writeTransferSyntax_;
    if (xfer == EXS_Unknown) xfer = ff->getDataset()->getOriginalXfer();

    OFCondition cond = ff->saveFile(fname, xfer, options_.sequenceType_,
        options_.groupLength_, options_.paddingType_, (Uint32)options_.filepad_,
        (Uint32)options_.itempad_, (options_.useMetaheader_) ? EWM_fileformat : EWM_dataset);

    if (cond.bad())
    {
      fprintf(stderr, "storescp: Cannot write image file: %s\n", fname);
      rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
    }
}

void DcmQueryRetrieveStoreContext::checkRequestAgainstDataset(
    T_DIMSE_C_StoreRQ *req,     /* original store request */
    const char* fname,          /* filename of dataset */
    DcmDataset *dataSet,        /* dataset to check */
    T_DIMSE_C_StoreRSP *rsp,    /* final store response */
    OFBool uidPadding)          /* correct UID passing */
{
    DcmFileFormat ff;

    if (dataSet == NULL)
    {
      ff.loadFile(fname);
      dataSet = ff.getDataset();
    }

    /* which SOP class and SOP instance ? */
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if (!DU_findSOPClassAndInstanceInDataSet(dataSet, sopClass, sopInstance, uidPadding))
    {
        DcmQueryRetrieveOptions::errmsg("Bad image file: %s", fname);
        rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
    } else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0) {
        rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
    } else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0) {
        rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
    }
}

void DcmQueryRetrieveStoreContext::callbackHandler(
    /* in */
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName,       /* being received into */
    DcmDataset **imageDataSet, /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **stDetail)
{
    updateDisplay(progress);

    if (progress->state == DIMSE_StoreEnd) {

        if (!options_.ignoreStoreData_ && rsp->DimseStatus == STATUS_Success) {
            if ((imageDataSet)&&(*imageDataSet)) {
                checkRequestAgainstDataset(req, NULL, *imageDataSet, rsp, correctUIDPadding);
            } else {
                checkRequestAgainstDataset(req, imageFileName, NULL, rsp, correctUIDPadding);
            }
        }

        if (!options_.ignoreStoreData_ && rsp->DimseStatus == STATUS_Success) {
            if ((imageDataSet)&&(*imageDataSet)) {
                writeToFile(dcmff, fileName, rsp);
            }
            if (rsp->DimseStatus == STATUS_Success) {
                saveImageToDB(req, fileName, rsp, stDetail);
            }
        }

        if (options_.verbose_) {
            printf("Sending:\n");
            DIMSE_printCStoreRSP(stdout, rsp);
        } else if (rsp->DimseStatus != STATUS_Success) {
            fprintf(stdout, "NOTICE: StoreSCP:\n");
            DIMSE_printCStoreRSP(stdout, rsp);
        }
        status = rsp->DimseStatus;
    }
}


/*
 * CVS Log
 * $Log: dcmqrcbs.cc,v $
 * Revision 1.4  2009-08-21 09:53:52  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 *
 * Revision 1.3  2005/12/15 12:38:06  joergr
 * Removed naming conflicts.
 *
 * Revision 1.2  2005/12/08 15:47:07  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2005/03/30 13:34:53  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */

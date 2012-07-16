/*
 *
 *  Copyright (C) 1993-2011, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQueryRetrieveStoreContext
 *
 */

#ifndef DCMQRCBS_H
#define DCMQRCBS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmqrdb/qrdefine.h"

class DcmQueryRetrieveDatabaseHandle;
class DcmQueryRetrieveOptions;
class DcmFileFormat;

/** this class maintains the context information that is passed to the
 *  callback function called by DIMSE_storeProvider.
 */
class DCMTK_DCMQRDB_EXPORT DcmQueryRetrieveStoreContext
{
public:
    /** constructor
     *  @param handle reference to database handle
     *  @param options options for the Q/R service
     *  @param s initial DIMSE status
     *  @param ff pointer to DcmFileformat object to be used for storing the dataset
     *  @param correctuidpadding flag indicating whether space padded UIDs should be silently corrected
     */
    DcmQueryRetrieveStoreContext(
      DcmQueryRetrieveDatabaseHandle& handle,
      const DcmQueryRetrieveOptions& options,
      DIC_US s,
      DcmFileFormat *ff,
      OFBool correctuidpadding)
    : dbHandle(handle)
    , options_(options)
    , status(s)
    , fileName(NULL)
    , dcmff(ff)
    , correctUIDPadding(correctuidpadding)
    {
    }

    /** set DIMSE status
     *  param s new status
     */
    void setStatus(DIC_US s) { status = s; }

    /// return current DIMSE status
    DIC_US getStatus() const { return status; }

    /** set file name under which the image should be stored
     *  @param fn file name. String is not copied.
     */
    void setFileName(const char *fn) { fileName = fn; }

    /** callback handler called by the DIMSE_storeProvider callback function.
     *  @param progress progress state (in)
     *  @param req original store request (in)
     *  @param imageFileName being received into (in)
     *  @param imageDataSet being received into (in)
     *  @param rsp  final store response (out)
     *  @param stDetail status detail dataset (out)
     */
    void callbackHandler(
        T_DIMSE_StoreProgress *progress,
        T_DIMSE_C_StoreRQ *req,
        char *imageFileName,
        DcmDataset **imageDataSet,
        T_DIMSE_C_StoreRSP *rsp,
        DcmDataset **stDetail);

private:

    void updateDisplay(T_DIMSE_StoreProgress * progress);

    void saveImageToDB(
        T_DIMSE_C_StoreRQ *req,             /* original store request */
        const char *imageFileName,
        /* out */
        T_DIMSE_C_StoreRSP *rsp,            /* final store response */
        DcmDataset **stDetail);

    void writeToFile(
        DcmFileFormat *ff,
        const char* fname,
        T_DIMSE_C_StoreRSP *rsp);

    void checkRequestAgainstDataset(
        T_DIMSE_C_StoreRQ *req,     /* original store request */
        const char* fname,          /* filename of dataset */
        DcmDataset *dataSet,        /* dataset to check */
        T_DIMSE_C_StoreRSP *rsp,    /* final store response */
        OFBool uidPadding);         /* correct UID padding */

    /// reference to database handle
    DcmQueryRetrieveDatabaseHandle& dbHandle;

    /// reference to Q/R service options
    const DcmQueryRetrieveOptions& options_;

    /// current DIMSE status
    DIC_US status;

    /// file name under which the incoming image should be stored
    const char *fileName;

    /// DICOM file format into which the image is received
    DcmFileFormat *dcmff;

    /// flag indicating whether space padded UIDs should be silently corrected
    OFBool correctUIDPadding;

};

#endif

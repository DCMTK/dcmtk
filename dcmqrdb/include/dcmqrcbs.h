/*
 *
 *  Copyright (C) 1993-2005, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-03-30 13:34:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmqrdb/include/Attic/dcmqrcbs.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMQRCBS_H
#define DCMQRCBS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dimse.h"

class DcmQueryRetrieveDatabaseHandle;
class DcmQueryRetrieveOptions;
class DcmFileFormat;

/** this class maintains the context information that is passed to the 
 *  callback function called by DIMSE_storeProvider.
 */
class DcmQueryRetrieveStoreContext
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
        const char* fileName,
        T_DIMSE_C_StoreRSP *rsp);

    void checkRequestAgainstDataset(
        T_DIMSE_C_StoreRQ *req,     /* original store request */
        const char* fname,          /* filename of dataset */
        DcmDataset *dataSet,        /* dataset to check */
        T_DIMSE_C_StoreRSP *rsp,    /* final store response */
        OFBool correctUIDPadding);  

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

/*
 * CVS Log
 * $Log: dcmqrcbs.h,v $
 * Revision 1.1  2005-03-30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */

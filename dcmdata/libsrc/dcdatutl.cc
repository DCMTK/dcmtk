/*
 *
 *  Copyright (C) 2011-2015, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier, Michael Onken
 *
 *  Purpose: Static helper functionality for dcmdata module
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdatutl.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcfilefo.h"


// --- static helpers ---

OFCondition DcmDataUtil::getSOPInstanceFromFile(const OFFilename &filename,
                                                OFString &sopClassUID,
                                                OFString &sopInstanceUID,
                                                OFString &transferSyntaxUID,
                                                const E_FileReadMode readMode)
{
  OFCondition status = EC_IllegalParameter;
  if (!filename.isEmpty())
  {
    DCMDATA_DEBUG("getting SOP Class UID, SOP Instance UID and Transfer Syntax UID from DICOM file");
    sopClassUID.clear();
    sopInstanceUID.clear();
    transferSyntaxUID.clear();
    // prefer to load file meta information header only (since this is more efficient)
    if (readMode != ERM_dataset)
    {
      DcmMetaInfo metaInfo;
      status = metaInfo.loadFile(filename);
      if (status.good())
      {
        // try to get the UIDs from the meta-header
        DCMDATA_DEBUG("trying to get SOP Class UID, SOP Instance UID and Transfer Syntax UID from meta-header");
        metaInfo.findAndGetOFStringArray(DCM_MediaStorageSOPClassUID, sopClassUID);
        metaInfo.findAndGetOFStringArray(DCM_MediaStorageSOPInstanceUID, sopInstanceUID);
        metaInfo.findAndGetOFStringArray(DCM_TransferSyntaxUID, transferSyntaxUID);
      }
    }
    // alternatively, get UIDs from the dataset (if required and desired)
    if ((readMode != ERM_fileOnly) && (readMode != ERM_metaOnly))
    {
      if (sopClassUID.empty() || sopInstanceUID.empty())
        DCMDATA_DEBUG("no SOP Class UID and/or SOP Instance UID found in meta-header, checking dataset instead");
      if (status.bad() || sopClassUID.empty() || sopInstanceUID.empty() || transferSyntaxUID.empty())
      {
        DcmFileFormat fileformat;
        status = fileformat.loadFile(filename, EXS_Unknown, EGL_noChange, 256 /* maxReadLength */, readMode);
        if (status.good())
        {
          DcmDataset *dataset = fileformat.getDataset();
          if (dataset != NULL)
          {
            if (sopClassUID.empty())
              dataset->findAndGetOFStringArray(DCM_SOPClassUID, sopClassUID);
            if (sopInstanceUID.empty())
              dataset->findAndGetOFStringArray(DCM_SOPInstanceUID, sopInstanceUID);
            if (transferSyntaxUID.empty())
            {
              DCMDATA_DEBUG("no Transfer Syntax UID found in meta-header, trying to determine from dataset instead");
              // empty string in case of unknown/unsupported transfer syntax
              transferSyntaxUID = DcmXfer(dataset->getOriginalXfer()).getXferID();
            }
          }
        }
      }
    }
  }
  return status;
}


OFCondition DcmDataUtil::getSOPInstanceFromDataset(DcmDataset *dataset,
                                                   const E_TransferSyntax datasetXfer,
                                                   OFString &sopClassUID,
                                                   OFString &sopInstanceUID,
                                                   OFString &transferSyntaxUID)
{
  OFCondition status = EC_IllegalParameter;
  // check for invalid dataset pointer
  if (dataset != NULL)
  {
    DCMDATA_DEBUG("getting SOP Class UID, SOP Instance UID and Transfer Syntax UID from DICOM dataset");
    sopClassUID.clear();
    sopInstanceUID.clear();
    transferSyntaxUID.clear();
    // check for correct class type
    if (dataset->ident() == EVR_dataset)
    {
      // try to determine the transfer syntax of the dataset
      E_TransferSyntax xfer = datasetXfer;
      if (xfer == EXS_Unknown)
        xfer = dataset->getOriginalXfer();
      if (xfer == EXS_Unknown)
      {
        // update the internally stored transfer syntax based on the pixel data (if any)
        dataset->updateOriginalXfer();
        xfer = dataset->getOriginalXfer();
      }
      if (xfer != EXS_Unknown)
      {
        status = EC_Normal;
        // store UID of the transfers syntax in result variable
        transferSyntaxUID = DcmXfer(xfer).getXferID();
        // get other UIDs directly from the dataset
        dataset->findAndGetOFStringArray(DCM_SOPClassUID, sopClassUID);
        dataset->findAndGetOFStringArray(DCM_SOPInstanceUID, sopInstanceUID);
      } else {
        DCMDATA_DEBUG("unable to determine transfer syntax from dataset");
        status = EC_UnknownTransferSyntax;
      }
    } else
      status = EC_CorruptedData;
  }
  return status;
}

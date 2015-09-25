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
 *  Module: dcmdata
 *
 *  Author: Joerg Riesmeier, Michael Onken
 *
 *  Purpose: Static helper functionality for dcmdata module
 *
 */

#ifndef DCDATUTL_H
#define DCDATUTL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcdefine.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcdatset.h"


/** Class with helper functions used within the dcmdata module (and beyond)
 */
class DCMTK_DCMDATA_EXPORT DcmDataUtil
{

public:

  /** Get SOP Class UID, SOP Instance UID and Transfer Syntax UID from a DICOM
   *  file. The first two UID values are either copied from the meta-header
   *  (preferred) or from the dataset. The latter is either copied from the
   *  meta-header (preferred) or determined automatically (if possible).
   *  @param  filename Name of the DICOM file from which the SOP Class UID
   *          and SOP Instance UID values are retrieved
   *  @param  sopClassUID Variable to which the value of the SOP Class UID
   *          is stored
   *  @param  sopInstanceUID Variable to which the value of the SOP Instance UID
   *          is stored
   *  @param  transferSyntaxUID  Variable to which the value of the Transfer
   *          Syntax UID is stored
   *  @param  readMode Read mode passed to the DcmFileFormat::loadFile() method.
   *          If ERM_fileOnly, only the file meta information header is
   *          loaded, i.e. the behavior is identical to using
   *          ERM_metaOnly.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getSOPInstanceFromFile(const OFFilename &filename,
                                            OFString &sopClassUID,
                                            OFString &sopInstanceUID,
                                            OFString &transferSyntaxUID,
                                            const E_FileReadMode readMode = ERM_fileOnly);

  /** Get SOP Class UID, SOP Instance UID and Transfer Syntax UID from a
   *  DICOM dataset. The first two UID values are directly copied from the
   *  dataset.  The latter is either taken from the parameter 'datasetXfer' or,
   *  if it is unknown, determined automatically from the dataset (if possible).
   *  @param  dataset DICOM dataset from which the SOP Class UID and SOP
   *          Instance UID values are retrieved
   *  @param  datasetXfer Transfer syntax of the dataset (if known, otherwise
   *          set to set to EXS_Unknown and then it is determined automatically)
   *  @param  sopClassUID Variable in which the value of the SOP Class UID is
   *          stored
   *  @param  sopInstanceUID Variable in which the value of the SOP Instance UID
   *          is stored
   *  @param  transferSyntaxUID  Variable in which the value of the Transfer
   *          Syntax UID is stored
   *  @return EC_Normal if successful, an error code otherwise
   */
    static OFCondition getSOPInstanceFromDataset(DcmDataset *dataset,
                                                 const E_TransferSyntax datasetXfer,
                                                 OFString &sopClassUID,
                                                 OFString &sopInstanceUID,
                                                 OFString &transferSyntaxUID);
};

#endif // DCDATUTL_H

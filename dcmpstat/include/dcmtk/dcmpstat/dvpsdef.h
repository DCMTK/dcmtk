/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *    definitions of constants and macros for pstat module
 *
 */

#ifndef DVPSDEF_H
#define DVPSDEF_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/oflog/oflog.h"

extern DCMTK_DCMPSTAT_EXPORT OFLogger DCM_dcmpstatLogger;
extern DCMTK_DCMPSTAT_EXPORT OFLogger DCM_dcmpstatDumpLogger;
extern DCMTK_DCMPSTAT_EXPORT OFLogger DCM_dcmpstatLogfileLogger;

#define DCMPSTAT_TRACE(msg) OFLOG_TRACE(DCM_dcmpstatLogger, msg)
#define DCMPSTAT_DEBUG(msg) OFLOG_DEBUG(DCM_dcmpstatLogger, msg)
#define DCMPSTAT_INFO(msg)  OFLOG_INFO(DCM_dcmpstatLogger, msg)
#define DCMPSTAT_WARN(msg)  OFLOG_WARN(DCM_dcmpstatLogger, msg)
#define DCMPSTAT_ERROR(msg) OFLOG_ERROR(DCM_dcmpstatLogger, msg)
#define DCMPSTAT_FATAL(msg) OFLOG_FATAL(DCM_dcmpstatLogger, msg)

#define DCMPSTAT_DUMP(msg) OFLOG_DEBUG(DCM_dcmpstatDumpLogger, msg)
#define DCMPSTAT_LOGFILE(msg) OFLOG_DEBUG(DCM_dcmpstatLogfileLogger, msg)

/* default for max PDU size */
#define DEFAULT_MAXPDU 16384

#define DEFAULT_filmDestination           "DEFAULT"
#define DEFAULT_filmOrientation           "PORTRAIT"
#define DEFAULT_filmSizeID                "DEFAULT"
#define DEFAULT_illumination              2000
#define DEFAULT_imageDisplayFormat        "STANDARD\\1,1"
#define DEFAULT_imageNumber               "1"
#define DEFAULT_magnificationType         "DEFAULT"
#define DEFAULT_maxDensity                320
#define DEFAULT_mediumType                "DEFAULT"
#define DEFAULT_minDensity                20
#define DEFAULT_numberOfCopies            "1"
#define DEFAULT_ownerID                   "DEFAULT"
#define DEFAULT_patientName               "^^^^"
#define DEFAULT_presentationLabel         "UNNAMED"
#define DEFAULT_printerStatus             "NORMAL"
#define DEFAULT_printerStatusInfo         "NORMAL"
#define DEFAULT_priority                  "MED"
#define DEFAULT_reflectedAmbientLight     10
#define DEFAULT_seriesNumber              "1"
#define DEFAULT_shutterPresentationValue  0
#define DEFAULT_specificCharacterSet      "ISO_IR 100"
#define DEFAULT_trim                      "NO"

/* SOP instance UID used by the Print SCP when creating a default IDENTITY Presentation LUT */
#define WELLKNOWN_IDENTITY_PLUT_UID       "1.2.276.0.7230010.3.4.1915765545.18030.917282194.1"

/* SOP class UID used by the Print SCP when storing a DIMSE log as a DICOM file */
#define PSTAT_DIMSE_LOG_STORAGE_UID       "1.2.276.0.7230010.3.4.1915765545.18030.917282194.2"

/* size_t value indicating that no index is active or available */
#define DVPS_IDX_NONE                     ((size_t)-1)

/* max study count for DB handle creation */
#define PSTAT_MAXSTUDYCOUNT               200

/* study size for DB handle creation */
#define PSTAT_STUDYSIZE                   DB_UpperMaxBytesPerStudy

/* filename suffixes for print jobs */
#define PRINTJOB_SUFFIX                   ".job"
#define PRINTJOB_DONE_SUFFIX              ".old"
#define PRINTJOB_TEMP_SUFFIX              ".tmp"

/* default AETitle for the Presentation State viewer */
#define PSTAT_AETITLE                     "DCMPSTAT"

/* default path for database folder */
#define PSTAT_DBFOLDER                    "."

/* default path for LUT folder */
#define PSTAT_LUTFOLDER                   "."

/* default path for report folder */
#define PSTAT_REPORTFOLDER                "."

/* default path for spool folder */
#define PSTAT_SPOOLFOLDER                 "."

/* config file facility constant for high resolution graphics */
#define L2_HIGHRESOLUTIONGRAPHICS       "HIGHRESOLUTIONGRAPHICS"

#define PSTAT_DCM_LogReservation                  DcmTag(0x0009, 0x0010, EVR_LO)
#define PSTAT_DCM_AcseSequence                    DcmTag(0x0009, 0x1100, EVR_SQ)
#define PSTAT_DCM_LogSequence                     DcmTag(0x0009, 0x1200, EVR_SQ)
#define PSTAT_DCM_LogEntryType                    DcmTag(0x0009, 0x1001, EVR_CS)
#define PSTAT_DCM_LogDate                         DcmTag(0x0009, 0x1002, EVR_DA)
#define PSTAT_DCM_LogTime                         DcmTag(0x0009, 0x1003, EVR_TM)
#define PSTAT_DCM_AssociateData                   DcmTag(0x0009, 0x1004, EVR_OB)

/* --------------- a few macros which avoid copy/paste code --------------- */

// inserts a copy of a dicom data element into a dataset
#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem, OFTrue); else result=EC_MemoryExhausted;    \
}

// inserts a copy of a dicom data element into an item
#define ADD_TO_DATASET2(a_type, a_name)                             \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) ditem->insert(delem, OFTrue); else result=EC_MemoryExhausted;  \
}

// inserts a copy of a dicom data element into an item
#define ADD_TO_PDATASET(a_type, a_name)                             \
if (writeresult==EC_Normal)                                         \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) rspDataset->insert(delem, OFTrue); else writeresult=EC_MemoryExhausted; \
}

// inserts a copy of a repeating element into a dataset, assigns group number
#define ADD_REPEATING_ELEMENT_TO_DATASET(a_type, a_name, a_group)   \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem)                                                        \
  {                                                                 \
    delem->setGTag(a_group);                                        \
    dset.insert(delem, OFTrue);                                     \
  } else result=EC_MemoryExhausted;                                 \
}

// reads a dicom element from a dataset if present
#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

// reads a dicom element from an item if present
#define READ_FROM_DATASET2(a_type, a_name)                          \
stack.clear();                                                      \
if (EC_Normal == item->search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

// reads a dicom element from an item if present
#define READ_FROM_PDATASET(a_type, a_name)                          \
stack.clear();                                                      \
if (rqDataset && (EC_Normal == rqDataset->search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse))) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}

// assigns a newly generated UID to a dicom element if it is empty
#define SET_UID(a_name)                                             \
if (result==EC_Normal)                                              \
{                                                                   \
     if (a_name.getLength()==0) result = a_name.putString(dcmGenerateUniqueIdentifier(uid)); \
}

#endif

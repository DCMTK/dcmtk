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
 *    definitions of constants and macros for pstat module
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-05-31 12:56:59 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSDEF_H__
#define __DVPSDEF_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */

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

/* default path for spool folder */
#define PSTAT_SPOOLFOLDER                 "."

/* config file facility constant for high resolution graphics */
#define L2_HIGHRESOLUTIONGRAPHICS       "HIGHRESOLUTIONGRAPHICS"


/* --------------- a few macros which avoid copy/paste code --------------- */

// inserts a copy of a dicom data element into a dataset
#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
}

// inserts a copy of a dicom data element into an item
#define ADD_TO_DATASET2(a_type, a_name)                             \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) ditem->insert(delem); else result=EC_MemoryExhausted;  \
}

// inserts a copy of a dicom data element into an item
#define ADD_TO_PDATASET(a_type, a_name)                             \
if (writeresult==EC_Normal)                                         \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) rspDataset->insert(delem); else writeresult=EC_MemoryExhausted; \
}

// inserts a copy of a repeating element into a dataset, assigns group number
#define ADD_REPEATING_ELEMENT_TO_DATASET(a_type, a_name, a_group)   \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem)                                                        \
  {                                                                 \
    delem->setGTag(a_group);                                        \
    dset.insert(delem);                                             \
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
     if (a_name.getLength()==0) result = a_name.putString(dcmGenerateUniqueIdentifer(uid)); \
}

#endif

/*
 *  $Log: dvpsdef.h,v $
 *  Revision 1.1  2000-05-31 12:56:59  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *
 */

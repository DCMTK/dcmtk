/*
 *
 *  Copyright (C) 1998-2018, OFFIS e.V.
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
 *    classes: DVPSReferencedSeries
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmpstat/dvpsrs.h"
#include "dcmtk/dcmpstat/dvpsdef.h"   /* for constants and macros */
#include "dcmtk/dcmpstat/dvpsri.h"    /* for DVPSReferencedImage, needed by MSVC5 with STL */

/* --------------- class DVPSReferencedSeries --------------- */

DVPSReferencedSeries::DVPSReferencedSeries()
: referencedImageList()
, seriesInstanceUID(DCM_SeriesInstanceUID)
, retrieveAETitle(DCM_RetrieveAETitle)
, storageMediaFileSetID(DCM_StorageMediaFileSetID)
, storageMediaFileSetUID(DCM_StorageMediaFileSetUID)
{
}

DVPSReferencedSeries::DVPSReferencedSeries(const DVPSReferencedSeries& copy)
: referencedImageList(copy.referencedImageList)
, seriesInstanceUID(copy.seriesInstanceUID)
, retrieveAETitle(copy.retrieveAETitle)
, storageMediaFileSetID(copy.storageMediaFileSetID)
, storageMediaFileSetUID(copy.storageMediaFileSetUID)
{
}

DVPSReferencedSeries::~DVPSReferencedSeries()
{
}

OFCondition DVPSReferencedSeries::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, seriesInstanceUID)
  READ_FROM_DATASET(DcmApplicationEntity, EVR_AE, retrieveAETitle)
  READ_FROM_DATASET(DcmShortString, EVR_SH, storageMediaFileSetID)
  READ_FROM_DATASET(DcmUniqueIdentifier, EVR_UI, storageMediaFileSetUID)
  if (result==EC_Normal) result = referencedImageList.read(dset);

  /* Now perform basic sanity checks */

  if (seriesInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced series SQ item with seriesInstanceUID absent or empty");
  }
  else if (seriesInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced series SQ item with seriesInstanceUID VM != 1");
  }
  else if (retrieveAETitle.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced series SQ item with retrieveAETitle VM > 1");
  }
  else if (storageMediaFileSetID.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced series SQ item with storageMediaFileSetID VM > 1");
  }
  else if (storageMediaFileSetUID.getVM() > 1)
  {
    result=EC_IllegalCall;
    DCMPSTAT_WARN("presentation state contains a referenced series SQ item with storageMediaFileSetUID VM > 1");
  }

  return result;
}

OFCondition DVPSReferencedSeries::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmElement *delem=NULL;

  ADD_TO_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
  if (retrieveAETitle.getLength() > 0)
  {
    ADD_TO_DATASET(DcmApplicationEntity, retrieveAETitle)
  }
  if (storageMediaFileSetID.getLength() > 0)
  {
    ADD_TO_DATASET(DcmShortString, storageMediaFileSetID)
  }
  if (storageMediaFileSetUID.getLength() > 0)
  {
    ADD_TO_DATASET(DcmUniqueIdentifier, storageMediaFileSetUID)
  }
  if (result==EC_Normal) result = referencedImageList.write(dset);

  return result;
}

OFBool DVPSReferencedSeries::isValid(OFString& sopclassuid)
{
  return referencedImageList.isValid(sopclassuid);
}

OFBool DVPSReferencedSeries::isSeriesUID(const char *uid)
{
  OFString aString;
  if (uid && (EC_Normal == seriesInstanceUID.getOFString(aString,0)))
  {
    if (aString == uid) return OFTrue;
  }
  return OFFalse;
}

DVPSReferencedImage *DVPSReferencedSeries::findImageReference(const char *sopinstanceuid)
{
  return referencedImageList.findImageReference(sopinstanceuid);
}

void DVPSReferencedSeries::removeImageReference(const char *sopinstanceuid)
{
  referencedImageList.removeImageReference(sopinstanceuid);
  return;
}

OFCondition DVPSReferencedSeries::addImageReference(
    const char *sopclassUID,
    const char *instanceUID,
    const char *frames)
{
  return referencedImageList.addImageReference(sopclassUID, instanceUID, frames);
}

void DVPSReferencedSeries::setSeriesInstanceUID(const char *uid)
{
  if (uid) seriesInstanceUID.putString(uid); else seriesInstanceUID.clear();
  return;
}

void DVPSReferencedSeries::setRetrieveLocation(const char *aetitle, const char *filesetID, const char *filesetUID)
{
  if (aetitle) retrieveAETitle.putString(aetitle); else retrieveAETitle.clear();
  if (filesetID) storageMediaFileSetID.putString(filesetID); else storageMediaFileSetID.clear();
  if (filesetUID) storageMediaFileSetUID.putString(filesetUID); else storageMediaFileSetUID.clear();
}

const char *DVPSReferencedSeries::getRetrieveAETitle()
{
  char *c = NULL;
  if (EC_Normal == retrieveAETitle.getString(c)) return c; else return NULL;
}

const char *DVPSReferencedSeries::getStorageMediaFileSetID()
{
  char *c = NULL;
  if (EC_Normal == storageMediaFileSetID.getString(c)) return c; else return NULL;
}

const char *DVPSReferencedSeries::getStorageMediaFileSetUID()
{
  char *c = NULL;
  if (EC_Normal == storageMediaFileSetUID.getString(c)) return c; else return NULL;
}


OFCondition DVPSReferencedSeries::getImageReference(
    size_t idx,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID,
    OFString& frames,
    OFString& aetitle,
    OFString& filesetID,
    OFString& filesetUID)
{
  OFCondition result = referencedImageList.getImageReference(idx, sopclassUID, instanceUID, frames);
  if (EC_Normal == result) result = seriesInstanceUID.getOFString(seriesUID,0); // must not be empty string
  if (EC_Normal == result)
  {
    if (retrieveAETitle.getLength() == 0)
      aetitle.clear();
    else
      result = retrieveAETitle.getOFString(aetitle,0);
  }
  if (EC_Normal == result)
  {
    if (storageMediaFileSetID.getLength() == 0)
      filesetID.clear();
    else
      result = storageMediaFileSetID.getOFString(filesetID,0);
  }
  if (EC_Normal == result)
  {
    if (storageMediaFileSetUID.getLength() == 0)
      filesetUID.clear();
    else
      result = storageMediaFileSetUID.getOFString(filesetUID,0);
  }
  return result;
}

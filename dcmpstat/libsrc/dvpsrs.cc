/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *    classes: DVPSReferencedSeries
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-07-22 16:40:01 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstring.h"
#include "dvpsrs.h"

/* --------------- a few macros avoiding copy/paste --------------- */

#define ADD_TO_DATASET(a_type, a_name)                              \
if (result==EC_Normal)                                              \
{                                                                   \
  delem = new a_type(a_name);                                       \
  if (delem) dset.insert(delem); else result=EC_MemoryExhausted;    \
}

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}


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

E_Condition DVPSReferencedSeries::read(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmStack stack;

  READ_FROM_DATASET(DcmUniqueIdentifier, seriesInstanceUID)
  READ_FROM_DATASET(DcmApplicationEntity, retrieveAETitle)
  READ_FROM_DATASET(DcmShortString, storageMediaFileSetID)
  READ_FROM_DATASET(DcmUniqueIdentifier, storageMediaFileSetUID)
  if (result==EC_Normal) result = referencedImageList.read(dset);
  
  /* Now perform basic sanity checks */

  if (seriesInstanceUID.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a referenced series SQ item with seriesInstanceUID absent or empty" << endl;
#endif
  }
  else if (seriesInstanceUID.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a referenced series SQ item with seriesInstanceUID VM != 1" << endl;
#endif
  }
  else if (retrieveAETitle.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a referenced series SQ item with retrieveAETitle VM > 1" << endl;
#endif
  }
  else if (storageMediaFileSetID.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a referenced series SQ item with storageMediaFileSetID VM > 1" << endl;
#endif
  }
  else if (storageMediaFileSetUID.getVM() > 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains a referenced series SQ item with storageMediaFileSetUID VM > 1" << endl;
#endif
  }

  return result;
}

E_Condition DVPSReferencedSeries::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
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

E_Condition DVPSReferencedSeries::addImageReference(
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


E_Condition DVPSReferencedSeries::getImageReference(
    size_t idx,
    OFString& seriesUID,
    OFString& sopclassUID,
    OFString& instanceUID, 
    OFString& frames,
    OFString& aetitle,
    OFString& filesetID,
    OFString& filesetUID)
{
  E_Condition result = referencedImageList.getImageReference(idx, sopclassUID, instanceUID, frames);
  if (EC_Normal == result) result = seriesInstanceUID.getOFString(seriesUID,0);
  if (EC_Normal == result) result = retrieveAETitle.getOFString(aetitle,0);
  if (EC_Normal == result) result = storageMediaFileSetID.getOFString(filesetID,0);
  if (EC_Normal == result) result = storageMediaFileSetUID.getOFString(filesetUID,0);
  return result;
}

/*
 *  $Log: dvpsrs.cc,v $
 *  Revision 1.4  1999-07-22 16:40:01  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.3  1999/01/15 17:32:57  meichel
 *  added methods to DVPresentationState allowing to access the image
 *    references in the presentation state.  Also added methods allowing to
 *    get the width and height of the attached image.
 *
 *  Revision 1.2  1998/12/14 16:10:47  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:46  meichel
 *  Initial Release.
 *
 *
 */


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
 *    classes: DVPSOverlayCurveActivationLayer_PList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:31 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"

#include "dcmtk/dcmpstat/dvpsall.h"
#include "dcmtk/dcmpstat/dvpsal.h"      /* for DVPSOverlayCurveActivationLayer */
#include "dcmtk/dcmpstat/dvpsovl.h"     /* for DVPSOverlay_PList */
#include "dcmtk/dcmpstat/dvpsgll.h"     /* for DVPSGraphicLayer_PList */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmpstat/dvpsov.h"      /* for DVPSOverlay, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgl.h"      /* for DVPSGraphicLayer, needed by MSVC5 with STL */

DVPSOverlayCurveActivationLayer_PList::DVPSOverlayCurveActivationLayer_PList()
: list_()
{
}

DVPSOverlayCurveActivationLayer_PList::DVPSOverlayCurveActivationLayer_PList(const DVPSOverlayCurveActivationLayer_PList &arg)
: list_()
{
  OFListConstIterator(DVPSOverlayCurveActivationLayer *) first = arg.list_.begin();
  OFListConstIterator(DVPSOverlayCurveActivationLayer *) last = arg.list_.end();
  while (first != last)
  {
    list_.push_back((*first)->clone());
    ++first;
  }
}

DVPSOverlayCurveActivationLayer_PList::~DVPSOverlayCurveActivationLayer_PList()
{
  clear();
}

void DVPSOverlayCurveActivationLayer_PList::clear()
{
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    delete (*first);
    first = list_.erase(first);
  }
}

OFCondition DVPSOverlayCurveActivationLayer_PList::read(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  DcmStack stack;
  DcmTagKey key(DCM_RETIRED_CurveActivationLayer);
  DVPSOverlayCurveActivationLayer *newLayer = NULL;

  Uint16 i = 0x5000;
  while (i < 0x6020)
  {
    if (result==EC_Normal)
    {
      stack.clear();
      key.setGroup(i);
      if (EC_Normal == dset.search(key, stack, ESM_fromHere, OFFalse))
      {
        newLayer = new DVPSOverlayCurveActivationLayer();
        if (newLayer)
        {
          result = newLayer->read(dset,i);
          list_.push_back(newLayer);
        } else result = EC_MemoryExhausted;
      }
    }
    i += 2;
    if (i == 0x5020) i = 0x6000;
  }
  return result;
}


OFCondition DVPSOverlayCurveActivationLayer_PList::write(DcmItem &dset)
{
  OFCondition result = EC_Normal;
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if (result==EC_Normal) result = (*first)->write(dset);
    ++first;
  }
  return result;
}

OFCondition DVPSOverlayCurveActivationLayer_PList::createFromImage(
    DcmItem &dset,
    DVPSGraphicLayer_PList &gLayerList,
    DVPSOverlay_PList &overlayList,
    DVPSoverlayActivation overlayActivation,
    OFBool                curveActivation,
    DVPSGraphicLayering   layering)
{
  OFCondition result = EC_Normal;
  long currentLayer = 0;
  long lastOverlayLayer = 0;
  char layerName[100];
  char layerDesc[100];
  OFString aString;
  Uint16 dimensions;

  OFBool found;
  DcmStack stack;
  DcmTagKey overlayRows(DCM_OverlayRows);
  DcmTagKey overlayColumns(DCM_OverlayColumns);
  DcmTagKey overlayType(DCM_OverlayType);
  DcmTagKey overlayOrigin(DCM_OverlayOrigin);
  DcmTagKey overlayBitsAllocated(DCM_OverlayBitsAllocated);
  DcmTagKey overlayBitPosition(DCM_OverlayBitPosition);

  DcmTagKey curveDimensions(DCM_RETIRED_CurveDimensions);
  DcmTagKey numberOfPoints(DCM_RETIRED_NumberOfPoints);
  DcmTagKey typeOfData(DCM_RETIRED_TypeOfData);
  DcmTagKey dataValueRepresentation(DCM_RETIRED_DataValueRepresentation);
  DcmTagKey curveData(DCM_RETIRED_CurveData);

  OFBool haveOverlays = OFFalse;
  /* first we handle overlays */
  if ((overlayActivation==DVPSO_referenceOverlays) || (overlayActivation==DVPSO_copyOverlays))
  {
    for (Uint16 group = 0x6000; ((result==EC_Normal)&&(group < 0x6020)); group += 2)
    {
      found = OFFalse;
      /* check if we have an internal overlay with this group */
      if (overlayList.haveOverlayGroup(group)) found=OFTrue;

      /* otherwise check if we have an external overlay with this group */
      if (!found)
      {
        overlayRows.setGroup(group);
        overlayColumns.setGroup(group);
        overlayType.setGroup(group);
        overlayOrigin.setGroup(group);
        overlayBitsAllocated.setGroup(group);
        overlayBitPosition.setGroup(group);
        stack.clear();
        if (EC_Normal == dset.search(overlayRows, stack, ESM_fromHere, OFFalse)) found = OFTrue;
        stack.clear();
        if (EC_Normal != dset.search(overlayColumns, stack, ESM_fromHere, OFFalse)) found = OFFalse;
        stack.clear();
        if (EC_Normal != dset.search(overlayType, stack, ESM_fromHere, OFFalse)) found = OFFalse;
        stack.clear();
        if (EC_Normal != dset.search(overlayOrigin, stack, ESM_fromHere, OFFalse)) found = OFFalse;
        stack.clear();
        if (EC_Normal != dset.search(overlayBitsAllocated, stack, ESM_fromHere, OFFalse)) found = OFFalse;
        stack.clear();
        if (EC_Normal != dset.search(overlayBitPosition, stack, ESM_fromHere, OFFalse)) found = OFFalse;
      }

      /* if found, create graphic layer if necessary. Create activation. */
      if (found)
      {
        switch (layering)
        {
          case DVPSG_oneLayer:
            if (currentLayer==0)
            {
              currentLayer++;
              sprintf(layerName, "LAYER");
              result = gLayerList.addGraphicLayer(layerName, 1, "Overlays and Curves");
            }
            break;
          case DVPSG_twoLayers:
            if (currentLayer==0)
            {
              currentLayer++;
              sprintf(layerName, "OVERLAY");
              result = gLayerList.addGraphicLayer(layerName, 1, "Overlays");
            }
            break;
          case DVPSG_separateLayers:
            currentLayer++;
            sprintf(layerName, "OVERLAY%04ld", (long)currentLayer);
            sprintf(layerDesc, "Overlay Layer %ld", (long)currentLayer);
            result = gLayerList.addGraphicLayer(layerName, currentLayer, layerDesc);
            break;
        }
        DVPSOverlayCurveActivationLayer *newLayer = new DVPSOverlayCurveActivationLayer();
        if (newLayer)
        {
          newLayer->setActivationLayer(layerName);
          newLayer->setRepeatingGroup(group);
          list_.push_back(newLayer);
          haveOverlays = OFTrue;
        } else result = EC_MemoryExhausted;
      }
    }
  }

  lastOverlayLayer = currentLayer;
  /* then we handle curves */
  if (curveActivation)
  {
    for (Uint16 group = 0x5000; ((result==EC_Normal)&&(group < 0x5020)); group += 2)
    {
      found = OFFalse;

      /* check if we have an external curve with this group */
      curveDimensions.setGroup(group);
      numberOfPoints.setGroup(group);
      typeOfData.setGroup(group);
      dataValueRepresentation.setGroup(group);
      curveData.setGroup(group);
      stack.clear();
      if (EC_Normal == dset.search(curveDimensions, stack, ESM_fromHere, OFFalse)) found = OFTrue;
      stack.clear();
      if (EC_Normal != dset.search(numberOfPoints, stack, ESM_fromHere, OFFalse)) found = OFFalse;
      stack.clear();
      if (EC_Normal != dset.search(typeOfData, stack, ESM_fromHere, OFFalse)) found = OFFalse;
      stack.clear();
      if (EC_Normal != dset.search(dataValueRepresentation, stack, ESM_fromHere, OFFalse)) found = OFFalse;
      stack.clear();
      if (EC_Normal != dset.search(curveData, stack, ESM_fromHere, OFFalse)) found = OFFalse;

      /* if we have found a curve, make sure that this is a type of curve we can display */
      if (found)
      {
        /* read the curve dimensions and curve type from the dataset */
        DcmUnsignedShort curveDimensionsValue(DCM_RETIRED_CurveDimensions);
        DcmCodeString typeOfDataValue(DCM_RETIRED_TypeOfData);
        curveDimensionsValue.setGTag(group);
        typeOfDataValue.setGTag(group);
        READ_FROM_DATASET(DcmUnsignedShort, curveDimensionsValue)
        READ_FROM_DATASET(DcmCodeString, typeOfDataValue)

        /* we can only display POLY and ROI curves */
        aString.clear();
        typeOfDataValue.getOFString(aString,0);
        if ((aString != "POLY")&&(aString != "ROI")) found=OFFalse;

        /* we can only display 2D curves */
        dimensions=0;
        curveDimensionsValue.getUint16(dimensions,0);
        if (dimensions != 2) found=OFFalse;
      }

      /* if found, create graphic layer if necessary. Create activation. */
      if (found)
      {
        switch (layering)
        {
          case DVPSG_oneLayer:
            if (currentLayer==0)
            {
              currentLayer++;
              sprintf(layerName, "LAYER");
              result = gLayerList.addGraphicLayer(layerName, 1, "Overlays and Curves");
            }
            break;
          case DVPSG_twoLayers:
            if ((currentLayer==0)||((currentLayer==1)&&(haveOverlays)))
            {
              currentLayer++;
              sprintf(layerName, "CURVE");
              result = gLayerList.addGraphicLayer(layerName, 1, "Curves");
            }
            break;
          case DVPSG_separateLayers:
            currentLayer++;
            sprintf(layerName, "CURVE%04ld", (long)currentLayer-lastOverlayLayer);
            sprintf(layerDesc, "Curve Layer %ld", (long)currentLayer-lastOverlayLayer);
            result = gLayerList.addGraphicLayer(layerName, currentLayer, layerDesc);
            break;
        }
        DVPSOverlayCurveActivationLayer *newLayer = new DVPSOverlayCurveActivationLayer();
        if (newLayer)
        {
          newLayer->setActivationLayer(layerName);
          newLayer->setRepeatingGroup(group);
          list_.push_back(newLayer);
        } else result = EC_MemoryExhausted;
      }
    }
  }

  return result;
}

OFCondition DVPSOverlayCurveActivationLayer_PList::setActivation(Uint16 group, const char *layer)
{
  /* first we make sure we have a valid overlay group */
  OFBool result = OFFalse;
  if ((group < 0x6020)&&(group >= 0x6000)&&((group & 1) == 0)) result = OFTrue;
  if ((group < 0x5020)&&(group >= 0x5000)&&((group & 1) == 0)) result = OFTrue;
  if (!result) return EC_IllegalCall;
  if (layer==NULL) return EC_IllegalCall;

  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isRepeatingGroup(group))
    {
      (*first)->setActivationLayer(layer);
      return EC_Normal;
    }
    ++first;
  }
  DVPSOverlayCurveActivationLayer * newLayer = new DVPSOverlayCurveActivationLayer();
  if (newLayer)
  {
    newLayer->setActivationLayer(layer);
    newLayer->setRepeatingGroup(group);
    list_.push_back(newLayer);
    return EC_Normal;
  }
  return EC_MemoryExhausted;
}

void DVPSOverlayCurveActivationLayer_PList::removeActivation(Uint16 group)
{
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isRepeatingGroup(group))
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
}

const char *DVPSOverlayCurveActivationLayer_PList::getActivationLayer(Uint16 group)
{
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if ((*first)->isRepeatingGroup(group)) return (*first)->getActivationLayer();
    ++first;
  }
  return NULL;
}

void DVPSOverlayCurveActivationLayer_PList::renameLayer(const char *oldName, const char *newName)
{
  if ((oldName==NULL)||(newName==NULL)) return;

  OFString aString(oldName);
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getActivationLayer())
    {
      (*first)->setActivationLayer(newName);
    }
    ++first;
  }
  return;
}

void DVPSOverlayCurveActivationLayer_PList::removeLayer(const char *name)
{
  if (name==NULL) return;

  OFString aString(name);
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getActivationLayer())
    {
      delete (*first);
      first = list_.erase(first);
    } else ++first;
  }
  return;
}

OFBool DVPSOverlayCurveActivationLayer_PList::usesLayerName(const char *name)
{
  if (name==NULL) return OFFalse;

  OFString aString(name);
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getActivationLayer()) return OFTrue;
    ++first;
  }
  return OFFalse;
}

size_t DVPSOverlayCurveActivationLayer_PList::getNumberOfActivations(const char *layer, OFBool isCurve)
{
  if (layer==NULL) return 0;

  size_t result = 0;
  Uint16 group;
  OFString aString(layer);
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getActivationLayer())
    {
      group = (*first)->getRepeatingGroup();
      if (((isCurve) && (group < 0x6000)) || ((!isCurve) && (group >= 0x6000))) result++;
    }
    ++first;
  }
  return result;
}


Uint16 DVPSOverlayCurveActivationLayer_PList::getActivationGroup(const char *layer, size_t idx, OFBool isCurve)
{
  if (layer==NULL) return 0;

  Uint16 group;
  OFString aString(layer);
  OFListIterator(DVPSOverlayCurveActivationLayer *) first = list_.begin();
  OFListIterator(DVPSOverlayCurveActivationLayer *) last = list_.end();
  while (first != last)
  {
    if (aString == (*first)->getActivationLayer())
    {
      group = (*first)->getRepeatingGroup();
      if (((isCurve) && (group < 0x6000)) || ((!isCurve) && (group >= 0x6000)))
      {
      	if (idx==0) return group; else idx--;
      }
    }
    ++first;
  }
  return 0;
}


/*
 *  $Log: dvpsall.cc,v $
 *  Revision 1.17  2010-10-14 13:14:31  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.15  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.14  2005/12/08 15:46:19  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.13  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.12  2003/09/05 08:37:46  meichel
 *  Fixed minor issue that caused certain error messages during the
 *    parse process on a GSPS object to be "swallowed".
 *
 *  Revision 1.11  2003/06/12 18:23:11  joergr
 *  Modified code to use const_iterators where appropriate (required for STL).
 *
 *  Revision 1.10  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.9  2003/06/04 10:18:07  meichel
 *  Replaced private inheritance from template with aggregation
 *
 *  Revision 1.8  2001/09/26 15:36:23  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:50:27  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/06/02 16:00:57  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.5  2000/05/31 13:02:36  meichel
 *  Moved dcmpstat macros and constants into a common header file
 *
 *  Revision 1.4  2000/03/08 16:29:02  meichel
 *  Updated copyright header.
 *
 *  Revision 1.3  1998/12/22 17:57:14  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/14 16:10:39  meichel
 *  Implemented Presentation State interface for graphic layers,
 *    text and graphic annotations, presentation LUTs.
 *
 *  Revision 1.1  1998/11/27 14:50:39  meichel
 *  Initial Release.
 *
 *
 */

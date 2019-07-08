/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
        READ_FROM_DATASET(DcmUnsignedShort, EVR_US, curveDimensionsValue)
        READ_FROM_DATASET(DcmCodeString, EVR_CS, typeOfDataValue)

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
    const char *activationLayer = (*first)->getActivationLayer();
    if (activationLayer && (aString == activationLayer))
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
    const char *activationLayer = (*first)->getActivationLayer();
    if (activationLayer && (aString == activationLayer))
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
    const char *activationLayer = (*first)->getActivationLayer();
    if (activationLayer && (aString == activationLayer)) return OFTrue;
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
    const char *activationLayer = (*first)->getActivationLayer();
    if (activationLayer && (aString == activationLayer))
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
    const char *activationLayer = (*first)->getActivationLayer();
    if (activationLayer && (aString == activationLayer))
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

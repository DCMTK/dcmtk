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
 *    classes: DVPSOverlay
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:44 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsov.h"

/* --------------- a few macros avoiding copy/paste --------------- */

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

#define READ_FROM_DATASET(a_type, a_name)                           \
stack.clear();                                                      \
if (EC_Normal == dset.search((DcmTagKey &)a_name.getTag(), stack, ESM_fromHere, OFFalse)) \
{                                                                   \
  a_name = *((a_type *)(stack.top()));                              \
}


/* --------------- class DVPSOverlay --------------- */

DVPSOverlay::DVPSOverlay()
: overlayGroup(0)
, overlayRows(DCM_OverlayRows)
, overlayColumns(DCM_OverlayColumns)
, overlayType(DCM_OverlayType)
, overlayOrigin(DCM_OverlayOrigin)
, overlayBitsAllocated(DCM_OverlayBitsAllocated)
, overlayBitPosition(DCM_OverlayBitPosition)
, overlayData(DCM_OverlayData)
, overlayDescription(DCM_OverlayDescription)
, overlayLabel(DCM_OverlayLabel)
{
}

DVPSOverlay::DVPSOverlay(const DVPSOverlay& copy)
: overlayGroup(copy.overlayGroup)
, overlayRows(copy.overlayRows)
, overlayColumns(copy.overlayColumns)
, overlayType(copy.overlayType)
, overlayOrigin(copy.overlayOrigin)
, overlayBitsAllocated(copy.overlayBitsAllocated)
, overlayBitPosition(copy.overlayBitPosition)
, overlayData(copy.overlayData)
, overlayDescription(copy.overlayDescription)
, overlayLabel(copy.overlayLabel)
{
}

DVPSOverlay::~DVPSOverlay()
{
}

E_Condition DVPSOverlay::read(DcmItem &dset, Uint8 ovGroup)
{
  E_Condition result = EC_Normal;
  DcmStack stack;
  
  overlayGroup = ovGroup;
  Uint16 gtag = 0x6000 + ovGroup;

  overlayRows.setGTag(gtag);
  overlayColumns.setGTag(gtag);
  overlayType.setGTag(gtag);
  overlayOrigin.setGTag(gtag);
  overlayBitsAllocated.setGTag(gtag);
  overlayBitPosition.setGTag(gtag);
  overlayData.setGTag(gtag);
  overlayDescription.setGTag(gtag);
  overlayLabel.setGTag(gtag);
  READ_FROM_DATASET(DcmUnsignedShort, overlayRows)
  READ_FROM_DATASET(DcmUnsignedShort, overlayColumns)
  READ_FROM_DATASET(DcmCodeString, overlayType)
  READ_FROM_DATASET(DcmSignedShort, overlayOrigin)
  READ_FROM_DATASET(DcmUnsignedShort, overlayBitsAllocated)
  READ_FROM_DATASET(DcmUnsignedShort, overlayBitPosition)
  READ_FROM_DATASET(DcmOverlayData, overlayData)
  READ_FROM_DATASET(DcmLongString, overlayDescription)
  READ_FROM_DATASET(DcmLongString, overlayLabel)

  /* Now perform basic sanity checks and adjust use flags */
  if (overlayRows.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayRows absent or empty" << endl;
#endif
  }
  else if (overlayRows.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayRows VM != 1" << endl;
#endif
  }
  
  if (overlayColumns.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayColumns absent or empty" << endl;
#endif
  }
  else if (overlayColumns.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayColumns VM != 1" << endl;
#endif
  }

  if (overlayType.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayType absent or empty" << endl;
#endif
  }
  else if (overlayType.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayType VM != 1" << endl;
#endif
  }

  if (overlayOrigin.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayOrigin absent or empty" << endl;
#endif
  }
  else if (overlayOrigin.getVM() != 2)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayOrigin VM != 2" << endl;
#endif
  }

  if (overlayBitsAllocated.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayBitsAllocated absent or empty" << endl;
#endif
  }
  else if (overlayBitsAllocated.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayBitsAllocated VM != 1" << endl;
#endif
  }

  if (overlayBitPosition.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayBitPosition absent or empty" << endl;
#endif
  }
  else if (overlayBitPosition.getVM() != 1)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayBitPosition VM != 1" << endl;
#endif
  }

  /* in a presentation state object, the overlay data must always be present in this group */
  if (overlayData.getLength() == 0)
  {
    result=EC_IllegalCall;
#ifdef DEBUG
    cerr << "Error: presentation state contains an overlay with overlayData absent or empty" << endl;
#endif
  }

  return result;
}


E_Condition DVPSOverlay::write(DcmItem &dset)
{
  E_Condition result = EC_Normal;
  DcmElement *delem=NULL;
  Uint16 repeatingGroup = 0x6000 + overlayGroup;
  
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayRows, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayColumns, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmCodeString, overlayType, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmSignedShort, overlayOrigin, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayBitsAllocated, repeatingGroup)
  ADD_REPEATING_ELEMENT_TO_DATASET(DcmUnsignedShort, overlayBitPosition, repeatingGroup)
  if (overlayData.getLength() >0)
  {
    ADD_REPEATING_ELEMENT_TO_DATASET(DcmOverlayData, overlayData, repeatingGroup)
  }
  if (overlayDescription.getLength() >0)
  {
    ADD_REPEATING_ELEMENT_TO_DATASET(DcmLongString, overlayDescription, repeatingGroup)
  }
  if (overlayLabel.getLength() >0)
  {
    ADD_REPEATING_ELEMENT_TO_DATASET(DcmLongString, overlayLabel, repeatingGroup)
  }

  return result;
}


/*
 *  $Log: dvpsov.cc,v $
 *  Revision 1.1  1998-11-27 14:50:44  meichel
 *  Initial Release.
 *
 *
 */


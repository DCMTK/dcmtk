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
 *    classes: DVPSPrintSCP
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-05-31 12:56:36 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DVPSPRT_H__
#define __DVPSPRT_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include <iostream.h>

#include "ofstring.h"
#include "dctk.h"
#include "dvpstyp.h"         /* for enum types */
#include "dvpspll.h"         /* for class DVPSPresentationLUT_PList */
#include "dvpsspl.h"         /* for class DVPSStoredPrint_PList */
#include "assoc.h"
#include "dimse.h"

class DVInterface;
class DVPSFilmSession;

/** the representation of a Print SCP
 */  

class DVPSPrintSCP
{
 public:

  /** constructor
   *  @param iface Interface to database and config file
   *  @param cfname symbolic name of print SCP in config file
   */
  DVPSPrintSCP(DVInterface &iface, const char *cfname);

  /// destructor
  virtual ~DVPSPrintSCP();

  void handleClient(OFBool opt_verbose);

  DVPSAssociationNegotiationResult negotiateAssociation(
    T_ASC_Network &net,
    OFBool opt_verbose);


private:

  /// private undefined assignment operator
  DVPSPrintSCP& operator=(const DVPSPrintSCP&);

  /// private undefined copy constructor
  DVPSPrintSCP(const DVPSPrintSCP& copy);

  int errorCond(CONDITION cond, const char *message);

  CONDITION refuseAssociation(OFBool isBadContext);

  void dropAssociation();

  CONDITION handleNGet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  CONDITION handleNSet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  CONDITION handleNAction(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  CONDITION handleNCreate(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  CONDITION handleNDelete(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  void printerNGet(T_DIMSE_Message& rq, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  void filmSessionNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  void filmBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  void imageBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  void filmSessionNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  void filmBoxNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  void filmSessionNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  void filmBoxNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  void presentationLUTNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  void filmSessionNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  void filmBoxNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  void presentationLUTNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);
  
  /* Interface to database and config file
   */
  DVInterface& dviface;
  
  /* symbolic name of print SCP in config file, not NULL.
   */
  const char *cfgname;

  /** blocking mode for receive
   */
  T_DIMSE_BlockingMode blockMode;

  /** timeout for receive
   */
  int timeout;

  /** basic film session instance
   */
  DVPSFilmSession *filmSession;
  
  /* Presentation LUT List
   */   
  DVPSPresentationLUT_PList presentationLUTList;  

  /* Stored Print List (contains Basic Film Boxes plus hierarchy)
   */
  DVPSStoredPrint_PList storedPrintList;
 
  /* the network association over which the print SCP is operating
   */
  T_ASC_Association *assoc;

  /// study UID for stored print and hardcopy images of one print session
  DcmUniqueIdentifier      studyInstanceUID;

  /// series UID for presentation state objects
  DcmUniqueIdentifier      psSeriesInstanceUID;

  /// series UID for hardcopy image objects
  DcmUniqueIdentifier      imageSeriesInstanceUID;

};

#endif

/*
 *  $Log: dvpsprt.h,v $
 *  Revision 1.1  2000-05-31 12:56:36  meichel
 *  Added initial Print SCP support
 *
 *
 */


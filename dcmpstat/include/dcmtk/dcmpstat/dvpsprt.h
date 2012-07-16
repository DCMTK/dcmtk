/*
 *
 *  Copyright (C) 2000-2012, OFFIS e.V.
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
 *    classes: DVPSPrintSCP
 *
 */


#ifndef DVPSPRT_H
#define DVPSPRT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmpstat/dvpstyp.h"         /* for enum types */
#include "dcmtk/dcmpstat/dvpspll.h"         /* for class DVPSPresentationLUT_PList */
#include "dcmtk/dcmpstat/dvpsspl.h"         /* for class DVPSStoredPrint_PList */
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"

class DVInterface;
class DVPSFilmSession;

/** the representation of a Print Management SCP. This class implements most of the
 *  DIMSE behaviour of a DICOM Print SCP and uses Stored Print and Hardcopy Grayscale
 *  objects to store received print jobs in the local database.
 */
class DCMTK_DCMPSTAT_EXPORT DVPSPrintSCP
{
 public:

  /** constructor
   *  @param iface Interface to database and config file
   *  @param cfname symbolic name of print SCP in config file
   */
  DVPSPrintSCP(DVInterface &iface, const char *cfname);

  /// destructor
  virtual ~DVPSPrintSCP();

  /** activates or deactivates dumping of the DIMSE communication
   *  in DICOM file format. This method should be called prior to
   *  association negotiation with negotiateAssociation().
   *  @param fname full path name of the file into which a log
   *   of the DIMSE communication is written after association release.
   *   NULL disables the DICOM DIMSE dump completely.
   */
  void setDimseLogPath(const char *fname);

  /** performs association negotiation for the Print SCP. Depending on the
   *  configuration file settings, Basic Grayscale Print and Presentation LUT
   *  are accepted with all uncompressed transfer syntaxes.
   *  If association negotiation is unsuccessful, an A-ASSOCIATE-RQ is sent
   *  and the association is dropped. If successful, an A-ASSOCIATE-AC is
   *  prepared but not (yet) sent.
   *  @param net DIMSE network over which to receive the association request
   *  @return result indicating whether association negotiation was successful,
   *    unsuccessful or whether termination of the server was requested.
   */
  DVPSAssociationNegotiationResult negotiateAssociation(T_ASC_Network &net);

  /** confirms an association negotiated with negotiateAssociation() and handles
   *  all DIMSE communication for the Print SCP. Returns after the association
   *  has been released or aborted.
   */
  void handleClient();

private:

  /// private undefined assignment operator
  DVPSPrintSCP& operator=(const DVPSPrintSCP&);

  /// private undefined copy constructor
  DVPSPrintSCP(const DVPSPrintSCP& copy);

  /** if the given condition indicates an error, prints the given string
   *  to the error log and returns true (nonzero), otherwise returns false.
   *  @param cond condition to be checked
   *  @param message to be printed, must not be NULL
   *  @return nonzero if cond indicates error, zero otherwise.
   */
  int errorCond(OFCondition cond, const char *message);

  /** sends A-ASSOCIATION-RQ as the result of an unsuccesful association
   *  negotiation.
   *  @param isBadContext defines the reason for the A-ASSOCIATE-RQ.
   *    true indicates an incorrect application context, false sends back
   *    an unspecified reject with no reason and is used when termination
   *    of the server application has been initiated.
   *  @return ASC_NORMAL if successful, an error code otherwise.
   */
  OFCondition refuseAssociation(OFBool isBadContext);

  /** destroys the association managed by this object.
   */
  void dropAssociation();

  /** handles any incoming N-GET-RQ message and sends back N-GET-RSP.
   *  @param rq request message
   *  @param presID presentation context over which the message was received
   *  @return DIMSE_NORMAL if successful, an error code otherwise
   */
  OFCondition handleNGet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  /** handles any incoming N-SET-RQ message and sends back N-SET-RSP.
   *  @param rq request message
   *  @param presID presentation context over which the message was received
   *  @return DIMSE_NORMAL if successful, an error code otherwise
   */
  OFCondition handleNSet(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  /** handles any incoming N-ACTION-RQ message and sends back N-ACTION-RSP.
   *  @param rq request message
   *  @param presID presentation context over which the message was received
   *  @return DIMSE_NORMAL if successful, an error code otherwise
   */
  OFCondition handleNAction(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  /** handles any incoming N-CREATE-RQ message and sends back N-CREATE-RSP.
   *  @param rq request message
   *  @param presID presentation context over which the message was received
   *  @return DIMSE_NORMAL if successful, an error code otherwise
   */
  OFCondition handleNCreate(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  /** handles any incoming N-DELETE-RQ message and sends back N-DELETE-RSP.
   *  @param rq request message
   *  @param presID presentation context over which the message was received
   *  @return DIMSE_NORMAL if successful, an error code otherwise
   */
  OFCondition handleNDelete(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  /** handles any incoming C-ECHO-RQ message and sends back C-ECHO-RSP.
   *  @param rq request message
   *  @param presID presentation context over which the message was received
   *  @return DIMSE_NORMAL if successful, an error code otherwise
   */
  OFCondition handleCEcho(T_DIMSE_Message& rq, T_ASC_PresentationContextID presID);

  /** implements the N-GET operation for the Printer SOP Class.
   *  @param rq request message
   *  @param rsp response message, already initialized
   *  @param rspDataset response dataset passed back in this parameter (if any)
   */
  void printerNGet(T_DIMSE_Message& rq, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  /** implements the N-SET operation for the Basic Film Session SOP Class.
   *  @param rq request message
   *  @param rqDataset request dataset, may be NULL
   *  @param rsp response message, already initialized
   *  @param rspDataset response dataset passed back in this parameter (if any)
   */
  void filmSessionNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  /** implements the N-SET operation for the Basic Film Box SOP Class.
   *  @param rq request message
   *  @param rqDataset request dataset, may be NULL
   *  @param rsp response message, already initialized
   *  @param rspDataset response dataset passed back in this parameter (if any)
   */
  void filmBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  /** implements the N-SET operation for the Basic Grayscale Image Box SOP Class.
   *  @param rq request message
   *  @param rqDataset request dataset, may be NULL
   *  @param rsp response message, already initialized
   *  @param rspDataset response dataset passed back in this parameter (if any)
   */
  void imageBoxNSet(T_DIMSE_Message& rq, DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  /** implements the N-ACTION operation for the Basic Film Session SOP Class.
   *  @param rq request message
   *  @param rsp response message, already initialized
   */
  void filmSessionNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  /** implements the N-ACTION operation for the Basic Film Box SOP Class.
   *  @param rq request message
   *  @param rsp response message, already initialized
   */
  void filmBoxNAction(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  /** implements the N-CREATE operation for the Basic Film Session SOP Class.
   *  @param rqDataset request dataset, may be NULL
   *  @param rsp response message, already initialized
   *  @param rspDataset response dataset passed back in this parameter (if any)
   */
  void filmSessionNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  /** implements the N-CREATE operation for the Basic Film Box SOP Class.
   *  @param rqDataset request dataset, may be NULL
   *  @param rsp response message, already initialized
   *  @param rspDataset response dataset passed back in this parameter (if any)
   */
  void filmBoxNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  /** implements the N-CREATE operation for the Presentation LUT SOP Class.
   *  @param rqDataset request dataset, may be NULL
   *  @param rsp response message, already initialized
   *  @param rspDataset response dataset passed back in this parameter (if any)
   */
  void presentationLUTNCreate(DcmDataset *rqDataset, T_DIMSE_Message& rsp, DcmDataset *& rspDataset);

  /** implements the N-DELETE operation for the Basic Film Session SOP Class.
   *  @param rq request message
   *  @param rsp response message, already initialized
   */
  void filmSessionNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  /** implements the N-DELETE operation for the Basic Film Box SOP Class.
   *  @param rq request message
   *  @param rsp response message, already initialized
   */
  void filmBoxNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  /** implements the N-DELETE operation for the Presentation LUT SOP Class.
   *  @param rq request message
   *  @param rsp response message, already initialized
   */
  void presentationLUTNDelete(T_DIMSE_Message& rq, T_DIMSE_Message& rsp);

  /** stores the binary log of the DIMSE communication in a DICOM file
   *  in the log directory.  Called upon association release or abort.
   */
  void saveDimseLog();

  /** adds an item to the given sequence containing the current date, time
   *  and the given text.
   *  @param seq sequence to which the item is added
   *  @param text text to be added, must not be NULL
   */
  static void addLogEntry(DcmSequenceOfItems *seq, const char *text);

  /** prints a dump of the given DIMSE message to the log stream.
   *  @param msg DIMSE message to be dumped
   *  @param dataset dataset to be dumped, may be NULL
   *  @param outgoing flag defining whether we are dumping an outgoing or an
   *    incoming message.
   */
  void dumpNMessage(T_DIMSE_Message &msg, DcmItem *dataset, OFBool outgoing);


  /* class data */

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

  /// DIMSE communication logged in this object if present
  DcmSequenceOfItems       *logSequence;

  /// ACSE communication logged in this object if present
  DcmSequenceOfItems       *acseSequence;

  /// full path of the file into which the DIMSE log is written
  OFString logPath;

};

#endif

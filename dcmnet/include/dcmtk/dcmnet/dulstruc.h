/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  For further copyrights, see the following paragraphs.
 *
 */

/*
          Copyright (C) 1993, 1994, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993, 1994 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993, 1994 DICOM Central Test Node project for, and
          under contract with, the Radiological Society of North America.

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER RSNA NOR
          WASHINGTON UNIVERSITY MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          jointly owned by RSNA and Washington University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include the three paragraphs of
          the copyright notice.
*/

/*
**          DICOM 93
**        Electronic Radiology Laboratory
**      Mallinckrodt Institute of Radiology
**    Washington University School of Medicine
**
** Module Name(s):
** Author, Date:  Stephen M. Moore, 19-May-93
** Intent:
**  This header contains private typedefs for the DICOM Upper Layer
**  (DUL) protocol package.  This is to be used to compile the DUL
**  package and is not intended to be seen by the applications programmer.
**  Files at the package level should include this file to get the
**  proper typedefs and include the public file "dulprotocol.h" to
**  get the public definitions and function prototypes.  I have omitted
**  the public definitions and prototypes on purpose so that they
**  exist in only one location.
*/

#ifndef DULSTRUC_H
#define DULSTRUC_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/extneg.h"
#include "dcmtk/dcmnet/dcuserid.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dntypes.h"
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmnet/lst.h"


class DcmTransportConnection;
class DcmTransportLayer;

#define NETWORK_DISCONNECTED  2
#define NETWORK_CONNECTED 3

typedef enum {
    DUL_ASSOC_WAITINGFORACK,
    DUL_ASSOC_ESTABLISHED,
    DUL_ASSOC_RELEASED,
    DUL_ASSOC_ABORTED,
    DUL_ASSOC_DROPPED
}   DUL_ASSOC_STATE;

#define ASSOCIATION_DISCONNECTED  2
#define ASSOCIATION_ABORTED   4
#define ASSOCIATION_RELEASED    5

typedef struct {
    char keyType[40];
    int applicationFunction;
    int networkState;
    int protocolState;
    int timeout;
    unsigned long options;
    union {
  struct {
      int port;
      DcmNativeSocketType listenSocket;
      DcmTransportLayer *tLayer;
      int tLayerOwned;
  }   TCP;
    }   networkSpecific;
}   PRIVATE_NETWORKKEY;

typedef struct {
    char keyType[40];
    int applicationFunction;
    char remoteNode[64];
    DUL_ASSOC_STATE associationState;
    int protocolState;
    int networkState;
    int timeout;
    time_t timerStart;
    unsigned long maxPDVRequestor;
    unsigned long maxPDVAcceptor;
    unsigned long maxPDV;
    unsigned long maxPDVInput;
    unsigned long receiveQp1;
    unsigned long receiveQp2;
    char calledAPTitle[20];
    char callingAPTitle[20];
    char applicationContextName[68];
    char abstractSyntaxName[68];
    void *receivePDUQueue;
    DUL_PRESENTATIONCONTEXTID presentationContextID;
    DcmTransportConnection *connection;
    DUL_PDVLIST pdvList;
    int inputPDU;
    unsigned char pduHead[6];
    unsigned char nextPDUType;
    unsigned char nextPDUReserved;
    unsigned long nextPDULength;
    unsigned long compatibilityMode;
    int pdvCount;
    int pdvIndex;
    void *logHandle;
    int associatePDUFlag;
    void *associatePDU;
    unsigned long associatePDULength;
    DUL_PDV currentPDV;
    unsigned char *pdvPointer;
    unsigned long fragmentBufferLength;
    unsigned char *fragmentBuffer;
    DUL_ModeCallback *modeCallback;
}   PRIVATE_ASSOCIATIONKEY;

#define KEY_NETWORK "KEY NETWORK"
#define KEY_ASSOCIATION "KEY ASSOCIATION"

#define AE_REQUESTOR  "AE REQUESTOR"
#define AE_ACCEPTOR "AE ACCEPTOR"
#define AE_BOTH   "AE BOTH"

#define NO_PDU    1
#define PDU_HEAD  2
#define PDU_DATA  2

/* Default timeout for waiting for PDUs.  100 seconds is high,
** but used for development so we have time to do things with the
** debugger.  A lower value would be used in a production system.
*/

#define DEFAULT_TIMEOUT     100

/*  Private definitions */

typedef struct dul_subitem {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    char data[DICOM_UI_LENGTH + 1];
}   DUL_SUBITEM;

typedef struct dul_maxlength {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    unsigned long maxLength;
}   DUL_MAXLENGTH;

typedef struct {
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    unsigned short maximumOperationsInvoked;
    unsigned short maximumOperationsProvided;
}   PRV_ASYNCOPERATIONS;

typedef struct {
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    char implementationClassUID[DICOM_UI_LENGTH + 1];
}   PRV_IMPLEMENTATIONCLASSUID;

typedef struct {
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    char implementationVersionName[16 + 1];
}   PRV_IMPLEMENTATIONVERSIONNAME;

typedef struct {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    unsigned short UIDLength;
    char SOPClassUID[DICOM_UI_LENGTH + 1];
    unsigned char SCURole;
    unsigned char SCPRole;
}   PRV_SCUSCPROLE;

typedef struct dul_presentationcontext {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    unsigned char contextID;
    unsigned char rsv2;
    unsigned char result;
    unsigned char rsv3;

    DUL_SUBITEM abstractSyntax;
    LST_HEAD *transferSyntaxList;
}   PRV_PRESENTATIONCONTEXTITEM;

typedef struct user_info {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned short length;
    DUL_MAXLENGTH maxLength;                             // 51H: maximum length
    PRV_ASYNCOPERATIONS asyncOperations;                 // 53H: async operations (not yet implemented!)
    DUL_SUBITEM implementationClassUID;                  // 52H: implementation class UID
    DUL_SUBITEM implementationVersionName;               // 55H: implementation version name
    LST_HEAD *SCUSCPRoleList;                            // 54H: SCP/SCU role selection
    SOPClassExtendedNegotiationSubItemList *extNegList;  // 56H: extended negotiation
                                                         // 57H: SOP CLASS COMMON EXTENDED NEGOTIATION (not implemented)
    UserIdentityNegotiationSubItem *usrIdent;            // 58H: user identity negotiation RQ or AC
}   DUL_USERINFO;

typedef struct dul_associatepdu {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned long length;
    unsigned short protocol;
    unsigned char rsv2[2];
    char calledAPTitle[18];
    char callingAPTitle[18];
    unsigned char rsv3[32];

    DUL_SUBITEM applicationContext;
    LST_HEAD *presentationContextList;
    DUL_USERINFO userInfo;
}   PRV_ASSOCIATEPDU;

typedef struct dul_rejectreleaseabortpdu {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned long length;
    unsigned char rsv2;
    unsigned char result;
    unsigned char source;
    unsigned char reason;
}   DUL_REJECTRELEASEABORTPDU;

typedef struct dul_presentationdatavalue {
    void *reserved[2];
    unsigned long length;
    unsigned char presentationContextID;
    unsigned char messageControlHeader;
    void *data;
}   DUL_PRESENTATIONDATAVALUE;

typedef struct dul_datapdu {
    void *reserved[2];
    unsigned char type;
    unsigned char rsv1;
    unsigned long length;
    DUL_PRESENTATIONDATAVALUE presentationDataValue;
}   DUL_DATAPDU;




#define DUL_PROTOCOL      (unsigned short) 0x01

#define DUL_TYPEAPPLICATIONCONTEXT  (unsigned char)0x10
#define DUL_TYPEPRESENTATIONCONTEXTRQ (unsigned char)0x20
#define DUL_TYPEPRESENTATIONCONTEXTAC (unsigned char)0x21
#define DUL_TYPEABSTRACTSYNTAX    (unsigned char)0x30
#define DUL_TYPETRANSFERSYNTAX    (unsigned char)0x40
#define DUL_TYPEUSERINFO    (unsigned char)0x50
#define DUL_TYPEMAXLENGTH   (unsigned char)0x51
#define DUL_TYPEIMPLEMENTATIONCLASSUID  (unsigned char)0x52
#define DUL_TYPEASYNCOPERATIONS   (unsigned char)0x53
#define DUL_TYPESCUSCPROLE    (unsigned char)0x54
#define DUL_TYPEIMPLEMENTATIONVERSIONNAME (unsigned char)0x55

#define COPY_LONG_BIG(A,B) {  \
  (B)[0] = (unsigned char)((A)>>24);    \
  (B)[1] = (unsigned char)((A)>>16) ; \
  (B)[2] = (unsigned char)((A)>>8) ;  \
  (B)[3] = (unsigned char)(A) ; }
#define COPY_SHORT_BIG(A,B) { \
  (B)[0] = (unsigned char)((A)>>8);   \
  (B)[1] = (unsigned char)(A) ; }

#define EXTRACT_LONG_BIG(A,B) {     \
  (B) = (unsigned long)(A)[3]       \
    | (((unsigned long)(A)[2]) << 8)    \
    | (((unsigned long)(A)[1]) << 16)   \
    | (((unsigned long)(A)[0]) << 24);  \
  }

#define EXTRACT_SHORT_BIG(A,B)  { (B) = (unsigned short)(A)[1] | (((unsigned short)(A)[0]) << 8); }

#endif // #ifndef DULSTRUC_H

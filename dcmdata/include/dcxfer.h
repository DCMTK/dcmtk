/*
 * 
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcxfer.h
 * 
 * Purpose:
 * handling of transfer syntax
 *
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#ifndef DCXFER_H
#define DCXFER_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"

/*
** Enumerated Types
*/

typedef enum {
    EXS_UNKNOWN = -1,
    EXS_LittleEndianImplicit = 0,
    EXS_BigEndianImplicit = 1,
    EXS_LittleEndianExplicit = 2,
    EXS_BigEndianExplicit = 3,
    EXS_JPEGProcess1TransferSyntax = 4,
    EXS_JPEGProcess2_4TransferSyntax = 5,
    EXS_JPEGProcess3_5TransferSyntax = 6,
    EXS_JPEGProcess6_8TransferSyntax = 7,
    EXS_JPEGProcess7_9TransferSyntax = 8,
    EXS_JPEGProcess10_12TransferSyntax = 9,
    EXS_JPEGProcess11_13TransferSyntax = 10,
    EXS_JPEGProcess14TransferSyntax = 11,
    EXS_JPEGProcess15TransferSyntax = 12,
    EXS_JPEGProcess16_18TransferSyntax = 13,
    EXS_JPEGProcess17_19TransferSyntax = 14,
    EXS_JPEGProcess20_22TransferSyntax = 15,
    EXS_JPEGProcess21_23TransferSyntax = 16,
    EXS_JPEGProcess24_26TransferSyntax = 17,
    EXS_JPEGProcess25_27TransferSyntax = 18,
    EXS_JPEGProcess28TransferSyntax = 19,
    EXS_JPEGProcess29TransferSyntax = 20,
    EXS_JPEGProcess14SV1TransferSyntax = 21
} E_TransferSyntax;


typedef enum {
    EBO_unknown = 0,
    EBO_LittleEndian = 1,
    EBO_BigEndian = 2
} E_ByteOrder;


typedef enum {
    EVT_Implicit = 0,
    EVT_Explicit = 1
} E_VRType;


typedef enum {
    EJE_NotEncapsulated = 0,
    EJE_Encapsulated = 1
} E_JPEGEncapsulated;


/*
** Class DcmXfer for lookup up Transfer Syntax properties and readable descriptions
*/

class DcmXfer {
    char               *xferID;
    char               *xferName;
    E_TransferSyntax   xferSyn;
    E_ByteOrder        byteOrder;
    E_VRType           vrType;
    E_JPEGEncapsulated encapsulated;
    T_VR_UL            JPEGProcess8;
    T_VR_UL            JPEGProcess12;

public:
    DcmXfer( E_TransferSyntax xfer );
    DcmXfer( const char *xferName_xferID );
    DcmXfer( const DcmXfer &newXfer );
    ~DcmXfer();

    DcmXfer & operator = ( const E_TransferSyntax xfer );
    DcmXfer & operator = ( const DcmXfer &newtag );

    E_TransferSyntax getXfer();
    const char*      getXferName();
    const char*      getXferID();
    BOOL             isLittleEndian();
    BOOL             isBigEndian();
    BOOL             isImplicitVR();
    BOOL             isExplicitVR();
    BOOL             isEncapsulated();
    BOOL             isNotEncapsulated();
    T_VR_UL          getJPEGProcess8Bit();
    T_VR_UL          getJPEGProcess12Bit();
};


#endif // DCXFER_H


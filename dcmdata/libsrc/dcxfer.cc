/*
 * 
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcxfer.cc
 * 
 * Purpose:
 * handling of transfer syntax
 *
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#include <string.h>

#include "dcxfer.h"
#include "dcdebug.h"


typedef struct {
    char               *xferID;
    char               *xferName;
    E_TransferSyntax   xfer;
    E_ByteOrder        byteOrder;
    E_VRType           vrType;
    E_JPEGEncapsulated encapsulated;
    T_VR_UL            JPEGProcess8;
    T_VR_UL            JPEGProcess12;
} S_XferNames;


#define ERROR_XferName "UnknownTransferSyntax"


S_XferNames XferNames[] =
{
    { DICOM_TRANSFERLITTLEENDIAN,                     // definiert in dctypes.h
            "LittleEndianImplicit",
            EXS_LittleEndianImplicit,
            EBO_LittleEndian,
            EVT_Implicit,
            EJE_NotEncapsulated,
            0L, 0L},
    { "",                                             // illegaler Typ
            "VirtualBigEndianImplicit",
            EXS_BigEndianImplicit,
            EBO_BigEndian,
            EVT_Implicit,
            EJE_NotEncapsulated,
            0L, 0L},
    { DICOM_TRANSFERLITTLEENDIANEXPLICIT,             // definiert in dctypes.h
            "LittleEndianExplicit",
            EXS_LittleEndianExplicit,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_NotEncapsulated,
            0L, 0L},
    { DICOM_TRANSFERBIGENDIANEXPLICIT,                // definiert in dctypes.h
            "BigEndianExplicit",
            EXS_BigEndianExplicit,
            EBO_BigEndian,
            EVT_Explicit,
            EJE_NotEncapsulated,
            0L, 0L},

    { DICOM_JPEGProcess1TransferSyntax,
            "JPEG Baseline",
            EXS_JPEGProcess1TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            1L, 1L},
    { DICOM_JPEGProcess2_4TransferSyntax,
            "JPEG Extended",
            EXS_JPEGProcess2_4TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            2L ,4L },
    { DICOM_JPEGProcess3_5TransferSyntax,
            "JPEG Extended",
            EXS_JPEGProcess3_5TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            3L ,5L },
    { DICOM_JPEGProcess6_8TransferSyntax,
            "JPEG Spectral Selection, Non-hierarchical",
            EXS_JPEGProcess6_8TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            6L ,8L },
    { DICOM_JPEGProcess7_9TransferSyntax,
            "JPEG Spectral Selection, Non-hierarchical",
            EXS_JPEGProcess7_9TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            7L ,9L },
    { DICOM_JPEGProcess10_12TransferSyntax,
            "JPEG Full Progression, Non-hierarchical",
            EXS_JPEGProcess10_12TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            10L ,12L },
    { DICOM_JPEGProcess11_13TransferSyntax,
            "JPEG Full Progression, Non-hierarchical",
            EXS_JPEGProcess11_13TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            11L ,13L },
    { DICOM_JPEGProcess14TransferSyntax,
            "JPEG Lossless, Non-hierarchical",
            EXS_JPEGProcess14TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            14L ,14L },
    { DICOM_JPEGProcess15TransferSyntax,
            "JPEG Lossless, Non-hierarchical",
            EXS_JPEGProcess15TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            15L ,15L },
    { DICOM_JPEGProcess16_18TransferSyntax,
            "JPEG Extended, Hierarchical",
            EXS_JPEGProcess16_18TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            16L ,18L },
    { DICOM_JPEGProcess17_19TransferSyntax,
            "JPEG Extended, Hierarchical",
            EXS_JPEGProcess17_19TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            17L ,19L },
    { DICOM_JPEGProcess20_22TransferSyntax,
            "JPEG Spectral Selection, Hierarchical",
            EXS_JPEGProcess20_22TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            20L ,22L },
    { DICOM_JPEGProcess21_23TransferSyntax,
            "JPEG Spectral Selection, Hierarchical",
            EXS_JPEGProcess21_23TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            21L ,23L },
    { DICOM_JPEGProcess24_26TransferSyntax,
            "JPEG Full Progression, Hierarchical",
            EXS_JPEGProcess24_26TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            24L ,26L },
    { DICOM_JPEGProcess25_27TransferSyntax,
            "JPEG Full Progression, Hierarchical",
            EXS_JPEGProcess25_27TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            25L ,27L },
    { DICOM_JPEGProcess28TransferSyntax,
            "JPEG Lossless, Hierarchical",
            EXS_JPEGProcess28TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            28L ,28L },
    { DICOM_JPEGProcess29TransferSyntax,
            "JPEG Lossless, Hierarchical",
            EXS_JPEGProcess29TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            29L ,29L },
    { DICOM_JPEGProcess14SV1TransferSyntax,
            "JPEG Lossless, Hierarchical, 1st Order Prediction",
            EXS_JPEGProcess14SV1TransferSyntax,
            EBO_LittleEndian,
            EVT_Explicit,
            EJE_Encapsulated,
            14L ,14L }
   // Hier koennen weitere Transfersyntaxen eingetragen werden:
};

short DIM_OF_XferNames = 22;




// ********************************


DcmXfer::DcmXfer( E_TransferSyntax xfer )
{
Bdebug((7, "dcxfer:DcmXfer::DcmXfer(xfer=%d)", xfer ));

    int i = 0;
    while (    (i < DIM_OF_XferNames)
            && XferNames[i].xfer != xfer
          )
        i++;
debug(( 7, "found index i=[%d] for xfer=%d", i, xfer));

    if (    (i < DIM_OF_XferNames)
         && XferNames[i].xfer == xfer
       )
    {
        xferSyn       = XferNames[i].xfer;
        xferID        = XferNames[i].xferID;
        xferName      = XferNames[i].xferName;
        byteOrder     = XferNames[i].byteOrder;
        vrType        = XferNames[i].vrType;
        encapsulated  = XferNames[i].encapsulated;
        JPEGProcess8  = XferNames[i].JPEGProcess8;
        JPEGProcess12 = XferNames[i].JPEGProcess12;
    }
    else
    {
        xferSyn       = EXS_UNKNOWN;
        xferID        = "";
        xferName      = ERROR_XferName;
        byteOrder     = EBO_unknown;
        vrType        = EVT_Implicit;
        encapsulated  = EJE_NotEncapsulated;
        JPEGProcess8  = 0L;
        JPEGProcess12 = 0L;
    }
Edebug(());

}


// ********************************


DcmXfer::DcmXfer( char *xferName_xferID )
{
Bdebug((7, "dcxfer:DcmXfer::DcmXfer(char*)" ));

    char *xname = xferName_xferID;
    if ( xname == (char*)NULL )
    {
        xferSyn       = EXS_UNKNOWN;
        xferID        = "";
        xferName      = ERROR_XferName;
        byteOrder     = EBO_unknown;
        vrType        = EVT_Implicit;
        encapsulated  = EJE_NotEncapsulated;
        JPEGProcess8  = 0L;
        JPEGProcess12 = 0L;
    }
    else
    {
        int i = 0;
        while ( (i < DIM_OF_XferNames)
                && (strcmp(XferNames[i].xferID, xname) != 0) )
            i++;
debug(( 7, "found index i=[%d] in first column", i));

        if ( (i < DIM_OF_XferNames)
             && (strcmp(XferNames[i].xferID, xname) == 0) )
        {
            xferSyn       = XferNames[i].xfer;
            xferID        = XferNames[i].xferID;
            xferName      = XferNames[i].xferName;
            byteOrder     = XferNames[i].byteOrder;
            vrType        = XferNames[i].vrType;
            encapsulated  = XferNames[i].encapsulated;
            JPEGProcess8  = XferNames[i].JPEGProcess8;
            JPEGProcess12 = XferNames[i].JPEGProcess12;
        }
        else
        {
            i = 0;
            while ( (i < DIM_OF_XferNames)
                    && (strcmp(XferNames[i].xferName, xname) != 0) )
                i++;
debug(( 7, "found index i=[%d] in second column", i));

            if ( (i < DIM_OF_XferNames)
                 && (strcmp(XferNames[i].xferName, xname) == 0) )
            {
                xferSyn       = XferNames[i].xfer;
                xferID        = XferNames[i].xferID;
                xferName      = XferNames[i].xferName;
                byteOrder     = XferNames[i].byteOrder;
                vrType        = XferNames[i].vrType;
                encapsulated  = XferNames[i].encapsulated;
                JPEGProcess8  = XferNames[i].JPEGProcess8;
                JPEGProcess12 = XferNames[i].JPEGProcess12;
            }
            else
            {
                xferSyn       = EXS_UNKNOWN;
                xferID        = "";
                xferName      = ERROR_XferName;
                byteOrder     = EBO_unknown;
                vrType        = EVT_Implicit;
                encapsulated  = EJE_NotEncapsulated;
                JPEGProcess8  = 0L;
                JPEGProcess12 = 0L;
            }
        }
    }
Edebug(());

}


// ********************************


DcmXfer::DcmXfer( const DcmXfer &newXfer )
{
Bdebug((7, "dcxfer:DcmXfer::DcmXfer(DcmXfer&)" ));

    xferSyn       = newXfer.xferSyn;
    xferID        = newXfer.xferID;
    xferName      = newXfer.xferName;
    byteOrder     = newXfer.byteOrder;
    vrType        = newXfer.vrType;
    encapsulated  = newXfer.encapsulated;
    JPEGProcess8  = newXfer.JPEGProcess8;
    JPEGProcess12 = newXfer.JPEGProcess12;
Edebug(());

}


// ********************************


DcmXfer::~DcmXfer()
{
Bdebug((7, "dcxfer:DcmXfer::~DcmXfer()" ));
Edebug(());

}


// ********************************


DcmXfer & DcmXfer::operator = ( const E_TransferSyntax xfer )
{
Bdebug((6, "dcxfer:DcmXfer::= (xfer=%d)", xfer ));

    int i = 0;
    while (    (i < DIM_OF_XferNames)
            && XferNames[i].xfer != xfer
          )
        i++;
debug(( 7, "found index i=[%d] for xfer=%d", i, xfer));

    if (    (i < DIM_OF_XferNames)
         && XferNames[i].xfer == xfer
       )
    {
        xferSyn       = XferNames[i].xfer;
        xferID        = XferNames[i].xferID;
        xferName      = XferNames[i].xferName;
        byteOrder     = XferNames[i].byteOrder;
        vrType        = XferNames[i].vrType;
        encapsulated  = XferNames[i].encapsulated;
        JPEGProcess8  = XferNames[i].JPEGProcess8;
        JPEGProcess12 = XferNames[i].JPEGProcess12;
    }
    else
    {
        xferSyn       = EXS_UNKNOWN;
        xferID        = "";
        xferName      = ERROR_XferName;
        byteOrder     = EBO_unknown;
        vrType        = EVT_Implicit;
        encapsulated  = EJE_NotEncapsulated;
        JPEGProcess8  = 0L;
        JPEGProcess12 = 0L;
    }
Edebug(());

    return *this;
}


// ********************************


DcmXfer & DcmXfer::operator = ( const DcmXfer &newXfer )
{
Bdebug((6, "dcxfer:DcmXfer::= (DcmXfer&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( this != &newXfer )
    {
        xferSyn       = newXfer.xferSyn;
        xferID        = newXfer.xferID;
        xferName      = newXfer.xferName;
        byteOrder     = newXfer.byteOrder;
        vrType        = newXfer.vrType;
        encapsulated  = newXfer.encapsulated;
        JPEGProcess8  = newXfer.JPEGProcess8;
        JPEGProcess12 = newXfer.JPEGProcess12;
debug(( 4, "DcmXfer & operator = ( DcmXfer & ): xfer=(%s)", xferName ));

    }
    else
    {
debug(( 1, "dcxfer:DcmXfer::DcmXfer(DcmXfer&)  Warning: self-assignment" ));

    }
Edebug(());

    return *this;
}


// ********************************


E_TransferSyntax DcmXfer::getXfer()
{
    return xferSyn;
}


// ********************************


char* DcmXfer::getXferName()
{
    return xferName;
}


// ********************************


char *DcmXfer::getXferID()
{
    return xferID;
}


// ********************************


T_VR_UL DcmXfer::getJPEGProcess8Bit()
{
    return JPEGProcess8;
}


// ********************************


T_VR_UL DcmXfer::getJPEGProcess12Bit()
{
    return JPEGProcess12;
}


// ********************************


BOOL DcmXfer::isLittleEndian()
{
    return byteOrder == EBO_LittleEndian;
}


// ********************************


BOOL DcmXfer::isBigEndian()
{
    return byteOrder == EBO_BigEndian;
}


// ********************************


BOOL DcmXfer::isImplicitVR()
{
    return vrType == EVT_Implicit;
}


// ********************************


BOOL DcmXfer::isExplicitVR()
{
    return vrType == EVT_Explicit;
}


// ********************************


BOOL DcmXfer::isEncapsulated()
{
    return encapsulated == EJE_Encapsulated;
}


// ********************************


BOOL DcmXfer::isNotEncapsulated()
{
    return encapsulated == EJE_NotEncapsulated;
}


// ********************************




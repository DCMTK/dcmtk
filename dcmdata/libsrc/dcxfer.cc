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
 * Revision:      $Revision: 1.8 $
 * Status:        $State: Exp $
 *
 */


#include <string.h>

#include "dcxfer.h"
#include "dcuid.h"
#include "dcdebug.h"

typedef struct {
    char               *xferID;
    char               *xferName;
    E_TransferSyntax   xfer;
    E_ByteOrder        byteOrder;
    E_VRType           vrType;
    E_JPEGEncapsulated encapsulated;
    Uint32            JPEGProcess8;
    Uint32            JPEGProcess12;
} S_XferNames;


#define ERROR_XferName "UnknownTransferSyntax"


S_XferNames XferNames[] =
{
    { UID_LittleEndianImplicitTransferSyntax,                    
      "LittleEndianImplicit",
      EXS_LittleEndianImplicit,
      EBO_LittleEndian,
      EVT_Implicit,
      EJE_NotEncapsulated,
      0L, 0L},
    { "",  // illegaler Typ
      "VirtualBigEndianImplicit",
      EXS_BigEndianImplicit,
      EBO_BigEndian,
      EVT_Implicit,
      EJE_NotEncapsulated,
      0L, 0L},
    { UID_LittleEndianExplicitTransferSyntax,         
      "LittleEndianExplicit",
      EXS_LittleEndianExplicit,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L},
    { UID_BigEndianExplicitTransferSyntax,                // definiert in dctypes.h
      "BigEndianExplicit",
      EXS_BigEndianExplicit,
      EBO_BigEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L},

    { UID_JPEGProcess1TransferSyntax,
      "JPEG Baseline",
      EXS_JPEGProcess1TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      1L, 1L},
    { UID_JPEGProcess2_4TransferSyntax,
      "JPEG Extended",
      EXS_JPEGProcess2_4TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      2L ,4L },
    { UID_JPEGProcess3_5TransferSyntax,
      "JPEG Extended",
      EXS_JPEGProcess3_5TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      3L ,5L },
    { UID_JPEGProcess6_8TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical",
      EXS_JPEGProcess6_8TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      6L ,8L },
    { UID_JPEGProcess7_9TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical",
      EXS_JPEGProcess7_9TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      7L ,9L },
    { UID_JPEGProcess10_12TransferSyntax,
      "JPEG Full Progression, Non-hierarchical",
      EXS_JPEGProcess10_12TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      10L ,12L },
    { UID_JPEGProcess11_13TransferSyntax,
      "JPEG Full Progression, Non-hierarchical",
      EXS_JPEGProcess11_13TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      11L ,13L },
    { UID_JPEGProcess14TransferSyntax,
      "JPEG Lossless, Non-hierarchical",
      EXS_JPEGProcess14TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      14L ,14L },
    { UID_JPEGProcess15TransferSyntax,
      "JPEG Lossless, Non-hierarchical",
      EXS_JPEGProcess15TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      15L ,15L },
    { UID_JPEGProcess16_18TransferSyntax,
      "JPEG Extended, Hierarchical",
      EXS_JPEGProcess16_18TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      16L ,18L },
    { UID_JPEGProcess17_19TransferSyntax,
      "JPEG Extended, Hierarchical",
      EXS_JPEGProcess17_19TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      17L ,19L },
    { UID_JPEGProcess20_22TransferSyntax,
      "JPEG Spectral Selection, Hierarchical",
      EXS_JPEGProcess20_22TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      20L ,22L },
    { UID_JPEGProcess21_23TransferSyntax,
      "JPEG Spectral Selection, Hierarchical",
      EXS_JPEGProcess21_23TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      21L ,23L },
    { UID_JPEGProcess24_26TransferSyntax,
      "JPEG Full Progression, Hierarchical",
      EXS_JPEGProcess24_26TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      24L ,26L },
    { UID_JPEGProcess25_27TransferSyntax,
      "JPEG Full Progression, Hierarchical",
      EXS_JPEGProcess25_27TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      25L ,27L },
    { UID_JPEGProcess28TransferSyntax,
      "JPEG Lossless, Hierarchical",
      EXS_JPEGProcess28TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      28L ,28L },
    { UID_JPEGProcess29TransferSyntax,
      "JPEG Lossless, Hierarchical",
      EXS_JPEGProcess29TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      29L ,29L },
    { UID_JPEGProcess14SV1TransferSyntax,
      "JPEG Lossless, Hierarchical, 1st Order Prediction",
      EXS_JPEGProcess14SV1TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      14L ,14L }
    // Hier koennen weitere Transfersyntaxen eingetragen werden:
};

const int DIM_OF_XferNames = (sizeof(XferNames) / sizeof(S_XferNames));




// ********************************


DcmXfer::DcmXfer( E_TransferSyntax xfer )
{
    int i = 0;
    while (    (i < DIM_OF_XferNames)
	       && XferNames[i].xfer != xfer
	)
        i++;
    if ( (i < DIM_OF_XferNames)
	 && XferNames[i].xfer == xfer)
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
        xferSyn       = EXS_Unknown;
        xferID        = "";
        xferName      = ERROR_XferName;
        byteOrder     = EBO_unknown;
        vrType        = EVT_Implicit;
        encapsulated  = EJE_NotEncapsulated;
        JPEGProcess8  = 0L;
        JPEGProcess12 = 0L;
    }
}


// ********************************


DcmXfer::DcmXfer( const char* xferName_xferID )
{
    const char* xname = xferName_xferID;
    if ( xname == (char*)NULL )
    {
        xferSyn       = EXS_Unknown;
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
                xferSyn       = EXS_Unknown;
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
}


// ********************************


DcmXfer::DcmXfer( const DcmXfer &newXfer )
{
    xferSyn       = newXfer.xferSyn;
    xferID        = newXfer.xferID;
    xferName      = newXfer.xferName;
    byteOrder     = newXfer.byteOrder;
    vrType        = newXfer.vrType;
    encapsulated  = newXfer.encapsulated;
    JPEGProcess8  = newXfer.JPEGProcess8;
    JPEGProcess12 = newXfer.JPEGProcess12;
}


// ********************************


DcmXfer::~DcmXfer()
{
}


// ********************************


DcmXfer & DcmXfer::operator = ( const E_TransferSyntax xfer )
{
    int i = 0;
    while (    (i < DIM_OF_XferNames)
	       && XferNames[i].xfer != xfer
	)
        i++;

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
        xferSyn       = EXS_Unknown;
        xferID        = "";
        xferName      = ERROR_XferName;
        byteOrder     = EBO_unknown;
        vrType        = EVT_Implicit;
        encapsulated  = EJE_NotEncapsulated;
        JPEGProcess8  = 0L;
        JPEGProcess12 = 0L;
    }
    return *this;
}


// ********************************


DcmXfer & DcmXfer::operator = ( const DcmXfer &newXfer )
{
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
    }
    return *this;
}


// ********************************

Uint32 DcmXfer::sizeofTagHeader(DcmEVR evr)
{
    Uint32 len = 0;
    if (isExplicitVR())
    {
	// some VR's have an extended format
	switch (evr) {
	case EVR_ox :
	case EVR_OB :
	case EVR_OW :
	case EVR_SQ :
	case EVR_UN :
        case EVR_UNKNOWN: /* unknown will be mapped to UN or OB during write */
	    len = 12;  // for Tag, Length, VR und reserved
	    break;
	default:
	    len = 8;   // for Tag, Length und VR
	    break;
	}
    } else {
	// all VR's have the same format
	len = 8;	   // for Tag und Length
    }
    return len;
}

// ********************************

static E_ByteOrder FindMachineTransferSyntax()   
{
    E_ByteOrder localByteOrderFlag;
    union
    {
	Uint32 ul;
	char uc[4];
    } tl;

    union
    {
	Uint16 us;
	char uc[2];
    } ts;

    tl.ul = 1;
    ts.us = 1;

    if (tl.uc[0] == 1 && !(tl.uc[1] | tl.uc[2] | tl.uc[3])
	&& ts.uc[0] == 1 && !(ts.uc[1]) )
	localByteOrderFlag = EBO_LittleEndian;
    else if (tl.uc[3] == 1 && !(tl.uc[0] | tl.uc[1] | tl.uc[2])
	     && ts.uc[1] == 1 && !(ts.uc[0]) )
	localByteOrderFlag = EBO_BigEndian;
    else
	localByteOrderFlag = EBO_unknown;


    return localByteOrderFlag;
}

E_ByteOrder gLocalByteOrder = FindMachineTransferSyntax();


/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: handling of transfer syntaxes
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-11-27 12:07:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcxfer.cc,v $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcxfer.h"
#include "dcuid.h"
#include "dcdebug.h"

#define INCLUDE_CSTRING
#include "ofstdinc.h"

typedef struct {
    const char          *xferID;
    const char          *xferName;
    E_TransferSyntax    xfer;
    E_ByteOrder         byteOrder;
    E_VRType            vrType;
    E_JPEGEncapsulated  encapsulated;
    Uint32              JPEGProcess8;
    Uint32              JPEGProcess12;
    E_StreamCompression streamCompression;
} S_XferNames;


#define ERROR_XferName "UnknownTransferSyntax"


const S_XferNames XferNames[] =
{
    { UID_LittleEndianImplicitTransferSyntax,                    
      "LittleEndianImplicit",
      EXS_LittleEndianImplicit,
      EBO_LittleEndian,
      EVT_Implicit,
      EJE_NotEncapsulated,
      0L, 0L,
      ESC_none },
    { "",  // illegaler Typ
      "VirtualBigEndianImplicit",
      EXS_BigEndianImplicit,
      EBO_BigEndian,
      EVT_Implicit,
      EJE_NotEncapsulated,
      0L, 0L,
      ESC_none },
    { UID_LittleEndianExplicitTransferSyntax,         
      "LittleEndianExplicit",
      EXS_LittleEndianExplicit,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L,
      ESC_none },
    { UID_BigEndianExplicitTransferSyntax,                // definiert in dctypes.h
      "BigEndianExplicit",
      EXS_BigEndianExplicit,
      EBO_BigEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L,
      ESC_none },
    { UID_JPEGProcess1TransferSyntax,
      "JPEG Baseline",
      EXS_JPEGProcess1TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      1L, 1L,
      ESC_none },
    { UID_JPEGProcess2_4TransferSyntax,
      "JPEG Extended, Process 2+4",
      EXS_JPEGProcess2_4TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      2L ,4L,
      ESC_none },
    { UID_JPEGProcess3_5TransferSyntax,
      "JPEG Extended, Process 3+5",
      EXS_JPEGProcess3_5TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      3L ,5L,
      ESC_none },
    { UID_JPEGProcess6_8TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical, Process 6+8",
      EXS_JPEGProcess6_8TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      6L ,8L,
      ESC_none },
    { UID_JPEGProcess7_9TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical, Process 7+9",
      EXS_JPEGProcess7_9TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      7L ,9L,
      ESC_none },
    { UID_JPEGProcess10_12TransferSyntax,
      "JPEG Full Progression, Non-hierarchical, Process 10+12",
      EXS_JPEGProcess10_12TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      10L ,12L,
      ESC_none },
    { UID_JPEGProcess11_13TransferSyntax,
      "JPEG Full Progression, Non-hierarchical, Process 11+13",
      EXS_JPEGProcess11_13TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      11L ,13L,
      ESC_none },
    { UID_JPEGProcess14TransferSyntax,
      "JPEG Lossless, Non-hierarchical, Process 14",
      EXS_JPEGProcess14TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      14L ,14L,
      ESC_none },
    { UID_JPEGProcess15TransferSyntax,
      "JPEG Lossless, Non-hierarchical, Process 15",
      EXS_JPEGProcess15TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      15L ,15L,
      ESC_none },
    { UID_JPEGProcess16_18TransferSyntax,
      "JPEG Extended, Hierarchical, Process 16+18",
      EXS_JPEGProcess16_18TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      16L ,18L,
      ESC_none },
    { UID_JPEGProcess17_19TransferSyntax,
      "JPEG Extended, Hierarchical, Process 17+19",
      EXS_JPEGProcess17_19TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      17L ,19L,
      ESC_none },
    { UID_JPEGProcess20_22TransferSyntax,
      "JPEG Spectral Selection, Hierarchical, Process 20+22",
      EXS_JPEGProcess20_22TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      20L ,22L,
      ESC_none },
    { UID_JPEGProcess21_23TransferSyntax,
      "JPEG Spectral Selection, Hierarchical, Process 21+23",
      EXS_JPEGProcess21_23TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      21L ,23L,
      ESC_none },
    { UID_JPEGProcess24_26TransferSyntax,
      "JPEG Full Progression, Hierarchical, Process 24+26",
      EXS_JPEGProcess24_26TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      24L ,26L,
      ESC_none },
    { UID_JPEGProcess25_27TransferSyntax,
      "JPEG Full Progression, Hierarchical, Process 25+27",
      EXS_JPEGProcess25_27TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      25L ,27L,
      ESC_none },
    { UID_JPEGProcess28TransferSyntax,
      "JPEG Lossless, Hierarchical, Process 28",
      EXS_JPEGProcess28TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      28L ,28L,
      ESC_none },
    { UID_JPEGProcess29TransferSyntax,
      "JPEG Lossless, Hierarchical, Process 29",
      EXS_JPEGProcess29TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      29L ,29L,
      ESC_none },
    { UID_JPEGProcess14SV1TransferSyntax,
      "JPEG Lossless, Non-hierarchical, 1st Order Prediction",
      EXS_JPEGProcess14SV1TransferSyntax,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      14L ,14L,
      ESC_none },
    { UID_RLELossless,
      "RLE Lossless",
      EXS_RLELossless,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      ESC_none },
    { UID_JPEGLSLossless,
      "JPEG-LS Lossless",
      EXS_JPEGLSLossless,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      ESC_none },
    { UID_JPEGLSLossy,
      "JPEG-LS Lossy (Near-lossless)",
      EXS_JPEGLSLossy,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      ESC_none },
    { UID_DeflatedExplicitVRLittleEndianTransferSyntax,
      "Deflated Explicit VR Little Endian",
      EXS_DeflatedLittleEndianExplicit,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L,
#ifdef WITH_ZLIB
      ESC_zlib },
#else
      ESC_unsupported },
#endif
    { UID_JPEG2000LosslessOnlyTransferSyntax,
      "JPEG 2000 (Lossless only)",
      EXS_JPEG2000LosslessOnly,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      ESC_none },
    { UID_JPEG2000TransferSyntax,
      "JPEG 2000 (Lossless or Lossy)",
      EXS_JPEG2000,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      ESC_none }

    // Enter further transfer syntaxes here:
};

const int DIM_OF_XferNames = (sizeof(XferNames) / sizeof(S_XferNames));


// ********************************


DcmXfer::DcmXfer( E_TransferSyntax xfer )
  : xferID(""),
    xferName(ERROR_XferName),
    xferSyn(EXS_Unknown),
    byteOrder(EBO_unknown),
    vrType(EVT_Implicit),
    encapsulated(EJE_NotEncapsulated),
    JPEGProcess8(0L),
    JPEGProcess12(0L),
    streamCompression(ESC_none)
{
    int i = 0;
    while (    (i < DIM_OF_XferNames)
               && XferNames[i].xfer != xfer
        )
        i++;
    if ( (i < DIM_OF_XferNames)
         && XferNames[i].xfer == xfer)
    {
        xferSyn           = XferNames[i].xfer;
        xferID            = XferNames[i].xferID;
        xferName          = XferNames[i].xferName;
        byteOrder         = XferNames[i].byteOrder;
        vrType            = XferNames[i].vrType;
        encapsulated      = XferNames[i].encapsulated;
        JPEGProcess8      = XferNames[i].JPEGProcess8;
        JPEGProcess12     = XferNames[i].JPEGProcess12;
        streamCompression = XferNames[i].streamCompression;
    }
}


// ********************************


DcmXfer::DcmXfer( const char* xferName_xferID )
  : xferID(""),
    xferName(ERROR_XferName),
    xferSyn(EXS_Unknown),
    byteOrder(EBO_unknown),
    vrType(EVT_Implicit),
    encapsulated(EJE_NotEncapsulated),
    JPEGProcess8(0L),
    JPEGProcess12(0L),
    streamCompression(ESC_none)
{
    const char* xname = xferName_xferID;
    if ( xname != (char*)NULL )
    {
        int i = 0;
        while ( (i < DIM_OF_XferNames)
                && (strcmp(XferNames[i].xferID, xname) != 0) )
            i++;

        if ( (i < DIM_OF_XferNames)
             && (strcmp(XferNames[i].xferID, xname) == 0) )
        {
            xferSyn           = XferNames[i].xfer;
            xferID            = XferNames[i].xferID;
            xferName          = XferNames[i].xferName;
            byteOrder         = XferNames[i].byteOrder;
            vrType            = XferNames[i].vrType;
            encapsulated      = XferNames[i].encapsulated;
            JPEGProcess8      = XferNames[i].JPEGProcess8;
            JPEGProcess12     = XferNames[i].JPEGProcess12;
            streamCompression = XferNames[i].streamCompression;
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
                xferSyn           = XferNames[i].xfer;
                xferID            = XferNames[i].xferID;
                xferName          = XferNames[i].xferName;
                byteOrder         = XferNames[i].byteOrder;
                vrType            = XferNames[i].vrType;
                encapsulated      = XferNames[i].encapsulated;
                JPEGProcess8      = XferNames[i].JPEGProcess8;
                JPEGProcess12     = XferNames[i].JPEGProcess12;
                streamCompression = XferNames[i].streamCompression;
            }
        }
    }
}


// ********************************


DcmXfer::DcmXfer( const DcmXfer &newXfer )
  : xferID(newXfer.xferID),
    xferName(newXfer.xferName),
    xferSyn(newXfer.xferSyn),
    byteOrder(newXfer.byteOrder),
    vrType(newXfer.vrType),
    encapsulated(newXfer.encapsulated),
    JPEGProcess8(newXfer.JPEGProcess8),
    JPEGProcess12(newXfer.JPEGProcess12),
    streamCompression(newXfer.streamCompression)
{
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
        xferSyn           = XferNames[i].xfer;
        xferID            = XferNames[i].xferID;
        xferName          = XferNames[i].xferName;
        byteOrder         = XferNames[i].byteOrder;
        vrType            = XferNames[i].vrType;
        encapsulated      = XferNames[i].encapsulated;
        JPEGProcess8      = XferNames[i].JPEGProcess8;
        JPEGProcess12     = XferNames[i].JPEGProcess12;
        streamCompression = XferNames[i].streamCompression;
    }
    else
    {
        xferSyn           = EXS_Unknown;
        xferID            = "";
        xferName          = ERROR_XferName;
        byteOrder         = EBO_unknown;
        vrType            = EVT_Implicit;
        encapsulated      = EJE_NotEncapsulated;
        JPEGProcess8      = 0L;
        JPEGProcess12     = 0L;
        streamCompression = ESC_none;
    }
    return *this;
}


// ********************************


DcmXfer & DcmXfer::operator = ( const DcmXfer &newXfer )
{
    if ( this != &newXfer )
    {
        xferSyn           = newXfer.xferSyn;
        xferID            = newXfer.xferID;
        xferName          = newXfer.xferName;
        byteOrder         = newXfer.byteOrder;
        vrType            = newXfer.vrType;
        encapsulated      = newXfer.encapsulated;
        JPEGProcess8      = newXfer.JPEGProcess8;
        JPEGProcess12     = newXfer.JPEGProcess12;
        streamCompression = newXfer.streamCompression;
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
        DcmVR vr(evr);
        if (vr.usesExtendedLengthEncoding()) {
            len = 12;  // for Tag, Length, VR und reserved
        } else {
            len = 8;   // for Tag, Length und VR
        }
    } else {
        // all VR's have the same format
        len = 8;           // for Tag und Length
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

const E_ByteOrder gLocalByteOrder = FindMachineTransferSyntax();


/*
 * CVS/RCS Log:
 * $Log: dcxfer.cc,v $
 * Revision 1.20  2002-11-27 12:07:01  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.19  2002/08/27 16:56:01  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.18  2002/06/19 15:35:01  meichel
 * Fixed typo in transfer syntax name
 *
 * Revision 1.17  2001/11/08 16:17:34  meichel
 * Updated data dictionary, UIDs and transfer syntaxes for DICOM 2001 edition.
 *
 * Revision 1.16  2001/06/01 15:49:23  meichel
 * Updated copyright header
 *
 * Revision 1.15  2001/01/17 10:20:38  meichel
 * Added toolkit support for JPEG-LS transfer syntaxes
 *
 * Revision 1.14  2000/04/14 16:10:35  meichel
 * Minor changes for thread safety.
 *
 * Revision 1.13  2000/03/08 16:26:53  meichel
 * Updated copyright header.
 *
 * Revision 1.12  1999/03/31 09:26:05  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */

/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: handling of transfer syntaxes
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcuid.h"

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


typedef struct
{
    const char         *xferID;
    const char         *xferName;
    E_TransferSyntax    xfer;
    E_ByteOrder         byteOrder;
    E_ByteOrder         pixelDataByteOrder;
    E_VRType            vrType;
    E_JPEGEncapsulated  encapsulated;
    Uint32              JPEGProcess8;
    Uint32              JPEGProcess12;
    OFBool              lossy;
    OFBool              retired;
    E_StreamCompression streamCompression;
} S_XferNames;


#define ERROR_XferName "Unknown Transfer Syntax"


const S_XferNames XferNames[] =
{
    // entry #0
    { UID_LittleEndianImplicitTransferSyntax,
      "Little Endian Implicit",
      EXS_LittleEndianImplicit,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Implicit,
      EJE_NotEncapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #1
    { "",  // illegal type
      "Virtual Big Endian Implicit",
      EXS_BigEndianImplicit,
      EBO_BigEndian,
      EBO_BigEndian,
      EVT_Implicit,
      EJE_NotEncapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #2
    { UID_LittleEndianExplicitTransferSyntax,
      "Little Endian Explicit",
      EXS_LittleEndianExplicit,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #3
    { UID_BigEndianExplicitTransferSyntax,
      "Big Endian Explicit",
      EXS_BigEndianExplicit,
      EBO_BigEndian,
      EBO_BigEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L,
      OFFalse,
      OFTrue, // retired with Supplement 98
      ESC_none },
    // entry #4
    { UID_JPEGProcess1TransferSyntax,
      "JPEG Baseline",
      EXS_JPEGProcess1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      1L, 1L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #5
    { UID_JPEGProcess2_4TransferSyntax,
      "JPEG Extended, Process 2+4",
      EXS_JPEGProcess2_4,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      2L ,4L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #6
    { UID_JPEGProcess3_5TransferSyntax,
      "JPEG Extended, Process 3+5",
      EXS_JPEGProcess3_5,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      3L ,5L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #7
    { UID_JPEGProcess6_8TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical, Process 6+8",
      EXS_JPEGProcess6_8,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      6L ,8L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #8
    { UID_JPEGProcess7_9TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical, Process 7+9",
      EXS_JPEGProcess7_9,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      7L ,9L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #9
    { UID_JPEGProcess10_12TransferSyntax,
      "JPEG Full Progression, Non-hierarchical, Process 10+12",
      EXS_JPEGProcess10_12,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      10L ,12L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #10
    { UID_JPEGProcess11_13TransferSyntax,
      "JPEG Full Progression, Non-hierarchical, Process 11+13",
      EXS_JPEGProcess11_13,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      11L ,13L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #11
    { UID_JPEGProcess14TransferSyntax,
      "JPEG Lossless, Non-hierarchical, Process 14",
      EXS_JPEGProcess14,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      14L ,14L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #12
    { UID_JPEGProcess15TransferSyntax,
      "JPEG Lossless, Non-hierarchical, Process 15",
      EXS_JPEGProcess15,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      15L ,15L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #13
    { UID_JPEGProcess16_18TransferSyntax,
      "JPEG Extended, Hierarchical, Process 16+18",
      EXS_JPEGProcess16_18,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      16L ,18L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #14
    { UID_JPEGProcess17_19TransferSyntax,
      "JPEG Extended, Hierarchical, Process 17+19",
      EXS_JPEGProcess17_19,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      17L ,19L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #15
    { UID_JPEGProcess20_22TransferSyntax,
      "JPEG Spectral Selection, Hierarchical, Process 20+22",
      EXS_JPEGProcess20_22,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      20L ,22L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #16
    { UID_JPEGProcess21_23TransferSyntax,
      "JPEG Spectral Selection, Hierarchical, Process 21+23",
      EXS_JPEGProcess21_23,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      21L ,23L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #17
    { UID_JPEGProcess24_26TransferSyntax,
      "JPEG Full Progression, Hierarchical, Process 24+26",
      EXS_JPEGProcess24_26,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      24L ,26L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #18
    { UID_JPEGProcess25_27TransferSyntax,
      "JPEG Full Progression, Hierarchical, Process 25+27",
      EXS_JPEGProcess25_27,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      25L ,27L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #19
    { UID_JPEGProcess28TransferSyntax,
      "JPEG Lossless, Hierarchical, Process 28",
      EXS_JPEGProcess28,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      28L ,28L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #20
    { UID_JPEGProcess29TransferSyntax,
      "JPEG Lossless, Hierarchical, Process 29",
      EXS_JPEGProcess29,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      29L ,29L,
      OFTrue,
      OFTrue,
      ESC_none },
    // entry #21
    { UID_JPEGProcess14SV1TransferSyntax,
      "JPEG Lossless, Non-hierarchical, 1st Order Prediction",
      EXS_JPEGProcess14SV1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      14L ,14L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #22
    { UID_RLELosslessTransferSyntax,
      "RLE Lossless",
      EXS_RLELossless,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #23
    { UID_DeflatedExplicitVRLittleEndianTransferSyntax,
      "Deflated Explicit VR Little Endian",
      EXS_DeflatedLittleEndianExplicit,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
#ifdef WITH_ZLIB
      ESC_zlib
#else
      ESC_unsupported
#endif
    },
    // entry #24
    { UID_JPEGLSLosslessTransferSyntax,
      "JPEG-LS Lossless",
      EXS_JPEGLSLossless,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #25
    { UID_JPEGLSLossyTransferSyntax,
      "JPEG-LS Lossy (Near-lossless)",
      EXS_JPEGLSLossy,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #26
    { UID_JPEG2000LosslessOnlyTransferSyntax,
      "JPEG 2000 (Lossless only)",
      EXS_JPEG2000LosslessOnly,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #27
    { UID_JPEG2000TransferSyntax,
      "JPEG 2000 (Lossless or Lossy)",
      EXS_JPEG2000,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #28
   { UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax,
      "JPEG 2000 Part 2 Multicomponent Image Compression (Lossless only)",
      EXS_JPEG2000MulticomponentLosslessOnly,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none },
    // entry #29
   { UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,
      "JPEG 2000 Part 2 Multicomponent Image Compression (Lossless or Lossy)",
      EXS_JPEG2000Multicomponent,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #30
   { UID_JPIPReferencedTransferSyntax,
      "JPIP Referenced",
      EXS_JPIPReferenced,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,  // in fact, pixel data shall be referenced via (0028,7FE0) Pixel Data Provider URL
      0L, 0L,
      OFTrue,               // really lossy?
      OFFalse,
      ESC_none },
    // entry #31
   { UID_JPIPReferencedDeflateTransferSyntax,
      "JPIP Referenced Deflate",
      EXS_JPIPReferencedDeflate,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_NotEncapsulated,  // in fact, pixel data shall be referenced via (0028,7FE0) Pixel Data Provider URL
      0L, 0L,
      OFTrue,               // really lossy?
      OFFalse,
#ifdef WITH_ZLIB
      ESC_zlib
#else
      ESC_unsupported
#endif
    },
    // entry #32
    { UID_MPEG2MainProfileAtMainLevelTransferSyntax,
      "MPEG2 Main Profile @ Main Level",  // changed with DICOM 2016e to: MPEG2 Main Profile / Main Level
      EXS_MPEG2MainProfileAtMainLevel,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #33
    { UID_MPEG2MainProfileAtHighLevelTransferSyntax,
      "MPEG2 Main Profile @ High Level",  // changed with DICOM 2016e to: MPEG2 Main Profile / High Level
      EXS_MPEG2MainProfileAtHighLevel,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #34
    { UID_MPEG4HighProfileLevel4_1TransferSyntax,
      "MPEG-4 AVC/H.264 High Profile / Level 4.1",
      EXS_MPEG4HighProfileLevel4_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #35
    { UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,
      "MPEG-4 AVC/H.264 BD-compatible High Profile / Level 4.1",
      EXS_MPEG4BDcompatibleHighProfileLevel4_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #36
    { UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,
      "MPEG-4 AVC/H.264 High Profile / Level 4.2 For 2D Video",
      EXS_MPEG4HighProfileLevel4_2_For2DVideo,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #37
    { UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,
      "MPEG-4 AVC/H.264 High Profile / Level 4.2 For 3D Video",
      EXS_MPEG4HighProfileLevel4_2_For3DVideo,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #38
    { UID_MPEG4StereoHighProfileLevel4_2TransferSyntax,
      "MPEG-4 AVC/H.264 Stereo High Profile / Level 4.2",
      EXS_MPEG4StereoHighProfileLevel4_2,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #39
    { UID_HEVCMainProfileLevel5_1TransferSyntax,
      "HEVC/H.265 Main Profile / Level 5.1",
      EXS_HEVCMainProfileLevel5_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #40
    { UID_HEVCMain10ProfileLevel5_1TransferSyntax,
      "HEVC/H.265 Main 10 Profile / Level 5.1",
      EXS_HEVCMain10ProfileLevel5_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EJE_Encapsulated,
      0L, 0L,
      OFTrue,
      OFFalse,
      ESC_none },
    // entry #41
    { UID_PrivateGE_LEI_WithBigEndianPixelDataTransferSyntax,
      "Private GE Little Endian Implicit with big endian pixel data",
      EXS_PrivateGE_LEI_WithBigEndianPixelData,
      EBO_LittleEndian,
      EBO_BigEndian,
      EVT_Implicit,
      EJE_NotEncapsulated,
      0L, 0L,
      OFFalse,
      OFFalse,
      ESC_none }
};

const int DIM_OF_XferNames = OFstatic_cast(int, sizeof(XferNames) / sizeof(S_XferNames));


// ********************************


DcmXfer::DcmXfer(E_TransferSyntax xfer)
  : xferID(""),
    xferName(ERROR_XferName),
    xferSyn(EXS_Unknown),
    byteOrder(EBO_unknown),
    pixelDataByteOrder(EBO_unknown),
    vrType(EVT_Implicit),
    encapsulated(EJE_NotEncapsulated),
    JPEGProcess8(0L),
    JPEGProcess12(0L),
    lossy(OFFalse),
    retired(OFFalse),
    streamCompression(ESC_none)
{
    int i = 0;
    while ((i < DIM_OF_XferNames) && XferNames[i].xfer != xfer)
        i++;
    if ((i < DIM_OF_XferNames) && (XferNames[i].xfer == xfer))
    {
        xferSyn            = XferNames[i].xfer;
        xferID             = XferNames[i].xferID;
        xferName           = XferNames[i].xferName;
        byteOrder          = XferNames[i].byteOrder;
        pixelDataByteOrder = XferNames[i].pixelDataByteOrder;
        vrType             = XferNames[i].vrType;
        encapsulated       = XferNames[i].encapsulated;
        JPEGProcess8       = XferNames[i].JPEGProcess8;
        JPEGProcess12      = XferNames[i].JPEGProcess12;
        lossy              = XferNames[i].lossy;
        retired            = XferNames[i].retired;
        streamCompression  = XferNames[i].streamCompression;
    }
}


// ********************************


DcmXfer::DcmXfer(const char* xferName_xferID)
  : xferID(""),
    xferName(ERROR_XferName),
    xferSyn(EXS_Unknown),
    byteOrder(EBO_unknown),
    pixelDataByteOrder(EBO_unknown),
    vrType(EVT_Implicit),
    encapsulated(EJE_NotEncapsulated),
    JPEGProcess8(0L),
    JPEGProcess12(0L),
    lossy(OFFalse),
    retired(OFFalse),
    streamCompression(ESC_none)
{
    const char* xname = xferName_xferID;
    if (xname != NULL)
    {
        int i = 0;
        while ((i < DIM_OF_XferNames) && (strcmp(XferNames[i].xferID, xname) != 0))
            i++;
        if ((i < DIM_OF_XferNames) && (strcmp(XferNames[i].xferID, xname) == 0))
        {
            xferSyn            = XferNames[i].xfer;
            xferID             = XferNames[i].xferID;
            xferName           = XferNames[i].xferName;
            byteOrder          = XferNames[i].byteOrder;
            pixelDataByteOrder = XferNames[i].pixelDataByteOrder;
            vrType             = XferNames[i].vrType;
            encapsulated       = XferNames[i].encapsulated;
            JPEGProcess8       = XferNames[i].JPEGProcess8;
            JPEGProcess12      = XferNames[i].JPEGProcess12;
            lossy              = XferNames[i].lossy;
            retired            = XferNames[i].retired;
            streamCompression  = XferNames[i].streamCompression;
        }
        else
        {
            i = 0;
            while ((i < DIM_OF_XferNames) && (strcmp(XferNames[i].xferName, xname) != 0))
                i++;
            if ((i < DIM_OF_XferNames) && (strcmp(XferNames[i].xferName, xname) == 0))
            {
                xferSyn            = XferNames[i].xfer;
                xferID             = XferNames[i].xferID;
                xferName           = XferNames[i].xferName;
                byteOrder          = XferNames[i].byteOrder;
                pixelDataByteOrder = XferNames[i].pixelDataByteOrder;
                vrType             = XferNames[i].vrType;
                encapsulated       = XferNames[i].encapsulated;
                JPEGProcess8       = XferNames[i].JPEGProcess8;
                JPEGProcess12      = XferNames[i].JPEGProcess12;
                lossy              = XferNames[i].lossy;
                retired            = XferNames[i].retired;
                streamCompression  = XferNames[i].streamCompression;
            }
        }
    }
}


// ********************************


DcmXfer::DcmXfer(const DcmXfer &newXfer)
  : xferID(newXfer.xferID),
    xferName(newXfer.xferName),
    xferSyn(newXfer.xferSyn),
    byteOrder(newXfer.byteOrder),
    pixelDataByteOrder(newXfer.pixelDataByteOrder),
    vrType(newXfer.vrType),
    encapsulated(newXfer.encapsulated),
    JPEGProcess8(newXfer.JPEGProcess8),
    JPEGProcess12(newXfer.JPEGProcess12),
    lossy(newXfer.lossy),
    retired(newXfer.retired),
    streamCompression(newXfer.streamCompression)
{
}


// ********************************


DcmXfer::~DcmXfer()
{
}


// ********************************


DcmXfer &DcmXfer::operator=(const E_TransferSyntax xfer)
{
    // tbc: if the enum value is consistent with the index of the array,
    //      why do we need to iterate over the array entries?
    int i = 0;
    while ((i < DIM_OF_XferNames) && (XferNames[i].xfer != xfer))
        i++;
    if ((i < DIM_OF_XferNames) && (XferNames[i].xfer == xfer))
    {
        xferSyn            = XferNames[i].xfer;
        xferID             = XferNames[i].xferID;
        xferName           = XferNames[i].xferName;
        byteOrder          = XferNames[i].byteOrder;
        pixelDataByteOrder = XferNames[i].pixelDataByteOrder;
        vrType             = XferNames[i].vrType;
        encapsulated       = XferNames[i].encapsulated;
        JPEGProcess8       = XferNames[i].JPEGProcess8;
        JPEGProcess12      = XferNames[i].JPEGProcess12;
        lossy              = XferNames[i].lossy;
        retired            = XferNames[i].retired;
        streamCompression  = XferNames[i].streamCompression;
    } else {
        xferSyn            = EXS_Unknown;
        xferID             = "";
        xferName           = ERROR_XferName;
        byteOrder          = EBO_unknown;
        pixelDataByteOrder = EBO_unknown;
        vrType             = EVT_Implicit;
        encapsulated       = EJE_NotEncapsulated;
        JPEGProcess8       = 0L;
        JPEGProcess12      = 0L;
        lossy              = OFFalse;
        retired            = OFFalse;
        streamCompression  = ESC_none;
    }
    return *this;
}


// ********************************


DcmXfer &DcmXfer::operator=(const DcmXfer &newXfer)
{
    if (this != &newXfer)
    {
        xferSyn            = newXfer.xferSyn;
        xferID             = newXfer.xferID;
        xferName           = newXfer.xferName;
        byteOrder          = newXfer.byteOrder;
        pixelDataByteOrder = newXfer.pixelDataByteOrder;
        vrType             = newXfer.vrType;
        encapsulated       = newXfer.encapsulated;
        JPEGProcess8       = newXfer.JPEGProcess8;
        JPEGProcess12      = newXfer.JPEGProcess12;
        lossy              = newXfer.lossy;
        retired            = newXfer.retired;
        streamCompression  = newXfer.streamCompression;
    }
    return *this;
}


// ********************************

Uint32 DcmXfer::sizeofTagHeader(DcmEVR evr) const
{
    Uint32 len = 0;
    if (isExplicitVR())
    {
        // some VRs have an extended format
        DcmVR vr(evr);
        if (vr.usesExtendedLengthEncoding()) {
            len = 12;  // for Tag, Length, VR and reserved
        } else {
            len = 8;   // for Tag, Length and VR
        }
    } else {
        // all implicit VRs have the same format
        len = 8;       // for Tag and Length
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

    if (tl.uc[0] == 1 && !(tl.uc[1] | tl.uc[2] | tl.uc[3]) && ts.uc[0] == 1 && !(ts.uc[1]))
        localByteOrderFlag = EBO_LittleEndian;
    else if (tl.uc[3] == 1 && !(tl.uc[0] | tl.uc[1] | tl.uc[2]) && ts.uc[1] == 1 && !(ts.uc[0]))
        localByteOrderFlag = EBO_BigEndian;
    else
        localByteOrderFlag = EBO_unknown;

    return localByteOrderFlag;
}

// global constant: local byte order (little or big endian)
const E_ByteOrder gLocalByteOrder = FindMachineTransferSyntax();

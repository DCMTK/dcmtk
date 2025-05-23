/*
 *
 *  Copyright (C) 1994-2025, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Joerg Riesmeier
 *
 *  Purpose: handling of transfer syntaxes
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcuid.h"
#include <cstring>

#define DCXFER_DEFAULT_MIME_TYPE "application/dicom"
#define DCXFER_DEFAULT_EXTENSION ".bin"

typedef struct
{
    const char            *xferID;
    const char            *xferName;
    E_TransferSyntax       xfer;
    E_ByteOrder            byteOrder;
    E_ByteOrder            pixelDataByteOrder;
    E_VRType               vrType;
    E_PixelDataEncoding    pixelDataEncoding;
    E_PixelDataCompression pixelDataCompression;
    OFBool                 pixelDataFragmentable;
    Uint32                 JPEGProcess8;
    Uint32                 JPEGProcess12;
    E_StreamCompression    streamCompression;
    E_XferValidity         xferValidity;
    const char            *mimeType;
    const char            *filenameExtension;
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
      EPE_Native,
      EPC_Uncompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #1
    { "",  // internal type, no UID defined
      "Virtual Big Endian Implicit",
      EXS_BigEndianImplicit,
      EBO_BigEndian,
      EBO_BigEndian,
      EVT_Implicit,
      EPE_Native,
      EPC_Uncompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Internal,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #2
    { UID_LittleEndianExplicitTransferSyntax,
      "Little Endian Explicit",
      EXS_LittleEndianExplicit,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Native,
      EPC_Uncompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #3
    { UID_BigEndianExplicitTransferSyntax,
      "Big Endian Explicit",
      EXS_BigEndianExplicit,
      EBO_BigEndian,
      EBO_BigEndian,
      EVT_Explicit,
      EPE_Native,
      EPC_Uncompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 98
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #4
    { UID_EncapsulatedUncompressedExplicitVRLittleEndianTransferSyntax,
      "Encapsulated Uncompressed Explicit VR Little Endian",
      EXS_EncapsulatedUncompressedLittleEndianExplicit,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_Uncompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #5
    { UID_JPEGProcess1TransferSyntax,
      "JPEG Baseline",
      EXS_JPEGProcess1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      1L, 1L,
      ESC_none,
      EXV_Standard,
      "image/jpeg",
      ".jpeg"
    },
    // entry #6
    { UID_JPEGProcess2_4TransferSyntax,
      "JPEG Extended, Process 2+4",
      EXS_JPEGProcess2_4,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      2L, 4L,
      ESC_none,
      EXV_Standard,
      "image/jpeg",
      ".jpeg"
    },
    // entry #7
    { UID_JPEGProcess3_5TransferSyntax,
      "JPEG Extended, Process 3+5",
      EXS_JPEGProcess3_5,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      3L, 5L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #8
    { UID_JPEGProcess6_8TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical, Process 6+8",
      EXS_JPEGProcess6_8,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      6L, 8L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #9
    { UID_JPEGProcess7_9TransferSyntax,
      "JPEG Spectral Selection, Non-hierarchical, Process 7+9",
      EXS_JPEGProcess7_9,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      7L, 9L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #10
    { UID_JPEGProcess10_12TransferSyntax,
      "JPEG Full Progression, Non-hierarchical, Process 10+12",
      EXS_JPEGProcess10_12,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      10L, 12L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #11
    { UID_JPEGProcess11_13TransferSyntax,
      "JPEG Full Progression, Non-hierarchical, Process 11+13",
      EXS_JPEGProcess11_13,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      11L, 13L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #12
    { UID_JPEGProcess14TransferSyntax,
      "JPEG Lossless, Non-hierarchical, Process 14",
      EXS_JPEGProcess14,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      14, 14L,
      ESC_none,
      EXV_Standard,
      "image/jpeg",
      ".jpeg"
    },
    // entry #13
    { UID_JPEGProcess15TransferSyntax,
      "JPEG Lossless, Non-hierarchical, Process 15",
      EXS_JPEGProcess15,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      15L, 15L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #14
    { UID_JPEGProcess16_18TransferSyntax,
      "JPEG Extended, Hierarchical, Process 16+18",
      EXS_JPEGProcess16_18,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      16L, 18L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #15
    { UID_JPEGProcess17_19TransferSyntax,
      "JPEG Extended, Hierarchical, Process 17+19",
      EXS_JPEGProcess17_19,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      17L, 19L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #16
    { UID_JPEGProcess20_22TransferSyntax,
      "JPEG Spectral Selection, Hierarchical, Process 20+22",
      EXS_JPEGProcess20_22,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      20L, 22L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #17
    { UID_JPEGProcess21_23TransferSyntax,
      "JPEG Spectral Selection, Hierarchical, Process 21+23",
      EXS_JPEGProcess21_23,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      21L, 23L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #18
    { UID_JPEGProcess24_26TransferSyntax,
      "JPEG Full Progression, Hierarchical, Process 24+26",
      EXS_JPEGProcess24_26,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      24L, 26L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #19
    { UID_JPEGProcess25_27TransferSyntax,
      "JPEG Full Progression, Hierarchical, Process 25+27",
      EXS_JPEGProcess25_27,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      25L, 27L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #20
    { UID_JPEGProcess28TransferSyntax,
      "JPEG Lossless, Hierarchical, Process 28",
      EXS_JPEGProcess28,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      28L, 28L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #21
    { UID_JPEGProcess29TransferSyntax,
      "JPEG Lossless, Hierarchical, Process 29",
      EXS_JPEGProcess29,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      29L, 29L,
      ESC_none,
      EXV_Retired,  // retired with Supplement 61
      "image/jpeg",
      ".jpeg"
    },
    // entry #22
    { UID_JPEGProcess14SV1TransferSyntax,
      "JPEG Lossless, Non-hierarchical, 1st Order Prediction",
      EXS_JPEGProcess14SV1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      14L, 14L,
      ESC_none,
      EXV_Standard,
      "image/jpeg",
      ".jpeg"
    },
    // entry #23
    { UID_RLELosslessTransferSyntax,
      "RLE Lossless",
      EXS_RLELossless,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/dicom-rle",
      ".dicom-rle"
    },
    // entry #24
    { UID_DeflatedExplicitVRLittleEndianTransferSyntax,
      "Deflated Explicit VR Little Endian",
      EXS_DeflatedLittleEndianExplicit,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Native,
      EPC_Uncompressed,
      OFFalse,
      0L, 0L,
#ifdef WITH_ZLIB
      ESC_zlib,
#else
      ESC_unsupported,
#endif
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #25
    { UID_DeflatedImageFrameCompressionTransferSyntax,
      "Deflated Image Frame Compression",
      EXS_DeflatedImageFrameCompression,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "application/x-deflate",
      ".x-deflate"
    },
    // entry #26
    { UID_JPEGLSLosslessTransferSyntax,
      "JPEG-LS Lossless",
      EXS_JPEGLSLossless,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jls",
      ".jls"
    },
    // entry #27
    { UID_JPEGLSLossyTransferSyntax,
      "JPEG-LS Lossy (Near-lossless)",
      EXS_JPEGLSLossy,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jls",
      ".jls"
    },
    // entry #28
    { UID_JPEG2000LosslessOnlyTransferSyntax,
      "JPEG 2000 (Lossless only)",
      EXS_JPEG2000LosslessOnly,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jp2",
      ".jp2"
    },
    // entry #29
    { UID_JPEG2000TransferSyntax,
      "JPEG 2000 (Lossless or Lossy)",
      EXS_JPEG2000,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jp2",
      ".jp2"
    },
    // entry #30
   { UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax,
      "JPEG 2000 Part 2 Multicomponent Image Compression (Lossless only)",
      EXS_JPEG2000MulticomponentLosslessOnly,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jpx",
      ".jpx"
    },
    // entry #31
   { UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,
      "JPEG 2000 Part 2 Multicomponent Image Compression (Lossless or Lossy)",
      EXS_JPEG2000Multicomponent,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jpx",
      ".jpx"
    },
    // entry #32
   { UID_JPIPReferencedTransferSyntax,
      "JPIP Referenced",
      EXS_JPIPReferenced,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Referenced,
      EPC_unknown,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #33
   { UID_JPIPReferencedDeflateTransferSyntax,
      "JPIP Referenced Deflate",
      EXS_JPIPReferencedDeflate,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Referenced,
      EPC_unknown,
      OFFalse,
      0L, 0L,
#ifdef WITH_ZLIB
      ESC_zlib,
#else
      ESC_unsupported,
#endif
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #34
    { UID_MPEG2MainProfileAtMainLevelTransferSyntax,
      "MPEG2 Main Profile @ Main Level",  // changed with DICOM 2016e to: MPEG2 Main Profile / Main Level
      EXS_MPEG2MainProfileAtMainLevel,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mpeg",
      ".mpeg"
    },
    // entry #35
    { UID_FragmentableMPEG2MainProfileMainLevelTransferSyntax,
      "Fragmentable MPEG2 Main Profile / Main Level",
      EXS_FragmentableMPEG2MainProfileMainLevel,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mpeg",
      ".mpeg"
    },
    // entry #36
    { UID_MPEG2MainProfileAtHighLevelTransferSyntax,
      "MPEG2 Main Profile @ High Level",  // changed with DICOM 2016e to: MPEG2 Main Profile / High Level
      EXS_MPEG2MainProfileAtHighLevel,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mpeg",
      ".mpeg"
    },
    // entry #37
    { UID_FragmentableMPEG2MainProfileHighLevelTransferSyntax,
      "Fragmentable MPEG2 Main Profile / High Level",
      EXS_FragmentableMPEG2MainProfileHighLevel,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mpeg",
      ".mpeg"
    },
    // entry #38
    { UID_MPEG4HighProfileLevel4_1TransferSyntax,
      "MPEG-4 AVC/H.264 High Profile / Level 4.1",
      EXS_MPEG4HighProfileLevel4_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #39
    { UID_FragmentableMPEG4HighProfileLevel4_1TransferSyntax,
      "Fragmentable MPEG-4 AVC/H.264 High Profile / Level 4.1",
      EXS_FragmentableMPEG4HighProfileLevel4_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #40
    { UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,
      "MPEG-4 AVC/H.264 BD-compatible High Profile / Level 4.1",
      EXS_MPEG4BDcompatibleHighProfileLevel4_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #41
    { UID_FragmentableMPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,
      "Fragmentable MPEG-4 AVC/H.264 BD-compatible High Profile / Level 4.1",
      EXS_FragmentableMPEG4BDcompatibleHighProfileLevel4_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #42
    { UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,
      "MPEG-4 AVC/H.264 High Profile / Level 4.2 For 2D Video",
      EXS_MPEG4HighProfileLevel4_2_For2DVideo,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #43
    { UID_FragmentableMPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,
      "Fragmentable MPEG-4 AVC/H.264 High Profile / Level 4.2 For 2D Video",
      EXS_FragmentableMPEG4HighProfileLevel4_2_For2DVideo,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #44
    { UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,
      "MPEG-4 AVC/H.264 High Profile / Level 4.2 For 3D Video",
      EXS_MPEG4HighProfileLevel4_2_For3DVideo,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #45
    { UID_FragmentableMPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,
      "Fragmentable MPEG-4 AVC/H.264 High Profile / Level 4.2 For 3D Video",
      EXS_FragmentableMPEG4HighProfileLevel4_2_For3DVideo,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #46
    { UID_MPEG4StereoHighProfileLevel4_2TransferSyntax,
      "MPEG-4 AVC/H.264 Stereo High Profile / Level 4.2",
      EXS_MPEG4StereoHighProfileLevel4_2,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #47
    { UID_FragmentableMPEG4StereoHighProfileLevel4_2TransferSyntax,
      "Fragmentable MPEG-4 AVC/H.264 Stereo High Profile / Level 4.2",
      EXS_FragmentableMPEG4StereoHighProfileLevel4_2,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/mp4",
      ".mp4"
    },
    // entry #48
    { UID_HEVCMainProfileLevel5_1TransferSyntax,
      "HEVC/H.265 Main Profile / Level 5.1",
      EXS_HEVCMainProfileLevel5_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/H265",
      ".H265"
    },
    // entry #49
    { UID_HEVCMain10ProfileLevel5_1TransferSyntax,
      "HEVC/H.265 Main 10 Profile / Level 5.1",
      EXS_HEVCMain10ProfileLevel5_1,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFTrue,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "video/H265",
      ".H265"
    },
    // entry #50
    { UID_JPEGXLLosslessTransferSyntax,
      "JPEG XL Lossless",
      EXS_JPEGXLLossless,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jxl",
      ".jxl"
    },
    // entry #51
    { UID_JPEGXLJPEGRecompressionTransferSyntax,
      "JPEG XL JPEG Recompression",
      EXS_JPEGXLJPEGRecompression,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jxl",
      ".jxl"
    },
    // entry #52
    { UID_JPEGXLTransferSyntax,
      "JPEG XL",
      EXS_JPEGXL,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jxl",
      ".jxl"
    },
    // entry #53
    { UID_HighThroughputJPEG2000ImageCompressionLosslessOnlyTransferSyntax,
      "High-Throughput JPEG 2000 Image Compression (Lossless Only)",
      EXS_HighThroughputJPEG2000LosslessOnly,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jphc",
      ".jphc"
    },
    // entry #54
    { UID_HighThroughputJPEG2000RPCLImageCompressionLosslessOnlyTransferSyntax,
      "High-Throughput JPEG 2000 with RPCL Options Image Compression (Lossless Only)",
      EXS_HighThroughputJPEG2000withRPCLOptionsLosslessOnly,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LosslessCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jphc",
      ".jphc"
    },
    // entry #55
    { UID_HighThroughputJPEG2000ImageCompressionTransferSyntax,
      "High-Throughput JPEG 2000 Image Compression",
      EXS_HighThroughputJPEG2000,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Encapsulated,
      EPC_LossyCompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      "image/jphc",
      ".jphc"
    },
    // entry #56
    { UID_JPIPHTJ2KReferencedTransferSyntax,
      "JPIP HTJ2K Referenced",
      EXS_JPIPHTJ2KReferenced,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Referenced,
      EPC_unknown,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #57
    { UID_JPIPHTJ2KReferencedDeflateTransferSyntax,
      "JPIP HTJ2K Referenced Deflate",
      EXS_JPIPHTJ2KReferencedDeflate,
      EBO_LittleEndian,
      EBO_LittleEndian,
      EVT_Explicit,
      EPE_Referenced,
      EPC_unknown,
      OFFalse,
      0L, 0L,
#ifdef WITH_ZLIB
      ESC_zlib,
#else
      ESC_unsupported,
#endif
      EXV_Standard,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    },
    // entry #58
    { UID_PrivateGE_LEI_WithBigEndianPixelDataTransferSyntax,
      "Private GE Little Endian Implicit with big endian pixel data",
      EXS_PrivateGE_LEI_WithBigEndianPixelData,
      EBO_LittleEndian,
      EBO_BigEndian,
      EVT_Implicit,
      EPE_Native,
      EPC_Uncompressed,
      OFFalse,
      0L, 0L,
      ESC_none,
      EXV_Private,
      DCXFER_DEFAULT_MIME_TYPE,
      DCXFER_DEFAULT_EXTENSION
    }
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
    pixelDataEncoding(EPE_unknown),
    pixelDataCompression(EPC_unknown),
    pixelDataFragmentable(OFFalse),
    JPEGProcess8(0L),
    JPEGProcess12(0L),
    streamCompression(ESC_none),
    xferValidity(EXV_unknown),
    mimeType(DCXFER_DEFAULT_MIME_TYPE),
    filenameExtension(DCXFER_DEFAULT_EXTENSION)
{
    /* casting the enum to an integer should be safe */
    const int i = OFstatic_cast(int, xfer);
    /* ... but double-check this to avoid invalid array access, e.g. for EXS_Unknown */
    if ((i >= 0) && (i < DIM_OF_XferNames) && (XferNames[i].xfer == xfer))
    {
        xferSyn               = XferNames[i].xfer;
        xferID                = XferNames[i].xferID;
        xferName              = XferNames[i].xferName;
        byteOrder             = XferNames[i].byteOrder;
        pixelDataByteOrder    = XferNames[i].pixelDataByteOrder;
        vrType                = XferNames[i].vrType;
        pixelDataEncoding     = XferNames[i].pixelDataEncoding;
        pixelDataCompression  = XferNames[i].pixelDataCompression;
        pixelDataFragmentable = XferNames[i].pixelDataFragmentable;
        JPEGProcess8          = XferNames[i].JPEGProcess8;
        JPEGProcess12         = XferNames[i].JPEGProcess12;
        streamCompression     = XferNames[i].streamCompression;
        xferValidity          = XferNames[i].xferValidity;
        mimeType              = XferNames[i].mimeType;
        filenameExtension     = XferNames[i].filenameExtension;
    }
}


// ********************************


DcmXfer::DcmXfer(const char *xferName_xferID)
  : xferID(""),
    xferName(ERROR_XferName),
    xferSyn(EXS_Unknown),
    byteOrder(EBO_unknown),
    pixelDataByteOrder(EBO_unknown),
    vrType(EVT_Implicit),
    pixelDataEncoding(EPE_unknown),
    pixelDataCompression(EPC_unknown),
    pixelDataFragmentable(OFFalse),
    JPEGProcess8(0L),
    JPEGProcess12(0L),
    streamCompression(ESC_none),
    xferValidity(EXV_unknown),
    mimeType(DCXFER_DEFAULT_MIME_TYPE),
    filenameExtension(DCXFER_DEFAULT_EXTENSION)
{
    if (xferName_xferID != NULL)
    {
        int i;
        /* first search for transfer syntax identifier (UID) */
        for (i = 0; (i < DIM_OF_XferNames) && (strcmp(XferNames[i].xferID, xferName_xferID) != 0); ++i);
        /* and then, if not found... */
        if (i >= DIM_OF_XferNames)
        {
            /* try with the transfer syntax name */
            for (i = 0; (i < DIM_OF_XferNames) && (strcmp(XferNames[i].xferName, xferName_xferID) != 0); ++i);
        }
        if (i < DIM_OF_XferNames)
        {
            /* if found, copy all properties */
            xferSyn               = XferNames[i].xfer;
            xferID                = XferNames[i].xferID;
            xferName              = XferNames[i].xferName;
            byteOrder             = XferNames[i].byteOrder;
            pixelDataByteOrder    = XferNames[i].pixelDataByteOrder;
            vrType                = XferNames[i].vrType;
            pixelDataEncoding     = XferNames[i].pixelDataEncoding;
            pixelDataCompression  = XferNames[i].pixelDataCompression;
            pixelDataFragmentable = XferNames[i].pixelDataFragmentable;
            JPEGProcess8          = XferNames[i].JPEGProcess8;
            JPEGProcess12         = XferNames[i].JPEGProcess12;
            streamCompression     = XferNames[i].streamCompression;
            xferValidity          = XferNames[i].xferValidity;
            mimeType              = XferNames[i].mimeType;
            filenameExtension     = XferNames[i].filenameExtension;
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
    pixelDataEncoding(newXfer.pixelDataEncoding),
    pixelDataCompression(newXfer.pixelDataCompression),
    pixelDataFragmentable(newXfer.pixelDataFragmentable),
    JPEGProcess8(newXfer.JPEGProcess8),
    JPEGProcess12(newXfer.JPEGProcess12),
    streamCompression(newXfer.streamCompression),
    xferValidity(newXfer.xferValidity),
    mimeType(newXfer.mimeType),
    filenameExtension(newXfer.filenameExtension)
{
}


// ********************************


DcmXfer::~DcmXfer()
{
}


// ********************************


DcmXfer &DcmXfer::operator=(const E_TransferSyntax xfer)
{
    /* casting the enum to an integer should be safe */
    const int i = OFstatic_cast(int, xfer);
    /* ... but double-check this to avoid invalid array access, e.g. for EXS_Unknown */
    if ((i >= 0) && (i < DIM_OF_XferNames) && (XferNames[i].xfer == xfer))
    {
        xferSyn               = XferNames[i].xfer;
        xferID                = XferNames[i].xferID;
        xferName              = XferNames[i].xferName;
        byteOrder             = XferNames[i].byteOrder;
        pixelDataByteOrder    = XferNames[i].pixelDataByteOrder;
        vrType                = XferNames[i].vrType;
        pixelDataEncoding     = XferNames[i].pixelDataEncoding;
        pixelDataCompression  = XferNames[i].pixelDataCompression;
        pixelDataFragmentable = XferNames[i].pixelDataFragmentable;
        JPEGProcess8          = XferNames[i].JPEGProcess8;
        JPEGProcess12         = XferNames[i].JPEGProcess12;
        streamCompression     = XferNames[i].streamCompression;
        xferValidity          = XferNames[i].xferValidity;
        mimeType              = XferNames[i].mimeType;
        filenameExtension     = XferNames[i].filenameExtension;
    } else {
        xferSyn               = EXS_Unknown;
        xferID                = "";
        xferName              = ERROR_XferName;
        byteOrder             = EBO_unknown;
        pixelDataByteOrder    = EBO_unknown;
        vrType                = EVT_Implicit;
        pixelDataEncoding     = EPE_unknown;
        pixelDataCompression  = EPC_unknown;
        pixelDataFragmentable = OFFalse;
        JPEGProcess8          = 0L;
        JPEGProcess12         = 0L;
        streamCompression     = ESC_none;
        xferValidity          = EXV_unknown;
        mimeType              = DCXFER_DEFAULT_MIME_TYPE;
        filenameExtension     = DCXFER_DEFAULT_EXTENSION;
    }
    return *this;
}


// ********************************


DcmXfer &DcmXfer::operator=(const DcmXfer &newXfer)
{
    if (this != &newXfer)
    {
        xferSyn               = newXfer.xferSyn;
        xferID                = newXfer.xferID;
        xferName              = newXfer.xferName;
        byteOrder             = newXfer.byteOrder;
        pixelDataByteOrder    = newXfer.pixelDataByteOrder;
        vrType                = newXfer.vrType;
        pixelDataEncoding     = newXfer.pixelDataEncoding;
        pixelDataCompression  = newXfer.pixelDataCompression;
        pixelDataFragmentable = newXfer.pixelDataFragmentable;
        JPEGProcess8          = newXfer.JPEGProcess8;
        JPEGProcess12         = newXfer.JPEGProcess12;
        streamCompression     = newXfer.streamCompression;
        xferValidity          = newXfer.xferValidity;
        mimeType              = newXfer.mimeType;
        filenameExtension     = newXfer.filenameExtension;
    }
    return *this;
}


// ********************************

const char *DcmXfer::getXferKeyword() const
{
    return dcmFindKeywordOfUID(xferID, "" /* defaultValue */);
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

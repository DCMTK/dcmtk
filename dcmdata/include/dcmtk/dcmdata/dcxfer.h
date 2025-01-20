/*
 *
 *  Copyright (C) 1994-2024, OFFIS e.V.
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
 *  Purpose: Handling of transfer syntaxes
 *
 */

#ifndef DCXFER_H
#define DCXFER_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/ofstd/ofdeprec.h"     /* for OFdeprecated */

// include this file in doxygen documentation

/** @file dcxfer.h
 *  @brief definition and handling of transfer syntaxes
 */

/** enumeration of all DICOM transfer syntaxes known to the toolkit
 */
typedef enum {
    /// unknown transfer syntax or dataset created in memory
    EXS_Unknown = -1,
    /// Implicit VR Little Endian
    EXS_LittleEndianImplicit = 0,
    /// Implicit VR Big Endian (does not really exist, only used internally)
    EXS_BigEndianImplicit = 1,
    /// Explicit VR Little Endian
    EXS_LittleEndianExplicit = 2,
    /// Explicit VR Big Endian
    EXS_BigEndianExplicit = 3,
    /// Encapsulated Uncompressed Explicit VR Little Endian
    EXS_EncapsulatedUncompressedLittleEndianExplicit = 4,
    /// JPEG Baseline (lossy)
    EXS_JPEGProcess1 = 5,
    /// JPEG Extended Sequential (lossy, 8/12 bit)
    EXS_JPEGProcess2_4 = 6,
    /// JPEG Extended Sequential (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess3_5 = 7,
    /// JPEG Spectral Selection, Non-Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess6_8 = 8,
    /// JPEG Spectral Selection, Non-Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess7_9 = 9,
    /// JPEG Full Progression, Non-Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess10_12 = 10,
    /// JPEG Full Progression, Non-Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess11_13 = 11,
    /// JPEG Lossless with any selection value
    EXS_JPEGProcess14 = 12,
    /// JPEG Lossless with any selection value, arithmetic coding
    EXS_JPEGProcess15 = 13,
    /// JPEG Extended Sequential, Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess16_18 = 14,
    /// JPEG Extended Sequential, Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess17_19 = 15,
    /// JPEG Spectral Selection, Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess20_22 = 16,
    /// JPEG Spectral Selection, Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess21_23 = 17,
    /// JPEG Full Progression, Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess24_26 = 18,
    /// JPEG Full Progression, Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess25_27 = 19,
    /// JPEG Lossless, Hierarchical
    EXS_JPEGProcess28 = 20,
    /// JPEG Lossless, Hierarchical, arithmetic coding
    EXS_JPEGProcess29 = 21,
    /// JPEG Lossless, Selection Value 1
    EXS_JPEGProcess14SV1 = 22,
    /// Run Length Encoding (lossless)
    EXS_RLELossless = 23,
    /// Deflated Explicit VR Little Endian
    EXS_DeflatedLittleEndianExplicit = 24,
    /// JPEG-LS (lossless)
    EXS_JPEGLSLossless = 25,
    /// JPEG-LS (lossless or near-lossless mode)
    EXS_JPEGLSLossy = 26,
    /// JPEG 2000 (lossless)
    EXS_JPEG2000LosslessOnly = 27,
    /// JPEG 2000 (lossless or lossy)
    EXS_JPEG2000 = 28,
    /// JPEG 2000 part 2 multi-component extensions (lossless)
    EXS_JPEG2000MulticomponentLosslessOnly = 29,
    /// JPEG 2000 part 2 multi-component extensions (lossless or lossy)
    EXS_JPEG2000Multicomponent = 30,
    /// JPIP Referenced
    EXS_JPIPReferenced = 31,
    /// JPIP Referenced Deflate
    EXS_JPIPReferencedDeflate = 32,
    /// MPEG2 Main Profile at Main Level
    EXS_MPEG2MainProfileAtMainLevel = 33,
    /// Fragmentable MPEG2 Main Profile / Main Level
    EXS_FragmentableMPEG2MainProfileMainLevel = 34,
    /// MPEG2 Main Profile at High Level
    EXS_MPEG2MainProfileAtHighLevel = 35,
    /// Fragmentable MPEG2 Main Profile / High Level
    EXS_FragmentableMPEG2MainProfileHighLevel = 36,
    /// MPEG4 High Profile / Level 4.1
    EXS_MPEG4HighProfileLevel4_1 = 37,
    /// Fragmentable MPEG4 High Profile / Level 4.1
    EXS_FragmentableMPEG4HighProfileLevel4_1 = 38,
    /// MPEG4 BD-compatible High Profile / Level 4.1
    EXS_MPEG4BDcompatibleHighProfileLevel4_1 = 39,
    /// Fragmentable MPEG4 BD-compatible High Profile / Level 4.1
    EXS_FragmentableMPEG4BDcompatibleHighProfileLevel4_1 = 40,
    /// MPEG4 High Profile / Level 4.2 For 2D Video
    EXS_MPEG4HighProfileLevel4_2_For2DVideo = 41,
    /// Fragmentable MPEG4 High Profile / Level 4.2 For 2D Video
    EXS_FragmentableMPEG4HighProfileLevel4_2_For2DVideo = 42,
    /// MPEG4 High Profile / Level 4.2 For 3D Video
    EXS_MPEG4HighProfileLevel4_2_For3DVideo = 43,
    /// Fragmentable MPEG4 Stereo High Profile / Level 4.2
    EXS_FragmentableMPEG4HighProfileLevel4_2_For3DVideo = 44,
    /// MPEG4 Stereo High Profile / Level 4.2
    EXS_MPEG4StereoHighProfileLevel4_2 = 45,
    /// Fragmentable HEVC/H.265 Main Profile / Level 5.1
    EXS_FragmentableMPEG4StereoHighProfileLevel4_2 = 46,
    /// HEVC/H.265 Main Profile / Level 5.1
    EXS_HEVCMainProfileLevel5_1 = 47,
    /// HEVC/H.265 Main 10 Profile / Level 5.1
    EXS_HEVCMain10ProfileLevel5_1 = 48,
    /// JPEG XL Lossless
    EXS_JPEGXLLossless = 49,
    /// JPEG XL JPEG Recompression
    EXS_JPEGXLJPEGRecompression = 50,
    /// JPEG XL
    EXS_JPEGXL = 51,
    /// High-Throughput JPEG 2000 Image Compression (Lossless Only)
    EXS_HighThroughputJPEG2000LosslessOnly = 52,
    /// High-Throughput JPEG 2000 with RPCL Options Image Compression (Lossless Only)
    EXS_HighThroughputJPEG2000withRPCLOptionsLosslessOnly = 53,
    /// High-Throughput JPEG 2000 Image Compression
    EXS_HighThroughputJPEG2000 = 54,
    /// JPIP HTJ2K Referenced
    EXS_JPIPHTJ2KReferenced = 55,
    /// JPIP HTJ2K Referenced Deflate
    EXS_JPIPHTJ2KReferencedDeflate = 56,
    /// Private GE Little Endian Implicit with big endian pixel data
    EXS_PrivateGE_LEI_WithBigEndianPixelData = 57
} E_TransferSyntax;


/** enumeration of byte orders
 */
typedef enum {
    /// unknown
    EBO_unknown = 0,
    /// little endian
    EBO_LittleEndian = 1,
    /// big endian
    EBO_BigEndian = 2
} E_ByteOrder;

/** enumeration of VR encoding options
 */
typedef enum {
    /// implicit VR encoding
    EVT_Implicit = 0,
    /// explicit VR encoding
    EVT_Explicit = 1
} E_VRType;

/** enumeration of pixel data encoding options
 */
typedef enum {
    /// unknown (e.g. invalid value)
    EPE_unknown = 0,
    /// pixel data in native format (uncompressed)
    EPE_Native = 1,
    /// pixel data encapsulated (e.g. compressed)
    EPE_Encapsulated = 2,
    /// pixel data referenced (e.g. using a URL)
    EPE_Referenced = 3
} E_PixelDataEncoding;

/** enumeration of pixel data compression options
 */
typedef enum {
    /// unknown (e.g. referenced pixel data)
    EPC_unknown = 0,
    /// pixel data is uncompressed
    EPC_Uncompressed = 1,
    /// pixel data is compressed in a lossless manner
    EPC_LosslessCompressed = 2,
    /// pixel data is compressed in a lossy manner
    EPC_LossyCompressed = 3
} E_PixelDataCompression;

/** enumeration of stream compression techniques
 */
typedef enum
{
    /// no stream compression
    ESC_none = 0
    /// unsupported stream compression
  , ESC_unsupported = 1
#ifdef WITH_ZLIB
    /// zlib stream compression.
    /// This enum value only available if DCMTK is compiled with ZLIB support enabled.
  , ESC_zlib = 2
#endif
} E_StreamCompression;

/** validity of the transfer syntax definition
 */
typedef enum {
    /// unknown (e.g. invalid value)
    EXV_unknown = 0,
    /// internal, not used outside DCMTK
    EXV_Internal = 1,
    /// defined in the DICOM standard
    EXV_Standard = 2,
    /// retired from the DICOM standard
    EXV_Retired = 3,
    /// private, non-standard definition
    EXV_Private = 4
} E_XferValidity;


/** a class that allows for a lookup of Transfer Syntax properties and readable descriptions
 */
class DCMTK_DCMDATA_EXPORT DcmXfer
{
public:
    /** constructor
     *  @param xfer transfer syntax enum
     */
    DcmXfer(E_TransferSyntax xfer);

    /** constructor.
     *  Performs a look-up based on the given transfer syntax name or identifier.
     *  @param xferName_xferID transfer syntax name or identifier (UID)
     */
    DcmXfer(const char *xferName_xferID);

    /** copy constructor
     *  @param newXfer transfer syntax to be copied
     */
    DcmXfer(const DcmXfer &newXfer);

    /** destructor
     */
    ~DcmXfer();

    /** assignment operator for transfer syntax enum
     *  @param xfer transfer syntax enum
     *  @return reference to this transfer syntax instance
     */
    DcmXfer &operator=(const E_TransferSyntax xfer);

    /** copy assignment operator
     *  @param newXfer transfer syntax to be copied
     *  @return reference to this transfer syntax instance
     */
    DcmXfer &operator=(const DcmXfer &newXfer);

    /** comparison operator
     *  @param xfer transfer syntax enum to compare with
     *  @return true if equal, false if not equal
     */
    OFBool operator==(const E_TransferSyntax xfer) const
    {
        return xferSyn == xfer;
    }

    /** comparison operator
     *  @param xfer transfer syntax to compare with
     *  @return true if equal, false if not equal
     */
    OFBool operator==(const DcmXfer &xfer) const
    {
        return xferSyn == xfer.getXfer();
    }

    /** comparison operator
     *  @param xfer transfer syntax enum to compare with
     *  @return true if not equal, false if equal
     */
    OFBool operator!=(const E_TransferSyntax xfer) const
    {
        return xferSyn != xfer;
    }

    /** comparison operator
     *  @param xfer transfer syntax to compare with
     *  @return true if not equal, false if equal
     */
    OFBool operator!=(const DcmXfer &xfer) const
    {
        return xferSyn != xfer.getXfer();
    }

    /** check whether transfer syntax is valid, i.e.\ not unknown or uninitialized.
     *  An unknown transfer syntax is usually the result of an unsuccessful look-up.
     *  An uninitialized transfer syntax is typically intended by the user (passing
     *  EXS_Unknown to the constructor of this class).
     *  @return true if transfer syntax is valid, false otherwise
     */
    inline OFBool isValid() const
    {
        return xferSyn != EXS_Unknown;
    }

    /** get transfer syntax enum for this transfer syntax
     *  @return transfer syntax enum for this transfer syntax
     */
    inline E_TransferSyntax getXfer() const
    {
        return xferSyn;
    }

    /** get byte order for this transfer syntax
     *  @return byte order for this transfer syntax
     */
    inline E_ByteOrder getByteOrder() const
    {
        return byteOrder;
    }

    /** get pixel data byte order for this transfer syntax
     *  @return pixel data byte order for this transfer syntax
     */
    inline E_ByteOrder getPixelDataByteOrder() const
    {
        return pixelDataByteOrder;
    }

    /** get DCMTK-specific name for this transfer syntax
     *  @return name string for this transfer syntax, empty string if undefined
     */
    inline const char *getXferName() const
    {
        return xferName;
    }

    /** get DICOM keyword for this transfer syntax
     *  @return keyword string for this transfer syntax, empty string if undefined
     */
    const char *getXferKeyword() const;

    /** get identifier (UID) for this transfer syntax
     *  @return UID string for this transfer syntax, empty string if undefined
     */
    inline const char *getXferID() const
    {
        return xferID;
    }

    /** check whether transfer syntax uses little endian byte order
     *  @return true if transfer syntax is little endian, false otherwise
     */
    inline OFBool isLittleEndian() const
    {
        return byteOrder == EBO_LittleEndian;
    }

    /** check whether transfer syntax uses big endian byte order
     *  @return true if transfer syntax is big endian, false otherwise
     */
    inline OFBool isBigEndian() const
    {
        return byteOrder == EBO_BigEndian;
    }

    /** check whether transfer syntax uses implicit VR encoding
     *  @return true if transfer syntax is implicit VR, false otherwise
     */
    inline OFBool isImplicitVR() const
    {
        return vrType == EVT_Implicit;
    }

    /** check whether transfer syntax uses explicit VR encoding
     *  @return true if transfer syntax is explicit VR, false otherwise
     */
    inline OFBool isExplicitVR() const
    {
        return vrType == EVT_Explicit;
    }

    /** check whether transfer syntax uses native format for pixel data
     *  @return true if transfer syntax uses native format, false otherwise
     */
    inline OFBool usesNativeFormat() const
    {
        return pixelDataEncoding == EPE_Native;
    }

    /** check whether transfer syntax uses encapsulated format for pixel data
     *  @return true if transfer syntax uses encapsulated format, false otherwise
     */
    inline OFBool usesEncapsulatedFormat() const
    {
        return pixelDataEncoding == EPE_Encapsulated;
    }

    /** check whether transfer syntax uses (0028,7FE0) Pixel Data Provider URL
     *  to reference pixel data
     *  @return true if transfer syntax uses URL reference to pixel data
     */
    inline OFBool usesReferencedPixelData() const
    {
        return pixelDataEncoding == EPE_Referenced;
    }

    /** check whether transfer syntax uses encapsulated format for pixel data
     *  (and whether the pixel data is compressed)
     *  @deprecated This method is deprecated and will be removed in the future.
     *    Instead, please use usesEncapsulatedFormat() and/or isPixelDataCompressed().
     *  @return true if transfer syntax uses encapsulated format, false otherwise
     */
    OFdeprecated inline OFBool isEncapsulated() const
    {
        return usesEncapsulatedFormat() && isPixelDataCompressed();
    }

    /** check whether transfer syntax uses non-encapsulated format for pixel data
     *  @deprecated This method is deprecated and will be removed in the future.
     *    Please use usesNativeFormat() or ! usesEncapsulatedFormat() instead.
     *    Also see usesReferencedPixelData().
     *  @return true if transfer syntax uses non-encapsulated format, false otherwise
     */
    OFdeprecated inline OFBool isNotEncapsulated() const
    {
        return usesNativeFormat();
    }

    /** return 8-bit JPEG process ID for this transfer syntax.
     *  Lossy JPEG transfer syntaxes support two alternative JPEG encoding processes:
     *  8 and 12 bits.
     *  @return 8-bit JPEG process ID, or 0 for a non-JPEG transfer syntax
     */
    inline Uint32 getJPEGProcess8Bit() const
    {
        return JPEGProcess8;
    }

    /** return 12-bit JPEG process ID for this transfer syntax.
     *  Lossy JPEG transfer syntaxes support two alternative JPEG encoding processes:
     *  8 and 12 bits.
     *  @return 12-bit JPEG process ID, or 0 for a non-JPEG transfer syntax
     */
    inline Uint32 getJPEGProcess12Bit() const
    {
        return JPEGProcess12;
    }

    /** check whether transfer syntax uses uncompressed pixel data
     *  @return true if transfer syntax uses uncompressed pixel data, false otherwise
     */
    inline OFBool isPixelDataUncompressed() const
    {
        return pixelDataCompression == EPC_Uncompressed;
    }

    /** check whether transfer syntax uses compressed pixel data
     *  @return true if transfer syntax uses compressed pixel data, false otherwise
     */
    inline OFBool isPixelDataCompressed() const
    {
        return isPixelDataLossyCompressed() || isPixelDataLosslessCompressed();
    }

    /** check whether transfer syntax uses a lossy compression for pixel data
     *  @return true if transfer syntax uses a lossy compression, false otherwise
     */
    inline OFBool isPixelDataLossyCompressed() const
    {
        return pixelDataCompression == EPC_LossyCompressed;
    }

    /** check whether transfer syntax uses a lossless compression for pixel data
     *  @return true if transfer syntax uses a lossless compression or no compression,
     *    false otherwise
     */
    inline OFBool isPixelDataLosslessCompressed() const
    {
        return pixelDataCompression == EPC_LosslessCompressed;
    }

    /** check whether transfer syntax allows the encapsulated pixel stream of
     *  encoded pixel data to be split into one or more fragments
     *  @return true if transfer syntax supports fragmentable pixel data
     */
    inline OFBool isPixelDataFragmentable() const
    {
        return usesEncapsulatedFormat() && pixelDataFragmentable;
    }

    /** check whether transfer syntax is defined in the DICOM standard
     *  @return true if transfer syntax is standard, false otherwise
     */
    inline OFBool isStandard() const
    {
        return xferValidity == EXV_Standard;
    }

    /** check whether transfer syntax has been retired from the DICOM standard
     *  @return true if transfer syntax is retired, false otherwise
     */
    inline OFBool isRetired() const
    {
        return xferValidity == EXV_Retired;
    }

    /** check whether transfer syntax is a private definition
     *  @return true if transfer syntax is private, false otherwise
     */
    inline OFBool isPrivate() const
    {
        return xferValidity == EXV_Private;
    }

    /** get stream compression type for this transfer syntax
     *  @return stream compression type for this transfer syntax
     */
    inline E_StreamCompression getStreamCompression() const
    {
        return streamCompression;
    }

    /** check whether transfer syntax compresses the entire dataset.
     *  This does not mean that the stream compression is actually supported.
     *  @return true if transfer syntax compresses the dataset, false otherwise
     */
    inline OFBool isDatasetCompressed() const
    {
        return streamCompression != ESC_none;
    }

    /** check whether transfer syntax uses any kind of lossless compression,
     *  either for the pixel data or the entire dataset
     *  @return true if transfer syntax uses any kind of lossless compression,
     *    false otherwise
     */
    inline OFBool isLosslessCompressed() const
    {
        return isPixelDataLosslessCompressed() || isDatasetCompressed();
    }

    /** get the number of bytes needed to describe the tag, length, VR and any
     *  reserved fields for this transfer syntax when encoding the specified VR.
     *  @param evr value representation to be encoded in this transfer syntax
     *  @return number of bytes needed
     */
    Uint32 sizeofTagHeader(DcmEVR evr) const;

private:
    /// transfer syntax UID
    const char            *xferID;

    /// transfer syntax name
    const char            *xferName;

    /// transfer syntax enum
    E_TransferSyntax       xferSyn;

    /// transfer syntax byte order
    E_ByteOrder            byteOrder;

    /// transfer syntax byte order for pixel data
    E_ByteOrder            pixelDataByteOrder;

    /// transfer syntax VR encoding (implicit or explicit)
    E_VRType               vrType;

    /// transfer syntax encoding of pixel data (e.g. native, encapsulated, referenced)
    E_PixelDataEncoding    pixelDataEncoding;

    /// transfer syntax compression of pixel data (e.g. uncompressed, lossless, lossy)
    E_PixelDataCompression pixelDataCompression;

    /// flag indicating whether this transfer syntax supports fragmentable pixel data
    OFBool                 pixelDataFragmentable;

    /// 8-bit lossy JPEG process ID for this transfer syntax, 0 if not applicable
    Uint32                 JPEGProcess8;

    /// 12-bit lossy JPEG process ID for this transfer syntax, 0 if not applicable
    Uint32                 JPEGProcess12;

    /// transfer syntax stream compression type (e.g. zlib)
    E_StreamCompression    streamCompression;

    /// validity of the transfer syntax definition (e.g. standard, retired, private)
    E_XferValidity         xferValidity;
};

/** global constant describing the byte order on the machine the application
 *  is currently executing on. This is runtime and not compile time information
 *  because of "fat" binaries that can be executed on multiple CPU types (e.g. NeXTStep)
 */
extern DCMTK_DCMDATA_EXPORT const E_ByteOrder gLocalByteOrder;

#endif // DCXFER_H

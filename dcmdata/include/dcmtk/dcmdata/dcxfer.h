/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Purpose: Handling of transfer syntaxes
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:43 $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCXFER_H
#define DCXFER_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcvr.h"

/** enumeration of all DICOM transfer syntaxes known to the toolkit
 */
typedef enum {
    /// unknown transfer syntax or dataset created in-memory
    EXS_Unknown = -1,
    /// Implicit VR Little Endian
    EXS_LittleEndianImplicit = 0,
    /// Implicit VR Big Endian (pseudo transfer syntax that does not really exist)
    EXS_BigEndianImplicit = 1,
    /// Explicit VR Little Endian
    EXS_LittleEndianExplicit = 2,
    /// Explicit VR Big Endian
    EXS_BigEndianExplicit = 3,
    /// JPEG Baseline (lossy)
    EXS_JPEGProcess1TransferSyntax = 4,
    /// JPEG Extended Sequential (lossy, 8/12 bit)
    EXS_JPEGProcess2_4TransferSyntax = 5,
    /// JPEG Extended Sequential (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess3_5TransferSyntax = 6,
    /// JPEG Spectral Selection, Non-Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess6_8TransferSyntax = 7,
    /// JPEG Spectral Selection, Non-Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess7_9TransferSyntax = 8,
    /// JPEG Full Progression, Non-Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess10_12TransferSyntax = 9,
    /// JPEG Full Progression, Non-Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess11_13TransferSyntax = 10,
    /// JPEG Lossless with any selection value
    EXS_JPEGProcess14TransferSyntax = 11,
    /// JPEG Lossless with any selection value, arithmetic coding
    EXS_JPEGProcess15TransferSyntax = 12,
    /// JPEG Extended Sequential, Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess16_18TransferSyntax = 13,
    /// JPEG Extended Sequential, Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess17_19TransferSyntax = 14,
    /// JPEG Spectral Selection, Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess20_22TransferSyntax = 15,
    /// JPEG Spectral Selection, Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess21_23TransferSyntax = 16,
    /// JPEG Full Progression, Hierarchical (lossy, 8/12 bit)
    EXS_JPEGProcess24_26TransferSyntax = 17,
    /// JPEG Full Progression, Hierarchical (lossy, 8/12 bit), arithmetic coding
    EXS_JPEGProcess25_27TransferSyntax = 18,
    /// JPEG Lossless, Hierarchical
    EXS_JPEGProcess28TransferSyntax = 19,
    /// JPEG Lossless, Hierarchical, arithmetic coding
    EXS_JPEGProcess29TransferSyntax = 20,
    /// JPEG Lossless, Selection Value 1
    EXS_JPEGProcess14SV1TransferSyntax = 21,
    /// Run Length Encoding (lossless)
    EXS_RLELossless = 22,
    /// JPEG-LS (lossless)
    EXS_JPEGLSLossless = 23,
    /// JPEG-LS (lossless or near-lossless mode)
    EXS_JPEGLSLossy = 24,
    /// Deflated Explicit VR Little Endian
    EXS_DeflatedLittleEndianExplicit = 25,
    /// JPEG 2000 (lossless)
    EXS_JPEG2000LosslessOnly = 26,
    /// JPEG 2000 (lossless or lossy)
    EXS_JPEG2000 = 27,
    /// MPEG2 Main Profile at Main Level
    EXS_MPEG2MainProfileAtMainLevel = 28,
    /// MPEG2 Main Profile at High Level
    EXS_MPEG2MainProfileAtHighLevel = 29,
    /// JPEG 2000 part 2 multi-component extensions (lossless)
    EXS_JPEG2000MulticomponentLosslessOnly = 30,
    /// JPEG 2000 part 2 multi-component extensions (lossless or lossy)
    EXS_JPEG2000Multicomponent = 31,
    /// JPIP Referenced
    EXS_JPIPReferenced = 32,
    /// JPIP Referenced Deflate
    EXS_JPIPReferencedDeflate = 33
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

/** enumeration of pixel data encapsulation options
 */
typedef enum {
    /// pixel data not encapsulated
    EJE_NotEncapsulated = 0,

    /// pixel data encapsulated
    EJE_Encapsulated = 1
} E_JPEGEncapsulated;

/** enumeration of stream compression techniques
 */
typedef enum
{
    /// no stream compression
    ESC_none = 0
    /// unsupported stream compression
  , ESC_unsupported = 1
#ifdef WITH_ZLIB
    /// zlib stream compression
  , ESC_zlib = 2
#endif
} E_StreamCompression;


/** a class that allows for a lookup of Transfer Syntax properties and readable descriptions
 */
class DcmXfer
{
public:
    /** constructor
     *  @param xfer transfer syntax enum
     */
    DcmXfer( E_TransferSyntax xfer );

    /** constructor
     *  @param xferName_xferID transfer syntax name as string
     */
    DcmXfer( const char *xferName_xferID );

    /// copy constructor
    DcmXfer( const DcmXfer &newXfer );

    /// destructor
    ~DcmXfer();

    /// assignment operator for transfer syntax enum
    DcmXfer & operator = ( const E_TransferSyntax xfer );

    /// copy assignment operator
    DcmXfer & operator = ( const DcmXfer &newtag );

    /// return transfer syntax enum for this transfer syntax
    inline E_TransferSyntax getXfer() const  { return xferSyn; }

    /// return byte order for this transfer syntax
    inline E_ByteOrder getByteOrder() const { return byteOrder; }

    /// return name string for this transfer syntax
    inline const char* getXferName() const { return xferName; }

    /// return UID string for this transfer syntax
    inline const char* getXferID() const { return xferID; }

    /// return true if transfer syntax is little endian, false otherwise
    inline OFBool isLittleEndian() const
    {
        return byteOrder == EBO_LittleEndian;
    }

    /// return true if transfer syntax is big endian, false otherwise
    inline OFBool isBigEndian() const { return byteOrder == EBO_BigEndian; }

    /// return true if transfer syntax is implicit VR, false otherwise
    inline OFBool isImplicitVR() const { return vrType == EVT_Implicit; }

    /// return true if transfer syntax is explicit VR, false otherwise
    inline OFBool isExplicitVR() const { return vrType == EVT_Explicit; }

    /// return true if transfer syntax is encapsulated, false otherwise
    inline OFBool isEncapsulated() const
    {
        return encapsulated == EJE_Encapsulated;
    }

    /// return true if transfer syntax is native (non-encapsulated), false otherwise
    inline OFBool isNotEncapsulated() const
    {
        return encapsulated == EJE_NotEncapsulated;
    }

    /** return 8-bit JPEG process ID for this transfer syntax.
     *  Lossy JPEG transfer syntaxes support two alternative JPEG encoding processes - 8 and 12 bits.
     *  When called for a non-JPEG transfer syntax, returns 0.
     *  @return 8-bit JPEG process ID
     */
    inline Uint32 getJPEGProcess8Bit() const { return JPEGProcess8; }

    /** return 12-bit JPEG process ID for this transfer syntax.
     *  Lossy JPEG transfer syntaxes support two alternative JPEG encoding processes - 8 and 12 bits.
     *  When called for a non-JPEG transfer syntax, returns 0.
     *  @return 12-bit JPEG process ID
     */
    inline Uint32 getJPEGProcess12Bit() const { return JPEGProcess12;}

    /// return true if transfer syntax is retired, false otherwise
    inline OFBool isRetired() const
    {
        return retired;
    }

    /// return stream compression type for this transfer syntax
    inline E_StreamCompression getStreamCompression() const { return streamCompression;}

    /** return the number of bytes needed to describe the tag, length, VR
     *  and any reserved fields for this transfer syntax when encoding the
     *  specified VR.
     *  @param evr value representation to be encoded in this transfer syntax
     *  @return number of bytes needed
     */
    Uint32 sizeofTagHeader(DcmEVR evr) const;

private:
    /// transfer syntax UID
    const char          *xferID;

    /// transfer syntax name
    const char          *xferName;

    /// transfer syntax enum
    E_TransferSyntax    xferSyn;

    /// transfer syntax byte order
    E_ByteOrder         byteOrder;

    /// transfer syntax VR encoding (implicit/explicit)
    E_VRType            vrType;

    /// transfer syntax encapsulated or native
    E_JPEGEncapsulated  encapsulated;

    /// 8-bit lossy JPEG process ID for this transfer syntax, 0 if not applicable
    Uint32              JPEGProcess8;

    /// 12-bit lossy JPEG process ID for this transfer syntax, 0 if not applicable
    Uint32              JPEGProcess12;

    /// flag indicating whether this transfer syntax has been retired from DICOM
    OFBool              retired;

    /// transfer syntax stream compression type
    E_StreamCompression streamCompression;

};

/** global constant describing the byte order on the machine the application
 *  is currently executing on. This is runtime and not compile time information
 *  because of "fat" binaries that can be executed on multiple CPU types (e.g. NeXTStep)
 */
extern const E_ByteOrder gLocalByteOrder;

#endif // DCXFER_H

/*
 * CVS/RCS Log:
 * $Log: dcxfer.h,v $
 * Revision 1.24  2010-10-14 13:15:43  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.23  2010-09-15 08:46:16  joergr
 * Added definition of XML encoding transfer syntax (Supplement 114) and JPIP
 * referenced transfer syntaxes (Supplement 106).
 *
 * Revision 1.22  2010-09-02 12:12:43  joergr
 * Added support for "MPEG2 Main Profile @ High Level" transfer syntax.
 *
 * Revision 1.21  2008-06-23 12:09:13  joergr
 * Fixed inconsistencies in Doxygen API documentation.
 *
 * Revision 1.20  2008-04-23 06:39:37  meichel
 * Added new method DcmXfer::isRetired that returns true for
 *   retired transfer syntaxes.
 *
 * Revision 1.19  2007/11/29 14:30:19  meichel
 * Write methods now handle large raw data elements (such as pixel data)
 *   without loading everything into memory. This allows very large images to
 *   be sent over a network connection, or to be copied without ever being
 *   fully in memory.
 *
 * Revision 1.18  2005/12/08 16:29:17  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.17  2005/10/25 08:55:32  meichel
 * Updated list of UIDs and added support for new transfer syntaxes
 *   and storage SOP classes.
 *
 * Revision 1.16  2004/04/06 18:01:50  joergr
 * Updated data dictionary, UIDs and transfer syntaxes for the latest Final Text
 * Supplements (42 and 47) and Correction Proposals (CP 25).
 *
 * Revision 1.15  2002/08/27 16:55:41  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.14  2001/11/08 16:17:30  meichel
 * Updated data dictionary, UIDs and transfer syntaxes for DICOM 2001 edition.
 *
 * Revision 1.13  2001/06/01 15:48:55  meichel
 * Updated copyright header
 *
 * Revision 1.12  2001/01/17 10:20:36  meichel
 * Added toolkit support for JPEG-LS transfer syntaxes
 *
 * Revision 1.11  2000/04/14 16:01:23  meichel
 * Minor changes for thread safety.
 *
 * Revision 1.10  2000/03/08 16:26:28  meichel
 * Updated copyright header.
 *
 * Revision 1.9  1999/03/31 09:25:13  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */

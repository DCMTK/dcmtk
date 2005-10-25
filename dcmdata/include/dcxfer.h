/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Purpose: Handling of transfer syntaxes
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-10-25 08:55:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcxfer.h,v $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCXFER_H
#define DCXFER_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcvr.h"

/*
** Enumerated Types
*/

typedef enum {
    EXS_Unknown = -1,
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
    EXS_JPEGProcess14SV1TransferSyntax = 21,
    EXS_RLELossless = 22,
    EXS_JPEGLSLossless = 23,
    EXS_JPEGLSLossy = 24,
    EXS_DeflatedLittleEndianExplicit = 25,
    EXS_JPEG2000LosslessOnly = 26,
    EXS_JPEG2000 = 27,
    EXS_MPEG2MainProfileAtMainLevel = 28,
    EXS_JPEG2000MulticomponentLosslessOnly = 29,
    EXS_JPEG2000Multicomponent = 30
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

/*
** Class DcmXfer for lookup up Transfer Syntax properties and readable descriptions
*/

class DcmXfer {
    const char          *xferID;
    const char          *xferName;
    E_TransferSyntax    xferSyn;
    E_ByteOrder         byteOrder;
    E_VRType            vrType;
    E_JPEGEncapsulated  encapsulated;
    Uint32              JPEGProcess8;
    Uint32              JPEGProcess12;
    E_StreamCompression streamCompression;

public:
    DcmXfer( E_TransferSyntax xfer );
    DcmXfer( const char *xferName_xferID );
    DcmXfer( const DcmXfer &newXfer );
    ~DcmXfer();

    DcmXfer & operator = ( const E_TransferSyntax xfer );
    DcmXfer & operator = ( const DcmXfer &newtag );

    inline E_TransferSyntax getXfer() const  { return xferSyn; }
    inline E_ByteOrder getByteOrder() const { return byteOrder; }
    inline const char* getXferName() const { return xferName; }
    inline const char* getXferID() const { return xferID; }
    inline OFBool isLittleEndian() const
    {
        return byteOrder == EBO_LittleEndian;
    }
    inline OFBool isBigEndian() const { return byteOrder == EBO_BigEndian; }
    inline OFBool isImplicitVR() const { return vrType == EVT_Implicit; }
    inline OFBool isExplicitVR() const { return vrType == EVT_Explicit; }
    inline OFBool isEncapsulated() const
    {
        return encapsulated == EJE_Encapsulated;
    }
    inline OFBool isNotEncapsulated() const
    {
        return encapsulated == EJE_NotEncapsulated;
    }
    inline Uint32 getJPEGProcess8Bit() const { return JPEGProcess8; }
    inline Uint32 getJPEGProcess12Bit() const { return JPEGProcess12;}
    inline E_StreamCompression getStreamCompression() const { return streamCompression;}

    /* return the number of bytes needed to describe the tag, length, VR
    ** and any reserved fields for this transfer syntax when encoding the
    ** specified VR.
    */
    Uint32 sizeofTagHeader(DcmEVR evr);

};


extern const E_ByteOrder gLocalByteOrder;

#endif // DCXFER_H

/*
 * CVS/RCS Log:
 * $Log: dcxfer.h,v $
 * Revision 1.17  2005-10-25 08:55:32  meichel
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

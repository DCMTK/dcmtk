/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of abstract class DcmCodec and the class DcmCodecStruct
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:48:34 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dccodec.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCCODEC_H
#define DCCODEC_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcerror.h"
#include "dcxfer.h"

class DcmStack;
class DcmRepresentationParameter;
class DcmPixelSequence;
class DcmPolymorphOBOW;

class DcmCodecParameter
{
};                    

class DcmCodec
{
public:
    DcmCodec() {}

    /** decompresses the given pixel sequence and
     *  stores the result in the given uncompressedPixelData element.
     *  @param fromRepParam current representation parameter of compressed data, may be NULL
     *  @param pixSeq compressed pixel sequence
     *  @param uncompressedPixelData uncompressed pixel data stored in this element
     *  @param cp codec parameters for this codec
     *  @param objStack stack pointing to the location of the pixel data
     *    element in the current dataset.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    virtual E_Condition decode(
        const DcmRepresentationParameter * fromRepParam,
        DcmPixelSequence * pixSeq,
        DcmPolymorphOBOW& uncompressedPixelData,
        const DcmCodecParameter * cp,
        const DcmStack& objStack) const = 0;

    // compress pixelData to pixSeq 
    virtual E_Condition encode(
        const Uint16 * pixelData,
        const Uint32 length,
        const DcmRepresentationParameter * toRepParam,
        DcmPixelSequence * & pixSeq,
        const DcmCodecParameter *cp,
        DcmStack & objStack) const = 0;

    // change compression type
    virtual E_Condition encode(
        const E_TransferSyntax fromRepType,
        const DcmRepresentationParameter * fromRepParam,
        DcmPixelSequence * fromPixSeq,
        const DcmRepresentationParameter * toRepParam,
        DcmPixelSequence * & toPixSeq,
        const DcmCodecParameter * cp,
        DcmStack & objStack) const = 0;

    // check if a change of coding pixel data is possible
    virtual OFBool canChangeCoding(
        const E_TransferSyntax oldRepType,
        const E_TransferSyntax newRepType) const = 0;
};


struct DcmCodecStruct
{
private:
    E_TransferSyntax repType;
    const DcmCodec * codec;
    const DcmRepresentationParameter * defaultRepParam;
    const DcmCodecParameter * codecParameter;

 // --- declarations to avoid compiler warnings
 
    DcmCodecStruct(const DcmCodecStruct &);
    DcmCodecStruct &operator=(const DcmCodecStruct &);

public:
    DcmCodecStruct(
        const E_TransferSyntax rt,
        DcmCodec * c, 
        DcmRepresentationParameter *drp,
        DcmCodecParameter * cp)
        : repType(rt), codec(c), 
          defaultRepParam(drp), codecParameter(cp) {}

    E_TransferSyntax getRepresentationType() const { return repType; }
    const DcmCodec * getCodec() const { return codec; }
    const DcmRepresentationParameter * 
      getDefaultRepresentationParameter() const { return defaultRepParam; }
    const DcmCodecParameter * 
      getCodecParameter() const { return codecParameter; }
    void setCodecParameter(DcmCodecParameter *cp) { codecParameter = cp; }

};

/* the following functions should NOT be called before main() is started,
 * e. g. from constructors of global objects.
 */
void registerGlobalCodec(const DcmCodecStruct * codecStruct);
const DcmCodecStruct * searchGlobalCodec(const E_TransferSyntax repType);

#endif

/*
** CVS/RCS Log:
** $Log: dccodec.h,v $
** Revision 1.9  2001-06-01 15:48:34  meichel
** Updated copyright header
**
** Revision 1.8  2001/05/25 09:53:51  meichel
** Modified DcmCodec::decode() interface, required for future dcmjpeg module.
**
** Revision 1.7  2000/09/27 08:19:54  meichel
** Minor changes in DcmCodec interface, required for future dcmjpeg module.
**
** Revision 1.6  2000/04/14 16:09:12  meichel
** Made function DcmCodec and related functions thread safe.
**   registerGlobalCodec() should not be called anymore from the constructor
**   of global objects.
**
** Revision 1.5  2000/03/08 16:26:11  meichel
** Updated copyright header.
**
** Revision 1.4  1999/03/31 09:24:31  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.3  1998/07/15 15:48:43  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.2  1997/07/24 13:07:45  andreas
** - Make DcmCodec:canChangeCoding abstract
**
** Revision 1.1  1997/07/21 07:54:57  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** 
*/

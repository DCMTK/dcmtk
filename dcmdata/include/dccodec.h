/*
**
** Author: Andreas Barth
** Kuratorium OFFIS e.V.
**
** Module: dccodec.h
**
** Purpose:
** Interface of abstract class DcmCodec and the class DcmCodecStruct
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-24 13:07:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dccodec.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
**
** CVS/RCS Log at end of file
**
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

class DcmCodecParameter {};		       

class DcmCodec
{
public:
    DcmCodec() {}

    // decompress pixSeq in an uncompressed transfer syntax
    virtual E_Condition decode(
	const DcmRepresentationParameter * fromRepParam,
	const DcmPixelSequence * pixSeq,
	Uint16 * & pixelData,
	Uint32 & length,
	const DcmCodecParameter * cp,
	DcmStack & objStack) = 0;

    // compress pixelData to pixSeq 
    virtual E_Condition encode(
	const Uint16 * pixelData,
	const Uint32 length,
	const DcmRepresentationParameter * toRepParam,
	DcmPixelSequence * & pixSeq,
	const DcmCodecParameter *cp,
	DcmStack & objStack) = 0;

    // change compression type
    virtual E_Condition encode(
	const E_TransferSyntax fromRepType,
	const DcmRepresentationParameter * fromRepParam,
	const DcmPixelSequence * fromPixSeq,
	const DcmRepresentationParameter * toRepParam,
	DcmPixelSequence * & toPixSeq,
	const DcmCodecParameter * cp,
	DcmStack & objStack) = 0;


    // check if a change of coding pixel data is possible
    virtual OFBool canChangeCoding(
	const E_TransferSyntax oldRepType,
	const E_TransferSyntax newRepType) = 0;
};


struct DcmCodecStruct
{
private:
    E_TransferSyntax repType;
    DcmCodec * codec;
    DcmRepresentationParameter * defaultRepParam;
    DcmCodecParameter * codecParameter;
public:
    DcmCodecStruct(
	const E_TransferSyntax rt,
	DcmCodec * c, 
	DcmRepresentationParameter *drp,
	DcmCodecParameter * cp)
	: repType(rt), codec(c), 
	  defaultRepParam(drp), codecParameter(cp) {}

    E_TransferSyntax getRepresentationType() const { return repType; }
    DcmCodec * getCodec() const { return codec; }
    const DcmRepresentationParameter * 
      getDefaultRepresentationParameter() const { return defaultRepParam; }
    const DcmCodecParameter * 
      getCodecParameter() const { return codecParameter; }
};

void registerGlobalCodec(DcmCodecStruct * codecStruct);
const DcmCodecStruct * searchGlobalCodec(const E_TransferSyntax repType);

#endif

/*
** CVS/RCS Log:
** $Log: dccodec.h,v $
** Revision 1.2  1997-07-24 13:07:45  andreas
** - Make DcmCodec:canChangeCoding abstract
**
** Revision 1.1  1997/07/21 07:54:57  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
**
** 
*/

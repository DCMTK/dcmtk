/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Purpose: Interface of class DcmPixelData
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-10 10:50:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcpixel.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCPIXEL_H
#define DCPIXEL_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrpobw.h"
#include "oflist.h"

class DcmCodec;
class DcmCodecList;
class DcmStack;
class DcmPixelSequence;
class DcmPixelData;
class DcmRepresentationEntry;

class DcmRepresentationParameter 
{
public:
    DcmRepresentationParameter() {}
    virtual ~DcmRepresentationParameter() {}
    
    // this methods creates a copy of type DcmRepresentationParameter *
    // it must be overweritten in every subclass.
    virtual DcmRepresentationParameter * copy() const
    { 
        return new DcmRepresentationParameter(*this); 
    }

    virtual OFBool operator==(const DcmRepresentationParameter & /*x*/) const
    {
        return OFTrue;
    }
};

typedef OFList<DcmRepresentationEntry *> DcmRepresentationList;
typedef OFListIterator(DcmRepresentationEntry *) DcmRepresentationListIterator;

// The class DcmPixelData stores different pixel representations identified by 
// a type (the transfer syntax) and some representation parameters
// The three unencapsulated transfer syntaxes belong to the same pixel
// representation. 
// A type (or transfer syntax) conforms to a representation if 
// the type and the representation type are equal or both are unencapsulated.
// If this is valid for the representation read or set by chooseRepresentation
// then this representation is the conforming representation.
// else a representation with the default parameter set defined in the 
// codec is the conforming representation.

class DcmPixelData : public DcmPolymorphOBOW
{
private:
    friend class DcmRepresentationEntry;

    // List of representations of pixel data
    DcmRepresentationList repList;
   
    // Iterator to the last dummy element in representation lis
    DcmRepresentationListIterator repListEnd;

    // Iterator to the original representation. if an uncompressed
    // representation is used the iterator points to repList.end()
    DcmRepresentationListIterator original;
    
    // current list element for some operations
    DcmRepresentationListIterator current;

    // shows if an unecapsulated representation is stored
    OFBool existUnencapsulated;

    // value representation of unencapsulated data
    DcmEVR unencapsulatedVR;

    // in write function: pointer to current pixel sequence
    DcmPixelSequence * pixelSeqForWrite;

    // erases all representation entries but leaveInList from
    // the representation List. The entries self are deleted, too.
    void clearRepresentationList(
        DcmRepresentationListIterator leaveInList);

    // find a conforming representation in the list of
    // encapsulated representations
    E_Condition findConformingEncapsulatedRepresentation(
        const DcmXfer & repType,
        const DcmRepresentationParameter * repParam,
        DcmRepresentationListIterator & result);


    // find a representation entry and return an iterator to the found entry
    // or the next element in the list. The condition returned can be EC_Normal
    // if such an entry is found or EC_RepresentationNotFound. The pixSeq
    // attribute in findEntry can be NULL, it is not needed for the find
    // operation!
    E_Condition findRepresentationEntry(
        const DcmRepresentationEntry & findEntry,
        DcmRepresentationListIterator & result);

    // insert or replace a representation entry in the list
    DcmRepresentationListIterator insertRepresentationEntry(
        DcmRepresentationEntry * repEntry);

    // decode representation to unencapsulated format
    E_Condition decode(
        const DcmXfer & fromType,
        const DcmRepresentationParameter * fromParam,
        const DcmPixelSequence * fromPixSeq,
        DcmStack & pixelStack);


    // encode to encapsulated format
    E_Condition encode(
        const DcmXfer & fromType, 
        const DcmRepresentationParameter * fromParam, 
        const DcmPixelSequence * fromPixSeq, 
        const DcmXfer & toType, 
        const DcmRepresentationParameter *toParam,
        DcmStack & pixelStack);

    void recalcVR()
    {
        if (current == repList.end()) Tag.setVR(unencapsulatedVR);
        else Tag.setVR(EVR_OB);
    }
 
public:
    DcmPixelData(const DcmTag & tag, const Uint32 len = 0);
    DcmPixelData(const DcmPixelData & pixelData);
    virtual ~DcmPixelData();

    DcmPixelData &operator=(const DcmPixelData &obj);

    virtual E_Condition setVR(DcmEVR vr);
    virtual DcmEVR ident() const { return EVR_PixelData; }

    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
                       const int level = 0, const char *pixelFileName = NULL,
		               size_t *pixelCounter = NULL);

    // tests if it is possible to write a specific representation
    // Only existing representations are considered, since this
    // method does not create a representation.
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    // returns length of representation conforming to the
    // transfer syntax with tag, vr, ... It does not create a 
    // representation. If no conforming representation exists an 
    // error code is set and 0 returned.
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);
    
    // returns length of representation value field conforming to
    // given transfer syntax. It does not create a representation.
    // If no conforming representation exists, an error code is set
    // and 0 returned.
    virtual Uint32 getLength(
        const E_TransferSyntax xfer  = EXS_LittleEndianImplicit,
        const E_EncodingType enctype = EET_UndefinedLength);

    // Initialize a streaming operation (read, write)
    virtual void transferInit();

    // reads a representation and sets the current and orignal 
    // representation to the new representation
    // it deletes all old representations before reading!
    virtual E_Condition read(DcmStream & inStream, 
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange, 
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    // writes a representation conforming to the transfer syntax
    // It does not create a representation. If no conforming 
    // representation exists,  an error code is returned.
    // The written representation is the new current representation 
    virtual E_Condition write(
        DcmStream & outStream,
        const E_TransferSyntax oxfer,
        const E_EncodingType enctype = EET_UndefinedLength);

    virtual E_Condition loadAllDataIntoMemory(void);

    // Finalize a streaming operation (read, write)
    virtual void transferEnd();

    // test if it is possible to choose the representation in the parameters
    OFBool canChooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    // choose a specific representation. if representation does not exist
    // it is created (if possible).
    // if repParam is zero, a representation is chosen or created that
    // is equal to the default representation parameters
    E_Condition chooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmStack & stack);

    // Inserts an original encapsulated representation. current and original
    // representations are changed, all old representations are deleted
    void putOriginalRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * pixSeq);

    // The following two put-methods insert an original unencapsulated 
    // representation. current and original representations are changed, 
    // all old representations are deleted
    virtual E_Condition putUint16Array(
        const Uint16 * wordValue,
        const unsigned long length);

    virtual E_Condition putUint8Array(
        const Uint8 * byteValue,
        const unsigned long length);


    // get a specific exisiting Representation, creates no representation
    // if repParam is NULL, then the representation conforming to the default
    // presentationParameters (defined with the codec) is returned.
    E_Condition getEncapsulatedRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * & pixSeq);

    // returns the representation identification (key) consisting of
    // representation type and parameter of the original representation
    void getOriginalRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    // returns the representation identification (key) consisting of
    // representation type and parameter of the current representation
    void getCurrentRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    // sets the representation identification parameter of the current
    // representation
    E_Condition setCurrentRepresentationParameter(
        const DcmRepresentationParameter * repParam);
    
    // returns if a specific conforming  representation exists. 
    OFBool hasRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam = NULL);
    
    // delete a representation. It is not possible to delete the
    // original representation with this method
    E_Condition removeRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    // removes all but the original representation
    void removeAllButOriginalRepresentations();

    // removes all but the current representation 
    // Makes the current representation original
    void removeAllButCurrentRepresentations();

    // delete original representation and set new original representation.
    // If the new representation does not exist, the original one is not
    // deleted and an error code returns
    E_Condition removeOriginalRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);
};

#endif

/*
** CVS/RCS Log:
** $Log: dcpixel.h,v $
** Revision 1.7  2000-02-10 10:50:52  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.6  2000/02/03 16:28:10  joergr
** Fixed bug: encapsulated data (pixel items) have never been loaded using
** method 'loadAllDataIntoMemory'. Therefore, encapsulated pixel data was
** never printed with 'dcmdump'.
**
** Revision 1.5  1999/03/31 09:24:44  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1998/11/12 16:47:42  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1998/07/15 15:48:50  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.2  1997/07/24 13:08:24  andreas
** - Removed const for method DcmRepresentationParameter::copy
**
** Revision 1.1  1997/07/21 07:54:57  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** 
*/





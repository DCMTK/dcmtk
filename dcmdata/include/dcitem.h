/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Purpose: Interface of class DcmItem
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-04-14 15:31:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcitem.h,v $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCITEM_H
#define DCITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dctypes.h"
#include "dcobject.h"
#include "dcvrui.h"
#include "dclist.h"
#include "dcstack.h"

class DcmItem : public DcmObject 
{
private:

 // --- declarations to avoid compiler warnings
 
    DcmItem &operator=(const DcmItem &);

protected:
    DcmList * elementList;
    OFBool lastElementComplete;
    Uint32 fStartPosition;

    DcmObject*  copyDcmObject(DcmObject *oldObj);

    E_Condition readTagAndLength(DcmStream & inStream,            // inout
                                 const E_TransferSyntax newxfer, // in
                                 DcmTag   &tag,                  // out
                                 Uint32 & length,         // out
                                 Uint32 & bytesRead);     // out

    E_Condition readSubElement(DcmStream & inStream,            // inout
                               DcmTag &newTag,                  // inout
                               const Uint32 newLength,          // in
                               const E_TransferSyntax xfer,     // in
                               const E_GrpLenEncoding glenc,    // in
                               const Uint32 maxReadLength = DCM_MaxReadLength); 

    E_Condition searchSubFromHere(const DcmTagKey &tag,         // in
                                  DcmStack &resultStack,     // inout
                                  OFBool searchIntoSub );      // in
    DcmObject * iterObject(const DcmObject * obj,
                           const E_ListPos pos);
    OFBool foundVR(char* atposition );
    E_TransferSyntax checkTransferSyntax(DcmStream & inStream);

public:
    DcmItem(); // create with an item tag
    DcmItem(const DcmTag &tag,
            const Uint32 len = 0);
    DcmItem( const DcmItem& old );
    virtual ~DcmItem();

    virtual DcmEVR ident(void) const;
    virtual OFBool isLeaf(void) const { return OFFalse; }
    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
                       const int level = 0, const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);
    virtual unsigned long getVM();
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual Uint32 getLength(const E_TransferSyntax xfer 
                             = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype 
                             = EET_UndefinedLength );

    virtual void transferInit();
    virtual void transferEnd();

    virtual OFBool canWriteXfer(const E_TransferSyntax oldXfer,
                              const E_TransferSyntax newXfer);

    virtual E_Condition read(DcmStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength 
                             = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype 
                              = EET_UndefinedLength);


    virtual unsigned long card();
    virtual E_Condition insert(DcmElement* elem,
                               OFBool replaceOld = OFFalse);
    virtual DcmElement* getElement(const unsigned long num);

    // get next Object from position in stack. If stack empty
    // get next Object in this item. if intoSub true, scan
    // complete hierarchy, false scan only elements direct in this
    // item (not deeper). 
    virtual E_Condition nextObject(DcmStack & stack, const OFBool intoSub);
    virtual DcmObject * nextInContainer(const DcmObject * obj);
    virtual DcmElement* remove(const unsigned long num);
    virtual DcmElement* remove(DcmObject* elem);
    virtual DcmElement* remove(const DcmTagKey & tag);
    virtual E_Condition clear();
    virtual E_Condition verify(const OFBool autocorrect = OFFalse );
    virtual E_Condition search(const DcmTagKey& xtag,          // in
                               DcmStack &resultStack,          // inout
                               E_SearchMode mode = ESM_fromHere,  // in
                               OFBool searchIntoSub = OFTrue );       // in
    virtual E_Condition searchErrors( DcmStack &resultStack );         // inout
    virtual E_Condition loadAllDataIntoMemory(void);

    virtual E_Condition computeGroupLengthAndPadding
                         (const E_GrpLenEncoding glenc,
                          const E_PaddingEncoding padenc = EPD_noChange,
                          const E_TransferSyntax xfer = EXS_Unknown,
                          const E_EncodingType enctype = EET_ExplicitLength,
                          const Uint32 padlen = 0,
                          const Uint32 subPadlen = 0,
                          Uint32 instanceLength = 0);

    /* simple tests for existance */
    OFBool tagExists(const DcmTagKey& key, OFBool searchIntoSub = OFFalse);
    OFBool tagExistsWithValue(const DcmTagKey& key, OFBool searchIntoSub = OFFalse);

    /* simplified search&get functions */
    E_Condition findString(
        const DcmTagKey& xtag,
        char* aString, size_t maxStringLength,
        OFBool searchIntoSub = OFFalse);

    E_Condition findOFStringArray(
        const DcmTagKey& xtag,
        OFString & aString, 
        OFBool normalize = OFTrue, OFBool searchIntoSub = OFFalse);

    E_Condition findOFString(
        const DcmTagKey& xtag,
        OFString & aString, const unsigned long which,
        OFBool normalize = OFTrue, OFBool searchIntoSub = OFFalse);

    /**
     * Search and get as an integer number (long).
     * Returns error if tag cannot be found or unsuitable VR.
     * Only handles integer-like VR (UL, up, SL, US, OW, xs, ox, SS, OB)
     */
    E_Condition findIntegerNumber(
        const DcmTagKey& xtag,
        long& aLong, const unsigned long which,
        OFBool searchIntoSub = OFFalse);

    /**
     * Search and get as a real number (double).
     * Returns error if tag cannot be found or unsuitable VR.
     * Only handles real-like VR (FL, FD).
     */
    E_Condition findRealNumber(
        const DcmTagKey& xtag,
        double& aDouble, const unsigned long which,
        OFBool searchIntoSub = OFFalse);

};

//
// SUPPORT FUNCTIONS
//



// Function: newDicomElement
// creates a new DicomElement from a Tag.
// 
// Input:
//   tag    : Tag of the new element
//   length : length of the element value
//
// Output:
//   newElement: point of a heap allocated new element. If the tag does not
//               describe a dicom element or has ambigious VR (e.g. EVR_ox)
//               a NULL pointer is returned.
//
// Result:
//   EC_Normal:     tag describes an element (possibly with ambiguous VR)
//   EC_InvalidTag: tag describes an item begin or an unknown element
//   EC_SequEnd:    tag describes a sequence delimitation element
//   EC_ItemEnd:    tag describes an item delmitation element
//   other: an error
E_Condition newDicomElement(DcmElement * & newElement,
                            const DcmTag & tag,
                            const Uint32 length = 0);



// Functions: newDicomElement
// creates a new DicomElement from a Tag. They differ from the above functions 
// in not returning a condition. 
DcmElement * newDicomElement(const DcmTag & tag,
                             const Uint32 length = 0);

// Function: nextUp
// pop Object from stack and get next Object in top of stack

E_Condition nextUp(DcmStack & stack);


#endif // DCITEM_H

/*
** CVS/RCS Log:
** $Log: dcitem.h,v $
** Revision 1.24  2000-04-14 15:31:32  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.23  2000/03/08 16:26:15  meichel
** Updated copyright header.
**
** Revision 1.22  2000/03/03 14:05:24  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.21  2000/02/10 10:50:51  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.20  1999/03/31 09:24:40  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.19  1998/07/15 15:48:48  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.18  1997/09/22 14:50:43  hewett
** - Added 2 simple methods to test for the existance of an attribute
**   to DcmItem class (tagExists and tagExistsWithValue).  This code
**   was part of dcmgpdir.cc but is more generally useful.
** - Added 2 methods to find an attribute and retrieve numeric values
**   to DcmItem class (findIntegerNumber and findRealNumber).  The old
**   method findLong is now marked as obsolete and reimplemented using
**   findIntegerNumber.
**
** Revision 1.17  1997/09/11 15:13:11  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.16  1997/08/29 08:32:39  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.15  1997/07/21 08:25:08  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.14  1997/07/07 07:42:03  andreas
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
**
** Revision 1.13  1997/05/30 06:45:42  andreas
** - fixed problem of inconsistent interfaces and implementation that the
**   syntax check of GNU C++ does not find.
**
** Revision 1.12  1997/05/27 13:48:28  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.11  1997/05/16 08:13:43  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
** - Deleted obsolete method DcmItem::calcHeaderLength because the
**   samce functionality is performed by DcmXfer::sizeofTagHeader
**
** Revision 1.10  1996/08/05 08:45:23  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.9  1996/07/17 12:38:58  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.8  1996/04/29 15:08:53  hewett
** Replaced DcmItem::findInt(...) with the more general DcmItem::findLong(...).
**
** Revision 1.7  1996/04/16 16:00:05  andreas
** - added const for Tag in newDicomElement
**
** Revision 1.6  1996/03/28 18:52:30  hewett
** Added 2 simple find&get methods (findString & findInt).
**
** Revision 1.5  1996/01/29 13:38:12  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:15  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:56  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/

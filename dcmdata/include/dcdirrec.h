/*
 *
 *  Copyright (C) 1994-2004, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmDirectoryRecord
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2004-02-13 14:10:57 $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDIRREC_H
#define DCDIRREC_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcitem.h"
#include "dcsequen.h"
#include "dcfilefo.h"


typedef enum {
    ERT_root = 0,
    ERT_Curve = 1,
    ERT_FilmBox = 2,
    ERT_FilmSession = 3,
    ERT_Image = 4,
    ERT_ImageBox = 5,
    ERT_Interpretation = 6,
    ERT_ModalityLut = 7,
    ERT_Mrdr = 8,
    ERT_Overlay = 9,
    ERT_Patient = 10,
    ERT_PrintQueue = 11,
    ERT_Private = 12,
    ERT_Results = 13,
    ERT_Series = 14,
    ERT_Study = 15,
    ERT_StudyComponent = 16,
    ERT_Topic = 17,
    ERT_Visit = 18,
    ERT_VoiLut = 19,
    ERT_StructReport = 20,
    ERT_Presentation = 21,
    ERT_Waveform = 22,
    ERT_RTDose = 23,
    ERT_RTStructureSet = 24,
    ERT_RTPlan = 25,
    ERT_RTTreatRecord = 26,
    ERT_StoredPrint = 27,
    ERT_KeyObjectDoc = 28,
    ERT_Registration = 29,
    ERT_Fiducial = 30
} E_DirRecType;


class DcmDicomDir;


class DcmDirectoryRecord : public DcmItem
{

    friend class DcmDicomDir;

private:
    char *recordsOriginFile;

 // --- declarations to avoid compiler warnings

    DcmDirectoryRecord &operator=(const DcmDirectoryRecord &);

protected:
    DcmSequenceOfItems *lowerLevelList;
    E_DirRecType DirRecordType;
    DcmDirectoryRecord *referencedMRDR;
    Uint32 numberOfReferences;
    Uint32 offsetInFile;

    // side-effect-free conversion routines:
    E_DirRecType        recordNameToType(const char *recordTypeName);
    char*               buildFileName(const char *origName, char *destName);
    OFCondition         checkHierarchy(const E_DirRecType upperRecord,
                                       const E_DirRecType lowerRecord);

    // access to data elements within the Directory Records:
    OFCondition         setRecordType(E_DirRecType newType);
    E_DirRecType        lookForRecordType();
    OFCondition         setReferencedFileID( const char *referencedFileID);
    const char*         lookForReferencedFileID();
    DcmDirectoryRecord* lookForReferencedMRDR();
    const char*         getReferencedFileName();      // local or in MRDR
    OFCondition         setRecordInUseFlag(const Uint16 newFlag);
    Uint16              lookForRecordInUseFlag();
    Uint32              getFileOffset();
    Uint32              setFileOffset(Uint32 position);


    // access to MRDR data element:
    OFCondition         setNumberOfReferences(Uint32 newRefNum);
    Uint32              lookForNumberOfReferences();
    Uint32              increaseRefNum();
    Uint32              decreaseRefNum();

    // misc:
    OFCondition         fillElementsAndReadSOP(const char *referencedFileID,
                                               const char *sourceFileName);
    OFCondition         masterInsertSub(DcmDirectoryRecord *dirRec,
                                        const unsigned long where = DCM_EndOfListIndex);
    OFCondition         purgeReferencedFile();

public:
    DcmDirectoryRecord();
    DcmDirectoryRecord(const DcmTag &tag,
                       const Uint32 len);
    DcmDirectoryRecord(const E_DirRecType recordType,
                       const char *referencedFileID,   // DICOM format with '\\'
                       const char *sourceFileName);    // OS Format

    DcmDirectoryRecord(const char *recordTypeName,
                       const char *referencedFileID,   // DICOM Format with '\\'
                       const char *sourceFileName);    // OS Format

    DcmDirectoryRecord(const DcmDirectoryRecord &oldDirRec);
    virtual ~DcmDirectoryRecord();

    virtual DcmEVR ident() const;

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual OFCondition verify(const OFBool autocorrect = OFFalse);
    virtual OFCondition search(const DcmTagKey &xtag,               // in
                               DcmStack &resultStack,               // inout
                               E_SearchMode mode = ESM_fromHere,    // in
                               OFBool searchIntoSub = OFTrue);      // in
    virtual OFCondition searchErrors(DcmStack &resultStack);        // inout

    virtual E_DirRecType getRecordType();
    virtual DcmDirectoryRecord* getReferencedMRDR();
    virtual OFCondition assignToMRDR(DcmDirectoryRecord *mrdr );    // in
    virtual OFCondition assignToSOPFile(const char *referencedFileID,
                                        const char *sourceFileName);

    // manipulation of the lower-level Directory Entities:
    virtual unsigned long cardSub();

    virtual OFCondition insertSub(DcmDirectoryRecord* dirRec,
                                  unsigned long where = DCM_EndOfListIndex,
                                  OFBool before = OFFalse);

    /** insert new directory child record at the current position.
     *  The current position is stored internally in the 'lowerLevelList' member variable.
     *  @param dirRec new child record to be inserted
     *  @param before flag indicating whether to insert the record before (OFFalse) or
     *    after (OFTrue) the current position
     *  @return status, EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition insertSubAtCurrentPos(DcmDirectoryRecord *dirRec,
                                              OFBool before = OFFalse);

    virtual DcmDirectoryRecord* getSub(const unsigned long num);

    /** get next directory child record starting at a given record
     *  @param dirRec record to start from (goto first record if NULL)
     *  @return pointer to next record if successful, NULL otherwise
     */
    virtual DcmDirectoryRecord* nextSub(const DcmDirectoryRecord *dirRec);

    virtual DcmDirectoryRecord* removeSub(const unsigned long num);
    virtual DcmDirectoryRecord* removeSub(DcmDirectoryRecord *dirRec);
    virtual OFCondition deleteSubAndPurgeFile(const unsigned long num);
    virtual OFCondition deleteSubAndPurgeFile(DcmDirectoryRecord *dirRec);
    virtual OFCondition clearSub();

    // note the records origin file
    virtual void setRecordsOriginFile(const char *fname);
    virtual const char* getRecordsOriginFile();

};


#endif // DCDIRREC_H


/*
** CVS/RCS Log:
** $Log: dcdirrec.h,v $
** Revision 1.25  2004-02-13 14:10:57  joergr
** Added support for new directory records REGISTRATION and FIDUCIAL introduced
** with supplement 73 (Spatial Registration Storage SOP Classes).
**
** Revision 1.24  2003/08/08 14:14:04  joergr
** Translated remaining German comments.
**
** Revision 1.23  2003/08/08 14:04:16  joergr
** Added two new methods insertSubAtCurrentPos() and nextSub() which allow for
** a much more efficient insertion (avoids re-searching for correct position).
**
** Revision 1.22  2002/12/06 12:49:09  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.21  2002/08/27 16:55:31  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.20  2001/09/25 17:19:25  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.19  2001/06/20 14:59:15  joergr
** Added support for new SOP class Key Object Selection Document (suppl. 59).
**
** Revision 1.18  2001/06/01 15:48:39  meichel
** Updated copyright header
**
** Revision 1.17  2000/12/14 12:47:20  joergr
** Updated for 2000 edition of the DICOM standard (added: SR, PR, WV, SP, RT).
**
** Revision 1.16  2000/04/14 16:02:23  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.15  2000/03/08 16:26:14  meichel
** Updated copyright header.
**
** Revision 1.14  2000/03/03 14:05:23  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.13  2000/02/10 10:50:50  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.12  1999/03/31 09:24:36  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1999/03/22 14:10:57  meichel
** Added support for Structured Reports to dcmgpdir.
**   Added preliminary support for including sequences into a DICOMDIR.
**
** Revision 1.10  1998/07/15 15:48:46  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.9  1997/07/21 08:00:45  andreas
** - Corrected error in DcmDirectoryRecord::write since this routine can
**   change the length of the record after this is calculated in the
**   sequence.
** - DcmDirectoryRecord can be build with a referenced Name and a source
**   filename. These name now can differ (lower case - upper case
**   characters).
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/07/07 07:42:04  andreas
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
**
** Revision 1.7  1997/05/16 08:31:21  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.6  1997/05/09 13:12:02  hewett
** Added a member variable and associated get/set methods to DcmDirectoryRecord
** to allow the origin of a directory record to be noted.
**
** Revision 1.5  1997/05/06 09:22:35  hewett
** Added a "before" flag to the insertion of items for compatibility with
** insertion in normal Sequences.
**
** Revision 1.4  1997/03/26 16:56:25  hewett
** Removed the DcmDirectoryRecord member function fillTypeElements since it
** did nothing useful.  Smoothed out some poor indentation.
**
** Revision 1.3  1996/08/05 08:45:20  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.2  1996/01/05 13:22:54  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/

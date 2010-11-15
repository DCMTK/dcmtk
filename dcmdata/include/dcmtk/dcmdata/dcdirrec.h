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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmDirectoryRecord
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-15 13:58:43 $
 *  CVS/RCS Revision: $Revision: 1.47 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDIRREC_H
#define DCDIRREC_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcfilefo.h"


/// types of directory records in a DICOMDIR
typedef enum {
    /// root
    ERT_root = 0,
    /// curve (retired)
    ERT_Curve = 1,
    /// film box (retired)
    ERT_FilmBox = 2,
    /// film session (retired)
    ERT_FilmSession = 3,
    /// image
    ERT_Image = 4,
    /// image box (retired)
    ERT_ImageBox = 5,
    /// interpretation (retired)
    ERT_Interpretation = 6,
    /// modality LUT (retired)
    ERT_ModalityLut = 7,
    /// MRDR (retired)
    ERT_Mrdr = 8,
    /// overlay (retired)
    ERT_Overlay = 9,
    /// patient
    ERT_Patient = 10,
    /// print queue (retired)
    ERT_PrintQueue = 11,
    /// private
    ERT_Private = 12,
    /// results
    ERT_Results = 13,
    /// series
    ERT_Series = 14,
    /// study
    ERT_Study = 15,
    /// study component (retired)
    ERT_StudyComponent = 16,
    /// topic (retired)
    ERT_Topic = 17,
    /// visit (retired)
    ERT_Visit = 18,
    /// VOI LUT (retired)
    ERT_VoiLut = 19,
    /// SR document
    ERT_SRDocument = 20,
    /// presentation state
    ERT_Presentation = 21,
    /// waveform
    ERT_Waveform = 22,
    /// RT dose
    ERT_RTDose = 23,
    /// RT structure set
    ERT_RTStructureSet = 24,
    /// RT plan
    ERT_RTPlan = 25,
    /// RT treatment record
    ERT_RTTreatRecord = 26,
    /// stored print (retired)
    ERT_StoredPrint = 27,
    /// key object selection document
    ERT_KeyObjectDoc = 28,
    /// registration
    ERT_Registration = 29,
    /// fiducial
    ERT_Fiducial = 30,
    /// raw data
    ERT_RawData = 31,
    /// spectroscopy
    ERT_Spectroscopy = 32,
    /// encapsulated document
    ERT_EncapDoc = 33,
    /// value map
    ERT_ValueMap = 34,
    /// hanging protocol
    ERT_HangingProtocol = 35,
    /// stereometric relationships
    ERT_Stereometric = 36,
    /// HL7 structured document
    ERT_HL7StrucDoc = 37,
    /// palette
    ERT_Palette = 38,
    /// surface
    ERT_Surface = 39,
    /// measurement
    ERT_Measurement = 40,
    /// implant
    ERT_Implant = 41,
    /// implant group
    ERT_ImplantGroup = 42,
    /// implant assembly
    ERT_ImplantAssy = 43
} E_DirRecType;


class DcmDicomDir;

/** a class representing a directory record dataset in a DICOMDIR.
 */
class DcmDirectoryRecord : public DcmItem
{

    friend class DcmDicomDir;

public:
    /// default constructor
    DcmDirectoryRecord();

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmDirectoryRecord(const DcmTag &tag,
                       const Uint32 len);

    /** constructor
     *  @param recordType record type
     *  @param referencedFileID referenced file ID in DICOM format
     *  @param sourceFileName path to referenced file in operating system specific format
     */
    DcmDirectoryRecord(const E_DirRecType recordType,
                       const char *referencedFileID,   // DICOM format with '\\'
                       const char *sourceFileName);    // OS Format

    /** constructor
     *  @param recordTypeName record type as string
     *  @param referencedFileID referenced file ID in DICOM format
     *  @param sourceFileName path to referenced file in operating system specific format
     */
    DcmDirectoryRecord(const char *recordTypeName,
                       const char *referencedFileID,   // DICOM Format with '\\'
                       const char *sourceFileName);    // OS Format

    /** copy constructor
     *  @param oldDirRec element to be copied
     */
    DcmDirectoryRecord(const DcmDirectoryRecord &oldDirRec);

    /** assignment operator
     *  @param obj the directory record to be copied
     */
    DcmDirectoryRecord &operator=(const DcmDirectoryRecord &obj);

    /// destructor
    virtual ~DcmDirectoryRecord();

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmDirectoryRecord(*this);
    }

    /** Virtual object copying. This method can be used for DcmObject
     *  and derived classes to get a deep copy of an object. Internally
     *  the assignment operator is called if the given DcmObject parameter
     *  is of the same type as "this" object instance. If not, an error
     *  is returned. This function permits copying an object by value
     *  in a virtual way which therefore is different to just calling the
     *  assignment operator of DcmElement which could result in slicing
     *  the object.
     *  @param rhs - [in] The instance to copy from. Has to be of the same
     *                class type as "this" object
     *  @return EC_Normal if copying was successful, error otherwise
     */
    virtual OFCondition copyFrom(const DcmObject& rhs);

    /** return identifier for this class. Every class derived from this class
     *  returns a unique value of type enum DcmEVR for this call. This is used
     *  as a "poor man's RTTI" to correctly identify instances derived from
     *  this class even on compilers not supporting RTTI.
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const;

    /// returns current status flag
    inline OFCondition error() const { return errorFlag; }

    /** print all elements of the item to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);


    /** This function reads the information of all attributes which
     *  are captured in the input stream and captures this information
     *  in elementList. Each attribute is represented as an element
     *  in this list. If not all information for an attribute could be
     *  read from the stream, the function returns EC_StreamNotifyClient.
     *  @param inStream      The stream which contains the information.
     *  @param xfer         The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies
     *                       what will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);


    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);

    /** a complex, stack-based, hierarchical search method. It allows for a search
     *  for a DICOM object with a given attribute within a given container,
     *  hierarchically, from a starting position identified through a cursor stack.
     *  @param xtag the DICOM attribute tag we are searching for
     *  @param resultStack Depending on the search mode (see below), this parameter
     *     either serves as an input and output parameter, or as an output parameter
     *     only (the latter being the default). When used as an input parameter,
     *     the cursor stack defines the start position for the search within a
     *     hierarchical DICOM dataset. Upon successful return, the stack contains
     *     the position of the element found, in the form of a pointer to each dataset,
     *     sequence, item and element from the main dataset down to the found element.
     *  @param mode search mode, controls how the search stack is handled.
     *     In the default mode, ESM_fromHere, the stack is ignored on input, and
     *     the search starts in the object for which this method is called.
     *     In the other modes, the stack is used both as an input and an output
     *     parameter and defines the starting point for the search.
     *  @param searchIntoSub if true, the search will be performed hierarchically descending
     *    into the sequences and items of the dataset. If false, only the current container
     *    (sequence or item) will be traversed.
     *  @return EC_Normal if found, EC_TagNotFound if not found, an error code is something went wrong.
     */
    virtual OFCondition search(const DcmTagKey &xtag,               // in
                               DcmStack &resultStack,               // inout
                               E_SearchMode mode = ESM_fromHere,    // in
                               OFBool searchIntoSub = OFTrue);      // in

    /// get record type of this directory record
    virtual E_DirRecType getRecordType();

    /** if this directory record references an MRDR (multi-reference directory record),
     *  return pointer to the MRDR referenced by this object.
     *  @return pointer to MRDR referenced by this object or NULL of no MRDR referenced
     */
    virtual DcmDirectoryRecord* getReferencedMRDR();

    /** create a reference from this record to an MRDR
     *  @param mrdr pointer to MRDR
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition assignToMRDR(DcmDirectoryRecord *mrdr );    // in

    /** open a DICOM file and make this directory record into adirectory
     *  record for that DICOM file. The most relevant record keys
     *  (SOP Class UID, SOP instance UID, Transfer Syntax UID) are inserted
     *  into the directory record.
     *  @param referencedFileID referenced file ID in DICOM format
     *  @param sourceFileName path to file in operating system specific format
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition assignToSOPFile(const char *referencedFileID,
                                        const char *sourceFileName);

    /// return number of directory records that are child record of this one
    virtual unsigned long cardSub();

    /** insert a child directory record
     *  @param dirRec directory record to be inserted. Must be allocated on heap, ownership is transferred to this object
     *  @param where index where to insert object
     *  @param before flag indicating whether to insert the record before or after the element identified by where
     *  @return EC_Normal upon success, an error code otherwise
     */
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

    /** access child directory record. Returns a pointer to the object maintained
     *  as a child, not a copy.
     *  @param num index, must be < cardSub()
     *  @return pointer to child directory record or NULL if not found
     */
    virtual DcmDirectoryRecord* getSub(const unsigned long num);

    /** get next directory child record starting at a given record
     *  @param dirRec record to start from (goto first record if NULL)
     *  @return pointer to next record if successful, NULL otherwise
     */
    virtual DcmDirectoryRecord* nextSub(const DcmDirectoryRecord *dirRec);

    /** remove child directory record. If found, the record is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmDirectoryRecord object.
     *  @param num index number of element, must be < cardSub()
     *  @return pointer to DcmDirectoryRecord if found, NULL otherwise
     */
    virtual DcmDirectoryRecord* removeSub(const unsigned long num);

    /** remove child directory record. If found, the record is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmDirectoryRecord object.
     *  @param dirRec pointer to element to be removed from list
     *  @return pointer to element if found, NULL otherwise
     */
    virtual DcmDirectoryRecord* removeSub(DcmDirectoryRecord *dirRec);

    /** remove child directory record and delete file referenced by that record, if any
     *  @param num index number of element, must be < cardSub()
     *  @return status, EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition deleteSubAndPurgeFile(const unsigned long num);

    /** remove child directory record and delete file referenced by that record, if any
     *  @param dirRec pointer to element to be removed from list
     *  @return status, EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition deleteSubAndPurgeFile(DcmDirectoryRecord *dirRec);

    /// revert the list of child directory records to default constructed (empty) state
    virtual OFCondition clearSub();

    /** store the filename from which this directory record was read from
     *  @param fname filename, must not be NULL
     */
    virtual void setRecordsOriginFile(const char *fname);

    /// get the filename from which this directory record was read from, NULL íf not set
    virtual const char* getRecordsOriginFile();

    /// get the offset in file of this directory record
    Uint32 getFileOffset() const;

protected:

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

private:

    /// string in which the filename (path) of the file from which this directory record was read is kept
    char *recordsOriginFile;

    /// list of child directory records, kept in a sequence of items
    DcmSequenceOfItems *lowerLevelList;

    /// directory record type of this record
    E_DirRecType DirRecordType;

    /// pointer to multi-referenced directory record (MRDR) if this record refers to one, NULL otherwise
    DcmDirectoryRecord *referencedMRDR;

    /// number of other directory records referring to this one; used for MRDR records
    Uint32 numberOfReferences;

    /// byte offset at which the start of this directory record resides in the file from which it was read
    Uint32 offsetInFile;

};


#endif // DCDIRREC_H


/*
** CVS/RCS Log:
** $Log: dcdirrec.h,v $
** Revision 1.47  2010-11-15 13:58:43  uli
** Fixed some errors in doxygen comments.
**
** Revision 1.46  2010-11-05 13:11:11  joergr
** Added support for new directory record types IMPLANT, IMPLANT GROUP and
** IMPLANT ASSY from Supplement 131 (Implant Templates).
**
** Revision 1.45  2010-10-14 13:15:40  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.44  2010-10-01 08:06:16  joergr
** Added new directory record type MEASUREMENT from Supplement 144.
**
** Revision 1.43  2010-09-30 16:43:18  joergr
** Added new directory record types HL7 STRUC DOC, PALETTE and SURFACE.
**
** Revision 1.42  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.41  2010-02-22 11:39:53  uli
** Remove some unneeded includes.
**
** Revision 1.40  2008-07-17 11:19:48  onken
** Updated copyFrom() documentation.
**
** Revision 1.39  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.38  2008-06-03 13:41:40  meichel
** DcmDirectoryRecord::getFileOffset() is now const and public.
**
** Revision 1.37  2007/11/29 14:30:35  meichel
** Updated doxygen API documentation
**
** Revision 1.36  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.35  2007/02/19 15:04:34  meichel
** Removed searchErrors() methods that are not used anywhere and added
**   error() methods only in the DcmObject subclasses where really used.
**
** Revision 1.34  2006/08/16 09:41:26  joergr
** Added missing CVS log entry.
**
** Revision 1.33  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.32  2006/07/27 13:05:05  joergr
** Added support for DICOMDIR record type "STEREOMETRIC" (CP 628).
** Renamed ERT_StructReport to ERT_SRDocument.
**
** Revision 1.31  2006/05/11 17:48:53  joergr
** Fixed wrong CVS log entry.
**
** Revision 1.30  2006/05/11 08:53:36  joergr
** Added "offset" attribute to DICOMDIR record items.
**
** Revision 1.29  2005/12/08 16:28:10  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.28  2005/10/27 13:30:20  joergr
** Added support for Encapsulated Document, Real World Value Mapping and
** Hanging Protocol objects to DICOMDIR tools.
**
** Revision 1.27  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.26  2004/02/13 17:36:46  joergr
** Added support for new directory records RAW DATA and SPECTROSCOPY introduced
** with CP 343.
**
** Revision 1.25  2004/02/13 14:10:57  joergr
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

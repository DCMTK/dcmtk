/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
    /// HL7 structured document (retired)
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
    ERT_ImplantAssy = 43,
    /// plan
    ERT_Plan = 44,
    /// surface scan
    ERT_SurfaceScan = 45,
    /// tractography
    ERT_Tract = 46,
    /// assessment
    ERT_Assessment = 47,
    /// radiotherapy
    ERT_Radiotherapy = 48
} E_DirRecType;


class DcmDicomDir;

/** a class representing a directory record dataset in a DICOMDIR.
 */
class DCMTK_DCMDATA_EXPORT DcmDirectoryRecord : public DcmItem
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
     *  @param fileFormat fileFormat for sourceFileName, can be NULL
     */
    DcmDirectoryRecord(const E_DirRecType recordType,
                       const char *referencedFileID,     // DICOM format with '\\'
                       const OFFilename &sourceFileName, // OS format
                       DcmFileFormat* fileFormat = NULL);

    /** constructor
     *  @param recordTypeName record type as string
     *  @param referencedFileID referenced file ID in DICOM format
     *  @param sourceFileName path to referenced file in operating system specific format
     *  @param fileFormat fileFormat for sourceFileName, can be NULL
     */
    DcmDirectoryRecord(const char *recordTypeName,
                       const char *referencedFileID,     // DICOM format with '\\'
                       const OFFilename &sourceFileName, // OS format
                       DcmFileFormat* fileFormat = NULL);

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

    /** mode specifying whether the SpecificCharacterSet (0008,0005) element should be
     *  checked by convertCharacterSet() or not, i.e.\ whether this element might be
     *  present on this dataset-level.
     *  @return always returns OFTrue, i.e.\ SpecificCharacterSet should be checked
     */
    virtual OFBool checkForSpecificCharacterSet() const { return OFTrue; }

    /** convert all element values that are contained in this record and that are
     *  affected by SpecificCharacterSet from the given source character set to the given
     *  destination character set. The defined terms for a particular character set can
     *  be found in the DICOM standard, e.g. "ISO_IR 100" for ISO 8859-1 (Latin 1) or
     *  "ISO_IR 192" for Unicode in UTF-8. An empty string denotes the default character
     *  repertoire, which is ASCII (7-bit). If multiple values are given for 'fromCharset'
     *  (separated by a backslash) code extension techniques are used and escape sequences
     *  may be encountered in the source string to switch between the specified character
     *  sets.
     *  @param fromCharset name of the source character set(s) used for the conversion
     *  @param toCharset name of the destination character set used for the conversion.
     *    Only a single value is permitted (i.e. no code extensions).
     *  @param flags optional flag used to customize the conversion (see DCMTypes::CF_xxx)
     *  @param updateCharset if OFTrue, the SpecificCharacterSet (0008,0005) element is
     *    updated, i.e.\ the current value is either replaced or a new element is inserted
     *    or the existing element is deleted. If OFFalse the SpecificCharacterSet element
     *    remains unchanged.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertCharacterSet(const OFString &fromCharset,
                                            const OFString &toCharset,
                                            const size_t flags = 0,
                                            const OFBool updateCharset = OFFalse);

    /** convert all element values that are contained in this record and that are
     *  affected by SpecificCharacterSet to the given destination character set. If not
     *  disabled, the source character set is determined automatically from the value of
     *  the SpecificCharacterSet (0008,0005) element. The defined terms for the
     *  destination character set can be found in the DICOM standard, e.g. "ISO_IR 100"
     *  for ISO 8859-1 (Latin 1) or "ISO_IR 192" for Unicode in UTF-8. An empty string
     *  denotes the default character repertoire, which is ASCII (7-bit).
     *  @param toCharset name of the destination character set used for the conversion.
     *    Only a single value is permitted (i.e. no code extensions).
     *  @param flags optional flag used to customize the conversion (see DCMTypes::CF_xxx)
     *  @param ignoreCharset if OFTrue, the value of SpecificCharacterSet is ignored.
     *    Also see checkForSpecificCharacterSet().
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertCharacterSet(const OFString &toCharset,
                                            const size_t flags = 0,
                                            const OFBool ignoreCharset = OFFalse);

    /** convert all element values that are contained in this record and that are
     *  affected by SpecificCharacterSet from the currently selected source character
     *  set to the currently selected destination character set. Since the Basic
     *  Directory IOD, which specifies the structure and content of a DICOMDIR, does not
     *  contain the SpecificCharacterSet (0008,0005) element in the main dataset but in
     *  each directory record, this method also checks for this element and creates a new
     *  character set converter for the contained data elements (if needed).
     *  @param converter character set converter to be used to convert the element values
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertCharacterSet(DcmSpecificCharacterSet &converter);

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
     *  @param xfer          The transfer syntax which was used to encode
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
     *  @param resultStack depending on the search mode (see below), this parameter
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

    /** open a DICOM file and make this directory record into a directory
     *  record for that DICOM file. The most relevant record keys
     *  (SOP Class UID, SOP instance UID, Transfer Syntax UID) are inserted
     *  into the directory record.
     *  @param referencedFileID referenced file ID in DICOM format
     *  @param sourceFileName path to file in operating system specific format
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition assignToSOPFile(const char *referencedFileID,
                                        const OFFilename &sourceFileName);

    /// return number of directory records that are child record of this one
    virtual unsigned long cardSub() const;

    /** insert a child directory record
     *  @param dirRec directory record to be inserted. Must be allocated on heap, ownership is
     *    transferred to this object
     *  @param where index where to insert object
     *  @param before flag indicating whether to insert the record before or after the element
     *    identified by where
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
     *  @param fname filename, must not be empty
     */
    virtual void setRecordsOriginFile(const OFFilename &fname);

    /// get the filename from which this directory record was read from, empty if not set
    virtual const OFFilename &getRecordsOriginFile();

    /// get the offset in file of this directory record
    Uint32 getFileOffset() const;

protected:

    // side-effect-free conversion routines:
    E_DirRecType        recordNameToType(const char *recordTypeName);
    char*               buildFileName(const char *origName, char *destName, size_t len) const;
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
    /** Load all necessary info for this directory record.
     *  @param referencedFileID file ID that is being referenced, may be NULL
     *  @param sourceFileName filename for the DICOM file, may be empty (unspecified)
     *  @param fileFormat If not NULL, then this should be the result of loading
     *         sourceFileName. May only be non-NULL if sourceFileName isn't empty.
     */
    OFCondition         fillElementsAndReadSOP(const char *referencedFileID,
                                               const OFFilename &sourceFileName,
                                               DcmFileFormat *fileFormat = NULL);
    OFCondition         masterInsertSub(DcmDirectoryRecord *dirRec,
                                        const unsigned long where = DCM_EndOfListIndex);
    OFCondition         purgeReferencedFile();

private:

    /// filename (path) of the file from which this directory record was read
    OFFilename recordsOriginFile;

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

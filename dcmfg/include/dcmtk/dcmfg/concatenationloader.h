/*
 *
 *  Copyright (C) 2019-2020, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for loading Concatenations
 *
 */

#ifndef CONCATENATIONLOADER_H
#define CONCATENATIONLOADER_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmfg/fgdefine.h"
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofmap.h"

class DcmDataset;

/** Class for scanning files for concatenations and if desired, load
 *  one of them. If successful, loading produces the merged SOP Instance
 *  as a single dataset plus its frames in a separate data structure. The following
 *  workflow must be used:
 *  <ul>
 *  <li>Call to one of the scan() methods.</li>
 *  <li>If scanning was successful, call getInfo() to get information about the
 *  concatenations found by scan(). getFailedFiles() will return those files
 *  who could not be read, do not provide Concatenation information or had other errors.</li>
 *  <li>Call to the load() method in order to load of the the Concatenations found
 *  during scan(). Selection is done via the Concatenations Concatenation UID.
 *  The result is made available as a single, merged dataset without Pixel Data attribute
 *  and a vector containing all frames of the merged instance.</li>
 * </ul>
 */
class DCMTK_DCMFG_EXPORT ConcatenationLoader
{

public:
    /// Structure representing information about a Concatenation,
    /// consisting of its instances and common data.
    struct DCMTK_DCMFG_EXPORT Info
    {
        /// Struct representing single SOP Instance being part of a Concatenation.
        struct DCMTK_DCMFG_EXPORT Instance
        {
            /// Filename of concatenation instance
            OFFilename m_Filename;
            /// SOP Instance UID of concatenation instance
            OFString m_sopInstanceUID;
            /// Number of Frames extracted from concatenation instance
            Uint32 m_NumberOfFrames;
            /// In-Concatenation Number extracted from concatenation instance
            Uint16 m_InConcatenationNumber;

            /** Virtual destructor, frees memory
             */
            virtual ~Instance();

            /** Default constructor
             */
            Instance();
        };

        /** Default constructor */
        Info();

        /** Copy constructor
         *  @param  rhs The Info to copy from
         */
        Info(const Info& rhs);

        /** Assignment operator
         *  @param  rhs The Info to copy from
         *  @return Reference to "this" object
         */
        Info& operator=(const Info& rhs);

        /// List of instances that belong to this Concatenation
        OFList<Instance> m_Files;

        /// File that contains the source of this Concatenation, i.e.
        /// the SOP Instance UID of Concatenation Source tag points to.
        /// Might be empty if such file is not found during scanning.
        OFFilename m_FileConatenationSource;

        /// Concatenation UID of this Concatenation, extracted from concatenation instances
        OFString m_ConcatenationUID;

        /// SOP Instance UID of Concatenation Source, extracted from concatenation instances
        OFString m_SourceUID;

        /// In-Concatenation Total Number, extracted from concatenation instances (optional in DICOM)
        Uint16 m_inConcatTotalNumber;

        /// Total number of frames, i.e. the number of frames that make up this concatenation instance.
        /// The number is computed by summing up the number of frames in instances found during scanning.
        size_t m_NumTotalFrames;

        /// Patient ID extracted from concatenation instances
        OFString m_PatientID;

        /// Study Instance UID extracted from concatenation instances
        OFString m_StudyInstanceUID;

        /// Series Instance UID extracted from concatenation instances
        OFString m_SeriesInstanceUID;

        /// SOP Class UID extracted from concatenation instances
        OFString m_SOPClassUID;

        /// Bits Allocated extracted from concatenation instances
        Uint16 m_BitsAlloc;

        /// Rows extracted from concatenation instances
        Uint16 m_Rows;

        /// Columns extracted from concatenation instances
        Uint16 m_Cols;

        /** Print overview of this Concatenation
         *  @param  out The stream to print to
         */
        void print(OFStringStream& out);

        /** Virtual destructor
         */
        virtual ~Info();
    };

    /// structure representing error information about a failed file
    struct DCMTK_DCMFG_EXPORT Failure
    {
      /// filename
      OFFilename fname;

      /// error text
      OFString errorText;

      /// SOP Instance UID
      OFString sopInstance;

      /** constructor.
       *  @param fn filename
       *  @param err error text
       *  @param uid SOP instance UID
       */
      Failure(const OFFilename& fn, const OFString& err, const OFString& uid)
      : fname(fn), errorText(err), sopInstance(uid) {}
    };

    /// Result type of scan()
    typedef OFMap<OFString, ConcatenationLoader::Info*> TScanResult;

    /// Iterator type for iterating over scan() results
    typedef OFMap<OFString, ConcatenationLoader::Info*>::iterator ScanResultIt;

    /// Result type for the failures reported by scan()
    typedef OFList<Failure> TScanFailures;

    /// Result type for iterating the failures reported by scan()
    typedef OFListIterator(Failure) TScanFailureIt;

    /** Constructor
     */
    ConcatenationLoader();

    /** Virtual destructor
     */
    virtual ~ConcatenationLoader();

    /** Ignore missing SOP Instance UID of Concatenation Source
     *  @param  ignore If OFTrue,  missing or empty SOP Instance UID of Concatenation Source tag is
     *          ignored and operation continues.
     */
    virtual void setIgnoreMissingSourceUID(const OFBool ignore);

    /** Scan given list of files for Concatenations. If successful, a results
     *  can be accessed by calling getInfo() which contains an overview of the
     *  Concatenations found (and files that failed during reading).
     *  @param  files The files to scan
     *  @return EC_Normal if scanning was successful, i.e. at least one Concatenation
     *          could be discovered successfully.
     */
    virtual OFCondition scan(const OFList<OFFilename>& files);

    /** Scan given directory for Concatenations. If successful, a results
     *  can be accessed by calling getInfo() which contains an overview of the
     *  Concatenations found (and files that failed during reading).
     *  @param  directory The directory to scan
     *  @param  pattern The search pattern (* per default)
     *  @param  recursive If OFTrue, sub directories are scanned, too (default: OFTrue)
     *  @return EC_Normal if scanning was successful, i.e. at least one Concatenation
     *          could be discovered successfully.
     */
    virtual OFCondition scan(const OFFilename& directory, const OFFilename& pattern = "*", OFBool recursive = OFTrue);

    /** Get successful results of scan() operation. Returns empty result set is scan() has
     *  not been called before.
     *  @return The results of the scan() operation
     */
    virtual const TScanResult& getInfo();

    /** Get failed files of scan() operation. Returns empty result set is scan() has
     *  not been called before (may be empty otherwise).
     *  @return The failed files of the scan() operation
     */
    virtual const TScanFailures& getFailedFiles();

    /** Load Concatenation and merge it into "original" source dataset.
     *  The Concatenation to be loaded is selected by its Concatenation UID.
     *  Frames are not written to the dataset's Pixel Data attribute but instead
     *  reside in a frame structure provided by the caller (since they could exceed
     *  the uncompressed maximum of 4 GB allowed in a single dataset).
     *  @param  concatenationUID The Concatenation UID of the Concatenation
     *          to be loaded
     *  @param  dataset The result dataset. The caller must hand in a pointer to
     *          a valid dataset, i.e. memory allocation must be done by the caller.
     *          During loading, the dataset must not be freed outside this class.
     *          However, the caller stays responsible to delete the dataset after usage.
     *  @param  frames The resulting frames. Vector should provided empty by the user.
     *          The user is responsible for deleting frames after successful operation.
     *          If load() fails, this parameter should return an empty vector.
     *  @return EC_Normal if loading Concatenation worked, error otherwise.
     */
    virtual OFCondition
    load(const OFString& concatenationUID, DcmDataset* dataset, OFVector<DcmIODTypes::Frame*>& frames);

protected:
    /** Handles single file of a Concatenation and extracts structure for later
     *  access by the user.
     *  @param  file The file to process
     *  @param  info The concatenation information structure to store results to
     */
    virtual void handleFile(const OFFilename& file, ConcatenationLoader::Info& info);

    /** Checks an current file's information whether it fits to the rest of the
     *  Concatenation instances already read. If this is the case, the instance's
     *  information is added to the results.
     *  @param  info Information for this Concatenation
     *  @param  inst Instance information that should be added to the Concatenation
     *  @param  error Error, if instance cannot be processed/added, otherwise empty.
     */
    virtual void checkAndInsertInfo(const ConcatenationLoader::Info& info,
                                    const ConcatenationLoader::Info::Instance& inst,
                                    OFString& error);

    /** Check whether two numbers equal to 0 or empty.
     *  @param  num1 The first number
     *  @param  num2 The second number
     *  @return OFTrue if both number are equal or one of them is 0, OFFalse otherwise.
     */
    virtual OFBool zeroOrEqual(const size_t num1, const size_t num2);

    /** Check whether two strings are equal or one of them is empty.
     *  @param  str1 The first string
     *  @param  str2 The second string
     *  @return OFTrue if both strings are equal or one of them is empty, OFFalse otherwise.
     */
    virtual OFBool emptyOrEqual(const OFString& str1, const OFString& str2);

    /** Get that number from both which is not 0.
     *  @param  num1 The first number
     *  @param  num2 The second number
     *  @return num1 if num 1 is not zero, num 2 otherwise.
     */
    template <typename T>
    T getNotZero(const T num1, const T num2);

    /** Perform consistency checks on the Concatenations found.
     *  @return EC_Normal if successful, failure otherwise
     */
    virtual OFCondition doScanFinalChecks();

    /** Prepare template (dataset) that is used to represent the final
     *  result dataset.
     *  @param  firstInstance The first (or any instance) of the concatenation
     *          instances that will be used to create the result dataset.
     *  @return EC_Normal if successful, failure otherwise
     */
    virtual OFCondition prepareTemplate(Info& firstInstance);

    /** Extract frames from given item. Works for Bits Allocated = 8
     *  and Bits Allocated = 16. Resulting frames are stored
     *  in member variable.
     *  @param  item The item to read Pixel Data attribute from
     *  @param  info Concatenation information providing pixel meta data
     *          like Rows and Columns
     *  @param  numFrames The number of frames to extract from item's
     *          Pixel Data attribute
     *  @return EC_Normal if extraction was successful, error otherwise.
     */
    virtual OFCondition extractFrames(DcmItem& item, Info& info, Uint32 numFrames);

    /** Extract binary frames (Bits Allocated = 1) from given item.
     *  The resulting frames are stored in member variable.
     *  @param  item The item to read Pixel Data attribute from
     *  @param  info Concatenation information providing pixel meta data
     *          like Rows and Columns
     *  @param  numFrames The number of frames to extract from item's
     *          Pixel Data attribute
     *  @return EC_Normal if extraction was successful, error otherwise.
     */
    virtual OFCondition extractBinaryFrames(DcmItem& item, Info& info, Uint32 numFrames);

    /** Deletes those concatenation attributes from given item that are not
     *  used or get a different value in the resulting merged dataset.
     *  @param  item The item to delete from
     *  @return EC_Normal if successful, error otherwise
     */
    virtual OFCondition deleteConcatAttributes(DcmItem& item);

    /** Moves all Per-Frame Functional Group items found in given item
     *  to result instance.
     *  @param  item The item to delete from
     *  @return EC_Normal if deletion was successful, error otherwise
     */
    virtual OFCondition movePerFrameItems(DcmItem& item);

    /** Insert/adapt attributes that are required with the merged result
     *  instance.
     *  @return  EC_Normal if successful, failure otherwise
     */
    virtual OFCondition insertDestinationAttributes();

    virtual OFCondition
    computeBytesPerFrame(const Uint16 rows, const Uint16 cols, const Uint16 bitsAlloc, size_t& bytes_per_frame);

private:
    /// Map with entries consisting each of ConcatenationUID and related
    /// Concatenation information.
    TScanResult m_Concats;

    /// List with failed files, each entry consisting of filename, error text and
    /// SOP Instance UID (latter might be empty).
    TScanFailures m_FailedFiles;

    /// If OFTrue, the missing of attribute SOP Instance UID of Concatenation
    /// Source is ignored and will not lead to an error
    OFBool m_ignoreMissingSourceUID;

    /// The dataset that will contain the final merged SOP instance produced
    /// by the load() method. Once a merged instance is provided to the caller,
    /// as a result of load(), the caller is responsible for deleting the
    /// related memory.
    DcmDataset* m_Result;

    /// The frames that will contain all the frames of the merged SOP instance
    /// produced by the load() method. Once a merged instance is provided to
    /// the caller, as a result of load(), the caller is responsible for
    /// deleting the related memory.
    OFVector<DcmIODTypes::Frame*> m_Frames;
};

#endif // CONCATENATIONLOADER_H

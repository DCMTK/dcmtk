/*
 *
 *  Copyright (C) 1996-2018, OFFIS e.V.
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
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens, Jan Schlamelcher
 *
 *  Purpose: Class for managing file system interaction.
 *
 */

#ifndef WlmFileSystemInteractionManager_h
#define WlmFileSystemInteractionManager_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oftypes.h"   /* for OFBool */
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/dcmwlm/wldefine.h"

struct WlmSuperiorSequenceInfoType;
class DcmDataset;
class DcmTagKey;
class OFCondition;
class DcmItem;
class OFdirectory_iterator;

/** This class encapsulates data structures and operations for managing
 *  data base interaction in the framework of the DICOM basic worklist
 *  management service.
 */
class DCMTK_DCMWLM_EXPORT WlmFileSystemInteractionManager
{
  private:

      /** Matching keys configuration. */
    class MatchingKeys;

      /** Privately defined copy constructor.
       *  @param old Object which shall be copied.
       */
    WlmFileSystemInteractionManager(const WlmFileSystemInteractionManager &old);

      /** Privately defined assignment operator.
       *  @param obj Object which shall be copied.
       */
    WlmFileSystemInteractionManager &operator=(const WlmFileSystemInteractionManager &obj);

  protected:
    /// path to database files
    OFString dfPath;
    /// indicates if wl-files which are lacking return type 1 attributes or information in such attributes shall be rejected or not
    OFBool enableRejectionOfIncompleteWlFiles;
    /// called AE title
    OFString calledApplicationEntityTitle;
    /// matching records
    OFVector<OFshared_ptr<DcmDataset> > matchingRecords;

      /** Increment the given directory iterator until it refers to a worklist file (or past-the-end).
       *  @param it A reference to an OFdirectory_iterator.
       */
    OFdirectory_iterator& FindNextWorklistFile( OFdirectory_iterator& it );

      /** This function checks if the given dataset (which represents the information from a
       *  worklist file) contains all necessary return type 1 information. According to the
       *  DICOM standard part 4 annex K, the following attributes are type 1 attributes in
       *  C-Find RSP messages:
       *        Attribute                             Tag      Return Key Type
       *    SpecificCharacterSet                  (0008,0005)        1C (will be checked in WlmDataSourceFileSystem::StartFindRequest(...); this attribute does not have to be checked here)
       *    ScheduledProcedureStepSequence        (0040,0100)        1
       *     > ScheduledStationAETitle            (0040,0001)        1
       *     > ScheduledProcedureStepStartDate    (0040,0002)        1
       *     > ScheduledProcedureStepStartTime    (0040,0003)        1
       *     > Modality                           (0008,0060)        1
       *     > ScheduledProcedureStepDescription  (0040,0007)        1C (The ScheduledProcedureStepDescription (0040,0007) or the ScheduledProtocolCodeSequence (0040,0008) or both shall be supported by the SCP; we actually support both, so we have to check if at least one of the two attributes contains valid information.)
       *     > ScheduledProtocolCodeSequence      (0040,0008)        1C (see abobve)
       *     > > CodeValue                        (0008,0100)        1
       *     > > CodingSchemeDesignator           (0008,0102)        1
       *     > ScheduledProcedureStepID           (0040,0009)        1
       *    RequestedProcedureID                  (0040,1001)        1
       *    RequestedProcedureDescription         (0032,1060)        1C (The RequestedProcedureDescription (0032,1060) or the RequestedProcedureCodeSequence (0032,1064) or both shall be supported by the SCP; we actually support both, so we have to check if at least one of the two attributes contains valid information.)
       *    RequestedProcedureCodeSequence        (0032,1064)        1C (see abobve)
       *     > > CodeValue                        (0008,0100)        1
       *     > > CodingSchemeDesignator           (0008,0102)        1
       *    StudyInstanceUID                      (0020,000D)        1
       *    ReferencedStudySequence               (0008,1110)        2
       *     > ReferencedSOPClassUID              (0008,1150)        1C (Required if a sequence item is present)
       *     > ReferencedSOPInstanceUID           (0008,1155)        1C (Required if a sequence item is present)
       *    ReferencedPatientSequence             (0008,1120)        2
       *     > ReferencedSOPClassUID              (0008,1150)        1C (Required if a sequence item is present)
       *     > ReferencedSOPInstanceUID           (0008,1155)        1C (Required if a sequence item is present)
       *    PatientName                           (0010,0010)        1
       *    PatientID                             (0010,0020)        1
       *  @param dataset - [in] The dataset of the worklist file which is currently examined.
       *  @return OFTrue in case the given dataset contains all necessary return type 1 information,
       *          OFFalse otherwise.
       */
    OFBool DatasetIsComplete( DcmDataset *dataset );

      /** This function checks if the specified sequence attribute is absent or existent but non-empty
       *  and incomplete in the given dataset.
       *  @param sequenceTagKey The sequence attribute which shall be checked.
       *  @param dset The dataset in which the attribute is contained.
       *  @return OFTrue in case the sequence attribute is absent (and cannot be added to the dataset)
       *          or existent but non-empty and incomplete, OFFalse otherwise.
       */
    OFBool ReferencedStudyOrPatientSequenceIsAbsentOrExistentButNonEmptyAndIncomplete( DcmTagKey sequenceTagKey, DcmItem *dset );

      /** This method ensures that either code or description is set to a non-empty value,
       *  and at the same time none of the attributes is present with a zero-length value.
       *  If one of these requirements are not met, then OFTrue is returned, otherwise OFFalse.
       *  @param descriptionTagKey The description attribute which shall be checked.
       *  @param codeSequenceTagKey The codeSequence attribute which shall be checked.
       *  @param dset The dataset in which the attributes are contained.
       *  @return OFFalse (i.e. no error regarding the standard) in case at least
       *          one of both attributes has a non-empty, valid value, and none
       *          is set to an empty value. OFTrue otherwise.
       */
    OFBool DescriptionAndCodeSequenceAttributesAreIncomplete( DcmTagKey descriptionTagKey, DcmTagKey codeSequenceTagKey, DcmItem *dset );

      /** This function checks if the specified attribute is absent or contains an empty value in the given dataset.
       *  @param elemTagKey The attribute which shall be checked.
       *  @param dset The dataset in which the attribute is contained.
       *  @return OFTrue in case the attribute is absent or contains an empty value, OFFalse otherwise.
       */
    OFBool AttributeIsAbsentOrEmpty( DcmTagKey elemTagKey, DcmItem *dset );

      /** This function returns OFTrue, if the matching key attribute values in the one of the items of the candidate sequence
       *  match the matching key attribute values in at least one of the items of the query sequence.
       *  @param candidate  The candidate sequence.
       *  @param query The query sequence.
       *  @param matchingKeys The matching keys to regard.
       *  @return OFTrue in case at least one item matches, OFFalse otherwise.
       */
    OFBool MatchSequences( DcmSequenceOfItems& candidate, DcmSequenceOfItems& query, const MatchingKeys& matchingKeys );

      /** Determine if a sequence is an universal match.
       *  @param query The query sequence.
       *  @param matchingKeys The matching keys to regard.
       *  @param normalize Normalize each attribute value before the check. Defaults to OFTrue, which
       *    means the value will be normalized as appropriate for the given VR, e.g. ignoring spaces
       *    used as padding.
       *  @param normalizeWildCards Whether to interpret a query only consisting of wild cards as
       *    an universal match. Defaults to OFTrue, which means wild cards will be normalized if an
       *    attribute's VR supports it and it is allowed for the attribute (as defined by the matchingKeys
       *    argument). Set to OFFalse to force strict interpretation instead.
       *  @return OFTrue if the sequence has no items or if all attributes in its item can be considered
       *    an universal match. Returns OFFalse otherwise.
       */
    OFBool IsUniversalMatch( DcmSequenceOfItems& query,
                             const MatchingKeys& matchingKeys,
                             const OFBool normalize = OFTrue,
                             const OFBool normalizeWildCards = OFTrue );

      /** This function returns OFTrue, if the matching key attribute values in the
       *  dataset match the matching key attribute values in the search mask.
       *  @param dataset    The dataset which shall be checked.
       *  @param searchMask The search mask.
       *  @param matchingKeys The matching keys to regard.
       *  @return OFTrue in case the dataset matches the search mask in the matching key attribute values, OFFalse otherwise.
       */
    OFBool DatasetMatchesSearchMask( DcmItem& dataset, DcmItem& searchMask, const MatchingKeys& matchingKeys );

  public:
      /** default constructor.
       */
    WlmFileSystemInteractionManager();

      /** destructor
       */
    ~WlmFileSystemInteractionManager();

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetEnableRejectionOfIncompleteWlFiles( OFBool value );

      /** Connects to the worklist file system database.
       *  @param dfPathv Path to worklist file system database.
       *  @return Indicates if the connection could be established or not.
       */
    OFCondition ConnectToFileSystem( const OFString& dfPathv );

      /** Disconnects from the worklist file system database.
       *  @return Indicates if the connection was disconnected successfully.
       */
    OFCondition DisconnectFromFileSystem();

      /** Checks if the given called application entity title is supported. If this is the case,
       *  OFTrue will be returned, else OFFalse.
       *  @param calledApplicationEntityTitlev The application entity title which shall be checked
       *                                       for support. Valid pointer expected.
       *  @return OFTrue, if the called application entity title is supported,
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    OFBool IsCalledApplicationEntityTitleSupported( const OFString& calledApplicationEntityTitlev );

      /** This function determines the records from the Worklist files that match
       *  the given search mask and returns the number of matching records. Also,
       *  this function will store the matching records inside the member variable
       *  matchingRecords.
       *  @param searchMask A pointer to the search mask.
       *  @return The number of matching records.
       */
    size_t DetermineMatchingRecords( DcmDataset* searchMask );

      /** Determine whether a Worklist file matches the search mask.
       *  @param searchMask A reference to the search mask.
       *  @param  worklistFile An OFpath (hopefully) referring to a Worklist
       *    file.
       *  @details
       *  This method will attempt to load the Worklist file referenced by the
       *  argument worklistFile and, on success, compare it against searchMask.
       *  If the file matches the search mask, its dataset part will be added
       *  to the matching records member variable.
       */
    void MatchWorklistFile( DcmDataset& searchMask, const OFpath& worklistFile );

      /** For the matching record that is identified through idx, this function returns the number
       *  of items that are contained in the sequence element that is referred to by sequenceTag.
       *  In case this sequence element is itself contained in a certain item of another superior
       *  sequence, superiorSequenceArray contains information about where to find the correct
       *  sequence element.
       *  @param sequenceTag            The tag of the sequence element for which the number of items
       *                                shall be determined.
       *  @param superiorSequenceArray  Array which contains information about superior sequence elements
       *                                the given sequence element is contained in.
       *  @param numOfSuperiorSequences The number of elements in the above array.
       *  @param idx                    Identifies the record from which the number of sequence items
       *                                shall be determined.
       *  @return The number of items that are contained in the sequence element that is referred to by
       *          sequenceTag and that can be found in sequence items which are specified in superiorSequenceArray.
       */
    unsigned long GetNumberOfSequenceItemsForMatchingRecord( DcmTagKey sequenceTag, WlmSuperiorSequenceInfoType *superiorSequenceArray, unsigned long numOfSuperiorSequences, unsigned long idx );

      /** This function determines an attribute value of a matching record
       *  and returns this value in a newly created string to the caller.
       *  @param tag                    Attribute tag. Specifies which attribute's value shall be returned.
       *  @param superiorSequenceArray  Array which contains information about superior sequence elements
       *                                the given element is contained in.
       *  @param numOfSuperiorSequences The number of elements in the above array.
       *  @param idx                    Identifies the record from which the attribute value shall be retrieved.
       *  @param value                  Pointer to a newly created string that contains the requested value.
       *                                If value was not found an emtpy string will be returned.
       */
    void GetAttributeValueForMatchingRecord( DcmTagKey tag, WlmSuperiorSequenceInfoType *superiorSequenceArray, unsigned long numOfSuperiorSequences, unsigned long idx, char *&value );

      /** This function frees the memory which was occupied by matchingRecords.
       *  It shall be called when the matching records are no longer needed.
       */
    void ClearMatchingRecords();
};

#endif

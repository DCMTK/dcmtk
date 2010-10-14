/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Class for managing file system interaction.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:39 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmFileSystemInteractionManager_h
#define WlmFileSystemInteractionManager_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oftypes.h"   /* for OFBool */

template <class T> class OFOrderedSet;
struct WlmSuperiorSequenceInfoType;
class DcmDataset;
class DcmTagKey;
class OFConsole;
class OFCondition;
class DcmItem;

/** This class encapsulates data structures and operations for managing
 *  data base interaction in the framework of the DICOM basic worklist
 *  management service.
 */
class WlmFileSystemInteractionManager
{
  private:
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
    /// array of matching records
    DcmDataset **matchingRecords;
    /// number of array fields
    unsigned long numOfMatchingRecords;

      /** This function determines all worklist files in the directory specified by
       *  dfPath and calledApplicationEntityTitle, and returns the complete path and
       *  filename information in an array of strings.
       *  @param worklistFiles Set of strings, each specifying path and filename to one worklist file.
       */
    void DetermineWorklistFiles( OFOrderedSet<OFString> &worklistFiles );

      /** This function returns OFTrue if the given filename refers to a worklist file,
       *  i.e. has an extension of ".wl".
       *  @param fname The name of the file.
       *  @return OFTrue in case the given filename refers to a worklist file, OFFalse otherwise.
       */
    OFBool IsWorklistFile( const char *fname );

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

      /** This function checks if the specified description and code sequence attribute are both incomplete in the given dataset.
       *  @param descriptionTagKey The description attribute which shall be checked.
       *  @param codeSequenceTagKey The codeSequence attribute which shall be checked.
       *  @param dset The dataset in which the attributes are contained.
       *  @return OFTrue in case both attributes are incomplete, OFFalse otherwise.
       */
    OFBool DescriptionAndCodeSequenceAttributesAreIncomplete( DcmTagKey descriptionTagKey, DcmTagKey codeSequenceTagKey, DcmItem *dset );

      /** This function checks if the specified attribute is absent or contains an empty value in the given dataset.
       *  @param elemTagKey The attribute which shall be checked.
       *  @param dset The dataset in which the attribute is contained.
       *  @return OFTrue in case the attribute is absent or contains an empty value, OFFalse otherwise.
       */
    OFBool AttributeIsAbsentOrEmpty( DcmTagKey elemTagKey, DcmItem *dset );

      /** This function returns OFTrue, if the matching key attribute values in the
       *  dataset match the matching key attribute values in the search mask.
       *  @param dataset    The dataset which shall be checked.
       *  @param searchMask The search mask.
       *  @return OFTrue in case the dataset matches the search mask in the matching key attribute values, OFFalse otherwise.
       */
    OFBool DatasetMatchesSearchMask( DcmDataset *dataset, DcmDataset *searchMask );

      /** This function determines the values of the matching key attributes in the given dataset.
       *  @param dataset Dataset from which the values shall be extracted.
       *  @param matchingKeyAttrValues Contains in the end the values of the matching key
       *         attributes in the search mask. Is an array of pointers.
       */
    void DetermineMatchingKeyAttributeValues( DcmDataset *dataset, const char **&matchingKeyAttrValues );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute scheduled station AE title match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ScheduledStationAETitlesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attributes scheduled procedure step start date and scheduled procedure step
       *  start time match; otherwise OFFalse will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; might be NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; might be NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ScheduledProcedureStepStartDateTimesMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute modality match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ModalitiesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute scheduled performing physician's names match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ScheduledPerformingPhysicianNamesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute patient's names match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool PatientsNamesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute patient id match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool PatientsIDsMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute accession number match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool AccessionNumbersMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute requested procedure id match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool RequestedProcedureIdsMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute referring physician's name match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool ReferringPhysicianNamesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute patient sex match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool PatientsSexesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute requesting physician match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool RequestingPhysiciansMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute admission id match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool AdmissionIdsMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute requested procedure priorities match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool RequestedProcedurePrioritiesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values in
       *  attribute patient's birth date match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool PatientsBirthDatesMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function performs a date time range match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateTimeRangeMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );

      /** This function performs a date range match and returns OFTrue if the dataset's and
       *  the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateRangeMatch( const char *datasetDateValue, const char *searchMaskDateValue );

      /** This function performs a time range match and returns OFTrue if the dataset's and
       *  the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool TimeRangeMatch( const char *datasetTimeValue, const char *searchMaskTimeValue );

      /** This function performs a date time single value match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateTimeSingleValueMatch( const char *datasetDateValue, const char *datasetTimeValue, const char *searchMaskDateValue, const char *searchMaskTimeValue );

      /** This function performs a date single value match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetDateValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskDateValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool DateSingleValueMatch( const char *datasetDateValue, const char *searchMaskDateValue );

      /** This function performs a time single value match and returns OFTrue if the dataset's
       *  and the search mask's values in the corresponding attributes match; otherwise OFFalse
       *  will be returned.
       *  @param datasetTimeValue    Value for the corresponding attribute in the dataset; might be NULL.
       *  @param searchMaskTimeValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool TimeSingleValueMatch( const char *datasetTimeValue, const char *searchMaskTimeValue );

      /** This function returns OFTrue if the dataset's and the search mask's values
       *  match while performing a case sensitive single value match; otherwise OFFalse
       *  will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; never NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool CaseSensitiveSingleValueMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function returns OFTrue if the dataset's and the search mask's values
       *  match while performing a wildcard match; otherwise OFFalse will be returned.
       *  @param datasetValue    Value for the corresponding attribute in the dataset; never NULL.
       *  @param searchMaskValue Value for the corresponding attribute in the search mask; never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool WildcardMatch( const char *datasetValue, const char *searchMaskValue );

      /** This function is called, if the search pattern contains a star symbol. It determines
       *  if dv (the dataset's value) still matches sv (the search mask's value). This function
       *  takes the star symbol in sv into account. (Note that the pattern value might contain
       *  more wild card symbols.) The function will return OFTrue if there is a match; if there
       *  is not a match it will return OFFalse.
       *  @param dv Dataset's value; never NULL.
       *  @param sv Search mask's value (may contain wild card symbols); never NULL.
       *  @return OFTrue if the values match, OFFalse otherwise.
       */
    OFBool MatchStarSymbol( const char *dv, const char *sv );

      /** This function extracts the actual lower and upper date or time values from a given
       *  date or time range.
       *  @param range Date or time range from which lower and upper values shall be extracted.
       *  @param lower Newly created string specifying the lower value from the date/time range;
       *               NULL if value is not specified in range.
       *  @param upper Newly created string specifying the upper value from the date/time range;
       *               NULL if value is not specified in range.
       */
    void ExtractValuesFromRange( const char *range, char *&lower, char *&upper );

  public:
      /** default constructor.
       */
    WlmFileSystemInteractionManager();

      /** destructor
       */
    ~WlmFileSystemInteractionManager();

      /**  Set value in member variable.
       *  @param value The value to set.
       */
    void SetEnableRejectionOfIncompleteWlFiles( OFBool value );

      /** Connects to the worklist file system database.
       *  @param dfPathv Path to worklist file system database.
       *  @return Indicates if the connection could be established or not.
       */
    OFCondition ConnectToFileSystem( const OFString& dfPathv );

      /** Disconnects from the worklist file system database..
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

      /** This function determines the records from the worklist files which match
       *  the given search mask and returns the number of matching records. Also,
       *  this function will store the matching records in memory in the array
       *  member variable matchingRecords.
       *  @param searchMask - [in] The search mask.
       *  @return Number of matching records.
       */
    unsigned long DetermineMatchingRecords( DcmDataset *searchMask );

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

/*
** CVS Log
** $Log: wlfsim.h,v $
** Revision 1.18  2010-10-14 13:16:39  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.17  2010-08-09 13:29:39  joergr
** Updated data dictionary to 2009 edition of the DICOM standard. From now on,
** the official "keyword" is used for the attribute name which results in a
** number of minor changes (e.g. "PatientsName" is now called "PatientName").
**
** Revision 1.16  2009-11-24 10:40:01  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.15  2009-11-02 15:50:25  joergr
** Changed forward declaration from "class" to "struct" in order to avoid
** warning messages reported by VisualStudio 2008.
**
** Revision 1.14  2009-09-30 08:40:34  uli
** Make dcmwlm's include headers self-sufficient by including all
** needed headers directly.
**
** Revision 1.13  2006-12-15 14:49:21  onken
** Removed excessive use char* and C-array in favour of OFString and
** OFList. Simplified some implementation details.
**
** Revision 1.12  2006/01/27 15:06:32  joergr
** Fixed issue with missing type 2 attributes in worklist files being reported
** as incomplete.  Now, the attributes are inserted automatically if required.
** Removed email address from CVS log.
**
** Revision 1.11  2005/12/08 16:05:42  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.10  2005/09/23 12:56:40  wilkens
** Added attribute PatientsBirthDate as a matching key attribute to wlmscpfs.
**
** Revision 1.9  2005/06/16 08:06:51  meichel
** Removed redundant class name, needed for gcc 3.4
**
** Revision 1.8  2005/05/04 11:34:31  wilkens
** Added two command line options --enable-file-reject (default) and
** --disable-file-reject to wlmscpfs: these options can be used to enable or
** disable a file rejection mechanism which makes sure only complete worklist files
** will be used during the matching process. A worklist file is considered to be
** complete if it contains all necessary type 1 information which the SCP might
** have to return to an SCU in a C-Find response message.
**
** Revision 1.7  2004/01/07 08:32:28  wilkens
** Added new sequence type return key attributes to wlmscpfs. Fixed bug that for
** equally named attributes in sequences always the same value will be returned.
** Added functionality that also more than one item will be returned in sequence
** type return key attributes.
**
** Revision 1.6  2003/12/23 13:04:36  wilkens
** Integrated new matching key attributes into wlmscpfs.
**
** Revision 1.5  2003/07/02 09:17:55  wilkens
** Updated documentation to get rid of doxygen warnings.
**
** Revision 1.4  2002/12/16 11:08:35  wilkens
** Added missing #include "osconfig.h" to certain files.
**
** Revision 1.3  2002/12/09 13:41:43  joergr
** Renamed parameter to avoid name clash with global function index().
** Added private undefined copy constructor and/or assignment operator.
**
** Revision 1.2  2002/08/12 10:56:08  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.1  2002/08/05 09:09:58  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.1  2002/07/17 13:10:21  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
**
*/

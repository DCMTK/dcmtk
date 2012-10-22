/*
 *
 *  Copyright (C) 1996-2012, OFFIS e.V.
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
 *  Purpose: (Partially) abstract class for connecting to an arbitrary data source.
 *
 */

#ifndef WlmDataSource_h
#define WlmDataSource_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmwlm/wltypdef.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/oflog/oflog.h"

extern DCMTK_DCMWLM_EXPORT OFLogger DCM_dcmwlmLogger;

#define DCMWLM_TRACE(msg) OFLOG_TRACE(DCM_dcmwlmLogger, msg)
#define DCMWLM_DEBUG(msg) OFLOG_DEBUG(DCM_dcmwlmLogger, msg)
#define DCMWLM_INFO(msg)  OFLOG_INFO(DCM_dcmwlmLogger, msg)
#define DCMWLM_WARN(msg)  OFLOG_WARN(DCM_dcmwlmLogger, msg)
#define DCMWLM_ERROR(msg) OFLOG_ERROR(DCM_dcmwlmLogger, msg)
#define DCMWLM_FATAL(msg) OFLOG_FATAL(DCM_dcmwlmLogger, msg)

class DcmAttributeTag;
class DcmLongString;

/** This class encapsulates data structures and operations for connecting to an arbitrary
 *  data source in the framework of the DICOM basic worklist management service.
 */
class DCMTK_DCMWLM_EXPORT WlmDataSource
{
  protected:

    /// indicates if the application shall fail on an invalid C-Find RQ message
    OFBool failOnInvalidQuery;
    /// called AE title
    OFString calledApplicationEntityTitle;
    /// the search mask which is contained in the C-Find RQ message
    DcmDataset *identifiers;
    /// list of error elements
    DcmAttributeTag *errorElements;
    /// list of offending elements
    DcmAttributeTag *offendingElements;
    /// error comment
    DcmLongString *errorComment;
    /// indicates if we encountered an unsupported optional key attribute in the search mask
    OFBool foundUnsupportedOptionalKey;
    /// indicates if a read lock was set on the data source
    OFBool readLockSetOnDataSource;
    /// indicates if the expansion of empty sequence attributes shall take place or not
    OFBool noSequenceExpansion;
    /// returned character set type
    WlmReturnedCharacterSetType returnedCharacterSet;
    /// array of matching datasets
    OFList<DcmDataset*> matchingDatasets;
    /// potentially specified specific character set (in search mask)
    OFString specificCharacterSet;
    /// information about superior sequence elements; necessary for inserting values into resultDataset
    WlmSuperiorSequenceInfoType *superiorSequenceArray;
    /// number of elements in above array
    unsigned long numOfSuperiorSequences;

      /** This function checks if the search mask has a correct format. It returns OFTrue if this
       *  is the case, OFFalse if this is not the case.
       *  @param searchMask Contains the search mask.
       *  @return OFTrue, in case the search mask has a correct format, OFFalse otherwise.
       */
    OFBool CheckSearchMask( DcmDataset *searchMask );

      /** This function checks if a non-sequence element in the search mask has a correct format.
       *  Note that if the current element is an unsupported element, the entire element will be re-
       *  moved from the search mask, since unsupported elements shall not be returned to the caller.
       *  @param searchMask                       Pointer to the search mask.
       *  @param invalidMatchingKeyAttributeCount Counter that counts invalid elements in the search mask.
       *  @param element                          Pointer to the currently processed element.
       *  @param supSequenceElement               Pointer to the superordinate sequence element of which
       *                                          the currently processed element is an attribute.
       */
    void CheckNonSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );

      /** This function checks if a sequence element in the search mask has a correct format.
       *  Note that if the current element is an unsupported element, the entire element will be re-
       *  moved from the search mask, since unsupported elements shall not be returned to the caller.
       *  Moreover, in case the sequence element in the search mask is supported but empty, this
       *  function will expand the sequence element by inserting all required attributes into that sequence.
       *  @param searchMask                       Pointer to the search mask.
       *  @param invalidMatchingKeyAttributeCount Counter that counts invalid elements in the search mask.
       *  @param element                          Pointer to the currently processed element.
       *  @param supSequenceElement               Pointer to the superordinate sequence element of which
       *                                          the currently processed element is an attribute.
       */
    void CheckSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );

      /** According to the 2001 DICOM standard (part 4, section C.2.2.2.6), if a search mask
       *  contains a sequence attribute which contains no item or a single empty item, all
       *  attributes from that particular sequence are in fact queried and shall be returned
       *  by the SCP. This implementation accounts for this specification by inserting a
       *  corresponding single item with all required attributes into such emtpy sequences.
       *  This function performs the insertion of the required item and attributes.
       *  @param element Pointer to the currently processed element.
       */
    void ExpandEmptySequenceInSearchMask( DcmElement *&element );

      /** This function checks if the given element refers to an attribute which is a supported
       *  matching key attribute. If this is the case OFTrue is returned, else OFFalse.
       *  Currently, the following attributes are supported as matching keys:
       *    DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
       *     > DCM_ScheduledStationAETitle                        (0040,0001)  AE  R  1
       *     > DCM_ScheduledProcedureStepStartDate                (0040,0002)  DA  R  1
       *     > DCM_ScheduledProcedureStepStartTime                (0040,0003)  TM  R  1
       *     > DCM_Modality                                       (0008,0060)  CS  R  1
       *     > DCM_ScheduledPerformingPhysicianName               (0040,0006)  PN  R  2
       *    DCM_PatientName                                       (0010,0010)  PN  R  1
       *    DCM_PatientID                                         (0010,0020)  LO  R  1
       *    DCM_AccessionNumber                                   (0008,0050)  SH  O  2
       *    DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
       *    DCM_ReferringPhysicianName                            (0008,0090)  PN  O  2
       *    DCM_PatientSex                                        (0010,0040)  CS  O  2
       *    DCM_RequestingPhysician                               (0032,1032)  PN  O  2
       *    DCM_AdmissionID                                       (0038,0010)  LO  O  2
       *    DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
       *    DCM_PatientBirthDate                                  (0010,0030)  DA  O  2
       *  @param element            Pointer to the element which shall be checked.
       *  @param supSequenceElement Pointer to the superordinate sequence element of which
       *                            the currently processed element is an attribute, or NULL if
       *                            the currently processed element does not belong to any sequence.
       *  @return OFTrue, in case the given tag is a supported matching key attribute, OFFalse otherwise.
       */
    OFBool IsSupportedMatchingKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );

      /** This function checks if the given element refers to an attribute which is a supported
       *  return key attribute. If this is the case OFTrue is returned, else OFFalse.
       *  Currently, the following attributes are supported as return keys:
       *    DCM_SpecificCharacterSet                              (0008,0005)  CS  O  1
       *    DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
       *     > DCM_ScheduledStationAETitle                        (0040,0001)  AE  R  1
       *     > DCM_ScheduledProcedureStepStartDate                (0040,0002)  DA  R  1
       *     > DCM_ScheduledProcedureStepStartTime                (0040,0003)  TM  R  1
       *     > DCM_Modality                                       (0008,0060)  CS  R  1
       *     > DCM_ScheduledPerformingPhysicianName               (0040,0006)  PN  R  2
       *     > DCM_ScheduledProcedureStepDescription              (0040,0007)  LO  O  1
       *     > DCM_ScheduledStationName                           (0040,0010)  SH  O  2
       *     > DCM_ScheduledProcedureStepLocation                 (0040,0011)  SH  O  2
       *     > DCM_PreMedication                                  (0040,0012)  LO  O  2
       *     > DCM_ScheduledProcedureStepID                       (0040,0009)  SH  O  1
       *     > DCM_RequestedContrastAgent                         (0032,1070)  LO  O  2
       *     > DCM_CommentsOnTheScheduledProcedureStep            (0040,0400)  LT  O  3  (from the Scheduled Procedure Step Module)
       *     > DCM_ScheduledProcedureStepStatus                   (0040,0020)  CS  O  3
       *     > DCM_ScheduledProcedureStepEndDate                  (0040,0004)  DA  O  3  (from the Scheduled Procedure Step Module)
       *     > DCM_ScheduledProcedureStepEndTime                  (0040,0005)  TM  O  3  (from the Scheduled Procedure Step Module)
       *     > DCM_ScheduledProtocolCodeSequence                  (0040,0008)  SQ  O  1C
       *     > > DCM_CodeValue                                    (0008,0100)  SH  O  1C
       *     > > DCM_CodingSchemeVersion                          (0008,0103)  SH  O  3
       *     > > DCM_CodingSchemeDesignator                       (0008,0102)  SH  O  1C
       *     > > DCM_CodeMeaning                                  (0008,0104)  LO  O  3
       *    DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
       *    DCM_RequestedProcedureDescription                     (0032,1060)  LO  O  1
       *    DCM_StudyInstanceUID                                  (0020,000d)  UI  O  1
       *    DCM_StudyDate                                         (0008,0020)  DA  O  3
       *    DCM_StudyTime                                         (0008,0030)  TM  O  3
       *    DCM_ReferencedStudySequence                           (0008,1110)  SQ  O  2
       *     > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  1
       *     > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  1
       *    DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
       *    DCM_PatientTransportArrangements                      (0040,1004)  LO  O  2
       *    DCM_AccessionNumber                                   (0008,0050)  SH  O  2
       *    DCM_RequestingPhysician                               (0032,1032)  PN  O  2
       *    DCM_ReferringPhysicianName                            (0008,0090)  PN  O  2
       *    DCM_AdmissionID                                       (0038,0010)  LO  O  2
       *    DCM_CurrentPatientLocation                            (0038,0300)  LO  O  2
       *    DCM_ReferencedPatientSequence                         (0008,1120)  SQ  O  2
       *     > DCM_ReferencedSOPClassUID                          (0008,1150)  UI  O  2
       *     > DCM_ReferencedSOPInstanceUID                       (0008,1155)  UI  O  2
       *    DCM_PatientName                                       (0010,0010)  PN  R  1
       *    DCM_PatientID                                         (0010,0020)  LO  R  1
       *    DCM_IssuerOfPatientID                                 (0010,0021)  LO  O  3  (from the Patient Identification Module)
       *    DCM_PatientBirthDate                                  (0010,0030)  DA  O  2
       *    DCM_PatientSex                                        (0010,0040)  CS  O  2
       *    DCM_PatientWeight                                     (0010,1030)  DS  O  2
       *    DCM_ConfidentialityConstraintOnPatientDataDescription (0040,3001)  LO  O  2
       *    DCM_PatientState                                      (0038,0500)  LO  O  2
       *    DCM_PregnancyStatus                                   (0010,21c0)  US  O  2
       *    DCM_MedicalAlerts                                     (0010,2000)  LO  O  2
       *    DCM_ContrastAllergies                                 (0010,2110)  LO  O  2
       *    DCM_SpecialNeeds                                      (0038,0050)  LO  O  2
       *    DCM_NamesOfIntendedRecipientsOfResults                (0040,1010)  PN  O  3  (from the Requested Procedure Module)
       *    DCM_InstitutionName                                   (0008,0080)  LO  O  3  (from the Visit Identification Module)
       *    DCM_AdmittingDiagnosesDescription                     (0008,1080)  LO  O  3  (from the Visit Admission Module)
       *    DCM_OtherPatientIDs                                   (0010,1000)  LO  O  3  (from the Patient Identification Module)
       *    DCM_PatientSize                                       (0010,1020)  DS  O  3  (from the Patient Demographic Module)
       *    DCM_EthnicGroup                                       (0010,2160)  SH  O  3  (from the Patient Demographic Module)
       *    DCM_PatientComments                                   (0010,4000)  LT  O  3  (from the Patient Demographic Module)
       *    DCM_AdditionalPatientHistory                          (0010,21b0)  LT  O  3  (from the Patient Medical Module)
       *    DCM_LastMenstrualDate                                 (0010,21d0)  DA  O  3  (from the Patient Medical Module)
       *    DCM_InstitutionAddress                                (0008,0081)  ST  O  3  (from the Visit Identification Module)
       *    DCM_OtherPatientNames                                 (0010,1001)  PN  O  3  (from the Patient Identification Module)
       *    DCM_PatientAddress                                    (0010,1040)  LO  O  3  (from the Patient Demographic Module)
       *    DCM_MilitaryRank                                      (0010,1080)  LO  O  3  (from the Patient Demographic Module)
       *    DCM_SmokingStatus                                     (0010,21a0)  CS  O  3  (from the Patient Medical Module)
       *    DCM_RequestingService                                 (0032,1033)  LO  O  3  (from the Imaging Service Request Module)
       *    DCM_IssuerOfAdmissionID                               (0038,0011)  LO  O  3  (from the Visit Identification Module)
       *    DCM_ReasonForTheRequestedProcedure                    (0040,1002)  LO  O  3  (from the Requested Procedure Module)
       *    DCM_RequestedProcedureLocation                        (0040,1005)  LO  O  3  (from the Requested Procedure Module)
       *    DCM_ConfidentialityCode                               (0040,1008)  LO  O  3  (from the Requested Procedure Module)
       *    DCM_ReportingPriority                                 (0040,1009)  SH  O  3  (from the Requested Procedure Module)
       *    DCM_RequestedProcedureComments                        (0040,1400)  LT  O  3  (from the Requested Procedure Module)
       *    DCM_RETIRED_ReasonForTheImagingServiceRequest         (0040,2001)  LO  O  3  (from the Imaging Service Request Module)
       *    DCM_IssueDateOfImagingServiceRequest                  (0040,2004)  DA  O  3  (from the Imaging Service Request Module)
       *    DCM_IssueTimeOfImagingServiceRequest                  (0040,2005)  TM  O  3  (from the Imaging Service Request Module)
       *    DCM_OrderEnteredBy                                    (0040,2008)  PN  O  3  (from the Imaging Service Request Module)
       *    DCM_OrderEnterersLocation                             (0040,2009)  SH  O  3  (from the Imaging Service Request Module)
       *    DCM_OrderCallbackPhoneNumber                          (0040,2010)  SH  O  3  (from the Imaging Service Request Module)
       *    DCM_PlacerOrderNumberImagingServiceRequest            (0040,2016)  LO  O  3  (from the Imaging Service Request Module)
       *    DCM_FillerOrderNumberImagingServiceRequest            (0040,2017)  LO  O  3  (from the Imaging Service Request Module)
       *    DCM_ImagingServiceRequestComments                     (0040,2400)  LT  O  3  (from the Imaging Service Request Module)
       *    DCM_RequestedProcedureCodeSequence                    (0032,1064)  SQ  O  3  (from the Requested Procedure Module)
       *     > DCM_CodeValue                                      (0008,0100)  SH  O  1C
       *     > DCM_CodingSchemeVersion                            (0008,0103)  SH  O  3
       *     > DCM_CodingSchemeDesignator                         (0008,0102)  SH  O  1C
       *     > DCM_CodeMeaning                                    (0008,0104)  LO  O  3
       *  @param element            Pointer to the element which shall be checked.
       *  @param supSequenceElement Pointer to the superordinate sequence element of which
       *                            the currently processed element is an attribute, or NULL if
       *                            the currently processed element does not belong to any sequence.
       *  @return OFTrue in case the given tag is a supported return key attribute, OFFalse otherwise.
       */
    OFBool IsSupportedReturnKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );

      /** This function removes all elements from the given DcmDataset object.
       *  @param idents pointer to object which shall be cleared.
       */
    void ClearDataset( DcmDataset *idents );

      /** This function inserts the tag of an offending element into the
       *  corresponding member variable, unless this tag is already con-
       *  tained in this variable.
       *  @param tag The tag that shall be inserted.
       */
    void PutOffendingElements( const DcmTagKey &tag );

      /** This function inserts the tag of an error element into the
       *  corresponding member variable, without checking if it is already
       *  contained in this variable.
       *  @param tag The tag that shall be inserted.
       */
    void PutErrorElements( const DcmTagKey &tag );

      /** This function checks if the passed matching key's value only uses characters
       *  which are part of its data type's character repertoire. Note that at the moment
       *  this application supports the following matching key attributes:
       *    DCM_ScheduledProcedureStepSequence                    (0040,0100)  SQ  R  1
       *     > DCM_ScheduledStationAETitle                        (0040,0001)  AE  R  1
       *     > DCM_ScheduledProcedureStepStartDate                (0040,0002)  DA  R  1
       *     > DCM_ScheduledProcedureStepStartTime                (0040,0003)  TM  R  1
       *     > DCM_Modality                                       (0008,0060)  CS  R  1
       *     > DCM_ScheduledPerformingPhysicianName               (0040,0006)  PN  R  2
       *    DCM_PatientName                                       (0010,0010)  PN  R  1
       *    DCM_PatientID                                         (0010,0020)  LO  R  1
       *    DCM_AccessionNumber                                   (0008,0050)  SH  O  2
       *    DCM_RequestedProcedureID                              (0040,1001)  SH  O  1
       *    DCM_ReferringPhysicianName                            (0008,0090)  PN  O  2
       *    DCM_PatientSex                                        (0010,0040)  CS  O  2
       *    DCM_RequestingPhysician                               (0032,1032)  PN  O  2
       *    DCM_AdmissionID                                       (0038,0010)  LO  O  2
       *    DCM_RequestedProcedurePriority                        (0040,1003)  SH  O  2
       *    DCM_PatientBirthDate                                  (0010,0030)  DA  O  2
       *  As a result, the following data types have to be supported in this function:
       *  AE, DA, TM, CS, PN, LO and SH. For the correct specification of these datatypes
       *  2003 DICOM standard, part 5, section 6.2, table 6.2-1.
       *  @param elem Element which shall be checked.
       *  @return OFTrue in case the given element's value only uses characters which are part of
       *          the element's data type's character repertoire. OFFalse in case the given element's value
       *          does not only use characters which are part of the element's data type's character repertoire.
       */
    OFBool CheckMatchingKey( const DcmElement *elem );

      /** This function returns OFTrue if all the characters of s can be found
       *  in the string charset.
       *  @param s String which shall be checked.
       *  @param charset Possible character set for s. (valid pointer expected.)
       *  @return This function returns OFTrue if all the characters of s can be found
       *          in the string charset. If s equals NULL, OFTrue will be returned.
       */
    OFBool ContainsOnlyValidCharacters( const char *s, const char *charset );

      /** This function checks if the given value is a valid date or date range.
       *  @param value The value which shall be checked.
       *  @return OFTrue in case the given value is a valid date or date range, OFFalse otherwise.
       */
    OFBool IsValidDateOrDateRange( const OFString& value );

      /** This function checks if the given date value is valid.
       *  According to the 2001 DICOM standard, part 5, Table 6.2-1, a date
       *  value is either in format "yyyymmdd" or in format "yyyy.mm.dd",
       *  so that e.g. "19840822" represents August 22, 1984.
       *  @param value The value which shall be checked.
       *  @return OFTrue in case the Date is valid, OFFalse otherwise.
       */
    OFBool IsValidDate( const OFString& value );

      /** This function checks if the given value is a valid time or time range.
       *  @param value The value which shall be checked.
       *  @return OFTrue in case the given value is a valid time or time range, OFFalse otherwise.
       */
    OFBool IsValidTimeOrTimeRange( const OFString& value );

      /** This function checks if the given time value is valid.
       *  According to the 2001 DICOM standard, part 5, Table 6.2-1, a time
       *  value is either in format "hhmmss.fracxx" or "hh:mm:ss.fracxx" where
       *  - hh represents the hour (0-23)
       *  - mm represents the minutes (0-59)
       *  - ss represents the seconds (0-59)
       *  - fracxx represents the fraction of a second in millionths of seconds (000000-999999)
       *  Note that one or more of the components mm, ss, or fracxx may be missing as
       *  long as every component to the right of a missing component is also missing.
       *  If fracxx is missing, the "." character in front of fracxx is also missing.
       *  @param value The value which shall be checked.
       *  @return OFTrue in case the time is valid, OFFalse otherwise.
       */
    OFBool IsValidTime( const OFString& value );

      /** This function returns the value of the given DICOM string element (attribute)
       *  in the parameter resultVal and returns OFTrue if successful.
       *  If the element does not refer to a string attribute or contains an empty value,
       *  OFFalse is returned.
       *  @param elem The DICOM element.
       *  @param resultVal The resulting string value
       *  @return The value of the given DICOM (string) element or NULL.
       */
    OFBool GetStringValue( const DcmElement *elem,
                           OFString& resultVal );

      /** This function makes a copy of value without leading and trailing blanks.
       *  @param value The source string.
       *  @return A copy of the given string without leading and trailing blanks.
       */
    OFString DeleteLeadingAndTrailingBlanks( const OFString& value );

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmDataSource( const WlmDataSource &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmDataSource &operator=( const WlmDataSource &Src );


  public:
      /** default constructor.
       */
    WlmDataSource();

      /** destructor
       */
    virtual ~WlmDataSource();

      /** Connects to the data source.
       * @return Indicates if the connection was established succesfully.
       */
    virtual OFCondition ConnectToDataSource() = 0;

      /** Disconnects from the data source.
       * @return Indicates if the disconnection was completed succesfully.
       */
    virtual OFCondition DisconnectFromDataSource() = 0;

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetCalledApplicationEntityTitle( const OFString& value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetFailOnInvalidQuery( OFBool value );

      /** Set value in a member variable.
       *  @param value The value to set.
       */
    void SetNoSequenceExpansion( OFBool value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetReturnedCharacterSet( WlmReturnedCharacterSetType value );

      /** Checks if the called application entity title is supported. This function expects
       *  that the called application entity title was made available for this instance through
       *  WlmDataSource::SetCalledApplicationEntityTitle(). If this is not the case, OFFalse
       *  will be returned.
       *  @return OFTrue, if the called application entity title is supported;
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    virtual OFBool IsCalledApplicationEntityTitleSupported() = 0;

      /** Based on the search mask which was passed, this function determines all the records
       *  in the database which match the values of matching key attributes in the search mask.
       *  For each matching record, a DcmDataset structure is generated which will later be
       *  returned to the SCU as a result of query. The DcmDataset structures for all matching
       *  records will be stored in the protected member variable matchingDatasets.
       *  @param findRequestIdentifiers Contains the search mask.
       *  @return A WlmDataSourceStatusType value denoting the following:
       *          WLM_SUCCESS: No matching records found;
       *          WLM_PENDING: Matching records found, all return keys supported by this application;
       *          WLM_PENDING_WARNING: Matching records found, not all return keys supported by this application;
       *          WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS: Error in the search mask encountered.
       */
    virtual WlmDataSourceStatusType StartFindRequest( const DcmDataset &findRequestIdentifiers ) = 0;

      /** This function will return the next dataset that matches the given search mask, if
       *  there is one more resulting dataset to return. In such a case, rstatus will be set
       *  to WLM_PENDING or WLM_PENDING_WARNING, depending on if an unsupported key attribute
       *  was encountered in the search mask or not. If there are no more datasets that match
       *  the search mask, this function will return an empty dataset and WLM_SUCCESS in rstatus.
       *  @param rStatus A value of type WlmDataSourceStatusType that can be used to
       *                 decide if there are still elements that have to be returned.
       *  @return The next dataset that matches the given search mask, or an empty dataset if
       *          there are no more matching datasets in the database.
       */
    virtual DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus ) = 0;

      /** This function handles a C-CANCEL Request during the processing of a C-FIND Request.
       *  In detail, in case there are still matching datasets captured in member variable
       *  matchingDatasets, memory for these datasets (and the array itself) is freed and
       *  all pointers are set to NULL.
       *  @return Always WLM_CANCEL.
       */
    WlmDataSourceStatusType CancelFindRequest();

      /** Get value from member variable.
       *  @return The member variable's value.
       */
    DcmAttributeTag *GetOffendingElements();

      /** Get value from member variable.
       *  @return The member variable's value.
       */
    DcmLongString *GetErrorComments();

      /** Set value in a member variable in a derived class.
       */
    virtual void SetDbDsn( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetDbUserName( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetDbUserPassword( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetCfgFileMatchRecords( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetCfgFileSelectValues( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetDatabaseType( WlmDatabaseType /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param int The value to set.
       */
    virtual void SetSerialNumber( const int /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param int The value to set.
       */
    virtual void SetInstitutionId( const unsigned int /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetDfPath( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetEnableRejectionOfIncompleteWlFiles( OFBool /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetCreateNullvalues( OFBool /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetPfFileName( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetModalityToReturn( const OFString& /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetCommaSeparatePatientName( OFBool /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       */
    virtual void SetReturnPatientUidInAccessionNumber( OFBool /*value*/ ) {}
};

#endif

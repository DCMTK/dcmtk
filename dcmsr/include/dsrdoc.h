/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRDocument
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-10 17:44:49 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRDOC_H
#define DSRDOC_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrdoctr.h"

#include <iostream.h>    /* for class ostream */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for 'dcmsr' (DICOM Structured Reporting Documents).
 *  This class supports reading, writing, creation, printing and rendering of
 *  DICOM SR documents (according to supplement 23 final text).
 */
class DSRDocument
  : protected DSRTypes
{

  public:

  // --- constructors and destructor ---

    /** default constructor.
     *  The parameter documentType is optional and has a default value.
     ** @param  documentType  type of the SR document (BasicText, Enhanced, Comprehensive)
     */
    DSRDocument(const E_DocumentType documentType = DT_BasicTextSR);

    /** destructor
     */
    virtual ~DSRDocument();


  // --- misc routines ---

    /** clear all internal member variables
     */
    void clear();

    /** check whether the current internal state is valid.
     *  The SR document is valid if the corresponding document tree is valid and
     *  the SOP instance UID as well as the SOP class UID are not "empty".
     ** @return OFTrue if valid, OFFalse otherwise
     */
    OFBool isValid();

    /** set the log stream.
     *  The log stream is used to report any warnings and error messages.
     ** @param  stream  pointer to the log stream (might be NULL = no messages)
     */
    void setLogStream(OFConsole *stream);


  // --- input and output ---

    /** print current SR document to specified output stream
     ** @param  stream  output stream
     *  @param  flags   optional flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition print(ostream &stream,
                      const size_t flags = 0);

    /** read SR document from DICOM dataset.
     *  Please note that the current document is also deleted if the reading fails.
     *  If the log stream is set and valid the reason for any error might be obtained
     *  from the error/warning output.
     ** @param  dataset  reference to DICOM dataset where the document should be read from
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition read(DcmItem &dataset);

    /** write current SR document to DICOM dataset
     ** @param  dataset  reference to DICOM dataset where the current document should be written to
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition write(DcmItem &dataset);

    /** write current SR document in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition writeXML(ostream &stream,
                         const size_t flags = 0);

    /** render current SR document in HTML format
     ** @param  stream      output stream to which the HTML document is written
     *  @param  flags       optional flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  styleSheet  optional filename/URL of a Cascading Style Sheet (CSS)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition renderHTML(ostream &stream,
                           const size_t flags = 0,
                           const char *styleSheet = NULL);


  // --- get/set misc attributes ---

    /** get the current SR document type
     ** @return document type (might be DT_invalid if read from dataset)
     */
    E_DocumentType getDocumentType() const;

    /** get document tree
     ** @return reference to the document tree
     */
    inline DSRDocumentTree &getTree()
    {
        return DocumentTree;
    }

    /** get specific character set type.
     *  If the type is unknown the original DICOM defined term can be retrieved
     *  with the method getSpecificCharacterSet().
     ** @return character set (might be CS_invalid/unknown if not supported)
     */
    E_CharacterSet getSpecificCharacterSetType() const;

    /** set specific character set type.
     *  The DICOM defined term (see SpecificCharacterSet) is set accordingly.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setSpecificCharacterSetType(const E_CharacterSet characterSet);

    /** get document completion flag
     ** @return completion flag (might be CF_invalid if read from dataset)
     */
    E_CompletionFlag getCompletionFlag() const;

    /** get document completion flag description
     ** @return pointer to string value (might be NULL)
     */
    const char *getCompletionFlagDescription() const;

    /** get document completion flag description
     ** @param  description  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getCompletionFlagDescription(OFString &description) const;

    /** set document completion flag description.
     *  The description can be removed from the DICOM dataset (type 3) by setting an empty string.
     ** @param  description  explanation of the value set for completion flag (optional, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setCompletionFlagDescription(const OFString &description);

    /** get document verification flag
     ** @return verification flag (might be VF_invalid if read from dataset)
     */
    E_VerificationFlag getVerificationFlag() const;

    /** get number of verifying observers.
     *  A document can be verified more than once.  The verification flag should be VERIFIED
     *  if any verifying observer is specified.  The details on the observer can be retrieved
     *  using the getVerifyingObserver() methods.
     ** @return number of verifying observers (if any), 0 otherwise
     */
    size_t getNumberOfVerifyingObservers();
    
    /** get information about a verifying observer.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain correct data.
     ** @param  idx           index of the verifying observer to be retrieved (starting with 1).
     *                        Use getNumberOfVerifyingObservers() to get the maximum value.
     *  @param  dateTime      reference to variable where the date and time when this document
     *                        has been verified should be stored (required)
     *  @param  observerName  reference to variable where the name of the person who has verified
     *                        this document should be stored (required)
     *  @param  organization  reference to variable where the name of the organization to which
     *                        the observer belongs should be stored (required)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getVerifyingObserver(const size_t idx,
                                     OFString &dateTime,
                                     OFString &observerName,
                                     OFString &organization);

    /** get information about a verifying observer.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain correct data.
     ** @param  idx           index of the verifying observer to be retrieved (starting with 1).
     *                        Use getNumberOfVerifyingObservers() to get the maximum value.
     *  @param  dateTime      reference to variable where the date and time when this document
     *                        has been verified should be stored (required)
     *  @param  observerName  reference to variable where the name of the person who has verified
     *                        this document should be stored (required)
     *  @param  observerCode  reference to variable where the observer code should be stored.
     *                        code identifying the verifying observer (optional, see previous method)
     *  @param  organization  reference to variable where the name of the organization to which
     *                        the observer belongs should be stored (required)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getVerifyingObserver(const size_t idx,
                                     OFString &dateTime,
                                     OFString &observerName,
                                     DSRCodedEntryValue &observerCode,
                                     OFString &organization);

    /** get number of predecessor documents.
     *  A document can have more than one (direct) predecessor document.  This is the case
     *  when two or more documents have been merged to created it.  The corresponding method
     *  createRevisedVersion() creates only one reference to the current document (and 
     *  replaces any existing referenced).
     ** @return number of predecessor documents (if any), 0 otherwise
     */
    size_t getNumberOfPredecessorDocuments();
    
    /** get information about a predecessor document.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain correct data.
     ** @param  idx             index of the predecessor document to be retrieved (starting with 1).
     *                          Use getNumberOfPredecessorDocuments() to get the maximum value.
     *  @param  sopClassUID     reference to variable where the SOP class UID of the predecessor
     *                          document should be stored (required)
     *  @param  sopInstanceUID  reference to variable where the SOP instance UID of the predecessor
     *                          document should be stored (required)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getPredecessorDocument(const size_t idx,
                                       OFString &sopClassUID,
                                       OFString &sopInstanceUID);

    /** get information about a predecessor document.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain correct data.
     ** @param  idx                index of the predecessor document to be retrieved (starting with 1).
     *                             Use getNumberOfPredecessorDocuments() to get the maximum value.
     *  @param  studyInstanceUID   reference to variable where the study instance UID of the predecessor
     *                             document should be stored (required)
     *  @param  seriesInstanceUID  reference to variable where the series instance UID of the predecessor
     *                             document should be stored (required)
     *  @param  sopClassUID        reference to variable where the SOP class UID of the predecessor
     *                             document should be stored (required)
     *  @param  sopInstanceUID     reference to variable where the SOP instance UID of the predecessor
     *                             document should be stored (required)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getPredecessorDocument(const size_t idx,
                                       OFString &studyInstanceUID,
                                       OFString &seriesInstanceUID,
                                       OFString &sopClassUID,
                                       OFString &sopInstanceUID);


  // --- get DICOM string attributes (C string) ---

    /** get modality
     ** @return pointer to string value (might be NULL)
     */
    const char *getModality() const;

    /** get SOP class UID
     ** @return pointer to string value (might be NULL)
     */
    const char *getSOPClassUID() const;

    /** get study instance UID
     ** @return pointer to string value (might be NULL)
     */
    const char *getStudyInstanceUID() const;

    /** get series instance UID
     ** @return pointer to string value (might be NULL)
     */
    const char *getSeriesInstanceUID() const;

    /** get SOP instance UID
     ** @return pointer to string value (might be NULL)
     */
    const char *getSOPInstanceUID() const;

    /** get instance creator UID
     ** @return pointer to string value (might be NULL)
     */
    const char *getInstanceCreatorUID() const;

    /** get specific character set
     ** @return pointer to string value (might be NULL)
     */
    const char *getSpecificCharacterSet() const;

    /** get patient's name
     ** @return pointer to string value (might be NULL)
     */
    const char *getPatientsName() const;

    /** get patient's birth date
     ** @return pointer to string value (might be NULL)
     */
    const char *getPatientsBirthDate() const;

    /** get patient's sex
     ** @return pointer to string value (might be NULL)
     */
    const char *getPatientsSex() const;

    /** get referring physicians name
     ** @return pointer to string value (might be NULL)
     */
    const char *getReferringPhysiciansName() const;

    /** get study description
     ** @return pointer to string value (might be NULL)
     */
    const char *getStudyDescription() const;

    /** get series description
     ** @return pointer to string value (might be NULL)
     */
    const char *getSeriesDescription() const;

    /** get manufacturer
     ** @return pointer to string value (might be NULL)
     */
    const char *getManufacturer() const;

    /** get study date
     ** @return pointer to string value (might be NULL)
     */
    const char *getStudyDate() const;

    /** get study time
     ** @return pointer to string value (might be NULL)
     */
    const char *getStudyTime() const;

    /** get instance creation date
     ** @return pointer to string value (might be NULL)
     */
    const char *getInstanceCreationDate() const;

    /** get instance creation time
     ** @return pointer to string value (might be NULL)
     */
    const char *getInstanceCreationTime() const;

    /** get content date
     ** @return pointer to string value (might be NULL)
     */
    const char *getContentDate() const;

    /** get content time
     ** @return pointer to string value (might be NULL)
     */
    const char *getContentTime() const;

    /** get study ID
     ** @return pointer to string value (might be NULL)
     */
    const char *getStudyID() const;

    /** get patient ID
     ** @return pointer to string value (might be NULL)
     */
    const char *getPatientID() const;

    /** get series number
     ** @return pointer to string value (might be NULL)
     */
    const char *getSeriesNumber() const;

    /** get instance number
     ** @return pointer to string value (might be NULL)
     */
    const char *getInstanceNumber() const;

    /** get accession number
     ** @return pointer to string value (might be NULL)
     */
    const char *getAccessionNumber() const;


  // --- get DICOM string attributes (C++ string) ---

    /** get modality
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getModality(OFString &string) const;

    /** get SOP class UID
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSOPClassUID(OFString &string) const;

    /** get study instance UID
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyInstanceUID(OFString &string) const;

    /** get series instance UID
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSeriesInstanceUID(OFString &string) const;

    /** get SOP instance UID
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSOPInstanceUID(OFString &string) const;

    /** get instance creator UID
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceCreatorUID(OFString &string) const;

    /** get specific character set
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSpecificCharacterSet(OFString &string) const;

    /** get patient's name
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientsName(OFString &string) const;

    /** get patient's birth date
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientsBirthDate(OFString &string) const;

    /** get patient's sex
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientsSex(OFString &string) const;

    /** get referring physicians name
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getReferringPhysiciansName(OFString &string) const;

    /** get study description
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyDescription(OFString &string) const;

    /** get series description
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSeriesDescription(OFString &string) const;

    /** get manufacturer
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getManufacturer(OFString &string) const;

    /** get study date
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyDate(OFString &string) const;

    /** get study time
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyTime(OFString &string) const;

    /** get instance creation date
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceCreationDate(OFString &string) const;

    /** get instance creation time
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceCreationTime(OFString &string) const;

    /** get content date
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getContentDate(OFString &string) const;

    /** get content time
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getContentTime(OFString &string) const;

    /** get study ID
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyID(OFString &string) const;

    /** get patient ID
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientID(OFString &string) const;

    /** get series number
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSeriesNumber(OFString &string) const;

    /** get instance number
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceNumber(OFString &string) const;

    /** get accession number
     ** @param  string  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getAccessionNumber(OFString &string) const;


  // --- set DICOM string attributes ---

    /** set specific character set.
     *  The passed string must be a valid DICOM Code String (CS).  The
     *  internal enumerated value is set accordingly.
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setSpecificCharacterSet(const OFString &string);

    /** set patient's name.
     *  The passed string must be a valid DICOM Person Name (PN).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setPatientsName(const OFString &string);

    /** set patient's birth date.
     *  The passed string must be a valid DICOM Date (DA).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setPatientsBirthDate(const OFString &string);

    /** set patient's sex.
     *  The passed string must be a valid DICOM Code String (CS).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setPatientsSex(const OFString &string);

    /** set referring physicians name.
     *  The passed string must be a valid DICOM Person Name (PN).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setReferringPhysiciansName(const OFString &string);

    /** set study description.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setStudyDescription(const OFString &string);

    /** set series description.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setSeriesDescription(const OFString &string);

    /** set manufacturer.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setManufacturer(const OFString &string);

    /** set study ID.
     *  The passed string must be a valid DICOM Short String (SH).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setStudyID(const OFString &string);

    /** set patient ID.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setPatientID(const OFString &string);

    /** set series number.
     *  The passed string must be a valid DICOM Short String (SH).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setSeriesNumber(const OFString &string);

    /** set instance number.
     *  The passed string must be a valid DICOM Integer String (IS).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setInstanceNumber(const OFString &string);

    /** set accession number.
     *  The passed string must be a valid DICOM Short String (SH).
     ** @param  string  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setAccessionNumber(const OFString &string);


  // --- document management functions ---

    /** create new study and series instance UIDs.
     *  Please note that the SOP instance UID is not changed.
     */
    void createNewStudyAndSeries();

    /** create a new series instance UID.
     *  Please note that the study and SOP instance UIDs are not changed.
     */
    void createNewSeries();

    /** create a new series instance UID.
     *  Please note that the SOP instance UIDs is not changed.
     ** @param  studyUID  study instance UID to be set (should be a valid UID)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition createNewSeries(const OFString &studyUID);

    /** create a new SOP instance.
     *  Generate a new SOP instance UID, set the instance creation date/time, clear the
     *  list of verifying observers and set the verification flag to UNVERIFIED.
     *  This method is used internally for createNewDocument(), createRevisedVersion()
     *  and during object initialization.
     *  It could also be used explicitly from the calling application if a new UID should
     *  be created (see DICOM standard for details).
     *  This method also updates the other DICOM header attributes (calling updateAttributes()).
     */
    void createNewSOPInstance();    

    /** create a new document.
     *  A new SOP instance is only created if the current document type was valid/supported.
     *  Please note that the current document is deleted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition createNewDocument();

    /** create a new document of the specified type.
     *  A new SOP instance is only created if the current document type was valid/supported.
     *  Please note that the current document is deleted.
     ** @param  documentType  type of the SR document (BasicText, Enhanced, Comprehensive)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition createNewDocument(const E_DocumentType documentType);

    /** create a revised version of the current document.
     *  A revised version can only be created if the current document is not yet completed
     *  (see completion flag).  If so a reference to the current document is included in the
     *  predecessor documents sequence (possible existing references are automatically
     *  replaced, i.e. there is never more than one reference in this sequence).  If all 
     *  revised versions of a SR document are stored (written to datasets/files) it is
     *  possible to track back the full chain of previous versions.
     *  A new SOP instance is created and the content date/time are set automatically.
     *  Furthermore the verifying observer sequence is deleted, the verifcation flag is set
     *  to UNVERIFIED and the completion flag is set to PARTIAL (i.e. not complete).
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition createRevisedVersion();

    /** complete the current document.
     *  Sets the completion flag to COMPLETE if not already done (fails otherwise).
     *  The completion flag description is set to an empty string (i.e. absent in DICOM
     *  dataset).
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition completeDocument();

    /** complete the current document and set completion description.
     *  Sets the completion flag to COMPLETE if not already done (fails otherwise).
     *  The completion flag description can be modified independently from the flag by means
     *  of the method setCompletionFlagDescription() - see above.
     ** @param  description  explanation of the value set for completion flag (optional, see
     *                       previous method, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition completeDocument(const OFString &description);

    /** verify the current document by a specific observer.
     *  A document can be verified more than once.  The observer information is added to a
     *  sequence stored in the dataset.  The verification flag is automatically set to
     *  VERIFIED (if not already done).
     *  Please note that only completed documents (see completion flag) can be verified.
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition verifyDocument(const OFString &observerName,
                               const OFString &organization);

    /** verify the current document by a specific observer.
     *  A document can be verified more than once.  The observer information is added to a
     *  sequence stored in the dataset.  The verification flag is automatically set to
     *  VERIFIED (if not already done).
     *  Please note that only completed documents (see completion flag) can be verified.
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  observerCode  code identifying the verifying observer (optional, see previous method)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition verifyDocument(const OFString &observerName,
                               const DSRCodedEntryValue &observerCode,
                               const OFString &organization);


  protected:

    /** render patient name, sex, birthdate and ID in HTML format
     ** @param  stream  output stream to which the HTML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::HF_xxx)
     */
    void renderHTMLPatientData(ostream &stream,
                               const size_t flags);

    /** check the given dataset before reading.
     *  This methods checks whether the dataset contains at least the DICOM attributes SOP class UID
     *  and modality.  Any incorrectness regarding these two attributes is reported to the log stream
     *  (if valid).  Unsupported SOP classes are also reported as error (e.g. ComprehensiveSR).
     ** @param  dataset  DICOM dataset to be checked
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition checkDatasetForReading(DcmItem &dataset);

    /** update several DICOM attributes.
     *  (e.g. set the modality to 'SR', generate a new SOP instance UID if required, set date/time, etc.)
     */
    void updateAttributes();


  private:

    /// SR document tree
    DSRDocumentTree DocumentTree;

    /// output stream for error messages, NULL for no messages
    OFConsole *LogStream;

    /// enumerated value: partial, complete
    E_CompletionFlag   CompletionFlagEnum;
    /// enumerated value: unverified, verified
    E_VerificationFlag VerificationFlagEnum;
    /// defined term: see DSRTypes
    E_CharacterSet     SpecificCharacterSetEnum;

    // DICOM attributes are listed here ordered by module.
    // The comments for each attribute describe the Name: (VR, VM, Type).

    // --- SOP Common Module (M) ---
    // (see SR Document General Module)

    /// SOP Class UID: (UI, 1, 1)
    DcmUniqueIdentifier SOPClassUID;
    /// SOP Instance UID: (UI, 1, 1)
    DcmUniqueIdentifier SOPInstanceUID;
    /// Specific Character Set: (CS, 1-n, 1C)
    DcmCodeString       SpecificCharacterSet;
    /// Instance Creation Date: (DA, 1, 3)
    DcmDate             InstanceCreationDate;
    /// Instance Creation Time: (TM, 1, 3)
    DcmTime             InstanceCreationTime;
    /// Instance Creator UID: (UI, 1, 3)
    DcmUniqueIdentifier InstanceCreatorUID;

    // --- General Study Module (M) ---

    /// Study Instance UID: (UI, 1, 1)
    DcmUniqueIdentifier StudyInstanceUID;
    /// Study Date: (DA, 1, 2)
    DcmDate             StudyDate;
    /// StudyTime: (TM, 1, 2)
    DcmTime             StudyTime;
    /// ReferringPhysiciansName: (PN, 1, 2)
    DcmPersonName       ReferringPhysiciansName;
    /// Study ID: (SH, 1, 2)
    DcmShortString      StudyID;
    /// Accession Number: (SH, 1, 2)
    DcmShortString      AccessionNumber;
    /// Study Description: (LO, 1, 3)
    DcmLongString       StudyDescription;

    // --- General Series Module ---
    // (see SR Document Series Module)

    /// Series Description: (LO, 1, 3)
    DcmLongString       SeriesDescription;

    // --- Patient Module (M) ---

    /// Person Name: (PN, 1, 2)
    DcmPersonName       PatientsName;
    /// Patient ID: (LO, 1, 2)
    DcmLongString       PatientID;
    /// Patient Birth Date: (DA, 1, 2)
    DcmDate             PatientsBirthDate;
    /// Patient Sex: (CS, 1, 2)
    DcmCodeString       PatientsSex;

    // --- General Equipment Module (M) ---

    /// Manufacturer: (LO, 1, 2)
    DcmLongString       Manufacturer;

    // --- SR Document Series Module (M) ---

    /// Modality: (CS, 1, 1)
    DcmCodeString       Modality;
    /// Series Instance Number: (UI, 1, 1)
    DcmUniqueIdentifier SeriesInstanceUID;
    /// Series Number: (IS, 1, 1)
    DcmIntegerString    SeriesNumber;
    /// Referenced Study Component Sequence: (SQ, 1, 2)
    DcmSequenceOfItems  ReferencedStudyComponent;

    // --- SR Document General Module (M) ---

    /// Instance Number: (IS, 1, 1)
    DcmIntegerString    InstanceNumber;
    /// Completion Flag: (CS, 1, 1)
    DcmCodeString       CompletionFlag;
    /// Completion Flag Description: (LO, 1, 3)
    DcmLongString       CompletionFlagDescription;
    /// Verification Flag: (CS, 1, 1)
    DcmCodeString       VerificationFlag;
    /// Content Date: (DA, 1, 1)
    DcmDate             ContentDate;
    /// Content Time: (TM, 1, 1)
    DcmTime             ContentTime;
    /// Verifying Observer Sequence: (SQ, 1, 1C)
    DcmSequenceOfItems  VerifyingObserver;
    /// Predecessor Documents Sequence: (SQ, 1, 1C)
    DcmSequenceOfItems  PredecessorDocuments;
    //  Referenced Request Sequence: (SQ, 1, 1C)

    //  Performed Procedure Code Sequence: (SQ, 1, 2)
    DcmSequenceOfItems  PerformedProcedureCode;

    //  Current Requested Procedure Evidence Sequence: (SQ, 1, 1C)

    //  Pertinent Other Evidence Sequence: (SQ, 1, 1C)


 // --- declaration copy constructor and assignment operator

    DSRDocument(const DSRDocument &);
    DSRDocument &operator=(const DSRDocument &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoc.h,v $
 *  Revision 1.10  2000-11-10 17:44:49  joergr
 *  Added new methods to set the completion flag description, create new study/
 *  series UIDs. Added missing type 2 sequence to dataset. Corrected wrong format
 *  of predecessor documents sequence. Changed behaviour of completion/verification
 *  flags. Improved HTML and print/dump output.
 *
 *  Revision 1.9  2000/11/09 20:32:07  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.8  2000/11/07 18:12:25  joergr
 *  Enhanced rendered HTML output of date, time, datetime and pname.
 *
 *  Revision 1.7  2000/11/01 16:16:33  joergr
 *  dded support for conversion to XML.
 *  Added support for Cascading Style Sheet (CSS) used optionally for HTML
 *  rendering.
 *  Enhanced support for specific character sets.
 *
 *  Revision 1.6  2000/10/26 14:17:06  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.5  2000/10/18 17:01:47  joergr
 *  Made some functions inline.
 *
 *  Revision 1.4  2000/10/17 12:35:05  joergr
 *  Added methods to retrieve information on predecessor documents and
 *  verifying observers.
 *
 *  Revision 1.3  2000/10/16 16:30:23  joergr
 *  Added missing get/setSeriesDescription() methods.
 *  Updated comments.
 *
 *  Revision 1.2  2000/10/16 11:59:04  joergr
 *  Made method creating a new SOP instance public. Added check for correct SOP
 *  instance UID and SOP class UID to validity check.
 *
 *  Revision 1.1  2000/10/13 07:49:25  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */

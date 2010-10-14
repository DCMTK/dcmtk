/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRDocument
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:32 $
 *  CVS/RCS Revision: $Revision: 1.53 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRDOC_H
#define DSRDOC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctr.h"
#include "dcmtk/dcmsr/dsrsoprf.h"
#include "dcmtk/dcmsr/dsrcsidl.h"

#include "dcmtk/ofstd/ofstream.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for 'dcmsr' (DICOM Structured Reporting Documents).
 *  This class supports reading, writing, creation, printing and rendering of DICOM
 *  Structured Reporting (SR) documents.
 *  The list of supported SOP classes is available in file "dsrtypes.h".
 */
class DSRDocument
  : protected DSRTypes
{

  public:

  // --- constructors and destructor ---

    /** (default) constructor.
     *  The parameter 'documentType' is optional and has a default value.
     ** @param  documentType  type of the SR document (see DSRTypes::E_DocumentType)
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

    /** check whether the document is finalized.
     *  A new document is originally not finalized but can be finalized using the method
     *  finalizeDocument().  This flag is e.g. used to indicate whether the entire document
     *  is digitally signed and, therefore, each newly added verifying observer would corrupt
     *  all previous signatures.
     ** @return OFTrue if finalized, OFFalse otherwise
     */
    OFBool isFinalized() const;


  // --- input and output ---

    /** print current SR document to specified output stream.
     *  The output format is identical to that of the dsrdump command line tool.
     ** @param  stream  output stream (e.g. COUT from "ofconsol.h")
     *  @param  flags   optional flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition print(STD_NAMESPACE ostream &stream,
                      const size_t flags = 0);

    /** read SR document from DICOM dataset.
     *  Please note that the current document is also deleted if the reading process fails.
     *  If the log stream is set and valid the reason for any error might be obtained
     *  from the error/warning output.
     ** @param  dataset  reference to DICOM dataset from which the document should be read
     *  @param  flags    optional flag used to customize the reading process (see DSRTypes::RF_xxx).
     *                   E.g. RF_readDigitalSignatures indicates whether to read the digital
     *                   signatures from the dataset or not.  If set the MACParametersSequence
     *                   and the DigitalSignaturesSequence are read for the general document
     *                   header (equivilent to top-level content item) and each content item
     *                   of the document tree.
     *                   If not removed manually (with 'DSRDocumentTree::removeSignatures')
     *                   the signatures are written back to the dataset when the method 'write'
     *                   is called.
     *                   Please note that the two signature sequences for any other sequence
     *                   (e.g. VerifyingObserver or PredecessorDocuments) are never read.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const size_t flags = 0);

    /** write current SR document to DICOM dataset.
     *  Please note that the ContentTemplateSequence for the root content item is not written
     *  automatically for particular SOP Classes (e.g. Key Object Selection Document).
     *  Instead, the template identification has to be set manually for the root CONTAINER
     *  (see DSRDocumentTreeNode::setTemplateIdentification()).  This is because the template
     *  constraints cannot be checked yet.
     ** @param  dataset      reference to DICOM dataset to which the current document should be
     *                       written
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *                       Can be used to digitally sign parts of the document tree.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset,
                      DcmStack *markedItems = NULL);

    /** read SR document from XML file.
     *  The format (Schema) of the XML document is expected to conform to the output format
     *  of the writeXML() method.  In addition, the document can be validated against an XML
     *  Schema by setting the flag XF_validateSchema.
     *  Digital signatures in the XML document are not yet supported.
     *  Please note that the current document is also deleted if the parsing process fails.
     ** @param  filename  name of the file from which the XML document is read ("-" for stdin)
     *  @param  flags     optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXML(const OFString &filename,
                        const size_t flags = 0);

    /** write current SR document in XML format.
     *  The output format is identical to that of the dsr2xml command line tool.  Digital
     *  signatures in the XML document are not yet supported.
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(STD_NAMESPACE ostream &stream,
                         const size_t flags = 0);

    /** render current SR document in HTML/XHTML format.
     *  The output format is identical to that of the dsr2html command line tool.
     ** @param  stream      output stream to which the HTML/XHTML document is written
     *  @param  flags       optional flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  styleSheet  optional filename/URL of a Cascading Style Sheet (CSS)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition renderHTML(STD_NAMESPACE ostream &stream,
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
     *  with the method getSpecificCharacterSet().  Please note that only the
     *  first of possibly multiple values is used to determine the type from the
     *  DICOM code string (multiple character sets are not yet supported).
     ** @return character set (might be CS_invalid/unknown if not supported)
     */
    E_CharacterSet getSpecificCharacterSetType() const;

    /** set specific character set type.
     *  The DICOM defined term (see SpecificCharacterSet) is set accordingly.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSpecificCharacterSetType(const E_CharacterSet characterSet);

    /** get document preliminary flag.
     *  Not applicable to Key Object Selection Documents.
     ** @return preliminary flag (might be PF_invalid if not specified)
     */
    E_PreliminaryFlag getPreliminaryFlag() const;

    /** get document completion flag.
     *  Not applicable to Key Object Selection Documents.
     ** @return completion flag (might be CF_invalid if read from dataset)
     */
    E_CompletionFlag getCompletionFlag() const;

    /** get document completion flag description.
     *  Not applicable to Key Object Selection Documents.
     ** @return pointer to string value (might be NULL)
     */
    const char *getCompletionFlagDescription() const;

    /** get document completion flag description.
     *  Not applicable to Key Object Selection Documents.
     ** @param  description  reference to character string in which the value should be stored
     ** @return character string (might be empty)
     */
    const OFString &getCompletionFlagDescription(OFString &description) const;

    /** get document verification flag.
     *  Not applicable to Key Object Selection Documents.
     ** @return verification flag (might be VF_invalid if read from dataset)
     */
    E_VerificationFlag getVerificationFlag() const;

    /** get number of verifying observers.
     *  A document can be verified more than once.  The verification flag should be VERIFIED
     *  if any verifying observer is specified.  The details on the observer can be retrieved
     *  using the getVerifyingObserver() methods.
     *  Not applicable to Key Object Selection Documents.
     ** @return number of verifying observers (if any), 0 otherwise
     */
    size_t getNumberOfVerifyingObservers();

    /** get information about a verifying observer.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain valid (empty) data.
     *  Not applicable to Key Object Selection Documents.
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
    OFCondition getVerifyingObserver(const size_t idx,
                                     OFString &dateTime,
                                     OFString &observerName,
                                     OFString &organization);

    /** get information about a verifying observer.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain valid (empty) data.
     *  Not applicable to Key Object Selection Documents.
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
    OFCondition getVerifyingObserver(const size_t idx,
                                     OFString &dateTime,
                                     OFString &observerName,
                                     DSRCodedEntryValue &observerCode,
                                     OFString &organization);

    /** get list of predecessor documents.
     *  A document can have more than one (direct) predecessor document.  This is e.g. the case
     *  when two or more documents have been merged to create it.  The corresponding method
     *  createRevisedVersion() automatically adds a reference to the current document.
     *  The DICOM standard states: "[The Predecessor Documents Sequence] Shall refer to SR SOP
     *  Instances (e.g. prior or provisional reports) whose content has been wholly or partially
     *  included in this document with or without modification." and "[...] the use of the
     *  Predecessor Document Sequence allows tracing back to the input SR Document, which in this
     *  case is the previous version."
     *  Not applicable to Key Object Selection Documents.
     ** @return reference to list object
     */
    DSRSOPInstanceReferenceList &getPredecessorDocuments();

    /** get list of identical documents.
     *  Please note that currently the user is responsible for filling and modifying the content of
     *  this list.  However, the list is automatically cleared when a new instance is created (incl.
     *  a revised version of the current document).  Possibly, there will be a createDuplicate()
     *  method or the like in the future which creates an identical copy of the current document in
     *  a new study/series.
     *  The DICOM standard states: "If identical copies of an SR Document are to be included in
     *  multiple Studies then the entire SR Document shall be duplicated with appropriate changes
     *  for inclusion into the different Studies (i.e. Study Instance UID, Series Instance UID, SOP
     *  Instance UID, Identical Documents Sequence etc.).  The Identical Documents Sequence Attribute
     *  in each SOP Instance shall contain references to all other duplicate SOP Instances."
     ** @return reference to list object
     */
    DSRSOPInstanceReferenceList &getIdenticalDocuments();

    /** get list of referenced SOP instances (Current Requested Procedure Evidence).
     *  The DICOM standard states: "The intent of the Current Requested Procedure Evidence Sequence
     *  is to reference all evidence created in order to satisfy the current Requested Procedure(s)
     *  for this SR Document.  This shall include, but is not limited to, all current evidence
     *  referenced in the content tree." and "For a completed SR Document satisfying (i.e., being
     *  the final report for) the current Requested Procedure(s), this sequence shall list the full
     *  set of Composite SOP Instances created for the current Requested Procedure(s).  For other
     *  SOP Instances that include the SR Document General Module, this sequence shall contain at
     *  minimum the set of Composite SOP Instances from the current Requested Procedure(s) that are
     *  referenced in the content tree." and "In the context of the Key Object Selection, the
     *  current evidence is considered to be only the set of instances referenced within the Key
     *  Object Selection."
     ** @return reference to list object
     */
    DSRSOPInstanceReferenceList &getCurrentRequestedProcedureEvidence();

    /** get list of referenced SOP instances (Pertinent Other Evidence).
     *  The DICOM standard states: "The Pertinent Other Evidence Sequence attribute is used to
     *  reference all other evidence considered pertinent for this SR Document that is not listed
     *  in the Current Requested Procedure Evidence Sequence.  This requires that the same SOP
     *  Instance shall not be referenced in both of these Sequences."
     *  Not applicable to Key Object Selection Documents.
     ** @return reference to list object
     */
    DSRSOPInstanceReferenceList &getPertinentOtherEvidence();

    /** get list of coding schemes used (Coding Scheme Identification).
     *  The Coding Scheme Identification Sequence maps Coding Scheme Designators to an external coding
     *  system registration, or to a private or local coding scheme.  See DICOM standard for details.
     *  @return reference to list object
     */
    DSRCodingSchemeIdentificationList &getCodingSchemeIdentification();


  // --- get DICOM string attributes (C string) ---
  // --- (these functions return the whole string value,
  // ---  i.e. all components of multi-valued attributes)

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
    const char *getPatientName() const;

    /** get patient's birth date
     ** @return pointer to string value (might be NULL)
     */
    const char *getPatientBirthDate() const;

    /** get patient's sex
     ** @return pointer to string value (might be NULL)
     */
    const char *getPatientSex() const;

    /** get referring physicians name
     ** @return pointer to string value (might be NULL)
     */
    const char *getReferringPhysicianName() const;

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

    /** get manufacturer's model name
     ** @return pointer to string value (might be NULL)
     */
    const char *getManufacturerModelName() const;

    /** get device serial number
     ** @return pointer to string value (might be NULL)
     */
    const char *getDeviceSerialNumber() const;

    /** get software version(s)
     ** @return pointer to string value (might be NULL)
     */
    const char *getSoftwareVersions() const;

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
  // --- (these functions return only the first
  // ---  component of multi-valued attributes)

    /** get modality
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getModality(OFString &value) const;

    /** get SOP class UID
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSOPClassUID(OFString &value) const;

    /** get study instance UID
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyInstanceUID(OFString &value) const;

    /** get series instance UID
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSeriesInstanceUID(OFString &value) const;

    /** get SOP instance UID
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSOPInstanceUID(OFString &value) const;

    /** get instance creator UID
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceCreatorUID(OFString &value) const;

    /** get specific character set
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSpecificCharacterSet(OFString &value) const;

    /** get patient's name
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientName(OFString &value) const;

    /** get patient's birth date
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientBirthDate(OFString &value) const;

    /** get patient's sex
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientSex(OFString &value) const;

    /** get referring physicians name
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getReferringPhysicianName(OFString &value) const;

    /** get study description
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyDescription(OFString &value) const;

    /** get series description
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSeriesDescription(OFString &value) const;

    /** get manufacturer
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getManufacturer(OFString &value) const;

    /** get manufacturer's model name
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getManufacturerModelName(OFString &value) const;

    /** get device serial number
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getDeviceSerialNumber(OFString &value) const;

    /** get software version(s). Please note that only the first component is returned.
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSoftwareVersions(OFString &value) const;

    /** get study date
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyDate(OFString &value) const;

    /** get study time
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyTime(OFString &value) const;

    /** get instance creation date
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceCreationDate(OFString &value) const;

    /** get instance creation time
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceCreationTime(OFString &value) const;

    /** get content date
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getContentDate(OFString &value) const;

    /** get content time
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getContentTime(OFString &value) const;

    /** get study ID
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getStudyID(OFString &value) const;

    /** get patient ID
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getPatientID(OFString &value) const;

    /** get series number
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getSeriesNumber(OFString &value) const;

    /** get instance number
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getInstanceNumber(OFString &value) const;

    /** get accession number
     ** @param  value  reference to character string in which the value should be stored
     ** @return character string (might empty)
     */
    const OFString &getAccessionNumber(OFString &value) const;


  // --- set DICOM string attributes ---

    /** set specific character set.
     *  The passed string must be a valid DICOM Code String (CS).  The internal enumerated
     *  value is set accordingly.
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSpecificCharacterSet(const OFString &value);

    /** set document preliminary flag.
     *  According to the DICOM standard, the concept of "completeness" is independent of the
     *  concept of "preliminary" or "final".  Therefore, this flag can be specified separately.
     ** @param  flag  preliminary flag to be set (use PF_invalid to omit this optional value)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPreliminaryFlag(const E_PreliminaryFlag flag);

    /** set document completion flag description.
     *  The description can be removed from the DICOM dataset (type 3) by setting an empty
     *  string.  Not applicable to Key Object Selection Documents.
     ** @param  value  explanation of the value set for completion flag (optional, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCompletionFlagDescription(const OFString &value);

    /** set patient's name.
     *  The passed string must be a valid DICOM Person Name (PN).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPatientName(const OFString &value);

    /** set patient's birth date.
     *  The passed string must be a valid DICOM Date (DA).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPatientBirthDate(const OFString &value);

    /** set patient's sex.
     *  The passed string must be a valid DICOM Code String (CS).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPatientSex(const OFString &value);

    /** set referring physicians name.
     *  The passed string must be a valid DICOM Person Name (PN).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setReferringPhysicianName(const OFString &value);

    /** set study description.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStudyDescription(const OFString &value);

    /** set series description.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSeriesDescription(const OFString &value);

    /** set manufacturer.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setManufacturer(const OFString &value);

    /** set manufacturer's model name.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setManufacturerModelName(const OFString &value);

    /** set device serial number.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setDeviceSerialNumber(const OFString &value);

    /** set software version(s).
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSoftwareVersions(const OFString &value);

    /** set content date.
     *  The passed string must be a valid DICOM Date (DA).  If an empty string
     *  is passed the current date is set when displaying or writing the document
     *  since the corresponding DICOM attribute is type 1 (= mandatory).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setContentDate(const OFString &value);

    /** set content time.
     *  The passed string must be a valid DICOM Time (TM).  If an empty string
     *  is passed the current time is set when displaying or writing the document
     *  since the corresponding DICOM attribute is type 1 (= mandatory).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setContentTime(const OFString &value);

    /** set study ID.
     *  The passed string must be a valid DICOM Short String (SH).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStudyID(const OFString &value);

    /** set patient ID.
     *  The passed string must be a valid DICOM Long String (LO).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPatientID(const OFString &value);

    /** set series number.
     *  The passed string must be a valid DICOM Short String (SH).  If an empty
     *  string is passed the value "1" is set when displaying or writing the
     *  document since the corresponding DICOM attribute is type 1 (= mandatory).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSeriesNumber(const OFString &value);

    /** set instance number.
     *  The passed string must be a valid DICOM Integer String (IS).  If an empty
     *  string is passed the value "1" is set when displaying or writing the
     *  document since the corresponding DICOM attribute is type 1 (= mandatory).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setInstanceNumber(const OFString &value);

    /** set accession number.
     *  The passed string must be a valid DICOM Short String (SH).
     ** @param  value  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setAccessionNumber(const OFString &value);


  // --- document management functions ---

    /** create new study.
     *  After generating a new study instance UID the method createNewSeries() is called,
     *  i.e. also a new series instance UID and SOP instance UID are generated.  This is
     *  a requirement of the DICOM standard.
     */
    void createNewStudy();

    /** create a new series.
     *  After generating a new series instance UID the method createNewSOPInstance() is
     *  called, i.e. also a SOP instance UID is generated.  This is a requirement of the
     *  DICOM standard.
     */
    void createNewSeries();

    /** create a new series within a given study.
     *  After generating a new series instance UID within the given study the method
     *  createNewSOPInstance() is called, i.e. also a SOP instance UID is generated.
     *  This is a requirement of the DICOM standard.
     ** @param  studyUID  study instance UID to be set (should be a valid UID)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createNewSeriesInStudy(const OFString &studyUID);

    /** create a new SOP instance.
     *  Generate a new SOP instance UID, set the instance creation date/time and reset the
     *  finalized flag (OFFalse).
     *  This method is used internally for createNewDocument(), createRevisedVersion()
     *  and during object initialization.
     *  It could also be used explicitly from the calling application if a new UID should
     *  be created (this is the case if the study instance UID or series instance UID has
     *  changed as well as any other attribute within the SR Document General Module or
     *  SR Document Content Module, see DICOM standard for details).
     *  This method also updates the other DICOM header attributes (calling updateAttributes()).
     */
    void createNewSOPInstance();

    /** create a new document.
     *  A new SOP instance is only created if the current document type was valid/supported.
     *  Please note that the current document is deleted (cleared).
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createNewDocument();

    /** create a new document of the specified type.
     *  A new SOP instance is only created if the current document type was valid/supported.
     *  Please note that the current document is deleted by this method.
     ** @param  documentType  type of the SR document (see DSRTypes::E_DocumentType)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createNewDocument(const E_DocumentType documentType);

    /** create a revised version of the current document.
     *  A revised version can only be created if the current document is already completed
     *  (see completion flag).  If so, a reference to the current document is added to the
     *  predecessor documents sequence.  If all revised versions of a SR document are
     *  stored (written to datasets/files) it is possible to trace back the full chain of
     *  previous versions.
     *  A new SOP instance is created and the content date/time are set automatically.
     *  Furthermore, the verifying observer and identical documents sequence are deleted,
     *  the verification flag is set to UNVERIFIED, the completion flag is set to PARTIAL
     *  (i.e. not complete), the completion flag description is deleted, all digital
     *  signatures contained in the document tree are deleted and the finalized flag is
     *  reset (OFFalse).  The preliminary flag is not modified by this method.
     *  Not applicable to Key Object Selection Documents.
     *  @param clearList clear list of predecessor documents before adding the current
     *    document if OFTrue. Append current document to existing list otherwise.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createRevisedVersion(const OFBool clearList = OFTrue);

    /** complete the current document.
     *  Sets the completion flag to COMPLETE if not already done (fails otherwise).
     *  The completion flag description is set to an empty string (i.e. absent in DICOM
     *  dataset).
     *  Not applicable to Key Object Selection Documents.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition completeDocument();

    /** complete the current document and set a completion description.
     *  Sets the completion flag to COMPLETE if not already done (fails otherwise).
     *  The completion flag description can be modified independently from the flag by means
     *  of the method setCompletionFlagDescription() - see above.
     *  Not applicable to Key Object Selection Documents.
     ** @param  description  explanation of the value set for completion flag.
     *                       (optional, see previous method, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition completeDocument(const OFString &description);

    /** verify the current document by a specific observer.
     *  A document can be verified more than once.  The observer information is added to a
     *  sequence stored in the dataset.  The verification flag is automatically set to
     *  VERIFIED (if not already done) and the finalized flag is reset (set to OFFalse).
     *  Please note that only completed documents (see completion flag) can be verified and that
     *  a new SOP instance UID has to be generated (manually) according to the DICOM standard when
     *  creating a dataset/file from this document.
     *  Not applicable to Key Object Selection Documents.
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition verifyDocument(const OFString &observerName,
                               const OFString &organization);

    /** verify the current document by a specific observer.
     *  Same as above but allows to specify the verification date time value.
     *  Only required since Sun CC 2.0.1 compiler does not support default parameter values for
     *  "complex types" like OFString.  Reports the error message: "Sorry not implemented" :-/
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     *  @param  dateTime      verification date time (optional). If empty/absent the current date and
     *                        time are used.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition verifyDocument(const OFString &observerName,
                               const OFString &organization,
                               const OFString &dateTime /*= ""*/);

    /** verify the current document by a specific observer.
     *  A document can be verified more than once.  The observer information is added to a
     *  sequence stored in the dataset.  The verification flag is automatically set to
     *  VERIFIED (if not already done) and the finalized flag is reset (set to OFFalse).
     *  Please note that only completed documents (see completion flag) can be verified and that
     *  a new SOP instance UID has to be generated (manually) according to the DICOM standard when
     *  creating a dataset/file from this document.
     *  Not applicable to Key Object Selection Documents.
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  observerCode  code identifying the verifying observer (optional, see previous method)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition verifyDocument(const OFString &observerName,
                               const DSRCodedEntryValue &observerCode,
                               const OFString &organization);

    /** verify the current document by a specific observer.
     *  Same as above but allows to specify the verification date time value.
     *  Only required since Sun CC 2.0.1 compiler does not support default parameter values for
     *  "complex types" like OFString.  Reports the error message: "Sorry not implemented" :-/
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  observerCode  code identifying the verifying observer (optional, see previous method)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     *  @param  dateTime      verification date time (optional). If empty/absent the current date and
     *                        time are used.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition verifyDocument(const OFString &observerName,
                               const DSRCodedEntryValue &observerCode,
                               const OFString &organization,
                               const OFString &dateTime /*= ""*/);

    /** remove verification information.
     *  The list of verifying observers is cleared, the verification flag is set to UNVERIFIED and
     *  the finalized flag is reset (set to OFFalse).
     *  Normally, there should be no need to call this method.  On the other hand, it is useful to
     *  guarantee a consistent state when processing documents which have not been created with this
     *  module/toolkit.
     */
    void removeVerification();

    /** finalize the current state of the document.
     *  A new document is originally not finalized but can be finalized using this method.
     *  This internal flag is e.g. used to indicate whether the entire document is digitally signed
     *  and, therefore, each newly added verifying observer would corrupt all previous signatures.
     *  NB: A document needs to be completed first in order to be finalized.  Some of the above
     *      document management functions do reset the flag (i.e. set the FinalizedFlag to OFFalse),
     *      other methods (e.g. setXXX) do not change the flag though the state of the document is
     *      not finalized any more after they have been called.
     *  Not applicable to Key Object Selection Documents since there's no completion flag in this
     *  type of SR document.  Please note that this method has nothing to do with the preliminary
     *  flag.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition finalizeDocument();


  protected:

    /** read XML document header
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLDocumentHeader(DSRXMLDocument &doc,
                                      DSRXMLCursor cursor,
                                      const size_t flags);

    /** read XML "patient" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLPatientData(const DSRXMLDocument &doc,
                                   DSRXMLCursor cursor,
                                   const size_t flags);

    /** read XML "study" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLStudyData(const DSRXMLDocument &doc,
                                 DSRXMLCursor cursor,
                                 const size_t flags);

    /** read XML "series" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLSeriesData(const DSRXMLDocument &doc,
                                  DSRXMLCursor cursor,
                                  const size_t flags);

    /** read XML "instance" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLInstanceData(const DSRXMLDocument &doc,
                                    DSRXMLCursor cursor,
                                    const size_t flags);

    /** read XML "document" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLDocumentData(const DSRXMLDocument &doc,
                                    DSRXMLCursor cursor,
                                    const size_t flags);

    /** read XML verifying observer data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLVerifyingObserverData(const DSRXMLDocument &doc,
                                             DSRXMLCursor cursor,
                                             const size_t flags);

    /** render patient name, sex, birthdate and ID in HTML/XHTML format
     ** @param  stream  output stream to which the HTML/XHTML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::HF_xxx)
     */
    void renderHTMLPatientData(STD_NAMESPACE ostream &stream,
                               const size_t flags);

    /** render list of referenced SOP instances in HTML/XHTML format
     ** @param  stream   output stream to which the HTML/XHTML document is written
     *  @param  refList  list of referenced SOP instances to be rendered
     *  @param  flags    flag used to customize the output (see DSRTypes::HF_xxx)
     */
    void renderHTMLReferenceList(STD_NAMESPACE ostream &stream,
                                 DSRSOPInstanceReferenceList &refList,
                                 const size_t flags);

    /** check the given dataset before reading.
     *  This methods checks whether the dataset contains at least the DICOM attributes SOP class UID
     *  and modality.  Any incorrectness regarding these two attributes is reported to the log stream
     *  (if valid).  Currently unsupported SOP classes are also reported as an error.
     ** @param  dataset       DICOM dataset to be checked
     *  @param  documentType  SR document type retrieved from the SOP class UID
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition checkDatasetForReading(DcmItem &dataset,
                                       E_DocumentType &documentType);

    /** update several DICOM attributes.
     *  (e.g. set the modality attribute, generate a new SOP instance UID if required, set date/time, etc.)
     ** @param  updateAll  flag indicating whether all DICOM attributes should be updated or only the
     *                     completion and verification flag. (set DICOM defined terms from enum values)
     */
    void updateAttributes(const OFBool updateAll = OFTrue);


  private:

    /// SR document tree
    DSRDocumentTree DocumentTree;

    /// flag indicating whether is document is finalized or not
    OFBool             FinalizedFlag;
    /// enumerated value: preliminary, final
    E_PreliminaryFlag  PreliminaryFlagEnum;
    /// enumerated value: partial, complete
    E_CompletionFlag   CompletionFlagEnum;
    /// enumerated value: unverified, verified
    E_VerificationFlag VerificationFlagEnum;
    /// defined term: see class DSRTypes
    E_CharacterSet     SpecificCharacterSetEnum;

    // DICOM attributes are listed ordered by module.
    // The comments for each attribute describe "Name: (VR, VM, Type)".
    // Please note that for particular SR documents (e.g. Key Object Selection)
    // other rules might apply.  See DICOM standard for further reference.

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
    /// Coding Scheme Identification Sequence: (SQ, 1, 3)
    DSRCodingSchemeIdentificationList CodingSchemeIdentification;

    // --- General Study Module (M) ---

    /// Study Instance UID: (UI, 1, 1)
    DcmUniqueIdentifier StudyInstanceUID;
    /// Study Date: (DA, 1, 2)
    DcmDate             StudyDate;
    /// StudyTime: (TM, 1, 2)
    DcmTime             StudyTime;
    /// ReferringPhysicianName: (PN, 1, 2)
    DcmPersonName       ReferringPhysicianName;
    /// Study ID: (SH, 1, 2)
    DcmShortString      StudyID;
    /// Accession Number: (SH, 1, 2)
    DcmShortString      AccessionNumber;
    /// Study Description: (LO, 1, 3)
    DcmLongString       StudyDescription;

    // --- Patient Module (M) ---

    /// Patient's Name: (PN, 1, 2)
    DcmPersonName       PatientName;
    /// Patient ID: (LO, 1, 2)
    DcmLongString       PatientID;
    /// Patient's Birth Date: (DA, 1, 2)
    DcmDate             PatientBirthDate;
    /// Patient's Sex: (CS, 1, 2)
    DcmCodeString       PatientSex;

    // --- General Equipment Module (M) ---

    /// Manufacturer: (LO, 1, 2)
    DcmLongString       Manufacturer;
    /// Manufacturer's Model Name: (LO, 1, 3)
    DcmLongString       ManufacturerModelName;
    /// Device Serial Number: (LO, 1, 3)
    DcmLongString       DeviceSerialNumber;
    /// Software Version(s): (LO, 1-n, 3)
    DcmLongString       SoftwareVersions;

    // --- Enhanced General Equipment Module (M - for some IODs) ---

    //  Manufacturer: (LO, 1, 1)
        //  - see 'General Equipment Module'
    /// Manufacturer's Model Name: (LO, 1, 1)
        //  - see 'General Equipment Module'
    /// Device Serial Number: (LO, 1, 1)
        //  - see 'General Equipment Module'
    /// Software Version(s): (LO, 1-n, 1)
        //  - see 'General Equipment Module'

    // --- Frame of Reference (C) ---

    // tbd: conditional module for X-Ray Radiation Dose SR not yet supported

    // --- SR Document Series Module (M) ---

    /// Modality: (CS, 1, 1)
    DcmCodeString       Modality;
    /// Series Instance Number: (UI, 1, 1)
    DcmUniqueIdentifier SeriesInstanceUID;
    /// Series Number: (IS, 1, 1)
    DcmIntegerString    SeriesNumber;
    /// Series Description: (LO, 1, 3)
    DcmLongString       SeriesDescription;
    /// Referenced Performed Procedure Step Sequence: (SQ, 1, 2)
    DcmSequenceOfItems  ReferencedPerformedProcedureStep;

    // --- SR Document General Module (M) ---

    /// Instance Number: (IS, 1, 1)
    DcmIntegerString    InstanceNumber;
    /// Preliminary Flag: (CS, 1, 3)
    DcmCodeString       PreliminaryFlag;
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
    DSRSOPInstanceReferenceList PredecessorDocuments;
    /// Identical Documents Sequence: (SQ, 1, 1C)
    DSRSOPInstanceReferenceList IdenticalDocuments;
    //  Referenced Request Sequence: (SQ, 1, 1C)
        // -- not yet supported --
    /// Performed Procedure Code Sequence: (SQ, 1, 2)
    DcmSequenceOfItems  PerformedProcedureCode;
    /// Current Requested Procedure Evidence Sequence: (SQ, 1, 1C)
    DSRSOPInstanceReferenceList CurrentRequestedProcedureEvidence;
    /// Pertinent Other Evidence Sequence: (SQ, 1, 1C)
    DSRSOPInstanceReferenceList PertinentOtherEvidence;

 // --- declaration of copy constructor and assignment operator ---

    DSRDocument(const DSRDocument &);
    DSRDocument &operator=(const DSRDocument &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoc.h,v $
 *  Revision 1.53  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.52  2010-09-30 08:49:40  joergr
 *  Fixed incomplete comment.
 *
 *  Revision 1.51  2010-09-29 15:07:06  joergr
 *  Enhanced comments on attributes from Enhanced General Equipment Module.
 *
 *  Revision 1.50  2010-09-29 10:07:12  joergr
 *  Added support for the recently introduced, optional PreliminaryFlag.
 *
 *  Revision 1.49  2010-09-28 16:26:43  joergr
 *  Added support for Enhanced General Equipment Module which is required for
 *  both X-Ray Radiation Dose SR and Colon CAD SR.
 *
 *  Revision 1.48  2010-08-09 13:26:25  joergr
 *  Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *  the official "keyword" is used for the attribute name which results in a
 *  number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *  Revision 1.47  2010-07-01 13:40:33  joergr
 *  Moved SeriesDescription (0008,103E) from General Series to SR Document Series
 *  Module (according to CP 703).
 *
 *  Revision 1.46  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.45  2009-04-21 08:32:38  joergr
 *  Fixed typo.
 *
 *  Revision 1.44  2007/11/15 16:33:30  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.43  2006/12/06 11:50:42  joergr
 *  Updated various citations according to the latest version of the DICOM
 *  standard (incl. CP 584). Removed references to a particular edition of the
 *  standard.
 *
 *  Revision 1.42  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.41  2006/07/25 13:19:53  joergr
 *  Renamed member variable "ReferencedPerformedProcedureStepSequence" to
 *  "ReferencedPerformedProcedureStep" for consistency reasons.
 *
 *  Revision 1.40  2006/06/01 15:23:14  joergr
 *  Removed explicit reference to a particular edition of the DICOM standard.
 *
 *  Revision 1.39  2006/05/11 09:18:21  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.38  2005/12/08 16:04:59  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.37  2005/11/30 12:00:24  joergr
 *  Added support for X-Ray Radiation Dose SR documents.
 *
 *  Revision 1.36  2005/07/27 16:41:55  joergr
 *  Updated reference to DICOM standard edition.
 *
 *  Revision 1.35  2004/11/22 16:39:09  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *  in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.34  2004/04/16 13:24:07  joergr
 *  Restructured code to avoid default parameter values for "complex types" like
 *  OFString. Required for Sun CC 2.0.1.
 *
 *  Revision 1.33  2004/01/05 14:36:02  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.32  2003/10/30 17:53:23  joergr
 *  Added full support for the ContentTemplateSequence (read/write, get/set
 *  template identification). Template constraints are not checked yet.
 *
 *  Revision 1.31  2003/10/09 13:18:16  joergr
 *  Added text from Supplement 66 to getCurrentRequestedProcedureEvidence() API
 *  comment.
 *
 *  Revision 1.30  2003/09/10 13:16:13  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdentificationSequence as
 *  required by CP 324.
 *
 *  Revision 1.29  2003/08/07 18:01:42  joergr
 *  Removed libxml dependency from header files.
 *
 *  Revision 1.28  2003/08/07 12:31:46  joergr
 *  Added readXML functionality.
 *  Updated documentation to get rid of doxygen warnings.
 *  Made method isFinalized() const.
 *  Added new option to createRevisedVersion() which allows to keep the current
 *  list of predecessor documents.
 *  Changed interface to access the list of predecessor documents. Now using the
 *  existing class DSRSOPInstanceReferenceList.
 *  Added preliminary support for the Identical Documents Sequence.
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
 *
 *  Revision 1.27  2002/05/14 08:15:24  joergr
 *  Updated comments.
 *
 *  Revision 1.26  2002/05/07 12:49:31  joergr
 *  Added support for the Current Requested Procedure Evidence Sequence and the
 *  Pertinent Other Evidence Sequence to the dcmsr module.
 *
 *  Revision 1.25  2002/04/16 13:50:52  joergr
 *  Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *  Revision 1.24  2001/11/09 16:10:48  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.23  2001/09/26 13:04:06  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.22  2001/04/03 08:24:01  joergr
 *  Added new command line option: ignore relationship content constraints
 *  specified for each SR document class.
 *
 *  Revision 1.21  2001/01/29 17:37:14  joergr
 *  Added methods to support a new state of finalized and unfinalized documents.
 *
 *  Revision 1.20  2001/01/25 11:48:43  joergr
 *  Corrected typos / enhanced comments.
 *
 *  Revision 1.19  2001/01/18 15:53:33  joergr
 *  Added support for digital signatures.
 *
 *  Revision 1.18  2000/12/08 13:42:40  joergr
 *  Renamed createNewSeries(studyUID) to createNewSeriesInStudy(studyUID).
 *
 *  Revision 1.17  2000/11/16 13:31:27  joergr
 *  Corrected behaviour of updateDicomAttributes().
 *
 *  Revision 1.16  2000/11/14 17:27:25  joergr
 *  Added method to remove verification information.
 *
 *  Revision 1.15  2000/11/14 16:36:21  joergr
 *  Added methods to set the content date/time.
 *
 *  Revision 1.14  2000/11/14 11:45:50  joergr
 *  Corrected behaviour of method createRevisedVersion().
 *
 *  Revision 1.13  2000/11/13 14:19:06  joergr
 *  Updated comments.
 *
 *  Revision 1.12  2000/11/13 10:26:21  joergr
 *  Added output of optional observation datetime to rendered HTML page.
 *
 *  Revision 1.11  2000/11/10 18:10:20  joergr
 *  Corrected behaviour of createNewSOPInstance() and createRevisedDocument().
 *
 *  Revision 1.10  2000/11/10 17:44:49  joergr
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
 *  Added support for conversion to XML.
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

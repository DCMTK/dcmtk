/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
 */


#ifndef DSRDOC_H
#define DSRDOC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctr.h"
#include "dcmtk/dcmsr/dsrrtpl.h"
#include "dcmtk/dcmsr/dsrsoprf.h"
#include "dcmtk/dcmsr/dsrrefin.h"
#include "dcmtk/dcmsr/dsrcsidl.h"

#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"

#include "dcmtk/ofstd/ofstream.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for 'dcmsr' (DICOM Structured Reporting Documents).
 *  This class supports reading, writing, creation, printing and rendering of DICOM
 *  Structured Reporting (SR) documents.
 *  The list of supported SOP classes is available in file "dsrtypes.h". Also see
 *  DSRTypes::E_DocumentType.
 */
class DCMTK_DCMSR_EXPORT DSRDocument
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
    virtual void clear();

    /** check whether the current internal state is valid.
     *  The SR document is valid if the corresponding document tree is valid and
     *  the SOP instance UID as well as the SOP class UID are not "empty".
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid();

    /** check whether the document is finalized.
     *  A new document is originally not finalized but can be finalized using the method
     *  finalizeDocument().  This flag is e.g. used to indicate whether the entire document
     *  is digitally signed and, therefore, each newly added verifying observer would corrupt
     *  all previous signatures.
     ** @return OFTrue if finalized, OFFalse otherwise
     */
    virtual OFBool isFinalized() const;


  // --- input and output ---

    /** print current SR document to specified output stream.
     *  The output format is identical to that of the dsrdump command line tool.
     ** @param  stream  output stream (e.g.\ COUT from "ofconsol.h")
     *  @param  flags   optional flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags = 0);

    /** read SR document from DICOM dataset.
     *  Please note that the current document is also deleted if the reading process fails.
     *  If logging is enabled, the reason for any error might be obtained from the log output.
     *  Also warning and debug messages are reported if the respective logger is enabled.
     ** @param  dataset  reference to DICOM dataset from which the document should be read
     *  @param  flags    optional flag used to customize the reading process (see
     *                   DSRTypes::RF_xxx).  E.g. DSRTypes::RF_readDigitalSignatures indicates
     *                   whether to read the digital signatures from the dataset or not.  If set,
     *                   the MACParametersSequence and the DigitalSignaturesSequence are read for
     *                   the general document header (equivalent to top-level content item) and
     *                   each content item of the document tree.
     *                   If not removed manually (with DSRDocumentTree::removeSignatures()),
     *                   the signatures are written back to the dataset when the write() method
     *                   is called.
     *                   Please note that the two signature sequences for any other sequence
     *                   (e.g. VerifyingObserver or PredecessorDocuments) are never read.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem &dataset,
                             const size_t flags = 0);

    /** read patient data from DICOM dataset.
     *  The list of data elements that are read can be found under "Patient Module" in the
     *  member variable section of this class.  Other data is not changed, so be careful
     *  when using this method.
     *  @param  dataset  reference to DICOM dataset from which the data should be read
     *  @param  flags    optional flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readPatientData(DcmItem &dataset,
                                        const size_t flags = 0);

    /** read study data from DICOM dataset. Also reads patient data.
     *  The list of data elements that are read can be found under "Patient Module" and
     *  "General Study Module" in the member variable section of this class.  Other data
     *  is not changed, so be careful when using this method.
     *  @param  dataset  reference to DICOM dataset from which the data should be read
     *  @param  flags    optional flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readStudyData(DcmItem &dataset,
                                      const size_t flags = 0);

    /** write current SR document to DICOM dataset.
     *  Please note that the ContentTemplateSequence for the root content item is not written
     *  automatically for particular SOP Classes (e.g. Key Object Selection Document).
     *  Instead, the template identification has to be set manually for the root CONTAINER
     *  (see DSRDocumentTreeNode::setTemplateIdentification()).  This is because the template
     *  constraints cannot be checked yet.
     *  If logging is enabled, warning and debug messages are reported to the log output.
     ** @param  dataset      reference to DICOM dataset to which the current document should be
     *                       written.  The 'dataset' is not cleared before writing to it!
     *  @param  markedItems  optional stack where pointers to all 'marked' content items
     *                       (DICOM datasets/items) are added to during the write process.
     *                       Can be used to digitally sign parts of the document tree.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmItem &dataset,
                              DcmStack *markedItems = NULL);

    /** read SR document from XML file.
     *  The format (Schema) of the XML document is expected to conform to the output format
     *  of the writeXML() method.  In addition, the document can be validated against an XML
     *  Schema by setting the flag DSRTypes::XF_validateSchema.
     *  Digital signatures in the XML document are not yet supported.
     *  Please note that the current document is also deleted if the parsing process fails.
     ** @param  filename  name of the file from which the XML document is read ("-" for stdin)
     *  @param  flags     optional flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const OFString &filename,
                                const size_t flags = 0);

    /** write current SR document in XML format.
     *  The output format is identical to that of the dsr2xml command line tool.  Digital
     *  signatures in the XML document are not yet supported.
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   optional flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags = 0);

    /** render current SR document in HTML/XHTML format.
     *  The output format is identical to that of the dsr2html command line tool.
     ** @param  stream      output stream to which the HTML/XHTML document is written
     *  @param  flags       optional flag used to customize the output (see DSRTypes::HF_xxx)
     *  @param  styleSheet  optional filename/URL of a Cascading Style Sheet (CSS)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &stream,
                                   const size_t flags = 0,
                                   const char *styleSheet = NULL);


  // --- get/set misc attributes ---

    /** get the current SR document type
     ** @return document type (might be DSRTypes::DT_invalid if read from dataset)
     */
    virtual E_DocumentType getDocumentType() const;

    /** get document tree
     ** @return reference to the document tree
     */
    inline DSRDocumentTree &getTree()
    {
        return DocumentTree;
    }

    /** set document tree.
     *  Replace the currently stored document tree with the given one.  Please note that the
     *  given 'tree' is checked before setting it, i.e. only a valid document tree is accepted.
     *  However, a new SOP instance is never created.  If needed, this has to be done with
     *  createNewSOPInstance() in addition to or with createNewDocument() before this method
     *  is called.
     ** @param  tree  document tree to be set (content will be copied)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTree(const DSRDocumentTree &tree);

    /** set document tree from root template.
     *  Replace the currently stored document tree with the one from the given root template.
     *  By default, this method expands the tree, i.e. instances of DSRIncludedTemplateTreeNode
     *  that were added to the tree with DSRDocumentSubTree::includeTemplate() are replaced by
     *  their content, i.e. by the internally managed subtree.
     *  Please note that the additional comments on the above setTree() method also apply.
     ** @param  rootTemplate  template specifying the document tree to be set (content will be
     *                        copied).  This parameter cannot be "const" because of
     *                        DSRRootTemplate::getTree(), which is called internally and which
     *                        modifies the tree.
     *  @param  expandTree    optional flag that allows for disabling the expanding of the
     *                        document tree before setting it.  Please note that various
     *                        output methods like write() or renderHTML() do not yet work on
     *                        such trees.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTreeFromRootTemplate(DSRRootTemplate &rootTemplate,
                                                const OFBool expandTree = OFTrue);

    /** get specific character set type.
     *  If the type is unknown, the original DICOM defined term can be retrieved
     *  with the method getSpecificCharacterSet().
     ** @return character set type (might be DSRTypes::CS_invalid or DSRTypes::CS_unknown)
     */
    virtual E_CharacterSet getSpecificCharacterSetType() const;

    /** set specific character set type.
     *  The DICOM defined term (see member variable SpecificCharacterSet) is set accordingly.
     ** @param  characterSet  specific character set to be set (use DSRTypes::CS_invalid to reset
     *                        to the default value, which is "unspecified")
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSpecificCharacterSetType(const E_CharacterSet characterSet);

    /** get document preliminary flag.
     *  @note Not applicable to Key Object Selection Documents.
     ** @return preliminary flag (might be DSRTypes::PF_invalid if not specified)
     */
    virtual E_PreliminaryFlag getPreliminaryFlag() const;

    /** set document preliminary flag.
     *  According to the DICOM standard, the concept of "completeness" is independent of the
     *  concept of "preliminary" or "final".  Therefore, this flag can be specified separately.
     *  @note Not applicable to Key Object Selection Documents.
     ** @param  flag  preliminary flag to be set (use DSRTypes::PF_invalid to omit this optional
     *                value)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPreliminaryFlag(const E_PreliminaryFlag flag);

    /** get document completion flag.
     *  According to the DICOM standard, this flag describes the estimated degree of completeness
     *  of an SR Document.  See DICOM standard for details.
     *  @note Not applicable to Key Object Selection Documents.
     ** @return completion flag (might be DSRTypes::CF_invalid if read from dataset)
     */
    virtual E_CompletionFlag getCompletionFlag() const;

    /** get document verification flag.
     *  @note Not applicable to Key Object Selection Documents.
     ** @return verification flag (might be DSRTypes::VF_invalid if read from dataset)
     */
    virtual E_VerificationFlag getVerificationFlag() const;

    /** check whether there are one or more verifying observers.
     *  @note Not applicable to Key Object Selection Documents.
     ** @return OFTrue if there is at least one verifying observer, OFFalse otherwise
     */
    virtual OFBool hasVerifyingObservers() const;

    /** get number of verifying observers.
     *  A document can be verified more than once.  The verification flag should be VERIFIED
     *  if any verifying observer is specified.  The details on the observer can be retrieved
     *  using the getVerifyingObserver() methods.
     *  @note Not applicable to Key Object Selection Documents.
     ** @return number of verifying observers (if any), 0 otherwise
     */
    virtual size_t getNumberOfVerifyingObservers() const;

    /** get information about a verifying observer.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain valid (empty) data.
     *  @note Not applicable to Key Object Selection Documents.
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
    virtual OFCondition getVerifyingObserver(const size_t idx,
                                             OFString &dateTime,
                                             OFString &observerName,
                                             OFString &organization);

    /** get information about a verifying observer.
     *  All reference variables are cleared before the information is retrieved, i.e. if an error
     *  occurs (return value != EC_Normal) non-empty variables do contain valid (empty) data.
     *  @note Not applicable to Key Object Selection Documents.
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
    virtual OFCondition getVerifyingObserver(const size_t idx,
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
     *  @note Not applicable to Key Object Selection Documents.
     ** @return reference to list object
     */
    virtual DSRSOPInstanceReferenceList &getPredecessorDocuments();

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
    virtual DSRSOPInstanceReferenceList &getIdenticalDocuments();

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
    virtual DSRSOPInstanceReferenceList &getCurrentRequestedProcedureEvidence();

    /** get list of referenced SOP instances (Pertinent Other Evidence).
     *  The DICOM standard states: "The Pertinent Other Evidence Sequence attribute is used to
     *  reference all other evidence considered pertinent for this SR Document that is not listed
     *  in the Current Requested Procedure Evidence Sequence.  This requires that the same SOP
     *  Instance shall not be referenced in both of these Sequences."
     *  @note Not applicable to Key Object Selection Documents.
     ** @return reference to list object
     */
    virtual DSRSOPInstanceReferenceList &getPertinentOtherEvidence();

    /** get list of referenced SOP instances significantly related to the current SOP instance.
     *  The DICOM standard states: "Such referenced Instances may include equivalent documents or
     *  renderings of this document. [...] Required if the identity of a CDA Document equivalent
     *  to the current SOP Instance is known at the time of creation of this SOP instance. May be
     *  present otherwise."  The Purpose of Reference Code should be taken from Defined Context
     *  Group 7006 (SR Document Purposes of Reference).
     *  Note: An equivalent rendering of the document might be provided as an "Encapsulated PDF"
     *  DICOM object.
     *  @note Not applicable to Key Object Selection Documents.
     ** @return reference to list object
     */
    virtual DSRReferencedInstanceList &getReferencedInstances();

    /** get list of coding schemes used (Coding Scheme Identification).
     *  The Coding Scheme Identification Sequence maps Coding Scheme Designators to an external coding
     *  system registration, or to a private or local coding scheme.  See DICOM standard for details.
     ** @return reference to list object
     */
    virtual DSRCodingSchemeIdentificationList &getCodingSchemeIdentification();


  // --- get DICOM string attributes ---

    /** get specific character set
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSpecificCharacterSet(OFString &value,
                                                const signed long pos = 0) const;

    /** get completion flag description.
     *  @note Not applicable to Key Object Selection Documents.
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getCompletionFlagDescription(OFString &value,
                                                     const signed long pos = 0) const;

    /** get modality
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getModality(OFString &value,
                                    const signed long pos = 0) const;

    /** get SOP class UID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSOPClassUID(OFString &value,
                                       const signed long pos = 0) const;

    /** get study instance UID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyInstanceUID(OFString &value,
                                            const signed long pos = 0) const;

    /** get series instance UID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesInstanceUID(OFString &value,
                                             const signed long pos = 0) const;

    /** get SOP instance UID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSOPInstanceUID(OFString &value,
                                          const signed long pos = 0) const;

    /** get instance creator UID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceCreatorUID(OFString &value,
                                              const signed long pos = 0) const;

    /** get timezone offset from UTC
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTimezoneOffsetFromUTC(OFString &value,
                                                 const signed long pos = 0) const;

    /** get patient's name
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientName(OFString &value,
                                       const signed long pos = 0) const;

    /** get patient's birth date
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientBirthDate(OFString &value,
                                            const signed long pos = 0) const;

    /** get patient's sex
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientSex(OFString &value,
                                      const signed long pos = 0) const;

    /** get referring physician's name
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getReferringPhysicianName(OFString &value,
                                                  const signed long pos = 0) const;

    /** get study description
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyDescription(OFString &value,
                                            const signed long pos = 0) const;

    /** get series description
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesDescription(OFString &value,
                                             const signed long pos = 0) const;

    /** get protocol name
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getProtocolName(OFString &value,
                                        const signed long pos = 0) const;

    /** get manufacturer
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturer(OFString &value,
                                        const signed long pos = 0) const;

    /** get manufacturer's model name
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getManufacturerModelName(OFString &value,
                                                 const signed long pos = 0) const;

    /** get device serial number
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDeviceSerialNumber(OFString &value,
                                              const signed long pos = 0) const;

    /** get software version(s)
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSoftwareVersions(OFString &value,
                                            const signed long pos = 0) const;

    /** get synchronization frame of reference UID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSynchronizationFrameOfReferenceUID(OFString &value,
                                                              const signed long pos = 0) const;

    /** get synchronization trigger
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSynchronizationTrigger(OFString &value,
                                                  const signed long pos = 0) const;

    /** get acquisition time synchronized
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAcquisitionTimeSynchronized(OFString &value,
                                                       const signed long pos = 0) const;

    /** get study date
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyDate(OFString &value,
                                     const signed long pos = 0) const;

    /** get study time
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyTime(OFString &value,
                                     const signed long pos = 0) const;

    /** get series date
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesDate(OFString &value,
                                      const signed long pos = 0) const;

    /** get series time
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesTime(OFString &value,
                                      const signed long pos = 0) const;

    /** get instance creation date
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceCreationDate(OFString &value,
                                                const signed long pos = 0) const;

    /** get instance creation time
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceCreationTime(OFString &value,
                                                const signed long pos = 0) const;

    /** get content date
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContentDate(OFString &value,
                                       const signed long pos = 0) const;

    /** get content time
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getContentTime(OFString &value,
                                       const signed long pos = 0) const;

    /** get study ID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getStudyID(OFString &value,
                                   const signed long pos = 0) const;

    /** get patient ID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPatientID(OFString &value,
                                     const signed long pos = 0) const;

    /** get issuer of patient ID
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getIssuerOfPatientID(OFString &value,
                                             const signed long pos = 0) const;

    /** get series number
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSeriesNumber(OFString &value,
                                        const signed long pos = 0) const;

    /** get instance number
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getInstanceNumber(OFString &value,
                                          const signed long pos = 0) const;

    /** get accession number
     ** @param  value  reference to variable in which the value should be stored
     *  @param  pos    index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getAccessionNumber(OFString &value,
                                           const signed long pos = 0) const;


  // --- set DICOM string attributes ---

    /** set specific character set.  The internal enumerated value is set accordingly.
     *  Please note that this method does not return an error if the given 'value' is not
     *  defined by the DICOM standard or not supported by this class, e.g. when no mapping
     *  to the character set names needed for HTML/XHTML or XML output is defined.
     *  If needed, check the return value of the method getSpecificCharacterSetType() for
     *  DSRTypes::CS_invalid or DSRTypes::CS_unknown after calling this method.
     ** @param  value  value to be set (single or multiple values) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1-n) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSpecificCharacterSet(const OFString &value,
                                                const OFBool check = OFTrue);

    /** set completion flag description.
     *  @note Not applicable to Key Object Selection Documents.
     ** @param  value  explanation of the value that is set for completion flag.  If an empty
     *                 string is passed, the description is removed from the dataset (type 3).
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setCompletionFlagDescription(const OFString &value,
                                                     const OFBool check = OFTrue);

    /** set timezone offset from UTC
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled.
     *                 Please note that it is not checked whether the 'value' conforms
     *                 to the requirements of a valid timezone offset (see DICOM PS3.3).
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setTimezoneOffsetFromUTC(const OFString &value,
                                                 const OFBool check = OFTrue);

    /** set patient's name
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientName(const OFString &value,
                                       const OFBool check = OFTrue);

    /** set patient's birth date
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientBirthDate(const OFString &value,
                                            const OFBool check = OFTrue);

    /** set patient's sex
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientSex(const OFString &value,
                                      const OFBool check = OFTrue);

    /** set referring physician's name
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (PN) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setReferringPhysicianName(const OFString &value,
                                                  const OFBool check = OFTrue);

    /** set study description
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyDescription(const OFString &value,
                                            const OFBool check = OFTrue);

    /** set series description
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesDescription(const OFString &value,
                                             const OFBool check = OFTrue);

    /** set protocol name
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setProtocolName(const OFString &value,
                                        const OFBool check = OFTrue);

    /** set manufacturer
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturer(const OFString &value,
                                        const OFBool check = OFTrue);

    /** set manufacturer's model name
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setManufacturerModelName(const OFString &value,
                                                 const OFBool check = OFTrue);

    /** set device serial number
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDeviceSerialNumber(const OFString &value,
                                              const OFBool check = OFTrue);

    /** set software version(s)
     ** @param  value  value to be set (possibly multi-valued) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1-n) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSoftwareVersions(const OFString &value,
                                            const OFBool check = OFTrue);

    /** set synchronization frame of reference UID
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSynchronizationFrameOfReferenceUID(const OFString &value,
                                                              const OFBool check = OFTrue);
    /** set synchronization trigger
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSynchronizationTrigger(const OFString &value,
                                                  const OFBool check = OFTrue);

    /** set acquisition time synchronized
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAcquisitionTimeSynchronized(const OFString &value,
                                                       const OFBool check = OFTrue);

    /** set content date
     ** @param  value  value to be set (single value only).  If an empty string is passed,
     *                 the current date is set when displaying or writing the document since
     *                 the corresponding DICOM attribute is mandatory.
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContentDate(const OFString &value,
                                       const OFBool check = OFTrue);

    /** set content time
     ** @param  value  value to be set (single value only).  If an empty string is passed,
     *                 the current time is set when displaying or writing the document since
     *                 the corresponding DICOM attribute is mandatory.
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setContentTime(const OFString &value,
                                       const OFBool check = OFTrue);

    /** set study date
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyDate(const OFString &value,
                                     const OFBool check = OFTrue);

    /** set study time
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyTime(const OFString &value,
                                     const OFBool check = OFTrue);

    /** set series date
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (DA) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesDate(const OFString &value,
                                      const OFBool check = OFTrue);

    /** set series time
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (TM) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesTime(const OFString &value,
                                      const OFBool check = OFTrue);

    /** set study ID
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setStudyID(const OFString &value,
                                   const OFBool check = OFTrue);

    /** set patient ID
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setPatientID(const OFString &value,
                                     const OFBool check = OFTrue);

    /** set issuer of patient ID
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setIssuerOfPatientID(const OFString &value,
                                             const OFBool check = OFTrue);

    /** set series number
     ** @param  value  value to be set (single value only).  If an empty string is passed,
     *                 the value "1" is set when displaying or writing the document since
     *                 the corresponding DICOM attribute is mandatory.
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setSeriesNumber(const OFString &value,
                                        const OFBool check = OFTrue);

    /** set instance number
     ** @param  value  value to be set (single value only).  If an empty string is passed,
     *                 the value "1" is set when displaying or writing the document since
     *                 the corresponding DICOM attribute is mandatory.
     *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setInstanceNumber(const OFString &value,
                                          const OFBool check = OFTrue);

    /** set accession number
     ** @param  value  value to be set (single value only) or "" for no value
     *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setAccessionNumber(const OFString &value,
                                           const OFBool check = OFTrue);


  // --- document management functions ---

    /** create new study.
     *  After generating a new study instance UID the method createNewSeries() is called,
     *  i.e. also a new series instance UID and SOP instance UID are generated.  This is
     *  a requirement of the DICOM standard.  All other study-related attributes are
     *  cleared.
     */
    virtual void createNewStudy();

    /** create a new series.
     *  After generating a new series instance UID the method createNewSOPInstance() is
     *  called, i.e. also a new SOP instance UID is generated.  This is a requirement of
     *  the DICOM standard.  All other series-related attributes are cleared.
     */
    virtual void createNewSeries();

    /** create a new series within a given study.
     *  After generating a new series instance UID within the given study the method
     *  createNewSOPInstance() is called, i.e. also a SOP instance UID is generated.
     *  This is a requirement of the DICOM standard.
     *  NB: There is no mechanism that makes sure that the study-related attributes are
     *      consistent for all series of a study.  This either has to be done manually
     *      or readStudyData() should be used in combination with createNewSeries().
     ** @param  studyUID  study instance UID to be set (should be a valid UID)
     *  @param  check     check 'studyUID' for conformance with VR and VM if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createNewSeriesInStudy(const OFString &studyUID,
                                               const OFBool check = OFTrue);

    /** create a new SOP instance.
     *  Generate a new SOP instance UID, set the instance creation date/time and reset the
     *  finalized flag (OFFalse).
     *  This method is used internally for createNewDocument(), createRevisedVersion()
     *  and during object initialization.
     *  It could also be used explicitly from the calling application if a new UID should
     *  be created (this is the case if the study instance UID or series instance UID has
     *  changed as well as any other attribute within the SR Document General Module or
     *  SR Document Content Module, see DICOM standard for details).
     *  This method also updates other DICOM header attributes (by calling updateAttributes()).
     *  However, instance-level attributes other than the SOP instance UID are usually not
     *  changed (if already set), e.g. the current instance number or content date/time.
     */
    virtual void createNewSOPInstance();

    /** create a new document.
     *  A new SOP instance is only created if the current document type was valid/supported.
     *  Please note that the current document is deleted (cleared).
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createNewDocument();

    /** create a new document of the specified type.
     *  A new SOP instance is only created if the current document type was valid/supported.
     *  Please note that the current document is deleted by this method.
     ** @param  documentType  type of the new SR document (see DSRTypes::E_DocumentType)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createNewDocument(const E_DocumentType documentType);

    /** change the type of the current document.
     *  Please note that only the type of the SR document is changed (if possible) but no new
     *  SOP instance is created.  If needed, this has to be done with createNewSOPInstance()
     *  in addition to this method or by calling createNewDocument() instead of this method.
     *  Before changing the document type, it is checked whether the currently stored document
     *  tree complies with the relationship content constraints of the new SR IOD.
     ** @param  documentType  new type of the SR document (see DSRTypes::E_DocumentType)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition changeDocumentType(const E_DocumentType documentType);

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
     *  reset (OFFalse).  The preliminary flag is not modified by this method.  Also the
     *  various lists of referenced instances remain unchanged, i.e. they have to be
     *  adapted manually if needed.
     *  @note Not applicable to Key Object Selection Documents.
     *  @param  clearList  clear list of predecessor documents before adding the current
     *                     document if OFTrue.  Append current document to existing list
     *                     otherwise.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createRevisedVersion(const OFBool clearList = OFTrue);

    /** complete the current document.
     *  Sets the completion flag to COMPLETE if not already done (fails otherwise).  This flag
     *  describes the estimated degree of completeness of an SR Document (see DICOM standard
     *  for details).  The completion flag description is set to an empty string (i.e. absent
     *  in DICOM dataset).
     *  @note Not applicable to Key Object Selection Documents.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition completeDocument();

    /** complete the current document and set a completion description.
     *  Sets the completion flag to COMPLETE if not already done (fails otherwise).  This flag
     *  describes the estimated degree of completeness of an SR Document (see DICOM standard
     *  for details).  The completion flag description can be modified independently from the
     *  flag by means of the method setCompletionFlagDescription() - see above.
     *  @note Not applicable to Key Object Selection Documents.
     ** @param  description  explanation of the value set for completion flag.
     *                       (optional, see previous method, VR=LO)
     *  @param  check        check 'description' for conformance with VR and VM if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition completeDocument(const OFString &description,
                                         const OFBool check = OFTrue);

    /** verify the current document by a specific observer.
     *  A document can be verified more than once.  The observer information is added to a
     *  sequence stored in the dataset.  The verification flag is automatically set to
     *  VERIFIED (if not already done) and the finalized flag is reset (set to OFFalse).
     *  Please note that only completed documents (see completion flag) can be verified and that
     *  a new SOP instance UID has to be generated (manually) according to the DICOM standard when
     *  creating a dataset/file from this document.
     *  @note Not applicable to Key Object Selection Documents.
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     *  @param  dateTime      verification date time (optional). If empty/absent the current date and
     *                        time are used.
     *  @param  check         check 'observerName', 'organization' and 'dateTime' for conformance with
     *                        VR and VM if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition verifyDocument(const OFString &observerName,
                                       const OFString &organization,
                                       const OFString &dateTime = "",
                                       const OFBool check = OFTrue);

    /** verify the current document by a specific observer.
     *  A document can be verified more than once.  The observer information is added to a
     *  sequence stored in the dataset.  The verification flag is automatically set to
     *  VERIFIED (if not already done) and the finalized flag is reset (set to OFFalse).
     *  Please note that only completed documents (see completion flag) can be verified and that
     *  a new SOP instance UID has to be generated (manually) according to the DICOM standard when
     *  creating a dataset/file from this document.
     *  @note Not applicable to Key Object Selection Documents.
     ** @param  observerName  name of the person who has verified this document (required, VR=PN)
     *  @param  observerCode  code identifying the verifying observer (optional, see previous method)
     *  @param  organization  name of the organization to which the observer belongs (required, VR=LO)
     *  @param  dateTime      verification date time (optional). If empty/absent the current date and
     *                        time are used.
     *  @param  check         check 'observerName', 'observerCode', 'organization' and 'dateTime' for
     *                        conformance with VR and VM if enabled
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition verifyDocument(const OFString &observerName,
                                       const DSRCodedEntryValue &observerCode,
                                       const OFString &organization,
                                       const OFString &dateTime = "",
                                       const OFBool check = OFTrue);

    /** remove verification information.
     *  The list of verifying observers is cleared, the verification flag is set to UNVERIFIED and
     *  the finalized flag is reset (set to OFFalse).
     *  Normally, there should be no need to call this method.  On the other hand, it is useful to
     *  guarantee a consistent state when processing documents which have not been created with this
     *  module/toolkit.
     */
    virtual void removeVerification();

    /** finalize the current state of the document.
     *  A new document is originally not finalized but can be finalized using this method.
     *  This internal flag is e.g. used to indicate whether the entire document is digitally signed
     *  and, therefore, each newly added verifying observer would corrupt all previous signatures.
     *  NB: A document needs to be completed first in order to be finalized.  Some of the above
     *      document management functions do reset the flag (i.e. set the FinalizedFlag to OFFalse),
     *      other methods (e.g. setXXX) do not change the flag though the state of the document is
     *      not finalized any more after they have been called.
     *  @note Not applicable to Key Object Selection Documents since there's no completion flag in
     *        this type of SR document.  Please note that this method has nothing to do with the
     *        preliminary flag.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition finalizeDocument();


  protected:

    /** read XML document header
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLDocumentHeader(DSRXMLDocument &doc,
                                      DSRXMLCursor cursor,
                                      const size_t flags);

    /** read XML "patient" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLPatientData(const DSRXMLDocument &doc,
                                   DSRXMLCursor cursor,
                                   const size_t flags);

    /** read XML "study" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLStudyData(const DSRXMLDocument &doc,
                                 DSRXMLCursor cursor,
                                 const size_t flags);

    /** read XML "series" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLSeriesData(const DSRXMLDocument &doc,
                                  DSRXMLCursor cursor,
                                  const size_t flags);

    /** read XML "instance" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLInstanceData(const DSRXMLDocument &doc,
                                    DSRXMLCursor cursor,
                                    const size_t flags);

    /** read XML "document" data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readXMLDocumentData(const DSRXMLDocument &doc,
                                    DSRXMLCursor cursor,
                                    const size_t flags);

    /** read XML verifying observer data
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
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

    /** render list of referenced SOP instances in HTML/XHTML format
     ** @param  stream   output stream to which the HTML/XHTML document is written
     *  @param  refList  list of referenced SOP instances to be rendered
     *  @param  flags    flag used to customize the output (see DSRTypes::HF_xxx)
     */
    void renderHTMLReferenceList(STD_NAMESPACE ostream &stream,
                                 DSRReferencedInstanceList &refList,
                                 const size_t flags);

    /** check the given dataset before reading.
     *  This methods checks whether the dataset contains at least the DICOM attributes SOP class UID
     *  and modality.  Any incorrectness regarding these two attributes is reported to the logger.
     *  Currently unsupported SOP classes are also reported as an error.
     ** @param  dataset       DICOM dataset to be checked
     *  @param  documentType  SR document type retrieved from the SOP class UID
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition checkDatasetForReading(DcmItem &dataset,
                                       E_DocumentType &documentType);

    /** get current value of specific character set
     ** @return pointer to character string (never NULL)
     */
    const char *getSpecificCharacterSet() const;

    /** update various DICOM attributes.
     *  (e.g. set the modality and SOP class UID, generate a new Study, Series and SOP instance UID
     *  if required, set date/time values, etc.)
     ** @param  updateAll   flag indicating whether all DICOM attributes should be updated or only
     *                      the IOD-specific ones. (e.g. set DICOM defined terms from enum values)
     *  @param  verboseMode report (more) processing details to the logger if enabled (default)
     */
    void updateAttributes(const OFBool updateAll = OFTrue,
                          const OFBool verboseMode = OFTrue);


  private:

    /// SR document tree
    DSRDocumentTree DocumentTree;

    /// flag indicating whether this document is finalized or not
    OFBool             FinalizedFlag;
    /// enumerated value: preliminary, final
    E_PreliminaryFlag  PreliminaryFlagEnum;
    /// enumerated value: partial, complete
    E_CompletionFlag   CompletionFlagEnum;
    /// enumerated value: unverified, verified
    E_VerificationFlag VerificationFlagEnum;
    /// defined term: see class DSRTypes
    E_CharacterSet     SpecificCharacterSetEnum;

    // DICOM attributes are listed ordered by Module.
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
    /// Coding Scheme Identification Sequence: (SQ, 1-n, 3)
    DSRCodingSchemeIdentificationList CodingSchemeIdentification;
    /// Context Group Identification Sequence: (SQ, 1-n, 3)
     // - tbd: optional attribute not yet supported
    /// Mapping Resource Identification Sequence: (SQ, 1-n, 3)
     // - tbd: optional attribute not yet supported
    /// Timezone Offset from UTC: (SH, 1, 3)
    DcmShortString      TimezoneOffsetFromUTC;

    // --- General Study Module (M) ---

    /// Study Instance UID: (UI, 1, 1)
    DcmUniqueIdentifier StudyInstanceUID;
    /// Study Date: (DA, 1, 2)
    DcmDate             StudyDate;
    /// Study Time: (TM, 1, 2)
    DcmTime             StudyTime;
    /// Referring Physician's Name: (PN, 1, 2)
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
    /// Issuer of Patient ID: (LO, 1, 3)
    DcmLongString       IssuerOfPatientID;
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

    // Manufacturer: (LO, 1, 1)
    // - see 'General Equipment Module'
    // Manufacturer's Model Name: (LO, 1, 1)
    // - see 'General Equipment Module'
    // Device Serial Number: (LO, 1, 1)
    // - see 'General Equipment Module'
    // Software Version(s): (LO, 1-n, 1)
    // - see 'General Equipment Module'

    // --- Synchronization Module (M/C/U - for some IODs) ---

    /// Synchronization Frame of Reference UID: (UI, 1, 1)
    DcmUniqueIdentifier SynchronizationFrameOfReferenceUID;
    /// Synchronization Trigger: (CS, 1, 1)
    DcmCodeString       SynchronizationTrigger;
    /// Acquisition Time Synchronized: (CS, 1, 1)
    DcmCodeString       AcquisitionTimeSynchronized;

    // --- SR Document Series / Key Object Document Series Module (M) ---

    /// Modality: (CS, 1, 1)
    DcmCodeString       Modality;
    /// Series Instance Number: (UI, 1, 1)
    DcmUniqueIdentifier SeriesInstanceUID;
    /// Series Number: (IS, 1, 1)
    DcmIntegerString    SeriesNumber;
    /// Series Date: (DA, 1, 3)
    DcmDate             SeriesDate;
    /// Series Time: (TM, 1, 3)
    DcmTime             SeriesTime;
    /// Protocol Name: (LO, 1, 3)
    DcmLongString       ProtocolName;
    /// Series Description: (LO, 1, 3)
    DcmLongString       SeriesDescription;
    /// Series Description Code Sequence: (SQ, 1, 3)
     // - tbd: optional attribute not yet supported
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
    /// Verifying Observer Sequence: (SQ, 1-n, 1C)
    DcmSequenceOfItems  VerifyingObserver;
    /// Author Observer Sequence: (SQ, 1-n, 3)
     // - tbd: optional attribute not yet supported
    /// Participant Sequence: (SQ, 1-n, 3)
     // - tbd: optional attribute not yet supported
    /// Custodial Organization Sequence: (SQ, 1, 3)
     // - tbd: optional attribute not yet supported
    /// Predecessor Documents Sequence: (SQ, 1-n, 1C)
    DSRSOPInstanceReferenceList PredecessorDocuments;
    /// Identical Documents Sequence: (SQ, 1-n, 1C)
    DSRSOPInstanceReferenceList IdenticalDocuments;
    // Referenced Request Sequence: (SQ, 1-n, 1C)
     // - tbd: conditional attribute not yet supported
    /// Performed Procedure Code Sequence: (SQ, 1-n, 2)
    DcmSequenceOfItems  PerformedProcedureCode;
    /// Current Requested Procedure Evidence Sequence: (SQ, 1-n, 1C)
    DSRSOPInstanceReferenceList CurrentRequestedProcedureEvidence;
    /// Pertinent Other Evidence Sequence: (SQ, 1-n, 1C)
    DSRSOPInstanceReferenceList PertinentOtherEvidence;
    /// Referenced Instance Sequence: (SQ, 1-n, 1C)
    DSRReferencedInstanceList ReferencedInstances;

    // --- Timezone Module (M - for some IODs) ---

    // Timezone Offset from UTC: (SH, 1, 1)
    // - see 'SOP Common Module'

 // --- declaration of copy constructor and assignment operator ---

    DSRDocument(const DSRDocument &);
    DSRDocument &operator=(const DSRDocument &);
};


#endif

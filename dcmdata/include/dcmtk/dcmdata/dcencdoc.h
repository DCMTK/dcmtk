/*
 *
 *  Copyright (C) 2018-2025, OFFIS e.V.
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
 *  Author:  Pedro Arizpe, Marco Eichelberg
 *
 *  Purpose: Class to control document encapsulation into DICOM files
 *
 */

#ifndef DCENCDOC_H
#define DCENCDOC_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */
#include "dcmtk/ofstd/ofcond.h"       /* for class OFCondition */
#include "dcmtk/dcmdata/dcdefine.h"   /* for DCMTK_DCMDATA_EXPORT */
#include "dcmtk/dcmdata/dcfilefo.h"   /* for class DcmFileFormat */
#include "dcmtk/ofstd/ofcmdln.h"      /* for OFCmdUnsignedInt */

class OFCommandLine;
class OFConsoleApplication;
class DcmItem;

/** This class handles common functions for the conversion of
 *  documents into encapsulated DICOM SOP instances.
 */
class DCMTK_DCMDATA_EXPORT DcmEncapsulatedDocument
{
public:

    /// document type of encapsulated document
    enum DocumentType
    {
        /// PDF document
        DT_pdfDocument,
        /// CDA document
        DT_cdaDocument,
        /// STL document
        DT_stlDocument,
        /// MTL document
        DT_mtlDocument,
        /// OBJ document
        DT_objDocument,
        /// unknown document
        DT_unknownDocument,
    };

    ///Constructor
    DcmEncapsulatedDocument();

    ///Destructor
    ~DcmEncapsulatedDocument();

    /** Add CDA specific command line options to the OFCommandLine object
     *  passed to the constructor.
     *  @param cmd a reference to an OFCommandLine object used to parse
     *    the command line argument give to the calling application.
     *  @return none.
     */
    void addCommandlineOptions(OFCommandLine& cmd) const;

    /** Parse and evaluate the given command line arguments.
     *  @param app a reference to an OFConsoleApplication object used in the
     *    calling application.
     *  @param cmd a reference to an OFCommandLine object used to parse
     *    the command line argument give to the calling application.
     *  @return none.
     */
    void parseArguments(OFConsoleApplication& app, OFCommandLine& cmd);

    /** Identify document format and insert encapsulated document data
     *  into the dataset
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition insertEncapsulatedDocument();

    /** Get study or series data from series file. Generate UID if none present.
     *  @note This method should be called after insertEncapsulatedDocument()
     *  to make sure that the document format has been identified.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition createIdentifiers();

    /** Perform format specific processing such as extracting information
     *  from the document content.
     *  @note This method should be called after insertEncapsulatedDocument()
     *    and after createIdentifiers() to make sure that the document format
     *    has been identified and inconsistencies between DICOM and document
     *    level can be checked.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition formatSpecificProcessing();

    /** Write header fields to the DICOM dataset
     *  @note This method should be called after insertEncapsulatedDocument()
     *  to make sure that the document format has been identified.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition createHeader();

    /** Copy override keys over existing keys in given dataset.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition applyOverrideKeys();

    /** Returns the input file name.
     *  @return the input file name as OFString.
     */
    OFString getInputFileName();

    /** Returns the output file name.
     *  @return the output file name as OFString.
     */
    OFString getOutputFileName();

    /** Save the output file.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition saveFile();

private:

    /** Retrieve patient, concept and document data from a CDA file and check for conflicts
     *  with series, study and user input. Also retrieve all mediatypes found in the CDA document.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition getCDAData();

    /** Add mandatory Frame of Reference Module attributes to dataset.
     *  @param dataset output dataset
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition addFrameOfReferenceModule(DcmItem *dataset);

    /** Add mandatory Enhanced General Equipment Module attributes to dataset.
     *  @param dataset output dataset
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition addEnhancedGeneralEquipmentModule(DcmItem *dataset);

    /** Add mandatory Manufacturing 3D Model Module attributes to dataset.
     *  @param dataset output dataset
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition addManufacturing3DModelModule(DcmItem *dataset);

    ///input file name
    OFString             ifname_;

    ///output file name
    OFString             ofname_;

    /// patient's birth date
    OFString             patientBirthdate_;

    /// patient ID
    OFString             patientID_;

    /// patient name
    OFString             patientName_;

    /// patient's sex
    OFString             patientSex_;

    /// document title: code meaning
    OFString             conceptCM_;

    /// document title: code scheme designator
    OFString             conceptCSD_;

    /// document title: code value
    OFString             conceptCV_;

    /// document title
    OFString             documentTitle_;

    /// file to read series data from
    OFString             seriesFile_;

    /// series instance UID
    OFString             seriesUID_;

    /// study instance UID
    OFString             studyUID_;

    /// specific character set (from series file)
    OFString             specificCharSet_;

    /// modality (from series file)
    OFString             modality_;

    /// DICOM sequence encoding: explicit or undefined length
    E_EncodingType          oenctype_;

    /// write mode for the DICOM file
    E_FileWriteMode         writeMode_;

    /// handling of group length encoding
    E_GrpLenEncoding        oglenc_;

    /// handling of data set trailing padding
    E_PaddingEncoding       opadenc_;

    /// transfer syntax for the DICOM file
    E_TransferSyntax        oxfer_;

    /// padding for the main dataset
    OFCmdUnsignedInt        filepad_;

    /// padding for sequence items
    OFCmdUnsignedInt        itempad_;

    /// true if we are supposed to read series information from another DICOM file
    OFBool                  readSeriesInfo_;

    /// burned-in annotation present?
    OFBool                  annotation_;

    /// increment instance number from given DICOM series file?
    OFBool                  increment_;

    /// instance number
    OFCmdSignedInt          instance_;

    /// list of DICOM attributes and attribute values to be applied after conversion
    OFList<OFString>        overrideKeys_;

    // CDA specific variables

    /// CDA media types
    OFString                cda_mediaTypes;

    /// CDA instance identifier
    OFString                hl7_InstanceIdentifier;

    /// should CDA header values override DICOM values?
    OFBool                  override_;

    // STL/MTL/OBJ specific variables
    // Frame of Reference module

    /// frame of reference UID
    OFString                frameOfReferenceUID_;

    /// position reference indicator
    OFString                positionReferenceIndicator_;

    // Enhanced general equipment module

    /// manufacturer
    OFString                manufacturer_;

    /// manufacturer's model name
    OFString                manufacturerModelName_;

    /// device serial number
    OFString                deviceSerialNumber_;

    /// software version(s) (VM 1-n)
    OFString                softwareVersions_;

    // Manufacturing 3D Model Module

    /// 3D model measurement units code meaning
    OFString                measurementUnitsCM_;

    /// 3D model measurement units code scheme designator
    OFString                measurementUnitsCSD_;

    /// 3D model measurement units code value
    OFString                measurementUnitsCV_;

    /// Type of file currently being converted.
    DocumentType ftype_;

    /// DICOM file
    DcmFileFormat dfile_;
};

#endif // DCENCDOC_H

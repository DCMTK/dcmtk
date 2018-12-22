/*
 *
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Author:  Pedro Arizpe
 *
 *  Purpose: Class to control document encapsulation into DICOM files
 *
 */

#ifndef DCENCDOC_H
#define DCENCDOC_H

//make sure OS specific configuration is included first
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */
#include "dcmtk/ofstd/ofcond.h"       /* for class OFCondition */
#include "dcmtk/dcmdata/dcdefine.h"   /* for DCMTK_DCMDATA_EXPORT */
#include "dcmtk/dcmdata/dcfilefo.h"   /* for class DcmFileFormat */

class XMLNode;
class OFCommandLine;
class OFConsoleApplication;
class OFLogger;

/** This class handles common functions of all command line applications
 *  for document encapsulation.
 */
class DCMTK_DCMDATA_EXPORT DcmEncapsulatedDocument
{
public:
  ///Constructor
  DcmEncapsulatedDocument();

  ///Destructor
  ~DcmEncapsulatedDocument();

  /** This function is only used to process CDA files.
   *  It retrieves all entries of an attribute and returns them separated by backslashes.
   *  @param fileNode the root XML node.
   *  @param attr the attribute to search for.
   *  @return OFstring containing all entries found, separated by double backslashes
   */
  OFString XMLgetAllAttributeValues(XMLNode fileNode, OFString attr);

  /** This function is only used to process CDA files.
   *  It retrieves the value from the CDA document
   *  corresponding to the DCM Tag. According to Standard v. 2013 part20/sect_A.8.
   *  @param fileNode the root XML node.
   *  @param attr the tag to search for in the CDA file.
   *  @return OFstring containing the value of the corresponding tag.
   */
  OFString XMLgetAttribute(XMLNode fileNode, DcmTagKey attr);

  /** Retrieves patient, concept and document data from the CDA file and checks for data conflicts
   *  with series, study and user input. It also retrieves all mediatypes found in the CDA document.
   *  @param filename The filename of the CDA document.
   *  @param appLogger The logger of the application calling this method.
   *  @return EXITCODE_NO_ERROR (0) if successful or error code in case of failure.
   */
  int getCDAData(const char *filename, OFLogger &appLogger);

  /** Recursive function used by getAttributeValues to get all occurrences of an attribute as list.
   *  @param currnode the current XML node to be processed.
   *  @param results a pointer to the list of strings where the results should be stored.
   *  @param attr the attribute to search for.
   *  @return OFTrue if the attribute value was found, OFFalse otherwise.
   */
  OFBool XMLsearchAttribute(XMLNode currnode, OFList<OFString> *results, OFString attr);

  /** Add CDA specific command line options to the OFCommandLine object
   *  passed to the constructor.
   *  @param cmd a reference to an OFCommandLine object used to parse
   *    the command line argument give to the calling application.
   *  @return none.
   */
  void addCDACommandlineOptions(OFCommandLine& cmd);

  /** Add PDF specific command line options to the OFCommandLine object
   *  passed to the constructor.
   *  @param cmd a reference to an OFCommandLine object used to parse
   *    the command line argument give to the calling application.
   *  @return none.
   */
  void addPDFCommandlineOptions(OFCommandLine& cmd);

  /** Add STL specific command line options to the OFCommandLine object
   *  passed to the constructor.
   *  @param cmd a reference to an OFCommandLine object used to parse
   *    the command line argument give to the calling application.
   *  @return none.
   */
  void addSTLCommandlineOptions(OFCommandLine& cmd);

  /** Add general command line options to the OFCommandLine object
   *  passed to the constructor.
   *  @param cmd a reference to an OFCommandLine object used to parse
   *    the command line argument give to the calling application.
   *  @return none.
   */
  void addGeneralOptions(OFCommandLine &cmd);

  /** Add command line options specific for documents to the OFCommandLine
   *  object passed to the constructor.
   *  @param cmd a reference to an OFCommandLine object used to parse
   *    the command line argument give to the calling application.
   *  @return none.
   */
  void addDocumentOptions(OFCommandLine &cmd);

  /** Add command line options specific for output to the OFCommandLine
   *  object passed to the constructor.
   *  @param cmd a reference to an OFCommandLine object used to parse
   *    the command line argument give to the calling application.
   *  @return none.
   */
  void addOutputOptions(OFCommandLine &cmd);

  /** Parse and evaluate the given command line arguments.
   *  @param app a reference to an OFConsoleApplication object used in the
   *    calling application.
   *  @param cmd a reference to an OFCommandLine object used to parse
   *    the command line argument give to the calling application.
   *  @return none.
   */
  void parseArguments(OFConsoleApplication& app, OFCommandLine& cmd);

  /** Includes basic information into the DICOM file.
   *  @param dataset a reference to a DcmItem containing the information to be included.
   *  @param logger The logger of the application calling this method.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition createHeader (DcmItem *dataset,
    OFLogger& logger);

  /** Correctly inserts encapsulated document data.
   *  @param dataset The dataset to which we should encapsulate this document.
   *  @param logger The logger of the application calling this method.
   *  @return EXITCODE_NO_ERROR (0) if successful or error code in case of failure.
   */
  int insertEncapsulatedDocument(DcmItem *dataset,
    OFLogger& logger);

  /** Get study or series data from provided file. Generate UID if none present.
   *  @param logger The logger of the application calling this method.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition createIdentifiers(OFLogger& appLogger);

  /** Copy override keys over existing keys in given dataset.
   *  @param outputDset dataset to which the override keys are copied
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition applyOverrideKeys(DcmDataset *outputDset);

  /** Specifies some attributes that should be inserted after encapsulation
   *  They will override any identical attributes already existing in the resulting encapsulated
   *  DICOM object. The override keys are applied at the very end of the conversion and do not
   *  undergo any validity checking.
   *  @param ovkeys override keys that can be tags, dictionary names and paths (see DcmPath
   *  for syntax). Also it is permitted to set a value if appropriate, e. g. "PatientName=Doe^John"
   *  would be a valid overridekey.
   *  @return none.
   */
  void setOverrideKeys(const OFList<OFString>& ovkeys);

  /** Returns the input file name.
   *  @return the input file name as OFString.
   */
  OFString getInputFileName();

  /** Sets the input file name to the given string.
   *  @param fName the file name to be set.
   *  @return none.
   */
  void setInputFileName(OFString fName);

  /** Returns the output file name.
   *  @return the output file name as OFString.
   */
  OFString getOutputFileName();

  /** Sets the output file name.
   *  @param fName the file name to be set.
   *  @return none.
   */
  void setOutputFileName(OFString fName);

  /** Attempt to save the output file .
   *  @param fileformat the DICOM Fileformat including the output file params.
   *  @return Error code as condition, if error occurs, EC_Normal otherwise.
   */
  OFCondition saveFile(DcmFileFormat fileformat);

  /** Returns the transfer syntax.
   *  @return the transfer syntax as E_TransferSyntax.
   */
  E_TransferSyntax getTransferSyntax();

  /** Returns the current filetype.
   *  @return the current filetype as OFString.
   */
  OFString getFileType();

  /** Sets the current filetype.
   *  @param fName the current filetype.
   *  @return none.
   */
  void setFileType(OFString fType);

private:
  ///input file name
  OFString             opt_ifname;
  ///output file name
  OFString             opt_ofname;

  ///optional parameters
  ///*patient data
  OFString             opt_patientBirthdate;
  OFString             opt_patientID;
  OFString             opt_patientName;
  OFString             opt_patientSex;
  ///*concept data
  OFString             opt_conceptCM;
  OFString             opt_conceptCSD;
  OFString             opt_conceptCV;
  ///*document specific options
  OFString             opt_documentTitle;
  OFString             opt_seriesFile;
  OFString             opt_seriesUID;
  OFString             opt_studyUID;

  ///*assign default values for file encoding and padding
  E_EncodingType          opt_oenctype;
  E_FileWriteMode         opt_writeMode;
  E_GrpLenEncoding        opt_oglenc;
  E_PaddingEncoding       opt_opadenc;
  E_TransferSyntax        opt_oxfer;
  OFCmdUnsignedInt        opt_filepad;
  OFCmdUnsignedInt        opt_itempad;
  ///*pre-existing series
  OFBool                  opt_readSeriesInfo;
  OFBool                  opt_annotation;
  OFBool                  opt_increment;

  OFCmdSignedInt          opt_instance;
  /** These attributes are applied to the dataset after conversion
   * (They are not checked by the isValid() function).
   */
  OFList<OFString>        opt_overrideKeys;

  ///CDA specific variables
  OFString                cda_mediaTypes;
  OFString                hl7_InstanceIdentifier;
  OFBool                  opt_override;

  /// STL specific variables
  /// Frame of Reference module
  OFString                opt_frameOfReferenceUID;
  OFString                opt_positionReferenceIndicator;
  ///Enhanced general equipment module
  /// Manufacturer (VM 1)
  OFString                opt_manufacturer;
  /// Manufacturer's Model Name (VM 1)
  OFString                opt_manufacturerModelName;
  /// Device Serial Number (VM 1)
  OFString                opt_deviceSerialNumber;
  /// Software Version(s) (VM 1-n)
  OFString                opt_softwareVersions;

  /// Manufacturing 3D Model Module
  /// 3d Model Measurement Units Code Meaning
  OFString                opt_measurementUnitsCM;
  /// 3d Model Measurement Units Code Scheme Designator
  OFString                opt_measurementUnitsCSD;
  /// 3d Model Measurement Units Code Value
  OFString                opt_measurementUnitsCV;

  ///Type of file currently being converted.
  OFString ftype;
};
#endif // DCENCDOC_H

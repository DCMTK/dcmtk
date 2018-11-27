/*
 *
<<<<<<< HEAD
 *  Copyright (C) 2018, OFFIS e.V.
=======
 *  Copyright (C) 2005-2018, OFFIS e.V.
>>>>>>> c04f23875ce248a32a3c524195128366fb689ab5
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
<<<<<<< HEAD
 *  Author:  Pedro Arizpe
 *
 *  Purpose: Encapsulate CDA file into a DICOM file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first*/
=======
 *  Author:  Marco Eichelberg, Pedro Arizpe
 *
 *  Purpose: Convert CDA file to DICOM format
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
>>>>>>> c04f23875ce248a32a3c524195128366fb689ab5

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"
<<<<<<< HEAD
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/dcmdata/dcencdoc.h"
=======
#include <list>
>>>>>>> c04f23875ce248a32a3c524195128366fb689ab5

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* for O_RDONLY */
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* required for sys/stat.h */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat, fstat */
#endif
END_EXTERN_C

<<<<<<< HEAD
=======
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/ofstd/ofxml.h"

>>>>>>> c04f23875ce248a32a3c524195128366fb689ab5
#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "cda2dcm"
<<<<<<< HEAD
=======
#define SHORTCOL 3
#define LONGCOL 19
>>>>>>> c04f23875ce248a32a3c524195128366fb689ab5

static OFLogger cda2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

<<<<<<< HEAD
int main(int argc, char *argv[])
{
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Encapsulate CDA file into DICOM format", rcsid);
  OFCommandLine cmd;
  int errorCode = EXITCODE_NO_ERROR;
  OFCondition result = EC_Normal;
  DcmFileFormat fileformat;
  DcmEncapsulatedDocument encapsulator;
  OFLOG_TRACE(cda2dcmLogger, "Including necessary options");
  encapsulator.addCDACommandlineOptions(cmd);
  OFLOG_TRACE(cda2dcmLogger, "Evaluating command line");
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);

  if (app.parseCommandLine(cmd, argc, argv)) {
    OFLOG_TRACE (cda2dcmLogger, "Checking exclusive options first");
    if (cmd.hasExclusiveOption())
    {
      if (cmd.findOption("--version"))
      {
        app.printHeader(OFTrue /*print host identifier*/);
        COUT << OFendl << "External libraries used: ";
#ifdef WITH_ZLIB
        COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
        COUT << " none" << OFendl;
#endif
        return EXITCODE_NO_ERROR;
      }
    }
    encapsulator.parseArguments(app, cmd);
  }
  //print resource identifier
  OFLOG_DEBUG(cda2dcmLogger, rcsid << OFendl);

  OFLOG_DEBUG(cda2dcmLogger, "making sure data dictionary is loaded");
  if (!dcmDataDict.isDictionaryLoaded())
  {
    OFLOG_WARN(cda2dcmLogger, "no data dictionary loaded, check environment variable: "
      << DCM_DICT_ENVIRONMENT_VARIABLE);
  }
  OFLOG_TRACE(cda2dcmLogger, "Creating identifiers (and reading series data)");
  result = encapsulator.createIdentifiers(cda2dcmLogger);
  if (result.bad())
  {
    OFLOG_FATAL(cda2dcmLogger, "There was an error while reading the series data");
    return EXITCODE_INVALID_INPUT_FILE;
  }
  OFLOG_DEBUG(cda2dcmLogger, "Fetching CDA Data");
  errorCode = encapsulator.getCDAData(encapsulator.getInputFileName().c_str(), cda2dcmLogger);
  if (errorCode != EXITCODE_NO_ERROR)
  {
    OFLOG_ERROR(cda2dcmLogger, "There was a problem with the CDA File");
    return errorCode;
  }
  else
  {
  OFLOG_INFO(cda2dcmLogger, "creating encapsulated CDA object");
  errorCode = encapsulator.insertEncapsulatedDocument(fileformat.getDataset(), cda2dcmLogger);
  }
  if (errorCode != EXITCODE_NO_ERROR)
  {
    OFLOG_ERROR(cda2dcmLogger, "unable to create CDA encapsulation to DICOM format");
    return errorCode;
  }
  OFLOG_INFO(cda2dcmLogger, "Generating an instance number that is guaranteed to be unique within a series.");
  result = encapsulator.createHeader(fileformat.getDataset(), cda2dcmLogger);
  if (result.bad())
  {
    OFLOG_ERROR(cda2dcmLogger, "unable to create DICOM header: " << result.text());
    return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
  }
  OFLOG_INFO(cda2dcmLogger, "writing encapsulated CDA object as file " << encapsulator.getOutputFileName());

  OFLOG_INFO(cda2dcmLogger, "Check if new output transfer syntax is possible");

  DcmXfer opt_oxferSyn(encapsulator.getTransferSyntax());

  fileformat.getDataset()->chooseRepresentation(encapsulator.getTransferSyntax(), NULL);
  if (fileformat.getDataset()->canWriteXfer(encapsulator.getTransferSyntax()))
  {
    OFLOG_INFO(cda2dcmLogger, "Output transfer syntax " << opt_oxferSyn.getXferName() << " can be written");
  }
  else {
    OFLOG_ERROR(cda2dcmLogger, "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
    return EXITCODE_COMMANDLINE_SYNTAX_ERROR;
  }
  OFLOG_INFO(cda2dcmLogger, "Checking for DICOM key overriding");
  result = encapsulator.applyOverrideKeys(fileformat.getDataset());
  if (result.bad())
  {
    OFLOG_ERROR(cda2dcmLogger, "There was a problem while overriding a key:" << OFendl
      << result.text());
    return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
  }
  OFLOG_INFO(cda2dcmLogger, "write converted DICOM file with metaheader");
  result = encapsulator.saveFile(fileformat);
  if (result.bad())
  {
    OFLOG_ERROR(cda2dcmLogger, result.text() << ": writing file: " << encapsulator.getOutputFileName());
    return EXITCODE_CANNOT_WRITE_OUTPUT_FILE;
  }

  OFLOG_INFO(cda2dcmLogger, "CDA encapsulation successful");

  return EXITCODE_NO_ERROR;
}
=======
static OFCondition createHeader(
	DcmItem *dataset,
	const char *opt_patientName,
	const char *opt_patientID,
	const char *opt_patientBirthdate,
	const char *opt_patientSex,
	OFBool opt_burnedInAnnotation,
	const char *opt_studyUID,
	const char *opt_seriesUID,
	const char *opt_documentTitle,
	const char *opt_conceptCSD,
	const char *opt_conceptCV,
	const char *opt_conceptCM,
	Sint32 opt_instanceNumber,
	const char *opt_mediaTypes = ""
	)
{
	OFCondition result = EC_Normal;
	char buf[80];

	// insert empty type 2 attributes
	if (result.good()) result = dataset->insertEmptyElement(DCM_StudyDate);
	if (result.good()) result = dataset->insertEmptyElement(DCM_StudyTime);
	if (result.good()) result = dataset->insertEmptyElement(DCM_AccessionNumber);
	if (result.good()) result = dataset->insertEmptyElement(DCM_Manufacturer);
	if (result.good()) result = dataset->insertEmptyElement(DCM_ReferringPhysicianName);
	if (result.good()) result = dataset->insertEmptyElement(DCM_StudyID);
	if (result.good()) result = dataset->insertEmptyElement(DCM_ContentDate);
	if (result.good()) result = dataset->insertEmptyElement(DCM_ContentTime);
	if (result.good()) result = dataset->insertEmptyElement(DCM_AcquisitionDateTime);
	if (result.good())
		if(opt_conceptCSD && opt_conceptCV && opt_conceptCM)
		{
			result = DcmCodec::insertCodeSequence(dataset, DCM_ConceptNameCodeSequence, opt_conceptCSD, opt_conceptCV, opt_conceptCM);
		}
		else
		{
			result = dataset->insertEmptyElement(DCM_ConceptNameCodeSequence);
		}

	// insert const value attributes
	if (result.good()) result = dataset->putAndInsertString(DCM_SpecificCharacterSet, "ISO_IR 100");
	//insert encapsulated file storage UID (CDA/PDF)
	if (result.good()) result = dataset->putAndInsertString(DCM_SOPClassUID, UID_EncapsulatedCDAStorage);
	// we are now using "DOC" for the modality, which seems to be more appropriate than "OT" (see CP-749)
	if (result.good()) result = dataset->putAndInsertString(DCM_Modality, "DOC");
	if (result.good()) result = dataset->putAndInsertString(DCM_ConversionType, "WSD");
	// according to C.24.2.1 on part 3, (0042,0012) is text/XML for CDA. 
	if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "text/XML");
	// there is no way we could determine a meaningful series number, so we just use a constant.
	if (result.good()) result = dataset->putAndInsertString(DCM_SeriesNumber, "1");

	// insert variable value attributes
	if (result.good()) result = dataset->putAndInsertString(DCM_DocumentTitle, opt_documentTitle);
	if (result.good()) result = dataset->putAndInsertString(DCM_PatientName, opt_patientName);
	if (result.good()) result = dataset->putAndInsertString(DCM_PatientID, opt_patientID);
	if (result.good()) result = dataset->putAndInsertString(DCM_PatientBirthDate, opt_patientBirthdate);
	if (result.good()) result = dataset->putAndInsertString(DCM_PatientSex, opt_patientSex);
	if (result.good()) result = dataset->putAndInsertString(DCM_BurnedInAnnotation, opt_burnedInAnnotation ? "YES" : "NO");
	
	if (strlen(opt_mediaTypes) >0) {
		if (result.good()) result = dataset->putAndInsertString(DCM_ListOfMIMETypes, opt_mediaTypes);
		std::cout << "mediatypes are:" << strlen(opt_mediaTypes) << "\n";
	}
	sprintf(buf, "%ld", OFstatic_cast(long, opt_instanceNumber));
	if (result.good()) result = dataset->putAndInsertString(DCM_InstanceNumber, buf);

	dcmGenerateUniqueIdentifier(buf, SITE_INSTANCE_UID_ROOT);
	if (result.good()) result = dataset->putAndInsertString(DCM_StudyInstanceUID, opt_studyUID);
	if (result.good()) result = dataset->putAndInsertString(DCM_SeriesInstanceUID, opt_seriesUID);
	if (result.good()) result = dataset->putAndInsertString(DCM_SOPInstanceUID, buf);

	// set instance creation date and time
	OFString s;
	if (result.good()) result = DcmDate::getCurrentDate(s);
	if (result.good()) result = dataset->putAndInsertOFStringArray(DCM_InstanceCreationDate, s);
	if (result.good()) result = DcmTime::getCurrentTime(s);
	if (result.good()) result = dataset->putAndInsertOFStringArray(DCM_InstanceCreationTime, s);

	return result;
}

//recursive function used by getmediatype to get all occurrences of "mediaType" attribute into a list
static OFBool searchmediaType(XMLNode currnode, OFList<OFString> *results)
{
	OFBool found = OFFalse;
	if (currnode.nChildNode() == 0)
	{
		//"currnode has no children (leaf)";
		if (currnode.isAttributeSet("mediaType"))
		{//mediatype found on leaf
			results->push_back(OFSTRING_GUARD(currnode.getAttribute("mediaType")));
			found = OFTrue;
		}
	}
	else {
		//"currnode has children (branch)";
		if (currnode.isAttributeSet("mediaType"))
		{//mediatype found on branch
			results->push_back(OFSTRING_GUARD(currnode.getAttribute("mediaType")));
			found = OFTrue;
		}
		for (int i = 0; i < currnode.nChildNode(); i++)
		{//search all children recursively
			OFBool childfound = searchmediaType(currnode.getChildNode(i), results);
			found |= childfound;
		}
	}
	return found;
}

static OFString getMediaTypes(XMLNode fileNode) {
	OFList<OFString> mediatypeslist;
	OFString mediatypes;
	if (searchmediaType(fileNode, &mediatypeslist))
	{
		//initialize with text/xml to exclude the primary MIME Type of the
		//encapsulated document
		mediatypes.append("text/xml");
		while (!mediatypeslist.empty())
		{
			if (mediatypes.find(mediatypeslist.front()) == OFString_npos) {
				if (!mediatypes.empty())mediatypes.append("\\");
				mediatypes.append(mediatypeslist.front());
			}
			mediatypeslist.pop_front();
		}
		//remove the primary MIME Type of the
		//encapsulated document
		if (mediatypes.size() > 9)
			mediatypes.erase(0, 9);
		else
			mediatypes = "";
	}
	return mediatypes;
}

static OFString getAttribute(XMLNode fileNode, OFString attr) {
	OFString result ="";
	if (attr == "docTitle") {
		if (fileNode.getChildNode("title").getText() != NULL) {
			result = OFString(OFSTRING_GUARD(fileNode.getChildNode("title").getText()));
		}
	}
	if (attr == "nameUse") {
		result = OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name").getAttribute("use")));
	}
	//according to Stantard v. 2013 part20/sect_A.8
	if (attr == "patientName") {
		result = OFString(OFSTRING_GUARD(
			fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/family").getText())) + "^" + OFString(OFSTRING_GUARD(
			fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name").getChildNode("given",0).getText())) + "^" + OFString(OFSTRING_GUARD(
			fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name").getChildNode("given",1).getText())) + "^" + OFString(OFSTRING_GUARD(
			fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/prefix").getText())) + "^" + OFString(OFSTRING_GUARD(
			fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/suffix").getText()));
	}
	if (attr == "patientSex") {	
		result = OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/administrativeGenderCode").getAttribute("code")));
	}
	if (attr == "patientBirthDate") {	
		result = OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/birthTime").getAttribute("value")));
	}
	//Table A.8-1. Basic Code Attributes Mapping to HL7 V3 Code Data Types (CV, CS, CE and CD)
	if (attr == "patientID")
	{
		result = OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/id").getAttribute("extension")));
	}
	if (attr == "CV")//Code Value
	{
		result = OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("code")));
	}
	if (attr == "CSU")//Coding Scheme UID (PS3.16)
	{
		result = OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("codeSystem")));
	}
	if (attr == "CSD")//Coding Scheme Designator (0008,0102)
	{
		result = OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("codeSystemName")));
	}
	if (attr == "CSV")//Coding Scheme Version (0008,0103)
	{
		result = OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("codeSystemVersion")));
	}
	if (attr == "CM")//Code Meaning (0008,0104)
	{
		result = OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("displayName")));
	}

	return result;
}

//retrieves patient, concept and document data and checks for conflicts with entered data
//also retrieves mediatypes found in DCA
static OFCondition getCDAData(const char *filename,
	OFString& docTitle,
	OFString& patientName,
	OFString& patientID,
	OFString& patientBirthDate,
	OFString& patientSex,
	OFString& conceptCSD,
	OFString& conceptCV,
	OFString& conceptCM,
	OFString& mediaTypes,
	OFBool& opt_override
	) {
	XMLResults err;
	XMLNode fileNode = XMLNode::parseFile(filename, "ClinicalDocument", &err);
	//checks if the XML file is correctly formatted
	if (0 != err.error)
	{
		OFLOG_ERROR(cda2dcmLogger, fileNode.getError(err.error));
		return EC_IllegalCall;
	}
	//get all media types from CDA file
	mediaTypes = getMediaTypes(fileNode);
	//read and compare patient information from CDA File
	OFString pID = getAttribute(fileNode, "patientID");
	if ((pID != "") && (patientID != pID))
	{
		if (patientID != "") {
			//if override option is inactive, return an error
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "Patient ID mismatch:"
					<<"\nData found in the CDA file : " << pID
					<< "\nEntered (or found in DCM file): " << patientID
					<< "\nIf you wish to override, run with +ov");
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "Patient ID mismatch:"
					<< "\nData found in the CDA file : " << pID
					<< "\nEntered (or found in DCM file): " << patientID);
			}
		}
		else patientID = pID;
	}
	OFString pBirthDate = getAttribute(fileNode, "patientBirthDate");
	if ((pBirthDate != "") && (patientBirthDate != pBirthDate))
	{
		if (patientBirthDate != "") {
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "Patient Birth Date mismatch:\n\tFound in the CDA file: " << pBirthDate
					<< "\t\nEntered (or found in DCM file): " << patientBirthDate
					<< "\nIf you wish to override, run with +ov");
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "Patient Birth Date mismatch:\n\tFound in the CDA file: " << pBirthDate
					<< "\t\nEntered (or found in DCM file): " << patientBirthDate);
			}
		}
		else patientBirthDate = pBirthDate;
	}
	OFString pSex = getAttribute(fileNode, "patientSex");
	if ((pSex != "") && (patientSex != pSex))
	{
		if (patientSex != "") {
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "Patient Sex mismatch:\n\tFound in the CDA file: " << pSex
					<< "\t\nEntered (or found in DCM file): " << patientSex
					<< "\nIf you wish to override, run with +ov");
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "Patient Sex mismatch:\n\tFound in the CDA file: " << pSex
					<< "\t\nEntered (or found in DCM file): " << patientSex);
			}
		}
		else patientSex = pSex;
	}
	OFString pName = getAttribute(fileNode, "patientName");
	if ((pName != "^^^^") && (patientName != pName))
	{
		if (patientName != "") {
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "Patient Name mismatch:\n\tFound in the CDA file: " << pName
					<< "\t\nEntered (or found in DCM file): " << patientName
					<< "\nIf you wish to override, run with +ov");
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "Patient Name mismatch:\n\tFound in the CDA file: " << pName
					<< "\t\nEntered (or found in DCM file): " << patientName);
			}
		} 
		else patientName = pName;
	}
	//get document title from CDA
	OFString dTitle = getAttribute(fileNode, "docTitle");
	if ((dTitle != "") && (docTitle != dTitle))
	{
		if (docTitle != "") {
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "Document Title mismatch:\n\tFound in the CDA file: " << dTitle
					<< "\tFound in the series):" << docTitle);
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "Document Title mismatch:\n\tFound in the CDA file: " << dTitle
					<< "\tFound in the series) : " << docTitle);
			}
		}
		else docTitle = dTitle;
	}
	//get Concept information from CDA
	OFString CSD = getAttribute(fileNode, "CSD");
	if ((CSD != "") && (conceptCSD != CSD))
	{
		if (conceptCSD != "") {
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "concept CSD mismatch:\n\tFound in the CDA file: " << CSD
					<< "\tFound in the series):" << conceptCSD);
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "concept CSD mismatch:\n\tFound in the CDA file: " << CSD
					<< "\tFound in the series) : " << conceptCSD);
			}
		}
		else conceptCSD = CSD;
	}
	OFString CV = getAttribute(fileNode, "CV");
	if ((CV != "") && (conceptCV != CV))
	{
		if (conceptCV != "") {
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "concept CV mismatch:\n\tFound in the CDA file: " << CV
					<< "\tFound in the series):" << conceptCV);
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "concept CV mismatch:\n\tFound in the CDA file: " << CV
					<< "\tFound in the series) : " << conceptCV);
			}
		}
		else conceptCV = CV;
	}
	OFString CM = getAttribute(fileNode, "CM");
	if ((CM != "") && (conceptCM != CM))
	{
		if (conceptCM != "") {
			if (!opt_override) {
				OFLOG_ERROR(cda2dcmLogger, "concept CM mismatch:\n\tFound in the CDA file: " << CM
					<< "\tFound in the series):" << conceptCM);
				return EC_IllegalCall;
			}
			else {
				OFLOG_WARN(cda2dcmLogger, "concept CM mismatch:\n\tFound in the CDA file: " << CM
					<< "\tFound in the series) : " << conceptCM);
			}
		}
		else conceptCM = CM;
	}
	return EC_Normal;
}

//inserts Document
static OFCondition insertEncapsulatedDocument(
	DcmItem *dataset,
	const char *filename)
{
	size_t fileSize = 0;
	struct stat fileStat;
	char buf[100];

	if (0 == stat(filename, &fileStat)) fileSize = OFstatic_cast(size_t, fileStat.st_size);
	else
	{
		OFLOG_ERROR(cda2dcmLogger, "file " << filename << " not found");
		return EC_IllegalCall;
	}

	if (fileSize == 0)
	{
		OFLOG_ERROR(cda2dcmLogger, "file " << filename << " is empty");
		return EC_IllegalCall;
	}

	FILE *encapfile = fopen(filename, "rb");
	if (encapfile == NULL)
	{
		OFLOG_ERROR(cda2dcmLogger, "unable to read file " << filename);
		return EC_IllegalCall;
	}

	size_t buflen = 100;
	if (fileSize < buflen) buflen = fileSize;
	if (buflen != fread(buf, 1, buflen, encapfile))
	{
		OFLOG_ERROR(cda2dcmLogger, "read error in file " << filename);
		fclose(encapfile);
		return EC_IllegalCall;
	}

	if (0 != fseek(encapfile, 0, SEEK_SET))
	{
		OFLOG_ERROR(cda2dcmLogger, "file " << filename << ": seek error");
		fclose(encapfile);
		return EC_IllegalCall;
	}

	OFCondition result = EC_Normal;
	DcmPolymorphOBOW *elem = new DcmPolymorphOBOW(DCM_EncapsulatedDocument);
	if (elem)
	{
		size_t numBytes = fileSize;
		if (numBytes & 1) ++numBytes;
		Uint8 *bytes = NULL;
		result = elem->createUint8Array(OFstatic_cast(Uint32, numBytes), bytes);
		if (result.good())
		{
			// blank pad byte
			bytes[numBytes - 1] = 0;

			// read file content
			if (fileSize != fread(bytes, 1, fileSize, encapfile))
			{
				OFLOG_ERROR(cda2dcmLogger, "read error in file " << filename);
				result = EC_IllegalCall;
			}
		}
	}
	else result = EC_MemoryExhausted;

	// if successful, insert element into dataset
	if (result.good()) result = dataset->insert(elem); else delete elem;

	// close file
	fclose(encapfile);

	return result;
}

//get series data
static void createIdentifiers(
	OFBool opt_readSeriesInfo,
	const char *opt_seriesFile,
	OFString& studyUID,
	OFString& seriesUID,
	OFString& patientName,
	OFString& patientID,
	OFString& patientBirthDate,
	OFString& patientSex,
	Sint32& incrementedInstance)
{
	char buf[100];
	if (opt_seriesFile)
	{
		DcmFileFormat dfile;
		OFCondition cond = dfile.loadFile(opt_seriesFile, EXS_Unknown, EGL_noChange);
		if (cond.bad())
		{
			OFLOG_WARN(cda2dcmLogger, cond.text() << ": reading file: " << opt_seriesFile);
		}
		else
		{
			const char *c = NULL;
			DcmDataset *dset = dfile.getDataset();
			if (dset)
			{
				// read patient attributes
				c = NULL;
				if (dset->findAndGetString(DCM_PatientName, c).good() && c) patientName = c;
				c = NULL;
				if (dset->findAndGetString(DCM_PatientID, c).good() && c) patientID = c;
				c = NULL;
				if (dset->findAndGetString(DCM_PatientBirthDate, c).good() && c) patientBirthDate = c;
				c = NULL;
				if (dset->findAndGetString(DCM_PatientSex, c).good() && c) patientSex = c;

				// read study attributes
				c = NULL;
				if (dset->findAndGetString(DCM_StudyInstanceUID, c).good() && c) studyUID = c;

				// read series attributes
				if (opt_readSeriesInfo)
				{
					c = NULL;
					if (dset->findAndGetString(DCM_SeriesInstanceUID, c).good() && c) seriesUID = c;
					if (dset->findAndGetSint32(DCM_InstanceNumber, incrementedInstance).good()) ++incrementedInstance; else incrementedInstance = 0;
				}
			}
		}
	}
	if (studyUID.empty())
	{
		dcmGenerateUniqueIdentifier(buf, SITE_STUDY_UID_ROOT);
		studyUID = buf;
	}
	if (seriesUID.empty())
	{
		dcmGenerateUniqueIdentifier(buf, SITE_SERIES_UID_ROOT);
		seriesUID = buf;
	}
}

static void initializeCommandLine(OFCommandLine &cmd)
{
	cmd.addParam("encapfile-in", "CDA input filename to be converted");
	cmd.addParam("dcmfile-out", "DICOM output filename");

	cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
	cmd.addOption("--help", "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
	cmd.addOption("--version", "print version information and exit", OFCommandLine::AF_Exclusive);
	OFLog::addOptions(cmd);

	cmd.addGroup("DICOM document options:");
	cmd.addSubGroup("burned-in annotation:");
	cmd.addOption("--annotation-yes", "+an", "CDA file (or its encapsulated documents) \ncontains patient identifying data (default)", OFCommandLine::AF_NoWarning);
	cmd.addOption("--annotation-no", "-an", "CDA file (or its encapsulated documents) \ndoes not contain patient identifying data", OFCommandLine::AF_NoWarning);
	cmd.addSubGroup("override CDA data");
	cmd.addOption("--no-override", "-ov", "CDA patient and document data\nmust match study, series or manually entered information\nOtherwise an error occurs (default)");
	cmd.addOption("--override", "+ov", "CDA's data will be overwritten\nby study, series or manually entered information");

	cmd.addSubGroup("document title:");
	cmd.addOption("--title", "+t", 1, "[t]itle: string (default: empty)",
		"document title");
	cmd.addOption("--concept-name", "+cn", 3, "[CSD] [CV] [CM]: string (default: empty)",
		"document title as concept name code sequence\n"
		"with coding scheme designator CSD, code value CV\n"
		"and code meaning CM");
	cmd.addSubGroup("patient data:");
	cmd.addOption("--patient-name", "+pn", 1, "[n]ame: string",
		"patient's name in DICOM PN syntax");
	cmd.addOption("--patient-id", "+pi", 1, "[i]d: string",
		"patient identifier");
	cmd.addOption("--patient-birthdate", "+pb", 1, "[d]ate: string (YYYYMMDD)",
		"patient's birth date");
	cmd.addOption("--patient-sex", "+ps", 1, "[s]ex: string (M, F or O)",
		"patient's sex");

	cmd.addSubGroup("study and series:");
	cmd.addOption("--generate", "+sg", "generate new study and series UIDs (default)");
	cmd.addOption("--study-from", "+st", 1, "[f]ilename: string",
		"read patient/study data from DICOM file");
	cmd.addOption("--series-from", "+se", 1, "[f]ilename: string",
		"read patient/study/series data from DICOM file");
	cmd.addSubGroup("instance number:");
	cmd.addOption("--instance-one", "+i1", "use instance number 1 (default, not with +se)", OFCommandLine::AF_NoWarning);
	cmd.addOption("--instance-inc", "+ii", "increment instance number (only with +se)", OFCommandLine::AF_NoWarning);
	cmd.addOption("--instance-set", "+is", 1, "[i]nstance number: integer", "use instance number i", OFCommandLine::AF_NoWarning);
}

int main(int argc, char *argv[])
{
	//required parameters
	////input and output file names. 
	const char* opt_ifname = NULL;
	const char* opt_ofname = NULL;
	//non-required parameters
	////document specific options
	//////patient data
	const char* opt_patientBirthdate = NULL;
	const char* opt_patientID = NULL;
	const char* opt_patientName = NULL;
	const char* opt_patientSex = NULL;
	//////concept 
	const char* opt_conceptCM = NULL;
	const char* opt_conceptCSD = NULL;
	const char* opt_conceptCV = NULL;
	const char* opt_documentTitle = NULL;
	const char* opt_seriesFile = NULL;
	//////assign default values for file encoding and padding
	E_EncodingType opt_oenctype = EET_ExplicitLength;
	E_GrpLenEncoding opt_oglenc = EGL_withoutGL;
	E_PaddingEncoding opt_opadenc = EPD_withoutPadding;
	E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
	OFCmdUnsignedInt opt_filepad = 0;
	OFCmdUnsignedInt opt_itempad = 0;
	////pre-existing series
	OFBool			opt_readSeriesInfo = OFFalse;
	OFBool			opt_annotation = OFTrue;
	OFBool			opt_increment = OFFalse;
	OFBool			opt_override = OFFalse;

	OFCmdSignedInt opt_instance = 1;

	OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Convert CDA file to DICOM", rcsid);
	OFCommandLine cmd;
	initializeCommandLine(cmd);
	/* evaluate command line */
	prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
	//readCmdOptions(app, cmd, argc, argv, opt_ifname, opt_ofname);

	if (app.parseCommandLine(cmd, argc, argv))
	{
		/* check exclusive options first */
		if (cmd.hasExclusiveOption())
		{
			if (cmd.findOption("--version"))
			{
				app.printHeader(OFTrue /*print host identifier*/);
				COUT << OFendl << "External libraries used: ";
#ifdef WITH_ZLIB
				COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
				COUT << " none" << OFendl;
#endif
				return 0;
			}
		}

		/* command line parameters and options */
		cmd.getParam(1, opt_ifname);
		cmd.getParam(2, opt_ofname);

		OFLog::configureFromCommandLine(cmd, app);

		dcmEnableGenerationOfNewVRs();

		cmd.beginOptionBlock();
		if (cmd.findOption("--generate"))
		{
			opt_seriesFile = NULL;
			opt_readSeriesInfo = OFFalse;
		}

		if (cmd.findOption("--series-from"))
		{
			app.checkValue(cmd.getValue(opt_seriesFile));
			opt_readSeriesInfo = OFTrue;
		}

		if (cmd.findOption("--study-from"))
		{
			app.checkValue(cmd.getValue(opt_seriesFile));
			opt_readSeriesInfo = OFFalse;
		}
		cmd.endOptionBlock();

		if (cmd.findOption("--title"))
		{
			app.checkValue(cmd.getValue(opt_documentTitle));
		}

		if (cmd.findOption("--concept-name"))
		{
			app.checkValue(cmd.getValue(opt_conceptCSD));
			app.checkValue(cmd.getValue(opt_conceptCV));
			app.checkValue(cmd.getValue(opt_conceptCM));
		}

		if (cmd.findOption("--patient-name"))
		{
			app.checkValue(cmd.getValue(opt_patientName));
			app.checkConflict("--patient-name", "--study-from or --series-from", opt_seriesFile != NULL);
		}
		if (cmd.findOption("--patient-id"))
		{
			app.checkValue(cmd.getValue(opt_patientID));
			app.checkConflict("--patient-id", "--study-from or --series-from", opt_seriesFile != NULL);
		}
		if (cmd.findOption("--patient-birthdate"))
		{
			app.checkValue(cmd.getValue(opt_patientBirthdate));
			app.checkConflict("--patient-birthdate", "--study-from or --series-from", opt_seriesFile != NULL);
		}
		if (cmd.findOption("--patient-sex"))
		{
			app.checkValue(cmd.getValue(opt_patientSex));
			app.checkConflict("--patient-sex", "--study-from or --series-from", opt_seriesFile != NULL);
		}
		cmd.beginOptionBlock();
		if (cmd.findOption("--override"))
		{
			opt_override = OFTrue;
		}
		if (cmd.findOption("--no-override")) {
			opt_override = OFFalse;
		}
		}

		cmd.beginOptionBlock();
		if (cmd.findOption("--annotation-yes"))
		{
			opt_annotation = OFTrue;
		if (cmd.findOption("--annotation-no"))
		{
			opt_annotation = OFTrue;
		}
		cmd.endOptionBlock();

		// initialize default for --series-from
		if (opt_seriesFile && opt_readSeriesInfo) opt_increment = OFTrue;

		cmd.beginOptionBlock();
		if (cmd.findOption("--instance-one"))
		{
			app.checkConflict("--instance-one", "--series-from", (opt_seriesFile != NULL) && opt_readSeriesInfo);
			opt_increment = OFFalse;
			opt_instance = 1;
		}
		if (cmd.findOption("--instance-inc"))
		{
			app.checkDependence("--instance-inc", "--series-from", (opt_seriesFile != NULL) && opt_readSeriesInfo);
			opt_increment = OFTrue;
		}
		if (cmd.findOption("--instance-set"))
		{
			opt_increment = OFFalse;
			app.checkValue(cmd.getValueAndCheckMin(opt_instance, 1));
		}
	}

	/* print resource identifier */
	OFLOG_DEBUG(cda2dcmLogger, rcsid << OFendl);

	/* make sure data dictionary is loaded */
	if (!dcmDataDict.isDictionaryLoaded())
	{
		OFLOG_WARN(cda2dcmLogger, "no data dictionary loaded, check environment variable: "
			<< DCM_DICT_ENVIRONMENT_VARIABLE);
	}

	// read raw file
	if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
	{
		OFLOG_FATAL(cda2dcmLogger, "invalid filename: <empty string>");
		return 1;
	}

	// create study and series UID
	OFString patientName;
	OFString patientID;
	OFString patientBirthDate;
	OFString patientSex;
	OFString studyUID;
	OFString seriesUID;
	Sint32 incrementedInstance = 0;
	//read patient and series info from options and file
	if (opt_patientName) patientName = opt_patientName;
	if (opt_patientID) patientID = opt_patientID;
	if (opt_patientBirthdate) patientBirthDate = opt_patientBirthdate;
	if (opt_patientSex) patientSex = opt_patientSex;
	createIdentifiers(opt_readSeriesInfo, opt_seriesFile, studyUID, seriesUID, patientName, patientID, patientBirthDate, patientSex, incrementedInstance);
	if (opt_increment) opt_instance = incrementedInstance;

	OFLOG_INFO(cda2dcmLogger, "creating encapsulated CDA object");
	//retrieve patient and document info from CDA
	OFString mediaTypes = "";
	OFString documentTitle;
	OFString conceptCSD;
	OFString conceptCV;
	OFString conceptCM;
	if (opt_documentTitle) documentTitle = opt_documentTitle;
	if (opt_conceptCSD) documentTitle = opt_conceptCSD;
	if (opt_conceptCV) documentTitle = opt_conceptCV;
	if (opt_conceptCM) documentTitle = opt_conceptCM;
	OFCondition result = getCDAData(opt_ifname, documentTitle, patientName, patientID, patientBirthDate, patientSex,
		conceptCSD, conceptCV, conceptCM, mediaTypes, opt_override);
	DcmFileFormat fileformat;
	if (result.bad())
	{
		OFLOG_ERROR(cda2dcmLogger, "There was a problem with the CDA File");
		return 10;
	}
	else result= insertEncapsulatedDocument(fileformat.getDataset(), opt_ifname);
	
	if (result.bad())
	{
		OFLOG_ERROR(cda2dcmLogger, "unable to create CDA DICOM encapsulation");
		return 10;
	}

	// now we need to generate an instance number that is guaranteed to be unique within a series.
	result = createHeader(fileformat.getDataset(), patientName.c_str(), patientID.c_str(),
		patientBirthDate.c_str(), patientSex.c_str(), opt_annotation, studyUID.c_str(),
		seriesUID.c_str(), documentTitle.c_str(), conceptCSD.c_str(), conceptCV.c_str(), conceptCM.c_str(), 
		OFstatic_cast(Sint32, opt_instance), mediaTypes.c_str());

	if (result.bad())
	{
		OFLOG_ERROR(cda2dcmLogger, "unable to create DICOM header: " << result.text());
		return 10;
	}

	OFLOG_INFO(cda2dcmLogger, "writing encapsulated CDA object as file " << opt_ofname);

	OFCondition error = EC_Normal;

	OFLOG_INFO(cda2dcmLogger, "Check if new output transfer syntax is possible");

	DcmXfer opt_oxferSyn(opt_oxfer);

	fileformat.getDataset()->chooseRepresentation(opt_oxfer, NULL);
	if (fileformat.getDataset()->canWriteXfer(opt_oxfer))
	{
		OFLOG_INFO(cda2dcmLogger, "Output transfer syntax " << opt_oxferSyn.getXferName() << " can be written");
	}
	else {
		OFLOG_ERROR(cda2dcmLogger, "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
		return 1;
	}

	OFLOG_INFO(cda2dcmLogger, "write converted DICOM file with metaheader");

	error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
		OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad));

	if (error.bad())
	{
		OFLOG_ERROR(cda2dcmLogger, error.text() << ": writing file: " << opt_ofname);
		return 1;
	}

	OFLOG_INFO(cda2dcmLogger, "conversion successful");
	return 0;
	}
>>>>>>> c04f23875ce248a32a3c524195128366fb689ab5

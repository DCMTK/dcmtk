/*
 *
 *  Copyright (C) 2005-2017, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, Pedro Arizpe
 *
 *  Purpose: Convert CDA file to DICOM format
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

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

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/ofstd/ofxml.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "cda2dcm"

static OFLogger cda2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

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
  Sint32 opt_instanceNumber)
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

    if (result.good() && opt_conceptCSD && opt_conceptCV && opt_conceptCM)
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
    if (result.good()) result = dataset->putAndInsertString(DCM_SOPClassUID,          UID_EncapsulatedCDAStorage);
    // we are now using "DOC" for the modality, which seems to be more appropriate than "OT" (see CP-749)
    if (result.good()) result = dataset->putAndInsertString(DCM_Modality,             "DOC");
    if (result.good()) result = dataset->putAndInsertString(DCM_ConversionType,       "WSD");
	// according to C.24.2.1 on part 3, (0042,0012) is text/XML for CDA. 
	// TODO: functionality for (0042,0014): list of MIME Types
    if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "text/XML");

    // there is no way we could determine a meaningful series number, so we just use a constant.
    if (result.good()) result = dataset->putAndInsertString(DCM_SeriesNumber,         "1");

    // insert variable value attributes
    if (result.good()) result = dataset->putAndInsertString(DCM_DocumentTitle,        opt_documentTitle);
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientName,          opt_patientName);
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientID,            opt_patientID);
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientBirthDate,     opt_patientBirthdate);
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientSex,           opt_patientSex);
    if (result.good()) result = dataset->putAndInsertString(DCM_BurnedInAnnotation,   opt_burnedInAnnotation ? "YES" : "NO");

    sprintf(buf, "%ld", OFstatic_cast(long, opt_instanceNumber));
    if (result.good()) result = dataset->putAndInsertString(DCM_InstanceNumber,       buf);

    dcmGenerateUniqueIdentifier(buf, SITE_INSTANCE_UID_ROOT);
    if (result.good()) result = dataset->putAndInsertString(DCM_StudyInstanceUID,     opt_studyUID);
    if (result.good()) result = dataset->putAndInsertString(DCM_SeriesInstanceUID,    opt_seriesUID);
    if (result.good()) result = dataset->putAndInsertString(DCM_SOPInstanceUID,       buf);

    // set instance creation date and time
    OFString s;
    if (result.good()) result = DcmDate::getCurrentDate(s);
    if (result.good()) result = dataset->putAndInsertOFStringArray(DCM_InstanceCreationDate, s);
    if (result.good()) result = DcmTime::getCurrentTime(s);
    if (result.good()) result = dataset->putAndInsertOFStringArray(DCM_InstanceCreationTime, s);

    return result;
}


//static const char* searchmediaType(const char *filename, const char *root)
//{
//	char findings[5000];
//	XMLNode currnode = XMLNode::parseFile(filename, root);
//	if (currnode.nChildNode() == 0)
//	{
//		if (currnode.isAttributeSet("mediaType"))
//		{
//			std::cout << "\nFOUND mediaType on a leaf called "<< currnode.getName() << "!\n" << currnode.getAttribute("mediaType") << " MediaType";
//			//return currnode.getAttribute("mediaType");
//			strcpy(findings, currnode.getAttribute("mediaType"));
//		}
//		else {
//			std::cout << "\nMedialess leaf";
//			strcpy(findings, "");
//		}
//	}
//	else {
//		if (currnode.isAttributeSet("mediaType"))
//		{
//			std::cout << "\nFOUND a mediaType on a branch called " << currnode.getName() << "! The mediatype is:\n" << currnode.getAttribute("mediaType");
//			//return currnode.getAttribute("mediaType");
//			strcpy(findings, currnode.getAttribute("mediaType"));
//		}
//		for (int i = 0; i <= currnode.nChildNode(); i++)
//		{
//			strcpy(findings, searchmediaType(filename, currnode.getChildNode(i).getName()));
//			std::cout<<"\n\ncalling SMT("<< currnode.getChildNode(i).getName();
//		//for (int i = 0; i <= currnode.nChildNode(); i++)
//		//{
//		//	if (currnode.getChildNode(i).isAttributeSet("mediaType"))
//		//	{
//		//		currnode.getChildNode(i).getAttribute("mediaType");
//		//		std::cout << "\nFOUND mediaType on the " << i << "th principal node called "<< currnode.getChildNode(i).getName() <<"!\n" << currnode.getChildNode(i).getAttribute("mediaType") << "\n";
//		//	}
//		//	XMLNode focusnode = currnode.getChildNode(i);
//		//	while (focusnode.nChildNode() != 0) {
//		//		for (int j = 0; j <= focusnode.nChildNode(); j++) {
//		//			if (focusnode.getChildNode(j).isAttributeSet("mediaType"))
//		//				std::cout << "\nFOUND mediaType on the " << j << "th child of the " << j << "th principal node!\n" << currnode.getChildNode(i).getAttribute("mediaType") << "\n";
//		//		}
//		//		focusnode = focusnode.getChildNode();
//		//	}
//		}
//	}
//	return findings;
//}

static const char* searchmediaType(XMLNode currnode)
{
	static const char* findings="meh";
	if (currnode.nChildNode() == 0)
	{
		std::cout << "\ncurrnode has no children";
		if (currnode.isAttributeSet("mediaType"))
		{
			std::cout << "\nFOUND mediaType on a leaf called " << currnode.getName() << "! The mediatype is:\n" << currnode.getAttribute("mediaType");
			//strcpy(findings, currnode.getAttribute("mediaType"));
		}
		else {
			std::cout << "\nMedialess leaf";
			//strcpy(findings, " ");
		}
	}
	else {
		std::cout << "\ncurrnode has children";
		if (currnode.isAttributeSet("mediaType"))
		{
			std::cout << "\nand mediaType";
			std::cout << "\nFOUND a mediaType on a branch called " << currnode.getName() << "! The mediatype is:\n" << currnode.getAttribute("mediaType");
			//strcpy(findings, currnode.getAttribute("mediaType"));
		}
		for (int i = 0; i < currnode.nChildNode(); i++)
		{
			std::cout << "\nprocessing child number "<<i;
			std::cout << "\n\ncalling SMT(" << currnode.getChildNode(i).getName() << ")";
			std::cout << searchmediaType(currnode.getChildNode(i));
			//strcpy(findings, searchmediaType(currnode.getChildNode(i)));
		}
	}
	return findings;
}

static OFCondition parseCDA(
	DcmItem *dataset,
	const char *filename)
{
	XMLNode fileNode = XMLNode::parseFile(filename, "ClinicalDocument");
	std::cout << "use of patient name (L)\n\t" << OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name").getAttribute("use"))) << "\n";
	std::cout << "Patient Name\n\t" << OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/given").getText())) << " " << OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/family").getText())) << "\n";
	//std::cout << "searching for media Types:\n"<< searchmediaType(filename, "ClinicalDocument");
	std::cout << "\n\ncalling SMT(" << fileNode.getName() << ")";
	std::cout << "searching for media Types:\n"<< searchmediaType(fileNode);
	//if(currnode.getChildNode(i).nAttribute()>0)std::cout << "enumcontents of " <<i<< "th node's attrib name(s?)\n"<< currnode.getChildNode(i).enumContents(0).attrib.lpszName << "\n";
	return EC_Normal;
}


//const char* findMediaTypeinNode(char *name, const char *filename)
//{
//	XMLNode node = XMLNode::parseFile(filename, "ClinicalDocument");
//	if (node.isAttributeSet(name))
//		return node.getAttribute(name);
//		//return node.getAttribute(name)+ findAttributeNameinNode(name, node.getChildNode());
//	else
//		return "";
//}



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
	//CDA SPECIFIC FUNCTIONS
    //check for ClinicalDocument
	//TODO: REWRITE WITH XMLParser
	 OFBool valid = OFFalse;
	 for (int i = 0; i < 99; ++i) {
		 char *cdavalidate = buf + i;
		 if (0 == strncmp("<ClinicalDocument", cdavalidate, 17))
		 {
			 valid = OFTrue;
			 break;
		 }
	 }
	
	 if(!valid){
		 OFLOG_ERROR(cda2dcmLogger, "file " << filename << " is not a CDA file");
		 fclose(encapfile);
		 return EC_IllegalCall;
	 }

	//check for media types

	//For an Encapsulated CDA Document:
	//  Document Title: 
	//  (0042,0010) shall have the value of the CDA Document Title,
	//  if one is present in the encapsulated document.
	//  Concept Name Code Sequence:
	//  (0040, A043) shall have the value of the CDA Document Type Code, 
	//  with transcoding as necessary for converting the HL7 CE Data Type
	//  to the DICOM Code Sequence item.
	//  The MIME Type of the Encapsulated Document:
	//  (0042, 0012) value shall be �text / XML�.
	
	//search for the first occurrence of "<title>" and set as title. 
	//Make sure no <section> was found beforehand.

	//List of MIME Types
	//  (0042, 0014):
	//  1C 
	//  MIME Types of subcomponents of the encapsulated document.
	//	Required if the encapsulated document incorporates subcomponents with MIME types
	//	different than the primary MIME Type of the encapsulated document.
	//	  Note: An Encapsulated CDA that includes an embedded JPEG image and an embedded
	//	  PDF would list �image / jpeg\application / pdf�.
	
	//  search for modifier "mediaType=" and return the substring that follows;

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
    } else result = EC_MemoryExhausted;

    // if successful, insert element into dataset
    if (result.good()) result = dataset->insert(elem); else delete elem;

    // close file
    fclose(encapfile);

    return result;
}


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
      OFLOG_WARN(cda2dcmLogger, cond.text() << ": reading file: "<< opt_seriesFile);
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


#define SHORTCOL 3
#define LONGCOL 19

int main(int argc, char *argv[])
{
  const char *opt_ifname = NULL;
  const char *opt_ofname = NULL;

  E_TransferSyntax opt_oxfer = EXS_LittleEndianExplicit;
  E_GrpLenEncoding opt_oglenc = EGL_withoutGL;
  E_EncodingType opt_oenctype = EET_ExplicitLength;
  E_PaddingEncoding opt_opadenc = EPD_withoutPadding;
  OFCmdUnsignedInt opt_filepad = 0;
  OFCmdUnsignedInt opt_itempad = 0;

  // document specific options
  const char *   opt_seriesFile = NULL;
  const char *   opt_patientName = NULL;
  const char *   opt_patientID = NULL;
  const char *   opt_patientBirthdate = NULL;
  const char *   opt_documentTitle = NULL;
  const char *   opt_conceptCSD = NULL;
  const char *   opt_conceptCV = NULL;
  const char *   opt_conceptCM = NULL;

  OFBool         opt_readSeriesInfo = OFFalse;
  const char *   opt_patientSex = NULL;
  OFBool         opt_annotation = OFTrue;
  OFCmdSignedInt opt_instance = 1;
  OFBool         opt_increment = OFFalse;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Convert CDA file to DICOM", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("encapfile-in",  "CDA input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                 "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

   cmd.addGroup("DICOM document options:");
    cmd.addSubGroup("burned-in annotation:");
      cmd.addOption("--annotation-yes",    "+an",    "CDA contains patient identifying data (default)");
      cmd.addOption("--annotation-no",     "-an",    "CDA does not contain patient identifying data");

    cmd.addSubGroup("document title:");
      cmd.addOption("--title",             "+t",  1, "[t]itle: string (default: empty)",
                                                     "document title");
      cmd.addOption("--concept-name",      "+cn", 3, "[CSD] [CV] [CM]: string (default: empty)",
                                                     "document title as concept name code sequence\n"
                                                     "with coding scheme designator CSD, code value CV\n"
                                                     "and code meaning CM");
    cmd.addSubGroup("patient data:");
      cmd.addOption("--patient-name",      "+pn", 1, "[n]ame: string",
                                                     "patient's name in DICOM PN syntax");
      cmd.addOption("--patient-id",        "+pi", 1, "[i]d: string",
                                                     "patient identifier");
      cmd.addOption("--patient-birthdate", "+pb", 1, "[d]ate: string (YYYYMMDD)",
                                                     "patient's birth date");
      cmd.addOption("--patient-sex",       "+ps", 1, "[s]ex: string (M, F or O)",
                                                     "patient's sex");

    cmd.addSubGroup("study and series:");
      cmd.addOption("--generate",          "+sg",    "generate new study and series UIDs (default)");
      cmd.addOption("--study-from",        "+st", 1, "[f]ilename: string",
                                                     "read patient/study data from DICOM file");
      cmd.addOption("--series-from",       "+se", 1, "[f]ilename: string",
                                                     "read patient/study/series data from DICOM file");
    cmd.addSubGroup("instance number:");
      cmd.addOption("--instance-one",      "+i1",    "use instance number 1 (default, not with +se)");
      cmd.addOption("--instance-inc",      "+ii",    "increment instance number (only with +se)");
      cmd.addOption("--instance-set",      "+is", 1, "[i]nstance number: integer", "use instance number i");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
          if (cmd.findOption("--version"))
          {
              app.printHeader(OFTrue /*print host identifier*/);
              COUT << OFendl << "External libraries used:";
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
      if (cmd.findOption("--annotation-yes"))
      {
        opt_annotation = OFTrue;
      }
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
    OFString studyUID;
    OFString seriesUID;
    OFString patientName;
    OFString patientID;
    OFString patientBirthDate;
    OFString patientSex;
    Sint32 incrementedInstance = 0;

    if (opt_patientName) patientName = opt_patientName;
    if (opt_patientID) patientID = opt_patientID;
    if (opt_patientBirthdate) patientBirthDate = opt_patientBirthdate;
    if (opt_patientSex) patientSex = opt_patientSex;

    createIdentifiers(opt_readSeriesInfo, opt_seriesFile, studyUID, seriesUID, patientName, patientID, patientBirthDate, patientSex, incrementedInstance);
    if (opt_increment) opt_instance = incrementedInstance;

    OFLOG_INFO(cda2dcmLogger, "creating encapsulated CDA object");

    DcmFileFormat fileformat;

    OFCondition result = insertEncapsulatedDocument(fileformat.getDataset(), opt_ifname);
	OFCondition resultxml = parseCDA(fileformat.getDataset(), opt_ifname);
	if (result.bad())
	{
		OFLOG_ERROR(cda2dcmLogger, "unable to create CDA DICOM encapsulation");
		return 10;
	}

	if (resultxml.bad())
	{
		OFLOG_ERROR(cda2dcmLogger, "unable to create CDA DICOM encapsulation");
		return 10;
	}
    if (result.bad()) return 10;
	if (resultxml.bad()) return 10;
	

    // now we need to generate an instance number that is guaranteed to be unique within a series.

    result = createHeader(fileformat.getDataset(), patientName.c_str(), patientID.c_str(),
      patientBirthDate.c_str(), patientSex.c_str(), opt_annotation, studyUID.c_str(),
      seriesUID.c_str(), opt_documentTitle, opt_conceptCSD, opt_conceptCV, opt_conceptCM, OFstatic_cast(Sint32, opt_instance));

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
    } else {
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

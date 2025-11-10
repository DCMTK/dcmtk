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
 *  Author:  Pedro ArizpeGomez, Marco Eichelberg
 *
 *  Purpose: Implementation of Document encapsulation
 *
 */

//make sure OS specific configuration is included first
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcencdoc.h"

#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofxml.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstd.h"

#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcvrpobw.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrtm.h"

#define SHORTCOL 3
#define LONGCOL 21

DcmEncapsulatedDocument::DcmEncapsulatedDocument()
: patientBirthdate_()
, patientID_()
, patientName_()
, patientSex_()
, conceptCM_()
, conceptCSD_()
, conceptCV_()
, documentTitle_()
, seriesFile_()
, seriesUID_()
, studyUID_()
, specificCharSet_()
, modality_()
, oenctype_(EET_ExplicitLength)
, writeMode_(EWM_fileformat)
, oglenc_(EGL_withoutGL)
, opadenc_(EPD_withoutPadding)
, oxfer_(EXS_LittleEndianExplicit)
, filepad_(0)
, itempad_(0)
, readSeriesInfo_(OFFalse)
, annotation_(OFTrue)
, increment_(OFFalse)
, instance_(1)
, overrideKeys_()
, cda_mediaTypes()
, hl7_InstanceIdentifier()
, override_(OFFalse)
, frameOfReferenceUID_()
, positionReferenceIndicator_()
, manufacturer_()
, manufacturerModelName_()
, deviceSerialNumber_()
, softwareVersions_()
, measurementUnitsCM_()
, measurementUnitsCSD_()
, measurementUnitsCV_()
, ftype_(DT_unknownDocument)
, dfile_()
{
}


DcmEncapsulatedDocument::~DcmEncapsulatedDocument()
{
}

// ========== static helper functions for processing CDA documents ==========

/** Recursive function used by getAttributeValues to get all occurrences of an attribute as a list.
 *  @param currnode the current XML node to be processed.
 *  @param list of strings to which the results are added.
 *  @param attr the attribute to search for.
 *  @return OFTrue if the attribute value was found, OFFalse otherwise.
 */
static OFBool XMLsearchAttribute(
        XMLNode currnode,
        OFList<OFString> &results,
        const OFString& attr)
{
  OFBool found = OFFalse;
#ifndef _XMLWIDECHAR
  if (currnode.nChildNode() == 0)
  {
    // currnode has no children (leaf)
    if (currnode.isAttributeSet(attr.c_str()))
    {
      //attribute found on leaf
      results.push_back(OFSTRING_GUARD(currnode.getAttribute(attr.c_str())));
      found = OFTrue;
    }
  }
  else
  {
    // currnode has children (branch)
    if (currnode.isAttributeSet(attr.c_str()))
    {
      //attribute found on branch
      results.push_back(OFSTRING_GUARD(currnode.getAttribute(attr.c_str())));
      found = OFTrue;
    }
    for (int i = 0; i < currnode.nChildNode(); i++)
    {
      //search all children recursively
      found |= XMLsearchAttribute(currnode.getChildNode(i), results, attr);
    }
  }
#endif
  return found;
}


/** Retrieve all entries of an attribute and returns them as a string, separated by backslashes.
 *  @param fileNode the root XML node.
 *  @param attr the attribute to search for.
 *  @return OFstring containing all entries found, separated by backslashes
 */
static OFString XMLgetAllAttributeValues(XMLNode& fileNode, const OFString& attr)
{
  OFString attributeValues;
#ifndef _XMLWIDECHAR
  OFList<OFString> attributeValueslist;
  if (XMLsearchAttribute(fileNode, attributeValueslist, attr))
  {
    // If the Attribute is mediaType, initialize with text/xml to exclude
    // the primary MIME Type of the encapsulated document
    if (attr == "mediaType") attributeValues.append("text/xml");

    while (!attributeValueslist.empty())
    {
      if (attributeValues.find(attributeValueslist.front()) == OFString_npos)
      {
        if (!attributeValues.empty()) attributeValues.append("\\");
        attributeValues.append(attributeValueslist.front());
      }
      attributeValueslist.pop_front();
    }

    // remove the primary MIME Type of the encapsulated document
    if (attr == "mediaType")
    {
      if (attributeValues.size() > 9)
        attributeValues.erase(0, 9);
      else
        attributeValues = "";
    }
  }
#endif
  return attributeValues;
}


/** Retrieve the value from the CDA document corresponding to the given DICOM Tag,
 *  according to DICOM Part 20, section A.8.
 *  @param fileNode the root XML node.
 *  @param attr the tag to search for in the CDA file.
 *  @return OFstring containing the value of the corresponding tag.
 */
static OFString XMLgetAttribute(XMLNode& fileNode, const DcmTagKey& attr)
{
#ifndef _XMLWIDECHAR
    if (attr == DCM_DocumentTitle)
    {
      if (fileNode.getChildNode("title").getText() != NULL)
      {
        return OFString(OFSTRING_GUARD(fileNode.getChildNode("title").getText()));
      }
    }

    if (attr == DCM_HL7InstanceIdentifier)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNode("id").getAttribute("root"))) + "^"
               + OFString(OFSTRING_GUARD(fileNode.getChildNode("id").getAttribute("extension")));
    }

    if (attr == DCM_PatientName)
    {
      return OFString(
              OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/family").getText())) + "^"
               + OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name").getChildNode("given", 0).getText())) + "^"
               + OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name").getChildNode("given", 1).getText())) + "^"
               + OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/prefix").getText())) + "^"
               + OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/name/suffix").getText()));
    }

    if (attr == DCM_PatientSex)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/administrativeGenderCode").getAttribute("code")));
    }

    if (attr == DCM_PatientBirthDate)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/patient/birthTime").getAttribute("value")));
    }

    //Table A.8-1. Basic Code Attributes Mapping to HL7 V3 Code Data Types (CV, CS, CE and CD)
    if (attr == DCM_PatientID)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNodeByPath("recordTarget/patientRole/id").getAttribute("extension")));
    }

    if (attr == DCM_CodeValue)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("code")));
    }
    if (attr == DCM_CodingSchemeUID)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("codeSystem")));
    }
    if (attr == DCM_CodingSchemeDesignator)
    {
      OFString CSDtemp = OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("codeSystemName")));

      // Abbreviate most common CSNs
      if (CSDtemp == OFString("LOINC"))
      {
        return OFString("LN");
      }
      else
      {
        if (CSDtemp == OFString("DICOM"))
        {
          return OFString("DCM");
        }
        else
        {
          if (CSDtemp == OFString("SNOMED"))
          {
            return OFString("SCT");
          }
          else
          {
            return CSDtemp;
          }
        }
      }
    }

    if (attr == DCM_CodingSchemeVersion)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("codeSystemVersion")));
    }

    if (attr == DCM_CodeMeaning)
    {
      return OFString(OFSTRING_GUARD(fileNode.getChildNode("code").getAttribute("displayName")));
    }

    // this shouldn't happen
    DCMDATA_ERROR("XMLgetAttribute called with unsupported attribute tag " << attr);
#endif

    return "";
}

// ========== class methods ==========

OFCondition DcmEncapsulatedDocument::getCDAData()
{
#ifdef _XMLWIDECHAR
#ifdef _MSC_VER
#pragma message("DCMTK compiled with 'wide char XML parser'. dcmencap will be unable to read and encapsulate CDA documents.")
#else
#warning "DCMTK compiled with 'wide char XML parser'. dcmencap will be unable to read and encapsulate CDA documents."
#endif
    DCMDATA_ERROR("DCMTK compiled with \"wide char XML parser\". Cannot parse CDA data because of incompatible API.");
    return EC_XMLParseError;
#else
    DCMDATA_INFO("Extracting information from CDA document content");

    XMLResults err;
    XMLNode fileNode = XMLNode::parseFile(ifname_.c_str(), "ClinicalDocument", &err);
    DCMDATA_TRACE("checking if the XML file is correctly formatted");
    if (0 != err.error)
    {
      DCMDATA_ERROR(fileNode.getError(err.error));
      return EC_XMLParseError;
    }
    else
    {
      DCMDATA_TRACE("XML file is correctly formatted");
    }
    DCMDATA_TRACE("Getting all media types from CDA file");
    cda_mediaTypes = XMLgetAllAttributeValues(fileNode, "mediaType");
    DCMDATA_TRACE("Following mediaTypes were found: " << cda_mediaTypes);
    DCMDATA_TRACE("Getting HL7 Instance Identifier from CDA file");
    hl7_InstanceIdentifier = XMLgetAttribute(fileNode, DCM_HL7InstanceIdentifier);
    DCMDATA_TRACE("Reading and comparing patient information between CDA File and options");
    OFString pID = XMLgetAttribute(fileNode, DCM_PatientID);
    if ((pID.length() > 0) && (patientID_ != pID))
    {
      if (patientID_.length() > 0)
      {
        if (override_) DCMDATA_WARN("Patient ID mismatch: '" << pID << "' in CDA, '" << patientID_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("Patient ID mismatch: '" << pID << "' in CDA, '" << patientID_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else
      {
        patientID_ = pID;
      }
    }
    OFString pBirthDate = XMLgetAttribute(fileNode, DCM_PatientBirthDate);
    if ((pBirthDate.length() > 0) && (patientBirthdate_ != pBirthDate))
    {
      if (patientBirthdate_.length() > 0)
      {
        if (override_) DCMDATA_WARN("Patient Birth Date mismatch: '" << pBirthDate << "' in CDA, '" << patientBirthdate_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("Patient Birth Date mismatch: '" << pBirthDate << "' in CDA, '" << patientBirthdate_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else patientBirthdate_ = pBirthDate;
    }
    OFString pSex = XMLgetAttribute(fileNode, DCM_PatientSex);
    if ((pSex.length() > 0) && (patientSex_ != pSex))
    {
      if (patientSex_.length() > 0)
      {
        if (override_) DCMDATA_WARN("Patient Sex mismatch: '" << pSex << "' in CDA, '" << patientSex_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("Patient Sex mismatch: '" << pSex << "' in CDA, '" << patientSex_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else patientSex_ = pSex;
    }
    OFString pName = XMLgetAttribute(fileNode, DCM_PatientName);
    if ((pName != "^^^^") && (patientName_ != pName))
    {
      if (patientName_.length() > 0)
      {
        if (override_) DCMDATA_WARN("Patient Name mismatch: '" << pName << "' in CDA, '" << patientName_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("Patient Name mismatch: '" << pName << "' in CDA, '" << patientName_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else patientName_ = pName;
    }
    //get document title from CDA
    OFString dTitle = XMLgetAttribute(fileNode, DCM_DocumentTitle);
    if ((dTitle.length() > 0) && (documentTitle_ != dTitle))
    {
      if (documentTitle_.length() > 0)
      {
        if (override_) DCMDATA_WARN("Document Title mismatch: '" << dTitle << "' in CDA, '" << documentTitle_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("Document Title mismatch: '" << dTitle << "' in CDA, '" << documentTitle_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else documentTitle_ = dTitle;
    }

    //get Concept information from CDA
    OFString cCSD = XMLgetAttribute(fileNode, DCM_CodingSchemeDesignator);
    if ((cCSD.length() > 0) && (conceptCSD_ != cCSD))
    {
      if (conceptCSD_.length() > 0)
      {
        if (override_) DCMDATA_WARN("concept CSD mismatch: '" << cCSD << "' in CDA, '" << conceptCSD_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("concept CSD mismatch: '" << cCSD << "' in CDA, '" << conceptCSD_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else conceptCSD_ = cCSD;
    }
    OFString cCV = XMLgetAttribute(fileNode, DCM_CodeValue);
    if ((cCV.length() > 0) && (conceptCV_ != cCV))
    {
      if (conceptCV_.length() > 0)
      {
        if (override_) DCMDATA_WARN("concept CV mismatch: '" << cCV << "' in CDA, '" << conceptCV_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("concept CV mismatch: '" << cCV << "' in CDA, '" << conceptCV_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else conceptCV_ = cCV;
    }
    OFString cCM = XMLgetAttribute(fileNode, DCM_CodeMeaning);
    if ((cCM.length() > 0) && (conceptCM_ != cCM))
    {
      if (conceptCM_.length() > 0)
      {
        if (override_) DCMDATA_WARN("concept CM mismatch: '" << cCM << "' in CDA, '" << conceptCM_ << "' in DICOM file or specified via command line.");
        else
        {
          DCMDATA_ERROR("concept CM mismatch: '" << cCM << "' in CDA, '" << conceptCM_ << "' in DICOM file or specified via command line.");
          return EC_InvalidValue;
        }
      }
      else conceptCM_ = cCM;
    }
    return EC_Normal;
#endif
}


static void addGeneralOptions(OFCommandLine &cmd)
{
    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help", "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version", "print version information and exit", OFCommandLine::AF_Exclusive);
    OFLog::addOptions(cmd);
}


static void addDocumentOptions(OFCommandLine &cmd)
{
    cmd.addGroup("DICOM document options:");

    cmd.addSubGroup("document title:");
    cmd.addOption("--title",               "+t", 1, "[t]itle: string (default: empty)", "document title");
    cmd.addOption("--concept-name",        "+cn", 3, "[CSD] [CV] [CM]: string (default: empty)",
                    "coded representation of document title defined\nby coding scheme designator CSD,\n"
                    "code value CV and code meaning CM");
    cmd.addSubGroup("patient data:");
    cmd.addOption("--patient-name",        "+pn", 1, "[n]ame: string", "patient's name in DICOM PN syntax");
    cmd.addOption("--patient-id",          "+pi", 1, "[i]d: string", "patient identifier");
    cmd.addOption("--patient-birthdate",   "+pb", 1, "[d]ate: string (YYYYMMDD)", "patient's birth date");
    cmd.addOption("--patient-sex",         "+ps", 1, "[s]ex: string (M, F or O)", "patient's sex");

    cmd.addSubGroup("device data:");
    cmd.addOption("--manufacturer",          "+mn", 1, "[n]ame: string", "manufacturer's name");
    cmd.addOption("--manufacturer-model",    "+mm", 1, "[n]ame: string", "manufacturer's model name");
    cmd.addOption("--device-serial",         "+ds", 1, "[n]umber: string", "device serial number");
    cmd.addOption("--software-versions",     "+sv", 1, "[v]ersions: string", "software versions");

    cmd.addSubGroup("manufacturing 3d model data (STL/MTL/OBJ only):");
    cmd.addOption("--measurement-units",     "+mu", 3, "[CSD] [CV] [CM]: string (default: UCUM, um, um)",
                  "measurement units defined by coding scheme\ndesignator CSD, code value CV, code meaning CM");

    cmd.addSubGroup("study and series:");
    cmd.addOption("--generate",            "+sg", "generate new study and\nseries UIDs (default)");
    cmd.addOption("--study-from",          "+st", 1, "[f]ilename: string", "read patient/study data from DICOM file");
    cmd.addOption("--series-from",         "+se", 1, "[f]ilename: string", "read patient/study/series data from DICOM file");

    cmd.addSubGroup("instance number:");
    cmd.addOption("--instance-one",        "+i1", "use instance number 1\n(default, not with +se)");
    cmd.addOption("--instance-inc",        "+ii", "increment instance number (only with +se)");
    cmd.addOption("--instance-set",        "+is", 1, "[i]nstance number: integer", "use instance number i");

    cmd.addSubGroup("burned-in annotation:");
    cmd.addOption("--annotation-yes",      "+an", "document contains patient identifying data\n(default)");
    cmd.addOption("--annotation-no",       "-an", "document does not contain patient identif. data");
}


static void addOutputOptions(OFCommandLine &cmd)
{
    cmd.addGroup("processing options:");
    cmd.addSubGroup("CDA processing options:");
    cmd.addOption("--no-override",           "-ov", "CDA patient and document data must match study,\nseries or manually entered information (default)");
    cmd.addOption("--override",              "+ov", "CDA's data will be overwritten by study, series\nor manually entered information");

    cmd.addSubGroup("other processing options:");
    cmd.addOption("--key",                 "-k", 1, "[k]ey: gggg,eeee=\"str\", path or dict. name=\"str\"", "add further attribute");

    cmd.addGroup("output options:");

    cmd.addSubGroup("output transfer syntax:");
    cmd.addOption("--write-xfer-little",   "+te", "write with explicit VR little endian (default)");
    cmd.addOption("--write-xfer-big",      "+tb", "write with explicit VR big endian TS");
    cmd.addOption("--write-xfer-implicit", "+ti", "write with implicit VR little endian TS");

    cmd.addSubGroup("group length encoding:");
    cmd.addOption("--group-length-remove", "-g", "write without group length elements (default)");
    cmd.addOption("--group-length-create", "+g", "write with group length elements");

    cmd.addSubGroup("length encoding in sequences and items:");
    cmd.addOption("--length-explicit",     "+e", "write with explicit lengths (default)");
    cmd.addOption("--length-undefined",    "-e", "write with undefined lengths");

    cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
    cmd.addOption("--padding-retain",      "-p=", "do not change padding (default)");
    cmd.addOption("--padding-off",         "-p", "no padding (implicit if --write-dataset)");
    cmd.addOption("--padding-create",      "+p", 2, "[f]ile-pad [i]tem-pad: integer",
                  "align file on multiple of f bytes\nand items on multiple of i bytes");
}


void DcmEncapsulatedDocument::addCommandlineOptions(OFCommandLine &cmd) const
{
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
    cmd.addParam("docfile-in",  "input filename to be converted");
    cmd.addParam("dcmfile-out", "DICOM output filename (\"-\" for stdout)");

    addGeneralOptions(cmd);

    cmd.addGroup("input options:", LONGCOL, SHORTCOL + 2);
    cmd.addSubGroup("input file format options:");
    cmd.addOption("--filetype-auto",         "+fa", "automatically determine file type (default)");
    cmd.addOption("--filetype-pdf",          "+fp", "expect PDF file");
    cmd.addOption("--filetype-cda",          "+fc", "expect CDA file");
    cmd.addOption("--filetype-stl",          "+fs", "expect STL file");
    cmd.addOption("--filetype-mtl",          "+fm", "expect MTL file");
    cmd.addOption("--filetype-obj",          "+fo", "expect OBJ file");

    addDocumentOptions(cmd);

    addOutputOptions(cmd);
}


void DcmEncapsulatedDocument::parseArguments(
        OFConsoleApplication &app,
        OFCommandLine &cmd)
{
    //command line parameters and options
    cmd.getParam(1, ifname_);
    cmd.getParam(2, ofname_);

    // ---------- parse general options ----------
    OFLog::configureFromCommandLine(cmd, app);

    // ---------- parse input file format options ----------
    cmd.beginOptionBlock();
    if (cmd.findOption("--filetype-auto")) ftype_ = DT_unknownDocument;
    if (cmd.findOption("--filetype-pdf"))  ftype_ = DT_pdfDocument;
    if (cmd.findOption("--filetype-cda"))  ftype_ = DT_cdaDocument;
    if (cmd.findOption("--filetype-stl"))  ftype_ = DT_stlDocument;
    if (cmd.findOption("--filetype-mtl"))  ftype_ = DT_mtlDocument;
    if (cmd.findOption("--filetype-obj"))  ftype_ = DT_objDocument;
    cmd.endOptionBlock();

    // ---------- parse document options ----------

    cmd.beginOptionBlock();
    if (cmd.findOption("--generate"))
    {
      seriesFile_ = "";
      readSeriesInfo_ = OFFalse;
    }

    if (cmd.findOption("--series-from"))
    {
      app.checkValue(cmd.getValue(seriesFile_));
      readSeriesInfo_ = OFTrue;
    }

    if (cmd.findOption("--study-from"))
    {
      app.checkValue(cmd.getValue(seriesFile_));
      readSeriesInfo_ = OFFalse;
    }
    cmd.endOptionBlock();

    if (cmd.findOption("--title"))
    {
      app.checkValue(cmd.getValue(documentTitle_));
    }
    if (cmd.findOption("--concept-name"))
    {
      app.checkValue(cmd.getValue(conceptCSD_));
      app.checkValue(cmd.getValue(conceptCV_));
      app.checkValue(cmd.getValue(conceptCM_));
    }
    if (cmd.findOption("--patient-name"))
    {
      app.checkValue(cmd.getValue(patientName_));
      app.checkConflict("--patient-name", "--study-from or --series-from", seriesFile_.length() > 0);
    }
    if (cmd.findOption("--patient-id"))
    {
      app.checkValue(cmd.getValue(patientID_));
      app.checkConflict("--patient-id", "--study-from or --series-from", seriesFile_.length() > 0);
    }
    if (cmd.findOption("--patient-birthdate"))
    {
      app.checkValue(cmd.getValue(patientBirthdate_));
      app.checkConflict("--patient-birthdate", "--study-from or --series-from", seriesFile_.length() > 0);
    }
    if (cmd.findOption("--patient-sex"))
    {
      app.checkValue(cmd.getValue(patientSex_));
      app.checkConflict("--patient-sex", "--study-from or --series-from", seriesFile_.length() > 0);
    }

    // initialize default for --series-from
    if (seriesFile_.length() > 0 && readSeriesInfo_) increment_ = OFTrue;

    cmd.beginOptionBlock();
    if (cmd.findOption("--instance-one"))
    {
      app.checkConflict("--instance-one", "--series-from", (seriesFile_.length() > 0) && readSeriesInfo_);
      increment_ = OFFalse;
      instance_ = 1;
    }
    if (cmd.findOption("--instance-inc"))
    {
      app.checkDependence("--instance-inc", "--series-from", (seriesFile_.length() > 0) && readSeriesInfo_);
      increment_ = OFTrue;
    }
    if (cmd.findOption("--instance-set"))
    {
      increment_ = OFFalse;
      app.checkValue(cmd.getValueAndCheckMin(instance_, 1));
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--annotation-yes"))
    {
      annotation_ = OFTrue;
    }
    if (cmd.findOption("--annotation-no"))
    {
      annotation_ = OFFalse;
    }
    cmd.endOptionBlock();

    // ---------- parse CDA processing options ----------
    cmd.beginOptionBlock();
    if (cmd.findOption("--override"))
    {
      app.checkConflict("--override", "--filetype-pdf", ftype_ == DT_pdfDocument);
      app.checkConflict("--override", "--filetype-stl", ftype_ == DT_stlDocument);
      app.checkConflict("--override", "--filetype-stl", ftype_ == DT_mtlDocument);
      app.checkConflict("--override", "--filetype-stl", ftype_ == DT_objDocument);
      override_ = OFTrue;
    }
    if (cmd.findOption("--no-override"))
    {
      app.checkConflict("--no-override", "--filetype-pdf", ftype_ == DT_pdfDocument);
      app.checkConflict("--no-override", "--filetype-stl", ftype_ == DT_stlDocument);
      app.checkConflict("--no-override", "--filetype-stl", ftype_ == DT_mtlDocument);
      app.checkConflict("--no-override", "--filetype-stl", ftype_ == DT_objDocument);
      override_ = OFFalse;
    }
    cmd.endOptionBlock();

    // ---------- parse device data options ----------

    if (cmd.findOption("--manufacturer"))
    {
        app.checkValue(cmd.getValue(manufacturer_));
    }
    if (cmd.findOption("--manufacturer-model"))
    {
        app.checkValue(cmd.getValue(manufacturerModelName_));
    }
    if (cmd.findOption("--device-serial"))
    {
        app.checkValue(cmd.getValue(deviceSerialNumber_));
    }
    if (cmd.findOption("--software-versions"))
    {
        app.checkValue(cmd.getValue(softwareVersions_));
    }

    // ---------- parse STL processing options ----------

    if (cmd.findOption("--measurement-units"))
    {
        app.checkConflict("--measurement-units", "--filetype-cda", ftype_ == DT_cdaDocument);
        app.checkConflict("--measurement-units", "--filetype-pdf", ftype_ == DT_pdfDocument);
        app.checkValue(cmd.getValue(measurementUnitsCSD_));
        app.checkValue(cmd.getValue(measurementUnitsCV_));
        app.checkValue(cmd.getValue(measurementUnitsCM_));
    }

    // ---------- parse input file format options ----------

    cmd.beginOptionBlock();
    if (cmd.findOption("--write-xfer-little")) oxfer_ = EXS_LittleEndianExplicit;
    if (cmd.findOption("--write-xfer-big")) oxfer_ = EXS_BigEndianExplicit;
    if (cmd.findOption("--write-xfer-implicit")) oxfer_ = EXS_LittleEndianImplicit;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--group-length-remove")) oglenc_ = EGL_withoutGL;
    if (cmd.findOption("--group-length-create")) oglenc_ = EGL_withGL;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--length-explicit")) oenctype_ = EET_ExplicitLength;
    if (cmd.findOption("--length-undefined")) oenctype_ = EET_UndefinedLength;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--padding-retain"))
    {
      app.checkConflict("--padding-retain", "--write-dataset",
                        writeMode_ == EWM_dataset);
      opadenc_ = EPD_noChange;
    }
    if (cmd.findOption("--padding-off")) opadenc_ = EPD_withoutPadding;
    if (cmd.findOption("--padding-create"))
    {
      app.checkConflict("--padding-create", "--write-dataset",
                        writeMode_ == EWM_dataset);
      app.checkValue(cmd.getValueAndCheckMin(filepad_, 0));
      app.checkValue(cmd.getValueAndCheckMin(itempad_, 0));
      opadenc_ = EPD_withPadding;
    }
    cmd.endOptionBlock();

    // create override attribute dataset
    overrideKeys_.clear();
    if (cmd.findOption("--key", 0, OFCommandLine::FOM_FirstFromLeft))
    {
      const char *ovKey = NULL;
      do
      {
        app.checkValue(cmd.getValue(ovKey));
        overrideKeys_.push_back(ovKey);
      } while (cmd.findOption("--key", 0, OFCommandLine::FOM_NextFromLeft));
    }

}


OFCondition DcmEncapsulatedDocument::createIdentifiers()
{
  char buf[100];
  OFCondition cond = EC_Normal;
  Sint32 incrementedInstance = 0;
  if (seriesFile_.length() > 0)
  {
    DCMDATA_DEBUG("Reading series file");
    DcmFileFormat dfile;
    cond = dfile.loadFile(seriesFile_, EXS_Unknown, EGL_noChange);

    if (cond.good())
    {
      const char *c = NULL;
      DcmDataset *dset = dfile.getDataset();
      if (dset)
      {
        DCMDATA_TRACE("reading patient attributes");
        c = NULL;
        if (dset->findAndGetString(DCM_PatientName, c).good() && c)
        {
          patientName_ = c;
        }
        c = NULL;
        if (dset->findAndGetString(DCM_PatientID, c).good() && c)
        {
          patientID_ = c;
        }
        c = NULL;
        if (dset->findAndGetString(DCM_PatientBirthDate, c).good() && c)
        {
          patientBirthdate_ = c;
        }
        c = NULL;
        if (dset->findAndGetString(DCM_PatientSex, c).good() && c)
        {
          patientSex_ = c;
        }
        DCMDATA_TRACE("reading study attributes");
        c = NULL;
        if (dset->findAndGetString(DCM_StudyInstanceUID, c).good() && c)
        {
          studyUID_ = c;
        }
        c = NULL;
        if (dset->findAndGetString(DCM_SpecificCharacterSet, c).good() && c)
        {
          specificCharSet_ = c;
        }
        DCMDATA_TRACE("reading series attributes");
        if (readSeriesInfo_)
        {
          c = NULL;
          if (dset->findAndGetString(DCM_SeriesInstanceUID, c).good() && c)
          {
            seriesUID_ = c;
          }
          c = NULL;
          if (dset->findAndGetString(DCM_Modality, c).good() && c)
          {
            modality_ = c;
          }
          if (dset->findAndGetSint32(DCM_InstanceNumber,
                                     incrementedInstance).good())
          {
            ++incrementedInstance;
          }
          else
          {
            incrementedInstance = 0;
          }
          if (increment_) instance_ = incrementedInstance;
        }

        DCMDATA_TRACE("reading reading device attributes");
        c = NULL;
        if (dset->findAndGetString(DCM_Manufacturer, c).good() && c)
        {
          manufacturer_ = c;
        }
        c = NULL;
        if (dset->findAndGetString(DCM_ManufacturerModelName, c).good() && c)
        {
          manufacturerModelName_ = c;
        }
        c = NULL;
        if (dset->findAndGetString(DCM_DeviceSerialNumber, c).good() && c)
        {
          deviceSerialNumber_ = c;
        }
        c = NULL;
        if (dset->findAndGetString(DCM_SoftwareVersions, c).good() && c)
        {
          softwareVersions_ = c;
        }

        if (ftype_ == DT_stlDocument || ftype_ == DT_mtlDocument || ftype_ == DT_objDocument)
        {
          DCMDATA_TRACE("reading STL/MTL/OBJ specific information");
          c = NULL;
          if (dset->findAndGetString(DCM_FrameOfReferenceUID, c).good() && c)
          {
            frameOfReferenceUID_ = c;
          }
          c = NULL;
          if (dset->findAndGetString(DCM_PositionReferenceIndicator, c).good() && c)
          {
            positionReferenceIndicator_ = c;
          }

        }
      }
    }
    else
    {
        DCMDATA_ERROR("Error reading series file '" << seriesFile_ << "': " << cond.text());
        return cond;
    }
  }

  if (studyUID_.empty())
  {
    dcmGenerateUniqueIdentifier(buf, SITE_STUDY_UID_ROOT);
    studyUID_ = buf;
  }
  if (seriesUID_.empty())
  {
    dcmGenerateUniqueIdentifier(buf, SITE_SERIES_UID_ROOT);
    seriesUID_ = buf;
  }

  return cond;
}


OFCondition DcmEncapsulatedDocument::formatSpecificProcessing()
{
    if (ftype_ == DT_cdaDocument)
    {
        // so far we only have format specific processing code for CDA
        return getCDAData();
    }
    return EC_Normal;
}

static size_t skipWhitespaceAndCommentLines(const char *buf, size_t buflen)
{
    size_t i=0;
    while (OFTrue)
    {
        // skip whitespace at beginning of line
        while (i < buflen && (buf[i]==' ' || buf[i]=='\t')) ++i;

        // skip comment line
        if ((i < buflen) && (buf[i] == '#'))
        {
          // skip to the end of the comment line
          while (i < buflen && (buf[i] != '\r' && buf[i] != '\n')) ++i;
        }
        else if (i < buflen && (buf[i]=='\r' || buf[i]=='\n'))
        {
            // skip empty line
            ++i;
        }
        else return i; // end of buffer or keyword found
    }
}

static OFString getNextKeyword(const char *buf)
{
    size_t i=0;
    char c = buf[i];
    while ((c != ' ') && (c != '\t') && (c != '\0') && (c != '\r') && (c != '\n'))
    {
      ++i;
      c = buf[i];
    }
    return OFString(buf, i);
}

static const char *objKeywords[] = {
    "bevel", "bmat", "c_interp", "con", "cstype", "ctech", "curv", "curv2",
    "d_interp", "deg", "end", "f", "g", "hole", "l", "lod", "mg", "mtllib",
    "o", "p", "parm", "s", "scrv", "shadow_obj", "sp", "stech", "step",
    "surf", "trace_obj", "trim", "usemtl", "v", "vn", "vp", "vt"
};


OFCondition DcmEncapsulatedDocument::insertEncapsulatedDocument()
{
#define INSERTBUFFERLENGTH 4096
    char buf[INSERTBUFFERLENGTH+1];
    size_t fileSize = 0;
    size_t buflen = INSERTBUFFERLENGTH;
    struct stat fileStat;
    DcmDataset *dataset = dfile_.getDataset();

    // determine length of document file
    if (0 == stat(ifname_.c_str(), &fileStat))
    {
      fileSize = OFstatic_cast(size_t, fileStat.st_size);
    }
    else
    {
      DCMDATA_ERROR("file " << ifname_ << " not found");
      return EC_InvalidStream;
    }
    if (fileSize == 0)
    {
      DCMDATA_ERROR("file " << ifname_ << " is empty");
      return EC_InvalidStream;
    }

    // read the first 4096 bytes (or the full document if shorter)
    FILE *encapfile = fopen(ifname_.c_str(), "rb");
    if (encapfile == NULL)
    {
      DCMDATA_ERROR("unable to read file " << ifname_);
      return EC_InvalidStream;
    }
    if (fileSize < buflen)
    {
      buflen = fileSize;
    }
    if (buflen != fread(buf, 1, buflen, encapfile))
    {
      DCMDATA_ERROR("read error in file " << ifname_);
      fclose(encapfile);
      return EC_InvalidStream;
    }

    // null terminate buffer
    buf[buflen] = '\0';

    // if the user has not specified the file type, determine it from the file content

    // check for PDF document type
    if (ftype_ == DT_unknownDocument)
    {
        // If the document starts with "%PDF-", it is a PDF document
        if ((buflen > 5) && (0 == strncmp("%PDF-", buf, 5))) ftype_ = DT_pdfDocument;
    }

    // check for CDA document type
    if (ftype_ == DT_unknownDocument)
    {
        size_t i = 0;
        // in a CDA document, the first non-whitespace character is "<"
        while (i < buflen && OFStandard::isspace(buf[i])) ++i;
        // we also check for the presence of the keyword "ClinicalDocument"
        if ((i < buflen && buf[i] == '<') && (NULL != strstr(buf, "ClinicalDocument"))) ftype_ = DT_cdaDocument;
    }

    // check for binary STL document type
    if (ftype_ == DT_unknownDocument)
    {
        // See comments in the STL section below for explanation of the identification algorithm
        if (fileSize >= 84)
        {
            const size_t facetSize32 = 12 * sizeof(Float32) + sizeof(Uint16);
            const size_t facetSize64 = 12 * sizeof(Float64) + sizeof(Uint16);

            // the number of triangles is stored as a 32-bit little endian integer at byte offset 80 in the file
            Uint32 nTriangles = 0;
            for (int j = 3; j >= 0; --j)
            {
                nTriangles = (nTriangles << 8) + OFstatic_cast(unsigned char, buf[80 + j]);
            }
            if (fileSize == (84 + nTriangles * facetSize32) || fileSize == (84 + nTriangles * facetSize64)) ftype_ = DT_stlDocument;
        }
    }

    // check for MTL document type
    if (ftype_ == DT_unknownDocument)
    {
        size_t i = skipWhitespaceAndCommentLines(buf, buflen);
        if (i < buflen)
        {
          OFString keyword = getNextKeyword(buf+i);
          if (keyword == "newmtl") ftype_ = DT_mtlDocument;
        }
    }

    // check for OBJ document type
    if (ftype_ == DT_unknownDocument)
    {
        size_t i = skipWhitespaceAndCommentLines(buf, buflen);
        if (i < buflen)
        {
          OFString keyword = getNextKeyword(buf+i);
          size_t numKeywords = sizeof(objKeywords)/sizeof(const char *);
          for (size_t j=0; j < numKeywords; ++j)
          {
              if (keyword == objKeywords[j])
              {
                  ftype_ = DT_objDocument;
                  break;
              }
          }
        }
    }

    // check if we have determined the file type, bail out otherwise
    if (ftype_ == DT_unknownDocument)
    {
        DCMDATA_ERROR("file " << ifname_ << " is of unknown file type");
        fclose(encapfile);
        return EC_InvalidStream;
    }

    OFBool found;
    char *version;

    // Each STL facet contains:
    //  - Normals: 3 floats (4 bytes)
    //  - Vertices: 3x floats (4 byte each, 12 bytes total)
    //  - AttributeCount: 1 short (2 bytes)
    // Total: 50 bytes per facet
    const size_t facetSize32 = 12 * sizeof(Float32) + sizeof(Uint16);
    const size_t facetSize64 = 12 * sizeof(Float64) + sizeof(Uint16);
    size_t i;

    switch (ftype_)
    {
        case DT_pdfDocument: // special handling for PDF documents
            // check magic word for PDF file
            if (0 != strncmp("%PDF-", buf, 5))
            {
              DCMDATA_ERROR("file " << ifname_ << " is not a PDF file");
              fclose(encapfile);
              return EC_InvalidStream;
            }
            // check PDF version number
            version = buf + 5;
            found = OFFalse;
            for (i = 0; i < 5; ++i)
            {
              if (version[i] == 10 || version[i] == 13)
              {
                version[i] = 0; // insert end of string
                found = OFTrue;
                break;
              }
            }
            if (!found)
            {
              DCMDATA_ERROR("file " << ifname_ << ": unable to decode PDF version number");
              fclose(encapfile);
              return EC_InvalidStream;
            }
            DCMDATA_INFO("file " << ifname_ << ": PDF " << version << ", " << (fileSize + 1023) / 1024 << "kB");
            break;

        case DT_cdaDocument: // special handling for CDA documents
            // proper xml validation occurs later
            DCMDATA_INFO("file " << ifname_ << ": HL7 CDA file (XML Format)" << ", " << (fileSize + 1023) / 1024 << "kB");
            break;

        case DT_stlDocument: // special handling for STL documents
            // Check for an ASCII STL document (which is not permitted in DICOM)
            if (fileSize < 15)
            {
              // "solid " and "endsolid " markers for an ASCII file
              DCMDATA_ERROR("The STL file is not long enough" << " (" << fileSize << "kB)");
              fclose(encapfile);
              return EC_InvalidStream;
            }

            // Binary STL files should never start with "solid ", but just in case, check for ASCII,
            // and if not valid then check for binary... Look for text "solid " in first 6 bytes,
            // indicating the possibility that this is an ASCII STL format.
            if (0 == strncmp("solid ", buf, 6))
            {
              DCMDATA_ERROR("File " << ifname_ << " starts with 'solid '. It is an STL file but it is in ASCII and DICOM only permits binary STL");
              return EC_InvalidStream;
            }
            else
            {
              // STL validation for Binary Format
              DCMDATA_DEBUG("Validating binary STL file");
              // 80-byte header + 4-byte "number of triangles" for a binary file
              if (fileSize < 84)
              {
                DCMDATA_ERROR("The binary STL file is not long enough" << " (" << fileSize << "kB)");
                fclose(encapfile);
                return EC_InvalidStream;
              }
              // Header is from bytes 0-79
              // The number of Triangles starts at byte offset 80 and is a
              // unsigned 32-bit integer in little endian byte order
              Uint32 nTriangles = 0;
              for (int j = 3; j >= 0; --j)
                nTriangles = (nTriangles << 8) + OFstatic_cast(unsigned char, buf[80 + j]);

              // Verify that file size equals the sum of
              // header + nTriangles value + all triangles
              DCMDATA_DEBUG("verifying if the file size is consistent");
              if (fileSize == (84 + nTriangles * facetSize32) ||
                  fileSize == (84 + nTriangles * facetSize64))
              {
                DCMDATA_INFO("file " << ifname_ << ": binary STL, " << nTriangles << " triangles");
                DCMDATA_DEBUG("The binary STL file is "
                        << fileSize << " bytes (expected "
                        << (84 + nTriangles * facetSize32) << " bytes for x86 or "
                        << (84 + nTriangles * facetSize64) << " bytes for x64), number of triangles: " << nTriangles);
              }
              else
              {
                DCMDATA_ERROR("The binary STL file is not consistent. Expected "
                        << (84 + nTriangles * facetSize32) << " bytes for x86 or "
                        << (84 + nTriangles * facetSize64) << " bytes for x64");
                fclose(encapfile);
                return EC_InvalidStream;
              }
            }
            break;

        case DT_mtlDocument: // special handling for MTL documents
            i = skipWhitespaceAndCommentLines(buf, buflen);
            if (i < buflen)
            {
                OFString keyword = getNextKeyword(buf+i);
                if (keyword != "newmtl")
                {
                    DCMDATA_ERROR("Not a valid MTL file. Expected 'newmtl' as first keyword, but found '" << keyword << "'");
                    fclose(encapfile);
                    return EC_InvalidStream;
                }
            }
            else
            {
                DCMDATA_ERROR("Not a valid MTL file. No keyword found");
                fclose(encapfile);
                return EC_InvalidStream;
            }
            DCMDATA_INFO("file " << ifname_ << ": Wavefront MTL");
            break;

        case DT_objDocument: // special handling for OBJ documents
            i = skipWhitespaceAndCommentLines(buf, buflen);
            if (i < buflen)
            {
                OFString keyword = getNextKeyword(buf+i);
                size_t numKeywords = sizeof(objKeywords)/sizeof(const char *);
                found = OFFalse;
                for (size_t j=0; j < numKeywords; ++j)
                {
                    if (keyword == objKeywords[j])
                    {
                        found = OFTrue;
                        break;
                    }
                }
                if (!found)
                {
                    DCMDATA_ERROR("Not a valid OBJ file. Unsupported keyword '" << keyword << "' found");
                    fclose(encapfile);
                    return EC_InvalidStream;
                }
            }
            else
            {
                DCMDATA_ERROR("Not a valid OBJ file. No keyword found");
                fclose(encapfile);
                return EC_InvalidStream;
            }
            DCMDATA_INFO("file " << ifname_ << ": Wavefront OBJ");
            break;

        case DT_unknownDocument:
            DCMDATA_ERROR("Unsupported filetype. This should not happen.");
            fclose(encapfile);
            return EC_InvalidStream;
            break;
    }

    // seek to start of file
    if (0 != fseek(encapfile, 0, SEEK_SET))
    {
      DCMDATA_ERROR("file " << ifname_ << ": seek error");
      fclose(encapfile);
      return EC_InvalidStream;
    }

    OFCondition result = EC_Normal;
    DcmPolymorphOBOW *elem = new DcmPolymorphOBOW(DCM_EncapsulatedDocument);
    if (elem)
    {
      size_t numBytes = fileSize;

      // Store Encapsulated Document Length, according to CP 1851
      result = dataset->putAndInsertUint32(DCM_EncapsulatedDocumentLength, OFstatic_cast(Uint32, numBytes));

      // allocate an even number of bytes
      if (numBytes & 1) ++numBytes;
      Uint8 *bytes = NULL;
      if (result.good())
      {
        result = elem->createUint8Array(OFstatic_cast(Uint32, numBytes), bytes);
      }
      else
      {
        return result;
      }

      // read file content into Encapsulated Document attribute
      if (result.good())
      {
        // blank pad byte
        bytes[numBytes - 1] = 0;
        // read file content
        if (fileSize != fread(bytes, 1, fileSize, encapfile))
        {
          DCMDATA_ERROR("read error in file " << ifname_);
          return result;
        }
      }
      else
      {
        return EC_MemoryExhausted;
      }
    }
    else
    {
      fclose(encapfile);
      return EC_MemoryExhausted;
    }
    // if successful, insert Encapsulated Document element into dataset
    if (result.good())
    {
      result = dataset->insert(elem);
    }
    else
    {
      delete elem;
      DCMDATA_ERROR("Unsuccessful, did not insert element.");
      return result;
    }

    // close file end return
    fclose(encapfile);
    return result;
}

OFCondition DcmEncapsulatedDocument::addFrameOfReferenceModule(DcmItem *dataset)
{
    DCMDATA_TRACE("Validating Frame of Reference UID value");
    char buf [100];
    if (frameOfReferenceUID_.empty())
    {
        DCMDATA_DEBUG("Frame of Reference UID " << DCM_FrameOfReferenceUID << " value missing, generating a new one."
        );
        dcmGenerateUniqueIdentifier(buf, SITE_SERIES_UID_ROOT);
        frameOfReferenceUID_ = buf;
    }
    else
    {
        if (DcmUniqueIdentifier::checkStringValue(frameOfReferenceUID_, "1").bad())
        {
            DCMDATA_DEBUG("Frame of Reference UID " << DCM_FrameOfReferenceUID << " value was faulty, generating a new one."
            );
            dcmGenerateUniqueIdentifier(buf, SITE_SERIES_UID_ROOT);
            frameOfReferenceUID_ = buf;
        }
    }
    DCMDATA_TRACE("Inserting Frame of Reference info to dataset");
    OFCondition result = dataset->putAndInsertOFStringArray(DCM_FrameOfReferenceUID, frameOfReferenceUID_);

    if (result.good())
        result = dataset->putAndInsertOFStringArray(DCM_PositionReferenceIndicator, positionReferenceIndicator_);

    return result;
}


OFCondition DcmEncapsulatedDocument::addEnhancedGeneralEquipmentModule(DcmItem *dataset)
{
    DCMDATA_TRACE("Validating and inserting Enhanced General Equipment fields");
    if (manufacturer_.empty())
    {
        manufacturer_ = "DCMTK_MANUFACTURING";
        DCMDATA_INFO("No Manufacturer " << DCM_Manufacturer << " specified, will use dummy value.");
    }
    OFCondition result = dataset->putAndInsertOFStringArray(DCM_Manufacturer, manufacturer_);
    if (result.good())
    {
        if (manufacturerModelName_.empty())
        {
            manufacturerModelName_ = "DCMTK_3DMODEL";
            DCMDATA_INFO("No Manufacturer Model Name " << DCM_ManufacturerModelName << " specified, will use dummy value.");
        }
        result = dataset->putAndInsertOFStringArray(DCM_ManufacturerModelName, manufacturerModelName_);
    }
    if (result.good())
    {
        if (deviceSerialNumber_.empty())
        {
            deviceSerialNumber_ = "DCMTK_1234567890";
            DCMDATA_INFO("No Device Serial Number " << DCM_DeviceSerialNumber << " specified, will use dummy value.");
        }
        result = dataset->putAndInsertOFStringArray(DCM_DeviceSerialNumber, deviceSerialNumber_);
    }
    if (result.good())
    {
        if (softwareVersions_.empty())
        {
            softwareVersions_ = OFFIS_DCMTK_VERSION;
            DCMDATA_INFO("No Software Versions " << DCM_SoftwareVersions << " specified, will use dummy value.");
        }
        result = dataset->putAndInsertOFStringArray(DCM_SoftwareVersions, softwareVersions_);
    }
    return result;
}


OFCondition DcmEncapsulatedDocument::addManufacturing3DModelModule(DcmItem *dataset)
{
    OFCondition result = EC_Normal;

    if ((measurementUnitsCSD_.length() > 0) && (measurementUnitsCV_.length() > 0) && (measurementUnitsCM_.length() > 0))
    {
      result = DcmCodec::insertCodeSequence(dataset, DCM_MeasurementUnitsCodeSequence,
                                            measurementUnitsCSD_.c_str(),
                                            measurementUnitsCV_.c_str(),
                                            measurementUnitsCM_.c_str());
    }
    else
    {
      DCMDATA_DEBUG("Measurement Units Code Sequence "
              << DCM_MeasurementUnitsCodeSequence
              << " had one or more empty values, generating default values."
      );
      result = DcmCodec::insertCodeSequence(dataset, DCM_MeasurementUnitsCodeSequence, "UCUM", "um", "um");
    }
    return result;
}


OFCondition DcmEncapsulatedDocument::createHeader()
{
    OFCondition result = EC_Normal;
    DcmDataset *dataset = dfile_.getDataset();

    char buf[80];
    // insert empty type 2 attributes
    if (result.good()) result = dataset->insertEmptyElement(DCM_StudyDate);
    if (result.good()) result = dataset->insertEmptyElement(DCM_StudyTime);
    if (result.good()) result = dataset->insertEmptyElement(DCM_AccessionNumber);
    if (result.good()) result = dataset->insertEmptyElement(DCM_ReferringPhysicianName);
    if (result.good()) result = dataset->insertEmptyElement(DCM_StudyID);
    if (result.good()) result = dataset->insertEmptyElement(DCM_ContentDate);
    if (result.good()) result = dataset->insertEmptyElement(DCM_ContentTime);
    if (result.good()) result = dataset->insertEmptyElement(DCM_AcquisitionDateTime);
    if (result.good())
    {
      if ((conceptCSD_.length() > 0) && (conceptCV_.length() > 0) && (conceptCM_.length() > 0))
      {
          result = DcmCodec::insertCodeSequence(dataset, DCM_ConceptNameCodeSequence,
                                                conceptCSD_.c_str(),
                                                conceptCV_.c_str(),
                                                conceptCM_.c_str());
      }
      else
      {
          result = dataset->insertEmptyElement(DCM_ConceptNameCodeSequence);
      }
    }

    // there is no way we could determine a meaningful series number, so we just use a constant.
    if (result.good()) result = dataset->putAndInsertString(DCM_SeriesNumber, "1");

    // insert variable value attributes
    if (result.good()) result = dataset->putAndInsertString(DCM_DocumentTitle, documentTitle_.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientName, patientName_.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientID, patientID_.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientBirthDate, patientBirthdate_.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_PatientSex, patientSex_.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_BurnedInAnnotation, annotation_ ? "YES" : "NO");
    OFStandard::snprintf(buf, sizeof(buf), "%ld", OFstatic_cast(long, instance_));
    if (result.good()) result = dataset->putAndInsertString(DCM_InstanceNumber, buf);
    dcmGenerateUniqueIdentifier(buf, SITE_INSTANCE_UID_ROOT);
    if (result.good()) result = dataset->putAndInsertString(DCM_StudyInstanceUID, studyUID_.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_SeriesInstanceUID, seriesUID_.c_str());
    if (result.good()) result = dataset->putAndInsertString(DCM_SOPInstanceUID, buf);
    // set instance creation date and time
    OFString s;
    if (result.good()) result = DcmDate::getCurrentDate(s);
    if (result.good()) result = dataset->putAndInsertOFStringArray(DCM_InstanceCreationDate, s);
    if (result.good()) result = DcmTime::getCurrentTime(s);
    if (result.good()) result = dataset->putAndInsertOFStringArray(DCM_InstanceCreationTime, s);

    // Device data. Special handling of these attributes for STL/MTL/OBJ,
    // where they are type 1, will be provided later in addEnhancedGeneralEquipmentModule()
    if (result.good() && (manufacturer_.length() > 0))
        result = dataset->putAndInsertOFStringArray(DCM_Manufacturer, manufacturer_.c_str());
        else result = dataset->insertEmptyElement(DCM_Manufacturer);
    if (result.good() && (manufacturerModelName_.length() > 0)) result = dataset->putAndInsertOFStringArray(DCM_ManufacturerModelName, manufacturerModelName_.c_str());
    if (result.good() && (deviceSerialNumber_.length() > 0)) result = dataset->putAndInsertOFStringArray(DCM_DeviceSerialNumber, deviceSerialNumber_.c_str());
    if (result.good() && (softwareVersions_.length() > 0)) result = dataset->putAndInsertOFStringArray(DCM_SoftwareVersions, softwareVersions_.c_str());

    //insert encapsulated file storage UID and additional attributes required per SOP class
    if (result.good())
    {
        switch (ftype_)
        {
            case DT_pdfDocument:
                DCMDATA_TRACE("Inserting SOPClassUID to dataset");
                result = dataset->putAndInsertString(DCM_SOPClassUID, UID_EncapsulatedPDFStorage);
                if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "application/pdf");
                if (result.good())
                {
                    if ((modality_.length() > 0) && (modality_ != "DOC"))
                    {
                        DCMDATA_ERROR("Cannot use series information from '" << seriesFile_ << "': modality mismatch, expected 'DOC', found '" << modality_ << "'");
                        result = EC_InvalidValue;
                    }
                    else result = dataset->putAndInsertString(DCM_Modality, "DOC");
                }
                if (result.good()) result = dataset->putAndInsertString(DCM_ConversionType, "WSD");
                if (result.good() && (specificCharSet_.length() > 0)) result = dataset->putAndInsertString(DCM_SpecificCharacterSet, specificCharSet_.c_str());
                break;

            case DT_cdaDocument:
                DCMDATA_TRACE("Inserting SOPClassUID to dataset");
                result = dataset->putAndInsertString(DCM_SOPClassUID, UID_EncapsulatedCDAStorage);
                if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "text/XML");
                if (result.good())
                {
                    if ((modality_.length() > 0) && (modality_ != "DOC"))
                    {
                        DCMDATA_ERROR("Cannot use series information from '" << seriesFile_ << "': modality mismatch, expected 'DOC', found '" << modality_ << "'");
                        result = EC_InvalidValue;
                    }
                    else result = dataset->putAndInsertString(DCM_Modality, "DOC");
                }
                if (result.good()) result = dataset->putAndInsertString(DCM_ConversionType, "WSD");
                // Patient Name and Patient ID are guaranteed to be in UTF-8 (ISO_IR 192) in the CDA document
                // and no other attributes from the series file are affected by character set issues
                if ((! override_) || (specificCharSet_.length() == 0) ) specificCharSet_ = "ISO_IR 192";
                if (result.good()) result = dataset->putAndInsertString(DCM_SpecificCharacterSet, specificCharSet_.c_str());
                if (result.good() && cda_mediaTypes.length() > 0)
                {
                    result = dataset->putAndInsertString(DCM_ListOfMIMETypes, cda_mediaTypes.c_str());
                }
                if (result.good() && hl7_InstanceIdentifier.length() > 0)
                {
                    result = dataset->putAndInsertString(DCM_HL7InstanceIdentifier, hl7_InstanceIdentifier.c_str());
                }
                break;

            case DT_stlDocument:
                DCMDATA_TRACE("Inserting SOPClassUID to dataset");
                result = dataset->putAndInsertString(DCM_SOPClassUID, UID_EncapsulatedSTLStorage);
                if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "model/stl");
                if (result.good())
                {
                    if ((modality_.length() > 0) && (modality_ != "M3D"))
                    {
                        DCMDATA_ERROR("Cannot use series information from '" << seriesFile_ << "': modality mismatch, expected 'M3D', found '" << modality_ << "'");
                        result = EC_InvalidValue;
                    }
                    else result = dataset->putAndInsertString(DCM_Modality, "M3D");
                }
                if (result.good() && (specificCharSet_.length() > 0)) result = dataset->putAndInsertString(DCM_SpecificCharacterSet, specificCharSet_.c_str());
                if (result.good()) result = addFrameOfReferenceModule(dataset);
                if (result.good()) result = addEnhancedGeneralEquipmentModule(dataset);
                if (result.good()) result = addManufacturing3DModelModule(dataset);
                break;

            case DT_mtlDocument:
                DCMDATA_TRACE("Inserting SOPClassUID to dataset");
                result = dataset->putAndInsertString(DCM_SOPClassUID, UID_EncapsulatedMTLStorage);
                if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "model/mtl");
                if (result.good())
                {
                    if ((modality_.length() > 0) && (modality_ != "M3D"))
                    {
                        DCMDATA_ERROR("Cannot use series information from '" << seriesFile_ << "': modality mismatch, expected 'M3D', found '" << modality_ << "'");
                        result = EC_InvalidValue;
                    }
                    else result = dataset->putAndInsertString(DCM_Modality, "M3D");
                }
                if (result.good() && (specificCharSet_.length() > 0)) result = dataset->putAndInsertString(DCM_SpecificCharacterSet, specificCharSet_.c_str());
                if (result.good()) result = addFrameOfReferenceModule(dataset);
                if (result.good()) result = addEnhancedGeneralEquipmentModule(dataset);
                if (result.good()) result = addManufacturing3DModelModule(dataset);
                break;

            case DT_objDocument:
                DCMDATA_TRACE("Inserting SOPClassUID to dataset");
                result = dataset->putAndInsertString(DCM_SOPClassUID, UID_EncapsulatedOBJStorage);
                if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "model/obj");
                if (result.good())
                {
                    if ((modality_.length() > 0) && (modality_ != "M3D"))
                    {
                        DCMDATA_ERROR("Cannot use series information from '" << seriesFile_ << "': modality mismatch, expected 'M3D', found '" << modality_ << "'");
                        result = EC_InvalidValue;
                    }
                    else result = dataset->putAndInsertString(DCM_Modality, "M3D");
                }
                if (result.good() && (specificCharSet_.length() > 0)) result = dataset->putAndInsertString(DCM_SpecificCharacterSet, specificCharSet_.c_str());
                if (result.good()) result = addFrameOfReferenceModule(dataset);
                if (result.good()) result = addEnhancedGeneralEquipmentModule(dataset);
                if (result.good()) result = addManufacturing3DModelModule(dataset);
                break;

            case DT_unknownDocument:
                break;
        }
    }

    if (result.bad())
    {
        DCMDATA_ERROR("Error while creating DICOM header: " << result.text());
    }
    return result;
}


OFCondition DcmEncapsulatedDocument::applyOverrideKeys()
{
    DcmDataset *outputDset = dfile_.getDataset();
    // replace specific keys by those in overrideKeys, copied from findscu
    OFListConstIterator(OFString) path = overrideKeys_.begin();
    OFListConstIterator(OFString) endOfList = overrideKeys_.end();
    OFCondition cond;
    DcmPathProcessor proc;
    if (path != endOfList)
    {
        DCMDATA_DEBUG("Applying override keys from command line");
    }
    while (path != endOfList)
    {
        cond = proc.applyPathWithValue(outputDset, *path);
        if (cond.bad())
        {
          OFString err;
          err += "Bad override key/path: ";
          err += *path;
          err += ": ";
          err += cond.text();
          DCMDATA_ERROR(err.c_str());
          return makeOFCondition(OFM_dcmdata, 18, OF_error, err.c_str());
        }
        path++;
    }
    return cond;
}


OFCondition DcmEncapsulatedDocument::saveFile()
{
    OFCondition result = dfile_.saveFile(
        ofname_, oxfer_, oenctype_, oglenc_, opadenc_,
        OFstatic_cast(Uint32, filepad_), OFstatic_cast(Uint32, itempad_));

    if (result.bad())
    {
        DCMDATA_ERROR(result.text() << ": writing file: '" << ofname_ << "'");
    }
    return result;
}


OFString DcmEncapsulatedDocument::getInputFileName()
{
    return ifname_;
}


OFString DcmEncapsulatedDocument::getOutputFileName()
{
    return ofname_;
}

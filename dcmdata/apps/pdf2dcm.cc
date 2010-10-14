/*
 *
 *  Copyright (C) 2005-2010, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Convert PDF file to DICOM format
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

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

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "pdf2dcm"

static OFLogger pdf2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

OFCondition createHeader(
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
    if (result.good()) result = dataset->putAndInsertString(DCM_SOPClassUID,          UID_EncapsulatedPDFStorage);
    if (result.good()) result = dataset->putAndInsertString(DCM_Modality,             "OT");
    if (result.good()) result = dataset->putAndInsertString(DCM_ConversionType,       "WSD");
    if (result.good()) result = dataset->putAndInsertString(DCM_MIMETypeOfEncapsulatedDocument, "application/pdf");

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

OFCondition insertPDFFile(
  DcmItem *dataset,
  const char *filename)
{
    size_t fileSize = 0;
    struct stat fileStat;
    char buf[100];

    if (0 == stat(filename, &fileStat)) fileSize = OFstatic_cast(size_t, fileStat.st_size);
    else
    {
      OFLOG_ERROR(pdf2dcmLogger, "file " << filename << " not found");
      return EC_IllegalCall;
    }

    if (fileSize == 0)
    {
      OFLOG_ERROR(pdf2dcmLogger, "file " << filename << " is empty");
      return EC_IllegalCall;
    }

    FILE *pdffile = fopen(filename, "rb");
    if (pdffile == NULL)
    {
      OFLOG_ERROR(pdf2dcmLogger, "unable to read file " << filename);
      return EC_IllegalCall;
    }

    size_t buflen = 100;
    if (fileSize < buflen) buflen = fileSize;
    if (buflen != fread(buf, 1, buflen, pdffile))
    {
      OFLOG_ERROR(pdf2dcmLogger, "read error in file " << filename);
      fclose(pdffile);
      return EC_IllegalCall;
    }

    // check magic word for PDF file
    if (0 != strncmp("%PDF-", buf, 5))
    {
      OFLOG_ERROR(pdf2dcmLogger, "file " << filename << " is not a PDF file");
      fclose(pdffile);
      return EC_IllegalCall;
    }

    // check PDF version number
    char *version = buf + 5;
    OFBool found = OFFalse;
    for (int i = 0; i < 5; ++i)
    {
      if (version[i] == 10 || version[i] == 13)
      {
      	version[i] = 0; // insert end of string
      	found = OFTrue;
      	break;
      }
    }

    if (! found)
    {
      OFLOG_ERROR(pdf2dcmLogger, "file " << filename << ": unable to decode PDF version number");
      fclose(pdffile);
      return EC_IllegalCall;
    }

    OFLOG_INFO(pdf2dcmLogger, "file " << filename << ": PDF " << version << ", " << (fileSize + 1023) / 1024 << "kB");

    if (0 != fseek(pdffile, 0, SEEK_SET))
    {
      OFLOG_ERROR(pdf2dcmLogger, "file " << filename << ": seek error");
      fclose(pdffile);
      return EC_IllegalCall;
    }

    OFCondition result = EC_Normal;
    DcmPolymorphOBOW *elem = new DcmPolymorphOBOW(DCM_EncapsulatedDocument);
    if (elem)
    {
      Uint32 numBytes = fileSize;
      if (numBytes & 1) ++numBytes;
      Uint8 *bytes = NULL;
      result = elem->createUint8Array(numBytes, bytes);
      if (result.good())
      {
        // blank pad byte
      	bytes[numBytes - 1] = 0;

        // read PDF content
        if (fileSize != fread(bytes, 1, fileSize, pdffile))
        {
          OFLOG_ERROR(pdf2dcmLogger, "read error in file " << filename);
          result = EC_IllegalCall;
        }
      }
    } else result = EC_MemoryExhausted;

    // if successful, insert element into dataset
    if (result.good()) result = dataset->insert(elem); else delete elem;

    // close file
    fclose(pdffile);

    return result;
}


void createIdentifiers(
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
      OFLOG_WARN(pdf2dcmLogger, cond.text() << ": reading file: "<< opt_seriesFile);
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
  if (studyUID.length() == 0)
  {
    dcmGenerateUniqueIdentifier(buf, SITE_STUDY_UID_ROOT);
    studyUID = buf;
  }
  if (seriesUID.length() == 0)
  {
    dcmGenerateUniqueIdentifier(buf, SITE_SERIES_UID_ROOT);
    seriesUID = buf;
  }
}


#define SHORTCOL 3
#define LONGCOL 19

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

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

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Convert PDF file to DICOM", rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("pdffile-in",  "PDF input filename to be converted");
  cmd.addParam("dcmfile-out", "DICOM output filename");

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                 "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                        "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

   cmd.addGroup("DICOM document options:");
    cmd.addSubGroup("burned-in annotation:");
      cmd.addOption("--annotation-yes",    "+an",    "PDF contains patient identifying data (default)");
      cmd.addOption("--annotation-no",     "-an",    "PDF does not contain patient identifying data");

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
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
          if (cmd.findOption("--version"))
          {
              app.printHeader(OFTrue /*print host identifier*/);
              ofConsole.lockCout() << OFendl << "External libraries used:";
              ofConsole.unlockCout();
#ifdef WITH_ZLIB
              ofConsole.lockCout() << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
              ofConsole.unlockCout();
#else
              ofConsole.lockCout() << " none" << OFendl;
              ofConsole.unlockCout();
#endif
              return 0;
          }
      }

      /* command line parameters and options */
      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      OFLog::configureFromCommandLine(cmd, app);

      dcmEnableUnknownVRGeneration.set(OFTrue);
      dcmEnableUnlimitedTextVRGeneration.set(OFTrue);

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
    OFLOG_DEBUG(pdf2dcmLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(pdf2dcmLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // read raw file
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(pdf2dcmLogger, "invalid filename: <empty string>");
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

    OFLOG_INFO(pdf2dcmLogger, "creating encapsulated PDF object");

    DcmFileFormat fileformat;

    OFCondition result = insertPDFFile(fileformat.getDataset(), opt_ifname);
    if (result.bad())
    {
        OFLOG_ERROR(pdf2dcmLogger, "unable to create PDF DICOM encapsulation");
        return 10;
    }
    if (result.bad()) return 10;

    // now we need to generate an instance number that is guaranteed to be unique within a series.

    result = createHeader(fileformat.getDataset(), patientName.c_str(), patientID.c_str(),
      patientBirthDate.c_str(), patientSex.c_str(), opt_annotation, studyUID.c_str(),
      seriesUID.c_str(), opt_documentTitle, opt_conceptCSD, opt_conceptCV, opt_conceptCM, OFstatic_cast(Sint32, opt_instance));

    if (result.bad())
    {
        OFLOG_ERROR(pdf2dcmLogger, "unable to create DICOM header: " << result.text());
        return 10;
    }

    OFLOG_INFO(pdf2dcmLogger, "writing encapsulated PDF object as file " << opt_ofname);

    OFCondition error = EC_Normal;

    OFLOG_INFO(pdf2dcmLogger, "Check if new output transfer syntax is possible");

    DcmXfer opt_oxferSyn(opt_oxfer);

    fileformat.getDataset()->chooseRepresentation(opt_oxfer, NULL);
    if (fileformat.getDataset()->canWriteXfer(opt_oxfer))
    {
        OFLOG_INFO(pdf2dcmLogger, "Output transfer syntax " << opt_oxferSyn.getXferName() << " can be written");
    } else {
        OFLOG_ERROR(pdf2dcmLogger, "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
        return 1;
    }

    OFLOG_INFO(pdf2dcmLogger, "write converted DICOM file with metaheader");

    error = fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc,
        OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad));

    if (error.bad())
    {
        OFLOG_ERROR(pdf2dcmLogger, error.text() << ": writing file: " << opt_ofname);
        return 1;
    }

    OFLOG_INFO(pdf2dcmLogger, "conversion successful");

    return 0;
}


/*
** CVS/RCS Log:
** $Log: pdf2dcm.cc,v $
** Revision 1.18  2010-10-14 13:13:30  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.17  2010-08-09 13:04:19  joergr
** Updated data dictionary to 2009 edition of the DICOM standard. From now on,
** the official "keyword" is used for the attribute name which results in a
** number of minor changes (e.g. "PatientsName" is now called "PatientName").
**
** Revision 1.16  2009-11-13 13:20:23  joergr
** Fixed minor issues in log output.
**
** Revision 1.15  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.14  2009-10-12 09:35:43  joergr
** Changed prefix of UIDs created for series and studies (now using constants
** SITE_SERIES_UID_ROOT and SITE_STUDY_UID_ROOT which are supposed to be used
** in these cases).
**
** Revision 1.13  2009-08-11 08:13:35  joergr
** Slightly modified code in order to avoid warning messages on MSVC compiler.
**
** Revision 1.12  2009-08-10 11:04:22  joergr
** Use helper functions checkDependence() and checkConflict() where appropriate.
**
** Revision 1.11  2009-08-10 10:25:23  joergr
** Replaced '\n' by OFendl where appropriate.
**
** Revision 1.10  2009-04-21 14:02:49  joergr
** Fixed minor inconsistencies in manpage / syntax usage.
**
** Revision 1.9  2008-09-25 14:38:48  joergr
** Moved output of resource identifier in order to avoid printing the same
** information twice.
**
** Revision 1.8  2008-09-25 11:19:48  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.7  2008-04-30 12:38:42  meichel
** Fixed compile errors due to changes in attribute tag names
**
** Revision 1.6  2006/08/15 15:50:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.5  2006/07/27 13:52:42  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
** Option "--help" is no longer an exclusive option by default.
**
** Revision 1.4  2005/12/08 15:40:53  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.3  2005/11/28 15:28:54  meichel
** File dcdebug.h is not included by any other header file in the toolkit
**   anymore, to minimize the risk of name clashes of macro debug().
**
** Revision 1.2  2005/10/26 13:33:49  joergr
** Slightly modified code to use more of the "new" helper functions.
**
** Revision 1.1  2005/10/25 13:01:02  meichel
** Added new tool pdf2dcm that allows to convert PDF files to DICOM
**   Encapsulated PDF Storage SOP instances.
**
*/

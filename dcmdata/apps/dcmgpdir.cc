/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose:
 *  Make a General Purpose DICOMDIR according to the General Purpose
 *  CD-R Image Interchange Profile (former Supplement 19).
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-06-05 10:08:31 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmgpdir.cc,v $
 *  CVS/RCS Revision: $Revision: 1.48 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include <iostream.h>

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#include <ctype.h>

#ifdef HAVE_LIBC_H
#include <libc.h>
#endif

#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>       /* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

BEGIN_EXTERN_C
/* This #if code is suggested by the gnu autoconf documentation */
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMELEN(dirent) (dirent)->d_namlen
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "ofstring.h"
#include "oflist.h"
#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmgpdir"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default File-Set ID */
#define DEFAULT_FSID "DCMTK_MEDIA_DEMO"
/* default File-Set Descriptor File ID */
#define DEFAULT_FSDFID "README"
/* default Specific Character Set of File-Set Descriptor File */
#define DEFAULT_SCSFSDF "ISO_IR 100"

/* an ISO 9660 format only allows 8 characters in file name */
#define MAX_FNAME_COMPONENT_SIZE 8
/* DICOM only allows max 8 path components in a file name */
#define MAX_FNAME_COMPONENTS 8

OFString ofname = "DICOMDIR";

/* actual File-Set ID */
OFString fsid = DEFAULT_FSID;
/* actual File-Set Descriptor File ID */
OFString fsdfid = ""; /* can be set to DEFAULT_FSDFID during option handling */
/* actual Specific Character Set of File-Set Descriptor File */
OFString scsfsdf = DEFAULT_SCSFSDF;

OFBool verbosemode = OFFalse;
OFBool writeDicomdir = OFTrue;
OFBool appendMode = OFFalse;
OFBool inventAttributes = OFFalse;
OFBool mapFilenames = OFFalse;
OFBool recurseFilesystem = OFFalse;

E_EncodingType lengthEncoding = EET_ExplicitLength;
E_GrpLenEncoding groupLengthEncoding = EGL_withoutGL;

#define SHORTCOL 3
#define LONGCOL 21

// ********************************************

static OFBool
expandFileNames(OFList<OFString>& fileNames, OFList<OFString>& expandedNames);

static OFBool
createDicomdirFromFiles(OFList<OFString>& fileNames);

// ********************************************

static OFBool
isaValidCharSetName(const OFString& cs)
{
    const char* dcmCharSetNames[] = {
        "ISO_IR 100",
        "ISO_IR 101",
        "ISO_IR 109",
        "ISO_IR 110",
        "ISO_IR 144",
        "ISO_IR 127",
        "ISO_IR 126",
        "ISO_IR 138",
        "ISO_IR 148"
    };
    int numberOfDcmCharSetNames =
        (sizeof(dcmCharSetNames) / sizeof(dcmCharSetNames[0]));

    OFBool found = OFFalse;
    for (int i=0; i<numberOfDcmCharSetNames && !found; i++) {
        found = (strcmp(cs.c_str(), dcmCharSetNames[i]) == 0);
    }
    return found;
}

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    int opt_debugMode = 0;
    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Create a general purpose DICOMDIR", rcsid);
    OFCommandLine cmd;

    OFString opt1 = "[i]d: string (default: ";
    opt1 += fsid;
    opt1 += ")";

    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in", "referenced DICOM file", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");

    cmd.addGroup("input options:");
      cmd.addSubGroup("dicomdir identifiers:");
        cmd.addOption("--output-file",            "+D",    1,  "[f]ilename: string",
                                                               "generate specific DICOMDIR file\n(default: DICOMDIR in current directory)");
        cmd.addOption("--fileset-id",             "+F",    1,   opt1.c_str(),
                                                               "use specific file set ID");
        cmd.addOption("--descriptor",             "+R",    1,  "[f]ilename: string",
                                                               "add a file set descriptor file ID\n(e.g. README, default: no descriptor)");
        cmd.addOption("--char-set",               "+C",    1,  "[c]har-set: string",
                                                               "add a specific character set for descriptor\n(default: \"ISO_IR 100\" if descriptor present)");
      cmd.addSubGroup("type 1 attributes:");
        cmd.addOption("--strict",                 "-I",        "exit with error if DICOMDIR type 1 attributes\nare missing in DICOM file (default)");
        cmd.addOption("--invent",                 "+I",        "invent DICOMDIR type 1 attributes\nif missing in DICOM file");
      cmd.addSubGroup("reading:");
        cmd.addOption("--keep-filenames",         "-m",        "expect filenames to be in DICOM format (default)");
        cmd.addOption("--map-filenames",          "+m",        "map to DICOM filenames (lowercase -> uppercase,\nand remove trailing period)");
        cmd.addOption("--no-recurse",             "-r",        "do not recurse within directories (default)");
        cmd.addOption("--recurse",                "+r",        "recurse within filesystem directories");
    cmd.addGroup("output options:");
      cmd.addSubGroup("writing:");
        cmd.addOption("--replace",                "-A",        "replace existing dicomdir (default)");
        cmd.addOption("--append",                 "+A",        "append to existing dicomdir");
        cmd.addOption("--discard",                "-w",        "do not write out dicomdir");
      cmd.addSubGroup("post-1993 value representations:");
        cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
        cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");
      cmd.addSubGroup("group length encoding:");
        cmd.addOption("--group-length-remove",    "-g",        "write without group length elements (default)");
        cmd.addOption("--group-length-create",    "+g",        "write with group length elements");
      cmd.addSubGroup("length encoding in sequences and items:");
        cmd.addOption("--length-explicit",        "+e",        "write with explicit lengths (default)");
        cmd.addOption("--length-undefined",       "-e",        "write with undefined lengths");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {

      if (cmd.findOption("--verbose")) verbosemode = OFTrue;
      if (cmd.findOption("--debug")) opt_debugMode = 5;

      if (cmd.findOption("--output-file")) app.checkValue(cmd.getValue(ofname));
      if (cmd.findOption("--fileset-id")) app.checkValue(cmd.getValue(fsid));
      if (cmd.findOption("--descriptor")) app.checkValue(cmd.getValue(fsdfid));
      if (cmd.findOption("--char-set"))
      {
          app.checkValue(cmd.getValue(scsfsdf));
          if (!isaValidCharSetName(scsfsdf))
          {
            OFString aString = "unknown char-set: ";
            aString += scsfsdf;
            app.printError(aString.c_str());
          }
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--strict")) inventAttributes = OFFalse;
      if (cmd.findOption("--invent")) inventAttributes = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--keep-filenames")) mapFilenames = OFFalse;
      if (cmd.findOption("--map-filenames")) mapFilenames = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--no-recurse")) recurseFilesystem = OFFalse;
      if (cmd.findOption("--recurse")) recurseFilesystem = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--replace"))
      {
        writeDicomdir = OFTrue;
        appendMode = OFFalse;
      }
      if (cmd.findOption("--append"))
      {
        writeDicomdir = OFTrue;
        appendMode = OFTrue;
      }
      if (cmd.findOption("--discard"))
      {
        writeDicomdir = OFFalse;
        appendMode = OFFalse;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-create")) groupLengthEncoding = EGL_withGL;
      if (cmd.findOption("--group-length-remove")) groupLengthEncoding = EGL_withoutGL;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit")) lengthEncoding = EET_ExplicitLength;
      if (cmd.findOption("--length-undefined")) lengthEncoding = EET_UndefinedLength;
      cmd.endOptionBlock();

    }

    const char *current = NULL;
    OFList<OFString> fnames;
    int count = cmd.getParamCount();
    for (int i=1; i<=count; i++)
    {
      cmd.getParam(i, current);
      // OFString fname(current);
      fnames.push_back(current);
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
        CERR << "Error: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
        return 1; /* DcmDicomDir class dumps core when no data dictionary */
    }

    OFBool ok = OFTrue;
    if (recurseFilesystem) {
        OFList<OFString> expandedNames;
        ok = expandFileNames(fnames, expandedNames);
        if (ok) {
            ok = createDicomdirFromFiles(expandedNames);
        }
    } else {
        ok = createDicomdirFromFiles(fnames);
    }
    return (ok)?(0):(1);
}

/*
** Debug assistance
*/

void
dcmPrint(DcmObject *obj)
{
    if (obj != NULL) obj->print(COUT);
}

/*
** Dataset manipulation utilities
*/

static OFBool
dcmTagExists(DcmItem* d, const DcmTagKey& key)
{
    return d->tagExists(key, OFFalse);
}

static OFBool
dcmTagExistsWithValue(DcmItem* d, const DcmTagKey& key)
{
    return d->tagExistsWithValue(key, OFFalse);
}

static OFString
dcmFindString(DcmItem* d, const DcmTagKey& key,
              OFBool searchIntoSub = OFFalse)
{
    OFString s;

    E_Condition ec = d->findOFStringArray(key, s, searchIntoSub);
    if (ec != EC_Normal && ec != EC_TagNotFound) {
        DcmTag tag(key);
        CERR << "dcmFindString: error while finding " << tag.getTagName()
             << " " << key << ": "
             << dcmErrorConditionToString(ec) << endl;
    }

    return s;
}

static OFString
dcmFindStringInFile(const OFString& fname, const DcmTagKey& key,
                    OFBool searchIntoSub = OFFalse)
{
    if (verbosemode) {
        COUT << "investigating file: " << fname << endl;
    }
    DcmFileStream myin(fname.c_str(), DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        CERR << "error : cannot open file: " << fname << endl;
        return OFFalse;
    }
    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_noChange);
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
        CERR << "error: "
             << dcmErrorConditionToString(ff.error())
             << ": reading file: " << fname << endl;
        return OFFalse;
    }

    DcmDataset *d = ff.getDataset();

    OFString s = dcmFindString(d, key, searchIntoSub);

    return s;
}

static OFBool
dcmInsertString(DcmItem* d, const DcmTagKey& key,
                const OFString& s, OFBool replaceOld = OFTrue)
{
    if (d == NULL) {
        CERR << "error: dcmInsertString: null DcmItem argument" << endl;
        return OFFalse;
    }

    DcmTag tag(key);
    DcmElement *elem = newDicomElement(tag);
    E_Condition cond = EC_Normal;

    if (elem == NULL) {
        CERR << "error: dcmInsertString: cannot create DcmElement" << endl;
        return OFFalse;
    }
    if (!s.empty()) {
        cond = elem->putOFStringArray(s);
        if (cond != EC_Normal) {
            CERR << "error: dcmInsertString: cannot put string" << endl;
            return OFFalse;
        }
    }
    cond = d->insert(elem, replaceOld);
    if (cond != EC_Normal) {
        CERR << "error: dcmInsertString: cannot insert element" << endl;
        return OFFalse;
    }

    return (cond == EC_Normal);
}

static OFBool
dcmCopyString(DcmItem* sink, const DcmTagKey& key, DcmItem* source)
{
    return dcmInsertString(sink, key, dcmFindString(source, key));
}

static OFBool
dcmCopyOptString(DcmItem* sink, const DcmTagKey& key, DcmItem* source)
{
    OFBool ok = OFTrue;
    if (dcmTagExists(source, key)) {
        ok = dcmInsertString(sink, key, dcmFindString(source, key));
    }
    return ok;
}

#if 0
/* currently unused */
static OFBool
dcmCopyStringWithDefault(DcmItem* sink, const DcmTagKey& key,
                         DcmItem* source, const char* defaultString)
{
    OFBool ok = OFTrue;
    if (dcmTagExistsWithValue(source, key)) {
        ok = dcmInsertString(sink, key, dcmFindString(source, key));
    } else {
        ok = dcmInsertString(sink, key, defaultString);
    }
    return ok;
}
#endif

static OFBool
dcmCopySequence(DcmItem* sink, const DcmTagKey& key, DcmItem* source)
{
    OFBool ok = OFTrue;
    DcmTag tag(key);

    if (tag.getEVR() != EVR_SQ) {
        CERR << "internal error: dcmCopySequence: "
             << key << " not SQ" << endl;
        abort();
    }

    if (dcmTagExists(source, key)) {
        DcmSequenceOfItems *sq = NULL;
        DcmStack stack;
        E_Condition ec = EC_Normal;

        ec = source->search(key, stack, ESM_fromHere, OFFalse);
        sq = (DcmSequenceOfItems*) stack.top();
        if (ec == EC_Normal && sq != NULL) {
            // make a copy of sq
            DcmSequenceOfItems *sqcopy = new DcmSequenceOfItems(*sq);
            // insert sqcopy into the sink
            ec = sink->insert(sqcopy, OFTrue);
            if (ec != EC_Normal) {
                CERR << "error: dcmCopySequence: cannot insert element" << endl;
                ok = OFFalse;
            }
        } else {
            ok = OFFalse;
        }
    } else {
        CERR << "dcmCopySequence: error while finding " << tag.getTagName() << " " << key << endl;
        ok = OFFalse;
    }
    return ok;
}

static OFBool
dcmCopyOptSequence(DcmItem* sink, const DcmTagKey& key, DcmItem* source)
{
    if (dcmTagExists(source, key)) return dcmCopySequence(sink, key, source);
    return OFTrue;
}

static OFString
currentDate()
{
    char dateBuf[32];
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts == NULL) {
        CERR << "ERROR: cannot get localtime" << endl;
        return "";
    }
    int year = 1900 + ts->tm_year;
    sprintf(dateBuf, "%04d%02d%02d", year, ts->tm_mon + 1, ts->tm_mday);
    return dateBuf;
}

static OFString
currentTime()
{
    char timeBuf[32];
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts == NULL) {
        CERR << "ERROR: cannot get localtime" << endl;
        return "";
    }
    sprintf(timeBuf, "%02d%02d%02d", ts->tm_hour, ts->tm_min, ts->tm_sec);
    return timeBuf;
}

static OFString
alternativeStudyDate(DcmItem* d)
{
    /* use another date if present */
    OFString date = dcmFindString(d, DCM_SeriesDate);
    if (!date.empty()) return date;
    date = dcmFindString(d, DCM_AcquisitionDate);
    if (!date.empty()) return date;
    date = dcmFindString(d, DCM_ContentDate);
    if (!date.empty()) return date;
    /* use current date */
    return currentDate();
}

static OFString
alternativeStudyTime(DcmItem* d)
{
    /* use another time if present */
    OFString timestr = dcmFindString(d, DCM_SeriesTime);
    if (!timestr.empty()) return timestr;
    timestr = dcmFindString(d, DCM_AcquisitionTime);
    if (!timestr.empty()) return timestr;
    timestr = dcmFindString(d, DCM_ContentTime);
    if (!timestr.empty()) return timestr;
    /* use current time */
    return currentTime();
}

static OFString
defaultNumber(int number)
{
    char numbuf[64];
    unsigned long ul = (unsigned long)number;
    sprintf(numbuf, "%lu", ul);
    return numbuf;
}

static OFString
defaultID(const OFString& prefix, int number)
{
    /* use a most 10 chars from prefix */
    OFString idbuf(prefix, 0, 10);
    unsigned long ul = (unsigned long)number;
    char nbuf[16];
    sprintf(nbuf, "%06lu", ul);
    idbuf.append(nbuf);
    return idbuf;
}


/*
** Filename manipulation
*/

static void
hostToDicomFilename(OFString& fname)
{
    /*
    ** Massage filename into dicom format
    ** (dos conventions for path separators)
    */
    OFString newname;
    int len = fname.size();
    for (int i=0; i<len; i++) {
        char c = fname[i];
        /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
        if (c == PATH_SEPARATOR) {
            newname += '\\';
        } else if (isalpha(c) || isdigit(c) || (c == '_') || (c == '\\')) {
            /* filenames in DICOM must always be in uppercase */
            newname += toupper(c);
        }
    }
    fname = newname;
}

static void
dicomToHostFilename(OFString& fname, OFBool mapToLower = OFFalse)
{
    /*
    ** Massage filename into machine format
    ** (replace dos conventions for path separators)
    */
    int len = fname.size();
    for (int i=0; i<len; i++) {
        char c = fname[i];
        /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
        if (c == '\\') {
            c = PATH_SEPARATOR;
        } else {
            if (mapToLower) {
                c = tolower(c);
            }
        }
        fname[i] = c;
    }
}

static int
componentCount(const OFString& fname, char separator = PATH_SEPARATOR)
{
    if (fname.empty()) return 0;
    int n = 1;
    int len = fname.size();
    for (int i=0; i<len; i++) {
        if (fname[i] == separator) {
            n++;
        }
    }
    return n;
}

static OFBool
isComponentTooLarge(const OFString& fname,
                    int componentLimit,
                    char separator = PATH_SEPARATOR)
{
    if (fname.empty()) return -1;
    int n = 0;
    const char*  s = fname.c_str();
    const char* ss = (const char*)strchr(s, separator);
    while (ss) {
        int diff = (ss - s);
        if (diff > componentLimit) {
            return OFTrue;
        }
        s = ss+1;
        ss = (const char*)strchr(s, separator);
        n++;
    }
    int len = strlen(s);
    if (mapFilenames && s[len-1] == '.') {
        /* disregard trailing point */
        len--;
    }
    if (len > componentLimit) {
        return OFTrue;
    }

    return OFFalse;
}

/*
** Check help functions
*/

static OFBool
fileExists(const OFString& fname)
{
    FILE* f = fopen(fname.c_str(), "r");
    if (f == NULL) {
        return OFFalse;
    }
    fclose(f);
    return OFTrue;
}


static OFBool
cmp(const OFString& s1, const OFString& s2)
{
    if (s1.empty() || s2.empty()) {
        return OFFalse;
    }
    return (s1.compare(s2) == 0)?(OFTrue):(OFFalse);
}

static OFBool
checkExists(DcmItem* d, const DcmTagKey& key, const OFString& fname)
{
    if (!dcmTagExists(d, key)) {
        DcmTag tag(key);
        CERR << "error: required attribute " << tag.getTagName()
             << " " << key << " missing in file: "
             << fname << endl;
        return OFFalse;
    }
    return OFTrue;
}

static OFBool
checkExistsWithValue(DcmItem* d, const DcmTagKey& key, const OFString& fname)
{
    if (!checkExists(d, key, fname)) {
        return OFFalse;
    }
    OFString s = dcmFindString(d, key);
    if (s.empty()) {
        DcmTag tag(key);
        CERR << "error: required attribute " << tag.getTagName()
             << " " << key << " has no value in file: "
             << fname << endl;
        return OFFalse;
    }
    return OFTrue;
}

static OFString
recordTypeToName(E_DirRecType t)
{
    const char* s = NULL;
    switch (t) {
    case ERT_root:
        s = "Root"; break;
    case ERT_Curve:
        s = "Curve"; break;
    case ERT_FilmBox:
        s = "FilmBox"; break;
    case ERT_FilmSession:
        s = "FilmSession"; break;
    case ERT_Image:
        s = "Image"; break;
    case ERT_ImageBox:
        s = "ImageBox"; break;
    case ERT_Interpretation:
        s = "Interpretation"; break;
    case ERT_ModalityLut:
        s = "ModalityLUT"; break;
    case ERT_Mrdr:
        s = "MRDR"; break;
    case ERT_Overlay:
        s = "Overlay"; break;
    case ERT_Patient:
        s = "Patient"; break;
    case ERT_PrintQueue:
        s = "PrintQueue"; break;
    case ERT_Private:
        s = "Private"; break;
    case ERT_Results:
        s = "Results"; break;
    case ERT_Series:
        s = "Series"; break;
    case ERT_Study:
        s = "Study"; break;
    case ERT_StudyComponent:
        s = "StudyComponent"; break;
    case ERT_Topic:
        s = "Topic"; break;
    case ERT_Visit:
        s = "Visit"; break;
    case ERT_VoiLut:
        s = "VOILUT"; break;
    case ERT_StructReport:
        s = "StructReport"; break;
    case ERT_Presentation:
        s = "Presentation"; break;
    case ERT_Waveform:
        s = "Waveform"; break;
    case ERT_RTDose:
        s = "RTDose"; break;
    case ERT_RTStructureSet:
        s = "RTStructureSet"; break;
    case ERT_RTPlan:
        s = "RTPlan"; break;
    case ERT_RTTreatRecord:
        s = "RTTreatRecord"; break;
    case ERT_StoredPrint:
        s = "StoredPrint"; break;
    default:
        s = "(unknown-directory-record-type)";
        break;
    }
    return s;
}

/*
** Sanity Checks
*/

static OFBool
checkImage(const OFString& fname, DcmFileFormat *ff)
{
    /*
    ** Do some sanity checks on the file.
    ** - is this a part 10 file format file?
    ** - can this file be part of a General Purpose DICOMDIR?
    ** - does it have the necessary attributes?
    ** - is the transfer syntax valid?
    **
    */

    if (ff == NULL) return OFFalse;

    OFBool ok = OFTrue;

    DcmMetaInfo *m = ff->getMetaInfo();
    if (m == NULL || m->card() == 0) {
        CERR << "error: file not part 10 format (no meta-header): "
             << fname << endl;
        ok = OFFalse;
    }

    DcmDataset *d = ff->getDataset();
    if (d == NULL) {
        CERR << "error: file contains no data (no dataset): "
             << fname << endl;
        /* give up checking */
        return OFFalse;
    }

    /*
    ** Is sop classs ok?
    */
    OFString mediaSOPClassUID = dcmFindString(m, DCM_MediaStorageSOPClassUID);
    if (mediaSOPClassUID.empty()) {
        CERR << "error: MediaStorageSOPClassUID missing in meta-header: "
             << fname << endl;
        ok = OFFalse;
    }
    /*
    ** Check if the SOP Class is a known storage SOP class (an image, overlay,
    ** curve, etc.
    */
    OFBool found = OFFalse;

    /* is it an image ? */
    for (int i=0; i<numberOfDcmImageSOPClassUIDs && !found; i++) {
        found = cmp(mediaSOPClassUID, dcmImageSOPClassUIDs[i]);
    }
    /* is it an overlay/curve/modality_lut/voi_lut etc. ? */
    found = found || cmp(mediaSOPClassUID, UID_StandaloneOverlayStorage);
    found = found || cmp(mediaSOPClassUID, UID_StandaloneCurveStorage);
    found = found || cmp(mediaSOPClassUID, UID_StandaloneModalityLUTStorage);
    found = found || cmp(mediaSOPClassUID, UID_StandaloneVOILUTStorage);
    found = found || cmp(mediaSOPClassUID, UID_PETCurveStorage);
    found = found || cmp(mediaSOPClassUID, UID_GrayscaleSoftcopyPresentationStateStorage);
    found = found || cmp(mediaSOPClassUID, UID_StoredPrintStorage);
    /* is it one of the RT SOP Classes? */
    found = found || cmp(mediaSOPClassUID, UID_RTDoseStorage);
    found = found || cmp(mediaSOPClassUID, UID_RTStructureSetStorage);
    found = found || cmp(mediaSOPClassUID, UID_RTBeamsTreatmentRecordStorage);
    found = found || cmp(mediaSOPClassUID, UID_RTPlanStorage);
    found = found || cmp(mediaSOPClassUID, UID_RTBrachyTreatmentRecordStorage);
    found = found || cmp(mediaSOPClassUID, UID_RTTreatmentSummaryRecordStorage);
    /* is it one of the structured reporting SOP Classes? */
    found = found || cmp(mediaSOPClassUID, UID_BasicTextSR);
    found = found || cmp(mediaSOPClassUID, UID_EnhancedSR);
    found = found || cmp(mediaSOPClassUID, UID_ComprehensiveSR);
    /* is it one of the waveform SOP Classes? */
    found = found || cmp(mediaSOPClassUID, UID_TwelveLeadECGWaveformStorage);
    found = found || cmp(mediaSOPClassUID, UID_GeneralECGWaveformStorage);
    found = found || cmp(mediaSOPClassUID, UID_AmbulatoryECGWaveformStorage);
    found = found || cmp(mediaSOPClassUID, UID_HemodynamicWaveformStorage);
    found = found || cmp(mediaSOPClassUID, UID_CardiacElectrophysiologyWaveformStorage);
    found = found || cmp(mediaSOPClassUID, UID_BasicVoiceAudioWaveformStorage);
    /* a detached patient mgmt sop class is also ok */
    found = found || cmp(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);

    if (!found) {
        OFString sopClassName = dcmFindNameOfUID(mediaSOPClassUID.c_str());
        if (sopClassName.empty()) {
            sopClassName = mediaSOPClassUID;
        }
        CERR << "error: invalid sop class (" << sopClassName
             << ") for STD-GEN-CD profile: " << fname << endl;
        /* give up checking */
        return OFFalse;
    }

    /*
    ** Is transfer syntax ok?
    */
    OFString transferSyntax = dcmFindString(m, DCM_TransferSyntaxUID);
    if (transferSyntax.empty()) {
        CERR << "error: TransferSyntaxUID missing in meta-header: "
             << fname << endl;
        ok = OFFalse;
    }
    found = cmp(transferSyntax, UID_LittleEndianExplicitTransferSyntax);
    if (!found) {
        CERR << "error: LittleEndianExplicitTransferSyntax expected: "
             << fname << endl;
        ok = OFFalse;
    }

    /*
    ** are mandatory attributes for DICOMDIR available and valued?
    */

    /* PatientID is type 1 in DICOMDIR and type 2 in images. */
    if (!inventAttributes) {
        if (!checkExistsWithValue(d, DCM_PatientID, fname)) ok = OFFalse;
    }
    /* PatientsName is type 2 in DICOMDIR and images */
    if (!checkExists(d, DCM_PatientsName, fname)) ok = OFFalse;
    /* StudyDate is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
        if (!checkExistsWithValue(d, DCM_StudyDate, fname)) ok = OFFalse;
    }
    /* StudyTime is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
        if (!checkExistsWithValue(d, DCM_StudyTime, fname)) ok = OFFalse;
    }
    /* StudyDescription is type 2 in DICOMDIR and type 3 im images.
    ** We can create an empty attribute in the directory
    ** if (!checkExists(d, DCM_StudyDescription, fname)) ok = OFFalse;
    */
    /* StudyInstanceUID is type 1 in DICOMDIR and images */
    if (!checkExistsWithValue(d, DCM_StudyInstanceUID, fname)) ok = OFFalse;
    /* StudyID is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
        if (!checkExistsWithValue(d, DCM_StudyID, fname)) ok = OFFalse;
    }
    /* AccessionNumber is type 2 in DICOMDIR and type 3 in images
    ** We can create an empty attribute in the directory
    ** if (!checkExists(d, DCM_AccessionNumber, fname)) ok = OFFalse;
    */
    /* Modality is type 1 in DICOMDIR and type 1 in images */
    if (!checkExistsWithValue(d, DCM_Modality, fname)) ok = OFFalse;
    /* SeriesInstanceUID is type 1 in DICOMDIR and type 1 in images */
    if (!checkExistsWithValue(d, DCM_SeriesInstanceUID, fname)) ok = OFFalse;
    /* SeriesNumber is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
        if (!checkExistsWithValue(d, DCM_SeriesNumber, fname)) ok = OFFalse;
    }
    /* Image etc Numbers are type 1 in DICOMDIR but type 2 in images */
    if (cmp(mediaSOPClassUID, UID_StandaloneOverlayStorage)) {
        /* an overlay */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_OverlayNumber, fname))
                ok = OFFalse;
        }
    } else if (cmp(mediaSOPClassUID, UID_StandaloneModalityLUTStorage)) {
        /* a modality lut */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_LookupTableNumber, fname))
                ok = OFFalse;
        }
    } else if (cmp(mediaSOPClassUID, UID_StandaloneVOILUTStorage)) {
        /* a voi lut */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_LookupTableNumber, fname))
                ok = OFFalse;
        }
    } else if (cmp(mediaSOPClassUID, UID_StandaloneCurveStorage) ||
               cmp(mediaSOPClassUID, UID_PETCurveStorage)) {
        /* a curve */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_CurveNumber, fname))
                ok = OFFalse;
        }

    } else if (cmp(mediaSOPClassUID, UID_BasicTextSR) ||
               cmp(mediaSOPClassUID, UID_EnhancedSR) ||
               cmp(mediaSOPClassUID, UID_ComprehensiveSR)) {
        /* a structured report */
        if (!checkExistsWithValue(d, DCM_InstanceNumber, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_CompletionFlag, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_VerificationFlag, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_ContentDate, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_ContentTime, fname)) ok = OFFalse;
        if (!dcmTagExistsWithValue(d, DCM_ConceptNameCodeSequence))
        {
          DcmTag cncsqtag(DCM_ConceptNameCodeSequence);
          CERR << "error: required attribute " << cncsqtag.getTagName()
               << " " << DCM_ConceptNameCodeSequence << " missing or empty in file: "
               << fname << endl;
          ok = OFFalse;
        }
        OFString verificationFlag(dcmFindString(d, DCM_VerificationFlag));
        if (verificationFlag == "VERIFIED")
        {
          // VerificationDateTime is required if verificationFlag is VERIFIED
          // retrieve most recent (= last) entry from VerifyingObserverSequence
          DcmStack stack;
          if (d->search(DCM_VerifyingObserverSequence, stack, ESM_fromHere, OFFalse) == EC_Normal)
          {
            DcmSequenceOfItems *dseq = (DcmSequenceOfItems *)stack.top();
            if ((dseq != NULL) && (dseq->card() > 0))
            {
              DcmItem *ditem = dseq->getItem(dseq->card() - 1);
              if ((ditem == NULL) || (!checkExistsWithValue(ditem, DCM_VerificationDateTime, fname)))
                ok = OFFalse;
            } else
              ok = OFFalse;
          } else
            ok = OFFalse;
        }
    } else if (cmp(mediaSOPClassUID, UID_GrayscaleSoftcopyPresentationStateStorage)) {
        /* a presentation state */
        if (!checkExistsWithValue(d, DCM_InstanceNumber, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_PresentationLabel, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_PresentationCreationDate, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_PresentationCreationTime, fname)) ok = OFFalse;
        if (!dcmTagExistsWithValue(d, DCM_ReferencedSeriesSequence))
        {
          DcmTag rssqtag(DCM_ReferencedSeriesSequence);
          CERR << "error: required attribute " << rssqtag.getTagName()
               << " " << DCM_ReferencedSeriesSequence << " missing or empty in file: "
               << fname << endl;
          ok = OFFalse;
        }
    } else if (cmp(mediaSOPClassUID, UID_TwelveLeadECGWaveformStorage) ||
               cmp(mediaSOPClassUID, UID_GeneralECGWaveformStorage) ||
               cmp(mediaSOPClassUID, UID_AmbulatoryECGWaveformStorage) ||
               cmp(mediaSOPClassUID, UID_HemodynamicWaveformStorage) ||
               cmp(mediaSOPClassUID, UID_CardiacElectrophysiologyWaveformStorage) ||
               cmp(mediaSOPClassUID, UID_BasicVoiceAudioWaveformStorage)) {
        /* a waveform */
        if (!checkExistsWithValue(d, DCM_InstanceNumber, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_ContentDate, fname)) ok = OFFalse;
        if (!checkExistsWithValue(d, DCM_ContentTime, fname)) ok = OFFalse;
    } else if (cmp(mediaSOPClassUID, UID_RTDoseStorage)) {
        /* a RT dose */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_InstanceNumber, fname))
                ok = OFFalse;
        }
        if (!checkExistsWithValue(d, DCM_DoseSummationType, fname)) ok = OFFalse;
    } else if (cmp(mediaSOPClassUID, UID_RTStructureSetStorage)) {
        /* a RT structure set */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_InstanceNumber, fname))
                ok = OFFalse;
        }
        if (!checkExistsWithValue(d, DCM_StructureSetLabel, fname)) ok = OFFalse;
    } else if (cmp(mediaSOPClassUID, UID_RTPlanStorage)) {
        /* a RT plan */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_InstanceNumber, fname))
                ok = OFFalse;
        }
        if (!checkExistsWithValue(d, DCM_RTPlanLabel, fname)) ok = OFFalse;
    } else if (cmp(mediaSOPClassUID, UID_RTBeamsTreatmentRecordStorage) ||
               cmp(mediaSOPClassUID, UID_RTBrachyTreatmentRecordStorage) ||
               cmp(mediaSOPClassUID, UID_RTTreatmentSummaryRecordStorage)) {
        /* a RT treat record */
        if (!checkExistsWithValue(d, DCM_InstanceNumber, fname)) ok = OFFalse;
    } else if (cmp(mediaSOPClassUID, UID_StoredPrintStorage)) {
        /* a stored print */
        /* (nothing to do) */
    } else {
        /* it can only be an image */
        if (!inventAttributes) {
            if (!checkExistsWithValue(d, DCM_InstanceNumber, fname))
                ok = OFFalse;
        }
    }

    return ok;
}

static
DcmDirectoryRecord* buildPatientRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Patient, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating patient record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_PatientID, d);
    dcmCopyString(rec, DCM_PatientsName, d);

    return rec;
}

DcmDirectoryRecord*
buildStudyRecord(const OFString& referencedFileName, DcmItem* d,
                 const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Study, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating study record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    if (dcmTagExistsWithValue(d, DCM_StudyDate)) {
        dcmCopyString(rec, DCM_StudyDate, d);
    } else {
        OFString altDate = alternativeStudyDate(d);
        CERR << "Warning: " << sourceFileName
             << ": StudyDate missing: using alternative: "
             << altDate << endl;
        dcmInsertString(rec, DCM_StudyDate, altDate);
    }
    if (dcmTagExistsWithValue(d, DCM_StudyTime)) {
        dcmCopyString(rec, DCM_StudyTime, d);
    } else {
        OFString altTime = alternativeStudyTime(d);
        CERR << "Warning: " << sourceFileName
             << ": StudyTime missing: using alternative: "
             << altTime << endl;
        dcmInsertString(rec, DCM_StudyTime, altTime);
    }
    dcmCopyString(rec, DCM_StudyDescription, d);
    dcmCopyString(rec, DCM_StudyInstanceUID, d);
    dcmCopyString(rec, DCM_StudyID, d);
    dcmCopyString(rec, DCM_AccessionNumber, d);

    return rec;
}

DcmDirectoryRecord*
buildSeriesRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Series, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating series record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_Modality, d);
    dcmCopyString(rec, DCM_SeriesInstanceUID, d);
    dcmCopyString(rec, DCM_SeriesNumber, d);

    return rec;
}

DcmDirectoryRecord*
buildImageRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Image, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating image record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);

    /* addition type 1C keys specified by STD-GEN-CD profile */
    dcmCopyOptString(rec, DCM_ImageType, d);
    dcmCopyOptSequence(rec, DCM_ReferencedImageSequence, d);

    return rec;
}

DcmDirectoryRecord*
buildOverlayRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Overlay, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating overlay record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_OverlayNumber, d);

    return rec;
}

DcmDirectoryRecord*
buildModalityLutRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_ModalityLut, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating modality lut record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_LookupTableNumber, d);

    return rec;
}

DcmDirectoryRecord*
buildVoiLutRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_VoiLut, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating voi lut record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_LookupTableNumber, d);

    return rec;
}

DcmDirectoryRecord*
buildCurveRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Curve, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating curve record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_CurveNumber, d);

    return rec;
}

DcmDirectoryRecord*
buildStructReportRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_StructReport, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating struct report record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyString(rec, DCM_CompletionFlag, d);
    dcmCopyString(rec, DCM_VerificationFlag, d);
    dcmCopyString(rec, DCM_ContentDate, d);
    dcmCopyString(rec, DCM_ContentTime, d);
    OFString verificationFlag(dcmFindString(d, DCM_VerificationFlag));
    if (verificationFlag == "VERIFIED")
    {
      // VerificationDateTime is required if verificationFlag is VERIFIED
      // retrieve most recent (= last) entry from VerifyingObserverSequence
      DcmStack stack;
      if (d->search(DCM_VerifyingObserverSequence, stack, ESM_fromHere, OFFalse) == EC_Normal)
      {
        DcmSequenceOfItems *dseq = (DcmSequenceOfItems *)stack.top();
        if ((dseq != NULL) && (dseq->card() > 0))
        {
          DcmItem *ditem = dseq->getItem(dseq->card() - 1);
          if (ditem != NULL)
            dcmCopyString(rec, DCM_VerificationDateTime, ditem);
        }
      }
    }
    dcmCopySequence(rec, DCM_ConceptNameCodeSequence, d);
    return rec;
}

DcmDirectoryRecord*
buildPresentationRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Presentation, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating presentation record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyString(rec, DCM_PresentationLabel, d);
    dcmCopyString(rec, DCM_PresentationDescription, d);
    dcmCopyString(rec, DCM_PresentationCreationDate, d);
    dcmCopyString(rec, DCM_PresentationCreationTime, d);
    dcmCopyString(rec, DCM_PresentationCreatorsName, d);
    dcmCopySequence(rec, DCM_ReferencedSeriesSequence, d);
    return rec;
}

DcmDirectoryRecord*
buildWaveformRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_Waveform, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating waveform record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyString(rec, DCM_ContentDate, d);
    dcmCopyString(rec, DCM_ContentTime, d);
    return rec;
}

DcmDirectoryRecord*
buildRTDoseRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_RTDose, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating rt dose record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyString(rec, DCM_DoseSummationType, d);
    dcmCopyOptString(rec, DCM_DoseComment, d);
    dcmCopyOptSequence(rec, DCM_IconImageSequence, d);
    return rec;
}

DcmDirectoryRecord*
buildRTStructureSetRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_RTStructureSet, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating rt structure set record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyString(rec, DCM_StructureSetLabel, d);
    dcmCopyString(rec, DCM_StructureSetDate, d);
    dcmCopyString(rec, DCM_StructureSetTime, d);
    return rec;
}

DcmDirectoryRecord*
buildRTPlanRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_RTPlan, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating rt plan record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyString(rec, DCM_RTPlanLabel, d);
    dcmCopyString(rec, DCM_RTPlanDate, d);
    dcmCopyString(rec, DCM_RTPlanTime, d);
    return rec;
}

DcmDirectoryRecord*
buildRTTreatmentRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_RTTreatRecord, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating rt treat record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyString(rec, DCM_TreatmentDate, d);
    dcmCopyString(rec, DCM_TreatmentTime, d);
    return rec;
}

DcmDirectoryRecord*
buildStoredPrintRecord(
    const OFString& referencedFileName,
    DcmItem* d,
    const OFString& sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
        ERT_StoredPrint, referencedFileName.c_str(), sourceFileName.c_str());
    if (rec == NULL) {
        CERR << "error: out of memory (creating stored print record)" << endl;
        return NULL;
    }
    if (rec->error() != EC_Normal) {
        CERR << "error: cannot create "
             << recordTypeToName(rec->getRecordType()) << " directory record: "
             << dcmErrorConditionToString(rec->error()) << endl;
        delete rec;
        return NULL;
    }

    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_InstanceNumber, d);
    dcmCopyOptSequence(rec, DCM_IconImageSequence, d);
    return rec;
}

static OFString
locateDicomFile(const OFString& fname)
{
    OFString fn = fname;
    dicomToHostFilename(fn);
    if (fileExists(fn)) {
        return fn;
    }
    /* trailing period */
    fn = fname + '.';
    dicomToHostFilename(fn);
    if (fileExists(fn)) {
        return fn;
    }
    /* lowercase */
    fn = fname;
    dicomToHostFilename(fn, OFTrue);
    if (fileExists(fn)) {
        return fn;
    }
    /* lowercase with trailing period */
    fn = fname + '.';
    dicomToHostFilename(fn, OFTrue);
    if (fileExists(fn)) {
        return fn;
    }

    return "";
}

static OFBool
recordMatchesDataset(DcmDirectoryRecord *rec, DcmItem* dataset)
{
    OFBool match = OFFalse;

    switch (rec->getRecordType()) {
    case ERT_Patient:
        if (dcmTagExistsWithValue(dataset, DCM_PatientID)) {
            /* PatientID is the primary key */
            match = cmp(dcmFindString(rec, DCM_PatientID),
                        dcmFindString(dataset, DCM_PatientID));
        } else {
            /* if there is no value for PatientID in the dataset
            ** try using the PatientsName
            */
            match = cmp(dcmFindString(rec, DCM_PatientsName),
                        dcmFindString(dataset, DCM_PatientsName));
        }
        break;
    case ERT_Study:
        if (dcmTagExists(rec, DCM_StudyInstanceUID)) {
            match = cmp(dcmFindString(rec, DCM_StudyInstanceUID),
                        dcmFindString(dataset, DCM_StudyInstanceUID));
        } else {
            /* the Study Instance UID can be in the referenced file instead */
            OFString reffname = dcmFindString(rec, DCM_ReferencedFileID);
            if (!reffname.empty()) {
                OFString fname = locateDicomFile(reffname);
                if (!fname.empty()) {
                    match = cmp(dcmFindStringInFile(fname,
                                                    DCM_StudyInstanceUID),
                                dcmFindString(dataset, DCM_StudyInstanceUID));
                } else {
                    CERR << "error: cannot locate referenced file: "
                         << reffname << endl;
                }
            }
        }
        break;
    case ERT_Series:
        match = cmp(dcmFindString(rec, DCM_SeriesInstanceUID),
                    dcmFindString(dataset, DCM_SeriesInstanceUID));
        break;
    case ERT_Image:
    case ERT_Overlay:
    case ERT_Curve:
    case ERT_ModalityLut:
    case ERT_VoiLut:
    case ERT_StructReport:
    case ERT_Presentation:
    case ERT_Waveform:
    case ERT_RTDose:
    case ERT_RTStructureSet:
    case ERT_RTPlan:
    case ERT_RTTreatRecord:
    case ERT_StoredPrint:
        /*
        ** The attribute ReferencedSOPInstanceUID is automatically
        ** put into a Directory Record when a filename is present.
        */
        match = cmp(dcmFindString(rec, DCM_ReferencedSOPInstanceUIDInFile),
                    dcmFindString(dataset, DCM_SOPInstanceUID));
        break;
    default:
        CERR << "error: record type not yet implemented" << endl;
        return OFFalse;
        /* break; */  // never reached after return statement
    }

    return match;
}

static DcmDirectoryRecord*
findExistingRecord(DcmDirectoryRecord *root, E_DirRecType dirtype,
                   DcmItem* dataset)
{
    if (root == NULL) {
        return (DcmDirectoryRecord*)NULL;
    }

    DcmDirectoryRecord* r = NULL;
    unsigned long n = root->cardSub();
    OFBool found = OFFalse;
    for (unsigned long i=0; i<n && !found; i++) {
        r = root->getSub(i);
        if (dirtype == r->getRecordType()) {
            found = recordMatchesDataset(r, dataset);
        }
    }
    return (found)?(r):((DcmDirectoryRecord*)NULL);
}

static DcmDirectoryRecord*
buildRecord(E_DirRecType dirtype, const OFString& referencedFileName,
            DcmItem* dataset, const OFString& sourceFileName)
{
    DcmDirectoryRecord *rec = NULL;

    switch (dirtype) {
    case ERT_Patient:
        rec = buildPatientRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_Study:
        rec = buildStudyRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_Series:
        rec = buildSeriesRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_Image:
        rec = buildImageRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_Overlay:
        rec = buildOverlayRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_Curve:
        rec = buildCurveRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_ModalityLut:
        rec = buildModalityLutRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_VoiLut:
        rec = buildVoiLutRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_StructReport:
        rec = buildStructReportRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_Presentation:
        rec = buildPresentationRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_Waveform:
        rec = buildWaveformRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_RTDose:
        rec = buildRTDoseRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_RTStructureSet:
        rec = buildRTStructureSetRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_RTPlan:
        rec = buildRTPlanRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_RTTreatRecord:
        rec = buildRTTreatmentRecord(referencedFileName, dataset, sourceFileName);
        break;
    case ERT_StoredPrint:
        rec = buildStoredPrintRecord(referencedFileName, dataset, sourceFileName);
        break;
    default:
        CERR << "error: record type not yet implemented" << endl;
        return OFFalse;
        /* break; */  // never reached after return statement
    }
    return rec;
}

static void
printAttribute(ostream& out, DcmTagKey& key, const OFString& s)
{
    DcmTag tag(key);
    out << tag.getTagName() << " " << key << "=\"" << s << "\"";
}

static void
printRecordUniqueKey(ostream& out, DcmDirectoryRecord *rec)
{
    switch (rec->getRecordType()) {
    case ERT_Patient:
        out << "PatientID " << DCM_PatientID << "=\""
            << dcmFindString(rec, DCM_PatientID) << "\"";
        break;
    case ERT_Study:
        out << "StudyInstanceUID " << DCM_StudyInstanceUID << "=\""
            << dcmFindString(rec, DCM_StudyInstanceUID) << "\"";
        break;
    case ERT_Series:
        out << "SeriesInstanceUID " << DCM_SeriesInstanceUID << "=\""
            << dcmFindString(rec, DCM_SeriesInstanceUID) << "\"";
        break;
    case ERT_Image:
    case ERT_Overlay:
    case ERT_Curve:
    case ERT_ModalityLut:
    case ERT_VoiLut:
    case ERT_StructReport:
    case ERT_Presentation:
    case ERT_Waveform:
    case ERT_RTDose:
    case ERT_RTStructureSet:
    case ERT_RTPlan:
    case ERT_RTTreatRecord:
    case ERT_StoredPrint:
        out << "ReferencedSOPInstanceUIDInFile "
            << DCM_ReferencedSOPInstanceUIDInFile << "=\""
            << dcmFindString(rec, DCM_ReferencedSOPInstanceUIDInFile) << "\"";
        break;
    default:
        CERR << "error: record type not yet implemented" << endl;
        break;
    }
}

static OFBool
compareStringAttributes(DcmTagKey& tag, DcmDirectoryRecord *rec,
                        DcmItem* dataset, const OFString& sourceFileName)
{
    OFString s1 = dcmFindString(rec, tag);
    OFString s2 = dcmFindString(dataset, tag);
    OFBool equals = cmp(s1, s2);

    if (!equals) {
        CERR << "Warning: file inconsistant with existing DICOMDIR record"
             << endl;
        CERR << "  " << recordTypeToName(rec->getRecordType())
             << " Record [Key: ";
        printRecordUniqueKey(CERR, rec);
        CERR << "]" << endl;
        CERR << "    Existing Record (origin: " << rec->getRecordsOriginFile()
             << ") defines: " << endl;
        CERR << "      ";
        printAttribute(CERR, tag, s1); CERR << endl;
        CERR << "    File (" << sourceFileName << ") defines:" << endl;
        CERR << "      ";
        printAttribute(CERR, tag, s2); CERR << endl;
    }

    return equals;
}


static OFBool
areTagsEqual(DcmObject* obj1, DcmObject* obj2)
{
    if (obj1 == NULL || obj2 == NULL) {
        return OFFalse;
    }

    return obj1->getTag().getXTag() == obj2->getTag().getXTag();
}

static OFString
constructTagName(DcmObject *obj)
{
    if (obj == NULL) {
        return "(NULL)";
    }
    DcmTag tag = obj->getTag();
    const char *tagName = tag.getTagName();

    // use tag name only if not equal to DcmTag_ERROR_TagName
    if (strcmp(tagName, DcmTag_ERROR_TagName)) return tagName;

    char buf[32];
    sprintf(buf, "(0x%04x,0x%04x)", tag.getGTag(), tag.getETag());
    return buf;
}

static OFString
intToOFString(int i)
{
    char buf[64];
    sprintf(buf, "%d", i);
    return buf;
}

static OFString
constructTagNameWithSQ(DcmObject *obj, DcmSequenceOfItems* fromSequence, int itemNumber)
{
    if (obj == NULL) {
        return "(NULL)";
    }
    OFString s;
    if (fromSequence != NULL) {
        s += constructTagName(fromSequence) + "[" + intToOFString(itemNumber) + "].";
    }
    s += constructTagName(obj);
    return s;
}

static OFBool
compareBinaryValues(DcmElement* elem1, DcmElement* elem2, OFString& reason)
{
    Uint8* value1 = NULL;
    Uint8* value2 = NULL;
    Uint16* u16 = NULL;
    Sint16* s16 = NULL;
    Uint32* u32 = NULL;
    Sint32* s32 = NULL;
    Float32* f32 = NULL;
    Float64* f64 = NULL;


    E_Condition ec1 = EC_Normal;
    E_Condition ec2 = EC_Normal;

    switch (elem1->getVR()) {
    case EVR_OB:
        ec1 = elem1->getUint8Array(value1);
        ec2 = elem2->getUint8Array(value2);
        break;
    case EVR_AT:
    case EVR_US:
    case EVR_OW:
        ec1 = elem1->getUint16Array(u16);
        value1 = (Uint8*)u16;
        ec2 = elem2->getUint16Array(u16);
        value2 = (Uint8*)u16;
        break;
    case EVR_SS:
        ec1 = elem1->getSint16Array(s16);
        value1 = (Uint8*)s16;
        ec2 = elem2->getSint16Array(s16);
        value2 = (Uint8*)s16;
        break;
    case EVR_UL:
        ec1 = elem1->getUint32Array(u32);
        value1 = (Uint8*)u32;
        ec2 = elem2->getUint32Array(u32);
        value2 = (Uint8*)u32;
        break;
    case EVR_SL:
        ec1 = elem1->getSint32Array(s32);
        value1 = (Uint8*)s32;
        ec2 = elem2->getSint32Array(s32);
        value2 = (Uint8*)s32;
        break;
    case EVR_FL:
        ec1 = elem1->getFloat32Array(f32);
        value1 = (Uint8*)f32;
        ec2 = elem2->getFloat32Array(f32);
        value2 = (Uint8*)f32;
        break;
    case EVR_FD:
        ec1 = elem1->getFloat64Array(f64);
        value1 = (Uint8*)f64;
        ec2 = elem2->getFloat64Array(f64);
        value2 = (Uint8*)f64;
        break;
    /* currently cannot handle any other VR types */
    default:
        {
          DcmVR vr1(elem1->getVR());
          reason = "INTERNAL ERROR: Unexpected VR encountered: ";
          reason += vr1.getVRName();
          reason += "(" + constructTagName(elem1) + ")";
        }
        return OFFalse;
    }

    if (ec1 != EC_Normal || ec2 != EC_Normal)
    {
        DcmTag tag(elem1->getTag()); // create non const copy
        CERR << "dcmFindSequence: error while getting value of " << tag.getTagName()
             << " " << tag.getXTag() << ": "
             << dcmErrorConditionToString((ec1 != EC_Normal)?(ec1):(ec2)) << endl;
        reason = "cannot access binary value";
        return OFFalse;
    }

    Uint32 len = elem1->getLength();
    if (elem2->getLength() != len) return OFFalse;

    for (Uint32 i=0; i<len; i++) {
        if (value1[i] != value2[i]) {
            return OFFalse;
        }
    }
    return OFTrue;
}


static OFBool
compareSQAttributes(DcmSequenceOfItems* sq1, DcmSequenceOfItems* sq2, OFString& reason);

static OFBool
compareAttributes(DcmElement* elem1, DcmElement* elem2, DcmSequenceOfItems* fromSequence, int itemNumber, OFString& reason)
{
    if (elem1 == NULL && elem2 == NULL) {
        return OFTrue; /* nothing to compare */
    }
    if (elem1 == NULL) {
        reason = "missing attribute: " +  constructTagNameWithSQ(elem2, fromSequence, itemNumber);
        return OFFalse;
    }
    if (elem2 == NULL) {
        reason = "missing attribute: " +  constructTagNameWithSQ(elem1, fromSequence, itemNumber);
        return OFFalse;
    }

    /* do we have the same attributes? */
    if (!areTagsEqual(elem1, elem2)) {
        reason = "INTERNAL ERROR: different attributes: " +
            constructTagNameWithSQ(elem1, fromSequence, itemNumber) + " != " +
            constructTagNameWithSQ(elem2, fromSequence, itemNumber);
        return OFFalse;
    }

    /* are the VRs the same? */
    DcmVR vr1(elem1->getVR());
    DcmVR vr2(elem2->getVR());
    if (vr1.getEVR() != vr2.getEVR()) {
        reason = "different VRs: ";
        reason += vr1.getVRName() + OFString("!=") + vr2.getVRName();
        reason += "(" + constructTagNameWithSQ(elem1, fromSequence, itemNumber) + ")";
        return OFFalse;
    }

    /* are the lengths the same? */
    if (elem1->getLength() != elem2->getLength()) {
        reason = "different value lengths: " +
            intToOFString((int)(elem1->getLength())) + "!=" + intToOFString((int)(elem2->getLength())) +
            " (" + constructTagNameWithSQ(elem1, fromSequence, itemNumber) + ")";
        return OFFalse;
    }

    /* are the contents the same? */
    if (elem1->getLength() == 0) {
        return OFTrue;  /* no value */
    }

    if (elem1->isaString()) {
        OFString value1;
        OFString value2;

        elem1->getOFStringArray(value1);
        elem2->getOFStringArray(value2);
        if (value1 != value2) {
            reason = "different values: \"" + value1 + "\"!=\"" + value2 + "\" (" +
                constructTagNameWithSQ(elem1, fromSequence, itemNumber) + ")";
            return OFFalse;
        }

    } else if (vr1.getEVR() == EVR_SQ) {
        /* compare embedded sequences recursively */
        if (!compareSQAttributes((DcmSequenceOfItems*)elem1, (DcmSequenceOfItems*)elem2, reason)) {
            return OFFalse;
        }
    } else {
        /* compare binary values */
        if (!compareBinaryValues(elem1, elem2, reason)) {
            return OFFalse;
        }
    }

    /* the 2 attributes must be identical if we get this far */
    return OFTrue;
}

static OFBool
compareItems(DcmItem* item1, DcmItem* item2, DcmSequenceOfItems* fromSequence, int itemNumber, OFString& reason)
{
    if (item1 == NULL && item2 == NULL) {
        return OFTrue; /* nothing to compare */
    }
    if (item1 == NULL || item2 == NULL) {
        reason = "missing item: " +  constructTagName(fromSequence) +
            "[" + intToOFString(itemNumber) + "]";
        return OFFalse;
    }


    int n1 = (int)(item1->card());
    int n2 = (int)(item2->card());

    if (n1 != n2) {
        reason = "different number attributes in items: " + constructTagName(fromSequence) +
            "[" + intToOFString(itemNumber) + "]";
        return OFFalse;
    }

    for (int i=0; i<n1; i++) {
        DcmElement* elem1 = item1->getElement(i);
        DcmElement* elem2 = item2->getElement(i);

        OFBool attributesAreEqual = compareAttributes(elem1, elem2, fromSequence, itemNumber, reason);
        if (!attributesAreEqual) {
            return OFFalse;
        }
    }

    /* the 2 items must be identical if we get this far */
    return OFTrue;
}


static OFBool
compareSQAttributes(DcmSequenceOfItems* sq1, DcmSequenceOfItems* sq2, OFString& reason)
{
    if (sq1 == NULL && sq2 == NULL) {
        return OFTrue; /* nothing to compare */
    }
    if (sq1 == NULL) {
        reason = "missing sequence: " +  constructTagName(sq2);
        return OFFalse;
    }
    if (sq2 == NULL) {
        reason = "missing sequence: " +  constructTagName(sq1);
        return OFFalse;
    }

    if (!areTagsEqual(sq1, sq2)) {
        reason = "INTERNAL ERROR: different sequences: " +
            constructTagName(sq1) + " != " + constructTagName(sq2);
        return OFFalse;
    }

    int n1 = (int)(sq1->card());
    int n2 = (int)(sq2->card());

    if (n1 != n2) {
        reason = "different number of items in sequence: " + constructTagName(sq1);
        return OFFalse;
    }

    for (int i=0; i<n1; i++) {
        DcmItem *item1 = sq1->getItem(i);
        DcmItem *item2 = sq2->getItem(i);

        OFBool itemsAreEqual = compareItems(item1, item2, sq1, i, reason);
        if (!itemsAreEqual) {
            return OFFalse;
        }
    }
    /* the 2 sequences must be identical if we get this far */
    return OFTrue;
}

static DcmSequenceOfItems*
dcmFindSequence(DcmItem* d, const DcmTagKey& key,
                OFBool searchIntoSub = OFFalse)
{
    DcmSequenceOfItems* sq = NULL;
    DcmElement *elem = NULL;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    ec = d->search(key, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();

    if (ec != EC_Normal && ec != EC_TagNotFound) {
        DcmTag tag(key);
        CERR << "dcmFindSequence: error while finding " << tag.getTagName()
             << " " << key << ": "
             << dcmErrorConditionToString(ec) << endl;
    }

    if (elem && elem->ident() == EVR_SQ) {
        sq = (DcmSequenceOfItems*)elem;
    }
    return sq;
}

static OFBool
compareSequenceAttributes(DcmTagKey& tag, DcmDirectoryRecord *rec,
                          DcmItem* dataset, const OFString& sourceFileName)
{
    DcmSequenceOfItems* sq1 = dcmFindSequence(rec, tag);
    DcmSequenceOfItems* sq2 = dcmFindSequence(dataset, tag);

    OFString reason;
    OFBool equals = compareSQAttributes(sq1, sq2, reason);

    if (!equals) {
        CERR << "Warning: file inconsistant with existing DICOMDIR record"
             << endl;
        CERR << "  " << recordTypeToName(rec->getRecordType())
             << " Record [Key: ";
        printRecordUniqueKey(CERR, rec);
        CERR << "]" << endl;
        CERR << "    Reason: " << reason << endl;;
        CERR << "    Existing Record (origin: " << rec->getRecordsOriginFile()
             << ") defines: " << endl;
        sq1->print(CERR, OFTrue, 2);
        CERR << "    File (" << sourceFileName << ") defines:" << endl;
        sq2->print(CERR, OFTrue, 2);
    }

    return equals;
}

static void
warnAboutInconsistantAttributes(DcmDirectoryRecord *rec,
                                DcmItem* dataset,
                                const OFString& sourceFileName)
{
    /*
    ** See if all the attributes in rec match the values in dataset
    */
    DcmElement *re = NULL;
    unsigned long n = rec->card();
    for (unsigned long i=0; i<n; i++) {
        re = rec->getElement(i);
        if (re->getLength() > 0) {
            /* record attribute has a value */
            DcmTagKey tag = re->getTag().getXTag();
            if (dcmTagExistsWithValue(dataset, tag)) {
                /*
                ** We currently only handle strings and sequences.
                ** This is not a huge problem since all the DICOMDIR
                ** attributes we generate are strings or sequences.
                */
                if (re->isaString()) {
                    compareStringAttributes(tag, rec, dataset, sourceFileName);
                } else if (re->getTag().getEVR() == EVR_SQ) {
                    compareSequenceAttributes(tag, rec, dataset, sourceFileName);
                } else {
                    CERR << "INTERNAL ERROR: cannot compare: " << tag << endl;
                }
            }
        }
    }
}

static int
getISNumber(DcmItem *i, const DcmTagKey& key)
{
    int num = 0;
    OFString s = dcmFindString(i, key);
    if (!s.empty()) {
        sscanf(s.c_str(), "%d", &num);
    }
    return num;
}

static E_Condition
insertWithISCriterion(DcmDirectoryRecord *parent, DcmDirectoryRecord *child,
                      const DcmTagKey& criterionKey)
{
    /*
    ** Insert the child directory record into the parent's list based
    ** on the numeric value of a VR=IS criterionKey attribute.
    ** Strange things will happen if criterionKey does not represent VR=IS.
    */

    E_Condition cond = EC_Normal;
    int number = getISNumber(child, criterionKey);
    int pos = 0;
    int count = (int)(parent->cardSub());
    OFBool found = OFFalse;
    if (count > 0) {
        for (int i=0; i<count && !found; i++) {
            DcmDirectoryRecord* rec = parent->getSub(i);
            int curNumber = getISNumber(rec, criterionKey);
            if (curNumber > number) {
                found = OFTrue;
            }
            pos = i;
        }
    }
    if (pos < (count-1)) {
        cond = parent->insertSub(child, pos, OFTrue);
    } else {
        cond = parent->insertSub(child); /* append */
    }
    return cond;
}

static E_Condition
insertSortedUnder(DcmDirectoryRecord *parent, DcmDirectoryRecord *child)
{
    E_Condition cond = EC_Normal;
    switch (child->getRecordType()) {
    case ERT_Image:
        /* try to insert based on Image/InstanceNumber */
        cond = insertWithISCriterion(parent, child, DCM_InstanceNumber);
        break;
    case ERT_Overlay:
        /* try to insert based on OverlayNumber */
        cond = insertWithISCriterion(parent, child, DCM_OverlayNumber);
        break;
    case ERT_Curve:
        /* try to insert based on CurveNumber */
        cond = insertWithISCriterion(parent, child, DCM_CurveNumber);
        break;
    case ERT_ModalityLut:
    case ERT_VoiLut:
        /* try to insert based on LUTNumber */
        cond = insertWithISCriterion(parent, child, DCM_LookupTableNumber);
        break;
    case ERT_StructReport:
    case ERT_Presentation:
    case ERT_Waveform:
    case ERT_RTDose:
    case ERT_RTStructureSet:
    case ERT_RTPlan:
    case ERT_RTTreatRecord:
    case ERT_StoredPrint:
        /* try to insert based on InstanceNumber */
        cond = insertWithISCriterion(parent, child, DCM_InstanceNumber);
        break;
    case ERT_Series:
        /* try to insert based on SeriesNumber */
        cond = insertWithISCriterion(parent, child, DCM_SeriesNumber);
        break;
    default:
        /* append */
        cond = parent->insertSub(child);
        break;
    }
    return cond;
}

static DcmDirectoryRecord*
includeRecord(DcmDirectoryRecord *parentRec, E_DirRecType dirtype,
              DcmItem* dataset, const OFString& referencedFileName,
              const OFString& sourceFileName)
{
    DcmDirectoryRecord *rec =
        findExistingRecord(parentRec, dirtype, dataset);
    if (rec == NULL) {
        rec = buildRecord(dirtype, referencedFileName,
                          dataset, sourceFileName);
        if (rec != NULL) {
            /* insert underneath correct parent record */
            E_Condition cond = insertSortedUnder(parentRec,rec);
            if (cond != EC_Normal) {
                CERR << "error: " << dcmErrorConditionToString(cond)
                     << ": cannot insert " << recordTypeToName(dirtype)
                     << " record" << endl;
                return NULL;
            }
        }
    }
    return rec;
}

static OFBool
addToDir(DcmDirectoryRecord* rootRec, const OFString& ifname)
{
    OFString fname = ifname;

    if (verbosemode) {
        COUT << "adding: " << fname << endl;
    }
    DcmFileStream myin(fname.c_str(), DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        CERR << "cannot open file: " << fname << endl;
        return OFFalse;
    }
    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_noChange );
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
        CERR << "error: " << dcmErrorConditionToString(ff.error())
             << ": reading file: " << fname << endl;
        return OFFalse;
    }

    /*
    ** Massage filename into dicom format
    ** (dos conventions for path separators)
    */
    hostToDicomFilename(fname);

    E_Condition cond = EC_Normal;
    DcmMetaInfo* metainfo = ff.getMetaInfo();
    DcmDataset* dataset = ff.getDataset();
    /* what kind of object (SOP Class) is stored in the file */
    OFString sopClass = dcmFindString(metainfo, DCM_MediaStorageSOPClassUID);

    /*
    ** Build the necessary dicomdir records
    */

    /*
    ** Add a patient record underneath the root
    */

    OFString emptyString;
    DcmDirectoryRecord *patientRec =
        includeRecord(rootRec, ERT_Patient, dataset, emptyString, ifname);
    if (patientRec == NULL) {
        return OFFalse;
    }
    if (patientRec->getRecordsOriginFile() == NULL) {
        // not yet noted
        patientRec->setRecordsOriginFile(ifname.c_str());
    }
    warnAboutInconsistantAttributes(patientRec, dataset, ifname);

    /* if PatientMgmgt file then attach it to Patient Record and stop */
    if (cmp(sopClass, UID_DetachedPatientManagementMetaSOPClass)) {
        cond = patientRec->assignToSOPFile(fname.c_str(), ifname.c_str());
        if (cond != EC_Normal) {
            CERR << "error: " << dcmErrorConditionToString(cond)
                 << ": cannot assign patient record to file: "
                 << fname << endl;
            return OFFalse;
        }
        /* stop adding STUDY/SERIES/.. records */
        return OFTrue;
    }

    /*
    ** Add a study record underneath the actual patient record
    */

    DcmDirectoryRecord *studyRec =
        includeRecord(patientRec, ERT_Study, dataset, emptyString, ifname);
    if (studyRec == NULL) {
        return OFFalse;
    }
    if (studyRec->getRecordsOriginFile() == NULL) {
        // not yet noted
        studyRec->setRecordsOriginFile(ifname.c_str());
    }
    warnAboutInconsistantAttributes(studyRec, dataset, ifname);

    /*
    ** Add a series record underneath the actual study
    */
    DcmDirectoryRecord *seriesRec =
        includeRecord(studyRec, ERT_Series, dataset, emptyString, ifname);
    if (seriesRec == NULL) {
        return OFFalse;
    }
    if (seriesRec->getRecordsOriginFile() == NULL) {
        // not yet noted
        seriesRec->setRecordsOriginFile(ifname.c_str());
    }
    warnAboutInconsistantAttributes(seriesRec, dataset, ifname);

    /*
    ** Add one of the image-like records underneath the actual series record
    */
    DcmDirectoryRecord *rec = NULL;

    if (cmp(sopClass, UID_StandaloneOverlayStorage)) {
        /* Add an overlay record */
        rec = includeRecord(seriesRec, ERT_Overlay, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_StandaloneModalityLUTStorage)) {
        /* Add a modality lut record */
        rec = includeRecord(seriesRec, ERT_ModalityLut, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_StandaloneVOILUTStorage)) {
        /* Add a voi lut record */
        rec = includeRecord(seriesRec, ERT_VoiLut, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_StandaloneCurveStorage) ||
               cmp(sopClass, UID_PETCurveStorage)) {
        /* Add a curve record */
        rec = includeRecord(seriesRec, ERT_Curve, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_BasicTextSR) ||
               cmp(sopClass, UID_EnhancedSR) ||
               cmp(sopClass, UID_ComprehensiveSR)) {
        /* Add a structured report */
        rec = includeRecord(seriesRec, ERT_StructReport, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_GrayscaleSoftcopyPresentationStateStorage)) {
        /* Add a presentation state */
        rec = includeRecord(seriesRec, ERT_Presentation, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_TwelveLeadECGWaveformStorage) ||
               cmp(sopClass, UID_GeneralECGWaveformStorage) ||
               cmp(sopClass, UID_AmbulatoryECGWaveformStorage) ||
               cmp(sopClass, UID_HemodynamicWaveformStorage) ||
               cmp(sopClass, UID_CardiacElectrophysiologyWaveformStorage) ||
               cmp(sopClass, UID_BasicVoiceAudioWaveformStorage)) {
        /* Add a waveform */
        rec = includeRecord(seriesRec, ERT_Waveform, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_RTDoseStorage)) {
        /* Add a RT dose */
        rec = includeRecord(seriesRec, ERT_RTDose, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_RTStructureSetStorage)) {
        /* Add a RT structure set */
        rec = includeRecord(seriesRec, ERT_RTStructureSet, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_RTPlanStorage)) {
        /* Add a RT plan */
        rec = includeRecord(seriesRec, ERT_RTPlan, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_RTBeamsTreatmentRecordStorage) ||
               cmp(sopClass, UID_RTBrachyTreatmentRecordStorage) ||
               cmp(sopClass, UID_RTTreatmentSummaryRecordStorage)) {
        /* Add a RT treat record */                 
        rec = includeRecord(seriesRec, ERT_RTTreatRecord, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else if (cmp(sopClass, UID_StoredPrintStorage)) {
        /* Add a stored print */ 
        rec = includeRecord(seriesRec, ERT_StoredPrint, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    } else {
        /* it can only be an image */
        /* Add an image record */
        rec = includeRecord(seriesRec, ERT_Image, dataset, fname, ifname);
        if (rec == NULL) {
            return OFFalse;
        }
    }
    if (rec->getRecordsOriginFile() == NULL) {
        // not yet noted
        rec->setRecordsOriginFile(ifname.c_str());
    }
    warnAboutInconsistantAttributes(rec, dataset, ifname);

    return OFTrue;
}

static OFBool
areCSCharsValid(const OFString& fname)
{
    int len = fname.size();
    for (int i=0; i<len; i++) {
        char c = fname[i];
        if ((c != ' ') && (c != '_') && !isdigit(c) &&
            !(isalpha(c) && isupper(c))) {
            CERR << "error: invalid character encountered: "
                 << char(c) << endl;
            return OFFalse;
        }
    }
    return OFTrue;
}

static OFString
locateInvalidFileNameChars(const OFString& fname,
                           char separator = PATH_SEPARATOR)
{
    int len = fname.size();
    if (mapFilenames && fname[len-1] == '.') {
        /* disregard trailing point */
        len--;
    }
    OFString invalidChars;
    for (int i=0; i<len; i++) {
        char c = fname[i];
        if ((c == '_') || isdigit(c) || (c == separator) ||
            (isalpha(c) && (isupper(c) || (islower(c) && mapFilenames)))) {
            /* all ok */
        } else if (c == '\\') {
            /* this is only ok if the underlying OS uses \ */
            if (separator != '\\') {
                if (invalidChars.find(c) == OFString_npos) {
                    invalidChars += c;
                }
            }
        } else if (isalpha(c) && islower(c)) {
            if (invalidChars.find(c) == OFString_npos) {
                invalidChars += c;
            }
        } else {
            if (invalidChars.find(c) == OFString_npos) {
                invalidChars += c;
            }
        }
    }
    return invalidChars;
}

static OFBool
isaValidFileName(const OFString& fname,
                 char sep = PATH_SEPARATOR)
{
    OFBool ok = OFTrue;
    if (fname.length() == 0) {
        CERR << "error: <empty string> not allowed as filename" << endl;
        ok = OFFalse;
    }
    /*
    ** Is the file name path ok?
    ** The filename is assumed to be in local format for this check.
    */
    OFString invalidChars = locateInvalidFileNameChars(fname, sep);
    if (!invalidChars.empty()) {
        CERR << "error: invalid characters (\"" << invalidChars
             << "\") in filename: " << fname << endl;
        ok = OFFalse;
    }
    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (componentCount(fname, sep) > MAX_FNAME_COMPONENTS) {
        CERR << "error: too many path components (max "
             << MAX_FNAME_COMPONENTS << ") in filename: "
             << fname << endl;
        ok = OFFalse;
    }
    /*
    ** Ensure that each component is not too large
    */
    if (isComponentTooLarge(fname, MAX_FNAME_COMPONENT_SIZE, sep)) {
        CERR << "error: component too large (max "
             << MAX_FNAME_COMPONENT_SIZE << " characters) in filename: "
             << fname << endl;
        ok = OFFalse;
    }
    return ok;
}

static OFBool
isaValidFileSetID(const OFString& aFsid)
{
    OFBool ok = OFTrue;
    if (aFsid.empty()) {
        return OFFalse;
    }
    /*
    ** Are the characters ok?
    */
    if (!areCSCharsValid(aFsid)) {
        CERR << "       invalid characters in FileSetID: "
             << aFsid << endl;
        ok = OFFalse;
    }
    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (componentCount(aFsid) != 1) {
        CERR << "error: too many components in FileSetID: "
             << aFsid << endl;
        ok = OFFalse;
    }
    /*
    ** Ensure that each component is not too large
    */
    DcmVR cs(EVR_CS);
    if (isComponentTooLarge(aFsid, (int)(cs.getMaxValueLength()))) {
        CERR << "error: too large: " << aFsid << endl;
        ok = OFFalse;
    }
    return ok;
}


static OFBool
checkFileCanBeUsed(const OFString& fname)
{
    if (verbosemode) {
        COUT << "checking: " << fname << endl;
    }

    if (!isaValidFileName(fname)) {
        return OFFalse;
    }

    /*
    ** Does the file exist??
    */
    DcmFileStream myin(fname.c_str(), DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        CERR << "error: cannot open file: " << fname << endl;
        /* cannot continue checking */
        return OFFalse;
    }

    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_noChange );
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
        CERR << "error: "
             << dcmErrorConditionToString(ff.error())
             << ": reading file: " << fname << endl;
        /* cannot continue checking */
        return OFFalse;
    }

    return checkImage(fname, &ff);
}

static void
inventMissingImageLevelAttributes(DcmDirectoryRecord *parent)
{
    int imageNumber = 1;
    int overlayNumber = 1;
    int lutNumber = 1;
    int curveNumber = 1;
    int rtDoseInstanceNumber = 1;
    int rtStructureSetInstanceNumber = 1;
    int rtPlanInstanceNumber = 1;
    int storedPrintInstanceNumber = 1;

    int count = (int)(parent->cardSub());
    for (int i=0; i<count; i++) {
        DcmDirectoryRecord* rec = parent->getSub(i);

        switch (rec->getRecordType()) {
        case ERT_Image:
            if (!dcmTagExistsWithValue(rec, DCM_InstanceNumber)) {
                OFString defNum = defaultNumber(imageNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing ImageNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_InstanceNumber, defNum);
            }
            break;
        case ERT_Overlay:
            if (!dcmTagExistsWithValue(rec, DCM_OverlayNumber)) {
                OFString defNum = defaultNumber(overlayNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing OverlayNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_OverlayNumber, defNum);
            }
            break;
        case ERT_ModalityLut:
        case ERT_VoiLut:
            if (!dcmTagExistsWithValue(rec, DCM_LookupTableNumber)) {
                OFString defNum = defaultNumber(lutNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing LutNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_LookupTableNumber, defNum);
            }
            break;
        case ERT_Curve:
            if (!dcmTagExistsWithValue(rec, DCM_CurveNumber)) {
                OFString defNum = defaultNumber(curveNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing CurveNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_CurveNumber, defNum);
            }
            break;
        case ERT_RTDose:
            if (!dcmTagExistsWithValue(rec, DCM_InstanceNumber)) {
                OFString defNum = defaultNumber(rtDoseInstanceNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing InstanceNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_InstanceNumber, defNum);
            }
            break;
        case ERT_RTStructureSet:
            if (!dcmTagExistsWithValue(rec, DCM_InstanceNumber)) {
                OFString defNum = defaultNumber(rtStructureSetInstanceNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing InstanceNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_InstanceNumber, defNum);
            }
            break;
        case ERT_RTPlan:
            if (!dcmTagExistsWithValue(rec, DCM_InstanceNumber)) {
                OFString defNum = defaultNumber(rtPlanInstanceNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing InstanceNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_InstanceNumber, defNum);
            }
            break;
        case ERT_StoredPrint:
            if (!dcmTagExistsWithValue(rec, DCM_InstanceNumber)) {
                OFString defNum = defaultNumber(storedPrintInstanceNumber++);
                CERR << "Warning: " <<  recordTypeToName(rec->getRecordType())
                     << "Record (origin: " << rec->getRecordsOriginFile()
                     << ") inventing InstanceNumber: "
                     << defNum << endl;
                dcmInsertString(rec, DCM_InstanceNumber, defNum);
            }
            break;
        case ERT_StructReport:
        case ERT_Presentation:
        case ERT_Waveform:
        case ERT_RTTreatRecord:
            /* nothing to do */
            break;
        default:
            CERR << "error: (INTERNAL): inventMissingImageLevelAttributes: "
                 << "encountered unexpected record: "
                 << recordTypeToName(rec->getRecordType())
                 << endl;
            break;
        }
    }
}

static void
inventMissingSeriesLevelAttributes(DcmDirectoryRecord *parent)
{
    int seriesNumber = 0;
    int count = (int)(parent->cardSub());
    for (int i=0; i<count; i++) {
        DcmDirectoryRecord* rec = parent->getSub(i);
        if (!dcmTagExistsWithValue(rec, DCM_SeriesNumber)) {
            OFString defNum = defaultNumber(seriesNumber++);
            CERR << "Warning: Series Record (origin: "
                 << rec->getRecordsOriginFile() << ") inventing SeriesNumber: "
                 << defNum << endl;
            dcmInsertString(rec, DCM_SeriesNumber, defNum);
        }
        inventMissingImageLevelAttributes(rec);
    }

}

static void
inventMissingStudyLevelAttributes(DcmDirectoryRecord *parent)
{
    static int studyNumber = 0; /* make invented StudyID global */

    int count = (int)(parent->cardSub());
    for (int i=0; i<count; i++) {
        DcmDirectoryRecord* rec = parent->getSub(i);
        if (!dcmTagExistsWithValue(rec, DCM_StudyID)) {
            OFString defId = defaultID("DCMTKSTUDY", studyNumber++);
            CERR << "Warning: Study Record (origin: "
                 << rec->getRecordsOriginFile() << ") inventing StudyID: "
                 << defId << endl;
            dcmInsertString(rec, DCM_StudyID, defId);
        }
        inventMissingSeriesLevelAttributes(rec);
    }
}

static void
inventMissingAttributes(DcmDirectoryRecord *root)
{
    static int patientNumber = 0; /* make invented PatientID global */

    int count = (int)(root->cardSub());
    for (int i=0; i<count; i++) {
        DcmDirectoryRecord* rec = root->getSub(i);
        if (!dcmTagExistsWithValue(rec, DCM_PatientID)) {
            OFString defId = defaultID("DCMTKPAT", patientNumber++);
            CERR << "Warning: Patient Record (origin: "
                 << rec->getRecordsOriginFile() << ") inventing PatientID: "
                 << defId << endl;
            dcmInsertString(rec, DCM_PatientID, defId);
        }
        inventMissingStudyLevelAttributes(rec);
    }
}

static OFBool
copyFile(const OFString& fromfname, const OFString& tofname)
{
    FILE* fromf = NULL;
    FILE* tof = NULL;
    if ((fromf = fopen(fromfname.c_str(), "rb")) == NULL) {
        CERR << "error: copying files: cannot open: " << fromfname << endl;
        return OFFalse;
    }
    if ((tof = fopen(tofname.c_str(), "wb")) == NULL) {
        CERR << "error: copying files: cannot create: " << tofname << endl;
        fclose(fromf);
        return OFFalse;
    }
    OFBool ok = OFTrue;
    int c = 0;
    while ((c = getc(fromf)) != EOF) {
        if (putc(c, tof) == EOF) {
            CERR << "error: copying files: " << fromfname << " to "
                 << tofname << endl;
            ok = OFFalse;
            break;
        }
    }
    fclose(fromf);
    fclose(tof);
    return ok;
}

static OFBool
createDicomdirFromFiles(OFList<OFString>& fileNames)
{
    OFBool ok = OFTrue;

    /*
    ** Check the FileSetID and the FileSetDescriptorFileID
    */
    if (!isaValidFileSetID(fsid)) {
        CERR << "error: invalid FileSetID: "
             << fsdfid << endl;
        ok = OFFalse;
    }

    if (!fsdfid.empty() && !isaValidFileName(fsdfid)) {
        CERR << "       bad FileSetDescriptorFileID: "
             << fsdfid << endl;
        ok = OFFalse;
    }

    if (!fsdfid.empty() && !fileExists(fsdfid)) {
        CERR << "error: cannot find FileSetDescriptorFileID: "
             << fsdfid << endl;
        ok = OFFalse;
    }

    /*
    ** Check the files before adding
    */

    OFList<OFString> goodFileNames;
    OFList<OFString> badFileNames;
    OFListIterator(OFString) iter = fileNames.begin();
    while (iter != fileNames.end()) {
        if (checkFileCanBeUsed(*iter)) {
            goodFileNames.push_back(*iter);
        } else {
            badFileNames.push_back(*iter);;
        }
        ++iter;
    }
    if (goodFileNames.empty()) {
        CERR << "FATAL ERROR: no good files: DICOMDIR not created" << endl;
        return OFFalse;
    } else if (!badFileNames.empty()) {
        CERR << "WARNING: " << badFileNames.size()
             << " file(s) cannot be added to DICOMDIR: " << endl;
        iter = badFileNames.begin();
        while (iter != badFileNames.end()) {
            CERR << "         " << *iter << endl;
            ++iter;
        }
    }

    OFBool backupCreated = OFFalse;
    OFString backupName;

    if (writeDicomdir && fileExists(ofname)) {
        /* rename existing DICOMDIR */
        backupName = ofname + ".BAK";
        unlink(backupName.c_str());
        if (verbosemode) {
            COUT << "creating backup dicomdir: " << backupName << endl;
        }
        if (copyFile(ofname, backupName)) {
            backupCreated = OFTrue;
        } else {
            CERR << "error: cannot create backup of: " << ofname << endl;
        }
    }

    if (!appendMode && writeDicomdir && fileExists(ofname)) {
        /*
        ** delete existing DICOMDIR because otherwise DcmDicomDir
        ** will parse it and try to append to existing records
        */
        unlink(ofname.c_str());
    }

    /*
    ** Create the DICOMDIR object
    */

    DcmDicomDir *dicomdir = new DcmDicomDir( ofname.c_str(), fsid.c_str() );
    DcmDirectoryRecord& rootRecord = dicomdir->getRootRecord();

    /*
    ** add optional parts to root record
    */
    if (!fsdfid.empty()) {
        dcmInsertString(dicomdir->getDirFileFormat().getDataset(),
                        DCM_FileSetDescriptorFileID, fsdfid);
        dcmInsertString(dicomdir->getDirFileFormat().getDataset(),
                        DCM_SpecificCharacterSetOfFileSetDescriptorFile, scsfsdf);
    }

    iter = goodFileNames.begin();
    while (iter != goodFileNames.end()) {
        if (addToDir(&rootRecord, *iter) == OFFalse) {
            return OFFalse;
        }
        ++iter;
    }

    if (inventAttributes) {
        inventMissingAttributes(&rootRecord);
    }

    if (writeDicomdir) {
        if (verbosemode) {
            COUT << "writing: " << ofname << endl;
        }
        // a DICOMDIR must be written using Little Endian Explicit
        dicomdir->write(EXS_LittleEndianExplicit,
                        lengthEncoding, groupLengthEncoding);
        if (dicomdir->error() != EC_Normal) {
            CERR << "error: cannot create: " << ofname << endl;
        } else {
            if (backupCreated) {
                if (verbosemode) {
                    COUT << "deleting backup: " << backupName << endl;
                }
                unlink(backupName.c_str());
            }
        }
    }

    delete dicomdir;

    return ok;
}

/* All versions of the expandFileNames routine expect that no
 * wildcard characters be present in the fileNames list.
 * On Unix, these are expanded by the shell.
 * On Windows, this should be handled by the OFCommandLine class.
 */

#ifdef HAVE__FINDFIRST
/*
** For Win32 and its evil associates.
*/

#ifdef USE__FINDFIRST
/*
 * This is the old version of expandFileNames() using _findfirst().
 * The new version below directly uses Win32 API calls and is, therefore,
 * more portable.
 */

 static OFBool
expandFileNames(OFList<OFString>& fileNames, OFList<OFString>& expandedNames)
{
    OFBool ok = OFTrue;
    struct _finddata_t fileData;
    long hFile;

    OFListIterator(OFString) iter = fileNames.begin();
    while (iter != fileNames.end()) {
        OFString fname(*iter);
        ++iter;
        if (hFile = _findfirst(fname.c_str(), &fileData) == -1L) {
            CERR << "error: cannot access: " << fname << endl;
                _findclose(hFile);
            ok = OFFalse;
        } else if (fileData.attrib & _A_SUBDIR) {
            /* it is a directory */
            OFList<OFString> subList;
            OFString newSearchname(fname);
            newSearchname += "\\*";
            _findclose(hFile);
            int ret = 0;
            for (hFile = _findfirst(newSearchname.c_str(), &fileData);
                         hFile != -1L && ret == 0; ret = _findnext(hFile, &fileData)) {
                /* filter out current and parent directory */
                if (!cmp(fileData.name, ".") && !cmp(fileData.name, "..")) {
                    OFString subname(fname);
                    subname += PATH_SEPARATOR;
                    subname += fileData.name;
                    subList.push_back(subname);
                }
            }
            _findclose(hFile);
            /* recurse */
            expandFileNames(subList, expandedNames);
        } else {
            /* add to expanded list  */
            _findclose(hFile);
            expandedNames.push_back(fname);
        }
    }
    return ok;
}

#else

static OFBool
expandFileNames(OFList<OFString>& fileNames, OFList<OFString>& expandedNames)
{
  OFBool ok = OFTrue;
  HANDLE     hFile;
  WIN32_FIND_DATA stWin32FindData;
  int             ret;

  OFListIterator(OFString) iter = fileNames.begin();
  while (iter != fileNames.end())
  {
    OFString fname(*iter);
    ++iter;

    hFile = FindFirstFile(fname.c_str(), &stWin32FindData);
    if (hFile == INVALID_HANDLE_VALUE)
    {
      CERR << "error: cannot access: " << fname << endl;
      ok = OFFalse;
    }
    else if(stWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      OFList<OFString> subList;
      OFString     newSearchname(fname);
      newSearchname += "\\*";
      FindClose(hFile);

      hFile = FindFirstFile(newSearchname.c_str(), &stWin32FindData);
      ret = (hFile != INVALID_HANDLE_VALUE);

      while(ret)
      {
                if (!cmp(stWin32FindData.cFileName, ".") && !cmp(stWin32FindData.cFileName, ".."))
                {
          OFString subname(fname);
          subname += PATH_SEPARATOR;
          subname += stWin32FindData.cFileName;
          subList.push_back(subname);
                }
        ret = FindNextFile(hFile, &stWin32FindData);
      }

      if(hFile != INVALID_HANDLE_VALUE)
      {
        FindClose(hFile);
        expandFileNames(subList, expandedNames);
          }
    } else {
      expandedNames.push_back(fname);
    }
  }

  return ok;
}
#endif /* USE__FINDFIRST */

#else
/*
** Unix-like environments
*/
static OFBool
expandFileNames(OFList<OFString>& fileNames, OFList<OFString>& expandedNames)
{
    OFBool ok = OFTrue;
    DIR *dirp = NULL;

    OFListIterator(OFString) iter = fileNames.begin();
    while (iter != fileNames.end()) {
        OFString fname(*iter);
        ++iter;
        if (!fileExists(fname)) {
            CERR << "error: cannot access: " << fname << endl;
            ok = OFFalse;
        } else if ((dirp = opendir(fname.c_str())) != NULL) {
            /* it is a directory */
            OFList<OFString> subList;
            struct dirent *dp = NULL;
            for (dp=readdir(dirp); dp!=NULL; dp=readdir(dirp)) {
                /* filter out current and parent directory */
                if (!cmp(dp->d_name, ".") && !cmp(dp->d_name, "..")) {
                    OFString subname(fname);
                    subname += PATH_SEPARATOR;
                    subname += dp->d_name;
                    subList.push_back(subname);
                }
            }
            closedir(dirp);
            /* recurse */
            expandFileNames(subList, expandedNames);
        } else {
            /* add to expanded list  */
            expandedNames.push_back(fname);
        }
    }
    return ok;
}

#endif

/*
** CVS/RCS Log:
** $Log: dcmgpdir.cc,v $
** Revision 1.48  2001-06-05 10:08:31  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.47  2001/06/01 15:48:29  meichel
** Updated copyright header
**
** Revision 1.46  2000/12/14 12:49:34  joergr
** Updated for 2000 edition of the DICOM standard (added: SR, PR, WV, SP, RT).
**
** Revision 1.45  2000/10/12 10:26:47  meichel
** Updated data dictionary for 2000 edition of the DICOM standard
**
** Revision 1.44  2000/04/14 16:01:01  meichel
** Restructured class DcmTag. Instances don't keep a permanent pointer
**   to a data dictionary entry anymore. Required for MT applications.
**
** Revision 1.43  2000/03/08 16:26:05  meichel
** Updated copyright header.
**
** Revision 1.42  2000/03/06 18:09:37  joergr
** Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
**
** Revision 1.41  2000/03/03 14:05:16  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.40  2000/02/29 11:48:50  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.39  2000/02/23 15:11:34  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.38  2000/02/03 11:49:05  meichel
** Updated dcmgpdir to new directory record structure in letter ballot text
**   of Structured Report.
**
** Revision 1.37  2000/02/02 15:17:13  meichel
** Replaced some #if statements by more robust #ifdef
**
** Revision 1.36  2000/02/01 10:11:58  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.35  1999/07/14 12:02:24  meichel
** Updated data dictionary for supplement 29, 39, 33_lb, CP packet 4 and 5.
**   Corrected dcmtk applications for changes in attribute name constants.
**
** Revision 1.34  1999/04/30 16:40:02  meichel
** Minor code purifications to keep Sun CC 2.0.1 quiet
**
** Revision 1.33  1999/04/27 17:50:51  joergr
** Adapted console applications to new OFCommandLine and OFConsoleApplication
** functionality.
**
** Revision 1.32  1999/04/27 12:23:26  meichel
** Prevented dcmdata applications from opening a file with empty filename,
**   leads to application crash on Win32.
**
** Revision 1.31  1999/04/22 13:43:09  meichel
** Removed carriage returns that VC++ has put into the source code
**
** Revision 1.30  1999/04/22 13:32:52  meichel
** Corrected Win32 API version of expandFileNames routine in dcmgpdir
**
** Revision 1.29  1999/03/31 09:24:20  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.28  1999/03/29 10:14:14  meichel
** Adapted command line options of dcmdata applications to new scheme.
**
** Revision 1.27  1999/03/22 14:10:55  meichel
** Added support for Structured Reports to dcmgpdir.
**   Added preliminary support for including sequences into a DICOMDIR.
**
** Revision 1.26  1999/03/22 09:58:48  meichel
** Reworked data dictionary based on the 1998 DICOM edition and the latest
**   supplement versions. Corrected dcmtk applications for minor changes
**   in attribute name constants.
**
** Revision 1.25  1998/07/15 15:55:04  joergr
** Removed compiler warnings reported by gcc 2.8.1 with additional
** options, e.g. missing const declaration of char pointers. Replaced
** tabs by spaces.
**
** Revision 1.24  1998/02/06 15:07:20  meichel
** Removed many minor problems (name clashes, unreached code)
**   reported by Sun CC4 with "+w" or Sun CC2.
**
** Revision 1.23  1998/01/27 10:49:23  meichel
** Minor bug corrections (string too short, incorrect return value).
**   Thanks to Andreas Barth <anba@bruker.de> for the report.
**
** Revision 1.22  1998/01/14 14:40:33  hewett
** Added support for the VRs UT (Unlimited Text) and VS (Virtual String).
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.21  1997/10/09 11:26:30  hewett
** Fixed dcmgpdir bug related to unlinking a DICOMDIR backup file.
**
** Revision 1.20  1997/10/07 10:12:59  meichel
** Corrected passing of pointer instead of reference.
**
** Revision 1.19  1997/09/22 16:40:10  hewett
** Modified to use the new attribute existance tests
** from DcmItem and the global list dcmImageSOPClassUIDs of SOP
** Classes which can be referenced from a DICOMDIR IMAGE record.
**
** Revision 1.18  1997/09/18 07:35:35  meichel
** Overloading ambiguity removed. Affects systems on which NULL is defined
**   as a (typeless) 0. OFString comparisons with a non-casted NULL will
**   cause compile errors on such systems because 0 is a valid pointer and a
**   valid char at the same time.
**
** Revision 1.17  1997/09/12 11:29:20  hewett
** Modified to use the OFString and OFList classes.  Removed program
** specific String and StringList classes.
**
** Revision 1.16  1997/08/06 12:20:02  andreas
** - Using Windows NT with Visual C++ 4.x the standard open mode for files
**   is TEXT with conversions. For binary files (image files, imagectn database
**   index) this must be changed (e.g. fopen(filename, "...b"); or
**   open(filename, ..... |O_BINARY);)
**
** Revision 1.15  1997/07/21 08:02:11  andreas
** - DcmDirectoryRecord can be build with a referenced Name and a source
**   filename. These name now can differ (lower case - upper case
**   characters).
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.14  1997/07/03 15:09:39  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.13  1997/06/26 12:50:03  andreas
** - Added function version expandFileNames for Windows NT/95
** - Include Additional headers (winsock.h, io.h) for Windows NT/95
**
** Revision 1.12  1997/05/29 15:52:52  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.11  1997/05/22 13:26:24  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.10  1997/05/16 08:31:05  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.9  1997/05/09 13:18:52  hewett
** Added improved error/warning messages and backup of existing DICOMDIR
** file (the backup file has the suffix .BAK and is removed if the new
** DICOMDIR file is sucessfully created).
**
** Revision 1.8  1997/05/06 16:43:47  hewett
** Now possible to invent a value for the PatientID attribute.
** Invention of missing attributes is now delayed when possible until
** after all directory records have been constructed.  This allows
** numbering to be local to the sub-branches of the directory tree.
**
** Revision 1.7  1997/05/06 11:52:30  hewett
** corrected spelling in usage output
**
** Revision 1.6  1997/05/06 09:15:57  hewett
** Added several new capabilities:
** 1. the +r option now enables resursion of a directory tree searching for
**    image files to add to the directory.
** 2. the +m option allows lowercase filenames and filenames with trailing
**    point ('.') to used.  References within the DICOMDIR will be converted
**    to uppercase and the trailing point removed.  This allows images to be
**    read directly from ISO9660 filesystems under Solaris (and probably other
**    OS's which automatically convert filenames on ISO9660 filesystems).
** 3. the +I option allows values for missing attributes to be invented.  Some
**    type 1 attributes in the DICOMDIR are only type 2 in images.  See the
**    documentation for more details.
** 4. Series, Image, Curve, Overlay, ModalityLUT and VOILUT records are now
**    inserted into the DICOMDIR based on the ordering defined by the *Number
**    (e.g. ImageNumber) attributes (if present).
**
** Revision 1.5  1997/04/24 12:16:53  hewett
** Added extended error checking when parsing images prior to
** creating a DICOMDIR.  Checks on allowed characters and
** lengths of file names.
**
** Revision 1.4  1997/04/18 08:06:56  andreas
** - Minor corrections: correct some warnings of the SUN-C++ Compiler
**   concerning the assignments of wrong types and inline compiler
**   errors
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.3  1997/03/27 18:17:25  hewett
** Added checks for attributes required in input files.
**
** Revision 1.2  1997/03/27 15:47:24  hewett
** Added command line switche to allow generation of UN to be
** disabled (it is enabled by default).
**
** Revision 1.1  1997/03/26 17:38:14  hewett
** Initial version.
**
*/

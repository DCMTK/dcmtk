/*
**
** Author: Andrew Hewett    17-03-97
**         
**
** Module: dcmgpdir.cc
**
** Purpose:
** Make a General Purpose DICOMDIR according to the General Purpose 
** CD-R Image Interchange Profile (Supplement 19).
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1999-03-22 14:10:55 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmgpdir.cc,v $
** CVS/RCS Revision:	$Revision: 1.27 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
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
#include <unix.h>	/* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

BEGIN_EXTERN_C
/* This #if code is suggested by the gnu autoconf documentation */
#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMELEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif
#if HAVE_IO_H
#  include <io.h>
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

#include "dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: dcmgpdir v"
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

OFString progname = "dcmgpdir";
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

// ********************************************

static OFBool
expandFileNames(OFList<OFString>& fileNames, OFList<OFString>& expandedNames);

static OFBool
createDicomdirFromFiles(OFList<OFString>& fileNames);

// ********************************************

static void
usage()
{
    cerr << rcsid <<
"\n\n"
"dcmgpdir: create a general purpose dicomdir\n"
"usage: dcmgpdir [options] referenced-dicom-file ...\n"
"options are:\n"
"  output options\n"
"    +D<dicomdir-path>  generate specific DICOMDIR file (default: DICOMDIR\n"
"                       in current directory)\n"
"    +F<file-set-id>    use specific file set ID (default: " << fsid << ")\n"
"    +R[<desc-file>]    add a file set descriptor file ID (e.g. README)\n"
"                       (default: add nothing)\n"
"    +C<char-set>       add a specific character set for descriptor\n"
"                       (default: if descriptor file presend add \"" 
	 << scsfsdf << "\")\n"
"    +I                 invent DICOMDIR type 1 attributes if missing in\n"
"                       image file (default: exit with error)\n"
"    +m                 map to DICOM filenames [lowercase -> uppercase,\n"
"                       and remove trailing period] (default: expect\n"
"                       filenames to already be in DICOM format)\n"
"    -w                 do not write out dicomdir (default: do write)\n"
"    +A                 append to existing dicomdir (default: create new)\n"
"    +r                 recurse within filesystem directories\n"
"  group length encoding:\n"
"    -g                 write without group lengths (default)\n"
"    +g                 write with group lengths\n"
"  length encoding in sequences and items:\n"
"    +e                 write with explicit lengths (default)\n"
"    -e                 write with undefined lengths\n"
"  other test/debug options:\n"
"    -u    disable generation of new VRs (UN/UT/VS)\n"
"    +V    verbose mode\n"
"    +dn   set debug level to n (n=1..9)\n";
}

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

static OFString
basename(const OFString& path)
{
    OFString base(path);
    size_t pos = path.rfind(PATH_SEPARATOR);
    if (pos != OFString_npos) {
	base = path.substr(pos+1);
    }
    return base;
}

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(0);

    prepareCmdLineArgs(argc, argv, progname.c_str());
    progname = basename(argv[0]);

    int localDebugLevel = 0;


    /* parse cmd line */
    if (argc <= 3) {
        usage();
        return 0;
    }

    OFList<OFString> fnames;

    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		cerr << "unknown argument: " << arg << endl;
		usage();
		return 1;
	    }
	    switch (arg[1]) {
	    case 'D':
		if (arg[0] == '+' && arg[2] != '\0') {
		    ofname = arg + 2;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'F':
		if (arg[0] == '+' && arg[2] != '\0') {
		    fsid = arg + 2;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'R':
		if (arg[0] == '+') {
		    if (arg[2] != '\0') {
			fsdfid = arg + 2;
		    } else {
			fsdfid = DEFAULT_FSDFID;
		    }
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'C':
		if (arg[0] == '+' && arg[2] != '\0') {
		    scsfsdf = arg + 2;
		    if (!isaValidCharSetName(scsfsdf)) {
			cerr << "unknown char-set: " << scsfsdf << endl;
		    }
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'I':
		if (arg[0] == '+' && arg[2] == '\0') {
		    inventAttributes = OFTrue;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'm':
		if (arg[0] == '+' && arg[2] == '\0') {
		    mapFilenames = OFTrue;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'w':
		if (arg[0] == '-' && arg[2] == '\0') {
		    writeDicomdir = OFFalse;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'A':
		if (arg[0] == '+' && arg[2] == '\0') {
		    appendMode = OFTrue;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'r':
		if (arg[0] == '+' && arg[2] == '\0') {
		    recurseFilesystem = OFTrue;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'u':
		if (arg[0] == '-' && arg[2] == '\0') {
		    dcmEnableUnknownVRGeneration = OFFalse;
		    dcmEnableUnlimitedTextVRGeneration = OFFalse;
		    dcmEnableVirtualStringVRGeneration = OFFalse;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'g':
		if (arg[0] == '+' && arg[2] == '\0') {
		    groupLengthEncoding = EGL_withGL;
		} else if (arg[0] == '-' && arg[2] == '\0') {
		    groupLengthEncoding = EGL_withoutGL;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'e':
		if (arg[0] == '+' && arg[2] == '\0') {
		    lengthEncoding = EET_ExplicitLength;
		} else if (arg[0] == '-' && arg[2] == '\0') {
		    lengthEncoding = EET_UndefinedLength;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'V':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verbosemode = OFTrue;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'd':
		if (sscanf(arg+2, "%d", &localDebugLevel) != 1) {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    default:
		cerr << "unknown option: " << arg << endl;
		return 1;
	    }
	} else {
	    OFString fname(arg);
	    fnames.push_back(fname);
	}
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
	cerr << "Error: no data dictionary loaded, " 
	     << "check environment variable: " 
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
	return 1; /* DcmDicomDir class dumps core when no data dictionary */
    }

    SetDebugLevel(localDebugLevel);

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
    if (obj != NULL) {
	obj->print();
    }
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
	cerr << "dcmFindString: error while finding " << tag.getTagName() 
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
	cout << "investigating file: " << fname << endl;
    }
    DcmFileStream myin(fname.c_str(), DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
	cerr << "error : cannot open file: " << fname << endl;
	return OFFalse;
    }
    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_noChange);
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
	cerr << "error: " 
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
	cerr << "error: dcmInsertString: null DcmItem argument" << endl;
	return OFFalse;
    }

    DcmTag tag(key);
    DcmElement *elem = newDicomElement(tag);
    E_Condition cond = EC_Normal;

    if (elem == NULL) {
	cerr << "error: dcmInsertString: cannot create DcmElement" << endl;
	return OFFalse;
    }
    if (!s.empty()) {
	cond = elem->putOFStringArray(s);
	if (cond != EC_Normal) {
	    cerr << "error: dcmInsertString: cannot put string" << endl;
	    return OFFalse;
	}
    }
    cond = d->insert(elem, replaceOld);
    if (cond != EC_Normal) {
	cerr << "error: dcmInsertString: cannot insert element" << endl;
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
dcmCopyOptSequence(DcmItem* sink, const DcmTagKey& key, DcmItem* source)
{
    OFBool ok = OFTrue;
    DcmTag tag(key);

    if (tag.getEVR() != EVR_SQ) {
	cerr << "internal error: dcmCopyOptSequence: " 
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
		cerr << "error: dcmInsertSequence: cannot insert element" 
		     << endl;
		ok = OFFalse;
	    }
	} else {
	    ok = OFFalse;
	}
    }
    return ok;
}

static OFString
currentDate() 
{
    char dateBuf[32];
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts == NULL) {
	cerr << "ERROR: cannot get localtime" << endl;
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
	cerr << "ERROR: cannot get localtime" << endl;
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
    date = dcmFindString(d, DCM_ImageDate);
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
    timestr = dcmFindString(d, DCM_ImageTime);
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
	cerr << "error: required attribute " << tag.getTagName() 
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
	cerr << "error: required attribute " << tag.getTagName() 
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
	cerr << "error: file not part 10 format (no meta-header): "
	     << fname << endl;
	ok = OFFalse;
    }

    DcmDataset *d = ff->getDataset();
    if (d == NULL) {
	cerr << "error: file contains no data (no dataset): "
	     << fname << endl;
	/* give up checking */
	return OFFalse;
    }

    /*
    ** Is sop classs ok?
    */
    OFString mediaSOPClassUID = dcmFindString(m, DCM_MediaStorageSOPClassUID);
    if (mediaSOPClassUID.empty()) {
	cerr << "error: MediaStorageSOPClassUID missing in meta-header: "
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
    /* is it an overlay/curve/modality_lut/voi_lut etc ? */
    found = found || cmp(mediaSOPClassUID, UID_StandaloneOverlayStorage);
    found = found || cmp(mediaSOPClassUID, UID_StandaloneCurveStorage);
    found = found || cmp(mediaSOPClassUID, UID_StandaloneModalityLUTStorage);
    found = found || cmp(mediaSOPClassUID, UID_StandaloneVOILUTStorage);
    found = found || cmp(mediaSOPClassUID, UID_PETCurveStorage);
    /* is it one of the Structured reporing SOP Classes? */
    found = found || cmp(mediaSOPClassUID, UID_SRTextStorage);
    found = found || cmp(mediaSOPClassUID, UID_SRAudioStorage);
    found = found || cmp(mediaSOPClassUID, UID_SRDetailStorage);
    found = found || cmp(mediaSOPClassUID, UID_SRComprehensiveStorage);

    /* a detached patient mgmt sop class is also ok */
    found = found || cmp(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);

    if (!found) {
	OFString sopClassName = dcmFindNameOfUID(mediaSOPClassUID.c_str());
	if (sopClassName.empty()) {
	    sopClassName = mediaSOPClassUID;
	}
	cerr << "error: invalid sop class (" << sopClassName
	     << ") for STD-GEN-CD profile: " << fname << endl;
	/* give up checking */
	return OFFalse;
    }

    /* 
    ** Is transfer syntax ok?
    */
    OFString transferSyntax = dcmFindString(m, DCM_TransferSyntaxUID);
    if (transferSyntax.empty()) {
	cerr << "error: TransferSyntaxUID missing in meta-header: "
	     << fname << endl;
	ok = OFFalse;
    }
    found = cmp(transferSyntax, UID_LittleEndianExplicitTransferSyntax);
    if (!found) {
	cerr << "error: LittleEndianExplicitTransferSyntax expected: "
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

    } else if (cmp(mediaSOPClassUID, UID_SRTextStorage) ||
	       cmp(mediaSOPClassUID, UID_SRAudioStorage) ||
	       cmp(mediaSOPClassUID, UID_SRDetailStorage) ||
	       cmp(mediaSOPClassUID, UID_SRComprehensiveStorage)) {
	/* a structured report */
	if (!checkExistsWithValue(d, DCM_ReportStatusID, fname)) 
	    ok = OFFalse; /* ReportStatusIS is type 1 in a Structured Report */
	if (!inventAttributes) {
            /* 
             * NOTE: SR uses the name "Instance Number" for (0020,0013) but we know it
             * as "Image Number".  CP 99 wants to change the name of this attribute.
             */
	    if (!checkExistsWithValue(d, DCM_ImageNumber, fname)) 
		ok = OFFalse;
	}
    } else {
	/* it can only be an image */ 
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_ImageNumber, fname)) 
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
	cerr << "error: out of memory (creating patient record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
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
	cerr << "error: out of memory (creating study record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
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
	cerr << "Warning: " << sourceFileName 
	     << ": StudyDate missing: using alternative: "
	     << altDate << endl;
	dcmInsertString(rec, DCM_StudyDate, altDate);
    }
    if (dcmTagExistsWithValue(d, DCM_StudyTime)) {
	dcmCopyString(rec, DCM_StudyTime, d);
    } else {
	OFString altTime = alternativeStudyTime(d);
	cerr << "Warning: " << sourceFileName
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
	cerr << "error: out of memory (creating series record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
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
	cerr << "error: out of memory (creating image record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
	     << recordTypeToName(rec->getRecordType()) << " directory record: "
	     << dcmErrorConditionToString(rec->error()) << endl;
	delete rec;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_ImageNumber, d);

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
	cerr << "error: out of memory (creating overlay record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
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
	cerr << "error: out of memory (creating modality lut record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
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
	cerr << "error: out of memory (creating voi lut record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
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
	cerr << "error: out of memory (creating curve record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
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
	cerr << "error: out of memory (creating curve record)" << endl;
	return NULL;
    }
    if (rec->error() != EC_Normal) {
	cerr << "error: cannot create " 
	     << recordTypeToName(rec->getRecordType()) << " directory record: "
	     << dcmErrorConditionToString(rec->error()) << endl;
	delete rec;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_ReportStatusID, d);
    /* 
     * NOTE: SR uses the name "Instance Number" for (0020,0013) but we know it
     * as "Image Number".  CP 99 wants to change the name of this attribute.
     */
    dcmCopyString(rec, DCM_ImageNumber, d);
    dcmCopyOptString(rec, DCM_InterpretationRecordedDate, d);

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
		    cerr << "error: cannot locate referenced file: " 
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
	/*
	** The attribute ReferencedSOPInstanceUID is automatically
	** put into a Directory Record when a filename is present.
	*/
	match = cmp(dcmFindString(rec, DCM_ReferencedSOPInstanceUIDInFile),
		    dcmFindString(dataset, DCM_SOPInstanceUID));
	break;
    default:	
	cerr << "error: record type not yet implemented" << endl;
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
    default:
	cerr << "error: record type not yet implemented" << endl;
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
	out << "ReferencedSOPInstanceUIDInFile " 
	    << DCM_ReferencedSOPInstanceUIDInFile << "=\""
	    << dcmFindString(rec, DCM_ReferencedSOPInstanceUIDInFile) << "\"";
	break;
    default:
	cerr << "error: record type not yet implemented" << endl;
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
	cerr << "Warning: file inconsistant with existing DICOMDIR record"
	     << endl;
	cerr << "  " << recordTypeToName(rec->getRecordType()) 
	     << " Record [Key: ";
	printRecordUniqueKey(cerr, rec);
	cerr << "]" << endl;
	cerr << "    Existing Record (origin: " << rec->getRecordsOriginFile()
	     << ") defines: " << endl;
	cerr << "      "; 
	printAttribute(cerr, tag, s1); cerr << endl;
	cerr << "    File (" << sourceFileName << ") defines:" << endl;
	cerr << "      "; 
	printAttribute(cerr, tag, s2); cerr << endl;
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

    if (tag.getDictRef() != NULL) {
        return tag.getTagName();
    }
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
        DcmVR vr1(elem1->getVR());
        reason = "INTERNAL ERROR: Unexpected VR encountered: ";
        reason += vr1.getVRName();
        reason += "(" + constructTagName(elem1) + ")"; 
        return OFFalse;
    }

    if (ec1 != EC_Normal || ec2 != EC_Normal) {
	DcmTag tag();
	cerr << "dcmFindSequence: error while getting value of " << elem1->getTag().getTagName() 
	     << " " << elem1->getTag().getXTag() << ": "
             << dcmErrorConditionToString((ec1 != EC_Normal)?(ec1):(ec2)) << endl;
        reason = "cannot access binary value";
        return OFFalse;
    }

    Uint32 len = elem1->getLength();

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
            intToOFString(elem1->getLength()) + "!=" + intToOFString(elem2->getLength()) +
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


    int n1 = item1->card();
    int n2 = item2->card();

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

    int n1 = sq1->card();
    int n2 = sq2->card();

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
	cerr << "dcmFindSequence: error while finding " << tag.getTagName() 
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
	cerr << "Warning: file inconsistant with existing DICOMDIR record"
	     << endl;
	cerr << "  " << recordTypeToName(rec->getRecordType()) 
	     << " Record [Key: ";
	printRecordUniqueKey(cerr, rec);
	cerr << "]" << endl;
        cerr << "    Reason: " << reason << endl;;
        cerr << "    Existing Record (origin: " << rec->getRecordsOriginFile()
	     << ") defines: " << endl;
	sq1->print(cerr, OFTrue, 2); 
	cerr << "    File (" << sourceFileName << ") defines:" << endl;
	sq2->print(cerr, OFTrue, 2); 
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
                    cerr << "INTERNAL ERROR: cannot compare: " << tag << endl;
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
	/* try to insert based on ImageNumber */
	cond = insertWithISCriterion(parent, child, DCM_ImageNumber);
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
	/* try to insert based on InstanceNumber */
        /* 
         * NOTE: SR uses the name "Instance Number" for (0020,0013) but we know it
         * as "Image Number".  CP 99 wants to change the name of this attribute.
         */
	cond = insertWithISCriterion(parent, child, DCM_ImageNumber);
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
		cerr << "error: " << dcmErrorConditionToString(cond) 
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
	cout << "adding: " << fname << endl;
    }
    DcmFileStream myin(fname.c_str(), DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
	cerr << "cannot open file: " << fname << endl;
	return OFFalse;
    }
    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_noChange );
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
	cerr << "error: " << dcmErrorConditionToString(ff.error())
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
	    cerr << "error: " << dcmErrorConditionToString(cond) 
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
    } else if (cmp(sopClass, UID_SRTextStorage) ||
	       cmp(sopClass, UID_SRAudioStorage) ||
	       cmp(sopClass, UID_SRDetailStorage) ||
	       cmp(sopClass, UID_SRComprehensiveStorage)) {
	/* Add a structured report */
	rec = includeRecord(seriesRec, ERT_StructReport, dataset, fname, ifname);
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
	    cerr << "error: invalid character encountered: "
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
    /*
    ** Is the file name path ok?
    ** The filename is assumed to be in local format for this check.
    */
    OFString invalidChars = locateInvalidFileNameChars(fname, sep);
    if (!invalidChars.empty()) {
	cerr << "error: invalid characters (\"" << invalidChars 
	     << "\") in filename: " << fname << endl;
	ok = OFFalse;
    }
    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (componentCount(fname, sep) > MAX_FNAME_COMPONENTS) {
	cerr << "error: too many path components (max " 
	     << MAX_FNAME_COMPONENTS << ") in filename: "
	     << fname << endl;
	ok = OFFalse;
    }
    /*
    ** Ensure that each component is not too large
    */
    if (isComponentTooLarge(fname, MAX_FNAME_COMPONENT_SIZE, sep)) {
	cerr << "error: component too large (max " 
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
	cerr << "       invalid characters in FileSetID: " 
	     << aFsid << endl;
	ok = OFFalse;
    }
    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (componentCount(aFsid) != 1) {
	cerr << "error: too many components in FileSetID: " 
	     << aFsid << endl;
	ok = OFFalse;
    }
    /*
    ** Ensure that each component is not too large
    */
    DcmVR cs(EVR_CS);
    if (isComponentTooLarge(aFsid, (int)(cs.getMaxValueLength()))) {
	cerr << "error: too large: " << aFsid << endl;
	ok = OFFalse;
    }
    return ok;
}


static OFBool
checkFileCanBeUsed(const OFString& fname) 
{
    if (verbosemode) {
	cout << "checking: " << fname << endl;
    }

    if (!isaValidFileName(fname)) {
	return OFFalse;
    }

    /*
    ** Does the file exist??
    */
    DcmFileStream myin(fname.c_str(), DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
	cerr << "error: cannot open file: " << fname << endl;
	/* cannot continue checking */
	return OFFalse;
    }

    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_noChange );
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
	cerr << "error: " 
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
    int structReportInstanceNumber = 1;

    int count = (int)(parent->cardSub());
    for (int i=0; i<count; i++) {
	DcmDirectoryRecord* rec = parent->getSub(i);
	
	switch (rec->getRecordType()) {
	case ERT_Image:
	    if (!dcmTagExistsWithValue(rec, DCM_ImageNumber)) {
		OFString defNum = defaultNumber(imageNumber++);
		cerr << "Warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing ImageNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_ImageNumber, defNum);
	    }
	    break;
	case ERT_Overlay:
	    if (!dcmTagExistsWithValue(rec, DCM_OverlayNumber)) {
		OFString defNum = defaultNumber(overlayNumber++);
		cerr << "Warning: " <<  recordTypeToName(rec->getRecordType())
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
		cerr << "Warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing LutNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_LookupTableNumber, defNum);
	    }
	    break;
	case ERT_Curve:
	    if (!dcmTagExistsWithValue(rec, DCM_CurveNumber)) {
		OFString defNum = defaultNumber(curveNumber++);
		cerr << "Warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing CurveNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_CurveNumber, defNum);
	    }
	    break;
	case ERT_StructReport:
            /* 
             * NOTE: SR uses the name "Instance Number" for (0020,0013) but we know it
             * as "Image Number".  CP 99 wants to change the name of this attribute.
             */
	    if (!dcmTagExistsWithValue(rec, DCM_ImageNumber)) {
		OFString defNum = defaultNumber(structReportInstanceNumber++);
		cerr << "Warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing InstanceNumber/ImageNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_ImageNumber, defNum);
	    }
	    break;
	default:
	    cerr << "error: (INTERNAL): inventMissingImageLevelAttributes: "
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
	    cerr << "Warning: Series Record (origin: " 
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
	    cerr << "Warning: Study Record (origin: " 
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
	    cerr << "Warning: Patient Record (origin: " 
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
	cerr << "error: copying files: cannot open: " << fromfname << endl;
	return OFFalse;
    }
    if ((tof = fopen(tofname.c_str(), "wb")) == NULL) {
	cerr << "error: copying files: cannot create: " << tofname << endl;
	fclose(fromf);
	return OFFalse;
    }
    OFBool ok = OFTrue;
    int c = 0;
    while ((c = getc(fromf)) != EOF) {
	if (putc(c, tof) == EOF) {
	    cerr << "error: copying files: " << fromfname << " to "
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
	cerr << "error: invalid FileSetID: " 
	     << fsdfid << endl;
	ok = OFFalse;
    }

    if (!fsdfid.empty() && !isaValidFileName(fsdfid)) {
	cerr << "       bad FileSetDescriptorFileID: " 
	     << fsdfid << endl;
	ok = OFFalse;
    }

    if (!fsdfid.empty() && !fileExists(fsdfid)) {
	cerr << "error: cannot find FileSetDescriptorFileID: " 
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
	cerr << "FATAL ERROR: no good files: DICOMDIR not created" << endl;
	return OFFalse;
    } else if (!badFileNames.empty()) {
	cerr << "WARNING: " << badFileNames.size() 
	     << " file(s) cannot be added to DICOMDIR: " << endl;
	iter = badFileNames.begin();
	while (iter != badFileNames.end()) {
	    cerr << "         " << *iter << endl;
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
	    cout << "creating backup dicomdir: " << backupName << endl;
	}
	if (copyFile(ofname, backupName)) {
	    backupCreated = OFTrue;
	} else {
	    cerr << "error: cannot create backup of: " << ofname << endl;
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
	    cout << "writing: " << ofname << endl;
	}
	// a DICOMDIR must be written using Little Endian Explicit
        dicomdir->write(EXS_LittleEndianExplicit, 
			lengthEncoding, groupLengthEncoding);
	if (dicomdir->error() != EC_Normal) {
	    cerr << "error: cannot create: " << ofname << endl;
	} else {
	    if (backupCreated) {
		if (verbosemode) {
		    cout << "deleting backup: " << backupName << endl;
		}
		unlink(backupName.c_str());
	    }
	}
    }

    delete dicomdir;

    return ok;
}



#if HAVE__FINDFIRST
/*
** For Win32 and its evil associates.
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
	    cerr << "error: cannot access: " << fname << endl;
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
	    cerr << "error: cannot access: " << fname << endl;
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
** Revision 1.27  1999-03-22 14:10:55  meichel
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
**
*/

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
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-08-06 12:20:02 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmgpdir.cc,v $
** CVS/RCS Revision:	$Revision: 1.16 $
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

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

#include "dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: dcmgpdir v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default File-Set ID */
#define DEFAULT_FSID "DEMO CAR 97"
/* default File-Set Descriptor File ID */
#define DEFAULT_FSDFID "README"
/* default Specific Character Set of File-Set Descriptor File */
#define DEFAULT_SCSFSDF "ISO_IR 100"

/* an ISO 9660 format only allows 8 characters in file name */
#define MAX_FNAME_COMPONENT_SIZE 8
/* DICOM only allows max 8 path components in a file name */
#define MAX_FNAME_COMPONENTS 8

char *progname = "dcmgpdir";
char *ofname = "DICOMDIR";

/* actual File-Set ID */
char *fsid = DEFAULT_FSID;
/* actual File-Set Descriptor File ID */
char *fsdfid = NULL; /* can be set to DEFAULT_FSDFID during option handling */
/* actual Specific Character Set of File-Set Descriptor File */
char *scsfsdf = DEFAULT_SCSFSDF;

OFBool verbosemode = OFFalse;
OFBool writeDicomdir = OFTrue;
OFBool appendMode = OFFalse;
OFBool inventAttributes = OFFalse;
OFBool mapFilenames = OFFalse;
OFBool recurseFilesystem = OFFalse;

E_EncodingType lengthEncoding = EET_ExplicitLength;
E_GrpLenEncoding groupLengthEncoding = EGL_withoutGL;


// ********************************************

class StrListNode {
private:
    friend class StrList;
    friend class StrListIterator;
    char* str;
    StrListNode* next;
public:
    const char* getValue() { return str; }
    void setValue(const char* s) {
	if (str) {
	    delete str;
	    str = NULL;
	}
	if (s == NULL) {
	    str = NULL;
	} else {
	    str = new char[strlen(s) + 1];
	    strcpy(str, s);
	} 
    }
    StrListNode(const char* s = NULL, StrListNode* n = NULL) {
	str = NULL;
	setValue(s);
	next = n; 
    }
    ~StrListNode() { delete str; }
};

class StrList {
private:
    friend class StrListIterator;
    StrListNode* head;
    StrListNode* tail;
    int count;
public:
    StrList() {
	head = tail = NULL;
	count = 0;
    }
    
    int size() {
	return count;
    }

    void append(const char* s) {
	StrListNode* sn = new StrListNode(s);
	if (tail != NULL) {
	    tail->next = sn;
	}
	tail = sn;
	if (head == NULL) {
	    head = sn;
	}
	count++;
    }

    void clear() {
	StrListNode* sn = head;
	while (sn) {
	    StrListNode* n = sn->next;
	    delete sn;
	    sn = n;
	}
	head = tail = NULL;
    }

    ~StrList() {
	clear();
    }
};

class StrListIterator {
private:
    StrListNode* current;
public:
    StrListIterator() {
	current = NULL;
    }
    StrListIterator(StrList& list) { 
	start(list);
    }
    void start(StrList& list) {
	current = list.head;
    }
    OFBool hasMore() {
	return (current != NULL);
    }
    const char* next() {
	const char* s = NULL;
	if (current != NULL) {
	    s = current->str;
	    current = current->next;
	}
	return s;
    }
};

// ********************************************

static OFBool
expandFileNames(StrList& fileNames, StrList& expandedNames);

static OFBool
createDicomdirFromFiles(StrList& fileNames);

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
"    -u    disable generation of unknown VR (UN)\n"
"    +V    verbose mode\n"
"    +dn   set debug level to n (n=1..9)\n";
}

static OFBool
isaValidCharSetName(const char* cs)
{
    char* dcmCharSetNames[] = {
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
	found = (strcmp(cs, dcmCharSetNames[i]) == 0);
    }
    return found;
}

static char*
basename(char* path)
{
    char* s = strrchr(path, PATH_SEPARATOR);
    if (s == NULL) {
	s = path;
    } else if (s[0] == '/') {
	s++;
    }
    return s;
}

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(0);

    prepareCmdLineArgs(argc, argv, progname);
    progname = basename(argv[0]);

    int localDebugLevel = 0;


    /* parse cmd line */
    if (argc <= 3) {
        usage();
        return 0;
    }

    StrList fnames;

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
	    fnames.append(arg);
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
    StrList expandedNames;
    StrList* slist = &fnames;
    if (recurseFilesystem) {
	ok = expandFileNames(fnames, expandedNames);
	slist = &expandedNames;
    }
    if (ok) {
	ok = createDicomdirFromFiles(*slist);
    }
    return (ok)?(0):(1);
}

/*
** Bebug assistance
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
dcmTagExists(DcmItem* d, const DcmTagKey& xtag, OFBool searchIntoSub = OFFalse)
{
    DcmStack stack;
    E_Condition ec = EC_Normal;
    
    ec = d->search(xtag, stack, ESM_fromHere, searchIntoSub);

    return (ec == EC_Normal);
}

static OFBool
dcmTagExistsWithValue(DcmItem* d, const DcmTagKey& xtag, 
		      OFBool searchIntoSub = OFFalse)
{
    DcmElement *elem = NULL;
    DcmStack stack;
    E_Condition ec = EC_Normal;
    Uint32 len = 0;

    
    ec = d->search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
	len = elem->getLength();
    }

    return (ec == EC_Normal) && (len > 0);
}

static const char* 
dcmFindString(DcmItem* d, const DcmTagKey& xtag, OFBool searchIntoSub = OFFalse)
{
    DcmElement *elem = NULL;
    DcmStack stack;
    E_Condition ec = EC_Normal;
    char* s = NULL;
    
    ec = d->search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
	ec = elem->getString(s);
    }

    return s;
}

static const char* 
dcmFindStringInFile(const char* fname, const DcmTagKey& xtag, 
		    OFBool searchIntoSub = OFFalse)
{
    /*
    ** This function copies the requested value into a static string 
    ** which is returned.  Callers should not modify the string.
    ** This is necesary because the DcmFileFormat is deleted.
    */

    if (verbosemode) {
	cout << "investigating file: " << fname << endl;
    }
    DcmFileStream myin(fname, DCM_ReadMode);
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

    DcmElement *elem = NULL;
    DcmStack stack;
    E_Condition ec = EC_Normal;
    char* s = NULL;
    
    ec = d->search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
	ec = elem->getString(s);
    }

    static char* staticstr = NULL;
    if (staticstr != NULL) {
	delete staticstr;
	staticstr = NULL;
    }
    if (s != NULL) {
	staticstr = new char[strlen(s)+1];
	strcpy(staticstr, s);
    }

    return staticstr;
}

static OFBool 
dcmInsertString(DcmItem* d, const DcmTagKey& xtag, 
		const char* s, OFBool replaceOld = OFTrue)
{
    if (d == NULL) {
	cerr << "error: dcmInsertString: null DcmItem argument" << endl;
	return OFFalse;
    }

    DcmTag tag(xtag);
    DcmElement *elem = newDicomElement(tag);
    E_Condition cond = EC_Normal;

    if (elem == NULL) {
	cerr << "error: dcmInsertString: cannot create DcmElement" << endl;
	return OFFalse;
    }
    if (s != NULL) {
	cond = elem->putString(s);
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
dcmCopyString(DcmItem* sink, const DcmTagKey& xtag, DcmItem* source)
{
    return dcmInsertString(sink, xtag, dcmFindString(source, xtag));
}

static OFBool 
dcmCopyOptString(DcmItem* sink, const DcmTagKey& xtag, DcmItem* source)
{
    OFBool ok = OFTrue;
    if (dcmTagExists(source, xtag)) {
	ok = dcmInsertString(sink, xtag, dcmFindString(source, xtag));
    }
    return ok;
}

#ifdef UNUSED_FUNCTION
static OFBool 
dcmCopyStringWithDefault(DcmItem* sink, const DcmTagKey& xtag, 
			 DcmItem* source, const char* defaultString)
{
    OFBool ok = OFTrue;
    if (dcmTagExistsWithValue(source, xtag)) {
	ok = dcmInsertString(sink, xtag, dcmFindString(source, xtag));
    } else {
	ok = dcmInsertString(sink, xtag, defaultString);
    }
    return ok;
}
#endif

static OFBool 
dcmCopyOptSequence(DcmItem* sink, const DcmTagKey& xtag, DcmItem* source)
{
    OFBool ok = OFTrue;
    DcmTag tag(xtag);

    if (tag.getEVR() != EVR_SQ) {
	cerr << "internal error: dcmCopyOptSequence: " 
	     << xtag << " not SQ" << endl;
	abort();
    }

    if (dcmTagExists(source, xtag)) {
	DcmSequenceOfItems *sq = NULL;
	DcmStack stack;
	E_Condition ec = EC_Normal;
    
	ec = source->search(xtag, stack, ESM_fromHere, OFFalse);

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

static const char*
currentDate() 
{
    static char dateBuf[32];
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts == NULL) {
	cerr << "ERROR: cannot get localtime" << endl;
	return NULL;
    }
    int year = 1900 + ts->tm_year;
    sprintf(dateBuf, "%04d%02d%02d", year, ts->tm_mon + 1, ts->tm_mday);
    return dateBuf;
}

static const char*
currentTime() 
{
    static char timeBuf[32];
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts == NULL) {
	cerr << "ERROR: cannot get localtime" << endl;
	return NULL;
    }
    sprintf(timeBuf, "%02d%02d%02d", ts->tm_hour, ts->tm_min, ts->tm_sec);
    return timeBuf;
}

static const char* 
alternativeStudyDate(DcmItem* d)
{
    /* use another date if present */
    const char* date = dcmFindString(d, DCM_SeriesDate);
    if (date != NULL) return date;
    date = dcmFindString(d, DCM_AcquisitionDate);
    if (date != NULL) return date;
    date = dcmFindString(d, DCM_ImageDate);
    if (date != NULL) return date;
    /* use current date */
    return currentDate();
}

static const char* 
alternativeStudyTime(DcmItem* d)
{
    /* use another time if present */
    const char* time = dcmFindString(d, DCM_SeriesTime);
    if (time != NULL) return time;
    time = dcmFindString(d, DCM_AcquisitionTime);
    if (time != NULL) return time;
    time = dcmFindString(d, DCM_ImageTime);
    if (time != NULL) return time;
    /* use current time */
    return currentTime();
}


static const char*
defaultID(const char* prefix, int number) 
{
    static char idbuf[128];
    unsigned long ul = (unsigned long)number;
    char cappedPrefix[11];
    memset(cappedPrefix, 0, 11);
    strncpy(cappedPrefix, prefix, 10); /* use a most 10 chars from prefix */
    sprintf(idbuf, "%s%06lu", cappedPrefix, ul);
    return idbuf;
}

static const char*
defaultNumber(int number) 
{
    static char numbuf[64];
    unsigned long ul = (unsigned long)number;
    sprintf(numbuf, "%lu", ul);
    return numbuf;
}


/*
** Filename manipulation
*/

static void 
hostToDicomFilename(char* fname)
{
    /*
    ** Massage filename into dicom format 
    ** (dos conventions for path separators)
    */
    int len = strlen(fname);
    int k = 0;
    for (int i=0; i<len; i++) {
	char c = fname[i];
	/* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
	if (c == PATH_SEPARATOR) {
	    fname[k++] = '\\';
	} else if (isalpha(c) || isdigit(c) || (c == '_') || (c == '\\')) {
	    /* filenames in DICOM must always be in uppercase */
	    fname[k++] = toupper(c);
	}
    }
    fname[k] = '\0';
}

static void 
dicomToHostFilename(char* fname, OFBool mapToLower = OFFalse)
{
    /*
    ** Massage filename into machine format 
    ** (replace dos conventions for path separators)
    */
    int len = strlen(fname);
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
componentCount(const char* fname, char separator = PATH_SEPARATOR)
{
    if (fname == NULL) return 0;
    int n = 1;
    int len = strlen(fname);
    for (int i=0; i<len; i++) {
	if (fname[i] == separator) {
	    n++;
	}
    }
    return n;
}

static OFBool 
isComponentTooLarge(const char* fname,
		    int componentLimit,
		    char separator = PATH_SEPARATOR)
{
    if (fname == NULL) return -1;
    int n = 0;
    const char* s = fname;
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
fileExists(const char* fname) 
{
    FILE* f = fopen(fname, "r");
    if (f == NULL) {
	return OFFalse;
    }
    fclose(f);
    return OFTrue;
}


static OFBool
cmp(const char* s1, const char* s2)
{
    if (s1 == NULL || s2 == NULL) {
	return OFFalse;
    }
    return (strcmp(s1,s2) == 0)?(OFTrue):(OFFalse);
}

static OFBool
checkExists(DcmItem* d, const DcmTagKey& key, const char* fname)
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
checkExistsWithValue(DcmItem* d, const DcmTagKey& key, const char* fname)
{
    if (!checkExists(d, key, fname)) {
	return OFFalse;
    }
    const char* s = NULL;
    if ((s = dcmFindString(d, key)) == NULL) {
	DcmTag tag(key);
	cerr << "error: required attribute " << tag.getTagName() 
	     << " " << key << " has no value in file: "
	     << fname << endl;
	return OFFalse;
    }
    return OFTrue;
}

static const char*
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
checkImage(const char* fname, DcmFileFormat *ff)
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
    const char* mediaSOPClassUID = 
	dcmFindString(m, DCM_MediaStorageSOPClassUID);
    if (mediaSOPClassUID == NULL) {
	cerr << "error: MediaStorageSOPClassUID missing in meta-header: "
	     << fname << endl;
	ok = OFFalse;
    }
    /* 
    ** Check if the SOP Class is a known storage SOP class (an image, overlay,
    ** curve, etc.
    */
    OFBool found = OFFalse;

    for (int i=0; i<numberOfDcmStorageSOPClassUIDs && !found; i++) {
	found = cmp(mediaSOPClassUID, dcmStorageSOPClassUIDs[i]);
    }
    /* a detached patient mgmt sop class is also ok */
    if (!found) {
	found = cmp(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);
    }
    if (!found) {
	cerr << "error: invalid sop class for STD-GEN-CD profile: "
	     << fname << endl;
	ok = OFFalse;
    }

    /* 
    ** Is transfer syntax ok?
    */
    const char* transferSyntax = dcmFindString(m, DCM_TransferSyntaxUID);
    if (transferSyntax == NULL) {
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
    /* PatientName is type 2 in DICOMDIR and images */
    if (!checkExists(d, DCM_PatientName, fname)) ok = OFFalse;
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
    /* StudyInstanceIOD is type 1 in DICOMDIR and images */
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
	    if (!checkExistsWithValue(d, DCM_LUTNumber, fname)) 
		ok = OFFalse;
	}
    } else if (cmp(mediaSOPClassUID, UID_StandaloneVOILUTStorage)) {
	/* a voi lut */
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_LUTNumber, fname)) 
		ok = OFFalse;
	}
    } else if (cmp(mediaSOPClassUID, UID_StandaloneCurveStorage)) {
	/* a curve */
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_CurveNumber, fname)) 
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
    const char * referencedFileName, 
    DcmItem* d,
    const char * sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_Patient, referencedFileName, sourceFileName);
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
    dcmCopyString(rec, DCM_PatientName, d);

    return rec;
}

DcmDirectoryRecord* 
buildStudyRecord(const char* referencedFileName, DcmItem* d, 
		 const char* sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_Study, referencedFileName, sourceFileName);
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
	const char* altDate = alternativeStudyDate(d);
	cerr << "warning: " << sourceFileName 
	     << ": StudyDate missing: using alternative: "
	     << altDate << endl;
	dcmInsertString(rec, DCM_StudyDate, altDate);
    }
    if (dcmTagExistsWithValue(d, DCM_StudyTime)) {
	dcmCopyString(rec, DCM_StudyTime, d);
    } else {
	const char* altTime = alternativeStudyTime(d);
	cerr << "warning: " << sourceFileName
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
    const char* referencedFileName, 
    DcmItem* d,
    const char * sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_Series, referencedFileName, sourceFileName);
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
    const char* referencedFileName, 
    DcmItem* d, 
    const char * sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_Image, referencedFileName, sourceFileName);
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
    const char* referencedFileName, 
    DcmItem* d,
    const char * sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_Overlay, referencedFileName, sourceFileName);
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
    const char* referencedFileName, 
    DcmItem* d,
    const char * sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_ModalityLut, referencedFileName, sourceFileName);
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
    dcmCopyString(rec, DCM_LUTNumber, d);

    return rec;
}

DcmDirectoryRecord* 
buildVoiLutRecord(
    const char* referencedFileName, 
    DcmItem* d,
    const char * sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_VoiLut, referencedFileName, sourceFileName);
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
    dcmCopyString(rec, DCM_LUTNumber, d);

    return rec;
}

DcmDirectoryRecord* 
buildCurveRecord(
    const char* referencedFileName, 
    DcmItem* d,
    const char * sourceFileName)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(
	ERT_Curve, referencedFileName, sourceFileName);
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

static char*
locateDicomFile(const char* fname)
{
    char* fn = new char[strlen(fname) + 2];
    strcpy(fn, fname);
    dicomToHostFilename(fn);
    if (fileExists(fn)) {
	return fn;
    }
    /* trailing period */
    strcpy(fn, fname);
    strcat(fn, ".");
    dicomToHostFilename(fn);
    if (fileExists(fn)) {
	return fn;
    }
    /* lowercase */
    strcpy(fn, fname);
    dicomToHostFilename(fn, OFTrue);
    if (fileExists(fn)) {
	return fn;
    }
    /* lowercase with trailing period */
    strcpy(fn, fname);
    strcat(fn, ".");
    dicomToHostFilename(fn, OFTrue);
    if (fileExists(fn)) {
	return fn;
    }

    return NULL;
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
	    ** try using the PatientName
	    */
	    match = cmp(dcmFindString(rec, DCM_PatientName),
			dcmFindString(dataset, DCM_PatientName));
	}
	break;
    case ERT_Study:
	if (dcmTagExists(rec, DCM_StudyInstanceUID)) {
	    match = cmp(dcmFindString(rec, DCM_StudyInstanceUID),
			dcmFindString(dataset, DCM_StudyInstanceUID));
	} else {
	    /* the Study Instance UID can be in the referenced file instead */
	    const char *reffname = dcmFindString(rec, DCM_ReferencedFileID);
	    if (reffname != NULL) {
		char* fname = locateDicomFile(reffname);
		if (fname != NULL) {
		    match = cmp(dcmFindStringInFile(fname, 
						    DCM_StudyInstanceUID),
				dcmFindString(dataset, DCM_StudyInstanceUID));
		} else {
		    cerr << "error: cannot locate referenced file: " 
			 << reffname << endl;
		}
		delete fname;
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
	break;
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
buildRecord(E_DirRecType dirtype, const char* referencedFileName, 
	    DcmItem* dataset, const char* sourceFileName)
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
    default:
	cerr << "error: record type not yet implemented" << endl;
	return OFFalse;
	break;
    }
    return rec;
}

static void 
printAttribute(ostream& o, DcmTagKey& key, const char* s)
{
    if (s == NULL) {
	s = "<null>";
    }
    DcmTag tag(key);
    o << tag.getTagName() << " " << key << "=\"" << s << "\"";
}

static OFBool
compareStringAttributes(DcmTagKey& tag, DcmDirectoryRecord *rec, 
			DcmItem* dataset, const char* sourceFileName)
{
    const char* s1 = dcmFindString(rec, tag);
    const char* s2 = dcmFindString(dataset, tag);
    OFBool equals = cmp(s1, s2);

    if (!equals) {
	cerr << "Warning: file inconsistant with existing DICOMDIR record"
	     << endl;
	cerr << "  " << recordTypeToName(rec->getRecordType()) 
	     << " Record (origin: " << rec->getRecordsOriginFile()
	     << ") defines: " << endl;
	cerr << "    "; 
	printAttribute(cerr, tag, s1); cerr << endl;
	cerr << "  File (" << sourceFileName << ") defines:" << endl;
	cerr << "    "; 
	printAttribute(cerr, tag, s2); cerr << endl;
    }

    return equals;
}

static void
warnAboutInconsistantAttributes(DcmDirectoryRecord *rec, 
				DcmItem* dataset, const char* sourceFileName)
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
		** We currently only handle strings.
		** This is not a huge problem since all the DICOMDIR
		** attributes we generate are strings.
		*/
		switch (re->getVR()) {
		case EVR_AE:
		case EVR_AS:
		case EVR_CS:
		case EVR_DA:
		case EVR_DS:
		case EVR_DT:
		case EVR_IS:
		case EVR_LO:
		case EVR_LT:
		case EVR_PN:
		case EVR_SH:
		case EVR_ST:
		case EVR_TM:
		case EVR_UI:
		    compareStringAttributes(tag, rec, dataset, sourceFileName);
		    break;
		default:
		    break;
		}
	    }
	}
    }
}

static int
getISNumber(DcmItem *i, const DcmTagKey& key) 
{
    int num = 0;
    const char* s = dcmFindString(i, key);
    if (s != NULL) {
	sscanf(s, "%d", &num);
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
    int count = parent->cardSub();
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
	cond = insertWithISCriterion(parent, child, DCM_LUTNumber);
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
	      DcmItem* dataset, const char* referencedFileName,
	      const char* sourceFileName)
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
addToDir(DcmDirectoryRecord* rootRec, const char* ifname)
{
    char fname[1024];

    strcpy(fname, ifname);

    if (verbosemode) {
	cout << "adding: " << fname << endl;
    }
    DcmFileStream myin(fname, DCM_ReadMode);
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
    const char* sopClass = 
	dcmFindString(metainfo, DCM_MediaStorageSOPClassUID);

    /*
    ** Build the necessary dicomdir records
    */

    /*
    ** Add a patient record underneath the root
    */
    DcmDirectoryRecord *patientRec = 
	includeRecord(rootRec, ERT_Patient, dataset, NULL, ifname);
    if (patientRec == NULL) {
	return OFFalse;
    }
    if (patientRec->getRecordsOriginFile() == NULL) {
	// not yet noted
	patientRec->setRecordsOriginFile(ifname);
    }
    warnAboutInconsistantAttributes(patientRec, dataset, ifname);

    /* if PatientMgmgt file then attach it to Patient Record and stop */
    if (cmp(sopClass, UID_DetachedPatientManagementMetaSOPClass)) {
	cond = patientRec->assignToSOPFile(fname, ifname);
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
	includeRecord(patientRec, ERT_Study, dataset, NULL, ifname);
    if (studyRec == NULL) {
	return OFFalse;
    }
    if (studyRec->getRecordsOriginFile() == NULL) {
	// not yet noted
	studyRec->setRecordsOriginFile(ifname);
    }
    warnAboutInconsistantAttributes(studyRec, dataset, ifname);

    /*
    ** Add a series record underneath the actual study
    */
    DcmDirectoryRecord *seriesRec = 
	includeRecord(studyRec, ERT_Series, dataset, NULL, ifname);
    if (seriesRec == NULL) {
	return OFFalse;
    }
    if (seriesRec->getRecordsOriginFile() == NULL) {
	// not yet noted
	seriesRec->setRecordsOriginFile(ifname);
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
    } else if (cmp(sopClass, UID_StandaloneCurveStorage)) {
	/* Add a curve record */
	rec = includeRecord(seriesRec, ERT_Curve, dataset, fname, ifname);
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
	rec->setRecordsOriginFile(ifname);
    }
    warnAboutInconsistantAttributes(rec, dataset, ifname);

    return OFTrue;
}

static OFBool 
areCSCharsValid(const char* fname)
{
    int len = strlen(fname);
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

static OFBool 
areFileNameCharsValid(const char* fname,
		      char separator = PATH_SEPARATOR)
{
    int len = strlen(fname);
    if (mapFilenames && fname[len-1] == '.') {
	/* disregard trailing point */
	len--;
    }
    for (int i=0; i<len; i++) {
	char c = fname[i];
	if ((c == '_') || isdigit(c) || (c == separator) || 
	    (isalpha(c) && (isupper(c) || (islower(c) && mapFilenames)))) {
	    /* all ok */
	} else if (c == '\\') {
	    /* this is only ok if the underlying OS uses \ */
	    if (separator != '\\') {
		cerr << "error: invalid character encountered: "
		     << char(c) << endl;
		return OFFalse;
	    }
	} else if (isalpha(c) && islower(c)) {
	    cerr << "error: lower-case characters encountered: "
		 << char(c) << endl;
	    return OFFalse;
	} else {
	    cerr << "error: invalid character encountered: "
		 << char(c) << endl;
	    return OFFalse;
	}
    }
    return OFTrue;
}

static OFBool
isaValidFileName(const char* fname,
		 char sep = PATH_SEPARATOR)
{
    OFBool ok = OFTrue;
    /*
    ** Is the file name path ok?
    ** The filename is assumed to be in local format for this check.
    */
    if (!areFileNameCharsValid(fname, sep)) {
	cerr << "       invalid characters in filename: " 
	     << fname << endl;
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
	cerr << "error: component " 
	     << " too large (max " << MAX_FNAME_COMPONENT_SIZE 
	     << " characters) in filename: "
	     << fname << endl;
	ok = OFFalse;
    }
    return ok;
}

static OFBool
isaValidFileSetID(const char* fsid)
{
    OFBool ok = OFTrue;
    if (fsid == NULL) {
	return OFFalse;
    }
    /*
    ** Are the characters ok?
    */
    if (!areCSCharsValid(fsid)) {
	cerr << "       invalid characters in FileSetID: " 
	     << fsid << endl;
	ok = OFFalse;
    }
    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (componentCount(fsid) != 1) {
	cerr << "error: too many components in FileSetID: " 
	     << fsid << endl;
	ok = OFFalse;
    }
    /*
    ** Ensure that each component is not too large
    */
    DcmVR cs(EVR_CS);
    if (isComponentTooLarge(fsid, cs.getMaxValueLength())) {
	cerr << "error: too large: " << fsid << endl;
	ok = OFFalse;
    }
    return ok;
}


static OFBool
checkFileCanBeUsed(const char* fname) 
{
    OFBool ok = OFTrue;
    if (verbosemode) {
	cout << "checking: " << fname << endl;
    }

    if (!isaValidFileName(fname)) {
	ok = OFFalse;
    }

    /*
    ** Does the file exist??
    */
    DcmFileStream myin(fname, DCM_ReadMode);
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

    if (checkImage(fname, &ff) == OFFalse) {
	ok = OFFalse;
    }

    return ok;
}

static void
inventMissingImageLevelAttributes(DcmDirectoryRecord *parent)
{
    int imageNumber = 0;
    int overlayNumber = 0;
    int lutNumber = 0;
    int curveNumber = 0;

    int count = parent->cardSub();
    for (int i=0; i<count; i++) {
	DcmDirectoryRecord* rec = parent->getSub(i);
	
	switch (rec->getRecordType()) {
	case ERT_Image:
	    if (!dcmTagExistsWithValue(rec, DCM_ImageNumber)) {
		const char* defNum = defaultNumber(imageNumber++);
		cerr << "warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing ImageNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_ImageNumber, defNum);
	    }
	    break;
	case ERT_Overlay:
	    if (!dcmTagExistsWithValue(rec, DCM_OverlayNumber)) {
		const char* defNum = defaultNumber(overlayNumber++);
		cerr << "warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing OverlayNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_OverlayNumber, defNum);
	    }
	    break;
	case ERT_ModalityLut:
	case ERT_VoiLut:
	    if (!dcmTagExistsWithValue(rec, DCM_LUTNumber)) {
		const char* defNum = defaultNumber(lutNumber++);
		cerr << "warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing LutNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_LUTNumber, defNum);
	    }
	    break;
	case ERT_Curve:
	    if (!dcmTagExistsWithValue(rec, DCM_CurveNumber)) {
		const char* defNum = defaultNumber(curveNumber++);
		cerr << "warning: " <<  recordTypeToName(rec->getRecordType())
		     << "Record (origin: " << rec->getRecordsOriginFile() 
		     << ") inventing CurveNumber: "
		     << defNum << endl;
		dcmInsertString(rec, DCM_CurveNumber, defNum);
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
    int count = parent->cardSub();
    for (int i=0; i<count; i++) {
	DcmDirectoryRecord* rec = parent->getSub(i);
	if (!dcmTagExistsWithValue(rec, DCM_SeriesNumber)) {
	    const char* defNum = defaultNumber(seriesNumber++);
	    cerr << "warning: Series Record (origin: " 
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

    int count = parent->cardSub();
    for (int i=0; i<count; i++) {
	DcmDirectoryRecord* rec = parent->getSub(i);
	if (!dcmTagExistsWithValue(rec, DCM_StudyID)) {
	    const char* defId = defaultID("DCMTKSTUDY", studyNumber++);
	    cerr << "warning: Study Record (origin: " 
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

    int count = root->cardSub();
    for (int i=0; i<count; i++) {
	DcmDirectoryRecord* rec = root->getSub(i);
	if (!dcmTagExistsWithValue(rec, DCM_PatientID)) {
	    const char* defId = defaultID("DCMTKPAT", patientNumber++);
	    cerr << "warning: Patient Record (origin: " 
		 << rec->getRecordsOriginFile() << ") inventing PatientID: "
		 << defId << endl;
	    dcmInsertString(rec, DCM_PatientID, defId);
	}
	inventMissingStudyLevelAttributes(rec);
    }
}

static OFBool
copyFile(const char* fromfname, const char* tofname)
{
    FILE* fromf = NULL;
    FILE* tof = NULL;
    if ((fromf = fopen(fromfname, "rb")) == NULL) {
	cerr << "error: copying files: cannot open: " << fromfname << endl;
	return OFFalse;
    }
    if ((tof = fopen(tofname, "wb")) == NULL) {
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
createDicomdirFromFiles(StrList& fileNames)
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

    if (fsdfid && !isaValidFileName(fsdfid)) {
	cerr << "       bad FileSetDescriptorFileID: " 
	     << fsdfid << endl;
	ok = OFFalse;
    }

    if (fsdfid && !fileExists(fsdfid)) {
	cerr << "error: cannot find FileSetDescriptorFileID: " 
	     << fsdfid << endl;
	ok = OFFalse;
    }

    /*
    ** Check the files before adding
    */

    int badFileCount = 0;
    int goodFileCount = 0;
    StrListIterator iter(fileNames);
    while (iter.hasMore()) {
	const char* fname = iter.next();
	if (checkFileCanBeUsed(fname)) {
	    goodFileCount++;
	} else {
	    badFileCount++;
	}
    }
    if (badFileCount > 0) {
	cerr << "error: " << badFileCount << " bad files (" << goodFileCount
	     << " good files): DICOMDIR not created" << endl;
	ok = OFFalse;
    }

    if (!ok) {
	return OFFalse;
    }

    OFBool backupCreated = OFFalse;
    char* backupName = NULL;

    if (writeDicomdir && fileExists(ofname)) {
	/* rename existing DICOMDIR */
	char* suffix = ".BAK";
	backupName = new char[strlen(ofname) + strlen(suffix) + 1];
	strcpy(backupName, ofname);
	strcat(backupName, suffix);
	unlink(backupName);
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
	unlink(ofname);
    }

    /*
    ** Create the DICOMDIR object
    */

    DcmDicomDir *dicomdir = new DcmDicomDir( ofname, fsid );
    DcmDirectoryRecord& rootRecord = dicomdir->getRootRecord();

    /* 
    ** add optional parts to root record
    */
    if (fsdfid != NULL) {
	dcmInsertString(dicomdir->getDirFileFormat().getDataset(), 
			DCM_FileSetDescriptorFileID, fsdfid);
	dcmInsertString(dicomdir->getDirFileFormat().getDataset(), 
			DCM_FileSetCharacterSet, scsfsdf);
    }

    iter.start(fileNames);
    while (iter.hasMore()) {
	const char* fname = iter.next();
	if (addToDir(&rootRecord, fname) == OFFalse) {
	    return OFFalse;
	}
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
		unlink(backupName);
	    }
	}
    }

    delete dicomdir;

    return OFTrue;
}



#if HAVE__FINDFIRST
static OFBool
expandFileNames(StrList & fileNames, StrList & expandedNames)
{
    OFBool ok = OFTrue;
    struct _finddata_t fileData;
	long hFile;

    StrListIterator iter(fileNames);
    while (iter.hasMore()) {
	const char* fname = iter.next();
	if (hFile = _findfirst(fname, &fileData) == -1L) {
	    cerr << "error: cannot access: " << fname << endl;
		_findclose(hFile);
	    ok = OFFalse;
	} else if (fileData.attrib & _A_SUBDIR) {
	    /* it is a directory */
	    StrList subList;
		char * newSearchname = new char[strlen(fname)+3];
		strcpy(newSearchname, fname);
		strcat(newSearchname, "\\*");
	    _findclose(hFile);
		int ret = 0;
	    for (hFile = _findfirst(newSearchname, &fileData); 
			 hFile != -1L && ret == 0; ret = _findnext(hFile, &fileData)) {
		/* filter out current and parent directory */
		if (!cmp(fileData.name, ".") && !cmp(fileData.name, "..")) {
		    char* subname = 
			new char[strlen(fname)+strlen(fileData.name)+2];
		    sprintf(subname, "%s%c%s", fname, PATH_SEPARATOR, 
			    fileData.name);
		    subList.append(subname);
		    delete subname;
		}
	    }
		delete newSearchname;
		_findclose(hFile);
		/* recurse */
	    expandFileNames(subList, expandedNames);
	} else {
	    /* add to expanded list  */
		_findclose(hFile);
	    expandedNames.append(fname);
	}
    }
    return ok;
}
#else

static OFBool
expandFileNames(StrList& fileNames, StrList& expandedNames)
{
    OFBool ok = OFTrue;
    DIR *dirp = NULL;

    StrListIterator iter(fileNames);
    while (iter.hasMore()) {
	const char* fname = iter.next();
	if (!fileExists(fname)) {
	    cerr << "error: cannot access: " << fname << endl;
	    ok = OFFalse;
	} else if ((dirp = opendir(fname)) != NULL) {
	    /* it is a directory */
	    StrList subList;
	    struct dirent *dp = NULL;
	    for (dp=readdir(dirp); dp!=NULL; dp=readdir(dirp)) {
		/* filter out current and parent directory */
		if (!cmp(dp->d_name, ".") && !cmp(dp->d_name, "..")) {
		    char* subname = 
			new char[strlen(fname)+strlen(dp->d_name)+2];
		    sprintf(subname, "%s%c%s", fname, PATH_SEPARATOR, 
			    dp->d_name);
		    subList.append(subname);
		    delete subname;
		}
	    }
	    closedir(dirp);
	    /* recurse */
	    expandFileNames(subList, expandedNames);
	} else {
	    /* add to expanded list  */
	    expandedNames.append(fname);
	}
    }
    return ok;
}

#endif

/*
** CVS/RCS Log:
** $Log: dcmgpdir.cc,v $
** Revision 1.16  1997-08-06 12:20:02  andreas
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

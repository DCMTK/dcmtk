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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-05-06 09:15:57 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmgpdir.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
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
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

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

BOOL verbosemode = FALSE;
BOOL writeDicomdir = TRUE;
BOOL appendMode = FALSE;
BOOL inventAttributes = FALSE;
BOOL mapFilenames = FALSE;
BOOL recurseFilesystem = FALSE;

E_EncodingType lengthEncoding = EET_ExplicitLength;
E_GrpLenEncoding groupLengthEncoding = EGL_withoutGL;

static int studyNumber = 0;
static int seriesNumber = 0;
static int imageNumber = 0;
static int overlayNumber = 0;
static int lutNumber = 0;
static int curveNumber = 0;


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
    BOOL hasMore() {
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

static BOOL
expandFileNames(StrList& fileNames, StrList& expandedNames);

static BOOL
createDicomdirFromFiles(StrList& fileNames);

// ********************************************

static void
usage()
{
    cerr << 
"dcmgpdir: create a general purpose dicomdir\n"
"usage: dcmgpdir [options] referenced-dicom-file ...\n"
"options are:\n"
"  output option\n"
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

static BOOL
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

    BOOL found = FALSE;
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

    SetDebugLevel(( 0 ));

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
		    inventAttributes = TRUE;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'm':
		if (arg[0] == '+' && arg[2] == '\0') {
		    mapFilenames = TRUE;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'w':
		if (arg[0] == '-' && arg[2] == '\0') {
		    writeDicomdir = FALSE;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'A':
		if (arg[0] == '+' && arg[2] == '\0') {
		    appendMode = FALSE;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'r':
		if (arg[0] == '+' && arg[2] == '\0') {
		    recurseFilesystem = TRUE;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'u':
		if (arg[0] == '-' && arg[2] == '\0') {
		    dcmEnableUnknownVRGeneration = FALSE;
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
		    verbosemode = TRUE;
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
    if (dcmDataDict.numberOfEntries() == 0) {
	cerr << "Warning: no data dictionary loaded, " 
	     << "check environment variable: " 
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
	return 1; /* DcmDicomDir class dumps core when no data dictionary */
    }

    SetDebugLevel(( localDebugLevel ));

    BOOL ok = TRUE;
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

static BOOL
dcmTagExists(DcmItem* d, const DcmTagKey& xtag, BOOL searchIntoSub = FALSE)
{
    DcmStack stack;
    E_Condition ec = EC_Normal;
    
    ec = d->search(xtag, stack, ESM_fromHere, searchIntoSub);

    return (ec == EC_Normal);
}

static BOOL
dcmTagExistsWithValue(DcmItem* d, const DcmTagKey& xtag, 
		      BOOL searchIntoSub = FALSE)
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
dcmFindString(DcmItem* d, const DcmTagKey& xtag, BOOL searchIntoSub = FALSE)
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
		    BOOL searchIntoSub = FALSE)
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
	return FALSE;
    }
    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_withGL );
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
	cerr << "error: " 
	     << dcmErrorConditionToString(ff.error())
	     << ": reading file: " << fname << endl;
	return FALSE;
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

static BOOL 
dcmInsertString(DcmItem* d, const DcmTagKey& xtag, 
		const char* s, BOOL replaceOld = TRUE)
{
    if (d == NULL) {
	cerr << "error: dcmInsertString: null DcmItem argument" << endl;
	return FALSE;
    }

    DcmTag tag(xtag);
    DcmElement *elem = newDicomElement(tag);
    E_Condition cond = EC_Normal;

    if (elem == NULL) {
	cerr << "error: dcmInsertString: cannot create DcmElement" << endl;
	return FALSE;
    }
    if (s != NULL) {
	cond = elem->putString(s);
	if (cond != EC_Normal) {
	    cerr << "error: dcmInsertString: cannot put string" << endl;
	    return FALSE;
	}
    }
    cond = d->insert(elem, replaceOld);
    if (cond != EC_Normal) {
	cerr << "error: dcmInsertString: cannot insert element" << endl;
	return FALSE;
    }

    return (cond == EC_Normal);
}

static BOOL 
dcmCopyString(DcmItem* sink, const DcmTagKey& xtag, DcmItem* source)
{
    return dcmInsertString(sink, xtag, dcmFindString(source, xtag));
}

static BOOL 
dcmCopyOptString(DcmItem* sink, const DcmTagKey& xtag, DcmItem* source)
{
    BOOL ok = TRUE;
    if (dcmTagExists(source, xtag)) {
	ok = dcmInsertString(sink, xtag, dcmFindString(source, xtag));
    }
    return ok;
}

static BOOL 
dcmCopyStringWithDefault(DcmItem* sink, const DcmTagKey& xtag, 
			 DcmItem* source, const char* defaultString)
{
    BOOL ok = TRUE;
    if (dcmTagExists(source, xtag)) {
	ok = dcmInsertString(sink, xtag, dcmFindString(source, xtag));
    } else {
	ok = dcmInsertString(sink, xtag, defaultString);
    }
    return ok;
}

static BOOL 
dcmCopyOptSequence(DcmItem* sink, const DcmTagKey& xtag, DcmItem* source)
{
    BOOL ok = TRUE;
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
    
	ec = source->search(xtag, stack, ESM_fromHere, FALSE);

	sq = (DcmSequenceOfItems*) stack.top();
	if (ec == EC_Normal && sq != NULL) {
	    // make a copy of sq
	    DcmSequenceOfItems *sqcopy = new DcmSequenceOfItems(*sq); 

	    // insert sqcopy into the sink
	    ec = sink->insert(sqcopy, TRUE);
	    if (ec != EC_Normal) {
		cerr << "error: dcmInsertSequence: cannot insert element" 
		     << endl;
		ok = FALSE;
	    }
	} else {
	    ok = FALSE;
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
    const char* time = dcmFindString(d, DCM_SeriesDate);
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
    sprintf(idbuf, "%s%06lu", prefix, ul);
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
dicomToHostFilename(char* fname, BOOL mapToLower = FALSE)
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

static BOOL 
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
	    return TRUE;
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
	return TRUE;
    }

    return FALSE;
}

/*
** Check help functions
*/

static BOOL
fileExists(const char* fname) 
{
    FILE* f = fopen(fname, "r");
    if (f == NULL) {
	return FALSE;
    }
    fclose(f);
    return TRUE;
}


static BOOL
cmp(const char* s1, const char* s2)
{
    if (s1 == NULL || s2 == NULL) {
	return FALSE;
    }
    return (strcmp(s1,s2) == 0)?(TRUE):(FALSE);
}

static BOOL
checkExists(DcmItem* d, const DcmTagKey& key, const char* fname)
{
    if (!dcmTagExists(d, key)) {
	DcmTag tag(key);
	cerr << "error: required attribute " << tag.getTagName() 
	     << " " << key << " missing in file: "
	     << fname << endl;
	return FALSE;
    }
    return TRUE;
}

static BOOL
checkExistsWithValue(DcmItem* d, const DcmTagKey& key, const char* fname)
{
    if (!checkExists(d, key, fname)) {
	return FALSE;
    }
    const char* s = NULL;
    if ((s = dcmFindString(d, key)) == NULL) {
	DcmTag tag(key);
	cerr << "error: required attribute " << tag.getTagName() 
	     << " " << key << " has no value in file: "
	     << fname << endl;
	return FALSE;
    }
    return TRUE;
}

/*
** Sanity Checks
*/

static BOOL 
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

    if (ff == NULL) return FALSE;

    BOOL ok = TRUE;

    DcmMetaInfo *m = ff->getMetaInfo();
    if (m == NULL || m->card() == 0) {
	cerr << "error: file not part 10 format (no meta-header): "
	     << fname << endl;
	ok = FALSE;
    }

    DcmDataset *d = ff->getDataset();
    if (d == NULL) {
	cerr << "error: file contains no data (no dataset): "
	     << fname << endl;
	/* give up checking */
	return FALSE;
    }

    /* 
    ** Is sop classs ok?
    */
    const char* mediaSOPClassUID = 
	dcmFindString(m, DCM_MediaStorageSOPClassUID);
    if (mediaSOPClassUID == NULL) {
	cerr << "error: MediaStorageSOPClassUID missing in meta-header: "
	     << fname << endl;
	ok = FALSE;
    }
    /* 
    ** Check if the SOP Class is a known storage SOP class (an image, overlay,
    ** curve, etc.
    */
    BOOL found = FALSE;

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
	ok = FALSE;
    }

    /* 
    ** Is transfer syntax ok?
    */
    const char* transferSyntax = dcmFindString(m, DCM_TransferSyntaxUID);
    if (transferSyntax == NULL) {
	cerr << "error: TransferSyntaxUID missing in meta-header: "
	     << fname << endl;
	ok = FALSE;
    }
    found = cmp(transferSyntax, UID_LittleEndianExplicitTransferSyntax);
    if (!found) {
	cerr << "error: LittleEndianExplicitTransferSyntax expected: "
	     << fname << endl;
	ok = FALSE;
    }

    /*
    ** are mandatory attributes for DICOMDIR available and valued?
    */

    /* PatientID is type 1 in DICOMDIR and type 2 in images.
    ** However, PatientID is a key attribute and we cannot invent it
    */
    if (!checkExistsWithValue(d, DCM_PatientID, fname)) ok = FALSE;
    /* PatientName is type 2 in DICOMDIR and images */
    if (!checkExists(d, DCM_PatientName, fname)) ok = FALSE;
    /* StudyDate is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
	if (!checkExistsWithValue(d, DCM_StudyDate, fname)) ok = FALSE;
    }
    /* StudyTime is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
	if (!checkExistsWithValue(d, DCM_StudyTime, fname)) ok = FALSE;
    }
    /* StudyDescription is type 2 in DICOMDIR and type 3 im images.
    ** We can create an empty attribute in the directory
    ** if (!checkExists(d, DCM_StudyDescription, fname)) ok = FALSE;
    */
    /* StudyInstanceIOD is type 1 in DICOMDIR and images */
    if (!checkExistsWithValue(d, DCM_StudyInstanceUID, fname)) ok = FALSE;
    /* StudyID is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
	if (!checkExistsWithValue(d, DCM_StudyID, fname)) ok = FALSE;
    }
    /* AccessionNumber is type 2 in DICOMDIR and type 3 in images
    ** We can create an empty attribute in the directory
    ** if (!checkExists(d, DCM_AccessionNumber, fname)) ok = FALSE;
    */
    /* Modality is type 1 in DICOMDIR and type 1 in images */
    if (!checkExistsWithValue(d, DCM_Modality, fname)) ok = FALSE;
    /* SeriesInstanceUID is type 1 in DICOMDIR and type 1 in images */
    if (!checkExistsWithValue(d, DCM_SeriesInstanceUID, fname)) ok = FALSE;
    /* SeriesNumber is type 1 in DICOMDIR and type 2 in images */
    if (!inventAttributes) {
	if (!checkExistsWithValue(d, DCM_SeriesNumber, fname)) ok = FALSE;
    }
    /* Image etc Numbers are type 1 in DICOMDIR but type 2 in images */
    if (cmp(mediaSOPClassUID, UID_StandaloneOverlayStorage)) {
	/* an overlay */
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_OverlayNumber, fname)) 
		ok = FALSE;
	}
    } else if (cmp(mediaSOPClassUID, UID_StandaloneModalityLUTStorage)) {
	/* a modality lut */
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_LUTNumber, fname)) 
		ok = FALSE;
	}
    } else if (cmp(mediaSOPClassUID, UID_StandaloneVOILUTStorage)) {
	/* a voi lut */
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_LUTNumber, fname)) 
		ok = FALSE;
	}
    } else if (cmp(mediaSOPClassUID, UID_StandaloneCurveStorage)) {
	/* a curve */
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_CurveNumber, fname)) 
		ok = FALSE;
	}
    } else {
	/* it can only be an image */ 
	if (!inventAttributes) {
	    if (!checkExistsWithValue(d, DCM_ImageNumber, fname)) 
		ok = FALSE;
	}
    }

    return ok;
}

static
DcmDirectoryRecord* buildPatientRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Patient, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating patient record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_PatientID, d);
    dcmCopyString(rec, DCM_PatientName, d);

    return rec;
}

DcmDirectoryRecord* 
buildStudyRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Study, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating study record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyStringWithDefault(rec, DCM_StudyDate, d, alternativeStudyDate(d));
    dcmCopyStringWithDefault(rec, DCM_StudyTime, d, alternativeStudyTime(d));
    dcmCopyString(rec, DCM_StudyDescription, d);
    dcmCopyString(rec, DCM_StudyInstanceUID, d);
    dcmCopyStringWithDefault(rec, DCM_StudyID, d, 
			     defaultID("STUDY", studyNumber++));
    dcmCopyString(rec, DCM_AccessionNumber, d);

    return rec;
}

DcmDirectoryRecord* 
buildSeriesRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Series, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating series record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_Modality, d);
    dcmCopyString(rec, DCM_SeriesInstanceUID, d);
    dcmCopyStringWithDefault(rec, DCM_SeriesNumber, d, 
			     defaultNumber(seriesNumber++));

    return rec;
}

DcmDirectoryRecord* 
buildImageRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Image, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating image record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyStringWithDefault(rec, DCM_ImageNumber, d,
			     defaultNumber(imageNumber++));
    /* addition type 1C keys specified by STD-GEN-CD profile */
    dcmCopyOptString(rec, DCM_ImageType, d);
    dcmCopyOptSequence(rec, DCM_ReferencedImageSequence, d);

    return rec;
}

DcmDirectoryRecord* 
buildOverlayRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Overlay, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating overlay record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyStringWithDefault(rec, DCM_OverlayNumber, d,
			     defaultNumber(overlayNumber++));

    return rec;
}

DcmDirectoryRecord* 
buildModalityLutRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_ModalityLut, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating modality lut record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyStringWithDefault(rec, DCM_LUTNumber, d,
			     defaultNumber(lutNumber++));

    return rec;
}

DcmDirectoryRecord* 
buildVoiLutRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_VoiLut, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating voi lut record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyStringWithDefault(rec, DCM_LUTNumber, d,
			     defaultNumber(lutNumber++));

    return rec;
}

DcmDirectoryRecord* 
buildCurveRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Curve, fname);
    if (rec == NULL) {
	cerr << "error: out of memory (creating curve record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyStringWithDefault(rec, DCM_CurveNumber, d,
			     defaultNumber(curveNumber));


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
    dicomToHostFilename(fn, TRUE);
    if (fileExists(fn)) {
	return fn;
    }
    /* lowercase with trailing period */
    strcpy(fn, fname);
    strcat(fn, ".");
    dicomToHostFilename(fn, TRUE);
    if (fileExists(fn)) {
	return fn;
    }

    return NULL;
}

static BOOL
recordMatchesDataset(DcmDirectoryRecord *rec, DcmItem* dataset)
{
    BOOL match = FALSE;

    switch (rec->getRecordType()) {
    case ERT_Patient:
	match = cmp(dcmFindString(rec, DCM_PatientID),
		    dcmFindString(dataset, DCM_PatientID));
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
	return FALSE;
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
    BOOL found = FALSE;
    for (unsigned long i=0; i<n && !found; i++) {
	r = root->getSub(i);
	if (dirtype == r->getRecordType()) {
	    found = recordMatchesDataset(r, dataset);
	}
    }
    return (found)?(r):((DcmDirectoryRecord*)NULL);
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

static DcmDirectoryRecord*
buildRecord(E_DirRecType dirtype, const char* fname, DcmItem* dataset)
{
    DcmDirectoryRecord *rec = NULL;

    switch (dirtype) {
    case ERT_Patient:
	rec = buildPatientRecord(fname, dataset);
	break;
    case ERT_Study:
	rec = buildStudyRecord(fname, dataset);
	break;
    case ERT_Series:
	rec = buildSeriesRecord(fname, dataset);
	break;
    case ERT_Image:
	rec = buildImageRecord(fname, dataset);
	break;
    case ERT_Overlay:
	rec = buildOverlayRecord(fname, dataset);
	break;
    case ERT_Curve:
	rec = buildCurveRecord(fname, dataset);
	break;
    case ERT_ModalityLut:
	rec = buildModalityLutRecord(fname, dataset);
    case ERT_VoiLut:
	rec = buildVoiLutRecord(fname, dataset);
	break;
    default:
	cerr << "error: record type not yet implemented" << endl;
	return FALSE;
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

static BOOL
compareStringAttributes(DcmTagKey& tag, DcmDirectoryRecord *rec, 
			DcmItem* dataset, const char* fname)
{
    const char* s1 = dcmFindString(rec, tag);
    const char* s2 = dcmFindString(dataset, tag);
    BOOL equals = cmp(s1, s2);

    if (!equals) {
	cerr << "Warning:  DICOMDIR inconsistant with file: "
	     << fname << endl;
	cerr << "  " << recordTypeToName(rec->getRecordType()) 
	     << " Record defines: " << endl;
	cerr << "    "; 
	printAttribute(cerr, tag, s1); cerr << endl;
	cerr << "  " << fname << " defines:" << endl;
	cerr << "    "; 
	printAttribute(cerr, tag, s2); cerr << endl;
    }

    return equals;
}

static void
warnAboutInconsistantAttributes(DcmDirectoryRecord *rec, 
				DcmItem* dataset, const char* fname)
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
		    compareStringAttributes(tag, rec, dataset, fname);
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
    BOOL found = FALSE;
    if (count > 0) {
	for (int i=0; i<count && !found; i++) {
	    DcmDirectoryRecord* rec = parent->getSub(i);
	    int curNumber = getISNumber(rec, criterionKey);
	    if (curNumber > number) {
		found = TRUE;
	    }
	    pos = i;
	}
    }
    if (pos < (count-1)) {
	cond = parent->insertSub(child, pos, TRUE);
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
	      DcmItem* dataset, const char* fname)
{
    DcmDirectoryRecord *rec = 
	findExistingRecord(parentRec, dirtype, dataset);
    if (rec == NULL) {
	rec = buildRecord(dirtype, fname, dataset);
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

static BOOL 
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
	return FALSE;
    }
    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_withGL );
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
	cerr << "error: " << dcmErrorConditionToString(ff.error())
	     << ": reading file: " << fname << endl;
	return FALSE;
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
	includeRecord(rootRec, ERT_Patient, dataset, NULL);
    if (patientRec == NULL) {
	return FALSE;
    }
    /* if PatientMgmgt file then attach it to Patient Record */
    if (cmp(sopClass, UID_DetachedPatientManagementMetaSOPClass)) {
	cond = patientRec->assignToSOPFile(fname);
	if (cond != EC_Normal) {
	    cerr << "error: " << dcmErrorConditionToString(cond) 
		 << ": cannot assign patient record to file: " 
		 << fname << endl;
	    return FALSE;
	}
    }
    warnAboutInconsistantAttributes(patientRec, dataset, ifname);

    /*
    ** Add a study record underneath the actual patient record
    */
    DcmDirectoryRecord *studyRec = 
	includeRecord(patientRec, ERT_Study, dataset, NULL);
    if (studyRec == NULL) {
	return FALSE;
    }
    warnAboutInconsistantAttributes(studyRec, dataset, ifname);

    /*
    ** Add a series record underneath the actual study
    */
    DcmDirectoryRecord *seriesRec = 
	includeRecord(studyRec, ERT_Series, dataset, NULL);
    if (seriesRec == NULL) {
	return FALSE;
    }
    warnAboutInconsistantAttributes(seriesRec, dataset, ifname);

    /*
    ** Add one of the image-like records underneath the actual series record
    */
    DcmDirectoryRecord *rec = NULL;

    if (cmp(sopClass, UID_StandaloneOverlayStorage)) {
	/* Add an overlay record */
	rec = includeRecord(seriesRec, ERT_Overlay, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else if (cmp(sopClass, UID_StandaloneModalityLUTStorage)) {
	/* Add a modality lut record */
	rec = includeRecord(seriesRec, ERT_ModalityLut, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else if (cmp(sopClass, UID_StandaloneVOILUTStorage)) {
	/* Add a voi lut record */
	rec = includeRecord(seriesRec, ERT_VoiLut, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else if (cmp(sopClass, UID_StandaloneCurveStorage)) {
	/* Add a curve record */
	rec = includeRecord(seriesRec, ERT_Curve, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else {
	/* it can only be an image */ 
	/* Add an image record */
	rec = includeRecord(seriesRec, ERT_Image, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    }
    warnAboutInconsistantAttributes(rec, dataset, ifname);

    return TRUE;
}

static BOOL 
areCSCharsValid(const char* fname)
{
    int len = strlen(fname);
    for (int i=0; i<len; i++) {
	char c = fname[i];
	if ((c != ' ') && (c != '_') && !isdigit(c) && 
	    !(isalpha(c) && isupper(c))) {
	    cerr << "error: invalid character encountered: "
		 << char(c) << endl;
	    return FALSE;
	}
    }
    return TRUE;
}

static BOOL 
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
		return FALSE;
	    }
	} else if (isalpha(c) && islower(c)) {
	    cerr << "error: lower-case characters encountered: "
		 << char(c) << endl;
	    return FALSE;
	} else {
	    cerr << "error: invalid character encountered: "
		 << char(c) << endl;
	    return FALSE;
	}
    }
    return TRUE;
}

static BOOL
isaValidFileName(const char* fname,
		 char sep = PATH_SEPARATOR)
{
    BOOL ok = TRUE;
    /*
    ** Is the file name path ok?
    ** The filename is assumed to be in local format for this check.
    */
    if (!areFileNameCharsValid(fname, sep)) {
	cerr << "       invalid characters in filename: " 
	     << fname << endl;
	ok = FALSE;
    }
    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (componentCount(fname, sep) > MAX_FNAME_COMPONENTS) {
	cerr << "error: too many path components (max " 
	     << MAX_FNAME_COMPONENTS << ") in filename: "
	     << fname << endl;
	ok = FALSE;
    }
    /*
    ** Ensure that each component is not too large
    */
    if (isComponentTooLarge(fname, MAX_FNAME_COMPONENT_SIZE, sep)) {
	cerr << "error: component " 
	     << " too large (max " << MAX_FNAME_COMPONENT_SIZE 
	     << " characters) in filename: "
	     << fname << endl;
	ok = FALSE;
    }
    return ok;
}

static BOOL
isaValidFileSetID(const char* fsid)
{
    BOOL ok = TRUE;
    if (fsid == NULL) {
	return FALSE;
    }
    /*
    ** Are the characters ok?
    */
    if (!areCSCharsValid(fsid)) {
	cerr << "       invalid characters in FileSetID: " 
	     << fsid << endl;
	ok = FALSE;
    }
    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (componentCount(fsid) != 1) {
	cerr << "error: too many components in FileSetID: " 
	     << fsid << endl;
	ok = FALSE;
    }
    /*
    ** Ensure that each component is not too large
    */
    DcmVR cs(EVR_CS);
    if (isComponentTooLarge(fsid, cs.getMaxValueLength())) {
	cerr << "error: too large: " << fsid << endl;
	ok = FALSE;
    }
    return ok;
}


static BOOL
checkFileCanBeUsed(const char* fname) 
{
    BOOL ok = TRUE;
    if (verbosemode) {
	cout << "checking: " << fname << endl;
    }

    if (!isaValidFileName(fname)) {
	ok = FALSE;
    }

    /*
    ** Does the file exist??
    */
    DcmFileStream myin(fname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
	cerr << "error: cannot open file: " << fname << endl;
	/* cannot continue checking */
	return FALSE;
    }

    DcmFileFormat ff;

    ff.transferInit();
    ff.read(myin, EXS_Unknown, EGL_withGL );
    ff.transferEnd();

    if (ff.error() != EC_Normal) {
	cerr << "error: " 
	     << dcmErrorConditionToString(ff.error())
	     << ": reading file: " << fname << endl;
	/* cannot continue checking */
	return FALSE;
    }

    if (checkImage(fname, &ff) == FALSE) {
	ok = FALSE;
    }

    return ok;
}

static BOOL
createDicomdirFromFiles(StrList& fileNames)
{
    BOOL ok = TRUE;
    /*
    ** Check the FileSetID and the FileSetDescriptorFileID
    */
    if (!isaValidFileSetID(fsid)) {
	cerr << "error: invalid FileSetID: " 
	     << fsdfid << endl;
	ok = FALSE;
    }

    if (fsdfid && !isaValidFileName(fsdfid)) {
	cerr << "       bad FileSetDescriptorFileID: " 
	     << fsdfid << endl;
	ok = FALSE;
    }

    if (fsdfid && !fileExists(fsdfid)) {
	cerr << "error: cannot find FileSetDescriptorFileID: " 
	     << fsdfid << endl;
	ok = FALSE;
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
	ok = FALSE;
    }

    if (!ok) {
	return FALSE;
    }

    if (!appendMode && writeDicomdir) {
	/* delete the existing DICOMDIR file */
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
	if (addToDir(&rootRecord, fname) == FALSE) {
	    return FALSE;
	}
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
	}
    }

    delete dicomdir;

    return TRUE;
}



static BOOL
expandFileNames(StrList& fileNames, StrList& expandedNames)
{
    BOOL ok = TRUE;
    DIR *dirp = NULL;

    StrListIterator iter(fileNames);
    while (iter.hasMore()) {
	const char* fname = iter.next();
	if (!fileExists(fname)) {
	    cerr << "error: cannot access: " << fname << endl;
	    ok = FALSE;
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



/*
** CVS/RCS Log:
** $Log: dcmgpdir.cc,v $
** Revision 1.6  1997-05-06 09:15:57  hewett
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

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
** Update Date:		$Date: 1997-04-18 08:06:56 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmgpdir.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

/* default File-Set ID */
#define DEFAULT_FSID "DCMTK_EXAMPLE"
/* default File-Set Descriptor File ID */
#define DEFAULT_FSDFID "README"
/* default Specific Character Set of File-Set Descriptor File */
#define DEFAULT_SCSFSDF "ISO_IR 100"

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

#define MAX_CMDLINE_FILENAMES 2048
char* cmdLineFileNames[MAX_CMDLINE_FILENAMES];
int cmdLineFileNamesCount = 0;

static BOOL
createDicomdirFromFiles(char* fileNames[], int numberOfFileNames);


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
"    -w                 do not write out dicomdir (default: do write)\n"
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

    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		fprintf(stderr, "unknown argument: %s\n", arg);
		usage();
		return 1;
	    }
	    switch (arg[1]) {
	    case 'D':
		if (arg[0] == '+' && arg[2] != '\0') {
		    ofname = arg + 2;
		} else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'F':
		if (arg[0] == '+' && arg[2] != '\0') {
		    fsid = arg + 2;
		} else {
		    fprintf(stderr, "unknown option: %s\n", arg);
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
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'C':
		if (arg[0] == '+' && arg[2] != '\0') {
		    scsfsdf = arg + 2;
		    if (!isaValidCharSetName(scsfsdf)) {
			fprintf(stderr, "unknown char-set: %s\n", scsfsdf);
		    }
		} else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'w':
		if (arg[0] == '-' && arg[2] == '\0') {
		    writeDicomdir = FALSE;
		} else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'u':
		if (arg[0] == '-' && arg[2] == '\0') {
		    dcmEnableUnknownVRGeneration = FALSE;
		} else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'V':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verbosemode = TRUE;
		} else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'd':
		if (sscanf(arg+2, "%d", &localDebugLevel) != 1) {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    default:
		fprintf(stderr, "unknown option: %s\n", arg);
		return 1;
	    }
	} else if ( cmdLineFileNamesCount < MAX_CMDLINE_FILENAMES ) {
	    cmdLineFileNames[cmdLineFileNamesCount++] = arg;
	} else {
	    fprintf(stderr, "too many arguments: %s\n", arg);
	    usage();
	    return 1;
	}
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	cerr << "Warning: no data dictionary loaded, check environment variable: " 
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
	return 1; /* DcmDicomDir class dumps core when no data dictionary */
    }

    SetDebugLevel(( localDebugLevel ));

    BOOL ok;
    ok = createDicomdirFromFiles(cmdLineFileNames, cmdLineFileNamesCount);

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

    return (ec == EC_Normal)?(TRUE):(FALSE);
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
	cerr << progname << ": cannot open file: " << fname << endl;
	return FALSE;
    }
    DcmFileFormat *ff = new DcmFileFormat();

    ff->transferInit();
    ff->read(myin, EXS_Unknown, EGL_withGL );
    ff->transferEnd();

    if (ff->error() != EC_Normal) {
	cerr << progname << ": error: " 
	     << dcmErrorConditionToString(ff->error())
	     << ": reading file: " << fname << endl;
	return FALSE;
    }

    DcmDataset *d = ff->getDataset();

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

    delete ff;

    return staticstr;
}


static BOOL 
dcmInsertString(DcmItem* d, const DcmTagKey& xtag, 
		const char* s, BOOL replaceOld = TRUE)
{
    if (d == NULL) {
	cerr << progname << 
	    ": error: dcmInsertString: null DcmItem argument" << endl;
	return FALSE;
    }

    DcmTag tag(xtag);
    DcmElement *elem = newDicomElement(tag);
    E_Condition cond = EC_Normal;

    if (elem == NULL) {
	cerr << progname << 
	    ": error: dcmInsertString: cannot create DcmElement" << endl;
	return FALSE;
    }
    if (s != NULL) {
	cond = elem->putString(s);
	if (cond != EC_Normal) {
	    cerr << progname << 
		": error: dcmInsertString: cannot put string" << endl;
	    return FALSE;
	}
    }
    cond = d->insert(elem, replaceOld);
    if (cond != EC_Normal) {
	cerr << progname << 
	    ": error: dcmInsertString: cannot insert element" << endl;
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
dcmCopyOptSequence(DcmItem* sink, const DcmTagKey& xtag, DcmItem* source)
{
    BOOL ok = TRUE;
    DcmTag tag(xtag);

    if (tag.getEVR() != EVR_SQ) {
	cerr << progname << ": internal error: dcmCopyOptSequence: " 
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
		cerr << progname << 
		    ": error: dcmInsertSequence: cannot insert element" 
		     << endl;
		ok = FALSE;
	    }
	} else {
	    ok = FALSE;
	}
    }
    return ok;
}

/*
** Filename manipulation
*/

void hostToDicomFilename(char* fname)
{
    /*
    ** Massage filename into dicom format 
    ** (dos conventions for path separators)
    */
    int len = strlen(fname);
    for (int i=0; i<len; i++) {
	/* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
	if (fname[i] == PATH_SEPARATOR) {
	    fname[i] = '\\';
	}
    }
}

void dicomToHostFilename(char* fname)
{
    /*
    ** Massage filename into machine format 
    ** (replace dos conventions for path separators)
    */
    int len = strlen(fname);
    for (int i=0; i<len; i++) {
	/* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
	if (fname[i] == '\\') {
	    fname[i] = PATH_SEPARATOR;
	}
    }
}

int dicomFilenameComponentCount(const char* fname)
{
    if (fname == NULL) return 0;
    int n = 1;
    int len = strlen(fname);
    for (int i=0; i<len; i++) {
	if (fname[i] == '\\') {
	    n++;
	}
    }
    return n;
}

/*
** Check help functions
*/

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
	cerr << progname << 
	    ": error: required attribute " << tag.getTagName() 
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
	cerr << progname << 
	    ": error: required attribute " << tag.getTagName() 
	     << " " << key << " has no value in file: "
	     << fname << endl;
	return FALSE;
    }
    return TRUE;
}

/*
** Sanity Checks
*/

static BOOL checkImage(const char* fname, DcmFileFormat *ff)
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

    DcmMetaInfo *m = ff->getMetaInfo();
    if (m == NULL || m->card() == 0) {
	cerr << progname << 
	    ": error: file not part 10 format (no meta-header): "
	     << fname << endl;
	return FALSE;
    }

    DcmDataset *d = ff->getDataset();
    if (d == NULL) {
	cerr << progname << 
	    ": error: file contains no data (no dataset): "
	     << fname << endl;
	return FALSE;
    }

    BOOL found = FALSE;

    /* 
    ** Is sop classs ok?
    */
    const char* mediaSOPClassUID = 
	dcmFindString(m, DCM_MediaStorageSOPClassUID);
    if (mediaSOPClassUID == NULL) {
	cerr << progname << 
	    ": error: MediaStorageSOPClassUID missing in meta-header: "
	     << fname << endl;
	return FALSE;
    }
    /* 
    ** Check if the SOP Class is a known storage SOP class (an image, overlay,
    ** curve, etc.
    */
    found = FALSE;
    for (int i=0; i<numberOfDcmStorageSOPClassUIDs && !found; i++) {
	found = cmp(mediaSOPClassUID, dcmStorageSOPClassUIDs[i]);
    }
    /* a detached patient mgmt sop class is also ok */
    if (!found) {
	found = cmp(mediaSOPClassUID, UID_DetachedPatientManagementSOPClass);
    }
    if (!found) {
	cerr << progname << 
	    ": error: invalid sop class for STD-GEN-CD profile: "
	     << fname << endl;
	return FALSE;
    }

    /* 
    ** Is transfer syntax ok?
    */
    const char* transferSyntax = dcmFindString(m, DCM_TransferSyntaxUID);
    if (transferSyntax == NULL) {
	cerr << progname << 
	    ": error: TransferSyntaxUID missing in meta-header: "
	     << fname << endl;
	return FALSE;
    }
    found = cmp(transferSyntax, UID_LittleEndianExplicitTransferSyntax);
    if (!found) {
	cerr << progname << 
	    ": error: LittleEndianExplicitTransferSyntax expected: "
	     << fname << endl;
	return FALSE;
    }

    /*
    ** are mandatory attributes for DICOMDIR available and valued?
    */
    if (!checkExistsWithValue(d, DCM_PatientID, fname)) return FALSE;
    if (!checkExists(d, DCM_PatientName, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_StudyDate, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_StudyTime, fname)) return FALSE;
    if (!checkExists(d, DCM_StudyDescription, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_StudyInstanceUID, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_StudyID, fname)) return FALSE;
    if (!checkExists(d, DCM_AccessionNumber, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_Modality, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_SeriesInstanceUID, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_SeriesNumber, fname)) return FALSE;
    if (!checkExistsWithValue(d, DCM_PatientID, fname)) return FALSE;

    if (cmp(mediaSOPClassUID, UID_StandaloneOverlayStorage)) {
	/* an overlay */
	if (!checkExistsWithValue(d, DCM_OverlayNumber, fname)) return FALSE;
    } else if (cmp(mediaSOPClassUID, UID_StandaloneModalityLUTStorage)) {
	/* a modality lut */
	if (!checkExistsWithValue(d, DCM_LUTNumber, fname)) return FALSE;
    } else if (cmp(mediaSOPClassUID, UID_StandaloneVOILUTStorage)) {
	/* a voi lut */
	if (!checkExistsWithValue(d, DCM_LUTNumber, fname)) return FALSE;
    } else if (cmp(mediaSOPClassUID, UID_StandaloneCurveStorage)) {
	/* a curve */
	if (!checkExistsWithValue(d, DCM_CurveNumber, fname)) return FALSE;
    } else {
	/* it can only be an image */ 
	if (!checkExistsWithValue(d, DCM_ImageNumber, fname)) return FALSE;
    }

    return TRUE;
}

static
DcmDirectoryRecord* buildPatientRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Patient, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating patient record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_PatientID, d);
    dcmCopyString(rec, DCM_PatientName, d);

    return rec;
}

DcmDirectoryRecord* buildStudyRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Study, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating study record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_StudyDate, d);
    dcmCopyString(rec, DCM_StudyTime, d);
    dcmCopyString(rec, DCM_StudyDescription, d);
    dcmCopyString(rec, DCM_StudyInstanceUID, d);
    dcmCopyString(rec, DCM_StudyID, d);
    dcmCopyString(rec, DCM_AccessionNumber, d);

    return rec;
}

DcmDirectoryRecord* buildSeriesRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Series, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating series record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_Modality, d);
    dcmCopyString(rec, DCM_SeriesInstanceUID, d);
    dcmCopyString(rec, DCM_SeriesNumber, d);

    return rec;
}

DcmDirectoryRecord* buildImageRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Image, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating image record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_ImageNumber, d);
    /* addition type 1C keys specified by STD-GEN-CD profile */
    dcmCopyOptString(rec, DCM_ImageType, d);
    dcmCopyOptSequence(rec, DCM_ReferencedImageSequence, d);

    return rec;
}

DcmDirectoryRecord* buildOverlayRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Overlay, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating overlay record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_OverlayNumber, d);

    return rec;
}

DcmDirectoryRecord* buildModalityLutRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_ModalityLut, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating modality lut record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_LUTNumber, d);

    return rec;
}

DcmDirectoryRecord* buildVoiLutRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_VoiLut, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating voi lut record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_LUTNumber, d);

    return rec;
}

DcmDirectoryRecord* buildCurveRecord(const char* fname, DcmItem* d)
{
    DcmDirectoryRecord* rec = new DcmDirectoryRecord(ERT_Curve, fname);
    if (rec == NULL) {
	cerr << progname << 
	    ": error: out of memory (creating curve record)" << endl;
	return NULL;
    }
    
    dcmCopyOptString(rec, DCM_SpecificCharacterSet, d);
    dcmCopyString(rec, DCM_CurveNumber, d);

    return rec;
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
		char* fname = new char[strlen(reffname)+1];
		strcpy(fname, reffname);
		dicomToHostFilename(fname);
		
		match = cmp(dcmFindStringInFile(fname, DCM_StudyInstanceUID),
			    dcmFindString(dataset, DCM_StudyInstanceUID));
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
	match = cmp(dcmFindString(rec, DCM_ReferencedSOPInstanceUID),
		    dcmFindString(dataset, DCM_SOPInstanceUID));
	break;
    default:
	
	cerr << progname << 
	    ": error: record type not yet implemented" << endl;
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
    for (int i=0; i<n && !found; i++) {
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
	cerr << progname << 
	    ": error: record type not yet implemented" << endl;
	return FALSE;
	break;
    }
    return rec;
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
	    E_Condition cond = parentRec->insertSub(rec);
	    if (cond != EC_Normal) {
		cerr << progname << ": error: " 
		     << dcmErrorConditionToString(cond) 
		     << ": cannot insert " << recordTypeToName(dirtype)
		     << " record" << endl;
		return NULL;
	    }
	}
    }
    return rec;
}

static BOOL addToDir(DcmDirectoryRecord* rootRec, const char* ifname)
{
    char fname[1024];

    strcpy(fname, ifname);

    if (verbosemode) {
	cout << "adding: " << fname << endl;
    }
    DcmFileStream myin(fname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
	cerr << progname << ": cannot open file: " << fname << endl;
	return FALSE;
    }
    DcmFileFormat *ff = new DcmFileFormat();

    ff->transferInit();
    ff->read(myin, EXS_Unknown, EGL_withGL );
    ff->transferEnd();

    if (ff->error() != EC_Normal) {
	cerr << progname << ": error: " 
	     << dcmErrorConditionToString(ff->error())
	     << ": reading file: " << fname << endl;
	return FALSE;
    }

    if (checkImage(fname, ff) == FALSE) {
	return FALSE;
    }

    /*
    ** Massage filename into dicom format 
    ** (dos conventions for path separators)
    */
    hostToDicomFilename(fname);

    /*
    ** Ensure that the max number of components is not being exceeded
    */
    if (dicomFilenameComponentCount(fname) > 8) {
	cerr << progname 
	     << ": error: too many path components (max 8) in filename: "
	     << fname << endl;
	return FALSE;
    }

    E_Condition cond = EC_Normal;
    DcmMetaInfo* metainfo = ff->getMetaInfo();
    DcmDataset* dataset = ff->getDataset();
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
	    cerr << progname << ": error: " 
		 << dcmErrorConditionToString(cond) 
		 << ": cannot assign patient record to file: " 
		 << fname << endl;
	    return FALSE;
	}
    }

    /*
    ** Add a study record underneath the actual patient record
    */
    DcmDirectoryRecord *studyRec = 
	includeRecord(patientRec, ERT_Study, dataset, NULL);
    if (studyRec == NULL) {
	return FALSE;
    }

    /*
    ** Add a series record underneath the actual study
    */
    DcmDirectoryRecord *seriesRec = 
	includeRecord(studyRec, ERT_Series, dataset, NULL);
    if (seriesRec == NULL) {
	return FALSE;
    }

    /*
    ** Add one of the image-like records underneath the actual series record
    */

    if (cmp(sopClass, UID_StandaloneOverlayStorage)) {
	/* Add an overlay record */
	DcmDirectoryRecord *rec = 
	    includeRecord(seriesRec, ERT_Overlay, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else if (cmp(sopClass, UID_StandaloneModalityLUTStorage)) {
	/* Add a modality lut record */
	DcmDirectoryRecord *rec = 
	    includeRecord(seriesRec, ERT_ModalityLut, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else if (cmp(sopClass, UID_StandaloneVOILUTStorage)) {
	/* Add a voi lut record */
	DcmDirectoryRecord *rec = 
	    includeRecord(seriesRec, ERT_VoiLut, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else if (cmp(sopClass, UID_StandaloneCurveStorage)) {
	/* Add a curve record */
	DcmDirectoryRecord *rec = 
	    includeRecord(seriesRec, ERT_Curve, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    } else {
	/* it can only be an image */ 
	/* Add an image record */
	DcmDirectoryRecord *rec = 
	    includeRecord(seriesRec, ERT_Image, dataset, fname);
	if (rec == NULL) {
	    return FALSE;
	}
    }

    delete ff;
    
    return TRUE;
}

static BOOL
createDicomdirFromFiles(char* fileNames[], int numberOfFileNames)
{
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

    for (int i=0; i<numberOfFileNames; i++) {
	if (addToDir(&rootRecord, fileNames[i]) == FALSE) {
	    return FALSE;
	}
    }

    if (writeDicomdir) {
        dicomdir->write(EXS_LittleEndianExplicit, EET_ExplicitLength);
    }

    delete dicomdir;

    return TRUE;
}

/*
** CVS/RCS Log:
** $Log: dcmgpdir.cc,v $
** Revision 1.4  1997-04-18 08:06:56  andreas
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

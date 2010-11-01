/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmwlm
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose:
 *   Program to create a worklist file from a WWW CGI perl script generated
 *   hexedecimal encoded string.
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 13:37:32 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: writwlst v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************

static void
usage()
{
    CERR << rcsid << "\n\n"
	"writlst: write a worklist file from a hex coded string\n"
	"usage: wlistgen [options] hexstring-file-in dcmfile-out\n"
	"arguments:\n"
	"  hexstring-file-in   a filename of - reads from stdin\n"
	"  dcmfile-out         the dicom file to create\n"
	"options are:\n"
	"  create options:\n"
	"    +f    write fileformat (default)\n"
	"    -f    write dataset\n"
	"    +o    overwrite dcmfile-out if existing (default)\n"
	"    -o    update dcmfile-out if existing\n"
	"  group length encoding:\n"
	"    +g    write with group lengths\n"
	"    -g    write without group lengths (default)\n"
	"  length encoding in sequences and items:\n"
	"    +e    write with explicit lengths (default)\n"
	"    -e    write with undefined lengths\n"
	"  output transfer syntax:\n"
	"    +ti   write with little-endian implicit transfer syntax\n"
	"    +te   write with little-endian explicit transfer syntax (default)\n"
	"    +tb   write with big-endian explicit transfer syntax\n"
	"  other test/debug options:\n"
	"    +V    verbose mode, print actions\n"
	"    +v    validate input data (currently almost useless)\n"
	"    +d    enable debug output\n";
}


// ********************************************

#define SimpleStr_defaultSize 32

class SimpleStr {
private:
    char* str;
    int len;
    int maxlen;

    int max(int a, int b) {
	return ((a)>(b))?(a):(b);
    }
    void expand(int newlen);
public:
    int length() const {
	return len;
    }
    int maxlength() const {
	return maxlen;
    }

    const char* c_str() const {
	return str;
    }

    void append(char c) {
	expand(len+1);
	str[len] = c;
	len++;
	str[len] = '\0';
    }
    void append(const char* s) {
	if (s) {
	    int slen = strlen(s);
	    expand(len+slen);
	    strcat(str, s);
	    len += slen;
	}
    }
    void append(const SimpleStr& s) {
	append(s.c_str());
    }

    void clear() {
	if (str) {
	    str[0] = '\0';
	    len = 0;
	}
    }

    char& operator[](int i) {
	return str[i];
    }

    SimpleStr(int capacity = SimpleStr_defaultSize)
    : str(NULL)
    , len(0)
    , maxlen(0)
    {
	expand(capacity);
    }
    SimpleStr(const char* s)
    : str(NULL)
    , len(0)
    , maxlen(0)
    {
	append(s);
    }
    SimpleStr(const SimpleStr& s)
    : str(NULL)
    , len(0)
    , maxlen(0)
    {
	append(s);
    }
    ~SimpleStr() { delete[] str; }

private:
    /*undefined*/ SimpleStr& operator=(const SimpleStr&);
};

void SimpleStr::expand(int newlen)
{
    if (newlen >= maxlen) {
        maxlen = max(newlen, maxlen+SimpleStr_defaultSize);
        char* newstr = new char[maxlen+1];
        newstr[0] = '\0';
        if (str) {
            strcpy(newstr, str);
            delete[] str;
        }
        str = newstr;
    }
}

// ********************************************

static OFBool
addStringAttr(DcmItem *item, const DcmTagKey& key, const SimpleStr& str)
{
    OFCondition ec = EC_Normal;
    DcmTag tag(key);
    DcmElement *newElement = NULL;

    if (item == NULL) {
	CERR << "error: cannot add to null item" << OFendl;
	return OFFalse;
    }

    ec = newDicomElement(newElement, tag);
    if (ec != EC_Normal) {
	CERR << "error: cannot create element: " << key << OFendl;
	return OFFalse;
    }

    ec = newElement->putString(str.c_str());
    if (ec != EC_Normal) {
	CERR << "error: cannot put element value: " << key << OFendl;
	return OFFalse;
    }

    ec = item->insert(newElement, OFTrue);
    if (ec != EC_Normal) {
	CERR << "error: cannot insert element: " << key << OFendl;
	return OFFalse;
    }

    return OFTrue;
}


static DcmItem*
getOrInsertSequenceItem(DcmItem *dset, const DcmTagKey& key,
			unsigned int itemNumber)
{
    OFCondition ec = EC_Normal;
    DcmStack stack;
    DcmItem *foundItem = NULL;

    if (dset == NULL) {
	CERR << "error: cannot add to null item" << OFendl;
	return NULL;
    }

    ec = dset->search(key, stack, ESM_fromHere, OFFalse);

    if ((ec != EC_Normal) || (stack.empty())) {
	/* insert the sequence and items */
	DcmTag tag(key);
	DcmSequenceOfItems *sq = new DcmSequenceOfItems(tag);
	for (unsigned int i=0; i<=itemNumber; i++) {
	    DcmItem *item = new DcmItem;
	    sq->insert(item, OFTrue);
	}
	dset->insert(sq, OFTrue);
	foundItem = sq->getItem(itemNumber);
    } else if (stack.top()->ident() == EVR_SQ) {
	DcmSequenceOfItems *sq = (DcmSequenceOfItems*)stack.top();
	if (itemNumber > sq->card()) {
	    /* we need to create some more items */
	    for (unsigned long i=sq->card(); i<=itemNumber; i++) {
		DcmItem *item = new DcmItem;
		sq->insert(item, OFTrue);
	    }
	}
	foundItem = sq->getItem(itemNumber);
    } else {
	CERR << "error: not a sequence: " << key << OFendl;
	return NULL;
    }

    return foundItem;
}

// ********************************************

struct KeyPos {
    int position;
    DcmTagKey key;
    OFBool withinSequence;

    KeyPos(int pos, const DcmTagKey& k, OFBool inSeq = OFFalse);
};

KeyPos::KeyPos(int pos, const DcmTagKey& k, OFBool inSeq)
: position(pos)
, key(k)
, withinSequence(inSeq)
{
}

// ********************************************

static OFBool
addValue(int valCount, SimpleStr& val, DcmDataset& dset)
{
    static KeyPos posTab[] = {
	KeyPos(  0, DCM_RequestedProcedureID ),
	KeyPos(  1, DCM_RequestedProcedureDescription ),
	KeyPos(  2, DCM_StudyInstanceUID ),
	KeyPos(  3, DCM_RequestedProcedurePriority ),
	KeyPos(  4, DCM_AccessionNumber ),
	KeyPos(  5, DCM_RequestingPhysician ),
	KeyPos(  6, DCM_PatientName ),
	KeyPos(  7, DCM_PatientID ),
	KeyPos(  8, DCM_PatientBirthDate ),
	KeyPos(  9, DCM_PatientSex ),
	KeyPos( 10, DCM_MedicalAlerts ),
	KeyPos( 11, DCM_Allergies ),
	KeyPos( 12, DCM_ScheduledStationAETitle, OFTrue ),
	KeyPos( 13, DCM_ScheduledProcedureStepStartDate, OFTrue ),
	KeyPos( 14, DCM_ScheduledProcedureStepStartTime, OFTrue ),
	KeyPos( 15, DCM_Modality, OFTrue ),
	KeyPos( 16, DCM_ScheduledPerformingPhysicianName, OFTrue ),
	KeyPos( 17, DCM_ScheduledProcedureStepDescription, OFTrue ),
	KeyPos( 18, DCM_ScheduledStationName, OFTrue ),
	KeyPos( 19, DCM_ScheduledProcedureStepLocation, OFTrue ),
	KeyPos( 20, DCM_PreMedication, OFTrue ),
	KeyPos( 21, DCM_ScheduledProcedureStepID, OFTrue ),
	KeyPos( 22, DCM_RequestedContrastAgent, OFTrue ),
	KeyPos( 23, DCM_CommentsOnTheScheduledProcedureStep, OFTrue )
    };

    int posTabCount = (sizeof(posTab) / sizeof(posTab[0]));
    if (valCount < 0 || valCount >= posTabCount) {
	/* ignore extra values */
	return OFTrue;
    }

    KeyPos *kp = &posTab[valCount];
    if (kp->position != valCount) {
	CERR << "internal error: inconsistent posTab (valCount="
	     << valCount <<")" << OFendl;
	return OFFalse;
    }

#ifdef DEBUG
    printf("adding string: %s\n", val.c_str());
#endif

    OFBool ok = OFTrue;
    if (kp->withinSequence) {
	DcmItem* item1 = getOrInsertSequenceItem(
	    &dset, DCM_ScheduledProcedureStepSequence, 0);
	ok = addStringAttr(item1, kp->key, val);
    } else {
	ok = addStringAttr(&dset, kp->key, val);
    }
    return ok;
}

// ********************************************

#define SEPARATOR_CHAR '\\'

static OFBool
parseWorklist(FILE* f, DcmDataset& dset)
{
    SimpleStr hexChar(2);
    SimpleStr val;
    int valCount = 0;
    int c;

    /* ignore first '\\' */
    while (((c = fgetc(f)) != EOF) && isspace(c)) {
	/* do nothing */
    }
    if (c != SEPARATOR_CHAR) {
	CERR << "error: expected \'" << char(SEPARATOR_CHAR) << "\' " <<
	    "as first input character" << OFendl;
	return OFFalse;
    }

    while ((c = fgetc(f)) != EOF) {
	if (c == SEPARATOR_CHAR) {
	    addValue(valCount, val, dset);
	    val.clear();
	    valCount++;
	} else if (isxdigit(c)) {
	    hexChar.append(OFstatic_cast(char, c));
	    if (hexChar.length() == 2) {
		/* convert into normal character */
		unsigned int h;
		const char *s = hexChar.c_str();
		sscanf(s, "%x", &h);
		char newc = OFstatic_cast(char, h & 0xff);
		val.append(newc);
		hexChar.clear();
	    }
	} else if (isspace(c)) {
	    /* ignore */
	} else {
	    CERR << "error bad hex value: " << char(c) << OFendl;
	    return OFFalse;
	}
    }

    /* add any trailing value */
    addValue(valCount, val, dset);

    return OFTrue;
}

// ********************************************

int main(int argc, char *argv[])
{
    if (argc < 3) {
	usage();
        return 1;
    }

    const char *ifname = NULL;
    const char *ofname = NULL;
    DcmFileFormat *fileformat = NULL;
    DcmDataset *dataset = NULL;
    E_TransferSyntax xfer = EXS_LittleEndianExplicit;
    E_EncodingType enctype = EET_ExplicitLength;
    E_GrpLenEncoding ogltype = EGL_withoutGL;
    OFBool verifymode = OFFalse;
    OFBool verbosemode = OFFalse;
    // OFBool createFileFormat = OFTrue;
    OFBool keepOverride = OFFalse;

    OFLog::configure();

    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if ((strcmp(arg, "-") == 0) && (ifname == NULL)) {
	    ifname = arg;
	} else if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		CERR << "unknown argument: " << arg << OFendl;
		usage();
		return 1;
	    }
	    switch (arg[1]) {
	    case 'f':
                /*
		if (arg[0] == '-') {
		    createFileFormat = OFFalse;
		} else {
		    createFileFormat = OFTrue;
		}
                */
		break;
	    case 'g':
		if (arg[0] == '-') {
		    ogltype = EGL_withoutGL;
		} else {
		    ogltype = EGL_withGL;
		}
		break;
	    case 'e':
		if (arg[0] == '-') {
		    enctype = EET_UndefinedLength;
		} else {
		    enctype = EET_ExplicitLength;
		}
		break;
	    case 'o':
		if (arg[0] == '-') {
		    keepOverride = OFTrue;
		} else {
		    keepOverride = OFFalse;
		}
		break;
	    case 't':
		switch (arg[2]) {
		case 'i':
		    xfer = EXS_LittleEndianImplicit;
		    break;
		case 'e':
		    xfer = EXS_LittleEndianExplicit;
		    break;
		case 'b':
		    xfer = EXS_BigEndianExplicit;
		    break;
		default:
		    CERR << "unknown option: " << arg << OFendl;
		    return 1;
		}
		break;
	    case 'v':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verifymode = OFTrue;
		} else {
		    CERR << "unknown option: " << arg << OFendl;
		    return 1;
		}
		break;
	    case 'V':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verbosemode = OFTrue;
		} else {
		    CERR << "unknown option: " << arg << OFendl;
		    return 1;
		}
		break;
	    case 'd':
		OFLog::configure(OFLogger::DEBUG_LOG_LEVEL);
		break;
	    default:
		CERR << "unknown option: " << arg << OFendl;
		return 1;
	    }
	} else if ( ifname == NULL ) {
	    ifname = arg;
	} else if ( ofname == NULL ) {
	    ofname = arg;
	} else {
	    CERR << "too many arguments: " << arg << OFendl;
	    usage();
	    return 1;
	}
    }

    if ( ifname == NULL ) {
	CERR << "missing input file\n";
	return 1;
    }

    if ( ofname == NULL ) {
	CERR << "missing output file\n";
	return 1;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
	CERR << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE << OFendl;
    }

    // create dicom metaheader and dataset

    fileformat = new DcmFileFormat();
    if (! fileformat) {
	CERR << "virtual memory exhausted" << OFendl;
	return 1;
    }

    if (keepOverride)
    {
      // update old WL file
      if (fileformat->loadFile(ofname).bad()) keepOverride=OFFalse;
    }

    dataset=fileformat->getDataset();

    OFBool parseOK = OFTrue;

    if (strcmp(ifname, "-") == 0) {
	/* read from stdin */
	parseOK = parseWorklist(stdin, *dataset);

    } else {
	FILE* f = NULL;
	if ((f = fopen(ifname, "r")) != NULL) {
	    parseOK = parseWorklist(f, *dataset);
	    fclose(f);
	} else {
	    CERR << "cannot open file: " << ifname << OFendl;
	}
    }

    if (!parseOK) {
	return 1;
    }

    /* must add an attribute for the specific character set */
    addStringAttr(dataset, DCM_SpecificCharacterSet, "ISO_IR 100");

    /* verify dataset */

    if (verifymode) {
	if (verbosemode)
	    COUT << "verifying dataset" << OFendl;
	dataset->verify(OFTrue);
    }

    /* write to file */

    if (verbosemode)
	COUT << "writing file: " << ofname << OFendl;

    OFCondition l_error = fileformat->saveFile(ofname, xfer, enctype, ogltype);

    if (l_error.bad()) {
	CERR << "Error: " << l_error.text()
	    << ": writing file: "  << ofname << OFendl;
	return 1;
    }

    return 0;
}

/*
 * CVS/RCS Log
 *   $Log: writwlst.cc,v $
 *   Revision 1.10  2010-11-01 13:37:32  uli
 *   Fixed some compiler warnings reported by gcc with additional flags.
 *
 *   Revision 1.9  2010-10-14 13:20:03  joergr
 *   Updated copyright header. Added reference to COPYRIGHT file.
 *
 *   Revision 1.8  2010-08-09 13:32:08  joergr
 *   Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *   the official "keyword" is used for the attribute name which results in a
 *   number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *   Revision 1.7  2009-11-24 10:40:01  uli
 *   Switched to logging mechanism provided by the "new" oflog module.
 *
 *   Revision 1.6  2008-04-30 12:38:43  meichel
 *   Fixed compile errors due to changes in attribute tag names
 *
 *   Revision 1.5  2006/08/15 16:15:48  meichel
 *   Updated the code in module dcmwlm to correctly compile when
 *     all standard C++ classes remain in namespace std.
 *
 *   Revision 1.4  2005/12/08 15:48:40  meichel
 *   Changed include path schema for all DCMTK header files
 *
 *   Revision 1.3  2005/03/09 18:04:14  joergr
 *   Fixed spelling error.
 *
 *   Revision 1.2  2002/12/13 13:43:26  meichel
 *   Removed unused code reported by the MIPSpro 7.3 optimizer
 *
 *   Revision 1.1  2002/12/03 12:17:37  wilkens
 *   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
 *   so that dcmwlm can now completely replace wlistctn in the public domain part
 *   of dcmtk. Pertaining to this replacement requirement, another optional return
 *   key attribute was integrated into the wlm utilities.
 *
 *
 */

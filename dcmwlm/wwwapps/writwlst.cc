/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmwlm
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose:
 *   Program to create a worklist file from a WWW CGI perl script generated 
 *   hexedecimal encoded string.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-12-13 13:43:26 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/wwwapps/writwlst.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#include "ofstdinc.h"

#include "ofstream.h"
#include "dctk.h"
#include "dcdebug.h"
#include "dcuid.h"    /* for dcmtk version name */

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
	"    +dn   set debug level to n (n=1..9)\n";
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
	CERR << "error: cannot add to null item" << endl;
	return OFFalse;
    }

    ec = newDicomElement(newElement, tag);
    if (ec != EC_Normal) {
	CERR << "error: cannot create element: " << key << endl;
	return OFFalse;
    }

    ec = newElement->putString(str.c_str());
    if (ec != EC_Normal) {
	CERR << "error: cannot put element value: " << key << endl;
	return OFFalse;
    }

    ec = item->insert(newElement, OFTrue);
    if (ec != EC_Normal) {
	CERR << "error: cannot insert element: " << key << endl;
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
	CERR << "error: cannot add to null item" << endl;
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
	CERR << "error: not a sequence: " << key << endl;
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
	KeyPos(  6, DCM_PatientsName ),
	KeyPos(  7, DCM_PatientID ),
	KeyPos(  8, DCM_PatientsBirthDate ),
	KeyPos(  9, DCM_PatientsSex ),
	KeyPos( 10, DCM_MedicalAlerts ),
	KeyPos( 11, DCM_ContrastAllergies ),
	KeyPos( 12, DCM_ScheduledStationAETitle, OFTrue ),
	KeyPos( 13, DCM_ScheduledProcedureStepStartDate, OFTrue ),
	KeyPos( 14, DCM_ScheduledProcedureStepStartTime, OFTrue ),
	KeyPos( 15, DCM_Modality, OFTrue ),
	KeyPos( 16, DCM_ScheduledPerformingPhysiciansName, OFTrue ),
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
	CERR << "internal error: inconsistant posTab (valCount=" 
	     << valCount <<")" << endl;
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
	    "as first input character" << endl;
	return OFFalse;
    }

    while ((c = fgetc(f)) != EOF) {
	if (c == SEPARATOR_CHAR) {
	    addValue(valCount, val, dset);
	    val.clear();
	    valCount++;
	} else if (isxdigit(c)) {
	    hexChar.append(c);
	    if (hexChar.length() == 2) {
		/* convert into normal character */
		unsigned int h;
		const char *s = hexChar.c_str();
		sscanf(s, "%x", &h);
		char newc = (h & 0xff);
		val.append(newc);
		hexChar.clear();
	    }
	} else if (isspace(c)) {
	    /* ignore */
	} else {
	    CERR << "error bad hex value: " << char(c) << endl;
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
    SetDebugLevel(( 0 ));

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
    int localDebugLevel = 0;
    OFBool keepOverride = OFFalse;

    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if ((strcmp(arg, "-") == 0) && (ifname == NULL)) {
	    ifname = arg;
	} else if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		CERR << "unknown argument: " << arg << endl;
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
		    CERR << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'v':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verifymode = OFTrue;
		} else {
		    CERR << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'V':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verbosemode = OFTrue;
		} else {
		    CERR << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'd':
		if (sscanf(arg+2, "%d", &localDebugLevel) != 1) {
		    CERR << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    default:
		CERR << "unknown option: " << arg << endl;
		return 1;
	    }
	} else if ( ifname == NULL ) {
	    ifname = arg;
	} else if ( ofname == NULL ) {
	    ofname = arg;
	} else {
	    CERR << "too many arguments: " << arg << endl;
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
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }
    
    SetDebugLevel(( localDebugLevel ));

    // create dicom metaheader and dataset

    fileformat = new DcmFileFormat();
    if (! fileformat) {
	CERR << "virtual memory exhausted" << endl;
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
	    CERR << "cannot open file: " << ifname << endl;
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
	    COUT << "verifying dataset" << endl;
	dataset->verify(OFTrue);
    }

    /* write to file */
    
    if (verbosemode)
	COUT << "writing file: " << ofname << endl;

    OFCondition l_error = fileformat->saveFile(ofname, xfer, enctype, ogltype);

    if (l_error.bad()) {
	CERR << "Error: " << l_error.text()
	    << ": writing file: "  << ofname << endl;
	return 1;
    }

    return 0;
}

/*
 * CVS/RCS Log
 *   $Log: writwlst.cc,v $
 *   Revision 1.2  2002-12-13 13:43:26  meichel
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

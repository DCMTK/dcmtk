/*
**
** Author: Andreas Barth   25.01.96 -- Created
** Kuratorium OFFIS e.V.
**
** Module: dump2dcm.cc
**
** Purpose:
** Program to create a Dicom FileFormat or DataSet from an ASCII-dump
**
** Input File Description:
** The input file be an output of dcmdump. One element (Tag, VR, value) must 
** be written into one line separated by arbitrary spaces or tab characters.
** A # begins a comment that ends at the line end. Empty lines are allowed.
** This parts of a line have the following syntax:
** Tag:   (gggg,eeee)
**        with gggg and eeee are 4 character hexadecimal values representing
**        group- and element-tag. Spaces and Tabs can be anywhere in a Tag
**        specification
** VR:    Value Representation must be written as 2 characters as in Part 6
**        of the DICOM 3.0 standard. No Spaces or Tabs are allowed between the
**        two characters. If the VR can determined from the Tag, this part of
**        a line is optional.
** Value: There are several rules for writing values:
**        1. US, SS, SL, UL, FD, FL are written as 
**           decimal strings that can be read by scanf.
**        2. OB, OW values are written as byte or word hexadecimal values
**           separated by \
**        3. UI is written as =Name in data dictionary or as 
**           unique identifer string (see  5.) , e.g. [1.2.840.....]
**        4. Strings without () <> [] spaces, tabs and # can be 
**           written directly 
**        5. Other strings with must be surrounded by [ ]. No 
**           bracket structure is passed. The value ends at the last ] in
**           the line. Anything after the ] is interpreted as comment.
**        6. ( < are interpreted special and may not be used when writing
**           an input file by hand as beginning characters of a string.
**        Multiple Value are separated by \
**        The sequence of lines must not be ordered but they can. 
**        References in DICOM Directories are not supported.
**        Semantic errors are not detected.
**
** Examples:
**  (0008,0020) DA  [19921012]          #     8,  1  StudyDate
**  (0008,0016) UI  =MRImageStorage     #    26,  1  SOPClassUID
**  (0002,0012) UI  [1.2.276.0.7230010.100.1.1] 
**  (0020,0032) DS  [0.0\0.0]           #     8,  2  ImagePositionPatient
**  (0028,0010) US  256     
**  (0002,0001) OB  01\00  
**
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1999-01-07 14:13:12 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dump2dcm.cc,v $
** CVS/RCS Revision:	$Revision: 1.20 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <iostream.h>
#include <ctype.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"
#include "dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: dump2dcm v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// Maximum Line Size

const unsigned int DCM_DumpMaxLineSize = 4096;


// Field definition separators

#define DCM_DumpCommentChar '#'
#define DCM_DumpTagDelim ')'
#define DCM_DumpOpenString '['
#define DCM_DumpCloseString ']'
#define DCM_DumpOpenDescription '('
#define DCM_DumpOpenFile '<'
#define DCM_DumpCloseFile '>'


static void
usage()
{
    cerr << rcsid << "\n\n"
	"dump2dcm: convert dicom dumpfile into dicom-fileformat or -dataset\n"
	"usage: dump2dcm [options] dumpfile-in dcmfile-out\n"
	"options are:\n"
	"  input options: \n"
	"      +ln     maximum line length n (default 4096)\n"
	"  output options:\n"
	"    DICOM fileformat (Sup. 1) support:\n"
	"      -F      write file without metaheader\n"
	"      +F      write file with metaheader (default)\n"
	"    output transfer syntax:\n"
	"      +ti     write with little-endian implicit transfer syntax\n"
	"      +te     write with little-endian explicit transfer syntax(default)\n"
	"      +tb     write with big-endian explicit transfer syntax\n"
	"    group length encoding:\n" 
	"      +g      write with group lengths\n"
	"      +g=     recalculate group lengths (default)\n"
	"      -g      write without group lengths\n"
	"    length encoding in sequences and items:\n"
	"      +e      write with explicit lengths (default)\n"
	"      -e      write with undefined lengths\n"
	"    padding (only applicable for DICOM files with metaheader)\n"
	"      -p      no padding (default)\n"
	"      +p n m  pad file x*n bytes and items y*m bytes\n"
        "    unknown VR\n"
	"      -u      disable generation of new VRs (UN/UT/VS)\n"
        "      +u      enable generation of new VRs (UN/UT/VS) (default)\n"
	"  other options:\n"
	"      -h      print this usage string\n"
	"      +V      verbose mode, print actions\n"
	"      +dn     set debug level to n (n=1..9)\n";
}



static void
stripWhitespace(char* s)
{
    char* p;

    if (s == NULL) return;
		
    p = s;
    while (*s != '\0') {
	if (isspace(*s) == OFFalse) {
	    *p++ = *s;
	}
	s++;
    }
    *p = '\0';
}

static char*
stripTrailingWhitespace(char* s)
{
    int i, n;

    if (s == NULL) return s;

    n = strlen(s);
    for (i = n - 1; i >= 0 && isspace(s[i]); i--)
        s[i] = '\0';
    return s;
}


static char *
stripPrecedingWhitespace(char * s)
{
    char * p;
    if (s == NULL) return s;

    for(p = s; *p && isspace(*p); p++)
	;

    return p;
}

static OFBool
onlyWhitespace(const char* s)
{
    int len = strlen(s);
    int charsFound = OFFalse;

    for (int i=0; (!charsFound) && (i<len); i++) {
	charsFound = !isspace(s[i]);
    }
    return (!charsFound)?(OFTrue):(OFFalse);
}

static char*
getLine(char* line, int maxLineLen, FILE* f, const unsigned long lineNumber)
{
    char* s;

    s = fgets(line, maxLineLen, f);

    // if line is too long, throw rest of it away
    if (s && strlen(s) == size_t(maxLineLen-1) && s[maxLineLen-2] != '\n')
    {
	int c = fgetc(f);
	while(c != '\n' && c != EOF)
	    c = fgetc(f);
	cerr << "line " << lineNumber << " to long." << endl;
    }
       
	
    /* strip any trailing white space */
    stripTrailingWhitespace(s);
		
    return s;
}


static OFBool
isaCommentLine(const char* s)
{
    OFBool isComment = OFFalse; /* assumption */
    int len = strlen(s);
    int i = 0;
    for (i=0; i<len && isspace(s[i]); i++) /*loop*/;
    isComment = (s[i] == DCM_DumpCommentChar);
    return isComment;
}

static OFBool
parseTag(char* & s, DcmTagKey& key)
{
    OFBool ok = OFTrue;
    char * p;
    unsigned int g, e;
    
    // find tag begin
    p = strchr(s, DCM_DumpTagDelim);
    if (p)
    {
	// string all white spaces and read tag
	int len = p-s+1;
	p = new char[len+1];
	strncpy(p, s, len);
	p[len] = '\0';
	stripWhitespace(p);
	s += len;

	if (sscanf(p, "(%x,%x)", &g, &e) == 2) {
	    key.set(g, e);
	} else {
	    ok = OFFalse;
	}
	delete[] p;
    }
    else ok = OFFalse;

    return ok;
}


static OFBool
parseVR(char * & s, DcmEVR & vr)
{
    OFBool ok = OFTrue;

    s = stripPrecedingWhitespace(s);

    // Are there two upper characters?
    if (isupper(*s) && isupper(*(s+1)))
    {
	char c_vr[3];
	strncpy(c_vr, s, 2);
	c_vr[2] = '\0';
	// Convert to VR
	DcmVR dcmVR(c_vr);
	vr = dcmVR.getEVR();
	s+=2;
    } 
    else if (((*s == 'o')&&(*(s+1) == 'x')) || ((*s == 'x')&&(*(s+1) == 's'))
      || ((*s == 'n')&&(*(s+1) == 'a')) || ((*s == 'u')&&(*(s+1) == 'p')))
    {
      // swallow internal VRs
      vr = EVR_UNKNOWN;
      s+=2;
    }
    else ok = OFFalse;

    return ok;
}


static int
searchLastClose(char *s, char closeChar)
{
    // search last close bracket in a line
    // no bracket structure will be considered

    char * found = NULL;
    char * p = s;
    
    while(p && *p)
    {
	p = strchr(p, closeChar);
	if (p)
	{
	    found = p;
	    p++;
	}
    }

    if (found)
	return (found - s) + 1;
    else
	return 0;
}
	

static int
searchCommentOrEol(char *s)
{
    char * comment = strchr(s, DCM_DumpCommentChar);
    if (comment)
	return comment - s;
    else if (s)
	return strlen(s);
    else
	return 0;
}
	
	

static OFBool
parseValue(char * & s, char * & value)
{
    OFBool ok = OFTrue;
    int len;
    value = NULL;

    s = stripPrecedingWhitespace(s);

    switch (*s)
    {
    case DCM_DumpOpenString:
	len = searchLastClose(s, DCM_DumpCloseString);
	if (len == 0)
	    ok = OFFalse;
	else if (len > 2)
	{
	    value = new char[len-1];
	    strncpy(value, s+1, len-2);
	    value[len-2] = '\0';
	}
	else 
	    value = NULL;
	break;
      
    case DCM_DumpOpenDescription:
	break;

    case DCM_DumpOpenFile:
	ok = OFFalse;  // currently not supported
	break;

    case DCM_DumpCommentChar:
	break;
	
    default:
	len = searchCommentOrEol(s);
	if (len)
	{
	    value = new char[len+1];
	    strncpy(value, s, len);
	    value[len] = '\0';
	    stripTrailingWhitespace(value);
	}
	break;
    }
    return ok;
}


static E_Condition 
insertIntoSet(DcmStack & stack, DcmTagKey tagkey, DcmEVR vr, char * value)
{
    // insert new Element into dataset or metaheader

    E_Condition l_error = EC_Normal;
    E_Condition newElementError = EC_Normal;

    if (stack.empty())
	l_error = EC_CorruptedData;

    if (l_error == EC_Normal)
    {
	DcmElement * newElement = NULL;
	DcmObject * topOfStack = stack.top();

	// convert tagkey to tag including VR
	DcmTag tag(tagkey);
	DcmVR dcmvr(vr);
	
	const DcmEVR tagvr = tag.getEVR();
	if (tagvr != vr && vr != EVR_UNKNOWN &&     
	    (tagvr != EVR_ox || (vr != EVR_OB && vr != EVR_OW)))
	{
	    cerr << "Warning: Tag " << tag << " with wrong VR " 
		 << dcmvr.getVRName() << " found, correct is " 
		 << tag.getVR().getVRName() << endl;
	}

	if (vr != EVR_UNKNOWN)
	    tag.setVR(dcmvr);

	// create new element
	newElementError = newDicomElement(newElement, tag);

	switch (newElementError)
	{
	case EC_Normal:
	    // tag describes an Element
	    if (!newElement)
		// Tag was ambiguous
		l_error = EC_InvalidVR;
	    else
	    {
		// fill value
		if (value)
		    l_error = newElement->putString(value);

		// insert element into hierarchy
		if (l_error == EC_Normal)
		{
		    switch(topOfStack->ident())
		    {
		    case EVR_item:
		    case EVR_dirRecord:
		    case EVR_dataset:
		    case EVR_metainfo:
		    {
			DcmItem * item = (DcmItem *)topOfStack;
			item -> insert(newElement);
			if (newElement->ident() == EVR_SQ || 
			    newElement->ident() == EVR_pixelSQ)
			    stack.push(newElement);
		    }
		    break; 
		    default:
			l_error = EC_InvalidTag;
			break;
		    }
		}
	    }
	    break;

	case EC_SequEnd:
	    // pop stack if stack object was a sequence
	    if (topOfStack->ident() == EVR_SQ || 
		topOfStack->ident() == EVR_pixelSQ)
		stack.pop();
	    else
		l_error = EC_InvalidTag;
	    break;


	case EC_ItemEnd:
	    // pop stack if stack object was an item
	    switch (topOfStack->ident())
	    {
	    case EVR_item:
	    case EVR_dirRecord:
	    case EVR_dataset:
	    case EVR_metainfo:
		stack.pop();
		break;
		
	    default:
		l_error = EC_InvalidTag;
		break;
	    }
	    break;

	case EC_InvalidTag:
	    if (tag.getXTag() == DCM_Item)
	    {
		DcmItem * item = NULL;
		if (topOfStack->getTag().getXTag() == DCM_DirectoryRecordSequence)
		{
		    // an Item must be pushed to the stack
		    item = new DcmDirectoryRecord(tag, 0);
		    ((DcmSequenceOfItems *) topOfStack) -> insert(item);
		    stack.push(item);
		}
		else if (topOfStack->ident() == EVR_SQ)
		{
		    // an item must be pushed to the stack
		    item = new DcmItem(tag);
		    ((DcmSequenceOfItems *) topOfStack) -> insert(item);
		    stack.push(item);
		}
		else
		    l_error = EC_InvalidTag;
		
	    }
	    else 
		l_error = EC_InvalidTag;
	    break;

	default:
	    l_error = EC_InvalidTag;
	    break;
	}
    }

    return l_error;
}

static OFBool
readDumpFile(DcmMetaInfo * metaheader, DcmDataset * dataset, 
	     FILE * infile, const char * ifname, 
	     const unsigned long maxLineLength)
{
    char * lineBuf = new char[maxLineLength];
    int lineNumber = 0;
    OFBool errorOnThisLine = OFFalse;
    char * parse = NULL;
    char * value = NULL;
    DcmEVR vr;
    int errorsEncountered = 0;
    DcmTagKey tagkey;
    DcmStack metaheaderStack;
    DcmStack datasetStack;

    if (metaheader)
	metaheaderStack.push(metaheader);

    datasetStack.push(dataset);

    while(getLine(lineBuf, int(maxLineLength), infile, lineNumber+1))
    {
	lineNumber++;

	// ignore empty lines and comment lines
	if (onlyWhitespace(lineBuf))
	    continue;
	if (isaCommentLine(lineBuf))
	    continue;

	errorOnThisLine = OFFalse;
	
	parse = &lineBuf[0];

	// parse tag an the line
	if (!parseTag(parse, tagkey))
	{
	    cerr << "dump2dcm: "<< ifname << ": "
		 << "no Tag found (line " 
		 << lineNumber << ")"<< endl;
	    errorOnThisLine = OFTrue;
	}

	// parse optional VR
	if (!errorOnThisLine && !parseVR(parse, vr))
	    vr = EVR_UNKNOWN;

	// parse optional value
	if (!errorOnThisLine && !parseValue(parse, value))
	{
	    cerr << "dump2dcm: "<< ifname << ": "
		 << "incorrect value specification (line " 
		 << lineNumber << ")"<< endl;
	    errorOnThisLine = OFTrue;
	}
	 

	// insert new element that consists of tag, VR, and value
	if (!errorOnThisLine)
	{
	    E_Condition l_error = EC_Normal;
	    if (tagkey.getGroup() == 2)
	    {
		if (metaheader)
		    l_error = insertIntoSet(metaheaderStack, tagkey, vr, value);
	    }
	    else
		l_error = insertIntoSet(datasetStack, tagkey, vr, value);

	    if (value)
	    {
		delete[] value;
		value = NULL;
	    }
	    if (l_error != EC_Normal)
	    {
		errorOnThisLine = OFTrue;
		cerr << "dump2dcm: " << ifname << ": Error in creating Element: "
		     << dcmErrorConditionToString(l_error) << " (line "
		     << lineNumber << ")"<< endl;
	    }

	}

	if (errorOnThisLine)
	    errorsEncountered++;
    }


    // test blocking structure
    if (metaheader && metaheaderStack.card() != 1)
    {
	cerr << "dump2dcm: " << ifname << ": Block Error in metaheader"
	     << endl;
	errorsEncountered++;
    }

    if (datasetStack.card() != 1)
    {
	cerr << "dump2dcm: " << ifname << ": Block Error in dataset"
	     << endl;
	errorsEncountered++;
    }

    delete[] lineBuf;

    if (errorsEncountered)
    {
	cerr << errorsEncountered << " Errors found in " <<  ifname << endl;
	return OFFalse;
    }
    else
	return OFTrue;
}

// ********************************************

int main(int argc, char *argv[])
{
#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(0);

    prepareCmdLineArgs(argc, argv, "dump2dcm");
        
    if (argc < 3) {
	usage();
        return 1;
    }

    const char*	ifname = NULL;
    const char*	ofname = NULL;
    DcmFileFormat * fileformat = NULL;
    DcmMetaInfo * metaheader = NULL;
    DcmDataset * dataset = NULL;
    E_TransferSyntax oxfer = EXS_LittleEndianExplicit;
    E_EncodingType oenctype = EET_ExplicitLength;
    E_GrpLenEncoding oglenc = EGL_recalcGL;
    E_PaddingEncoding opadenc = EPD_withoutPadding;
    Uint32 padlen = 0;
    Uint32 subPadlen = 0;
    OFBool verbosemode = OFFalse;
    OFBool createFileFormat = OFTrue;
    int localDebugLevel = 0;
    unsigned long maxLineLength = DCM_DumpMaxLineSize;

    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		cerr << "unknown argument: " << arg << endl;
		return 1;
	    }
	    switch (arg[1]) {
	    case 'F':
		if (arg[0] == '-' && arg[2] =='\0') 
		    createFileFormat = OFFalse;
		else if (arg[0] == '+' && arg[2] == '\0')
		    createFileFormat = OFTrue;
		else
		{
		    cerr << "unknown argument: "<< arg << endl;
		    return 1;
		}
		break;
	    case 't':
	    {
		if (arg[0] != '+' || arg[2] == '\0' || arg[3] != '\0')
		{
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		switch (arg[2]) {
		case 'i':
		    oxfer = EXS_LittleEndianImplicit;
		    break;
		case 'e':
		    oxfer = EXS_LittleEndianExplicit;
		    break;
		case 'b':
		    oxfer = EXS_BigEndianExplicit;
		    break;
		default:
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
	    }
	    break;
	    case 'e':
		if (arg[0] == '-' && arg[2] == '\0') 
		    oenctype = EET_UndefinedLength;
		else if (arg[0] == '+' && arg[2] == '\0')
		    oenctype = EET_ExplicitLength;
		else
		{
		    cerr << "wrong parameter option +p n m\n";
		    return 1;
		}
		break;
	    case 'g':
		if (arg[0] == '+' && arg[2] == '\0')
		    oglenc = EGL_withGL;
		else if (arg[0] == '+' && arg[2] == '=' && arg[3] == '\0')
		    oglenc = EGL_recalcGL;
		else if (arg[0] == '-' && arg[2] == '\0')
		    oglenc = EGL_withoutGL;
		else 
		{
		    cerr << "unknown argument: "<< arg << endl;
		    return 1;
		}
		break;
	    case 'p':
		if (arg[0] == '-' && arg[2] == '\0')
		    opadenc = EPD_withoutPadding;
		else if (arg[0] == '+' && arg[2] == '\0')
		{
		    opadenc = EPD_withPadding;
#if SIZEOF_LONG == 8
		    if (sscanf(argv[++i], "%d", &padlen) != 1)
#else
		    if (sscanf(argv[++i], "%ld", &padlen) != 1)
#endif
		    {
			cerr << "wrong parameter option +p n m\n";
			return 1;
		    }
#if SIZEOF_LONG == 8
		    if (sscanf(argv[++i], "%d", &subPadlen) != 1)
#else
		    if (sscanf(argv[++i], "%ld", &subPadlen) != 1)
#endif
		    {
			cerr << "wrong parameter option +p n m\n";
			return 1;
		    }
		}
		else
		{
		    cerr << "wrong parameter option +p n m\n";
		    return 1;
		}
		break;
	    case 'u':
		if (arg[0] == '-' && arg[2] == '\0') {
		    dcmEnableUnknownVRGeneration = OFFalse;
		    dcmEnableUnlimitedTextVRGeneration = OFFalse;
		    dcmEnableVirtualStringVRGeneration = OFFalse;
		} else if (arg[0] == '+' && arg[2] == '\0') {
		    dcmEnableUnknownVRGeneration = OFTrue;
		    dcmEnableUnlimitedTextVRGeneration = OFTrue;
		    dcmEnableVirtualStringVRGeneration = OFTrue;
		} else {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'h':
		if (arg[0] == '-' && arg[2] == '\0')
		{
		    usage();
		    return 0;
		}
		else
		{
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		/* break; */ /* never reached after return */
	    case 'V':
		if (arg[0] == '+' && arg[2] == '\0') 
		    verbosemode = OFTrue;
		else 
		{
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
	    case 'l':
		if (sscanf(arg+2, "%lu", &maxLineLength) != 1) 
		{
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    default:
		cerr << "unknown option: " << arg << endl;
		return 1;
	    }
	}
	else if ( ifname == NULL ) 
	    ifname = arg;
	else if ( ofname == NULL ) 
	    ofname = arg;
	else 
	{
	    cerr << "too many arguments: " << arg << endl;
	    return 1;
	}
    }

    if (!createFileFormat && opadenc == EPD_withPadding)
    {
	cerr << "Padding is not allowed in datasets\n";
	return 1;
    }

    if ( ifname == NULL ) {
	cerr << "missing input file name\n";
	return 1;
    }

    if ( ofname == NULL ) {
	cerr << "missing output file name\n";
	return 1;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
	cerr << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }
    
    SetDebugLevel(localDebugLevel);

    if (verbosemode) 
	cout << "reading dumfile: " << ifname << endl;


    // create dicom metaheader and dataset
    OFBool memoryError = OFFalse;
    if (createFileFormat)
    {
	fileformat = new DcmFileFormat();
	if (fileformat)
	{
	    metaheader = fileformat -> getMetaInfo();
	    dataset = fileformat -> getDataset();
	    if (!metaheader || !dataset)
		memoryError = OFTrue;
	}
	else
	    memoryError = OFTrue;
    }
    else
    {
	dataset = new DcmDataset();
	if (!dataset)
	    memoryError = OFTrue;
    }

    if (memoryError)
    {
	cerr << "virtual memory exhausted" << endl;
	return 1;
    }


    // open input dump file
    FILE * dumpfile = fopen(ifname, "r");
    if (!dumpfile)
    {
	cerr << "inputfile does not exist: " << ifname << endl;
	return 1;
    }

    // read dump file into metaheader and dataset
    if (readDumpFile(metaheader, dataset, dumpfile, ifname, maxLineLength))
    {
	// write into file format or dataset
	if (verbosemode)
	    cout << "writing as DICOM file format or dataset" << endl;

	DcmFileStream oStream(ofname, DCM_WriteMode);

	if (oStream.Fail())
	{
	    cerr << "cannot open output file: " << ofname << endl;
	    return 1;
	}

	E_Condition l_error = EC_Normal;
	if (fileformat)
	{
	    fileformat -> transferInit();
	    l_error = fileformat -> write(oStream, oxfer, oenctype, oglenc, 
					  opadenc, padlen, subPadlen);
	}
	else if (dataset)
	{
	    dataset -> transferInit();
	    l_error = dataset -> write(oStream, oxfer, oenctype, oglenc, EPD_withoutPadding);
	}

	if (l_error == EC_Normal)
	    cout << "Dump successfully converted." << endl;
	else
	{
	    cerr << "Error: " << dcmErrorConditionToString(l_error) 
		 << ": reading file: "  << ifname << endl;
	    return 1;
	}
    }

    return 0;
}

//*******************************

/*
** CVS/RCS Log:
** $Log: dump2dcm.cc,v $
** Revision 1.20  1999-01-07 14:13:12  meichel
** Corrected bug in dump2dcm that prevented the correct processing of
**   dumps created with dcmdump if they contained the "internal" VR markers
**   "xs" (US or SS) or "ox" (OB or OW).
**
** Revision 1.19  1998/01/27 10:51:27  meichel
** Removed some unused variables, meaningless const modifiers
**   and unreached statements.
**
** Revision 1.18  1998/01/14 14:41:15  hewett
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.17  1997/08/05 07:34:54  andreas
** Corrected Error handling of SQ in dump2dcm
**
** Revision 1.16  1997/07/21 07:59:02  andreas
** - Deleted support for DcmPixelItems and DcmPixelSequences in dump2dcm
**   ToDo: New support should be added in the future compatible to
**   the new DcmPixel class.
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.15  1997/07/03 15:09:40  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.14  1997/05/30 06:44:57  andreas
** - fixed scanf format problem leading to warnings on 64 bit machines.
**
** Revision 1.13  1997/05/29 15:52:52  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.12  1997/05/22 13:26:25  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.11  1997/05/20 07:57:12  andreas
** - Removed obsolete applications file2ds and ds2file. The functionality of these
**   applications is now peformed by dcmconv. Unified calling parameters
**   are implemented in dump2dcm, dcmdump and dcmconv.
**
** Revision 1.10  1997/05/16 08:31:06  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.9  1997/04/18 08:06:56  andreas
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
** Revision 1.8  1997/03/27 15:47:25  hewett
** Added command line switche to allow generation of UN to be
** disabled (it is enabled by default).
**
** Revision 1.7  1996/09/24 16:13:51  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.6  1996/05/02 17:00:23  hewett
** Corrected program name in usage description.
**
** Revision 1.5  1996/05/02 15:55:11  hewett
** Stopped whitespace being stripped from inside value strings when
** no [] delimiter present.  Now only leading and trailing whitespace
** is stripped.
**
** Revision 1.4  1996/04/27 12:13:01  hewett
** Corrected bug in last bug-fix.  A tag value [some text] was being
** parsed as an empty string.  Now both [] and [some text] appear to
** work as intended.
**
** Revision 1.3  1996/03/22 12:38:44  andreas
** Correct some mistakes: handling [] as empty string (no value field)
**                        handling =Name correct if Name is not correct
**
** Revision 1.2  1996/03/12 15:11:39  hewett
** Added call to prepareCmdLineArgs to enable command line arguments
** in environments which do not provide them.
**
** Revision 1.1  1996/01/29 13:36:38  andreas
** dump2dcm added convert ASCII descriptions into DICOM files
**
**
*/






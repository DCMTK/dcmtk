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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-04-27 12:13:01 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dump2dcm.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
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
#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

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
    cerr <<
	   "dump2dcm: convert dicom dumpfile into dicom-fileformat or -dataset\n"
	   "usage: dcmconv [options] dumpfile-in dcmfile-out\n"
	   "options are:\n"
           "  read options: \n"
           "    +ln   maximum line length n (default 4096)\n"
           "  create options:\n"
	   "    +f    write fileformat (default)\n"
	   "    -f    write dataset\n"
	   "  group length encoding:\n" 
	   "    +g    write with group lengths (default)\n"
	   "    -g    write without group lengths\n"
	   "  length encoding in sequences and items:\n"
	   "    +e    write with explicit lengths (default)\n"
	   "    -e    write with undefined lengths\n"
	   "  output transfer syntax:\n"
	   "    +ti   write with little-endian implicit transfer syntax (default)\n"
	   "    +te   write with little-endian explicit transfer syntax\n"
	   "    +tb   write with big-endian explicit transfer syntax\n"
	   "  other test/debug options:\n"
	   "    +V    verbose mode, print actions\n"
	   "    +v    validate input data (currently almost useless)\n"
	   "    +dn   set debug level to n (n=1..9)\n";
}



static void
stripWhitespace(char* s)
{
    char* p;

    if (s == NULL) return;
		
    p = s;
    while (*s != '\0') {
	if (isspace(*s) == FALSE) {
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

static BOOL
onlyWhitespace(const char* s)
{
    int len = strlen(s);
    int charsFound = FALSE;

    for (int i=0; (!charsFound) && (i<len); i++) {
	charsFound = !isspace(s[i]);
    }
    return (!charsFound)?(TRUE):(FALSE);
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


static BOOL
isaCommentLine(const char* s)
{
    BOOL isComment = FALSE; /* assumption */
    int len = strlen(s);
    int i = 0;
    for (i=0; i<len && isspace(s[i]); i++) /*loop*/;
    isComment = (s[i] == DCM_DumpCommentChar);
    return isComment;
}

static BOOL
parseTag(char* & s, DcmTagKey& key)
{
    BOOL ok = TRUE;
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
	    ok = FALSE;
	}
	delete[] p;
    }
    else ok = FALSE;

    return ok;
}


static BOOL
parseVR(char * & s, DcmEVR & vr)
{
    BOOL ok = TRUE;

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
    else ok = FALSE;

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
	
	

static BOOL
parseValue(char * & s, char * & value)
{
    BOOL ok = TRUE;
    int len;
    value = NULL;

    s = stripPrecedingWhitespace(s);

    switch (*s)
    {
    case DCM_DumpOpenString:
	len = searchLastClose(s, DCM_DumpCloseString);
	if (len == 0)
	    ok = FALSE;
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
	ok = FALSE;  // currently not supported
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
	    stripWhitespace(value);
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
		    l_error = newElement->put(value);

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
		else if (topOfStack->ident() == EVR_pixelSQ)
		{
		    // pixel item is a DcmElement, not a DcmItem
		    ((DcmPixelSequence *) topOfStack) -> insert(new DcmPixelItem(tag));;
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

static BOOL
readDumpFile(DcmMetaInfo * metaheader, DcmDataset * dataset, 
	     FILE * infile, const char * ifname, 
	     const unsigned long maxLineLength)
{
    char * lineBuf = new char[maxLineLength];
    int lineNumber = 0;
    BOOL errorOnThisLine = FALSE;
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

    while(getLine(lineBuf, maxLineLength, infile, lineNumber+1))
    {
	lineNumber++;

	// ignore empty lines and comment lines
	if (onlyWhitespace(lineBuf))
	    continue;
	if (isaCommentLine(lineBuf))
	    continue;

	errorOnThisLine = FALSE;
	
	parse = &lineBuf[0];

	// parse tag an the line
	if (!parseTag(parse, tagkey))
	{
	    cerr << "dump2dcm: "<< ifname << ": "
		 << "no Tag found (line " 
		 << lineNumber << ")"<< endl;
	    errorOnThisLine = TRUE;
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
	    errorOnThisLine = TRUE;
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
		errorOnThisLine = TRUE;
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
	return FALSE;
    }
    else
	return TRUE;
}

// ********************************************

int main(int argc, char *argv[])
{
    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv);
        
    if (argc < 3) {
	usage();
        return 1;
    }

    const char*	ifname = NULL;
    const char*	ofname = NULL;
    DcmFileFormat * fileformat = NULL;
    DcmMetaInfo * metaheader = NULL;
    DcmDataset * dataset = NULL;
    E_TransferSyntax xfer = EXS_LittleEndianImplicit;
    E_EncodingType enctype = EET_ExplicitLength;
    E_GrpLenEncoding ogltype = EGL_withGL;
    BOOL verifymode = FALSE;
    BOOL verbosemode = FALSE;
    BOOL createFileFormat = TRUE;
    int localDebugLevel = 0;
    unsigned long maxLineLength = DCM_DumpMaxLineSize;

    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		cerr << "unknown argument: " << arg << endl;
		usage();
		return 1;
	    }
	    switch (arg[1]) {
	    case 'f':
		if (arg[0] == '-') {
		    createFileFormat = FALSE;
		} else {
		    createFileFormat = TRUE;
		}
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
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'v':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verifymode = TRUE;
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
	    case 'l':
		if (sscanf(arg+2, "%lu", &maxLineLength) != 1) {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    default:
		cerr << "unknown option: " << arg << endl;
		return 1;
	    }
	} else if ( ifname == NULL ) {
	    ifname = arg;
	} else if ( ofname == NULL ) {
	    ofname = arg;
	} else {
	    cerr << "too many arguments: " << arg << endl;
	    usage();
	    return 1;
	}
    }

    if ( ifname == NULL ) {
	cerr << "missing input file\n";
	return 1;
    }

    if ( ofname == NULL ) {
	cerr << "missing output file\n";
	return 1;
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	cerr << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }
    
    SetDebugLevel(( localDebugLevel ));

    if (verbosemode) {
	cout << "reading " << ifname << endl;
    }


    // create dicom metaheader and dataset
    BOOL memoryError = FALSE;
    if (createFileFormat)
    {
	fileformat = new DcmFileFormat();
	if (fileformat)
	{
	    metaheader = fileformat -> getMetaInfo();
	    dataset = fileformat -> getDataset();
	    if (!metaheader || !dataset)
		memoryError = TRUE;
	}
	else
	    memoryError = TRUE;
    }
    else
    {
	dataset = new DcmDataset();
	if (!dataset)
	    memoryError = TRUE;
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
	if (metaheader)
	    metaheader -> removeGroupLengthElements();
	dataset -> removeGroupLengthElements();
	if (verifymode)
	{
	    if (verbosemode)
		cout << "verify DICOM file format or dataset" << endl;
	    if (metaheader)
		metaheader->verify(TRUE);
	    dataset->verify(TRUE);
	}

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
	    l_error = fileformat -> write(oStream, xfer, enctype, ogltype);
	}
	else if (dataset)
	{
	    dataset -> transferInit();
	    l_error = dataset -> write(oStream, xfer, enctype, ogltype);
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
** Revision 1.4  1996-04-27 12:13:01  hewett
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






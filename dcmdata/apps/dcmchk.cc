/*
**
** Author: Andrew Hewett    17-03-97
**         
**
** Module: dcmchk.cc
**
** Purpose:
** Perform very simple checking of a dicom file for encoding errors
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-03-26 17:38:14 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/Attic/dcmchk.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <iostream.h>
#include <string.h>
#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
#include <GUSI.h>
#include <SIOUX.h>
#endif

static BOOL verbose = FALSE;
static BOOL prependSequenceHierachy = TRUE;

static int dcmchk(ostream & out, const char* ifname, BOOL isDataset, 
		  E_TransferSyntax xfer, BOOL showFullData, BOOL loadMode);


// ********************************************

static void
usage()
{
    fprintf(stderr, 
	   "dcmdump: dump dicom file and data set\n"
	   "usage: dcmdump [options] dcmfile-in [options] dcmfile-in\n"
	   "Options are valid if specified before filename.\n"
	   "options are:\n"
	   "  output option\n"
           "    +E    print to stderr\n"
	   "    -E    print to stdout (default)\n"
           "    +L    print long tag values\n"
           "    -L    print short tag values (default)\n"
           "  input options:\n"
           "    +D    read as a dataset\n"
           "    -D    read as a fileformat or dataset (default)\n"
	   "  input transfer syntax: use only after +D\n"
	   "    +t=   use transfer syntax recognition (default)\n"
	   "    +ti   read with little-endian implicit transfer syntax\n"
	   "    +te   read with little-endian explicit transfer syntax\n"
	   "    +tb   read with big-endian explicit transfer syntax\n"
	   "  other test/debug options:\n"
	   "    +v    verbose mode\n"
	   "    +l    force load of all input data into memory\n");
}

int main(int argc, char *argv[])
{
    BOOL showFullData = FALSE;
    BOOL loadMode = FALSE;
    BOOL isDataset = FALSE;
    BOOL perr = FALSE;
    E_TransferSyntax xfer = EXS_Unknown;

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    /* set options for the Metrowerks CodeWarrior SIOUX console */
    SIOUXSettings.asktosaveonclose = FALSE;
    SIOUXSettings.showstatusline = TRUE;
    /* set options for the GUSI sockets library */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv, "dcmchk");
    
    if (argc < 2) {
	usage();
        return 1;
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }
    
    /* 
    ** Setup operating paramters for dcmdata library 
    */
    dcmEnableAutomaticInputDataCorrection = FALSE; /* make no changes */
    
    
    /* parse cmd line */
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
		if (arg[0] == '+' && arg[2] == '\0')
		    isDataset = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    isDataset = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'E':
		if (arg[0] == '+' && arg[2] == '\0')
		    perr = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    perr = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'L':
		if (arg[0] == '+' && arg[2] == '\0') 
		    showFullData = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    showFullData = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'h':
		usage();
		return 0;
	    case 'v':
		if (arg[0] == '+' && arg[2] == '\0') 
		    verbose = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    verbose = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'l':
		if (arg[0] == '+' && arg[2] == '\0') 
		    loadMode = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    loadMode = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 't':
		if (!isDataset)
		{
		    fprintf(stderr, 
			    "option %s is not allowed for fileformats\n", arg);
		    return 1;
		}
		else
		{
		    switch (arg[2]) {
		    case '=':
			xfer = EXS_Unknown;
			break;
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
			fprintf(stderr, "unknown option: %s\n", arg);
			return 1;
		    }
		}
		break;
	    default:
		fprintf(stderr, "unknown option: %s\n", arg);
		return 1;
	    }
	}
	else
	{
	    if (!perr)
		dcmchk(cout, arg, isDataset, xfer, showFullData, loadMode);
	    else
		dcmchk(cerr, arg, isDataset, xfer, showFullData, loadMode);
	}
    }
	    
    return 0;
}

static void printResult(ostream& out, DcmStack& stack, BOOL showFullData, BOOL& alreadyOutput)
{
    if (alreadyOutput) {
        /* attribute name already printed so don't print again */
        return;
    }
    
    int n = stack.card();
    if (n == 0) {
	return;
    }

    if (prependSequenceHierachy) {
	/* print the path leading up to the top stack elem */
	for (int i=n-1; i>=1; i--) {
	    DcmObject *dobj = stack.elem(i);
	    /* do not print if a DCM_Item as this is not 
	     * very helpful to distinguish instances.
	     */
	    if (dobj != NULL && dobj->getTag().getXTag() != DCM_Item) {
		char buf[128];
		sprintf(buf, "(%x,%x).", 
			(unsigned)dobj->getGTag(), 
			(unsigned)dobj->getETag());
		out << buf;
	    }
	}
    }

    /* print the tag and its value */
    DcmObject *dobj = stack.top();
    dobj->print(out, showFullData);
    
    alreadyOutput = TRUE;
}

static BOOL isaStringVR(DcmVR& vr)
{
    BOOL isaString = FALSE;
    switch(vr.getEVR())
    {
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
        isaString = TRUE;
        break;
    default:
        isaString = FALSE;
	break;
    }
    return isaString;
}

static const char* streamvm(const DcmDictEntry *e)
{
    static char buf[256];
    if (e->isFixedSingleVM()) {
	sprintf(buf, "%d", e->getVMMax());
    } else if (e->isVariableRangeVM()) {
	sprintf(buf, "%d-n", e->getVMMin());
    } else if (e->isFixedRangeVM()){
	sprintf(buf, "%d-%d", e->getVMMin(), e->getVMMax());
    } else {
	sprintf(buf, "?(%d-%d)?", e->getVMMin(), e->getVMMax());
    }
    return buf;
}

static const char* streamLengthOfValue(DcmVR& vr)
{
    static char buf[256];
    Uint32 min = vr.getMinValueLength();
    Uint32 max = vr.getMaxValueLength();
    Uint32 undefLen = DCM_UndefinedLength;

    if (min==max) {
	sprintf(buf, "%d bytes fixed length", (int)min);
    } else if (min==0) {
        if (max==undefLen) {
            sprintf(buf, "unrestricted length");
        } else {
            sprintf(buf, "%d bytes maximum", (int)max);
        }
    } else {
        sprintf(buf, "range %d-%d bytes length", (int)min, (int)max);
    }      
    return buf;
}

static int
splitFields(char* line, char* fields[], int maxFields, char splitChar)
{
    char* p;
    int foundFields = 0;
    int len;

    do {
	p = strchr(line, splitChar);
	if (p == NULL) {
	    len = strlen(line);
	} else {
	    len = p - line;
	}
	fields[foundFields] = new char[len+1];
	strncpy(fields[foundFields], line, len);
	fields[foundFields][len] = '\0';
	
	foundFields++;
	line = p + 1;
    } while ((foundFields < maxFields) && (p != NULL));

    return foundFields;
}

static BOOL isaKnownPointer(DcmTag& t)
{
    /*
    ** The DICOMDIR code automatically converts any pointers
    ** to have VR=up even if when the data has an explicit VR encoding.
    ** If this attribute is a known pointer then it is ok for it too
    ** have the internal VR=up.  
    */

    const DcmDictEntry *dictRef = t.getDictRef();

    if (dictRef == NULL) {
	return FALSE;
    }
    if (t.getEVR() == EVR_up && t.getEVR() == dictRef->getEVR()) {
	return TRUE;
    }
    return FALSE;
}

static int checkelem(ostream & out, DcmElement *elem,  DcmXfer& oxfer, 
	DcmStack& stack, BOOL showFullData, BOOL& alreadyOutput)
{
    DcmVR vr(elem->getVR());
    Uint32 len = elem->getLength();
    DcmTag tag(elem->getTag());
    Uint32 vm = elem->getVM();
    const DcmDictEntry *dictRef = tag.getDictRef();
    int i = 0;

    if (verbose) {
	printResult(out, stack, showFullData, alreadyOutput);
    }

    /* 
    ** if the data was encoded in explicit VR then check that the given VR matches
    ** the value in the data dictionary.
    */
    if (oxfer.isExplicitVR()) {
        if (!vr.isStandard() && !isaKnownPointer(tag)) {
            printResult(out, stack, showFullData, alreadyOutput);
            out << "error: invalid VR=" << vr.getVRName() << endl;    
	} else if (dictRef && !vr.isEquivalent(dictRef->getVR())) {
            printResult(out, stack, showFullData, alreadyOutput);
            out << "error: attribute uses unexpected VR=" << vr.getVRName() << " (" << 
                dictRef->getVR().getVRName() << " in data dictionary)" << endl;
	}
    }

    if (len == 0) {
       /* type 2 attributes can be empty.  there is nothing more to check */
       return 0;
    }
    
    /*
    ** Check value multiplicity
    */
    if (vm < (Uint32)tag.getMinVM()) {
        printResult(out, stack, showFullData, alreadyOutput);
        out << "error: VM=" << vm << " too small (expected " << 
            streamvm(dictRef) << ")" << endl;       
    }

    if (vm > (Uint32)tag.getMaxVM()) {
        printResult(out, stack, showFullData, alreadyOutput);
        out << "error: VM=" << vm << " too large (expected " << 
            streamvm(dictRef) << ")" << endl;       
    }

    /*
    ** Check length of attribute
    */
    /* Need to split the value into its components if VM>1 */
    if (isaStringVR(vr)) {
        /* only strings have variable length components */
        char* value = ((DcmByteString*)elem)->get();
        
        char **fields = new char*[vm+1];
        if (fields == NULL) {
            printResult(out, stack, showFullData, alreadyOutput);
            out << "internal error: out of memory (value multiplicity too large)" << endl;     
        } else {
            int nfields = splitFields(value, fields, vm, '\\');
            if ((Uint32)nfields != vm) {
                printResult(out, stack, showFullData, alreadyOutput);
                out << "internal error: nfields != vm (" 
                    << nfields << "!=" << vm << ")" << endl;
                exit(1);
            }
            for (i=0; (Uint32)i<vm; i++) {
                char* s = fields[i];
                int slen = strlen(s);
                if ((Uint32)slen > vr.getMaxValueLength()) {
                    printResult(out, stack, showFullData, alreadyOutput);
                    out << "error: value length of " << slen << " bytes is too large (" << 
                    	vr.getVRName() << " is " << streamLengthOfValue(vr) << ")" << endl;       
		    if (vm > 1) {
                        out << "       affected value[" << i << "]=\"" << s << "\"" << endl;
                    }
                }
                if ((Uint32)slen < vr.getMinValueLength()) {
                    printResult(out, stack, showFullData, alreadyOutput);
                    out << "error: value length of " << slen << " bytes is too small (" << 
                    	vr.getVRName() << " is " << streamLengthOfValue(vr) << ")" << endl;       
		    if (vm > 1) {
                        out << "       affected value[" << i << "]=\"" << s << "\"" << endl;
                    }
                }
                
                delete fields[i];
                fields[i] = NULL;
            }
            delete fields;
        }
    } else {
        Uint32 componentSize = len/vm;
        if (componentSize > vr.getMaxValueLength()) {
            printResult(out, stack, showFullData, alreadyOutput);
            out << "error: value length of " << componentSize << " bytes is too large (" <<
                vr.getVRName() << " is " << streamLengthOfValue(vr) << ")" << endl;       
        }
        if (componentSize < vr.getMinValueLength()) {
            printResult(out, stack, showFullData, alreadyOutput);
            out << "error: value length of " << componentSize << " bytes is too small (" << 
                vr.getVRName() << " is " << streamLengthOfValue(vr) << ")" << endl;       
        }
    }
    
    return 0;
}

static int checkitem(ostream & out, DcmItem *item,  DcmXfer& oxfer, 
	DcmStack& stack, BOOL showFullData)
{
    
    if (item == NULL) {
        return 0;
    }
    /*
    ** Step through each attribute and check it.
    */

    int count = item->card();
    for (int i=0; i<count; i++) {
    	DcmElement *elem = item->getElement(i);
    	BOOL alreadyOutput = FALSE;
    	
        stack.push(elem);
    	checkelem(out, elem, oxfer, stack, showFullData, alreadyOutput);
        stack.pop();
    	
    	if (elem->ident() == EVR_SQ) {
    	    DcmSequenceOfItems *seq = (DcmSequenceOfItems*)elem;
    	    int nitems = seq->card();
    	    for (int j=0; j<nitems; j++) {
    	        /* check each item.  an item is just another dataset */
    	        stack.push(seq);
    	    	checkitem(out, seq->getItem(j), oxfer, stack, showFullData);
    	    	stack.pop();
    	    }
    	}    
    }
    return 0;
}

static int dcmchk(ostream & out, const char* ifname, BOOL isDataset, 
		  E_TransferSyntax xfer, BOOL showFullData, BOOL loadMode)
{
    DcmFileStream myin(ifname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        fprintf(stderr, "dcmdump: cannot open file: %s\n", ifname);
        return 1;
    }

    DcmObject * ds = NULL;
    if (isDataset)
	ds = new DcmDataset();
    else
	ds = new DcmFileFormat();

    ds->transferInit();
    ds->read(myin, xfer, EGL_withGL );
    ds->transferEnd();

    if (ds->error() != EC_Normal) {
	fprintf(stderr, "dcmdump: error: %s: reading file: %s\n", 
		dcmErrorConditionToString(ds->error()), ifname);
    }

    if (loadMode) {
        ds->loadAllDataIntoMemory();
	if (ds->error() != EC_Normal)
	{
	    fprintf(stderr, "Error: %s: reading file: %s\n", 
		    dcmErrorConditionToString(ds->error()), ifname);
	    return 1;
	}
    }

    DcmStack stack;
    DcmXfer oxfer(EXS_Unknown);
    
    if (ds->ident() == EVR_fileFormat) {
        DcmFileFormat *dfile = (DcmFileFormat*)ds;
        
        oxfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;
        checkitem(out, dfile->getMetaInfo(), oxfer, stack, showFullData);
        
        oxfer = dfile->getDataset()->getOriginalXfer();
        checkitem(out, dfile->getDataset(),  oxfer, stack, showFullData);
    } else {
        oxfer = ((DcmDataset*)ds)->getOriginalXfer();
        checkitem(out, (DcmDataset*)ds, oxfer, stack, showFullData);
    }
    
    delete ds;

    return 0;
}


/*
** CVS/RCS Log:
** $Log: dcmchk.cc,v $
** Revision 1.1  1997-03-26 17:38:14  hewett
** Initial version.
**
**
*/

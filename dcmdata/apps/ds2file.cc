/*
**
** Author: Andrew Hewett    Created:  14-11-95
**
** Module: ds2file.cc
**
** Purpose:
** Convert a dicom dataset to a dicom file encoding
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-03-27 15:47:25 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/Attic/ds2file.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"


// ********************************************

static void
usage()
{
    fprintf(stderr, 
	   "ds2file: convert dicom dataset to a dicom file\n"
	   "usage: ds2file [options] dcm-dataset dcm-file\n"
	   "options are:\n"
	   "  input transfer syntax:\n"
	   "    -t=   try and discover input transfer syntax (can fail)\n"
	   "    -ti   input is little-endian implicit transfer syntax (default)\n"
	   "    -te   input is little-endian explicit transfer syntax\n"
	   "    -tb   input is big-endian explicit transfer syntax\n"
	   "  group length encoding:\n" 
	   "    +g    write with group lengths (default)\n"
	   "    -g    write without group lengths\n"
	   "  length encoding in sequences and items:\n"
	   "    +e    write with explicit lengths (default)\n"
	   "    -e    write with undefined lengths\n"
	   "  output transfer syntax:\n"
	   "    +t=   write with same transfer syntax as input (default)\n"
	   "    +ti   write with little-endian implicit transfer syntax\n"
	   "    +te   write with little-endian explicit transfer syntax\n"
	   "    +tb   write with big-endian explicit transfer syntax\n"
	   "  other test/debug options:\n"
	   "    -u    disable generation of unknown VR (UN)\n"
	   "    +V    verbose mode, print actions\n"
	   "    +v    validate input data (currently almost useless)\n"
	   "    +dn   set debug level to n (n=1..9)\n");
}


// ********************************************

static void
printstack( DcmStack &stack, FILE* f = stdout)
{
    DcmObject *obj;
    int i = (int)stack.card();
    while ( i > 0 )
    {
	obj = stack.elem( i-1 );

        fprintf(f, " (%d) p=%p (%4.4hx,%4.4hx) vr(%s/%d) err=%d \"%s\"\n",
		i-1, obj, obj->getGTag(), obj->getETag(), 
		DcmVR( obj->getVR() ).getVRName(),
                obj->ident(), obj->error(), obj->getTag().getTagName() );
	i--;
    }
}


// ********************************************

static void
verify(DcmDataset& dcmds, BOOL verbosemode, FILE* f)
{
    E_Condition cond = dcmds.verify( TRUE );
    if (verbosemode) {
	fprintf(f,"verify claims: %s\n", dcmErrorConditionToString(cond));
    }

    DcmStack stk;
    dcmds.searchErrors( stk );

    if (stk.card() > 0) {
	fprintf(f,"verify: errors:\n");
	printstack(stk, f);
    }
}

// ********************************************

int main(int argc, char *argv[])
{
#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv, "ds2file");

    if (argc < 3) {
	usage();
        return 1;
    }

    char*            ifname = (char*)NULL;
    char*            ofname = (char*)NULL;
    E_EncodingType   enctype = EET_ExplicitLength;
    E_GrpLenEncoding ogltype = EGL_withGL;
    E_TransferSyntax xfer_in = EXS_LittleEndianImplicit;
    E_TransferSyntax xfer_out = EXS_Unknown;
    BOOL verifymode = FALSE;
    BOOL verbosemode = FALSE;
    int localDebugLevel = 0;

    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		fprintf(stderr, "unknown argument: %s\n", arg);
		usage();
		return 1;
	    }
	    switch (arg[1]) {
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
		case '=':
		    if (arg[0] == '-') xfer_in = EXS_Unknown;
		    else xfer_out = EXS_Unknown;
		    break;
		case 'i':
		    if (arg[0] == '-') xfer_in = EXS_LittleEndianImplicit;
		    else xfer_out = EXS_LittleEndianImplicit;
		    break;
		case 'e':
		    if (arg[0] == '-') xfer_in = EXS_LittleEndianExplicit;
		    else xfer_out = EXS_LittleEndianExplicit;
		    break;
		case 'b':
		    if (arg[0] == '-') xfer_in = EXS_BigEndianExplicit;
		    else xfer_out = EXS_BigEndianExplicit;
		    break;
		default:
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
	    case 'v':
		if (arg[0] == '+' && arg[2] == '\0') {
		    verifymode = TRUE;
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
	} else if ( ifname == NULL ) {
	    ifname = arg;
	} else if ( ofname == NULL ) {
	    ofname = arg;
	} else {
	    fprintf(stderr, "too many arguments: %s\n", arg);
	    usage();
	    return 1;
	}
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }
    
    SetDebugLevel(( localDebugLevel ));

    if (verbosemode) {
	printf("reading %s\n", ifname);
    }
    DcmFileStream inf( ifname, DCM_ReadMode );
    if ( inf.Fail() ) {
        fprintf(stderr, "cannot open file: %s\n", ifname);
        return 1;
    }

    DcmDataset dcmds;
	dcmds.transferInit();
    dcmds.read(inf, xfer_in, EGL_withGL );
	dcmds.transferEnd();

    if (dcmds.error() != EC_Normal) {
	fprintf(stderr, "Error: %s: reading dataset: %s\n", 
		dcmErrorConditionToString(dcmds.error()), ifname);
	return 1;
    }

    if (verifymode) {
	if (verbosemode) {
	    printf("verifying input dataset\n");
	}
	verify(dcmds, verbosemode, stdout);
    }

    DcmFileFormat dcmff(&dcmds);

    /* write out new file */
    if (verbosemode) {
	printf("writing %s\n", ofname);
    }

    DcmFileStream outf( ofname, DCM_WriteMode );
    if ( outf.Fail() ) {
        fprintf(stderr, "cannot create file: %s\n", ofname);
        return 1;
    }

    if (xfer_out == EXS_Unknown) {
	/* use the same as the input */
	xfer_out = dcmds.getOriginalXfer();
    }

    dcmff.transferInit();
	dcmff.write( outf, xfer_out, enctype, ogltype );
	dcmff.transferEnd();

    if (dcmff.error() != EC_Normal) {
	fprintf(stderr, "Error: %s: writing file: %s\n", 
		dcmErrorConditionToString(dcmff.error()), ifname);
	return 1;
    }

    return 0;
}


/*
** CVS/RCS Log:
** $Log: ds2file.cc,v $
** Revision 1.6  1997-03-27 15:47:25  hewett
** Added command line switche to allow generation of UN to be
** disabled (it is enabled by default).
**
** Revision 1.5  1996/09/24 16:13:50  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.4  1996/03/12 15:11:38  hewett
** Added call to prepareCmdLineArgs to enable command line arguments
** in environments which do not provide them.
**
** Revision 1.3  1996/01/05 13:29:36  andreas
** - new streaming facilities
** - unique read/write methods for block and file transfer
** - more cleanups
**
** Revision 1.2  1995/11/23 17:10:33  hewett
** Updated for loadable data dictionary.
**
**
*/


/*
**
** Author: Andrew Hewett    Created:  14-11-95
**
** Module: dcmconv.cc
**
** Purpose:
** Convert dicom file encoding
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-09 11:05:59 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmconv.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/
                        

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dctk.h"
#include "dcdebug.h"

// ********************************************

static void
usage()
{
    fprintf(stderr, 
	   "dcmconv: convert dicom file encoding\n"
	   "usage: dcmconv [options] dcmfile-in dcmfile-out\n"
	   "options are:\n"
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
	   "    +V    verbose mode, print actions\n"
	   "    +l    force load of all input data into memory\n"
	   "    +v    validate input data (currently almost useless)\n"
	   "    +c    copy all input data before writing output\n"
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
verify(DcmFileFormat& dcmff, BOOL verbosemode, FILE* f)
{
    E_Condition cond = dcmff.verify( TRUE );
    if (verbosemode) {
	fprintf(f, "verify claims: %s\n", dcmErrorConditionToString(cond));
    }

    DcmStack stk;
    dcmff.searchErrors( stk );

    if (stk.card() > 0) {
	fprintf(f,"verify: errors:\n");
	printstack(stk, f);
    }
}

// ********************************************


int main(int argc, char *argv[])
{
    SetDebugLevel(( 0 ));

    
   if (argc < 3) {
		usage();
        return 1;
    }

    const char*	ifname = NULL;
    const char*	ofname = NULL;
    E_TransferSyntax xfer = EXS_Unknown;
    E_EncodingType enctype = EET_ExplicitLength;
    E_GrpLenEncoding ogltype = EGL_withGL;
    BOOL copymode = FALSE;
    BOOL loadmode = FALSE;
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
			  case 'l':
				if (arg[0] == '+' && arg[2] == '\0') {
					loadmode = TRUE;
				} else {
					fprintf(stderr, "unknown option: %s\n", arg);
					return 1;
				}
				break;
			  case 't':
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
				break;
			  case 'v':
				if (arg[0] == '+' && arg[2] == '\0') {
					verifymode = TRUE;
				} else {
					fprintf(stderr, "unknown option: %s\n", arg);
					return 1;
				}
				break;
			  case 'c':
				if (arg[0] == '+' && arg[2] == '\0') {
					copymode = TRUE;
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
    DcmFileStream inf(ifname, DCM_ReadMode);
    if ( inf.Fail() ) {
        fprintf(stderr, "cannot open file: %s\n", ifname);
        return 1;
    }

    DcmFileFormat *dcmff = new DcmFileFormat();
	dcmff->transferInit();
    dcmff->read(inf, EXS_Unknown, EGL_withGL );
	dcmff->transferEnd();

    if (dcmff->error() != EC_Normal) {
		fprintf(stderr, "Error: %s: reading file: %s\n", 
				dcmErrorConditionToString(dcmff->error()), ifname);
		return 1;
    }

    if (loadmode) {
		if (verbosemode) {
			printf("loading all data into memory\n");
		}
        dcmff->loadAllDataIntoMemory();
		if (dcmff-> error() != EC_Normal)
		{
			fprintf(stderr, "Error: %s: reading file: %s\n", 
					dcmErrorConditionToString(dcmff->error()), ifname);
			return 1;
		}
	}

	if (verifymode) {
		if (verbosemode) {
			printf("verifying input file data\n");
		}
		verify(*dcmff, verbosemode, stdout);
	}

	DcmFileFormat* ff = dcmff;
	if ( copymode ) {
		if (verbosemode) {
			printf("copying FileFormat data\n");
		}
		ff = new DcmFileFormat( *dcmff );
		delete dcmff;

		if (verifymode) {
			if (verbosemode) {
				printf("verifying copied file data\n");
			}
			verify(*ff, verbosemode, stdout);
		}
	}

	/* write out new file */
	if (verbosemode) {
		printf("writing %s\n", ofname);
	}

	DcmFileStream outf( ofname, DCM_WriteMode );
	if ( outf.Fail() ) {
		fprintf(stderr, "cannot create file: %s\n", ofname);
		return 1;
	}

	if (xfer == EXS_Unknown) {
		/* use the same as the input */
		xfer = ff->getDataset()->getOriginalXfer();
	}

	ff->transferInit();
	ff->write( outf, xfer, enctype, ogltype );
	ff->transferEnd();

	if (verifymode) {
		if (verbosemode) {
			printf("verifying output file data\n");
		}
		verify(*ff, verbosemode, stdout);
	}

	delete ff;

	return 0;
}


/*
** CVS/RCS Log:
** $Log: dcmconv.cc,v $
** Revision 1.3  1996-01-09 11:05:59  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Tags
**
** Revision 1.2  1996/01/05 13:29:34  andreas
** - new streaming facilities
** - unique read/write methods for block and file transfer
** - more cleanups
**
** Revision 1.1  1995/11/23 17:10:30  hewett
** Updated for loadable data dictionary.
**
**
*/


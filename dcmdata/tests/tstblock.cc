/*
**
** Author: Andrew Hewett    Created:  14-11-95
**
** Module: dcmconv.cc
**
** Purpose:
** Test the block read/write code
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1995-11-28 10:46:15 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/tests/Attic/tstblock.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <stream.h>
#include "dcdatset.h"
#include "dcdict.h"
#include "dcdebug.h"

// ********************************************


static BOOL verbosemode = FALSE;


// ********************************************


static void
usage()
{
    fprintf(stderr, 
	   "tstblock: test block read/write operations\n"
	   "usage: dcmconv [options] dataset-in dataset-out\n"
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
	   "    +bn   use block size n (default 1024)\n"
	   "    +V    verbose mode, print actions\n"
	   "    +dn   set debug level to n (n=1..9)\n");
}


// ********************************************


E_Condition readObject( DcmDataset **dset,
                        const char *filename,
			int blockSize,
			E_TransferSyntax xfer,
                        E_GrpLenEncoding igltype )
{
    long buflen = blockSize;
    long packetlen = blockSize;
    unsigned long bytesread = 0;

    FILE* f = NULL;
    if ((f = fopen(filename, "rb")) == NULL) {
	fprintf(stderr, "cannot open: %s\n", filename);
	return EC_InvalidStream;
    }

    int readlen = 0;
    E_Condition econd = EC_Normal;

    iDicomStream *streambuf = new iDicomStream( buflen );

    /* attach streambuf to dset */
    *dset = new DcmDataset( streambuf );

    econd = (*dset)->readBlockInit();

    char *buffer = new char[ packetlen ];

    do {
	readlen = fread(buffer, sizeof(char), packetlen, f); 

	if (verbosemode) 
	    putchar('.');

	if (readlen > 0) {
	    econd = streambuf->fillBuffer(buffer, readlen);
	    if (econd == EC_BufferFull) {
		delete buffer;
		return econd;
	    }
	}

	if (feof(f)) {
	    streambuf->markBufferEOF();
	}

	econd = (*dset)->readBlock(xfer, igltype);

	bytesread += readlen;

    } while (!ferror(f) && !feof(f));

    fclose(f);
    
    delete buffer;

    if (verbosemode) {
	printf("(%ld bytes)\n", bytesread);
    }

    return econd;

}


// ********************************************


E_Condition writeObject( DcmDataset& dset,
			 const char *filename,
			 int blockSize,
			 E_TransferSyntax xfer,
                         E_EncodingType enctype,
                         E_GrpLenEncoding ogltype )
{
    long buflen = blockSize;
    long packetlen = blockSize;
    E_Condition econd = EC_Normal;
    unsigned long readlen;
    unsigned long byteswritten = 0;

    FILE* f = NULL;
    if ((f = fopen(filename, "wb")) == NULL) {
	fprintf(stderr, "cannot create: %s\n", filename);
	return EC_InvalidStream;
    }

    oDicomStream streambuf(buflen);

    econd = dset.writeBlockInit();

    char *buffer = new char[ packetlen ];

    BOOL last = FALSE;
    BOOL allBlocksInStream = FALSE;

    E_Condition bufcond = EC_Normal;

    while (!last) {
	
	if (!allBlocksInStream) {
	    econd = dset.writeBlock(streambuf, xfer, enctype, ogltype);
	    if ( econd == EC_Normal ) {
		/* last block written to stream buffer */
		allBlocksInStream = TRUE;
		streambuf.markBufferEOF();
	    } else if (econd == EC_InvalidStream) {
		/* ok, not really invalid, just no more space in streambuf */
	    } else {
		/* some error has occurred */
		delete buffer;
		return econd;
	    }
	}

	bufcond = streambuf.readBuffer(buffer, packetlen, &readlen);
	if (bufcond != EC_Normal) {
	    if (bufcond == EC_EndOfBuffer) {
		last = TRUE;
	    } else {
		fprintf(stderr, "readBuffer failed\n");
		delete buffer;
		return bufcond;
	    }
	}

	int writelen = fwrite(buffer, sizeof(char), readlen, f);
	if (writelen != (long)readlen) {
	    perror("fwrite");
	    delete buffer;
	    return EC_InvalidStream;
	}

	if (verbosemode) {
	    putchar('.');
	}

	byteswritten += writelen;

    }

    delete buffer;

    if (verbosemode) {
	printf("(%ld bytes)\n", byteswritten);
    }
    return econd;
}


// ********************************************


int main(int argc, char *argv[])
{
#ifdef HAVE_LIBIOSTREAM
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
#endif

    if (argc < 3) {
	usage();
        return 1;
    }

    const char*	ifname = NULL;
    const char*	ofname = NULL;
    E_TransferSyntax xfer_in = EXS_LittleEndianImplicit;
    E_TransferSyntax xfer_out = EXS_UNKNOWN;
    E_EncodingType enctype = EET_ExplicitLength;
    E_GrpLenEncoding ogltype = EGL_withGL;
    int localDebugLevel = 0;
    int blockSize = 1024;

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
		    if (arg[0] == '-') xfer_in = EXS_UNKNOWN;
		    else xfer_out = EXS_UNKNOWN;
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
	    case 'b':
		if (sscanf(arg+2, "%d", &blockSize) != 1) {
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

    if (ifname == NULL) {
	fprintf(stderr, "missing input file\n");
	usage();
	return 1;
    }
    if (ofname == NULL) {
	fprintf(stderr, "missing output file\n");
	usage();
	return 1;
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }
    
    SetDebugLevel(( localDebugLevel ));

    DcmDataset *dset = NULL;

    E_Condition econd;

    if (verbosemode) {
	printf("block reading; %s\n", ifname);
    }
    econd = readObject( &dset, ifname, blockSize, xfer_in, EGL_withGL );

    if (econd != EC_Normal) {
	fprintf(stderr, "Error: %s: block reading: %s\n", 
		dcmErrorConditionToString(econd), ifname);
	return 1;
    }
    
    if (verbosemode) {
	printf("block writing: %s\n", ofname);
    }

    econd = writeObject( *dset, ofname, blockSize, xfer_out, enctype, ogltype );

    if (econd != EC_Normal) {
	fprintf(stderr, "Error: %s: block writing: %s\n", 
		dcmErrorConditionToString(econd), ifname);
	return 1;
    }

    delete dset;

    return 0;
}

/*
**
** $Log: tstblock.cc,v $
** Revision 1.4  1995-11-28 10:46:15  hewett
** Added calls to readInitBlock() and writeInitBlock().
**
**
*/

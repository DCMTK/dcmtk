/*
 *
 * Author: Gerd Ehlers      Created:  04-17-94
 *                          Modified: 02-07-95
 *
 * Module: convdir.cc
 *
 * Purpose:
 * 	convert and verify dicom directories
 *
 * Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dctk.h"
#include "dcdicdir.h"
#include "dcvr.h"
#include "dcdebug.h"

// ********************************************


#if 0
#ifdef DEBUG
extern int debugLevel;


void operator delete( void *name )
{
    if ( 1 <= debugLevel )
    {
        printf("*** %p deleted\n", name );
    }
    free( name );
}


// ********************************************


void* operator new( size_t size )
{
    void* place = malloc( size );
    if ( 1 <= debugLevel )
    {
        printf("*** %p  [%ld] allocated\n", place, size );
    }
    return place;
}


#endif  // DEBUG
#endif


// ********************************************


void printstack( DcmStack &stack )
{
Bdebug((5, "tstdir:printstack(DcmStack&)" ));

    cout << "#Stack Frame:" << endl;
    DcmObject *obj;
    int i = (int)stack.card();
    while ( i > 0 )
    {
	obj = stack.elem( i-1 );

	printf(" (%d) p=%p (%4.4hx,%4.4hx) vr(%s/%d) err=%d \"%s\"",
		    i-1,
		    obj,
		    obj->getGTag(),
		    obj->getETag(),
		    DcmVR( obj->getVR() ).getVRName(),
                    obj->ident(),
		    obj->error(),
                    obj->getTag().getTagName() );
	cout << endl;
	i--;
    }
Edebug(());

}


// ********************************************


E_Condition search( DcmObject* pobj, DcmTagKey xtag )
{
Bdebug((1, "tstdir:search(pobj*,xtag=(%x,%x))", xtag.getGroup(), xtag.getElement() ));

    E_Condition l_error = EC_Normal;
    DcmTag s_tag( xtag );
    DcmStack stack;
    int count = 0;

    while ( (l_error=pobj->search( s_tag, stack, ESM_afterStackTop, TRUE ))
	    == EC_Normal )
    {
	count++;
        if ( stack.top() == (DcmObject*)NULL )
        {
            fprintf(stderr, "Error: internal error in method search(): %d. loop\n", count);
            break;
        }
debug(( 1, "Search succeeded" ));

	cerr << "Found " << dec << count;
	cerr << ". tag=(" << hex << stack.top()->getGTag() << ",";
        cerr <<              hex << stack.top()->getETag() << ") at p=";
	cerr << hex << stack.top() << " : ";
        cerr << stack.top()->getTag().getTagName() << dec << endl;

        cout << "# Stack returned after " << count << ". search:" << endl;
	printstack( stack );
    }
Vdebug((1, l_error == EC_IllegalCall, "Search returned IllegalCall" ));
Vdebug((1, l_error != EC_Normal
	   && l_error != EC_TagNotFound
	   && l_error != EC_IllegalCall, "Search returned error=%d", l_error ));

    cout << "# Stack returned after all searches:" << endl;
    printstack( stack );
Edebug(());

    return l_error;
}


// ********************************************


char *pathToDicom( char *name, char* newname )
{
    if ( name != (char*)NULL && newname != (char*)NULL )
    {
	char *s = name;
	char *t = newname;
	while ( *s )
	{
	    if ( *s == '/' )
		*t++ = '\\';
	    else
		*t++ = *s;
	    s++;
	}
	*t = *s;
    }
    return newname;
}


// ********************************************


int main(int argc, char *argv[])
{
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
    SetDebugLevel(( 0 ));

    if (argc <= 1)
    {
        cerr << endl
        <<
  "usage: tstdir DICOMDIR [-s xtag] [-a new-dcmfile] [-remove]\n"
  "              [-v] [-z] [-w [enctype]] [-d level]\n"
  "    List DICOMDIR\n"
  "    -s xtag:                        search Tag xtag\n"
  "    -a new-dcmfile:                 add a new DicomFileFormat\n"
  "    -remove:                        remove first record with sub and files\n"
  "    -v:                             verify and correct DICOMDIR\n"
  "    -z:                             super verify DICOMDIR\n"
  "    -w [enctype]:                   write it back\n"
  "         enctype=(ExplicitLen,UndefLen)=(0,1)   def=0\n"
  "    -d level:                       set debuglevel\n"
  "         level=(0..9)                           def=0\n"
        ;
	return 1;
    }

    char*	     ifname = NULL;
    char*	     addfname = NULL;
    E_EncodingType   enctype = EET_ExplicitLength;
    DcmTagKey	     xtag = DCM_BitsAllocated;
    BOOL	     searchmode = FALSE;
    BOOL	     addmode = FALSE;
    BOOL             removemode = FALSE;
    BOOL	     verifymode = FALSE;
    BOOL             superverifymode = FALSE;
    BOOL	     writemode = FALSE;
    int              localDebugLevel = 0;
    int 	     arg_offset = 0;

    if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
    {
        arg_offset++;
        ifname = argv[1];
    }
    else
        ifname = (char*)NULL;
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-s" ) == 0 )
    {
	arg_offset++;
	if ( argc >= 2+arg_offset && *argv[1+arg_offset] != '-' )
	{
            searchmode = TRUE;
	    unsigned int g, e;
	    if (sscanf(argv[1+arg_offset], "(%x,%x)", &g, &e) == 2) {
		xtag.set(g, e);
	    } else {
		cerr << argv[0] << ": invalid search tag: " << argv[1+arg_offset]
		     << endl;
		cerr << "expected format: (gggg,eeee)" << endl;
		return 1;
	    }
	    arg_offset++;
	}
    }
    if (    argc >= 2+arg_offset
	 && strcmp( argv[1+arg_offset], "-a" ) == 0 )
    {
	arg_offset++;
	if ( argc >= 2+arg_offset && *argv[1+arg_offset] != '-' )
	{
	    addmode = TRUE;
	    addfname = argv[1+arg_offset];
	    arg_offset++;
	}
    }
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-remove" ) == 0 )
    {
	arg_offset++;
        removemode = TRUE;
    }
    if (    argc >= 2+arg_offset
	 && strcmp( argv[1+arg_offset], "-v" ) == 0 )
    {
	arg_offset++;
	verifymode = TRUE;
    }
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-z" ) == 0 )
    {
	arg_offset++;
        superverifymode = TRUE;
    }
    if (    argc >= 2+arg_offset
	 && strcmp( argv[1+arg_offset], "-w" ) == 0 )
    {
	writemode = TRUE;
	arg_offset++;
	if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
	{
	    enctype = (E_EncodingType)atoi( argv[1+arg_offset] );
	    arg_offset++;
	}
    }
    if (    argc >= 2+arg_offset
	 && strcmp( argv[1+arg_offset], "-d" ) == 0 )
    {
	arg_offset++;
	if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
	{
	    localDebugLevel = (E_TransferSyntax)atoi( argv[1+arg_offset] );
	    arg_offset++;
	}
    }
    cerr << argv[0] << ":reading and printing " << ifname << endl;
    if ( searchmode )
        cerr << "and searching for xtag=" << xtag << endl;
    if ( addmode )
	cerr << "and adding " << addfname << endl;
    if ( removemode )
        cerr << "and removing first record with lower-level-records and"
                " referenced files" << endl;
    if ( verifymode )
        cerr << "and verifying and correcting DICOMDIR " << endl;
    if ( superverifymode )
        cerr << "and verifying DICOMDIR with special function" << endl;
    if ( writemode )
    {
	cerr << "and writing " << ifname;
        cerr << " with enctype=" << enctype << " back to disk" << endl;
    }
    cerr << "DebugLevel is " << localDebugLevel << endl;
    
    SetDebugLevel(( localDebugLevel ));

Bdebug((1, "tstdir:main()" ));

    DcmDicomDir *dir = new DcmDicomDir( ifname, "2. EXAMPLE" );

    if ( searchmode )
    {
        search( &dir->getRootRecord(), xtag );
        search( &dir->getMRDRSequence(), xtag );
    }

    if ( addmode )
    {
	char *newfname = new char[ strlen( addfname ) + 1 ];
	pathToDicom( addfname, newfname );

    // 1. Moeglichkeit:
	DcmDirectoryRecord *newDR;
        DcmDirectoryRecord *refMRDR = dir->matchOrCreateMRDR( newfname );
        if ( refMRDR != (DcmDirectoryRecord*)NULL )
        {
            newDR = new DcmDirectoryRecord( ERT_Private, "" );
            newDR->assignToMRDR( refMRDR );
        }
        else
            newDR = new DcmDirectoryRecord( ERT_Private, newfname );
        dir->getRootRecord().insertSub( newDR );

    // 2. Moeglichkeit (ineffizienter, da zweifaches Parsing von newfname):
    //  DcmDirectoryRecord *newDR = new DcmDirectoryRecord( ERT_Private,
    //                                                      newfname );
    //  DcmDirectoryRecord *refMRDR = dir->matchOrCreateMRDR( newfname );
    //  newDR->assignToMRDR( refMRDR );
    //  dir->getRootRecord().insertSub( newDR );
    //
        delete newfname;
        newfname = (char*)NULL;
	fflush( stdout );
	cout << endl;
	cout << "#----------------------------------------------------" << endl;
	cout << "# new DirectoryRecord:" << endl;
	newDR->print();
	cout << "#----------------------------------------------------" << endl;
	cout << flush;
    }

    if ( removemode )
    {
        dir->getRootRecord().deleteSubAndPurgeFile( (T_VR_UL)0 );
        writemode = TRUE;                         // forciere das Abspeichern

	fflush( stdout );
	cout << endl;
	cout << "#----------------------------------------------------" << endl;
        cout << "# DicomDir after removing records:" << endl;
	dir->print();
	cout << "#----------------------------------------------------" << endl;
	cout << flush;
    }

    fflush( stdout );
    cout << endl;
    cout << "#----------------------------------------------------" << endl;
    cout << "# DicomDir after reading and possibly inserting new DirectoryRecord";
    cout << endl;
    dir->print();
    cout << "#----------------------------------------------------" << endl;
    cout << flush;

    if ( verifymode )
    {
        dir->verify( TRUE );
        DcmStack stk, stk1;
        dir->getRootRecord().searchErrors( stk );
        dir->getMRDRSequence().searchErrors( stk1 );
	fflush( stdout );
	cout << flush;

	cout << "#----------------------------------------------------" << endl;
	cout << "# Collected errors after verifying RootRecord:" << endl;
	printstack( stk );
	cout << "#----------------------------------------------------" << endl;
        printstack( stk1 );
	cout << "#----------------------------------------------------" << endl;
    }

    if ( superverifymode )
    {
        dir->getRootRecord().verify( TRUE );
        dir->getMRDRSequence().verify( TRUE );
        DcmStack stk, stk1;
        dir->getRootRecord().searchErrors( stk );
        dir->getMRDRSequence().searchErrors( stk1 );
	fflush( stdout );
	cout << flush;

	cout << "#----------------------------------------------------" << endl;
        cout << "# Collected errors after super verifying RootRecord and MRDRs:" << endl;
	printstack( stk );
	cout << "#----------------------------------------------------" << endl;
        printstack( stk1 );
	cout << "#----------------------------------------------------" << endl;
        cout << "# DicomDir after super verifying:" << endl;
	dir->print();
	cout << "#----------------------------------------------------" << endl;
    }

    if ( writemode )
    {
        dir->write( EXS_LittleEndianExplicit, enctype, EGL_withoutGL );

	fflush( stdout );
	cout << endl;
	cout << "#----------------------------------------------------" << endl;
	cout << "# DicomDir after writing:" << endl;
	dir->print();
	cout << "#----------------------------------------------------" << endl;
	cout << flush;
    }
    delete dir;

Edebug(());

    return 0;
}


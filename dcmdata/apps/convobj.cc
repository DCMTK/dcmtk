/*
 *
 * Author: Gerd Ehlers      Created:  04-17-94
 *                          Modified: 02-07-95
 *
 * Module: tstobj.cc
 *
 * Purpose:
 * 	convert Dicom file object into different transfer syntaxes
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "dctk.h"
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
    // free( name );
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
Bdebug((5, "tstobj:printstack(DcmStack&)" ));

    fprintf(stderr, "#Stack Frame:\n");
    DcmObject *obj;
    int i = (int)stack.card();
    while ( i > 0 )
    {
	obj = stack.elem( i-1 );

        printf(" (%d) p=%p (%4.4hx,%4.4hx) vr(%s/%d) err=%d \"%s\"\n",
		    i-1,
		    obj,
		    obj->getGTag(),
		    obj->getETag(),
		    DcmVR( obj->getVR() ).getVRName(),
                    obj->ident(),
		    obj->error(),
                    obj->getTag().getTagName() );
	i--;
    }
Edebug(());

}


// ********************************************


E_Condition search( DcmObject* pobj, ETag xtag )
{
Bdebug((5, "tstobj:search(pobj*,xtag=%d)", xtag ));

    E_Condition l_error = EC_Normal;
    DcmTag s_tag( xtag );
    DcmStack stack;
    int count = 0;

    while ( (l_error=pobj->search( s_tag, stack, ESM_afterStackTop, TRUE ))
	    == EC_Normal )
    {
debug(( 1, "Search succeeded" ));

	count++;
	printf("Found %d. tag=( %x, %x ) : %s\n", 
		   count, stack.top()->getGTag(), stack.top()->getETag(), 
		   stack.top()->getTag().getTagName());

#ifdef DEBUG
    fprintf(stderr, "# Stack returned after %d. search:\n", count);
	printstack( stack );
#endif
    }
Vdebug((1, l_error == EC_IllegalCall, "Search returned IllegalCall" ));
Vdebug((1, l_error != EC_Normal
	   && l_error != EC_TagNotFound
	   && l_error != EC_IllegalCall, "Search returned error=%d", l_error ));

#ifdef DEBUG
    fprintf(stderr, "# Stack returned after all searches:\n");
    printstack( stack );
#endif
Edebug(());

    return l_error;
}


// ********************************************


int main(int argc, char *argv[])
{
    SetDebugLevel(( 0 ));

    if (argc <= 1)
    {
        printf( "\n"
  "convert Dicom files into differnt transfer syntaxes \n"
  "usage: tstobj dcmfile-in [igltype] [-v] [-c] [-s xtag]\n"
  "              [-w dcmfile-out [xfer [enctype [ogltype]]]] [-d level]\n"
  "    read in DICOM-data and print contents to stdout\n"
  "         igltype=(withoutGL,withGL)=(0,1)      def=0\n"
  "    -l:                                    load complete FileFormat\n"
  "    -v:                                    verify and validate Format\n"
  "    -c:                                    copy FileFormat\n"
  "    -s xtag:                               search Tag xtag\n"
  "    -w dcmfile [xfer [enctype [ogltype]]]: write it back\n"
  "         xfer=(LI,LE,BE)=(0,2,3)               def=0\n"
  "         enctype=(ExplicitLen,UndefLen)=(0,1)  def=0\n"
  "         ogltype=(withoutGL,withGL)=(0,1)      def=0\n"
  "    -d level:                              set debuglevel\n"
  "         level=(0..9)                          def=0\n")
        ;
        return 1;
    }

    char*	     ifname = NULL;
    char*	     ofname = NULL;
    E_TransferSyntax xfer = EXS_LittleEndianImplicit;
    E_EncodingType   enctype = EET_ExplicitLength;
    E_GrpLenEncoding igltype = EGL_withoutGL;
    E_GrpLenEncoding ogltype = EGL_withoutGL;
    ETag	     xtag = ET_BitsAllocated0028;
    BOOL	     searchmode = FALSE;
    BOOL             copymode = FALSE;
    BOOL             loadmode = FALSE;
    BOOL             verifymode = FALSE;
    BOOL             writemode = FALSE;
    E_Condition      l_error = EC_Normal;
    int              localDebugLevel = 0;
    int              arg_offset = 0;

    if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
    {
        arg_offset++;
        ifname = argv[1];
    }
    else
        ifname = (char*)NULL;
    if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
    {
        igltype = (E_GrpLenEncoding)atoi( argv[1+arg_offset] );
        arg_offset++;
    }
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-l" ) == 0 )
    {
	arg_offset++;
        loadmode = TRUE;
    }
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-v" ) == 0 )
    {
	arg_offset++;
        verifymode = TRUE;
    }
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-c" ) == 0 )
    {
	arg_offset++;
        copymode = TRUE;
    }
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-s" ) == 0 )
    {
	arg_offset++;
	if ( argc >= 2+arg_offset && *argv[1+arg_offset] != '-' )
	{
            searchmode = TRUE;
            xtag = (ETag)atoi( argv[1+arg_offset] );
	    arg_offset++;
	}
    }
    if (    argc >= 2+arg_offset
	 && strcmp( argv[1+arg_offset], "-w" ) == 0 )
    {
	arg_offset++;
	if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
	{
            writemode = TRUE;
            ofname = argv[1+arg_offset];
	    arg_offset++;
	}
	if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
	{
	    xfer = (E_TransferSyntax)atoi( argv[1+arg_offset] );
	    arg_offset++;
	}
	if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
	{
	    enctype = (E_EncodingType)atoi( argv[1+arg_offset] );
	    arg_offset++;
	}
	if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
	{
            ogltype = (E_GrpLenEncoding)atoi( argv[1+arg_offset] );
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
    printf("%s:reading and printing %s", argv[0], ifname);
    printf(" with igltype= %d\n", igltype);
    if ( loadmode )
        printf("and loading complete FileFormat before doing any operation\n");
    if ( verifymode )
        printf("and verifying/validating Format \n");
    if ( copymode )
        printf("and copying entire FileFormat %s\n", ifname);
    if ( searchmode )
        printf("and searching for xtag= %d\n", xtag);
    if ( writemode && copymode )
    {
        printf("and writing copy to file %s", ofname);
        printf(" with xfer= %d", xfer);
        printf(", enctype= %d", enctype);
        printf(" and ogltype= %d\n", ogltype); 
    }
    if ( writemode && !copymode )
    {
        printf("and converting %s to ", ifname);
        printf("%s with xfer= %d", ofname, xfer);
        printf(", enctype= %d", enctype);
        printf(" and ogltype= %d\n", ogltype);
    }
    printf("DebugLevel is %d\n", localDebugLevel);
    
    SetDebugLevel(( localDebugLevel ));

    iDicomStream myin( ifname );
    if ( myin.fail() )
    {
        fprintf(stderr, "%s: file %s not found.\n", argv[0], ifname);
        return 1;
    }
Bdebug((1, "tstobj:main()" ));

    DcmObject *pobj = new DcmFileFormat( &myin );
    pobj->read( EXS_UNKNOWN, igltype );
    fflush( stdout );
    printf("# List contents of DICOM-File %s\n", ifname);
    pobj->print();
	fflush(stdout);

    if ( loadmode )
    {
        pobj->loadAllDataIntoMemory();
        fflush( stdout );
    }

    if ( verifymode )
    {
	BOOL boVeri = pobj->verify( TRUE );
        fflush( stdout );
	printf( "Verify returned with: ( %s ) after autocorrecting\n", boVeri ? "true" : "false");
	pobj->print();
    fflush(stdout);
    }

    if ( copymode )
    {
        printf("# copy FileFormat by Object-Reference and print it \n");
        DcmFileFormat ff( *pobj );
        ff.print();
        fflush(stdout);
        if ( writemode )
        {
            oDicomStream myout( ofname );
            ff.write( myout, xfer, enctype, ogltype );
            fflush(stdout);
            BOOL boVeri = ff.verify( FALSE );
            printf("Verify returned with: (%s) after copying and writing\n",
				   boVeri ? "true" : "false");
        }
        DcmStack stk;
        ff.searchErrors( stk );
        fflush( stdout );

#ifdef DEBUG
        fprintf(stderr, "# Collected errors:\n"); 
        printstack( stk );
        fflush(stderr);
#endif
    }

    if ( searchmode )
    {
	l_error = search( pobj, xtag );
    }

    if ( writemode && !copymode )
    {
        oDicomStream myout( ofname );
        pobj->write( myout, xfer, enctype, ogltype );
	fflush(stdout);
        DcmStack stk;
        pobj->searchErrors( stk );
        fflush( stdout );

#ifdef DEBUG
        fprintf( stderr,"# Collected errors after reading & writing complete Dataset: \n");
        printstack( stk );
#endif

        BOOL boVeri = pobj->verify( FALSE );
        printf("Verify returned with: (%s) after writing\n", boVeri ? "true" : "false");
	pobj->print();
    }
    DcmStack stk;
    pobj->searchErrors( stk );
    fflush( stdout );

#ifdef DEBUG
    fprintf(stderr, "# Collected errors after all operations:\n");
    printstack( stk );
#endif

    delete pobj;
Edebug(());

    return 0;
}


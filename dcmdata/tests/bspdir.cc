// bspdir.cc	04-17-94  until  02-15-95

#include "dctk.h"
#include "dcdebug.h"

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


int main()
{
    SetDebugLevel(( 0 ));

    DcmDicomDir *dir = new DcmDicomDir( "DICOMDIR", "1. EXAMPLE" );

    char* addfname = "PRIMITIV.DIO";
    char *newfname = new char[ strlen( addfname ) + 1 ];
    pathToDicom( addfname, newfname );
    DcmDirectoryRecord *newDR;
    DcmDirectoryRecord *refMRDR = dir->matchOrCreateMRDR( newfname );
    if ( refMRDR != (DcmDirectoryRecord*)NULL )
    {
        newDR = new DcmDirectoryRecord( ERT_Private, "" );
        newDR->assignToMRDR( refMRDR );
    }
    else
        newDR = new DcmDirectoryRecord( ERT_Private, newfname );

    delete newfname;
    dir->getRootRecord().insertSub( newDR );

    dir->write( EXS_LittleEndianExplicit, EET_ExplicitLength, EGL_withoutGL );

    delete dir;
    return 0;
}


// mknulldicomdir.cc
// Erzeuge ein leeres DICOM-Verzeichnis unter explizitem Einfuegen von
// Datenelementen. Die Klasse DcmDicomDir wird nicht verwendet.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream.h>

#include "dctk.h"
#include "dcdebug.h"

#ifndef HAVE_GETHOSTID
#ifdef HAVE_SYSINFO

#include <sys/systeminfo.h>
static long gethostid(void)
{
    char buf[128];
    if (sysinfo(SI_HW_SERIAL, buf, 128) == -1) {
       perror("sysinfo");
       exit(1);
    }
#ifdef HAVE_STRTOUL
    return(strtoul(buf, NULL, 0));
#else
	long result;
	sscanf(buf, "%ld", &result);
	return result;
#endif
}

#else // !HAVE_SYSINFO

static long gethostid(void) { return 4711; }   // workaround for MSDOS

#endif // !HAVE_SYSINFO

#else // HAVE_GETHOSTID

#ifndef HAVE_UNISTD_H
extern "C" {
int gethostid();
int getpid();
}
#endif

#endif // HAVE_GETHOSTID

#define OFFIS_UID_ROOT "1.2.276.0.7230010.100"
#define THIS_UID_INFIX "957"


void makeUID(DcmUniqueIdentifier & ui);


int main(int argc, char *argv[])
{
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
    SetDebugLevel(( 0 ));

    char* ofname = "DICOMDIR";
    char* fileSetID = "EXAMPLE";
    E_TransferSyntax xfer = EXS_LittleEndianExplicit;
    E_EncodingType   enctype = EET_ExplicitLength;
    DcmFileFormat* pobj = new DcmFileFormat();
    DcmMetaInfo* metaInfo = NULL;
    DcmTag tag(0,0);

    /* parse cmd line */
    if (argc != 3) {
        cerr << "usage: " << argv[0] << " File-set-ID DICOMDIR\n";
        exit(0);
    }
 
    fileSetID = argv[1];
    ofname = argv[2];

    /* create a minimal DICOMDIR data set */
    DcmDataset* dataSet = pobj->getDataset();
    DcmUnsignedLongOffset *uloP;
    DcmUnsignedShort *usP;
    DcmCodeString *csP;

    DcmTag fileIDTag( ET_FilesetID );
    csP = new DcmCodeString( fileIDTag );
    csP->put( fileSetID );
    dataSet->insert(csP, TRUE );
    
    DcmTag firstRecTag( ET_OffsetoftheFirstDirectoryRecordoftheRootDirectoryEntity );
    uloP = new DcmUnsignedLongOffset( firstRecTag );
    uloP->put( 0L );
    dataSet->insert( uloP, TRUE );

    DcmTag lastRecTag( ET_OffsetoftheLastDirectoryRecordoftheRootDirectoryEntity );
    uloP = new DcmUnsignedLongOffset( lastRecTag );
    uloP->put( 0L );
    dataSet->insert( uloP, TRUE );

    DcmTag fileConsTag( ET_FilesetConsistencyFlag );
    usP = new DcmUnsignedShort( fileConsTag );
    usP->put( 0 );
    dataSet->insert( usP, TRUE );

    DcmTag dirSeqTag( ET_DirectoryRecordSequence );
    DcmSequenceOfItems* dirRecSeq = new DcmSequenceOfItems( dirSeqTag );
    dataSet->insert( dirRecSeq, TRUE );

    /* set up the special things for the meta header */
    metaInfo = pobj->getMetaInfo();

    tag = ET_MediaStoredSOPClassUID;
    DcmUniqueIdentifier* mediaStoredSOPClassUID = new DcmUniqueIdentifier( tag );
    mediaStoredSOPClassUID->put( "1.2.840.10008.1.3.10" );
    metaInfo->insert( mediaStoredSOPClassUID, TRUE );


/*  // Wird von DcmFileFormat automatisch generiert, falls noch nicht vorhanden:

    tag = ET_MediaStoredSOPInstanceUID;
    DcmUniqueIdentifier* mediaStoredSOPInstanceUID =
                                      new DcmUniqueIdentifier( tag );
    makeUID( mediaStoredSOPInstanceUID );
    metaInfo->insert( mediaStoredSOPInstanceUID, TRUE );
*/

    /* create the basic meta header */
    pobj->validateMetaInfo( xfer );

#if 0
    // Die auskommentierten Programmzeilen sind nicht erforderlich, weil jetzt
    // die sopClassUID und sopInstanceUID bereits im MetaInformationHeader
    // vorhanden sind:

    /* when creating the meta-header the toolkit complains if
     * there is no SOP Class UID and SOP Instance UID in the data set
     * because these are needed to build the meta header
     */

    tag = ET_SOPClassUID;
    DcmUniqueIdentifier* sopClassUID = new DcmUniqueIdentifier( tag );
    sopClassUID->put( "1.2.840.10008.1.3.10" );
    dataSet->insert( sopClassUID, TRUE );

    tag = ET_SOPInstanceUID;
    DcmUniqueIdentifier* sopInstanceUID = new DcmUniqueIdentifier( tag );
    makeUID( *sopInstanceUID );
    dataSet->insert( sopInstanceUID, TRUE );

    
    /* create the basic meta header */
    pobj->validateMetaInfo( xfer );


    /* remote the SOP Class & Instance UIDs from data set */
    dataSet->remove( sopClassUID );
    dataSet->remove( sopInstanceUID );
#endif

    /* write out the DICOMDIR file */ 
    oDicomStream myout( ofname );
    E_Condition cond = pobj->write( myout, xfer, enctype );

    delete pobj;
    
    return 0;
}

/*  // Die folgende Funktion ist jetzt Teil der Methode
    // DcmFileFormat::validateMetaInfo():

void makeUID(DcmUniqueIdentifier & ui)
{
    char uid[1024];
    static long currentUID = 0;

    // time() get the seconds since 00:00:00 GMT,  Jan.  1,  1970 
    sprintf(uid, "%s.%s.%ld.%ld.%ld.%ld", OFFIS_UID_ROOT, THIS_UID_INFIX,
	gethostid(), getpid(), time(NULL), currentUID++);

    ui.put(uid);
}
*/

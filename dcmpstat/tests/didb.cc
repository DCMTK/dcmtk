/*
 *
 *  Copyright (C) 1998-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: imagectn
 *
 *  Author: Lutz Vorwerk
 *
 *  Purpose:
 *    testprogram which mark an instance as viewed, selects a study output 
 *    studyuid, selects a series and output seriesuid, selects an instance 
 *    and output instanceduid. Finally it delete a selected instance. 
 *
 *  Last Update:      $Author: vorwerk $
 *  Update Date:      $Date: 1999-01-12 17:02:22 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "imagedb.h"
#include "dbpriv.h"
#include "dbstore.h"
#include "dviface.h"
#include "osconfig.h"
#include "dctk.h"

#if defined windows
BEGIN_EXTERN_C
#define access my_access	// Workaround to make Visual C++ Compiler happy!
#include <io.h>
#undef access
END_EXTERN_C
#endif

void main()
{
/*
	
	char dbcommand[100];
DVInterface *db=new DVInterface("dbfolder");
while (true) {
	strcpy(dbcommand,"");
	cout << "Input: "; cin >> dbcommand;
	cout << "dbcommand: " << dbcommand << endl;
	if (strcmp(dbcommand,"lock")==0) {
if (db->lockDatabase()==EC_IllegalCall) cout << "lock error1 !!" << endl;
else
cout << "locked !!" << endl;
db->getNumberOfStudies();
	}
	else
	if (strcmp(dbcommand,"unlock")==0) {
if (db->unlockDatabase()==EC_IllegalCall) cout << "unlock error1 !!" << endl;
else
cout << "unlocked !!" << endl;
	}
else
	if (strcmp(dbcommand,"quit")==0) {
cout << "exiting" << endl;
break;
}
}
}
*/


DVInterface *db=new DVInterface("dbfolder");
if (db->lockDatabase()==EC_IllegalCall) cout << "lock error1 !!" << endl;
else
cout << "locked !!" << endl;
cout << db->getNumberOfStudies() << endl;
int eingabe;
cout << "Study ?";
cin >> eingabe;
if (db->selectStudy(eingabe)==EC_IllegalCall) cout << "study selection error !!" << endl;
else
cout << "study selected !!" << endl;
cout << db->getNumberOfSeries() << endl;
if (db->selectSeries(0)==EC_IllegalCall) cout << "series selection error !!" << endl;
else
cout << "series selected !!" << endl;
if ((db->isPresentationStateSeries())==OFFalse) cout << "PR only " << endl;
else
cout << "PR is not clean !!" << endl;
 
if (db->unlockDatabase()==EC_IllegalCall) cout << "unlock error1 !!" << endl;
else
cout << "unlocked !!" << endl;

// new section
cout << "---------------------" << endl;
if (db->lockDatabase()==EC_IllegalCall) cout << "lock error1 !!" << endl;
else
cout << "locked !!" << endl;
cout << db->getNumberOfStudies() << endl;
if (db->selectStudy(3)==EC_IllegalCall) cout << "study selection error !!" << endl;
else
cout << "study selected !!" << endl;
cout << db->getNumberOfSeries() << endl;
if (db->selectSeries(0)==EC_IllegalCall) cout << "series selection error !!" << endl;
else
cout << "series selected !!" << endl;
if ((db->isPresentationStateSeries())==OFFalse) cout << "PR only " << endl;
else
cout << "PR is not clean !!" << endl;
 
if (db->unlockDatabase()==EC_IllegalCall) cout << "unlock error1 !!" << endl;
else
cout << "unlocked !!" << endl;

cout << "---------------------" << endl;
// new section


if (db->lockDatabase()==EC_IllegalCall) cout << "lock error1 !!" << endl;
else
cout << "locked !!" << endl;
db->getNumberOfStudies();

if (db->instanceReviewed("1.3.46.670589.11.0.1.1996021610440010","1.3.46.670589.11.0.2.1996021610440010","1.3.46.670589.11.0.4.1996021610440010" )==EC_IllegalCall)
cout << "instance can not be reviewed !" << endl;
 else
	 cout << "reviewed" << endl;
if (db->unlockDatabase()==EC_IllegalCall) cout << "unlock error1 !!" << endl;
else
cout << "unlocked !!" << endl;

cout << "---------------------" << endl;
// new section

if (db->lockDatabase()==EC_IllegalCall) cout << "lock error1 !!" << endl;
else
cout << "locked !!" << endl;
cout << "Number of Studies: " << db->getNumberOfStudies() << endl;
if (db->selectStudy(3)==EC_IllegalCall) cout << "study selection error !!" << endl;
else
cout << "study selected !!" << endl;

cout << "Study UID: " << db->getStudyUID() << endl;
cout << "Number of Series: " << db->getNumberOfSeries() << endl;

if (db->selectSeries(0)==EC_IllegalCall) cout << "series selection error !!" << endl;
else
cout << "series selected !!" << endl;

cout << "Series UID: " << db->getSeriesUID() << endl;
db->getNumberOfInstances();
cout << "Number of Instances: " << db->getNumberOfInstances() << endl;
if (db->selectInstance(0)==EC_IllegalCall) cout << "instance selection error !!" << endl;
else
cout << "instance selected !!" << endl;
cout << "Instance UID: " << db->getInstanceUID() << endl;
//db->deleteStudy("1.3.46.670589.11.0.1.1996021610440014");
if (db->unlockDatabase()==EC_IllegalCall) cout << "unlock error1 !!" << endl;
else
cout << "unlocked !!" << endl;

delete db;
}

/*
 *  $Log: didb.cc,v $
 *  Revision 1.2  1999-01-12 17:02:22  vorwerk
 *  testroutines for seriesselection and locking for windows NT added
 *
 *  Revision 1.1  1998/12/23 09:53:51  vorwerk
 *   testprogram which mark an instance as viewed, selects a study output
 *   studyuid, selects a series and output seriesuid, selects an instance
 *   and output instanceduid. Finally it delete a selected instance.
 *
 *  Revision 1.1  1998/12/22 15:31:02  vorwerk
 *  testprogram which marks, selects and deletes Instances
 *
 *
 */







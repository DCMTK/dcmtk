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
 *  Update Date:      $Date: 1998-12-23 09:53:51 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "imagedb.h"
#include "dbpriv.h"
#include "dbstore.h"
//#include "didocu.h"
#include "dviface.h"
int 
main()
{
DVInterface *db=new DVInterface("dbfolder");

db->lockDatabase();
db->getNumberOfStudies();

 db->instanceReviewed("1.3.46.670589.11.0.1.1996021610440010","1.3.46.670589.11.0.2.1996021610440010","1.3.46.670589.11.0.4.1996021610440010" );
 
db->unlockDatabase();


db->lockDatabase();
cout << "Number of Studies: " << db->getNumberOfStudies() << endl;
db->selectStudy(2);
cout << "Study UID: " << db->getStudyUID() << endl;
db->selectSeries(0);
cout << "Number of Series: " << db->getNumberOfSeries() << endl;
cout << "Number of Series: " << db->getNumberOfSeries() << endl;
cout << "Number of Series: " << db->getNumberOfSeries() << endl;

cout << "Series UID: " << db->getSeriesUID() << endl;
//db->getNumberOfInstances();
cout << "Number of Instances: " << db->getNumberOfInstances() << endl;
db->selectInstance(0);
cout << "Instance UID: " << db->getInstanceUID() << endl;
db->deleteStudy("1.3.46.670589.11.0.1.1996021610440014");
db->unlockDatabase();

delete db;
}

/*
 *  $Log: didb.cc,v $
 *  Revision 1.1  1998-12-23 09:53:51  vorwerk
 *   testprogram which mark an instance as viewed, selects a study output
 *   studyuid, selects a series and output seriesuid, selects an instance
 *   and output instanceduid. Finally it delete a selected instance.
 *
 *  Revision 1.1  1998/12/22 15:31:02  vorwerk
 *  testprogram which marks, selects and deletes Instances
 *
 *
 */







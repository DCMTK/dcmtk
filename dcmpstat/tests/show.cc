/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *    testprogram which shows database contents 
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:30:07 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dbpriv.h"
#include "dbstore.h"
#include "dviface.h"
int 
main()
{
 
DB_Private_Handle 	*phandle ;  
DB_Handle 	*handle; 
DB_createHandle(
		  "dbfolder", 
		  100,
		  1000000, 
		  &handle
		  );

   if (handle==NULL) exit(1); 
  phandle = (DB_Private_Handle *) handle ; 
  DB_PrintIndexFile ("dbfolder");   
  DB_destroyHandle(&handle); 
}

/*
 *  $Log: show.cc,v $
 *  Revision 1.2  2000-03-08 16:30:07  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1998/12/23 09:54:39  vorwerk
 *  testprogram which shows database contents
 *
 *  Revision 1.1  1998/12/22 15:30:13  vorwerk
 *  testprogram which shows contents of indexfile from imagectn
 *
 *
 */

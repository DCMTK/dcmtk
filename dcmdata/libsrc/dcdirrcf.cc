/*
 *
 * Author: Gerd Ehlers	    Created:  02-15-95
 *			    Modified: 02-15-95
 *
 * Module: dcdirrcf.cc
 *
 * Purpose:
 * Implementation of one method of the class DcmDirectoryRecord
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "dcdirrec.h"
#include "dctk.h"
#include "dcxfer.h"
#include "dcvr.h"
#include "dcvrus.h"
#include "dcdebug.h"




// ********************************


// PENDING


E_Condition DcmDirectoryRecord::fillTypeElements( E_DirRecType type,
						  DcmFileFormat *fromFile )
{
Bdebug((3, "dcdirrec:DcmDirectoryRecord::fillTypeElements(type=%d,DcmFileFormat*)",
	   type ));

    E_Condition l_error = EC_IllegalCall;
    if ( fromFile != (DcmFileFormat*)NULL )
    {

	cerr << "ERROR: DcmDirectoryRecord::fillTypeElements not yet implemented" << endl;
    }
Edebug(());

    return l_error;
}


// ********************************


/*
 * 
 * Author: Gerd Ehlers          Created:  03-20-94
 *                              Modified: 11-23-94
 *
 * Module: stacktst.cc
 * 
 * Purpose:
 * testing the stack-class
 * 
 * 
 * Last Update:	  $Author: andreas $
 * Revision:      $Revision: 1.3 $
 * Status:        $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stream.h>
#include "dcstack.h"
#include "dcobject.h"
#include "dcvrus.h"
#include "dcdebug.h"
#include "dcdeftag.h"

main()
{
    SetDebugLevel(( 0 ));
    DcmStack s;
    unsigned long i;
    DcmObject *obj;

    for (i=1; i<=10; i++)
        s.push(new DcmUnsignedShort( DcmTag(DCM_Item), i*i));

    cout << "DcmStack: \n";
    while ( !s.empty() ) {
        obj = s.pop();
        cout << obj->getLength() << " ";
    }
    cout << endl;
}


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
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stream.h>
#include "dcstack.h"
#include "dcobject.h"
#include "dcvrus.h"
#include "dcdebug.h"


main()
{
    SetDebugLevel(( 0 ));
    DcmStack s;
    unsigned long i;
    DcmObject *obj;

    for (i=1; i<=10; i++)
        s.push(new DcmUnsignedShort( DcmTag(ET_Item), i*i, NULL ));

    cout << "DcmStack: \n";
    while ( !s.empty() ) {
        obj = s.pop();
        cout << obj->getLength() << " ";
    }
    cout << endl;
}


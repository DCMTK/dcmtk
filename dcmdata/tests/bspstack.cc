// bspstack.cc    03-20-94  until  11-23-94

#include <iostream.h>
#include "dcstack.h"
#include "dcobject.h"
#include "dcvrus.h"
#include "dcdeftag.h"

main()
{
    DcmStack s;

    for (Uint32 i=1; i<=10; i++)
        s.push(new DcmUnsignedShort( DcmTag(DCM_Item), i*i));

    cout << "DcmStack: \n";
    while ( !s.empty() ) {
       cout << s.pop()->getLength() << " ";
    }
    cout << endl;
}


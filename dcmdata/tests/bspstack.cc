// bspstack.cc    03-20-94  until  11-23-94

#include <stream.h>
#include "dcstack.h"
#include "dcobject.h"
#include "dcvrus.h"


main()
{
    DcmStack s;

    for (T_VR_UL i=1; i<=10; i++)
        s.push(new DcmUnsignedShort( DcmTag(ET_Item), i*i, NULL ));

    cout << "DcmStack: \n";
    while ( !s.empty() ) {
       cout << s.pop()->getLength() << " ";
    }
    cout << endl;
}


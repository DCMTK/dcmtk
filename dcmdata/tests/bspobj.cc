// bspobj.cc    03-20-94  until  02-15-95

#include <stdio.h>
#include <iostream.h>

#include "dctk.h"
#include "dcvr.h"
#include "dcdebug.h"


void printstack( DcmStack &stack )
{
    int i = (int)stack.card();
    while ( i-- > 0 )
    {
        DcmObject *obj = stack.elem( i );
        printf(" elem(%d):%p tag(%4.4x,%4.4x) vr(%s) err(%d) \"%s\"\n",
               i, obj, obj->getGTag(), obj->getETag(),
               DcmVR( obj->getVR() ).getVRName(),
               obj->error(), obj->getTag().getTagName() );
    }
}


void search( DcmObject* pobj, DcmTagKey xtag )
{
    DcmStack stack;
    while ( pobj->search(xtag, stack, ESM_afterStackTop, TRUE)
            == EC_Normal )
        printf(" Found element with tag=(%4.4x,%4.4x) pointer=%p \"%s\"",
               stack.top()->getGTag(), stack.top()->getETag(),
               stack.top(), stack.top()->getTag().getTagName() );

    cout << "# Stack returned after all searches:" << endl;
    printstack( stack );
}


int main()
{
    SetDebugLevel(( 0 ));
    iDicomStream myin( "PRIMITIV.DIO" );
    if ( myin.fail() )
        return 1;

    DcmFileFormat *pobj = new DcmFileFormat( &myin );
    pobj->read( EXS_UNKNOWN, EGL_withGL );
    pobj->print();

    DcmFileFormat ff( *pobj );               // copy FileFormat

    DcmStack stk;
    ff.searchErrors( stk );                  // search errors
    printstack( stk );                       // print collected errors

    search( pobj, DCM_BitsAllocated );    // search one tag

    oDicomStream myout( "PRIMITIV.OUT" );
    pobj->write( myout, EXS_BigEndianExplicit,
                 EET_ExplicitLength, EGL_withoutGL );

    cerr << "Verify returned with: " << pobj->verify( TRUE ) << endl;

    delete pobj;
    return 0;
}


/*
 *
 * Author: Gerd Ehlers      Created:  04-27-94
 *                          Modified: 02-07-95
 *
 * Module: dcelem.cc
 *
 * Purpose:
 * Implementation of class DcmElement
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "dcelem.h"
#include "dcobject.h"
#include "dcdebug.h"




// ********************************


DcmElement::DcmElement( DcmTag &tag )
    : DcmObject( tag )
{
Bdebug((6, "dcelem:DcmElement::DcmElement(DcmTag&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    ElementValue = (char*)NULL;
Edebug(());

}


// ********************************


DcmElement::DcmElement( DcmTag &tag,
			T_VR_UL len,
			iDicomStream *iDStream )
    : DcmObject( tag, len, iDStream )
{
Bdebug((6, "dcelem:DcmElement::DcmElement(DcmTag&,len=%ld,*iDS)", len ));
debug(( 8, "Object pointer this=0x%p", this ));

    ElementValue = (char*)NULL;
Edebug(());

}


// ********************************


DcmElement::~DcmElement()
{
Bdebug((6, "dcelem:DcmElement::~DcmElement()"));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( ElementValue != (char*)NULL )
	delete ElementValue;
Edebug(());

}


// ********************************


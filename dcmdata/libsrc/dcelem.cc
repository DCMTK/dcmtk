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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdlib.h>

#include "dcelem.h"
#include "dcobject.h"
#include "dcdebug.h"



// ********************************


DcmElement::DcmElement( const DcmTag &tag,
			T_VR_UL len,
			iDicomStream *iDStream )
    : DcmObject( tag, len, iDStream )
{
    Bdebug((6, "dcelem:DcmElement::DcmElement(DcmTag&,len=%ld,*iDS)", len ));
    debug(( 8, "Object pointer this=0x%p", this ));

    elementValue = NULL;

    Edebug(());
}


// ********************************


DcmElement::DcmElement( const DcmElement& elem )
    : DcmObject( elem )
{
    Bdebug((6, "dcelem:DcmElement::DcmElement(const DcmObject&)" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    elementValue = NULL;

    Edebug(());
}


// ********************************


DcmElement::~DcmElement()
{
    Bdebug((6, "dcelem:DcmElement::~DcmElement()"));
    debug(( 8, "Object pointer this=0x%p", this ));

    if (elementValue != NULL) {
	delete elementValue;
    }

    Edebug(());
}


// ********************************


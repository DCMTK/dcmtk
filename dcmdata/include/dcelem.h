/*
 *
 * Author: Gerd Ehlers	    Created:  04-27-94
 *                          Modified: 02-07-95
 *
 * Module: dcelem.h
 *
 * Purpose:
 * Interface of class DcmElement
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCELEM_H
#define DCELEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcobject.h"



class DcmElement : public DcmObject {
    char*	ElementValue;
public:
    DcmElement( DcmTag &tag );
    DcmElement( DcmTag &tag,
		T_VR_UL len,
		iDicomStream *iDStream );
    virtual ~DcmElement();
};


#endif // DCELEM_H

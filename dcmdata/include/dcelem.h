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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCELEM_H
#define DCELEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcobject.h"



class DcmElement : public DcmObject {
protected:
    void* elementValue; /* currently not used */
public:
    DcmElement( const DcmTag &tag,
		T_VR_UL len = 0,
		iDicomStream *iDStream = NULL );
    DcmElement( const DcmElement& elem );
    virtual ~DcmElement();
};


#endif // DCELEM_H

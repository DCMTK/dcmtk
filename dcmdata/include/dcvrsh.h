/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrsh.h
 *
 * Purpose:
 * Interface of class DcmShortString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRSH_H
#define DCVRSH_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmShortString : public DcmCharString {
public:
    DcmShortString( const DcmTag &tag,
                    T_VR_UL len = 0,
                    iDicomStream *iDStream = NULL);
    DcmShortString( const DcmShortString& old );
    virtual ~DcmShortString();
    virtual DcmEVR ident() const;
};


#endif // DCVRSH_H


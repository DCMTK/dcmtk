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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRSH_H
#define DCVRSH_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmShortString : public DcmCharString {
public:
    DcmShortString( DcmTag &tag );
    DcmShortString( DcmTag &tag,
                    T_VR_UL len,
                    iDicomStream *iDStream );
    DcmShortString( const DcmObject &oldObj );
    DcmShortString( const DcmShortString &newSH );
    virtual ~DcmShortString();
    virtual EVR ident() const;
};


#endif // DCVRSH_H


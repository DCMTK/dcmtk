/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrda.h
 *
 * Purpose:
 * Interface of class DcmDate
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRDA_H
#define DCVRDA_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmDate : public DcmByteString {
public:
    DcmDate( DcmTag &tag );
    DcmDate( DcmTag &tag,
             T_VR_UL len,
             iDicomStream *iDStream );
    DcmDate( const DcmObject &oldObj );
    DcmDate( const DcmDate &newDA );
    virtual ~DcmDate();
    virtual EVR ident() const;
};


#endif // DCVRDA_H


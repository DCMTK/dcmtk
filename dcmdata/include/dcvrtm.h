/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrtm.h
 *
 * Purpose:
 * Interface of class DcmTime
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRTM_H
#define DCVRTM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmTime : public DcmByteString {
public:
    DcmTime( DcmTag &tag );
    DcmTime( DcmTag &tag,
             T_VR_UL len,
             iDicomStream *iDStream );
    DcmTime( const DcmObject &oldObj );
    DcmTime( const DcmTime &newTM );
    virtual ~DcmTime();
    virtual EVR ident() const;
};


#endif // DCVRTM_H


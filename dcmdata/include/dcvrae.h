/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrae.h
 *
 * Purpose:
 * Interface of class DcmApplicationEntity
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRAE_H
#define DCVRAE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmApplicationEntity : public DcmByteString {
public:
    DcmApplicationEntity( DcmTag &tag );
    DcmApplicationEntity( DcmTag &tag,
                          T_VR_UL len,
                          iDicomStream *iDStream );
    DcmApplicationEntity( const DcmObject &oldObj );
    DcmApplicationEntity( const DcmApplicationEntity &newAE );
    virtual ~DcmApplicationEntity();
    virtual EVR ident() const;
};


#endif // DCVRAE_H


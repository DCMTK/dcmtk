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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRAE_H
#define DCVRAE_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmApplicationEntity : public DcmByteString {
public:
    DcmApplicationEntity( const DcmTag &tag,
                          T_VR_UL len = 0,
                          iDicomStream *iDStream = NULL);
    DcmApplicationEntity( const DcmApplicationEntity &newAE );

    virtual ~DcmApplicationEntity();
    virtual DcmEVR ident() const;
};


#endif // DCVRAE_H


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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRTM_H
#define DCVRTM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmTime : public DcmByteString {
public:
    DcmTime( const DcmTag &tag,
             T_VR_UL len = 0,
             iDicomStream *iDStream = NULL);
    DcmTime( const DcmTime& old );
    virtual ~DcmTime();
    virtual DcmEVR ident() const;
};


#endif // DCVRTM_H


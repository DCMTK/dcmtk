/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrcs.h
 *
 * Purpose:
 * Interface of class DcmCodeString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRCS_H
#define DCVRCS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmCodeString : public DcmByteString {
public:
    DcmCodeString( const DcmTag &tag,
                   T_VR_UL len = 0,
                   iDicomStream *iDStream = NULL);
    DcmCodeString( const DcmCodeString &newCS );
    virtual ~DcmCodeString();
    virtual DcmEVR ident() const;
};


#endif // DCVRCS_H


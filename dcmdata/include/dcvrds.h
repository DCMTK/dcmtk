/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrds.h
 *
 * Purpose:
 * Interface of class DcmDecimalString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRDS_H
#define DCVRDS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmDecimalString : public DcmByteString {
public:
    DcmDecimalString( const DcmTag &tag,
                      T_VR_UL len = 0,
                      iDicomStream *iDStream = NULL);
    DcmDecimalString( const DcmDecimalString &newDS );
    virtual ~DcmDecimalString();
    virtual DcmEVR ident() const;
};


#endif // DCVRDS_H


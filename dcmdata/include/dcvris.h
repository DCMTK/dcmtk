/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvris.h
 *
 * Purpose:
 * Interface of class DcmIntegerString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRIS_H
#define DCVRIS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmIntegerString : public DcmByteString {
public:
    DcmIntegerString( const DcmTag &tag,
                      T_VR_UL len = 0,
                      iDicomStream *iDStream = NULL);
    DcmIntegerString( const DcmIntegerString& old );
    virtual ~DcmIntegerString();
    virtual DcmEVR ident() const;
};


#endif // DCVRIS_H


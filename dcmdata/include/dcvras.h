/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvras.h
 *
 * Purpose:
 * Interface of class DcmAgeString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRAS_H
#define DCVRAS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmAgeString : public DcmByteString {
public:
    DcmAgeString( const DcmTag &tag,
                  T_VR_UL len = 0,
                  iDicomStream *iDStream = NULL);
    DcmAgeString( const DcmAgeString &newAS );

    virtual ~DcmAgeString();
    virtual DcmEVR ident() const;
};


#endif // DCVRAS_H


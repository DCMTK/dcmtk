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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRAS_H
#define DCVRAS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmAgeString : public DcmByteString {
public:
    DcmAgeString( DcmTag &tag );
    DcmAgeString( DcmTag &tag,
                  T_VR_UL len,
                  iDicomStream *iDStream );
    DcmAgeString( const DcmObject &oldObj );
    DcmAgeString( const DcmAgeString &newAS );
    virtual ~DcmAgeString();
    virtual EVR ident() const;
};


#endif // DCVRAS_H


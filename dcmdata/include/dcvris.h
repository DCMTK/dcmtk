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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRIS_H
#define DCVRIS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmIntegerString : public DcmByteString {
public:
    DcmIntegerString( DcmTag &tag );
    DcmIntegerString( DcmTag &tag,
                      T_VR_UL len,
                      iDicomStream *iDStream );
    DcmIntegerString( const DcmObject &oldObj );
    DcmIntegerString( const DcmIntegerString &newIS );
    virtual ~DcmIntegerString();
    virtual EVR ident() const;
};


#endif // DCVRIS_H


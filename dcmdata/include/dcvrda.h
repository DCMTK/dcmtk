/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrda.h
 *
 * Purpose:
 * Interface of class DcmDate
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRDA_H
#define DCVRDA_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmDate : public DcmByteString {
public:
    DcmDate( const DcmTag &tag,
             T_VR_UL len = 0,
             iDicomStream *iDStream = 0);
    DcmDate( const DcmDate &newDA );
    virtual ~DcmDate();
    virtual DcmEVR ident() const;
};


#endif // DCVRDA_H


/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrlt.h
 *
 * Purpose:
 * Interface of class DcmLongText
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCVRLT_H
#define DCVRLT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmLongText : public DcmCharString {
public:
    DcmLongText( const DcmTag &tag,
                 T_VR_UL len = 0,
                 iDicomStream *iDStream = NULL);
    DcmLongText( const DcmLongText& old );
    virtual ~DcmLongText();
    virtual DcmEVR ident() const;
    virtual T_VR_UL getVM();
};


#endif // DCVRLT_H


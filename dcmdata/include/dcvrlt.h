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
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCVRLT_H
#define DCVRLT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmLongText : public DcmCharString {
public:
    DcmLongText( DcmTag &tag );
    DcmLongText( DcmTag &tag,
                 T_VR_UL len,
                 iDicomStream *iDStream );
    DcmLongText( const DcmObject &oldObj );
    DcmLongText( const DcmLongText &newLT );
    virtual ~DcmLongText();
    virtual EVR ident() const;
    virtual T_VR_UL getVM();
};


#endif // DCVRLT_H


/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrst.h
 *
 * Purpose:
 * Interface of class DcmShortText
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRST_H
#define DCVRST_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmShortText : public DcmCharString {
public:
    DcmShortText( const DcmTag &tag,
                  T_VR_UL len = 0,
                  iDicomStream *iDStream = NULL);
    DcmShortText( const DcmShortText& old );
    virtual ~DcmShortText();
    virtual DcmEVR ident() const;
    virtual T_VR_UL getVM();
};


#endif // DCVRST_H


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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRST_H
#define DCVRST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcchrstr.h"



class DcmShortText : public DcmCharString {
public:
    DcmShortText( DcmTag &tag );
    DcmShortText( DcmTag &tag,
                  T_VR_UL len,
                  iDicomStream *iDStream );
    DcmShortText( const DcmObject &oldObj );
    DcmShortText( const DcmShortText &newST );
    virtual ~DcmShortText();
    virtual EVR ident() const;
    virtual T_VR_UL getVM();
};


#endif // DCVRST_H


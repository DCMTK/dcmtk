/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrui.h
 *
 * Purpose:
 * Interface of class DcmUniqueIdentifier
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRUI_H
#define DCVRUI_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmUniqueIdentifier : public DcmByteString {
public:
    DcmUniqueIdentifier( DcmTag &tag );
    DcmUniqueIdentifier( DcmTag &tag,
                         T_VR_UL len,
                         iDicomStream *iDStream );
    DcmUniqueIdentifier( const DcmObject &oldObj );
    DcmUniqueIdentifier( const DcmUniqueIdentifier &newUI );
    virtual ~DcmUniqueIdentifier();
    virtual EVR         ident() const;
    virtual void	print(	int level = 0 );
};


#endif // DCVRUI_H


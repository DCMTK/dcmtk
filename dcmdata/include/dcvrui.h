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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRUI_H
#define DCVRUI_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcbytstr.h"



class DcmUniqueIdentifier : public DcmByteString {
public:
    DcmUniqueIdentifier( const DcmTag &tag,
                         T_VR_UL len = 0,
                         iDicomStream *iDStream = NULL);
    DcmUniqueIdentifier( const DcmUniqueIdentifier &old );
    virtual ~DcmUniqueIdentifier();
    virtual DcmEVR         ident() const;
    virtual void	print(	int level = 0 );
};


#endif // DCVRUI_H


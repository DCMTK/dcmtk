/*
 *
 * Author: Gerd Ehlers      Created:  06-03-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrulup.h
 *
 * Purpose:
 * Interface of class DcmUnsignedLongOffset
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRULUP_H
#define DCVRULUP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcvrul.h"



class DcmUnsignedLongOffset : public DcmUnsignedLong {
protected:
    DcmObject* nextRecord;

public:
    DcmUnsignedLongOffset( const DcmTag &tag,
                           T_VR_UL len = 0,
                           iDicomStream *iDStream = NULL);
    DcmUnsignedLongOffset( const DcmUnsignedLongOffset& old );
    virtual ~DcmUnsignedLongOffset();

    virtual DcmEVR         ident() const;
    virtual void	print(	int level = 0 );

    virtual DcmObject*  setNextRecord( DcmObject* record );
    virtual DcmObject*  getNextRecord();
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
};


#endif // DCVRUSUP_H


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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRULUP_H
#define DCVRULUP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcvrul.h"



class DcmUnsignedLongOffset : public DcmUnsignedLong {
protected:
    DcmObject* nextRecord;

public:
    DcmUnsignedLongOffset( DcmTag &tag );
    DcmUnsignedLongOffset( DcmTag &tag,
                           T_VR_UL len,
                           iDicomStream *iDStream );
    DcmUnsignedLongOffset( const DcmObject &oldObj );
    DcmUnsignedLongOffset( const DcmUnsignedLongOffset &newULup );
    virtual ~DcmUnsignedLongOffset();

    virtual EVR         ident() const;
    virtual void	print(	int level = 0 );

    virtual DcmObject*  setNextRecord( DcmObject* record );
    virtual DcmObject*  getNextRecord();
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
};


#endif // DCVRUSUP_H


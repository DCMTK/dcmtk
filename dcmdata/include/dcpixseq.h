/*
 *
 * Author: Gerd Ehlers      Created:  11-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcpixseq.h
 *
 * Purpose:
 * Interface of class DcmPixelSequence
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcsequen.h"


class DcmPixelSequence : public DcmSequenceOfItems {
protected:
    virtual E_Condition readSubItem( DcmTag &newTag,             // in
                                     T_VR_UL newLength,          // in
                                     E_TransferSyntax xfer,      // in
                                     E_GrpLenEncoding gltype );  // in
public:
    DcmPixelSequence(   DcmTag &tag );
    DcmPixelSequence(   DcmTag &tag,
                        T_VR_UL len,
                        iDicomStream *iDStream );
    DcmPixelSequence(   const DcmObject &oldObj );
    DcmPixelSequence(   const DcmPixelSequence &oldForm );
    virtual ~DcmPixelSequence();

    virtual EVR         ident() const;
    virtual void        print( int level = 0 );
};



#endif // DCPIXSEQ_H


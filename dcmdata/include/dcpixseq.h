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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcsequen.h"


class DcmPixelSequence : public DcmSequenceOfItems {
protected:
    virtual E_Condition readSubItem( const DcmTag &newTag,       // in
                                     T_VR_UL newLength,          // in
                                     E_TransferSyntax xfer,      // in
                                     E_GrpLenEncoding gltype );  // in
public:
    DcmPixelSequence( const DcmTag &tag,
                      T_VR_UL len = 0,
                      iDicomStream *iDStream = NULL);
    DcmPixelSequence( const DcmPixelSequence &old );
    virtual ~DcmPixelSequence();

    virtual DcmEVR      ident() const;
    virtual void        print( int level = 0 );
};

#endif // DCPIXSEQ_H


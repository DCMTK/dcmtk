/*
 *
 * Author: Gerd Ehlers      Created:  11-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcpxitem.h
 *
 * Purpose:
 * Interface of class DcmPixelItem
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCPXITEM_H
#define DCPXITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcitem.h"



class DcmPixelItem : public DcmItem {
    U_CHAR *PixelValue;

protected:
    virtual E_Condition readValueField( E_TransferSyntax xfer );
    virtual E_Condition alignValue();

public:
    DcmPixelItem();
    DcmPixelItem( DcmTag &tag );
    DcmPixelItem( DcmTag &tag,
                  T_VR_UL len,
                  iDicomStream *iDStream );
    DcmPixelItem( const DcmObject &newObj );
    DcmPixelItem( const DcmPixelItem &newPixIt );
    virtual ~DcmPixelItem();

    virtual EVR 	ident() const;
    virtual void        print(      int level = 0 );
    virtual T_VR_UL	getVM();

    virtual E_Condition read(       E_TransferSyntax xfer,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition write(      oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype = EET_UndefinedLength,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition readBlock(  E_TransferSyntax xfer,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition writeBlock( oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype = EET_UndefinedLength,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition put(        U_CHAR *bytevalue,
                                    T_VR_UL length );      // number of bytes
    virtual U_CHAR*     getBytes();
    virtual E_Condition clear();
    virtual E_Condition verify(     BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCPXITEM_H

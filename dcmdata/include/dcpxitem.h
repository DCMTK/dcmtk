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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCPXITEM_H
#define DCPXITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcitem.h"



class DcmPixelItem : public DcmItem {
    BYTE *PixelValue;

protected:
    virtual E_Condition readValueField( E_TransferSyntax xfer );
    virtual E_Condition alignValue();

public:
    DcmPixelItem();
    DcmPixelItem( const DcmTag &tag,
                  T_VR_UL len = 0,
                  iDicomStream *iDStream = NULL);
    DcmPixelItem( const DcmPixelItem &old );
    virtual ~DcmPixelItem();

    virtual DcmEVR 	ident() const;
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
    virtual E_Condition put(        BYTE *bytevalue,
                                    T_VR_UL length );      // number of bytes
    virtual BYTE*     getBytes();
    virtual E_Condition clear();
    virtual E_Condition verify(     BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCPXITEM_H

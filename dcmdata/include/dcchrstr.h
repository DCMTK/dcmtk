/*
 *
 * Author: Gerd Ehlers	    Created:  05-05-94
 *                          Modified: 02-07-95
 *
 * Module: dcchrstr.h
 *
 * Purpose:
 * Interface of class DcmCharString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCCHRSTR_H
#define DCCHRSTR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"


// In dieser Implementierung ist die Zeichenbreite auf 8-Bit beschraenkt.
// Bei einer Erweiterung auf einen Zeichensatz mit 16-Bit-Zeichen muessen
// alle Quelltext-Zeilen ueberprueft und gegebenenfalls angepasst werden, die
// den folgenden Typ ALT_CHAR benutzen.
// Insbesondere sollten alle von DcmCharString abgeleiteten Klassen
// ueberprueft werden.

typedef char ALT_CHAR;


class DcmCharString : public DcmElement {
protected:
    ALT_CHAR *CharStringValue;
    ALT_CHAR paddingChar;
    T_VR_UL  maxLength;
    T_VR_UL  realLength;

    virtual E_Condition readValueField( E_TransferSyntax xfer );
    virtual E_Condition alignValue();
    virtual E_Condition deAlignValue();

public:
    DcmCharString( const DcmTag &tag,
		   T_VR_UL len = 0,
		   iDicomStream *iDStream = NULL );
    DcmCharString( const DcmCharString& old,
                   DcmEVR oldIdent = EVR_UNKNOWN );
    virtual ~DcmCharString();

    virtual DcmEVR      ident() const { return EVR_UNKNOWN; }
    virtual void	print(	int level = 0 );
    virtual T_VR_UL	getVM();

    virtual E_Condition read(   E_TransferSyntax xfer,
                                E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition write(  oDicomStream &oDS,
                                E_TransferSyntax oxfer,
                                E_EncodingType enctype = EET_UndefinedLength,
                                E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition readBlock(  E_TransferSyntax xfer,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition writeBlock( oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype = EET_UndefinedLength,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition put( const ALT_CHAR *charstringvalue );
    virtual ALT_CHAR*	get();
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCCHRSTR_H


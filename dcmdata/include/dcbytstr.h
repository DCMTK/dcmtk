/*
 *
 * Author: Gerd Ehlers	    Created:  05-05-94
 *                          Modified: 02-07-95
 *
 * Module: dcbytstr.h
 *
 * Purpose:
 * Interface of class DcmByteString
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCBYTSTR_H
#define DCBYTSTR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"



class DcmByteString : public DcmElement {
protected:
    char    *ByteStringValue;
    char    paddingChar;
    T_VR_UL maxLength;
    T_VR_UL realLength;

    virtual E_Condition readValueField( E_TransferSyntax xfer );
    virtual E_Condition alignValue();
    virtual E_Condition deAlignValue();

public:
    DcmByteString( const DcmTag &tag,
		   T_VR_UL len = 0,
		   iDicomStream *iDStream = NULL);

    DcmByteString( const DcmByteString& old,
                   DcmEVR oldIdent = EVR_UNKNOWN );

    virtual ~DcmByteString();

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
    virtual E_Condition put( const char *bytestringvalue );
    virtual const char*	get();
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCBYTSTR_H


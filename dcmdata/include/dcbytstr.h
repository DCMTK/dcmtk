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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCBYTSTR_H
#define DCBYTSTR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
    DcmByteString( DcmTag &tag );
    DcmByteString( DcmTag &tag,
		   T_VR_UL len,
		   iDicomStream *iDStream );
    DcmByteString( const DcmObject &oldObj,
                   EVR oldIdent );
    virtual ~DcmByteString();

    virtual EVR         ident() const = 0;
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
    virtual E_Condition put(	char *bytestringvalue );
    virtual char*	get();
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCBYTSTR_H


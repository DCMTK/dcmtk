/*
 *
 * Author: Gerd Ehlers	    Created:  05-05-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrobow.h
 *
 * Purpose:
 * Interface of class DcmOtherByteOtherWord
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVROBOW_H
#define DCVROBOW_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"



class DcmOtherByteOtherWord : public DcmElement {
    void*   OtherValue;
    BOOL    mustSwap, swappedToWord;

    void    swapValueToWord( BOOL toWord );

protected:
    virtual E_Condition readValueField( E_TransferSyntax xfer );
    virtual E_Condition alignValue();

public:
    DcmOtherByteOtherWord( const DcmTag &tag,
			   T_VR_UL len = 0,
			   iDicomStream *iDStream = NULL);
    DcmOtherByteOtherWord( const DcmOtherByteOtherWord& old );
    virtual ~DcmOtherByteOtherWord();

    virtual E_Condition setVR( DcmEVR vr );
    virtual DcmEVR 	ident() const;
    virtual void	print( int level = 0 );
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
    virtual E_Condition put(    BYTE *bytevalue,
				T_VR_UL length );      // number of bytes
    virtual E_Condition put(	T_VR_US *wordvalue,
				T_VR_UL length );      // number of words
    virtual BYTE*	getBytes();		       // Restriction of C++
    virtual T_VR_US*	getWords();		       //	   -""-
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCVROBOW_H

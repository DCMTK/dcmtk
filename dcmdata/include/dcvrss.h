/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrss.h
 *
 * Purpose:
 * Interface of class DcmSignedShort
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRSS_H
#define DCVRSS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"



class DcmSignedShort : public DcmElement {
    T_VR_SS *SigShortValue;

protected:
    virtual E_Condition readValueField( E_TransferSyntax xfer );

public:
    DcmSignedShort( DcmTag &tag );
    DcmSignedShort( DcmTag &tag,
		    T_VR_UL len,
		    iDicomStream *iDStream );
    DcmSignedShort( const DcmObject &oldObj );
    DcmSignedShort( const DcmSignedShort &newSS );
    virtual ~DcmSignedShort();

    virtual EVR         ident() const;
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
    virtual E_Condition put(    T_VR_SS *sshort,
                                T_VR_UL length );     // number of words
    virtual E_Condition put(    T_VR_SS sshort );     // for one short only
    virtual E_Condition put(    T_VR_SS sshort,       // any longs
                                T_VR_UL num );
    virtual T_VR_SS*    get();
    virtual T_VR_SS     get(    T_VR_UL num );
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCVRSS_H


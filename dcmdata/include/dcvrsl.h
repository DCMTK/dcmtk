/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrsl.h
 *
 * Purpose:
 * Interface of class DcmSignedLong
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRSL_H
#define DCVRSL_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"



class DcmSignedLong : public DcmElement {
    T_VR_SL *SigLongValue;

protected:
    virtual E_Condition readValueField( E_TransferSyntax xfer );

public:
    DcmSignedLong( const DcmTag &tag,
		   T_VR_UL len = 0,
		   iDicomStream *iDStream = NULL);
    DcmSignedLong( const DcmSignedLong& old );
    virtual ~DcmSignedLong();

    virtual DcmEVR         ident() const;
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
    virtual E_Condition put(    T_VR_SL *slong,
                                T_VR_UL length );     // number of longs
    virtual E_Condition put(    T_VR_SL slong );      // for one long only
    virtual E_Condition put(    T_VR_SL slong,        // any longs
                                T_VR_UL num );
    virtual T_VR_SL*    get();
    virtual T_VR_SL     get(    T_VR_UL num );
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCVRSL_H


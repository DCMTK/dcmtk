/*
 *
 * Author: Gerd Ehlers	    Created:  04-26-94
 *                          Modified: 02-07-95
 *
 * Module: dcdatset.h
 *
 * Purpose:
 * Interface of class DcmDataset
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCDATSET_H
#define DCDATSET_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcitem.h"



class DcmDataset : public DcmItem {
    T_VR_UL		resolveAmbigous();

public:
    DcmDataset();
    DcmDataset( iDicomStream *iDStream );
    DcmDataset( const DcmDataset &old );
    virtual ~DcmDataset();

    virtual DcmEVR 	ident() const;
    virtual void	print(	    int level = 0 );

    virtual E_Condition read(       E_TransferSyntax xfer = EXS_UNKNOWN,
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
};



#endif // DCDATSET_H


/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrfd.h
 *
 * Purpose:
 * Interface of class DcmFloatingPointDouble
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRFD_H
#define DCVRFD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcelem.h"



class DcmFloatingPointDouble : public DcmElement {
    T_VR_FD *DoubleValue;

protected:
    virtual E_Condition readValueField( E_TransferSyntax xfer );

public:
    DcmFloatingPointDouble( DcmTag &tag  );
    DcmFloatingPointDouble( DcmTag &tag,
			    T_VR_UL len,
			    iDicomStream *iDStream );
    DcmFloatingPointDouble( const DcmObject &oldObj );
    DcmFloatingPointDouble( const DcmFloatingPointDouble &newFD );
    virtual ~DcmFloatingPointDouble();

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
    virtual E_Condition put(    T_VR_FD *doubleval,
                                T_VR_UL length );     // number of doubles
    virtual E_Condition put(    T_VR_FD doubleval );  // for one double only
    virtual E_Condition put(    T_VR_FD doubleval,    // any longs
                                T_VR_UL num );
    virtual T_VR_FD*    get();
    virtual T_VR_FD     get(    T_VR_UL num );
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCVRFD_H


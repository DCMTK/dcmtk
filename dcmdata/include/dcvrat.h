/*
 *
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrat.h
 *
 * Purpose:
 * Interface of class DcmAttributeTag
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCVRAT_H
#define DCVRAT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"



class DcmAttributeTag : public DcmElement {
    T_VR_US *AttrValue;

protected:
    virtual E_Condition readValueField( E_TransferSyntax xfer );

public:
    DcmAttributeTag( const DcmTag &tag,
		     T_VR_UL len = 0,
		     iDicomStream *iDStream = NULL);
    DcmAttributeTag( const DcmAttributeTag &newAT );
    virtual ~DcmAttributeTag();

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
    virtual E_Condition put(	T_VR_US *attrvalue,
                                T_VR_UL length );   // number of double words
    virtual E_Condition put(    DcmTag &attrTag );  // for one Tag only
    virtual E_Condition put(    DcmTag &attrTag,    // any Tags
                                T_VR_UL num );
    virtual T_VR_US*	get();
    virtual DcmTag      get(    T_VR_UL num );
    virtual E_Condition clear();
    virtual E_Condition verify( BOOL autocorrect = FALSE );
    virtual E_Condition loadAllDataIntoMemory();
};


#endif // DCVRAT_H


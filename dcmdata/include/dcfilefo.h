/*
 *
 * Author: Gerd Ehlers	    Created:  05-15-94
 *                          Modified: 02-07-95
 *
 * Module: dcfilefo.h
 *
 * Purpose:
 * Interface of class DcmFileFormat
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCFILEFO_H
#define DCFILEFO_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcsequen.h"
#include "dcmetinf.h"
#include "dcdatset.h"




class DcmFileFormat : public DcmSequenceOfItems {
    E_Condition      checkValue(    DcmMetaInfo *metainfo,
				    DcmDataset *dataset,
				    const DcmTagKey &atagkey,
				    DcmObject* obj,
				    E_TransferSyntax oxfer );
    E_TransferSyntax lookForXfer(   DcmMetaInfo* metainfo );

public:
    DcmFileFormat();
    DcmFileFormat( DcmDataset *dataset );
    DcmFileFormat( iDicomStream *iDStream );
    DcmFileFormat( const DcmFileFormat &old );
    virtual ~DcmFileFormat();

    virtual E_Condition  validateMetaInfo( E_TransferSyntax oxfer );

    virtual DcmEVR 	 ident() const;
    virtual void	 print(      int level = 0 );

    virtual E_Condition  read(       E_TransferSyntax xfer = EXS_UNKNOWN,
                                     E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition  write(      oDicomStream &oDS,
                                     E_TransferSyntax oxfer,
                                     E_EncodingType enctype = EET_UndefinedLength,
                                     E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual DcmMetaInfo* getMetaInfo();
    virtual DcmDataset*  getDataset();

// Die folgenden Methoden werden nicht benoetigt und sind deshalb ohne Funktion:

    virtual E_Condition  readBlock(  E_TransferSyntax xfer,
                                     E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition  writeBlock( oDicomStream &oDS,
                                     E_TransferSyntax oxfer,
                                     E_EncodingType enctype = EET_UndefinedLength,
                                     E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition  insertItem( DcmItem* item,
				     T_VR_UL where = UNDEF_LEN );
    virtual DcmItem*	 remove(     T_VR_UL num );
    virtual DcmItem*     remove(     DcmItem* item );
    virtual E_Condition  clear();
};



#endif // DCFILEFO_H


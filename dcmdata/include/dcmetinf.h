/*
 *
 * Author: Gerd Ehlers	    Created:  05-15-94
 *                          Modified: 02-07-95
 *
 * Module: dcmetinf.h
 *
 * Purpose:
 * Interface of class DcmMetaInfo
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCMETINF_H
#define DCMETINF_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcitem.h"


#define DICOM_MAGIC                        "DICM"
#define DICOM_MAGIC_LEN                    4
#define DICOM_PREAMBLE_LEN                 128
#define META_HEADER_DEFAULT_TRANSFERSYNTAX EXS_LittleEndianExplicit



class DcmMetaInfo : public DcmItem {
    char filePreamble[ DICOM_PREAMBLE_LEN + DICOM_MAGIC_LEN ];
    BOOL preambleUsed;

    void	setPreamble();
    BOOL	checkAndReadPreamble( E_TransferSyntax *newxfer );	 // out
    BOOL	nextTagIsMeta();
    E_Condition readGroupLength(      E_TransferSyntax xfer,             // in
                                      const DcmTagKey &xtag,             // in
                                      E_GrpLenEncoding gltype,           // in
				      T_VR_UL *headerLen,		 // out
				      T_VR_UL *bytes_read );		 // out

public:
    DcmMetaInfo();
    DcmMetaInfo( iDicomStream *iDStream );
    DcmMetaInfo( const DcmMetaInfo &old );
    virtual ~DcmMetaInfo();

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



#endif // DCMETINF_H


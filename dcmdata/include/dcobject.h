/*
 *
 * Author: Gerd Ehlers	    Created:  04-09-94
 *                          Modified: 02-07-95
 *
 * Module: dcobject.h
 *
 * Purpose:
 * This file contains the interface to routines which provide
 * DICOM object encoding/decoding, search and lookup facilities.
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCOBJECT_H
#define DCOBJECT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcerror.h"
#include "dcstream.h"
#include "dctag.h"
#include "dcstack.h"


const Uint32 DCM_UndefinedLength = 0xffffffff;
/*
** Remove UNDEF_LEN when all use have been changed to DCM_UndefinedLength
*/
#define UNDEF_LEN DCM_UndefinedLength


class DcmObject {
protected:
    int              testConstructDestruct;   // for debugging

    DcmTag	     *Tag;
    T_VR_UL	     Length;
    E_Condition      errorFlag;
    iDicomStream     *iDS;
    E_TransferSyntax Xfer;
    long	     offsetInFile;
    BOOL	     valueInMemory;
    BOOL	     valueModified;
    E_ReadWriteState rdStat;
    E_ReadWriteState wrStat;
    T_VR_UL	     bytesRead;
    T_VR_UL	     bytesWritten;

    virtual void	printInfoLine(	   int level,
					   const char *info );
    virtual void	printInfoLine(	   int level,
                                           const DcmTag &tag,
					   T_VR_UL length,
					   const char *info );
    virtual E_Condition writeTagAndLength( oDicomStream &oDS,           // in
					   E_TransferSyntax oxfer,	// in
					   T_VR_UL *written_bytes );	// out
public:
    DcmObject( const DcmTag &tag, T_VR_UL len = 0, iDicomStream *iDStream = NULL);
    DcmObject( const DcmObject& obj );

    virtual ~DcmObject();

    virtual DcmEVR 	     ident() const = 0;
    virtual void	     print( int level = 0 ) = 0;
    virtual E_Condition      error();

    virtual E_ReadWriteState readState();
    virtual E_ReadWriteState writeState();
    virtual E_Condition      readBlockInit();
    virtual E_Condition      writeBlockInit();

    virtual E_TransferSyntax getOriginalXfer();

    virtual T_VR_US	getGTag();
    virtual T_VR_US	getETag();
    virtual DcmTag&     getTag();
    virtual DcmEVR 	getVR();
    virtual E_Condition setVR(	    DcmEVR vr );
    virtual T_VR_UL	getVM() = 0;
    virtual T_VR_UL	getLength(  E_TransferSyntax xfer = EXS_LittleEndianImplicit,
				    E_EncodingType enctype = EET_UndefinedLength );
    virtual E_Condition read(       E_TransferSyntax xfer = EXS_UNKNOWN,
                                    E_GrpLenEncoding gltype = EGL_withoutGL ) = 0;
    virtual E_Condition write(      oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype = EET_UndefinedLength,
                                    E_GrpLenEncoding gltype = EGL_withoutGL ) = 0;
    virtual E_Condition readBlock(  E_TransferSyntax xfer = EXS_UNKNOWN,
                                    E_GrpLenEncoding gltype = EGL_withoutGL ) = 0;
    virtual E_Condition writeBlock( oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype = EET_UndefinedLength,
                                    E_GrpLenEncoding gltype = EGL_withoutGL ) = 0;
    virtual E_Condition clear() = 0;
    virtual E_Condition verify(     BOOL autocorrect = FALSE ) = 0;
    virtual E_Condition search(     const DcmTag &tag,                 // in
				    DcmStack &resultStack,	       // inout
				    E_SearchMode mode = ESM_fromHere,  // in
				    BOOL searchIntoSub = TRUE );       // in
    virtual E_Condition search(     const DcmTagKey &xtag,	       // in
				    DcmStack &resultStack,	       // inout
				    E_SearchMode mode = ESM_fromHere,  // in
				    BOOL searchIntoSub = TRUE );       // in
    virtual E_Condition searchErrors( DcmStack &resultStack );	       // inout
    virtual E_Condition loadAllDataIntoMemory() = 0;

// Folgende Methoden koennen nicht abstrakt deklariert werden,
// weil sie unterschiedliche Parameter- und Rueckgabetypen besitzen:
//    virtual E_Condition put( void* );
//    virtual E_Condition insert( void* );
//    virtual void* get();
//    virtual void* remove();

}; // class DcmObject


#endif // DCOBJECT_H

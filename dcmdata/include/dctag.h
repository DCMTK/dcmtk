/*
 *
 * Author: Gerd Ehlers	    Created:  04-16-94
 *			    Modified: 02-07-95
 *
 * Module: dctag.h
 *
 * Purpose:
 * Definition of the class DcmTag
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCTAG_H
#define DCTAG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcdicenu.h"

#define UNKNOWN_TAG 0xffff


// *** class definition ********************************


class DcmTag {
    int     testConstructDestruct;   // for debugging

    T_VR_US grpTag;
    T_VR_US elemTag;
    ETag    xTag;
    EVR     vrType;
    short   maxVM;
    E_Condition errorFlag;

    BOOL binsearch( unsigned int left,
		    unsigned int right,
		    T_VR_US grp,
		    T_VR_US elem,
		    unsigned int &t_index );

public:
    DcmTag( T_VR_US grp,
	    T_VR_US elem,
	    EVR vr = EVR_UNKNOWN,
	    short vm = -1 );
    DcmTag( ETag tagidx );
    DcmTag( char *tagname );
    DcmTag( const DcmTag &newtag );
    ~DcmTag();

    DcmTag & operator =  ( const ETag tagidx );
    DcmTag & operator =  ( const char *tagname );
    DcmTag & operator =  ( const DcmTag &newtag );
    int operator      == ( const DcmTag &newtag );
    int operator      != ( const DcmTag &newtag );
    int operator      >  ( const DcmTag &newtag );
    int operator      >= ( const DcmTag &newtag );
    int operator      <  ( const DcmTag &newtag );
    int operator      <= ( const DcmTag &newtag );

    EVR 	      setVR( EVR vr );	  // nicht-eindeutige VR aufloesen
    EVR 	      getVR();
    T_VR_US	      getGTag();
    T_VR_US	      getETag();
    ETag	      getXTag();
    short	      getMaxVM();
    char*	      getTagName();

    E_Condition       error();

};


// *** global constants ********************************


extern DcmTag ItemTag;
extern DcmTag InternalUseTag;


#endif // DCTAG_H


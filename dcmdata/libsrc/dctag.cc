/*
 *
 * Author: Gerd Ehlers	    Created:  04-16-94
 *			    Modified: 02-15-95
 *
 * Module: dctag.cc
 *
 * Purpose:
 * Implementation of the class DcmTag
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <iostream.h>

#include "dctag.h"
#include "dcdebug.h"


// *** necessary definitions for including "ddicvarz.i" *************


typedef enum {
    EIn_NA, EIn_NULL, EIn_NV, EIn_SO16BW, EIn_SOC, EIn_SOB, EIn_VAR
} EIn;

typedef enum {
    ELi_byteString, ELi_fixed, ELi_maximum, ELi_NULL, ELi_variable,
    ELi_wordString
} ELi;

typedef struct {
    T_VR_US	  TagG;
    T_VR_US	  TagE;
    EVR 	  XVRType;
    short	  VM;
} S_TAGVR;

typedef struct {
    short	  VL;
    ELi 	  Li;
    EIn 	  In;
} S_VRTYPES;

typedef struct {
    EVR 	  XVRType;
    char	  *ExpText;
} S_VRvar;


#include "ddicnamz.i"
#include "ddicvarz.i"


#define DIM_OF(a) (sizeof(a) / sizeof(a[0]))
#define ERROR_TagName	"Unknown Tag & Data"
#define PRIVATE_TagName "Private Tag & Data"



// ********************************


BOOL DcmTag::binsearch( unsigned int left,
			unsigned int right,
			T_VR_US grp,
			T_VR_US elem,
			unsigned int &t_index )
{
    unsigned int m;
    if ( right < left )
	return FALSE;
    else
    {
	m = ( left + right ) / 2;
	if (	grp < TagVR[m].TagG
	     || (    grp == TagVR[m].TagG
		  && elem < TagVR[m].TagE
		)
	   )
	    return binsearch( left, m-1, grp, elem, t_index );
	else if (    grp > TagVR[m].TagG
		 || (	 grp == TagVR[m].TagG
		      && elem > TagVR[m].TagE
		    )
		)
	    return binsearch( m+1, right, grp, elem, t_index );
	else
	{
	    t_index = m;
	    return TRUE;
	}
    }
}


// ********************************


DcmTag::DcmTag( T_VR_US grp,
		T_VR_US elem,
		EVR vr,
		short vm )
{
Bdebug((7, "dctag:DcmTag::DcmTag(grp=0x%4.4x,elem=0x%4.4x,vr=%d,vm=%hd)",
	   grp, elem, vr, vm ));
debug(( 8, "Object pointer this=0x%p", this ));

    grpTag  = grp;
    elemTag = elem;
    T_VR_US tgrp = grp;

    if (    ((grp & 0xff00) == 0x5000)	     // set GroupNumber of repeated
	 || ((grp & 0xff00) == 0x6000)	     // groups to 1. group
       )
	tgrp = (grp & 0xff00);

    unsigned int t_index = 0;
    if ( binsearch( 1, DIM_OF(TagVR) - 1, tgrp, elem, t_index ) )
    {
	xTag	  = (ETag)t_index;
	if ( vr == EVR_UNKNOWN )
	    vrType = TagVR[t_index].XVRType;
	else
	    vrType = vr;
	if ( vm == -1 )
	    maxVM = TagVR[t_index].VM;
	else
	    maxVM = vm;
	errorFlag = EC_Normal;
    }
    else
    {
	xTag	  = ET_UNKNOWN;
	vrType	  = vr;
	maxVM	  = vm;
	errorFlag = EC_InvalidTag;
    }

    testConstructDestruct = 1; // for debugging

Edebug(());

}


// ********************************


DcmTag::DcmTag( ETag tagidx )
{
Bdebug((7, "dctag:DcmTag::DcmTag(xtag=%d)", tagidx ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( tagidx >= 0 && tagidx < (ETag)DIM_OF(TagVR) )
    {
	xTag	  = (ETag)tagidx;
	grpTag	  = TagVR[tagidx].TagG;
	elemTag   = TagVR[tagidx].TagE;
	vrType	  = TagVR[tagidx].XVRType;
	maxVM	  = TagVR[tagidx].VM;
	errorFlag = EC_Normal;
    }
    else
    {
	grpTag	  = UNKNOWN_TAG;
	elemTag   = UNKNOWN_TAG;
	xTag	  = ET_UNKNOWN;
	vrType	  = EVR_UNKNOWN;
	maxVM	  = -1;
	errorFlag = EC_InvalidTag;
    }

    testConstructDestruct = 1; // for debugging

Edebug(());

}


// ********************************


DcmTag::DcmTag( char *tagname )
{
Bdebug((7, "dctag:DcmTag::DcmTag(char*)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( tagname == (char*)NULL )
    {
	xTag	  = ET_UNKNOWN;
	vrType	  = EVR_UNKNOWN;
	maxVM	  = -1;
	grpTag	  = UNKNOWN_TAG;
	elemTag   = UNKNOWN_TAG;
	errorFlag = EC_InvalidTag;
    }
    else
    {
	unsigned int i = 0;
	while ((i < DIM_OF(TagName)) && (strcmp(TagName[i], tagname) != 0))
	    i++;

	if ((i < DIM_OF(TagName)) && (strcmp(TagName[i], tagname) == 0))
	{
	    xTag      = (ETag)i;
	    grpTag    = TagVR[i].TagG;
	    elemTag   = TagVR[i].TagE;
	    vrType    = TagVR[i].XVRType;
	    maxVM     = TagVR[i].VM;
	    errorFlag = EC_Normal;
	}
	else
	{
	    xTag      = ET_UNKNOWN;
	    vrType    = EVR_UNKNOWN;
	    maxVM     = -1;
	    grpTag    = UNKNOWN_TAG;
	    elemTag   = UNKNOWN_TAG;
	    errorFlag = EC_InvalidTag;
	}
    }

    testConstructDestruct = 1; // for debugging

Edebug(());

}


// ********************************


DcmTag::DcmTag( const DcmTag &newtag )
{
Bdebug((7, "dctag:DcmTag::DcmTag(DcmTag&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    grpTag    = newtag.grpTag;
    elemTag   = newtag.elemTag;
    xTag      = newtag.xTag;
    vrType    = newtag.vrType;
    maxVM     = newtag.maxVM;
    errorFlag = newtag.errorFlag;

    testConstructDestruct = 1; // for debugging

Edebug(());

}


// ********************************


DcmTag::~DcmTag()
{
Bdebug((7, "dctag:DcmTag::~DcmTag()"));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( testConstructDestruct == 1 )			// for debugging
	testConstructDestruct = 2; // for debugging
    else
    {
debug(( 1, "Error: ~DcmTag called more than once (%d)",
	   testConstructDestruct ));

	cerr << "Error: ~DcmTag called more than once ("
	     << testConstructDestruct << ")" << endl;
	testConstructDestruct++;
    }

Edebug(());

}


// ********************************


DcmTag & DcmTag::operator = ( const ETag tagidx )
{
Bdebug((6, "dctag:DcmTag::= (xtag=%d)", tagidx ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( tagidx >= 0 && tagidx < (ETag)DIM_OF(TagVR) )
    {
	xTag	  = (ETag)tagidx;
	grpTag	  = TagVR[tagidx].TagG;
	elemTag   = TagVR[tagidx].TagE;
	vrType	  = TagVR[tagidx].XVRType;
	maxVM	  = TagVR[tagidx].VM;
	errorFlag = EC_Normal;
    }
    else
    {
	grpTag	  = UNKNOWN_TAG;
	elemTag   = UNKNOWN_TAG;
	xTag	  = ET_UNKNOWN;
	vrType	  = EVR_UNKNOWN;
	maxVM	  = -1;
	errorFlag = EC_InvalidTag;
    }
debug(( 4, "DcmTag::= (xtag=%d): (0x%4.4x,0x%4.4x)",
	   xTag, grpTag, elemTag ));
Edebug(());

    return *this;
}


// ********************************


DcmTag & DcmTag::operator = ( const char *tagname )
{
Bdebug((6, "dctag:DcmTag::= (char*)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( tagname == (char*)NULL )
    {
	xTag	  = ET_UNKNOWN;
	vrType	  = EVR_UNKNOWN;
	maxVM	  = -1;
	grpTag	  = UNKNOWN_TAG;
	elemTag   = UNKNOWN_TAG;
	errorFlag = EC_InvalidTag;
    }
    else
    {
	unsigned int i = 0;
	while ((i < DIM_OF(TagName)) && (strcmp(TagName[i], tagname) != 0))
	    i++;

	if ((i < DIM_OF(TagName)) && (strcmp(TagName[i], tagname) == 0))
	{
	    xTag      = (ETag)i;
	    grpTag    = TagVR[i].TagG;
	    elemTag   = TagVR[i].TagE;
	    vrType    = TagVR[i].XVRType;
	    maxVM     = TagVR[i].VM;
	    errorFlag = EC_Normal;
	}
	else
	{
	    xTag      = ET_UNKNOWN;
	    vrType    = EVR_UNKNOWN;
	    maxVM     = -1;
	    grpTag    = UNKNOWN_TAG;
	    elemTag   = UNKNOWN_TAG;
	    errorFlag = EC_InvalidTag;
	}
    }
debug(( 6, "DcmTag::= (xtag=%d): (0x%4.4x,0x%4.4x)",
	   xTag, grpTag, elemTag ));
Edebug(());

    return *this;
}


// ********************************


DcmTag & DcmTag::operator = ( const DcmTag &newtag )
{
Bdebug((6, "dctag:DcmTag::= (DcmTag&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( this != &newtag )
    {
	grpTag	  = newtag.grpTag;
	elemTag   = newtag.elemTag;
	xTag	  = newtag.xTag;
	vrType	  = newtag.vrType;
	maxVM	  = newtag.maxVM;
	errorFlag = newtag.errorFlag;
debug(( 4, "DcmTag & operator = ( DcmTag & ): (0x%4.4x,0x%4.4x)",
	   grpTag, elemTag ));

    }
    else
    {
debug(( 1, "dctag:DcmTag::DcmTag(DcmTag&)  Warning: self-assignment" ));

    }
Edebug(());

    return *this;
}


// ********************************


int DcmTag::operator == ( const DcmTag &newtag )
{
Bdebug((6, "dctag:DcmTag(0x%4.4x,0x%4.4x)::== (0x%4.4x,0x%4.4x)",
	   grpTag, elemTag, newtag.grpTag, newtag.elemTag ));

    BOOL cmp = (    grpTag == newtag.grpTag
		 && elemTag == newtag.elemTag
	       );
Vdebug((6, cmp, "boths operands are equal" ));
Edebug(());

    return cmp;
}


// ********************************


int DcmTag::operator < ( const DcmTag &newtag )
{
Bdebug((6, "dctag:DcmTag(0x%4.4x,0x%4.4x)::< (0x%4.4x,0x%4.4x)",
	   grpTag, elemTag, newtag.grpTag, newtag.elemTag ));

    BOOL cmp = (    grpTag < newtag.grpTag
		 || (	 grpTag == newtag.grpTag
		      && elemTag < newtag.elemTag
		    )
	       );
Vdebug((6, cmp, "first operand is less than second" ));
Edebug(());

    return cmp;
}


// ********************************


int DcmTag::operator > ( const DcmTag &newtag )
{
Bdebug((6, "dctag:DcmTag(0x%4.4x,0x%4.4x)::> (0x%4.4x,0x%4.4x)",
	   grpTag, elemTag, newtag.grpTag, newtag.elemTag ));

    BOOL cmp = (    grpTag > newtag.grpTag
		 || (	 grpTag == newtag.grpTag
		      && elemTag > newtag.elemTag
		    )
	       );
Vdebug((6, cmp, "first operand is greater than second" ));
Edebug(());

    return cmp;
}


// ********************************


int DcmTag::operator != ( const DcmTag &newtag )
{
Bdebug((6, "dctag:DcmTag(0x%4.4x,0x%4.4x)::!= (0x%4.4x,0x%4.4x)",
	   grpTag, elemTag, newtag.grpTag, newtag.elemTag ));

    BOOL cmp = (    grpTag != newtag.grpTag
		 || elemTag != newtag.elemTag
	       );
Vdebug((6, cmp, "boths operands are not equal" ));
Edebug(());

    return cmp;
}


// ********************************


int DcmTag::operator <= ( const DcmTag &newtag )
{
Bdebug((6, "dctag:DcmTag(0x%4.4x,0x%4.4x)::<= (0x%4.4x,0x%4.4x)",
	   grpTag, elemTag, newtag.grpTag, newtag.elemTag ));

    BOOL cmp = (    grpTag < newtag.grpTag
		 || (	 grpTag == newtag.grpTag
		      && elemTag <= newtag.elemTag
		    )
	       );
Vdebug((6, cmp, "first operand is less than or equal to second" ));
Edebug(());

    return cmp;
}


// ********************************


int DcmTag::operator >= ( const DcmTag &newtag )
{
Bdebug((6, "dctag:DcmTag(0x%4.4x,0x%4.4x)::=> (0x%4.4x,0x%4.4x)",
	   grpTag, elemTag, newtag.grpTag, newtag.elemTag ));

    BOOL cmp = (    grpTag > newtag.grpTag
		 || (	 grpTag == newtag.grpTag
		      && elemTag >= newtag.elemTag
		    )
	       );
Vdebug((6, cmp, "first operand is greater than or equal to second" ));
Edebug(());

    return cmp;
}


// ********************************


EVR DcmTag::setVR( EVR vr )    // nicht-eindeutige VR aufloesen
{
    if ( (vr < EVR_AE) || (vr > EVR_xx) )
    {
	vrType = EVR_UNKNOWN;
	errorFlag = EC_InvalidVR;
    }
    else
    {
	vrType = vr;
	errorFlag = EC_Normal;
    }
    return vrType;
}


// ********************************


EVR DcmTag::getVR()
{
    errorFlag = (vrType == EVR_UNKNOWN) ? EC_InvalidTag : EC_Normal;
    return vrType;
}


// ********************************


T_VR_US DcmTag::getGTag()
{
    errorFlag = (grpTag == UNKNOWN_TAG) ? EC_InvalidTag : EC_Normal;
    return grpTag;
}


// ********************************


T_VR_US DcmTag::getETag()
{
    errorFlag = (elemTag == UNKNOWN_TAG) ? EC_InvalidTag : EC_Normal;
    return elemTag;
}


// ********************************


ETag DcmTag::getXTag()
{
    errorFlag = (xTag == ET_UNKNOWN) ? EC_InvalidTag : EC_Normal;
    return xTag;
}


// ********************************


short DcmTag::getMaxVM()
{
    errorFlag = (xTag == ET_UNKNOWN) ? EC_InvalidTag : EC_Normal;
    return maxVM;
}


// ********************************


char* DcmTag::getTagName()
{
    char* retval = (char*)NULL;

    if ( xTag >= 0 && xTag < (ETag)DIM_OF(TagVR) )
    {
	errorFlag = EC_Normal;
	retval = TagName[xTag];
    }
    else
    {
	errorFlag = EC_InvalidTag;
	if ( (grpTag & 0x0001) == 0x0001 )
	    retval = PRIVATE_TagName;
	else
	    retval = ERROR_TagName;
    }
    return retval;
}


// ********************************


E_Condition DcmTag::error()
{
    return errorFlag;
}


// ********************************
// *** global constants ***********
// ********************************


DcmTag InternalUseTag( 0x0001, 0xffff );
DcmTag ItemTag( ET_Item );



/*
 *
 * Author: Andreas Barth    Created:  07-17-95
 *                          Modified: 07-17-95
 *
 * Module: dcswap.cc
 *
 * Purpose:
 * ByteOrder functions
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dcswap.h"
#include "dcdebug.h"
#include "dcxfer.h"

// *****************************************************
// *** global variables ********************************
// *****************************************************



E_ByteOrder ByteOrderFlag = EBO_unknown; // modified by
					 //FindMachineByteOrder()


// *****************************************************
// *** functions ***************************************
// *****************************************************


E_ByteOrder MachineByteOrder()
{
    return (ByteOrderFlag != EBO_unknown)
	? ByteOrderFlag : FindMachineByteOrder();
}




E_ByteOrder FindMachineByteOrder()     // modifies ByteOrderFlag
{
Bdebug((2, "dcswap:FindMachineByteOrder()" ));

    E_ByteOrder localByteOrderFlag;
    union
    {
	T_VR_UL ul;
	char uc[4];
    } tl;
    union
    {
	T_VR_US us;
	char uc[2];
    } ts;

    tl.ul = 1;
    ts.us = 1;

    if (      tl.uc[0] == 1 && !(tl.uc[1] | tl.uc[2] | tl.uc[3])
	   && ts.uc[0] == 1 && !(ts.uc[1]) )
	localByteOrderFlag = EBO_LittleEndian;
    else if ( tl.uc[3] == 1 && !(tl.uc[0] | tl.uc[1] | tl.uc[2])
	   && ts.uc[1] == 1 && !(ts.uc[0]) )
	localByteOrderFlag = EBO_BigEndian;
    else
	localByteOrderFlag = EBO_unknown;

    // Achtung Seiteneffekt:
    ByteOrderFlag = localByteOrderFlag;
debug(( 1, "detected MachineByteOrder(u,L,B)=(0,1,2)=%d", ByteOrderFlag ));
Edebug(());

    return localByteOrderFlag;
}


// *****************************************************


E_ByteOrder ConvXferToByteOrder( E_TransferSyntax ts )
{
Bdebug((6, "dcswap:ConvXferToByteOrder(xfer=%d)", ts ));

    E_ByteOrder order;
    switch (ts)
    {
	case EXS_BigEndianExplicit :
	case EXS_BigEndianImplicit :
	    order = EBO_BigEndian;
	    break;
	case EXS_LittleEndianExplicit :
	case EXS_LittleEndianImplicit :
	    order = EBO_LittleEndian;
	    break;
	default:
        {
            DcmXfer xferSyn( ts );     // zeitintensiv, da Suche in Tabelle
            order = xferSyn.isBigEndian() ? EBO_BigEndian : EBO_LittleEndian;
        }
            break;
    }
Vdebug((6, order==EBO_LittleEndian, "use LittleEndian (%d)", order ));
Vdebug((6, order==EBO_BigEndian,    "use BigEndian (%d)", order ));
Edebug(());

    return order;
}


// *****************************************************


char* SwapN(char *Psrc, int P_width, int times )
{
Bdebug((6, "dcswap:SwapN(Psrc,P_width=%d,times=%d)", P_width, times ));

    int    i, t;
    char   *Plast;
    char   *Pstart;
    char   save;

    for ( t = 0; t<times; t++ )
    {
        Pstart = Psrc + P_width * t;

        for ( i = 0; i < P_width/2; i++ )
	{
            Plast = Pstart + P_width - 1 - i;
	    save = *Plast;
	    *Plast = *( Pstart + i );
	    *( Pstart + i ) = save;
	}
    }
Edebug(());

    return Psrc;
}


// *****************************************************


T_VR_US SwapShort( T_VR_US &sval )
{
    union
    {
	T_VR_US us;
	unsigned char uc[2];
    } ts, ts1;

    ts.us = sval;
    ts1.uc[0] = ts.uc[1];
    ts1.uc[1] = ts.uc[0];
    return sval = ts1.us;
}


// *****************************************************


T_VR_UL SwapLong(T_VR_UL &lval)
{
    union
    {
	T_VR_UL ul;
	unsigned char uc[4];
    } tl, tl1;

    tl.ul = lval;
    tl1.uc[0] = tl.uc[3];
    tl1.uc[1] = tl.uc[2];
    tl1.uc[2] = tl.uc[1];
    tl1.uc[3] = tl.uc[0];
    return lval = tl1.ul;
}


// *****************************************************


T_VR_FD SwapDouble(T_VR_FD &dval)
{
    union
    {
	T_VR_FD ud;
	unsigned char uc[4];
    } td, td1;

    td.ud = dval;
    for (int i=0; i<8; i++)
	td1.uc[i] = td.uc[7-i];
    return dval = td1.ud;
}


// *****************************************************



/*
 * 
 * Author: Gerd Ehlers      Created:  04-17-94
 *                          Modified: 02-07-95
 *
 * Module: dcdic.h
 * 
 * Purpose:
 * interface for "ddicenuz.i"
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCDICENU_H
#define DCDICENU_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ddicenuz.i"


typedef enum 
{
   EVR_UNKNOWN=-1,
   EVR_AE=0,
   EVR_AS=1,
   EVR_AT=2,
   EVR_CS=3,
   EVR_DA=4,
   EVR_DS=5,
   EVR_DT=6,
   EVR_FL=7,
   EVR_FD=8,
   EVR_IS=9,
   EVR_LO=10,
   EVR_LT=11,
   EVR_OB=12,
   EVR_OW=13,
   EVR_PN=14,
   EVR_SH=15,
   EVR_SL=16,
   EVR_SQ=17,
   EVR_SS=18,
   EVR_ST=19,
   EVR_TM=20,
   EVR_UI=21,
   EVR_UL=22,
   EVR_US=23,
   EVR_RET=24,
   EVR_na=25,
   EVR_oa=26,
   EVR_oc=27,
   EVR_op=28,
   EVR_squ=29,
   EVR_up=30,
   EVR_ux=31,
   EVR_xs=32,
   EVR_xx=33,

   EVR_item=34,
   EVR_metainfo=35,
   EVR_dataset=36,
   EVR_fileFormat=37,
   EVR_dicomDir=38,
   EVR_dirRecord=39,

   EVR_pixelSQ=40,
   EVR_pixelItem=41

} EVR;



#endif // DCDICENU_H


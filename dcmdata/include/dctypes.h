/*
 *
 * Author: Gerd Ehlers      Created:  03-20-94
 *                          Modified: 02-07-95
 *
 * Module: dctypes.h
 *
 * Purpose:
 * This file contains global type definitions
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCTYPES_H
#define DCTYPES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

typedef unsigned int BOOL;

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif
#ifndef NULL
#define NULL	0
#endif


typedef double		T_VR_FD;
typedef float		T_VR_FL;

#if SIZEOF_LONG == 8
typedef unsigned int	T_VR_UL;
typedef signed int	T_VR_SL;
#else
typedef unsigned long	T_VR_UL;
typedef signed long	T_VR_SL;
#endif

typedef unsigned short	T_VR_US;
typedef signed short	T_VR_SS;

typedef unsigned char	U_CHAR;


typedef enum {
    EXS_UNKNOWN = -1,
    EXS_LittleEndianImplicit = 0,
    EXS_BigEndianImplicit = 1,
    EXS_LittleEndianExplicit = 2,
    EXS_BigEndianExplicit = 3,
    EXS_JPEGProcess1TransferSyntax = 4,
    EXS_JPEGProcess2_4TransferSyntax = 5,
    EXS_JPEGProcess3_5TransferSyntax = 6,
    EXS_JPEGProcess6_8TransferSyntax = 7,
    EXS_JPEGProcess7_9TransferSyntax = 8,
    EXS_JPEGProcess10_12TransferSyntax = 9,
    EXS_JPEGProcess11_13TransferSyntax = 10,
    EXS_JPEGProcess14TransferSyntax = 11,
    EXS_JPEGProcess15TransferSyntax = 12,
    EXS_JPEGProcess16_18TransferSyntax = 13,
    EXS_JPEGProcess17_19TransferSyntax = 14,
    EXS_JPEGProcess20_22TransferSyntax = 15,
    EXS_JPEGProcess21_23TransferSyntax = 16,
    EXS_JPEGProcess24_26TransferSyntax = 17,
    EXS_JPEGProcess25_27TransferSyntax = 18,
    EXS_JPEGProcess28TransferSyntax = 19,
    EXS_JPEGProcess29TransferSyntax = 20,
    EXS_JPEGProcess14SV1TransferSyntax = 21
} E_TransferSyntax;


typedef enum {
    EBO_unknown = 0,
    EBO_LittleEndian = 1,
    EBO_BigEndian = 2
} E_ByteOrder;


typedef enum {
    EVT_Implicit = 0,
    EVT_Explicit = 1
} E_VRType;


typedef enum {
    EJE_NotEncapsulated = 0,
    EJE_Encapsulated = 1
} E_JPEGEncapsulated;


typedef enum {
    EET_ExplicitLength = 0,
    EET_UndefinedLength = 1
} E_EncodingType;


typedef enum {
    EGL_withoutGL = 0,
    EGL_withGL = 1
} E_GrpLenEncoding;


typedef enum {
    ESM_fromHere = 0,
    ESM_fromStackTop = 1,
    ESM_afterStackTop = 2
} E_SearchMode;


typedef enum {
    ERW_init = 0,
    ERW_ready = 1,
    ERW_inWork = 2
} E_ReadWriteState;


typedef enum {
    EC_Normal = 0,
    EC_InvalidTag = 1,
    EC_TagNotFound = 2,
    EC_InvalidVR = 3,
    EC_InvalidStream = 4,
    EC_EndOfFile = 5,
    EC_CorruptedData = 6,
    EC_IllegalCall = 7,
    EC_SequEnd = 8,
    EC_DoubledTag = 9,
    EC_BufferFull = 10,
    EC_EndOfBuffer = 11
} E_Condition;


#define DICOM_TRANSFERLITTLEENDIAN		"1.2.840.10008.1.2"
#define DICOM_TRANSFERLITTLEENDIANEXPLICIT      "1.2.840.10008.1.2.1"
#define DICOM_TRANSFERBIGENDIANEXPLICIT         "1.2.840.10008.1.2.2"

#define DICOM_JPEGProcess1TransferSyntax        "1.2.840.10008.1.2.4.50"
#define DICOM_JPEGProcess2_4TransferSyntax      "1.2.840.10008.1.2.4.51"
#define DICOM_JPEGProcess3_5TransferSyntax      "1.2.840.10008.1.2.4.52"
#define DICOM_JPEGProcess6_8TransferSyntax      "1.2.840.10008.1.2.4.53"
#define DICOM_JPEGProcess7_9TransferSyntax      "1.2.840.10008.1.2.4.54"
#define DICOM_JPEGProcess10_12TransferSyntax    "1.2.840.10008.1.2.4.55"
#define DICOM_JPEGProcess11_13TransferSyntax    "1.2.840.10008.1.2.4.56"
#define DICOM_JPEGProcess14TransferSyntax       "1.2.840.10008.1.2.4.57"
#define DICOM_JPEGProcess15TransferSyntax       "1.2.840.10008.1.2.4.58"
#define DICOM_JPEGProcess16_18TransferSyntax    "1.2.840.10008.1.2.4.59"
#define DICOM_JPEGProcess17_19TransferSyntax    "1.2.840.10008.1.2.4.60"
#define DICOM_JPEGProcess20_22TransferSyntax    "1.2.840.10008.1.2.4.61"
#define DICOM_JPEGProcess21_23TransferSyntax    "1.2.840.10008.1.2.4.62"
#define DICOM_JPEGProcess24_26TransferSyntax    "1.2.840.10008.1.2.4.63"
#define DICOM_JPEGProcess25_27TransferSyntax    "1.2.840.10008.1.2.4.64"
#define DICOM_JPEGProcess28TransferSyntax       "1.2.840.10008.1.2.4.65"
#define DICOM_JPEGProcess29TransferSyntax       "1.2.840.10008.1.2.4.66"
#define DICOM_JPEGProcess14SV1TransferSyntax    "1.2.840.10008.1.2.4.70"

#define DIC_EURO_UIDROOT                        "1.2.276.0.7230010.100"
#define DIC_EURO_IMPLEMENTATIONCLASSUID		"1.2.276.0.7230010.100.1.1"
#define DIC_EURO_UIDROOT_INFIX                  "958"

#define DIC_EURO_STUDYUID_ROOT			"1.2.276.0.7230010.100.2"
#define DIC_EURO_SERIESUID_ROOT			"1.2.276.0.7230010.100.3"
#define DIC_EURO_INSTANCEUID_ROOT		"1.2.276.0.7230010.100.4"
#define DIC_EURO_FRAMEOFREFERENCEUID_ROOT	"1.2.276.0.7230010.100.5"
#define DIC_EURO_INSTANCECREATORUID_ROOT	"1.2.276.0.7230010.100.6"
#define DIC_EURO_FILESETUID_ROOT		"1.2.276.0.7230010.100.7"

#define DIC_EURO_DTK_IMPLEMENTATIONVERSIONNAME  "OOP-DTK-1.3"


#endif // DCTYPES_H


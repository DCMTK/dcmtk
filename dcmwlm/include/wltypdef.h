/*
 *
 *  Copyright (C) 1996-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens, Andrew Hewett
 *
 *  Purpose: Type definitions and macros for dcmwlm project.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 16:31:00 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wltypdef.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmTypeDefinitions_h
#define WlmTypeDefinitions_h

enum WlmDataSourceStatusType
{
  WLM_REFUSED_OUT_OF_RESOURCES = 0xa700,
  WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS = 0xa900,
  WLM_FAILED_UNABLE_TO_PROCESS = 0xc000,
  WLM_CANCEL = 0xfe00,
  WLM_SUCCESS = 0x0000,
  WLM_PENDING = 0xff00,
  WLM_PENDING_WARNING = 0xff01
};

enum WlmObjectStatusType
{
  WLM_STATUS_UNKNOWN,
  WLM_STATUS_INIT_FAILED,
  WLM_STATUS_OK
};

enum WlmDataSourceType
{
  DATA_SOURCE_IS_DATABASE,
  DATA_SOURCE_IS_DATA_FILES
};

#define LOCKFILENAME "lockfile"

struct WlmProcessSlotType
{
  DIC_NODENAME peerName;
  DIC_AE callingAETitle;
  DIC_AE calledAETitle;
  int processId;
  time_t startTime;
  OFBool hasStorageAbility;
  WlmProcessSlotType *next;
};

struct WlmProcessTableType
{
  int pcnt;
  WlmProcessSlotType *plist;
};

enum WlmRefuseReasonType
{
  WLM_TOO_MANY_ASSOCIATIONS,
  WLM_CANNOT_FORK,
  WLM_BAD_APP_CONTEXT,
  WLM_BAD_AE_PEER,
  WLM_BAD_AE_SERVICE,
  WLM_FORCED,
  WLM_NO_IC_UID
};

const OFConditionConst ECC_InitializationOfDataSourceFailed        ( OFM_dcmwlm,  1, OF_error, "Initialization of data source failed.");
const OFConditionConst ECC_DataSourceNotAvailable                  ( OFM_dcmwlm,  2, OF_error, "Specified data source not available.");
const OFConditionConst ECC_InsufficientPortPrivileges              ( OFM_dcmwlm,  3, OF_error, "Insufficient privileges to listen to port.");
const OFConditionConst ECC_InitializationOfNetworkConnectionFailed ( OFM_dcmwlm,  4, OF_error, "Initialization of network connection failed.");
const OFConditionConst ECC_TerminationOfNetworkConnectionFailed    ( OFM_dcmwlm,  5, OF_error, "Termination of network connection failed.");

const OFCondition WLM_EC_InitializationOfDataSourceFailed          ( ECC_InitializationOfDataSourceFailed );
const OFCondition WLM_EC_DataSourceNotAvailable                    ( ECC_DataSourceNotAvailable );
const OFCondition WLM_EC_InsufficientPortPrivileges                ( ECC_InsufficientPortPrivileges );
const OFCondition WLM_EC_InitializationOfNetworkConnectionFailed   ( ECC_InitializationOfNetworkConnectionFailed );
const OFCondition WLM_EC_TerminationOfNetworkConnectionFailed      ( ECC_TerminationOfNetworkConnectionFailed );

#endif

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
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-04-18 14:20:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wltypdef.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmTypeDefinitions_h
#define WlmTypeDefinitions_h

  /** Data source status types
   */
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

  /** Data source types
   */
enum WlmDataSourceType
{
  DATA_SOURCE_IS_DATABASE,
  DATA_SOURCE_IS_DATA_FILES,
  DATA_SOURCE_IS_PKI_FILE
};

  /** Database types
   */
enum WlmDatabaseType
{
  DATABASE_TYPE_UNKNOWN,
  DATABASE_ORACLE
};

  /** Returned Character Set types
   */
enum WlmReturnedCharacterSetType
{
  RETURN_NO_CHARACTER_SET,
  RETURN_CHARACTER_SET_ISO_IR_100
};

  // name of lockfile for worklist management based on file system
#define LOCKFILENAME "lockfile"

  // structure for non-single process mode
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

  // structure for non-single process mode
struct WlmProcessTableType
{
  int pcnt;
  WlmProcessSlotType *plist;
};

  /** Reason for refusing association types
   */
enum WlmRefuseReasonType
{
  WLM_TOO_MANY_ASSOCIATIONS,
  WLM_CANNOT_FORK,
  WLM_BAD_APP_CONTEXT,
  WLM_BAD_AE_SERVICE,
  WLM_FORCED,
  WLM_NO_IC_UID
};

  // const objects for error objects below
const OFConditionConst ECC_InsufficientPortPrivileges              ( OFM_dcmwlm,  1, OF_error, "Insufficient privileges to listen to port.");
const OFConditionConst ECC_InitializationOfNetworkConnectionFailed ( OFM_dcmwlm,  2, OF_error, "Initialization of network connection failed.");
const OFConditionConst ECC_TerminationOfNetworkConnectionFailed    ( OFM_dcmwlm,  3, OF_error, "Termination of network connection failed.");
const OFConditionConst ECC_DatabaseStatementConfigFilesNotExistent ( OFM_dcmwlm,  4, OF_error, "Database statement configuration files not existent.");
const OFConditionConst ECC_CannotConnectToDataSource               ( OFM_dcmwlm,  5, OF_error, "Cannot connect to data source.");

  /// error: Insufficient privileges to listen to port.
const OFCondition WLM_EC_InsufficientPortPrivileges                ( ECC_InsufficientPortPrivileges );

  /// error: Initialization of network connection failed.
const OFCondition WLM_EC_InitializationOfNetworkConnectionFailed   ( ECC_InitializationOfNetworkConnectionFailed );

  /// error: Termination of network connection failed.
const OFCondition WLM_EC_TerminationOfNetworkConnectionFailed      ( ECC_TerminationOfNetworkConnectionFailed );

  /// error: Database statement configuration files not existent.
const OFCondition WLM_EC_DatabaseStatementConfigFilesNotExistent   ( ECC_DatabaseStatementConfigFilesNotExistent );

  /// error: Cannot connect to data source.
const OFCondition WLM_EC_CannotConnectToDataSource                 ( ECC_CannotConnectToDataSource );


#endif

/*
** CVS Log
** $Log: wltypdef.h,v $
** Revision 1.3  2002-04-18 14:20:09  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
**
**
*/

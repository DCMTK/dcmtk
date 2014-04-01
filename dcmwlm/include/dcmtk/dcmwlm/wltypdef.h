/*
 *
 *  Copyright (C) 1996-2014, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Type definitions and macros for dcmwlm project.
 *
 */

#ifndef WlmTypeDefinitions_h
#define WlmTypeDefinitions_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmnet/dicom.h"     /* for DIC_AE and DIC_NODENAME */
#include "dcmtk/dcmwlm/wldefine.h"

// include this file in doxygen documentation

/** @file wltypdef.h
 *  @brief type definitions and macros for the dcmwlm module
 */


/// Data source status types
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

/// Data source types
enum WlmDataSourceType
{
  DATA_SOURCE_IS_DATABASE,
  DATA_SOURCE_IS_DATA_FILES,
  DATA_SOURCE_IS_PKI_FILE
};

/// Database types
enum WlmDatabaseType
{
  WLM_DATABASE_TYPE_UNKNOWN,
  WLM_DATABASE_ORACLE
};

/// Returned Character Set types
enum WlmReturnedCharacterSetType
{
  RETURN_NO_CHARACTER_SET,
  RETURN_CHARACTER_SET_ISO_IR_100,
  RETURN_CHARACTER_SET_FROM_FILE
};

/// name of lockfile for worklist management based on file system
#define LOCKFILENAME "lockfile"

/// structure for non-single process mode
struct DCMTK_DCMWLM_EXPORT WlmProcessSlotType
{
  /// name of peer
  DIC_NODENAME peerName;
  /// calling AE title
  DIC_AE callingAETitle;
  /// called AE title
  DIC_AE calledAETitle;
  /// process id
  int processId;
  /// start time
  time_t startTime;
  /// indicator if process has storage ability
  OFBool hasStorageAbility;
};

/// Reason for refusing association types
enum WlmRefuseReasonType
{
  WLM_TOO_MANY_ASSOCIATIONS,
  WLM_CANNOT_FORK,
  WLM_BAD_APP_CONTEXT,
  WLM_BAD_AE_SERVICE,
  WLM_FORCED,
  WLM_NO_IC_UID
};

/// const objects for error objects
makeOFConditionConst(WLM_EC_InsufficientPortPrivileges,              OFM_dcmwlm,  1, OF_error, "Insufficient privileges to listen to port.");
makeOFConditionConst(WLM_EC_InitializationOfNetworkConnectionFailed, OFM_dcmwlm,  2, OF_error, "Initialization of network connection failed.");
makeOFConditionConst(WLM_EC_TerminationOfNetworkConnectionFailed,    OFM_dcmwlm,  3, OF_error, "Termination of network connection failed.");
makeOFConditionConst(WLM_EC_DatabaseStatementConfigFilesNotExistent, OFM_dcmwlm,  4, OF_error, "Database statement configuration files not existent.");
makeOFConditionConst(WLM_EC_CannotConnectToDataSource,               OFM_dcmwlm,  5, OF_error, "Cannot connect to data source.");

/// number of currently supported matching key attributes
#define NUMBER_OF_SUPPORTED_MATCHING_KEY_ATTRIBUTES 17

/// structure for handling arbitrary types of superior sequences in query result datasets
struct DCMTK_DCMWLM_EXPORT WlmSuperiorSequenceInfoType
{
  WlmSuperiorSequenceInfoType() : sequenceTag(), numOfItems(0), currentItem(0)
  { }

  /// pointer to sequence element
  DcmTagKey sequenceTag;
  /// number of items within this sequence element
  unsigned long numOfItems;
  /// currently processed item
  unsigned long currentItem;
};


#endif

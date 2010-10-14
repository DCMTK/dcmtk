/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:39 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmTypeDefinitions_h
#define WlmTypeDefinitions_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmnet/dicom.h"     /* for DIC_AE and DIC_NODENAME */

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
struct WlmProcessSlotType
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

  /// const objects for error objects below
const OFConditionConst WLM_ECC_InsufficientPortPrivileges              ( OFM_dcmwlm,  1, OF_error, "Insufficient privileges to listen to port.");
const OFConditionConst WLM_ECC_InitializationOfNetworkConnectionFailed ( OFM_dcmwlm,  2, OF_error, "Initialization of network connection failed.");
const OFConditionConst WLM_ECC_TerminationOfNetworkConnectionFailed    ( OFM_dcmwlm,  3, OF_error, "Termination of network connection failed.");
const OFConditionConst WLM_ECC_DatabaseStatementConfigFilesNotExistent ( OFM_dcmwlm,  4, OF_error, "Database statement configuration files not existent.");
const OFConditionConst WLM_ECC_CannotConnectToDataSource               ( OFM_dcmwlm,  5, OF_error, "Cannot connect to data source.");

  /// error: Insufficient privileges to listen to port.
const OFCondition WLM_EC_InsufficientPortPrivileges                ( WLM_ECC_InsufficientPortPrivileges );

  /// error: Initialization of network connection failed.
const OFCondition WLM_EC_InitializationOfNetworkConnectionFailed   ( WLM_ECC_InitializationOfNetworkConnectionFailed );

  /// error: Termination of network connection failed.
const OFCondition WLM_EC_TerminationOfNetworkConnectionFailed      ( WLM_ECC_TerminationOfNetworkConnectionFailed );

  /// error: Database statement configuration files not existent.
const OFCondition WLM_EC_DatabaseStatementConfigFilesNotExistent   ( WLM_ECC_DatabaseStatementConfigFilesNotExistent );

  /// error: Cannot connect to data source.
const OFCondition WLM_EC_CannotConnectToDataSource                 ( WLM_ECC_CannotConnectToDataSource );

  /// number of currently supported matching key attributes
#define NUMBER_OF_SUPPORTED_MATCHING_KEY_ATTRIBUTES 15

  /// structure for handling arbitrary types of superior sequences in query result datasets
struct WlmSuperiorSequenceInfoType
{
  /// pointer to sequence element
  DcmTagKey sequenceTag;
  /// number of items within this sequence element
  unsigned long numOfItems;
  /// currently processed item
  unsigned long currentItem;
};


#endif

/*
** CVS Log
** $Log: wltypdef.h,v $
** Revision 1.15  2010-10-14 13:16:39  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.14  2009-09-30 08:40:34  uli
** Make dcmwlm's include headers self-sufficient by including all
** needed headers directly.
**
** Revision 1.13  2007-08-10 14:25:21  meichel
** Added new command line option --keep-char-set that returns
**   any specific character set as encoded in the worklist file.
**
** Revision 1.12  2006/12/15 14:49:22  onken
** Removed excessive use char* and C-array in favour of OFString and
** OFList. Simplified some implementation details.
**
** Revision 1.11  2005/12/08 16:05:44  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.10  2005/09/23 12:56:40  wilkens
** Added attribute PatientsBirthDate as a matching key attribute to wlmscpfs.
** Thanks to Andre M. Descombes <andre@descombes.info> for the code template.
**
** Revision 1.9  2004/01/07 08:32:28  wilkens
** Added new sequence type return key attributes to wlmscpfs. Fixed bug that for
** equally named attributes in sequences always the same value will be returned.
** Added functionality that also more than one item will be returned in sequence
** type return key attributes.
**
** Revision 1.8  2003/12/23 13:04:36  wilkens
** Integrated new matching key attributes into wlmscpfs.
**
** Revision 1.7  2003/07/02 09:17:55  wilkens
** Updated documentation to get rid of doxygen warnings.
**
** Revision 1.6  2002/12/16 11:08:36  wilkens
** Added missing #include "osconfig.h" to certain files.
**
** Revision 1.5  2002/08/12 10:56:09  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.4  2002/07/17 13:10:37  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.3  2002/04/18 14:20:09  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
**
**
*/

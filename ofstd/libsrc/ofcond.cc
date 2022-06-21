/*
 *
 *  Copyright (C) 2001-2015, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose: class OFCondition and helper classes
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"


/* ---------- global condition constants ---------- */

makeOFConditionConst(EC_Normal,                        0,  0, OF_ok,      "Normal");
makeOFConditionConst(EC_IllegalParameter,              0,  1, OF_error,   "Illegal parameter");
makeOFConditionConst(EC_MemoryExhausted,               0,  2, OF_failure, "Virtual Memory exhausted");
makeOFConditionConst(EC_NoEncodingLibrary,             0,  3, OF_error,   "No character encoding library available");
makeOFConditionConst(EC_NoEncodingSelected,            0,  4, OF_error,   "No character encoding selected");
// codes 5..8 are used for dynamically created error messages (see constants below)
makeOFConditionConst(EC_CouldNotCreateTemporaryFile,   0,  9, OF_error,   "Could not create temporary file");
makeOFConditionConst(EC_InvalidFilename,               0, 10, OF_error,   "Invalid filename");
makeOFConditionConst(EC_DirectoryDoesNotExist,         0, 11, OF_error,   "Directory does not exist");
makeOFConditionConst(EC_DirectoryNotWritable,          0, 12, OF_error,   "Directory is not writable");
// code 13 is for dynamically created error messages (see constant below)
makeOFConditionConst(EC_CouldNotGenerateFilename,      0, 14, OF_error,   "Could not generate filename");
makeOFConditionConst(EC_CouldNotGenerateDirectoryName, 0, 15, OF_error,   "Could not generate directory name");
makeOFConditionConst(EC_setuidFailed,                  0, 16, OF_error,   "Call to setuid() failed");
makeOFConditionConst(EC_NotYetImplemented,             0, 17, OF_error,   "Not yet implemented");

makeOFConditionConst(EC_IPCMessageQueueExists,         0, 18, OF_error,   "IPC message queue exists");
makeOFConditionConst(EC_IPCMessageNoQueue,             0, 19, OF_error,   "IPC message no queue");
makeOFConditionConst(EC_IPCMessageQueueFailure,        0, 20, OF_error,   "IPC message queue failure");
makeOFConditionConst(EC_IPCMessageQueueEmpty,          0, 21, OF_error,   "IPC message queue empty");
makeOFConditionConst(EC_IPCEmptyMessage,               0, 22, OF_error,   "IPC empty message");


// NOTE:
// error codes 1024..2047 are reserved for inter-process communication
// errors defined elsewhere in module ofstd

const unsigned short EC_CODE_CannotOpenEncoding     = 5;
const unsigned short EC_CODE_CannotCloseEncoding    = 6;
const unsigned short EC_CODE_CannotConvertEncoding  = 7;
const unsigned short EC_CODE_CannotControlConverter = 8;
const unsigned short EC_CODE_CannotCreateDirectory  = 13;

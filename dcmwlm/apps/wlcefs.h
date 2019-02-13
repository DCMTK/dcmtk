/*
 *
 *  Copyright (C) 1996-2019, OFFIS e.V.
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
 *  Purpose: Class representing a console engine for basic worklist
 *           management service class providers based on the file system.
 *
 */

#ifndef WlmConsoleEngineFileSystem_h
#define WlmConsoleEngineFileSystem_h

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dimse.h"

class WlmDataSource;
class OFConsoleApplication;
class OFCommandLine;

/** This class encapsulates data structures and operations for a console application that
 *  can act as a basic worklist management service class provider based on the file system.
 */
class WlmConsoleEngineFileSystem
{
  protected:
    /// contains application's id string
    char rcsid[200];
    /// returned character set type
    WlmReturnedCharacterSetType opt_returnedCharacterSet;
    /// path to database files
    OFString opt_dfPath;
    /// port on which this application is listening
    OFCmdUnsignedInt opt_port;
    /// indicates if incoming associations shall be refused or not
    OFBool opt_refuseAssociation;
    /// indicates if incoming associations shall be refused if no implementation class uid is specified
    OFBool opt_rejectWithoutImplementationUID;
    /// indicates how long the application shall sleep after a find
    OFCmdUnsignedInt opt_sleepAfterFind;
    /// indicates how long the application shall sleep during a find
    OFCmdUnsignedInt opt_sleepDuringFind;
    /// max PDU size
    OFCmdUnsignedInt opt_maxPDU;
    /// preferred network transfer syntax
    E_TransferSyntax opt_networkTransferSyntax;
    /// indicates if find shall fail on an invalid query or not
    OFBool opt_failInvalidQuery;
    /// indicates if this application is run in single process mode or not
    OFBool opt_singleProcess;
    /// indicates if this process is called as a child process, used by dcmnet
    OFBool opt_forkedChild;
    /// indicates how many associations can be accepted at the same time
    int opt_maxAssociations;
    /// indicates if an expansion of empty sequences in C-Find RQ messages shall take place or not
    OFBool opt_noSequenceExpansion;
    /// indicates if wl-files which are lacking return type 1 attributes or information in such attributes shall be rejected or not
    OFBool opt_enableRejectionOfIncompleteWlFiles;
    /// blocking mode for DIMSE operations
    T_DIMSE_BlockingMode opt_blockMode;
    /// timeout for DIMSE operations
    int opt_dimse_timeout;
    /// timeout for ACSE operations
    int opt_acse_timeout;
    /// instance of console application class (for handling command line arguments)
    OFConsoleApplication *app;
    /// instance of command line class (for handling command line arguments)
    OFCommandLine *cmd;
    /// number of command line arguments, needed for multiprocess mode on WIN32
    int command_argc;
    /// complete command line, needed for multiprocess mode on WIN32
    char **command_argv;
    /// data source which shall be queried on incoming C-Find RQ messages
    WlmDataSource *dataSource;

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmConsoleEngineFileSystem( const WlmConsoleEngineFileSystem &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmConsoleEngineFileSystem &operator=( const WlmConsoleEngineFileSystem &Src );

  public:
      /** constructor.
       *  @param argc            Number of arguments that were passed to main.
       *  @param argv            Arguments that were passed to main.
       *  @param applicationName The name of this application.
       *  @param dataSourcev     Object which provides access to the data source.
       */
    WlmConsoleEngineFileSystem( int argc, char *argv[], const char *applicationName, WlmDataSource *dataSourcev );

      /** destructor
       */
    ~WlmConsoleEngineFileSystem();

      /** Starts providing the implemented service for calling SCUs.
       *  After having created an instance of this class, this function
       *  shall be called from main.
       *  @return Return value that is supposed to be returned from main().
       */
    int StartProvidingService();
};

#endif

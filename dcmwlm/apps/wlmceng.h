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
 *  Purpose: Class representing a console engine for basic worklist
 *           management service class providers.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-04-18 14:19:56 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wlmceng.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmConsoleEngine_h
#define WlmConsoleEngine_h

class WlmDataSource;
class OFConsoleApplication;
class OFCommandLine;

  /** This class encapsulates data structures and operations for a console application that
   *  can act as a basic worklist management service class provider.
   */
class WlmConsoleEngine
{

  protected:
    WlmDataSourceType dataSourceType;
    char rcsid[200];
    const char *opt_dbDsn;
    const char *opt_dbUserName;
    const char *opt_dbUserPassword;
    const char *opt_cfgFileMatchRecords;
    const char *opt_cfgFileSelectValues;
    WlmDatabaseType opt_databaseType;
    WlmReturnedCharacterSetType opt_returnedCharacterSet;
    OFCmdUnsignedInt opt_serialNumber;
    const char *opt_dfPath;
    const char *opt_pfFileName;
    OFCmdUnsignedInt opt_port;
    OFBool opt_refuseAssociation;
    OFBool opt_rejectWithoutImplementationUID;
    OFCmdUnsignedInt opt_sleepAfterFind;
    OFCmdUnsignedInt opt_sleepDuringFind;
    OFCmdUnsignedInt opt_maxPDU;
    E_TransferSyntax opt_networkTransferSyntax;
    OFBool opt_verbose;
    OFBool opt_debug;
    OFBool opt_failInvalidQuery;
    OFBool opt_singleProcess;
    int opt_maxAssociations;
    OFConsoleApplication *app;
    OFCommandLine *cmd;
    WlmDataSource *dataSource;

  public:
      /** constructor.
       *  @param argc            Number of arguments that were passed to main.
       *  @param argv            Arguments that were passed to main.
       *  @param dataSourceTypev The type of datasource.
       *  @param applicationName The name of this application.
       *  @param dataSourcev     Object which provides access to the data source.
       */
    WlmConsoleEngine( int argc, char *argv[], WlmDataSourceType dataSourceTypev, const char *applicationName, WlmDataSource *dataSourcev );

      /** destructor
       */
    ~WlmConsoleEngine();

      /** Starts providing the implemented service for calling SCUs.
       *  After having created an instance of this class, this function
       *  shall be called from main.
       *  @return Return value that is supposed to be returned from main().
       */
    int StartProvidingService();
};

#endif

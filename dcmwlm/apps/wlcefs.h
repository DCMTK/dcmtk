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
 *  Author:  Thomas Wilkens
 *
 *  Purpose: Class representing a console engine for basic worklist
 *           management service class providers based on the file system.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-08-05 09:09:17 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/wlcefs.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmConsoleEngineFileSystem_h
#define WlmConsoleEngineFileSystem_h

class WlmDataSource;
class OFConsoleApplication;
class OFCommandLine;

/** This class encapsulates data structures and operations for a console application that
 *  can act as a basic worklist management service class provider based on the file system.
 */
class WlmConsoleEngineFileSystem
{
  protected:
    char rcsid[200];
    WlmReturnedCharacterSetType opt_returnedCharacterSet;
    const char *opt_dfPath;
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
    OFBool opt_noSequenceExpansion;
    OFConsoleApplication *app;
    OFCommandLine *cmd;
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

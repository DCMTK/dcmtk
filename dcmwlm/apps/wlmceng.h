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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 17:13:16 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wlmceng.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmConsoleEngine_h
#define WlmConsoleEngine_h

class WlmConsoleEngine
// Date   : December 17, 2001
// Author : Thomas Wilkens
// Task   : This class encapsulates data structures and operations for a console application that
//          can act as a basic worklist management service class provider.
{

  protected:
    WlmDataSourceType dataSourceType;
    char rcsid[200];
    const char *opt_dbDsn;
    const char *opt_dbUserName;
    const char *opt_dbUserPassword;
    OFCmdUnsignedInt opt_serialNumber;
    const char *opt_dfPath;
    OFCmdUnsignedInt opt_port;
    OFBool opt_refuseAssociation;
    OFBool opt_rejectWithoutImplementationUID;
    OFCmdUnsignedInt opt_sleepAfterFind;
    OFCmdUnsignedInt opt_sleepDuringFind;
    OFCmdUnsignedInt opt_maxPDU;
    E_TransferSyntax opt_networkTransferSyntax;
    E_GrpLenEncoding opt_groupLength;
    E_EncodingType opt_sequenceType;
    OFBool opt_verbose;
    OFBool opt_debug;
    OFBool opt_failInvalidQuery;
    OFBool opt_singleProcess;
    int opt_maxAssociations;
    OFConsoleApplication *app;
    OFCommandLine *cmd;

  public:
    // Constructor/Destructor
    WlmConsoleEngine( int argc, char *argv[], WlmDataSourceType dataSourceTypev, const char *applicationName );
    ~WlmConsoleEngine();

    int StartProvidingService();
      // Task         : Starts providing the implemented service for calling SCUs.
      //                After having created an instance of this class, this function
      //                shall be called from main.
      // Parameters   : none.
      // Return Value : Return value that is supposed to be returned from main().
};

#endif

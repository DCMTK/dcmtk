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
 *  Purpose: Activity manager class for basic worklist management service
 *           class provider engines.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 17:45:34 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wlmactmg.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmActivityManager_h
#define WlmActivityManager_h

class WlmDataSource;
class OFCondition;
class OFConsole;

class WlmActivityManager
// Date   : December 10, 2001
// Author : Thomas Wilkens
// Task   : This class encapsulates data structures and operations for basic worklist management service
//          class providers.
{
  protected:
    WlmDataSourceType dataSourceType;
    WlmProcessTableType processTable;
    char **supportedAbstractSyntaxes;
    int numberOfSupportedAbstractSyntaxes;
    WlmDataSource *dataSource;
    char *opt_dbDsn;
    char *opt_dbUserName;
    char *opt_dbUserPassword;
    char *opt_dfPath;
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
    OFConsole *logStream;
    const int opt_serialNumber;

    void DumpMessage( const char *message );
    void WaitForAssociation( T_ASC_Network *net );
    void CleanChildren();
    OFCondition NegotiateAssociation( T_ASC_Association *assoc );
    void AddProcessToTable( int pid, T_ASC_Association * assoc );
    int CountChildProcesses();
    void RemoveProcessFromTable( int pid );
    void RefuseAssociation( T_ASC_Association **assoc, WlmRefuseReasonType reason );
    void HandleAssociation( T_ASC_Association *assoc );
    OFCondition ReceiveAndHandleCommands( T_ASC_Association *assoc );
    OFCondition HandleEchoSCP( T_ASC_Association *assoc, T_DIMSE_C_EchoRQ *req, T_ASC_PresentationContextID presId );
    OFCondition HandleFindSCP( T_ASC_Association *assoc, T_DIMSE_C_FindRQ *request, T_ASC_PresentationContextID presID );

  public:
    //Constructor/Destructor
    WlmActivityManager( WlmDataSourceType dataSourceTypev, const char *opt_dbDsnv, const char *opt_dbUserNamev, const char *opt_dbUserPasswordv, const char *opt_dfPathv, OFCmdUnsignedInt opt_port, OFBool opt_refuseAssociation, OFBool opt_rejectWithoutImplementationUID, OFCmdUnsignedInt opt_sleepAfterFind, OFCmdUnsignedInt opt_sleepDuringFind, OFCmdUnsignedInt opt_maxPDU, E_TransferSyntax opt_networkTransferSyntax, E_GrpLenEncoding opt_groupLength, E_EncodingType opt_sequenceType, OFBool opt_verbose, OFBool opt_debug, OFBool opt_failInvalidQuery, OFBool opt_singleProcess, int opt_maxAssociations, OFConsole *logStreamv, const int serialNumberv );
    ~WlmActivityManager();

    OFCondition StartProvidingService();
    // Task         : Starts providing the implemented service for calling SCUs.
    //                After having created an instance of this class, this function
    //                shall be called from main.
    // Parameters   : none.
    // Return Value : Return value that is supposed to be returned from main().
};

#endif

/*
** CVS Log
** $Log: wlmactmg.h,v $
** Revision 1.3  2002-01-08 17:45:34  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 17:35:39  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/

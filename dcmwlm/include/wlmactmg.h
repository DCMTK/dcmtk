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
 *  Purpose: Activity manager class for basic worklist management service
 *           class provider engines.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-08-05 09:09:59 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wlmactmg.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
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

/** This class encapsulates data structures and operations for basic worklist management service
 *  class providers.
 */
class WlmActivityManager
{
  protected:
    WlmDataSource *dataSource;
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
    char **supportedAbstractSyntaxes;
    int numberOfSupportedAbstractSyntaxes;
    OFConsole *logStream;
    WlmProcessTableType processTable;

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

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmActivityManager( const WlmActivityManager &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmActivityManager &operator=( const WlmActivityManager &Src );


  public:
      /** constructor.
       *  @param dataSourcev                         Pointer to the data source which shall be used.
       *  @param opt_portv                           The port on which the application is supposed to listen.
       *  @param opt_refuseAssociationv              Specifies if an association shall always be refused by the SCP.
       *  @param opt_rejectWithoutImplementationUIDv Specifies if the application shall reject an association if no implementation class UID is provided by the calling SCU.
       *  @param opt_sleepAfterFindv                 Specifies how many seconds the application is supposed to sleep after having handled a C-FIND-Rsp.
       *  @param opt_sleepDuringFindv                Specifies how many seconds the application is supposed to sleep during the handling of a C-FIND-Rsp.
       *  @param opt_maxPDUv                         Maximum length of a PDU that can be received in bytes.
       *  @param opt_networkTransferSyntaxv          Specifies the preferred network transfer syntaxes.
       *  @param opt_verbosev                        Specifies if the application shall print processing details or not.
       *  @param opt_debugv                          Specifies if the application shall print debug information.
       *  @param opt_failInvalidQueryv               Specifies if the application shall fail on an invalid query.
       *  @param opt_singleProcessv                  Specifies if the application shall run in a single process.
       *  @param opt_maxAssociationsv                Specifies many concurrent associations the application shall be able to handle.
       *  @param logStreamv                          A stream information can be dumped to.
       */
    WlmActivityManager( WlmDataSource *dataSourcev, OFCmdUnsignedInt opt_portv, OFBool opt_refuseAssociationv, OFBool opt_rejectWithoutImplementationUIDv, OFCmdUnsignedInt opt_sleepAfterFindv, OFCmdUnsignedInt opt_sleepDuringFindv, OFCmdUnsignedInt opt_maxPDUv, E_TransferSyntax opt_networkTransferSyntaxv, OFBool opt_verbosev, OFBool opt_debugv, OFBool opt_failInvalidQueryv, OFBool opt_singleProcessv, int opt_maxAssociationsv, OFConsole *logStreamv );

      /** destructor
       */
    ~WlmActivityManager();

      /** Starts providing the implemented service for calling SCUs.
       *  After having created an instance of this class, this function
       *  shall be called from main.
       *  @return Value that is supposed to be returned from main().
       */
    OFCondition StartProvidingService();
};

#endif

/*
** CVS Log
** $Log: wlmactmg.h,v $
** Revision 1.8  2002-08-05 09:09:59  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.7  2002/07/17 13:10:37  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.6  2002/06/10 11:25:06  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.5  2002/04/18 14:20:09  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.4  2002/01/08 19:10:04  joergr
** Minor adaptations to keep the gcc compiler on Linux and Solaris happy.
** Currently only the "file version" of the worklist SCP is supported on
** Unix systems.
**
** Revision 1.3  2002/01/08 17:45:34  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 17:35:39  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/

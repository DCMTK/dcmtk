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
 *  Purpose: Class for connecting to a database-based data source.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 17:38:15 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wldsdb.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

class WlmDatabaseInteractionManager;
class OFConsole;

class WlmDataSourceDatabase : public WlmDataSource
// Date   : December 10, 2001
// Author : Thomas Wilkens
// Task   : This class encapsulates data structures and operations for connecting to a database-based
//          data source in the framework of the DICOM basic worklist management service.
{
  protected:
    char *dbDsn;
    char *dbUserName;
    char *dbUserPassword;
    const int opt_serialNumber;
    WlmDatabaseInteractionManager *databaseInteractionManager;
    DcmDataset **matchingDatasets;
    unsigned long numOfMatchingDatasets;

    int SetReadlock();
    int ReleaseReadlock();
    OFBool IsSupportedMatchingKeyAttribute( const DcmTagKey &key, int level );
    OFBool IsSupportedReturnKeyAttribute( const DcmTagKey &key, int level );
    void DetermineMatchingKeyAttributeValues( char **&matchingKeyValues, unsigned long &numOfMatchingKeyValues );
    void HandleNonSequenceElementInResultDataset( DcmItem *matchingDataset, DcmElement *&element, long matchingRecordID );
    void HandleSequenceElementInResultDataset( DcmItem *matchingDataset, DcmElement *&element, long matchingRecordID );

  public:
    // Constructor/Destructor
    WlmDataSourceDatabase( OFConsole *logStreamv, const OFBool verbose, char *dbDsnv, char *dbUserNamev, char *dbUserPasswordv, const int serialNumberv );
    ~WlmDataSourceDatabase();

    // Check if the specified data source is available.
    OFBool IsDataSourceAvailable();

    // Check if the called application entity title is supported whithin the data source
    OFBool IsCalledApplicationEntityTitleSupported();

    // Determine the records that match the search mask.
    WlmDataSourceStatusType StartFindRequest( DcmDataset &findRequestIdentifiers );

    // Get the next matching record/data set
    DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus );
};


/*
** CVS Log
** $Log: wldsdb.h,v $
** Revision 1.3  2002-01-08 17:38:15  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/

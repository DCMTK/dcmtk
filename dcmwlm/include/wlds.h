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
 *  Purpose: (Partially) abstract class for connecting to an arbitrary data source.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 16:50:12 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wlds.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

class WlmDataSource
// Date   : December 10, 2001
// Author : Thomas Wilkens
// Task   : This class encapsulates data structures and operations for connecting to an arbitrary
//          data source in the framework of the DICOM basic worklist management service.
{
  protected:
    WlmObjectStatusType objectStatus;
    OFBool failOnInvalidQuery;
    char *calledApplicationEntityTitle;
    OFBool verbose;
    DcmDataset *identifiers;
    DcmList *objlist;
    DcmAttributeTag *errorElements;
    DcmAttributeTag *offendingElements;
    DcmLongString *errorComment;
    OFBool foundUnsupportedOptionalKey;
    OFBool readLockSetOnDataSource;
    OFConsole *logStream;

    void ClearObjectList();
    void ClearDataset( DcmDataset *identifiers );
    OFBool CheckIdentifiers( DcmDataset *identifiers );
    void PutOffendingElements( DcmTagKey &tag );
    void PutErrorElements( DcmTagKey &tag );
    OFBool CheckMatchingKey( DcmElement *elem );
    OFBool CheckCharSet( const char *s, const char *charset );
    virtual int SetReadlock() = 0;
    virtual int ReleaseReadlock() = 0;
    virtual OFBool IsSupportedMatchingKeyAttribute( const DcmTagKey &key, int level ) = 0;
    virtual OFBool IsSupportedReturnKeyAttribute( const DcmTagKey &key, int level ) = 0;
    char *GetStringValue( DcmElement *elem );
    void DumpMessage( const char *message );

  public:
    // Constructor/Destructor
    WlmDataSource( OFConsole *logStreamv, const OFBool verbosev );
    virtual ~WlmDataSource();

    // Check if the object status is ok
    OFBool IsObjectStatusOk();

    // Check if the specified data source is available at all.
    virtual OFBool IsDataSourceAvailable() = 0;

    // Set member variable that specifies called application entity title
    void SetCalledApplicationEntityTitle( char *value );

    // Check if the called application entity title is supported whithin the data source
    virtual OFBool IsCalledApplicationEntityTitleSupported() = 0;

    // Set member variable that determines if a call should fail on an invalid query.
    void SetFailOnInvalidQuery( OFBool value );

    // Set member variable that determines if information shall be dumped at processing time or not
    void SetVerbose( OFBool value );

    // Determine the records that match the search mask.
    virtual WlmDataSourceStatusType StartFindRequest( DcmDataset &findRequestIdentifiers ) = 0;

    // Get the next matching record/data set
    virtual DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus ) = 0;

    // Cancel the find request
    WlmDataSourceStatusType CancelFindRequest();

    // Get offending elements
    DcmAttributeTag *GetOffendingElements();

    // Get error comments
    DcmLongString *GetErrorComments();
};


/*
** CVS Log
** $Log: wlds.h,v $
** Revision 1.2  2002-01-08 16:50:12  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:30:59  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/

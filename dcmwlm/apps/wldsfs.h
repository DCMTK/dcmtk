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
 *  Purpose: Class for connecting to a file-based data source.
 *
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-04-18 14:19:53 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldsfs.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmDataSourceFileSystem_h
#define WlmDataSourceFileSystem_h

class DcmDataset;
class DcmTagKey;
class DcmItem;
class OFCondition;

  /** This class encapsulates data structures and operations for connecting to a file-based
   *  data source in the framework of the DICOM basic worklist management service.
   */
class WlmDataSourceFileSystem : public WlmDataSource
{
  protected:
    int handleToReadLockFile;
    char *dfPath;

    OFBool IsDirectory( const char* path );
    int SetReadlock();
    int ReleaseReadlock();
    OFBool CheckIdentifiers( DcmDataset *identifiers );
    OFBool IsSupportedMatchingKeyAttribute( const DcmTagKey &key, int level );
    OFBool IsSupportedReturnKeyAttribute( const DcmTagKey &key, int level );
    OFBool IsWorklistFile( const char *fname );
    OFBool IsValidWorklistFile( DcmDataset *dsetfile, const char *fname );
    char *FindStringValue( DcmItem *dset, const DcmTagKey &tag );
    OFBool ValueMatchesPattern( const char *value, const char *pattern );
    OFBool WildcardMatch( const char *value, const char *pattern );
    OFBool MatchStarSymbol( const char *value, const char *pattern );
    OFBool MatchTime( const char *value, const char *pattern );
    OFBool MatchDate( const char *value, const char *pattern );
    OFBool RangematchTime( const char *timeValue, const char *timeRange );
    OFBool RangematchDate( const char *dateValue, const char *dateRange );
    char *StandardizeDate( const char *dateString );
    char *StandardizeTime( const char *timeString );

  public:
      /** default constructor.
       */
    WlmDataSourceFileSystem();

      /** destructor
       */
    ~WlmDataSourceFileSystem();

      /** Connects to the database.
       * @return Indicates if the connection was established succesfully.
       */
    OFCondition ConnectToDataSource();

      /** Disconnects from the data source.
       * @return Indicates if the disconnection was completed succesfully.
       */
    OFCondition DisconnectFromDataSource();

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetDfPath( const char *value );

      /** Checks if the called application entity title is supported. This function expects
       *  that the called application entity title was made available for this instance through
       *  WlmDataSource::SetCalledApplicationEntityTitle(). If this is not the case, OFFalse
       *  will be returned.
       *  @return OFTrue, if the called application entity title is supported;
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    OFBool IsCalledApplicationEntityTitleSupported();

      /** This function mainly goes through all worklist files and determines those records
       *  that match the search mask which was passed. All those records will be written to the
       *  member variable objlist. (This variable will be cleared at the beginning of this
       *  function so that in the end only those records are contained in this variable.)
       *  @param findRequestIdentifiers Contains the search mask.
       *  @return A value of type WlmDataSourceStatusType denoting if the function call was
       *          successful or not.
       */
    WlmDataSourceStatusType StartFindRequest( DcmDataset &findRequestIdentifiers );

      /** This function assumes that function StartFindRequest(...) was called earlier and that that
       *  function has determined (from the set of worklist files) all those records which match the
       *  given search mask. In detail, it is expected that the above mentioned function has written
       *  all those records to the member variable objlist. On each call, this function will return
       *  one of those records as a (newly created) DcmDataset object. The DcmFileFormat object which
       *  is contained in objlist and which refers to the returned object will be removed from objlist.
       *  @param rStatus A value of type WlmDataSourceStatusType denoting if the function call was
       *                 successful or not.
       *  @return The next record which matches the given search mask.
       */
    DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus );
};

#endif

/*
** CVS Log
** $Log: wldsfs.h,v $
** Revision 1.2  2002-04-18 14:19:53  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.3  2002/01/08 17:45:34  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 16:47:53  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:30:59  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/

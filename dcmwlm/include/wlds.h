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
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-12-16 11:08:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wlds.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmDataSource_h
#define WlmDataSource_h

#include "osconfig.h"

class DcmDataset;
class DcmList;
class DcmAttributeTag;
class DcmLongString;
class DcmTagKey;
class DcmElement;
class OFConsole;
class OFCondition;
class DcmSequenceOfItems;

/** This class encapsulates data structures and operations for connecting to an arbitrary
 *  data source in the framework of the DICOM basic worklist management service.
 */
class WlmDataSource
{
  protected:
    OFBool failOnInvalidQuery;
    char *calledApplicationEntityTitle;
    OFBool verbose;
    OFBool debug;
    DcmDataset *identifiers;
    DcmList *objlist;
    DcmAttributeTag *errorElements;
    DcmAttributeTag *offendingElements;
    DcmLongString *errorComment;
    OFBool foundUnsupportedOptionalKey;
    OFBool readLockSetOnDataSource;
    OFConsole *logStream;
    OFBool noSequenceExpansion;
    WlmReturnedCharacterSetType returnedCharacterSet;

    OFBool CheckSearchMask( DcmDataset *searchMask );
    void CheckNonSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    void CheckSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    void ExpandEmptySequenceInSearchMask( DcmElement *&element );
    OFBool IsSupportedMatchingKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    OFBool IsSupportedReturnKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    void ClearObjectList();
    void ClearDataset( DcmDataset *identifiers );
    void PutOffendingElements( DcmTagKey &tag );
    void PutErrorElements( DcmTagKey &tag );
    OFBool CheckMatchingKey( DcmElement *elem );
    OFBool ContainsOnlyValidCharacters( const char *s, const char *charset );
    OFBool IsValidDateOrDateRange( const char *value );
    OFBool IsValidDate( const char *value );
    OFBool IsValidTimeOrTimeRange( const char *value );
    OFBool IsValidTime( const char *value );
    char *GetStringValue( DcmElement *elem );
    void DumpMessage( const char *message );
    char *DeleteLeadingAndTrailingBlanks( const char *value );

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmDataSource( const WlmDataSource &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmDataSource &operator=( const WlmDataSource &Src );


  public:
      /** default constructor.
       */
    WlmDataSource();

      /** destructor
       */
    virtual ~WlmDataSource();

      /** Connects to the data source.
       * @return Indicates if the connection was established succesfully.
       */
    virtual OFCondition ConnectToDataSource() = 0;

      /** Disconnects from the data source.
       * @return Indicates if the disconnection was completed succesfully.
       */
    virtual OFCondition DisconnectFromDataSource() = 0;

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetCalledApplicationEntityTitle( char *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetFailOnInvalidQuery( OFBool value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetLogStream( OFConsole *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetVerbose( OFBool value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetDebug( OFBool value );

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    void SetNoSequenceExpansion( const OFBool value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetReturnedCharacterSet( WlmReturnedCharacterSetType value );

      /** Checks if the called application entity title is supported. This function expects
       *  that the called application entity title was made available for this instance through
       *  WlmDataSource::SetCalledApplicationEntityTitle(). If this is not the case, OFFalse
       *  will be returned.
       *  @return OFTrue, if the called application entity title is supported;
       *          OFFalse, if the called application entity title is not supported or it is not given.
       */
    virtual OFBool IsCalledApplicationEntityTitleSupported() = 0;

      /** Based on the search mask which was passed, this function determines all the records
       *  in the database which match the values of matching key attributes in the search mask.
       *  For each matching record, a DcmDataset structure is generated which will later be
       *  returned to the SCU as a result of query. The DcmDataset structures for all matching
       *  records will be stored in the protected member variable matchingDatasets.
       *  @param findRequestIdentifiers Contains the search mask.
       *  @return A WlmDataSourceStatusType value denoting the following:
       *          WLM_SUCCESS: No matching records found;
       *          WLM_PENDING: Matching records found, all return keys supported by this application;
       *          WLM_PENDING_WARNING: Matching records found, not all return keys supported by this application;
       *          WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS: Error in the search mask encountered.
       */
    virtual WlmDataSourceStatusType StartFindRequest( DcmDataset &findRequestIdentifiers ) = 0;

      /** This function will return the next dataset that matches the given search mask, if
       *  there is one more resulting dataset to return. In such a case, rstatus will be set
       *  to WLM_PENDING or WLM_PENDING_WARNING, depending on if an unsupported key attribute
       *  was encountered in the search mask or not. If there are no more datasets that match
       *  the search mask, this function will return an empty dataset and WLM_SUCCESS in rstatus.
       *  @param rStatus A value of type WlmDataSourceStatusType that can be used to
       *                 decide if there are still elements that have to be returned.
       *  @return The next dataset that matches the given search mask, or an empty dataset if
       *          there are no more matching datasets in the database.
       */
    virtual DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus ) = 0;

      /** This function handles a C-CANCEL Request during the processing of a C-FIND Request.
       *  In detail, the member variable objlist is cleared.
       *  @return Always WLM_CANCEL.
       */
    WlmDataSourceStatusType CancelFindRequest();

      /** Get value from member variable.
       *  @return The member variable's value.
       */
    DcmAttributeTag *GetOffendingElements();

      /** Get value from member variable.
       *  @return The member variable's value.
       */
    DcmLongString *GetErrorComments();

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetDbDsn( const char * /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetDbUserName( const char * /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetDbUserPassword( const char * /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetCfgFileMatchRecords( const char * /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetCfgFileSelectValues( const char * /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetDatabaseType( WlmDatabaseType /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetSerialNumber( const int /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetDfPath( const char * /*value*/ ) {}

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    virtual void SetPfFileName( const char * /*value*/ ) {}
};

#endif

/*
** CVS Log
** $Log: wlds.h,v $
** Revision 1.10  2002-12-16 11:08:33  wilkens
** Added missing #include "osconfig.h" to certain files.
**
** Revision 1.9  2002/08/12 10:56:07  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.8  2002/07/17 13:10:36  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.7  2002/07/01 14:13:56  wilkens
** Some more corrections to get rid of msvc6's warnings.
**
** Revision 1.6  2002/06/10 11:25:05  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.5  2002/05/08 13:20:51  wilkens
** Added new command line option -nse to wlmscpki and wlmscpdb.
**
** Revision 1.4  2002/04/18 14:20:08  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.3  2002/01/08 17:45:34  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 16:50:12  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:30:59  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/

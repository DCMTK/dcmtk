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
 *  Last Update:      $Author: wilkens $
 *  Update Date:      $Date: 2002-06-10 11:24:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldsdb.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmDataSourceDatabase_h
#define WlmDataSourceDatabase_h

class WlmDatabaseInteractionManager;
class DcmDataset;
class OFConsole;
class OFCondition;
class DcmElement;
class DcmSequenceOfItems;

  /** This class encapsulates data structures and operations for connecting to a database-based
   *  data source in the framework of the DICOM basic worklist management service.
   */
class WlmDataSourceDatabase : public WlmDataSource
{
  protected:
    WlmDatabaseInteractionManager *databaseInteractionManager;
    DcmDataset **matchingDatasets;
    unsigned long numOfMatchingDatasets;
    char *dbDsn;
    char *dbUserName;
    char *dbUserPassword;
    char *cfgFileMatchRecords;
    char *cfgFileSelectValues;
    WlmDatabaseType databaseType;
    WlmReturnedCharacterSetType returnedCharacterSet;
    int serialNumber;
    OFBool noSequenceExpansion;

    int SetReadlock();
    int ReleaseReadlock();
    OFBool CheckSearchMask( DcmDataset *searchMask );
    void CheckNonSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    void CheckSequenceElementInSearchMask( DcmDataset *searchMask, int &invalidMatchingKeyAttributeCount, DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    void ExpandEmptySequenceInSearchMask( DcmElement *&element );
    OFBool IsSupportedMatchingKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    OFBool IsSupportedReturnKeyAttribute( DcmElement *element, DcmSequenceOfItems *supSequenceElement=NULL );
    void DetermineMatchingKeyAttributeValues( const char **&matchingKeyValues, unsigned long &numOfMatchingKeyValues );
    void HandleNonSequenceElementInResultDataset( DcmElement *element, long matchingRecordID );
    void HandleSequenceElementInResultDataset( DcmElement *element, long matchingRecordID );

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmDataSourceDatabase( const WlmDataSourceDatabase &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmDataSourceDatabase &operator=( const WlmDataSourceDatabase &Src );


  public:
      /** default constructor.
       */
    WlmDataSourceDatabase();

      /** destructor
       */
    ~WlmDataSourceDatabase();

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
    void SetDbDsn( const char *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetDbUserName( const char *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetDbUserPassword( const char *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetSerialNumber( const int value );

      /** Set value in a member variable in a derived class.
       *  @param value The value to set.
       */
    void SetNoSequenceExpansion( const OFBool value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetCfgFileMatchRecords( const char *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetCfgFileSelectValues( const char *value );

      /** Set value in member variable.
       *  @param value The value to set.
       */
    void SetDatabaseType( WlmDatabaseType value );

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
    OFBool IsCalledApplicationEntityTitleSupported();

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
    WlmDataSourceStatusType StartFindRequest( DcmDataset &findRequestIdentifiers );

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
    DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus );
};

#endif

/*
** CVS Log
** $Log: wldsdb.h,v $
** Revision 1.4  2002-06-10 11:24:52  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.3  2002/05/08 13:20:37  wilkens
** Added new command line option -nse to wlmscpki and wlmscpdb.
**
** Revision 1.2  2002/04/18 14:19:51  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.4  2002/01/08 17:45:34  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.3  2002/01/08 17:38:15  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/

/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Update Date:      $Date: 2002-12-16 11:08:34 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/include/Attic/wldsfs.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef WlmDataSourceFileSystem_h
#define WlmDataSourceFileSystem_h

#include "osconfig.h"
#include "wlds.h"

class WlmFileSystemInteractionManager;
class DcmDataset;
class OFCondition;
class DcmElement;

/** This class encapsulates data structures and operations for connecting to a file-based
 *  data source in the framework of the DICOM basic worklist management service.
 */
class WlmDataSourceFileSystem : public WlmDataSource
{
  protected:
    WlmFileSystemInteractionManager *fileSystemInteractionManager;
    DcmDataset **matchingDatasets;
    unsigned long numOfMatchingDatasets;
    char *dfPath;
    int handleToReadLockFile;

    int SetReadlock();
    int ReleaseReadlock();
    void DetermineMatchingKeyAttributeValues( const char **&matchingKeyValues, unsigned long &numOfMatchingKeyValues );
    void HandleNonSequenceElementInResultDataset( DcmElement *element, unsigned long idx );
    void HandleSequenceElementInResultDataset( DcmElement *element, unsigned long idx );

      /** Protected undefined copy-constructor. Shall never be called.
       *  @param Src Source object.
       */
    WlmDataSourceFileSystem( const WlmDataSourceFileSystem &Src );

      /** Protected undefined operator=. Shall never be called.
       *  @param Src Source object.
       *  @return Reference to this.
       */
    WlmDataSourceFileSystem &operator=( const WlmDataSourceFileSystem &Src );


  public:
      /** default constructor.
       */
    WlmDataSourceFileSystem();

      /** destructor
       */
    ~WlmDataSourceFileSystem();

      /** Connects to the data source.
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

      /** Based on the search mask which was passed, this function determines all the records in the
       *  worklist database files which match the values of matching key attributes in the search mask.
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
       *          there are no more matching datasets in the worklist database files.
       */
    DcmDataset *NextFindResponse( WlmDataSourceStatusType &rStatus );
};

#endif

/*
** CVS Log
** $Log: wldsfs.h,v $
** Revision 1.9  2002-12-16 11:08:34  wilkens
** Added missing #include "osconfig.h" to certain files.
**
** Revision 1.8  2002/12/09 13:40:49  joergr
** Renamed parameter to avoid name clash with global function index().
**
** Revision 1.7  2002/08/12 10:56:08  wilkens
** Made some modifications in in order to be able to create a new application
** which contains both wlmscpdb and ppsscpdb and another application which
** contains both wlmscpfs and ppsscpfs.
**
** Revision 1.6  2002/08/05 09:09:57  wilkens
** Modfified the project's structure in order to be able to create a new
** application which contains both wlmscpdb and ppsscpdb.
**
** Revision 1.4  2002/07/17 13:10:17  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.3  2002/06/10 11:24:54  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.2  2002/04/18 14:19:53  wilkens
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

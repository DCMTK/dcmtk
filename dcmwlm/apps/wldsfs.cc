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
 *  Update Date:      $Date: 2002-04-18 10:26:48 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldsfs.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"  // specific configuration for operating system

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>     // for O_RDWR
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>  // for struct DIR, opendir()
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>    // for struct DIR, opendir()
#endif
END_EXTERN_C

#include "dicom.h"     // for DIC_NODENAME etc. used in "wltypdef.h"
#include "wltypdef.h"  // for type definitions
#include "oftypes.h"   // for OFBool
#include "ofstd.h"     // for OFStandard::isReadable()
#include "dcdatset.h"  // for DcmDataset
#include "dcvrat.h"    // for DcmAttributTag
#include "dcvrlo.h"    // for DcmLongString
#include "dcompat.h"   // for R_OK on Win32
#include "dcdeftag.h"  // for DCM_OffendingElement, ...
#include "dcfilefo.h"  // for DcmFileFormat
#include "diutil.h"    // for DU_stripTrailingSpaces()
#include "dcvm.h"      // for getVMFromString()
#include "wlds.h"

#include "wldsfs.h"

// ----------------------------------------------------------------------------

WlmDataSourceFileSystem::WlmDataSourceFileSystem()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : WlmDataSource(), handleToReadLockFile( 0 ), dfPath( NULL )
{
}

// ----------------------------------------------------------------------------

WlmDataSourceFileSystem::~WlmDataSourceFileSystem()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // release read lock on data source if it is set
  if( readLockSetOnDataSource ) ReleaseReadlock();

  //free memory
  if( dfPath != NULL ) delete dfPath;
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceFileSystem::ConnectToDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Connects to the data source.
// Parameters   : none.
// Return Value : Indicates if the connection was established succesfully.
{
  // check if the specified path is existent and accessible for reading
  if( OFStandard::isReadable(dfPath) )
    return( WLM_EC_CannotConnectToDataSource );
  else
    return( EC_Normal );
}

// ----------------------------------------------------------------------------

OFCondition WlmDataSourceFileSystem::DisconnectFromDataSource()
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Disconnects from the data source.
// Parameters   : none.
// Return Value : Indicates if the disconnection was completed succesfully.
{
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

void WlmDataSourceFileSystem::SetDfPath( const char *value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  if( value != NULL )
  {
    if( dfPath != NULL ) delete dfPath;
    dfPath = new char[ strlen(value) + 1 ];
    strcpy( dfPath, value );
  }
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::CheckIdentifiers( DcmDataset *idents )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function checks if all elements in the search mask are supported matching key
//                attributes. It returns OFTrue if this is the case, OFFalse if this is not the case.
//                (Thomas Wilkens: This function is different from WlmDataSourceDatabase::CheckSearchMask
//                This file here contains the original source code, the code in WlmDataSourceDatabase::
//                CheckSearchMask is the later (and more correct) version.)
// Parameters   : idents - [in] Contains the search mask.
// Return Value : OFTrue  - No invalid matching key attribute was found in the search mask.
//                OFFalse - At least one invalid matching key attribute was found in the search mask.
{
  char msg[200];

  // Initialize counter that counts invalid elements in the search mask. This
  // variable will later be used to determine the return value for this function.
  int invalidMatchingKeyCount = 0;

  // remember the number of data elements in the search mask
  unsigned long cids = idents->card();

  // dump some information if required
  if( verbose )
    DumpMessage( "Checking the search mask." );

  // this member variable indicates if we encountered an unsupported
  // optional key attribute in the search mask; initialize it with false.
  foundUnsupportedOptionalKey = OFFalse;

  // start a loop; go through all data elements in the search mask, for each element, do some checking
  for( unsigned long i=0; i<cids ; i++ )
  {
    // determine current element
    DcmElement *element = idents->getElement(i);

    // determine current element's length
    Uint32 length = element->getLength();

    // determine current element's tag
    DcmTag tag( element->getTag() );

    // if the current element is NOT a sequence
    if( element->ident() != EVR_SQ )
    {
      // figure out if the current element refers to an attribute which
      // is a supported matching key attribute. Use level 0 for this check.
      if( IsSupportedMatchingKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if the current element is a supported matching key attribute, do nothing (everything is ok)
      }
      // if current element refers to an attribute which is not a supported
      // matching key attribute, figure out if the current element refers to an
      // attribute which is a supported return key attribute. Use level 0 for this check.
      else if( IsSupportedReturnKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if the current element refers to a supported return key attribute
        // everything is okay as long as this attribute does not contain a
        // value. In general, according to the DICOM standard it does not make
        // sense that a return key attribute which is NOT a matching key attribute
        // contains a value. Hence, we specify that in such a case (i.e. if the
        // current element's length does not equal 0) we've found an unsupported
        // optional key attribute. Note that we do not want to remember this attribute
        // in the list of error elements.
        if (length != 0)
        {
          foundUnsupportedOptionalKey = OFTrue;
        }
      }
      // if current element does neither refer to a supported matching
      // key attribute nor does it refer to a supported return key attribute,
      // we've found an unsupported optional key attribute; we want to remember
      // this attribute in the list of error elements.
      else
      {
        foundUnsupportedOptionalKey = OFTrue;
        PutErrorElements( tag );
      }

      // check if the current element meets certain conditions (at the moment
      // we only check if the element's data type and value go together)
      // ### Also das hier macht keinen Sinn. Man muß doch nur diejenigen Elemente auf
      // ### Validität ihrer Attribute checken, die tatsächlich zum Matchen benutzt werden.
      // ### Wenn in den return key attributen ein Wert drinsteht, dann muß eine Warnmeldung
      // ### ausgegeben werden!!
      if( !CheckMatchingKey( element ) )
      {
        // if there is a problem with the current element, increase the corresponding counter
        invalidMatchingKeyCount++;
      }
    }
    // or if the current element IS a sequence
    else
    {
      // figure out if the current sequence element refers to an attribute which
      // is a supported matching key attribute. Use level 0 for this check.
      if( IsSupportedMatchingKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if the current element is a supported matching key attribute, check its length
        if( length != 0 )
        {
          // if length does not equal 0, check inside the sequence

          // remember that elements is a sequence of items
          DcmSequenceOfItems *sequits = (DcmSequenceOfItems*)element;

          // check the cardinality of the sequence; note that we may
          // not have more than 1 item in a sequence item whithin the search mask
          if( sequits->card() > 1 )
          {
            PutOffendingElements(tag);
            errorComment->putString("More than 1 item in sequence.");
            sprintf( msg, "WlmDataSource::CheckIdentifiers : Error: More than one item in sequence %s (search mask) encountered.", tag.getTagName() );
            DumpMessage( msg );
            invalidMatchingKeyCount++;
          }

          // get the first item
          DcmItem *item = sequits->getItem(0);

          // determine the cardinality of this item
          unsigned long cit = item->card();

          // go through all elements of this item
          for( unsigned long k=0 ; k<cit ; k++ )
          {
            // determine the current element
            DcmElement *elementseq = item->getElement(k);

            // determine the length of the current element
            Uint32 length1 = elementseq->getLength();

            // determine the current element's tag
            DcmTag tagseq( elementseq->getTag() );

            // figure out if the current element refers to an attribute which
            // is a supported matching key attribute. Use level 1 for this check.
            if( IsSupportedMatchingKeyAttribute( tagseq.getXTag(), 1 ) )
            {
              // if the current element is a supported matching key attribute, do nothing (everything is ok)
            }
            // if current element refers to an attribute which is not a supported
            // matching key attribute, figure out if the current element refers to an
            // attribute which is a supported return key attribute. Use level 1 for this check.
            else if( IsSupportedReturnKeyAttribute( tagseq.getXTag(), 1 ) )
            {
              // if the current element refers to a supported return key attribute
              // everything is okay as long as this attribute does not contain a
              // value. In general, according to the DICOM standard it does not make
              // sense that a return key attribute which is NOT a matching key attribute
              // contains a value. Hence, we specify that in such a case (i.e. if the
              // current element's length does not equal 0) we've found an unsupported
              // optional key attribute. Note that we do not want to remember this attribute
              // in the list of error elements.
              if( length1 != 0 )
              {
                foundUnsupportedOptionalKey = OFTrue;
              }
            }
            // if current element does neither refer to a supported matching
            // key attribute nor does it refer to a supported return key attribute,
            // we've found an unsupported optional key attribute; we want to remember
            // this attribute in the list of error elements.
            else
            {
              foundUnsupportedOptionalKey = OFTrue;
              PutErrorElements(tagseq);
            }

            // check if the current element meets certain conditions (at the moment
            // we only check if the element's data type and value go together)
            // ### Genau wie oben: Das hier macht keinen Sinn. Man muß doch nur diejenigen Elemente auf
            // ### Validität ihrer Attribute checken, die tatsächlich zum Matchen benutzt werden.
            // ### Wenn in den return key attributen ein Wert drinsteht, dann muß eine Warnmeldung
            // ### ausgegeben werden!!
            if( !CheckMatchingKey( elementseq ) )
            {
              // if there is a problem with the current element, increase the corresponding counter
              invalidMatchingKeyCount++;
            }
          }

#ifdef DELETE_UNEXPECTED_ELEMENTS_FROM_IDENTIFIER
          // remove unexpected (error) elements from this item
          unsigned long d = errorElements->getVM();
          for( unsigned long e=0 ; e<d ; e++ )
          {
            DcmTag errortag;
            errortag = errorElements->get(e);
            delete item->remove(errortag);
          }
#endif
        }
      }

      // if current sequence element refers to an attribute which is not a supported
      // matching key attribute, figure out if the current sequence element refers to an
      // attribute which is a supported return key attribute. Use level 0 for this check.
      else if( IsSupportedReturnKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if this is the case and the current element's length does
        // not equal 0, we've found an unsupported optional key attribute
        if( length != 0 )
        {
          foundUnsupportedOptionalKey = OFTrue;
        }
      }
      // if current element does neither refer to a supported matching key
      // attribute nor does it refer to a supported return key attribute,
      // we've found an unsupported optional key attribute; we want to remember
      // this attribute in the list of error elements.
      else
      {
        foundUnsupportedOptionalKey = OFTrue;
        PutErrorElements( tag );
      }
    }
  }

#ifdef DELETE_UNEXPECTED_ELEMENTS_FROM_IDENTIFIER
  // remove unexpected (error) elements from this item
  unsigned long d = errorElements->getVM();
  for( unsigned long h=0 ; h<d ; h++ )
  {
    DcmTag errortag;
    errortag = errorElements->get(h);
    delete idents->remove(errortag);
  }
#endif

  // if there was more than 1 error, override the error comment
  if( invalidMatchingKeyCount > 1 )
    errorComment->putString("Syntax error in 1 or more matching keys");

  // return OFTrue or OFFalse depending on the number of invalid matching attributes
  return( invalidMatchingKeyCount == 0 );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::IsCalledApplicationEntityTitleSupported()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Checks if the called application entity title is supported. This function expects
//                that the called application entity title was made available for this instance through
//                WlmDataSource::SetCalledApplicationEntityTitle(). If this is not the case, OFFalse
//                will be returned.
// Parameters   : none.
// Return Value : OFTrue  - The called application entity title is supported.
//                OFFalse - The called application entity title is not supported or it is not given.
{
  // Check if calledApplicationEntityTitle does not have a valid value
  if( calledApplicationEntityTitle == NULL )
    return( OFFalse );

  // Determine complete path to the files that make up the data source.
  char dbPath[2048];
  sprintf( dbPath, "%s%c%s", dfPath, PATH_SEPARATOR, calledApplicationEntityTitle );

  // Check if the path is existent.
  if( !IsDirectory( dbPath ) )
    return( OFFalse );
  else
    return( OFTrue );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::IsDirectory( const char* path )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function checks if the specified path refers to an existing directory.
// Parameters   : path - [in] The path that will be checked.
// Return Value : OFTrue  - The path refers to an existing directory.
//                OFFalse - The path does not refer to an existing directory.
{
  return OFStandard::dirExists(path);
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSourceFileSystem::StartFindRequest( DcmDataset &findRequestIdentifiers )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function mainly goes through all worklist files and determines those records
//                that match the search mask which was passed. All those records will be written to the
//                member variable objlist. (This variable will be cleared at the beginning of this
//                function so that in the end only those records are contained in this variable.)
// Parameters   : findRequestIdentifiers - [in] Contains the search mask.
// Return Value : A value of type WlmDataSourceStatusType denoting if the function call was
//                successful or not.
{
// ### wilkens: Hier könnte man auch einiges mal umbenennen, um sprechende Bezeichner zu haben:
// ### identifiers -> searchMask
// ### CheckIdentifiers() -> IsValidSearchMask()

  WlmDataSourceStatusType status = WLM_SUCCESS;
  OFBool match = OFFalse;
  OFBool *seqmatch = NULL;

  // Initialize offending elements, error elements and error comment.
  delete offendingElements;
  delete errorElements;
  delete errorComment;
  offendingElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorComment = new DcmLongString( DCM_ErrorComment, 0 );

  // Initialize member variable identifiers; this variable will contain the search mask.
  ClearDataset( identifiers );
  delete identifiers;
  identifiers = new DcmDataset( findRequestIdentifiers );

  // Remove group length and padding elements from the search mask.
  identifiers->computeGroupLengthAndPadding( EGL_withoutGL, EPD_withoutPadding );

  // Remove all elements from member variable objlist; this variable
  // will in the end contain all records that match the search mask.
  ClearObjectList();

  // This member variable indicates if we encountered an unsupported
  // optional key attribute in the search mask; initialize it with false.
  foundUnsupportedOptionalKey = OFFalse;

  // Scrutinize the search mask.
  if( !CheckIdentifiers( identifiers ) )
  {
    // In case we encountered an error in the search
    // mask, we may have to return to the caller
    if( failOnInvalidQuery )
      return( WLM_FAILED_IDENTIFIER_DOES_NOT_MATCH_SOP_CLASS );
  }

  // Dump some information if required.
  if( verbose )
    COUT << "=============================" << endl << "Worklist Database Files" << endl;

  // If there is no directory path, return.
  if( dfPath == NULL )
    return( WLM_REFUSED_OUT_OF_RESOURCES );

  // Set a read lock on the LOCKFILE in the given directory.
  /*int error =*/ SetReadlock();

/* wilkens: Das hier hab ich mal auskommentiert, weil es im alten wlistctn auch nicht
   gemacht wird. Manchmal klappt ein Readlock nicht, dann soll aber weitergearbeitet
   werden. So macht es auch der alte wlistctn. Wir müssen uns dennoch fragen: Ist
   diese Funktionalität so gewollt? Ich denke nicht.

  if( error )
    return( WLM_REFUSED_OUT_OF_RESOURCES );
*/

  // Determine complete path to data source files (dfPath + PATH_SEPARATOR + calledApplicationEntityTitle)
  OFString path( dfPath );
  if( path.length() > 0 && path[path.length()-1] != PATH_SEPARATOR )
    path += PATH_SEPARATOR;
  path += calledApplicationEntityTitle;

#ifdef HAVE__FINDFIRST
  struct _finddata_t fileData;

  // Determine if the given directory path actually refers to a directory.
  long hFile = _findfirst( (char *)path.c_str(), &fileData );
  if( hFile != -1L && ( fileData.attrib & _A_SUBDIR ) )
  {
    // If does refer to a directory, we want to read all directory entries in this directory.

    // Close the search handle hFile again.
    _findclose(hFile);

    // Create a search string.
    OFString searchname = path + PATH_SEPARATOR + '*';
    int ret = 0;

    // start a loop; in each iteration another directory entry is determined
    for( hFile = _findfirst( (char *)searchname.c_str(), &fileData ) ; hFile != -1L && ret == 0 ; ret = _findnext( hFile, &fileData ) )
    {
      // if the current entry refers to a worklist file, do something
      if( strcmp( fileData.name, "." ) !=0 &&
          strcmp( fileData.name, ".." )!=0 &&
          IsWorklistFile( fileData.name ) )
      {
        // Create a string that contains path and filename of the current worklist file.
        OFString subname( path );
        subname += PATH_SEPARATOR;
        subname += fileData.name;
        char file1[200];
        strcpy( file1, subname.c_str() );
#else
  // Determine if the given directory path actually refers to a directory.
  DIR *dirp = opendir( path.c_str() );
  if( dirp != NULL )
  {
    // If does refer to a directory, we want to read all directory entries in this directory.

    // Start a loop; in each iteration another directory entry is determined.
    struct dirent *dp;
#if defined(_REENTRANT) && !defined(_WIN32) && !defined(__CYGWIN__)
    struct dirent dirEntry;
    // use readdir_r instead of readdir
    for( dp = readdir_r( dirp, &dirEntry ) ; dp != NULL ; dp = readdir_r( dirp, &dirEntry ) )
#else
    for( dp = readdir( dirp ) ; dp != NULL ; dp = readdir( dirp ) )
#endif
    {
      // If the current entry refers to a worklist file, do something.
      if( IsWorklistFile( dp->d_name ) )
      {
        // Create a string that contains path and filename of the current worklist file.
        char file1[200];
        sprintf( file1, "%s%c%s", path.c_str(), PATH_SEPARATOR, dp->d_name );
#endif

        // Dump current file specification (path and filename).
        COUT << file1 << endl;

        // Associate file with a stream, open in read mode.
        DcmFileStream i1Stream( file1, DCM_ReadMode );

        // Check if stream could be opened.
        if( i1Stream.Fail() )
        {
          CERR << "WlmDataSourceFileSystem::StartFindRequest : Cannot open worklist file " << file1 << endl;
          status = WLM_REFUSED_OUT_OF_RESOURCES;
          break;
        }

        // Read information from worklist file. After the call to DcmFileFormat::read(...) the in-
        // formation which is encapsulated in the file will be available through the DcmFileFormat
        // object. In detail, it will be available through calls to DcmFileFormat::getMetaInfo()
        // (for meta header information) and DcmFileFormat::getDataset() (for data set information).
        DcmFileFormat *fileform = new DcmFileFormat();
        fileform->transferInit();
        fileform->read( i1Stream, EXS_Unknown, EGL_withoutGL );
        fileform->transferEnd();

        // Figure out if an error occured while the file was read.
        if( fileform->error() != EC_Normal )
        {
          CERR << "WlmDataSourceFileSystem::StartFindRequest : Error: " << fileform->error().text() << ": while reading worklist file " << file1 << endl;
          errorComment->putString("Error in SCP Worklist Database");
          status = WLM_FAILED_UNABLE_TO_PROCESS;
          delete fileform;
          break;
        }

        // Determine the data set information which is contained in the current worklist file.
        DcmDataset *dsetfile = fileform->getDataset();
        if( dsetfile == NULL )
        {
          CERR << "WlmDataSourceFileSystem::StartFindRequest : Error: empty worklist file " << file1 << endl;
          errorComment->putString("Error in SCP Worklist Database");
          status = WLM_FAILED_UNABLE_TO_PROCESS;
          delete fileform;
          break;
        }

        // Check if the current worklist file is valid, i.e. if all data elements in the data set
        // information of the currently processed worklist file do refer to supported return key
        // attributes. If this is not the case, we want to return an error.
        if( IsValidWorklistFile( dsetfile, file1 ) == OFFalse )
        {
          delete fileform;
          errorComment->putString("Error in SCP Worklist Database");
          status = WLM_FAILED_UNABLE_TO_PROCESS;
          break;
        }

        // If we get to this point we need to go through all data elements
        // in the search mask and figure out if they match the corresponding
        // elements in the data set of the currently processed worklist file.
        // If that is the case, we want to add the corresponding worklist
        // record (data set) to the member variable objlist. (This variable
        // will in the end contain all records that match the search mask.)
        // Note that in the following loop the variable 'match' is used to
        // indicate if two corresponding element's match or do not match.

        // Determine the number of elements in the search mask.
        unsigned long cdset = identifiers->card();

        // Start a loop; go through all the elements in the search mask.
        for( unsigned long i=0; i<cdset ; i++ )
        {
          const char *val1;
          char *val2;

          // Get the current element of the search mask.
          DcmElement *element = identifiers->getElement(i);

          // Determine the current element's length.
          Uint32 length = element->getLength();

          // Determine the current element's tag.
          DcmTagKey tag( element->getTag() );

          // If the current element is NOT a sequence.
          if( element->ident() != EVR_SQ )
          {
            // ### wilkens: Bei nicht-Sequenz Elementen in der Suchmaske werden zum Matchen
            // ### lediglich die Attribute PatientsName und PatientID beutzt. Andere (optionale)
            // ### Matching Key Attributes werden momentan nicht unterstützt. Das ist nach
            // ### 2001er Standard, Teil 4, Abschnitt K.2.2.1.1.2 auch okay. ABER: Im Moment
            // ### ist die Applikation so implementiert, dass optionale Matching Key Attributes
            // ### nur dann einen Match liefern, wenn sie keinen Wert enthalten. Das ist entgegen
            // ### dem 2001er Standard, Teil 4, Abschnitt K.2.2.1.1.2: Dort steht, daß optionale
            // ### Matching Key Attribute (und deren Werte in der Suchmaske), wenn sie nicht
            // ### von der Applikation unterstützt werden, fürs Matchen komplett ignoriert werden
            // ### sollen. Sie sollen lediglich beim Zurücklieferen von Wertem berücksichtigt werden.
            // ### Der Code an dieser Stelle muß also ein wenig umgeschrieben werden.
            // ### (Wir geben am besten eine Warnung aus, wenn wir auf ein optionales, von dieser
            // ### Applikation nicht unterstütztes Matching Key Attribut treffen.)

            // ### wilkens: Das Matchen hier ist ein bischen umständlich programmiert. Am Anfang
            // ### setzt er die match Variable auf false und kuckt dann, ob jedes Element matched.
            // ### Wenn er eins findet das matched, wird match explizit immer auf true gesetzt, und
            // ### nur, wenn eines mal nicht matcht, wird match auf false gesetzt, und die Schleife
            // ### über die Suchmaskenelemente beendet. Intuitiv würde man doch sagen, daß man am
            // ### Anfang match auf true setzt und dann alle Elemente der Suchmaske durchläuft,
            // ### aber nur solange, bis man eines findet, das nicht matcht. Nur dann wird match
            // ### verändert und die Schleife abgebrochen. Das geht doch schneller und der Code ist
            // ### auch kürzer. Das müßte man mal so umprogrammieren.

            // And if it is NOT of data type (VR) UL.
            if( element->ident() != EVR_UL )
            {
              // If we are looking at a patient's name or a patient's ID
              if( tag == DCM_PatientsName || tag == DCM_PatientID )
              {
                // If the length of the search mask's element equals 0, it is considered to be a match.
                if( length == 0 )
                  match = OFTrue;
                // if the length of the search mask's element does not equal 0, we actually
                // have to compare two values to figure out if this is still a match.
                else
                {
                  // Determine the two corresponding values
                  val2 = GetStringValue( element );
                  val1 = FindStringValue( dsetfile, tag );

                  // and figure out if they match. If they do match, set the indicator variable to
                  // OFTrue, if they don't match set it to OFFalse and terminate the loop that
                  // iterates over all elements in the search mask.
                  if( ValueMatchesPattern( val1, val2 ) )
                  {
                    match = OFTrue;
                  }
                  else
                  {
                    match = OFFalse;
                    break;
                  }
                }
              }
              // if we are not looking at a patient's name or a patient's ID
              else
              {
                // this situation is only considered to be a match if
                // the length of the search mask's element equals 0
                if( length == 0 )
                {
                  match = OFTrue;
                }
              }
            }
            // if the current element is of data type UL
            else
            {
              // this situation is only considered to be a match if
              // the length of the search mask's element equals 0
              if( length == 0 )
              {
                match = OFTrue;
              }
            }
          }
          // if the current element IS a sequence
          else
          {
            // ### wilkens: Am Anfang hier gleich der erste Fehler: Die Laenge einer Sequenz in der
            // ### Suchmaske darf laut Standard niemals 0 sein, da eine Sequenz in der Suchmaske immer
            // ### genau ein Item beinhalten soll (das seinerseits jedoch leer sein kann
            // ### (siehe 2001er Standard, Teil 4, Abschnitt C.2.2.2.6). In solchen Fällen wollen
            // ### wir bestimmt eine Warnmeldung ausgeben. (siehe auch CheckIdentifiers().)
            // ### Wenn wir diese Funktion dann so implementieren, daß match am Anfang auf true steht
            // ### und nur dann auf false gesetzt wird, wenn etwas explizit nicht matched, können wir
            // ### außer der Ausgabe der Warnemldung auch einfach nichts weiter machen als zum nächsten
            // ### Element der Suchmaske übergehen.

            // if the length of the search mask's element equals 0, it is considered to be a match
            if( length == 0 )
              match = OFTrue;
            else
            {
              // if the current element is a sequence and its length does not
              // equal 0 we need to consider all the elements of this sequence element.

              // consider the current element as a sequence of items
              DcmSequenceOfItems *sequits = (DcmSequenceOfItems*)element;

              // ### wilkens: Sollte man hier eventuell noch mal prüfen, ob wir eventuell mehr
              // ### als ein Item in einer Sequenz in der Suchmaske haben? In einen solchen
              // ### unerlaubten Fall sollte sicherlich auch eine Warnmeldung ausgegeben werden.
              // ### Das wird schon in CheckIdentifiers geprüft, aber für den Fall,
              // ### das eine ungültige Suchmaske trotzdem bearbeitet wird, sollte man das sicherlich
              // ### auch hier noch mal machen.

              // get the first item
              DcmItem *item = sequits->getItem(0);

              // get the first item's length
              Uint32 lengthofit = item->getLength();

              // if the first item's length equals 0, it is considered to be a match
              if( lengthofit == 0 )
                  match = OFTrue;
              // ### wilkens: Aha! Hier wird geprüft, wenn die Sequenz nicht leer ist, ob das erste Item
              // ### die Länge 0 hat. Wenn ja, ist es ein match. Der Fall einer nichtleeren Sequenz
              // ### mit genau einem Item, das allerdings leer ist, ist also offensichtlich möglich,
              // ### ein einziges leeres Item in einer Sequenz wird beim Empfang also offensichtlich
              // ### nicht wegoptimiert. Das sollte man aber noch mal explizit testen.
              else
              {
                // get the first item's cardinality
                unsigned long cit = item->card();

                // search for the same element (the sequence element)
                // in the data set of the worklist file
                DcmStack stk1;
                dsetfile->search( tag, stk1, ESM_fromHere, OFFalse );
                DcmObject *obj1 = stk1.top();
                DcmSequenceOfItems *sequits1 = (DcmSequenceOfItems*)obj1;

                // get the sequence element's cardinality
                unsigned long csequ1 = 0;
                if( sequits1 != NULL )
                  csequ1 = sequits1->card();

                // create an array of boolean values, one array
                // field for each of the sequence's items
                seqmatch = new OFBool[csequ1];

                // ### wilkens: Hier ist unschön, daß die einzelnen Arrayfelder
                // ### nicht initialisiert werden. Wenn das gemacht werden
                // ### würde, könnte man sich innerhalb der folgenden Schleife
                // ### stets das setzen auf einen der beiden Werte sparen.

                // go through all the sequence element's items (this
                // is the sequence element whithin the worklist file)
                for( unsigned long j=0 ; j<csequ1 ; j++ )
                {
                  // get the current item and consider it as a data set
                  DcmItem *item1 = sequits1->getItem(j);
                  DcmDataset *dset1 = (DcmDataset*)item1;

                  // go through all the elements of the search mask sequence element's first item
                  for( unsigned long k=0 ; k<cit ; k++ )
                  {
                    // get the current element of the search mask sequence element's first item
                    DcmElement *elementseq = item->getElement(k);

                    // if the current element is NOT of data type (VR) UL
                    if( elementseq->ident()!=EVR_UL )
                    {
                      // get the current element's length
                      Uint32 length1 = elementseq -> getLength();

                      // get the current element's tag
                      DcmTagKey tagseq( elementseq->getTag() );

                      // if we encountered a certain tag (see below)
                      if( tagseq == DCM_Modality ||
                          tagseq == DCM_ScheduledPerformingPhysiciansName ||
                          tagseq == DCM_ScheduledProcedureStepLocation ||
                          tagseq == DCM_CommentsOnTheScheduledProcedureStep )
                      {
                        // and the length of the (search mask's) current element
                        // equals 0, this is considered to be a match
                        if( length1 == 0 )
                          seqmatch[j] = OFTrue;
                        // if the length of the (search mask's) element does not equal 0, we actually
                        // have to compare two values to figure out if this is still a match.
                        else
                        {
                          // determine the two corresponding values
                          val2 = GetStringValue( elementseq );
                          val1 = FindStringValue( dset1, tagseq );

                          // and figure out if they match
                          if( ValueMatchesPattern( val1, val2 ) )
                          {
                            seqmatch[j] = OFTrue;
                          }
                          else
                          {
                            seqmatch[j] = OFFalse;

                            // in case they don't match we need to
                            // terminate the corresponding loop
                            break;
                          }
                        }
                      }
                      // if we encountered a certain different tag (see below)
                      else if( tagseq == DCM_ScheduledStationAETitle ||
                               tagseq ==DCM_ScheduledStationName )
                      {
                        // and the length of the (search mask's) current element
                        // equals 0, this is considered to be a match
                        if( length1 == 0 )
                          seqmatch[j] = OFTrue;
                        // if the length of the (search mask's) element does not equal 0, we
                        // need to do something to figure out if this is still a match.
                        else
                        {
                          // the initial assumption is that there is no match
                          seqmatch[j] = OFFalse;

                          // determine the two values which shall be matched
                          val2 = GetStringValue( elementseq );
                          val1 = FindStringValue( dset1, tagseq );

                          // if the whole thing matches, this is for sure considered a match
                          if( ValueMatchesPattern( val1, val2 ) )
                          {
                            seqmatch[j] = OFTrue;
                          }
                          // if the entire thing did not match, we need to
                          // match against any multiple values
                          else
                          {
                            // determine the value multiplicity (VM) of the string
                            // (i.e. how many values (which are seperated by '\') are
                            // stored whithin the string)
                            unsigned long vm = getVMFromString( val1 );

                            // go through all the string's values
                            for( unsigned long v=0 ; v<vm ; v++ )
                            {
                              // get the current value
                              char * val3;
                              val3 = getFirstValueFromString( val1 );  // this function modifies val1. After the call it will point to the next string in the former val1. Also note that val3 refers to a newly created string.

                              // figure out if it matches the pattern
                              if( ValueMatchesPattern( val3, val2 ) )
                              {
                                seqmatch[j] = OFTrue;
                              }
                              else
                              {
                                seqmatch[j] = OFFalse;
                              }

                              // free memory
                              delete val3;

                              // if the current value did match the
                              // pattern, we can terminate the loop
                              if( seqmatch[j] )
                              {
                                break;
                              }
                            }
                          }

                          // if the current search mask's element was not matched
                          // we can terminate the corresponding loop
                          if( !seqmatch[j] )
                          {
                            break;
                          }
                        }
                      }
                      // if we encountered a certain different tag (see below)
                      else if( tagseq == DCM_ScheduledProcedureStepStartTime )
                      {
                        // ### wilkens: Hier ist ein Fehler in der Implementierung.
                        // ### Laut 2001er Standard, Teil 4, Tabelle K.6-1 auf Seite 258 muß überprüft
                        // ### werden, ob bezüglich DCM_ScheduledProcedureStepStartTime und DCM_ScheduledProcedureStepStartDate
                        // ### Single Value Matching oder Range Matching gemacht werden muß. Wie Range Matching
                        // ### genau funktioniert steht in Abschnitt C.2.2.2.5 in Teil 4.

                        // and the length of the (search mask's) current element
                        // equals 0, this is considered to be a match
                        if( length1 == 0 )
                          seqmatch[j] = OFTrue;
                        // if the length of the (search mask's) element does not equal 0, we
                        // need to do something to figure out if this is still a match.
                        else
                        {
                          // get the time values which shall be compared
                          val2 = GetStringValue( elementseq );
                          val1 = FindStringValue( dset1, tagseq );

                          // and compare these two values
                          seqmatch[j] = MatchTime( val1, val2 );

                          // if the current search mask's element was not matched
                          // we can terminate the corresponding loop
                          if( !seqmatch[j] )
                          {
                            break;
                          }
                        }
                      }
                      // if we encountered a certain different tag (see below)
                      else if( tagseq == DCM_ScheduledProcedureStepStartDate )
                      {
                        // and the length of the (search mask's) current element
                        // equals 0, this is considered to be a match
                        if( length1 == 0 )
                          seqmatch[j] = OFTrue;
                        // if the length of the (search mask's) element does not equal 0, we
                        // need to do something to figure out if this is still a match.
                        else
                        {
                          // get the date values which shall be compared
                          val2 = GetStringValue( elementseq );
                          val1 = FindStringValue( dset1, tagseq );

                          // and compare these two values
                          seqmatch[j] = MatchDate( val1, val2 );

                          // if the current search mask's element was not matched
                          // we can terminate the corresponding loop
                          if( !seqmatch[j] )
                          {
                            break;
                          }
                        }
                      }
                      // ### wilkens: Und hier benötigt man nun auch keinen else-Zweig, der für alle
                      // ### anderen möglichen Attribute Überprüfungen durchführt. Und warum: weil
                      // ### das Array nicht initialisiert wird und unten nur auf ein tatsächlich
                      // ### vorhandenes true geprüft wird. Das ist besser so, aber inkonsistent zu
                      // ### oben. Im allgemeinen denke ich, dass man diese Funktion übersichtlicher
                      // ### gestalten könnte. Und außerdem sind ja auch inzwischen Sequenzen in
                      // ### Sequenzen möglich, das kann diese Methode noch nicht. Man sollte
                      // ### Hilfsroutinen schreiben, die sich mit dem Matchem beschäftigen, bezogen
                      // ### auf das aktuell untersuchte Attribut der Suchmaske.
                    }
                    // ### wilkens: Auch hier benötigt man keinen else-Zweig, gleicher Grund wie oben
                  }
                }

                // go through all array fields of the result array
                for( int z = (int)(csequ1-1) ; z>=0 ; z-- )
                {
                  // if the current array field shows a value of OFTrue, set 'match' to OFTrue
                  if( seqmatch[z] == OFTrue )
                  {
                    match = OFTrue;
                  }
                  // else delete the corresponding sequence item from
                  // the sequence element of the worklist file
                  else
                  {
                    DcmItem *anItem = sequits1->remove(z);
                    delete anItem;
                  }
                }

                // delete the result array
                delete seqmatch;

                // if no item in the sequence was matched, we will set 'match' to OFFalse
                // and terminate the loop that goes through all elements in the search mask
                if( sequits1 && ( sequits1->card() == 0 ) )
                {
                  match = OFFalse;
                  break;
                }
              }
            }
          }
        } //for

        // if the search pattern and the data set from the current worklist file match,
        // we need to insert the current DcmFileFormat object (which contains the data
        // set that is considered to be a match) into the member variable objlist. Plus,
        // we need to figure out a corresponding result value.
        if( match )
        {
          objlist->append( fileform );
          if( foundUnsupportedOptionalKey )
            status = WLM_PENDING_WARNING;
          else
            status = WLM_PENDING;
        }
        // else we can just delete the current DcmFileFormat object
        else
          delete fileform;
      }
    }
#ifdef HAVE__FINDFIRST
    // close the search handle hFile
    _findclose( hFile );
#else
    // perform the opposite to opendir(...) which was called above
    closedir( dirp );
#endif
  }
  // if it does not refer to a directory, remember this as an error
  else
    status = WLM_REFUSED_OUT_OF_RESOURCES;

  // release the read lock
  ReleaseReadlock();

  // set pointer to the first element in objlist
  objlist->seek( ELP_first );

  // return result
  return status;
}

// ----------------------------------------------------------------------------

DcmDataset *WlmDataSourceFileSystem::NextFindResponse( WlmDataSourceStatusType &rStatus )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function assumes that function StartFindRequest(...) was called earlier and that that
//                function has determined (from the set of worklist files) all those records which match the
//                given search mask. In detail, it is expected that the above mentioned function has written
//                all those records to the member variable objlist. On each call, this function will return
//                one of those records as a (newly created) DcmDataset object. The DcmFileFormat object which
//                is contained in objlist and which refers to the returned object will be removed from objlist.
// Parameters   : rStatus - [out] A value of type WlmDataSourceStatusType denoting if the function call was
//                          successful or not.
// Return Value : The next record which matches the given search mask.
{
  // Create data set result variable.
  DcmDataset *dsetfile2 = new DcmDataset();

  // If objlist is empty (i.e. has cardinality 0),
  // there are no more records that can be returned.
  if( objlist->card() == 0 )
  {
    rStatus = WLM_SUCCESS;
    return dsetfile2;
  }

  // If we get to this point, there is still at least one record
  // (data set) in objlist that has to be returned.

  // Create some temporary variables.
  WlmDataSourceStatusType status = WLM_FAILED_UNABLE_TO_PROCESS;
  DcmElement *newElement = NULL;
  DcmItem *newItem = NULL;

  // Determine the (worklist file) data set whose values shall be returned.
  DcmElement *el = (DcmElement*)objlist->seek_to( 0 );
  DcmFileFormat *theFile = (DcmFileFormat*)el;
  DcmDataset *set = theFile->getDataset();

  // The member variable 'identifiers' contains the search mask. Determine it's cardinality.
  unsigned long int cids = identifiers->card();

  // Go through all elements in the search mask.
  for( unsigned long int j=0; j<cids; j++ )
  {
    // Get the current (search mask) element.
    DcmElement *element = identifiers->getElement( j );

    // Get the current (search mask) element's tag.
    DcmTag tag( element->getTag() );

    // If the current (search mask) element is NOT a sequence.
    if( element->ident() != EVR_SQ )
    {
      // If the current (search mask) element is a supported return key attribute, we
      // need to return it (insert it into the result data set variable).
      if( IsSupportedReturnKeyAttribute( tag.getXTag(), 0 ) )
      {
        // ### wilkens: Werden in der neuen Version eventuell noch andere Datentypen unterstützt?
        // ### Das mal überprüfen.

        // In case the current (search mask) element is of VR (data type) US, like this:
        if( element->ident() == EVR_US )
        {
          newDicomElement( newElement, tag );
          Uint16 USVal;
          if( set->findAndGetUint16( tag, USVal ).good() )
            newElement->putUint16( USVal,0 );
          dsetfile2->insert( newElement, OFTrue /*replaceOld*/ );
        }
        // In all other cases like this:
        else
        {
          newDicomElement( newElement, tag );
          char *val1 = FindStringValue( set, tag );
          if( val1 ) newElement->putString( val1 );
          dsetfile2->insert( newElement, OFTrue /*replaceOld*/ );
        }
      }
      // If the current (search mask) element is not a supported return key attribute,
      // insert an empty element into the result data set variable.
      // ### wilkens: Nach dem 2001er Standard, Teil 4, Abschnitt K.2.2.1.2 sollen die
      // ### Return Attribute gemäß ihrem Data Element Type (1, 1C, 2, 2C, 3) unterstützt werden.
      // ### Wir müssen festlegen, welche Return Attribute von uns unterstützt werden sollen und
      // ### dementsprechend die Funktion IsSupportedReturnKeyAttribute() anpassen. Wenn wir
      // ### trotzdem noch in der Suchmaske auf ein Element treffen, das wir nicht unterstützn, was
      // ### wollen wir dann machen? Wollen wir wirklich ein leeres Element mit dem gegebenen Tag
      // ### einfügen? Ich glaube das würde Sinn machen. Außerdem macht es vielleicht Sinn, an dieser
      // ### Stelle zusätzlich noch eine Warnung auszugeben für den Fall, daß wir alle im aktuellen
      // ### Standard vorgesehenen Return Key Attribute unterstützen und trotzdem auf ein von uns
      // ### nicht-unterstütztes Attribut treffen.
      else
      {
        newDicomElement( newElement, tag );
        dsetfile2->insert( newElement, OFTrue /*replaceOld*/ );
      }
    }
    // If the current (search mask) element IS a sequence
    else
    {
      // Consider this (search mask) element as a sequence of items.
      DcmSequenceOfItems *sequits = (DcmSequenceOfItems*)element;

      // Create a new (result) DcmElement object based on the (search mask's) sequence element's tag.
      newDicomElement( newElement, tag, 0 );

      // Consider this new (result) DcmElement object as a sequence of items.
      DcmSequenceOfItems *newsequits = (DcmSequenceOfItems *)newElement;

      // Insert this (result) DcmSequenceOfItems object into the result data set variable.
      dsetfile2->insert( newsequits, OFTrue /*replaceOld*/ );

      // Search for the sequence of items element in
      // the (worklist-file) data set object
      DcmStack stk1;
      set->search( tag.getXTag(), stk1, ESM_fromHere, OFFalse );
      DcmObject *obj1 = stk1.top();
      DcmSequenceOfItems *sequits1 = (DcmSequenceOfItems*)obj1;

      // Get the (worklist-file's data set's) sequence element's cardinality
      unsigned long csequ1 = 0;
      if( sequits1 )
        csequ1 = sequits1->card();

      // Go through all the (worklist-file's data set's) sequence element's items.
      for( unsigned long int z=0 ; z<csequ1 ; z++ )
      {
        // Get the current (worklist-file's, data set's, sequence element's) item.
        DcmItem *item1 = sequits1->getItem(z);

        // Create a new DcmItem object.
        newItem = new DcmItem();

        // Insert this object into the above created (result) DcmSequenceOfItems object.
        newsequits->insert( newItem );

        // Initialize an indicator variable that indicates if the search
        // mask's sequence of items element is actually empty.
        // ### (wilkens: Das hier sollte man lieber außerhalb der Schleife machen.)
        // ### wilkens: Fehler: Die Laenge einer Sequenz in der Suchmaske darf laut
        // ### Standard niemals 0 sein, da eine Sequenz in der Suchmaske immer
        // ### genau ein Item beinhalten soll (das seinerseits jedoch leer sein kann
        // ### (siehe 2001er Standard, Teil 4, Abschnitt C.2.2.2.6). In solchen Fällen wollen
        // ### wir bestimmt eine Warnmeldung ausgeben. (siehe auch CheckIdentifiers().)
        OFBool emptyIdentifiersItem = OFFalse;
        if( sequits->card() == 0 )
          emptyIdentifiersItem = OFTrue;
        else
        {
          DcmItem *item = sequits->getItem(0);
          if( item == NULL || item->card() == 0 )
            emptyIdentifiersItem = OFTrue;
        }

        // If it actually IS empty.
        // ### wilkens: Diesen Fall wollen wir sicherlich weiter berücksichtigen, auch wenn es
        // ### laut Standard nicht vorkommen darf. Es ist dann nur noch die Frage, was wir
        // ### in diesem Fall machen wollen. Soll die zurückgegebene Sequenz dann auch leer sein?
        // ### Oder wollen wir alle Inhalt aller Items der Sequenz des worklist files zurückliefern?
        // ### Ich denke, die Sequenz sollte dann auch wieder leer sein. (Dann würde die leere Sequenz der
        // ### Suchmaske wie eine Sequenz mit genau einem leeren Item behandelt. Das macht Sinn.)
        // ### Der Schnickschnack hier müßte also umgeschrieben werden.
        if( emptyIdentifiersItem )
        {
          // Go through all elements of the (worklist-file's, data set's, sequence element's) current item
          unsigned long cit1 = item1->card();
          for( unsigned long int w=0 ; w<cit1 ; w++ )
          {
            // Get the current element
            DcmElement *elementseq1 = item1->getElement(w);

            // Get the current element's tag
            DcmTag tagseq1( elementseq1->getTag() );

            // If the current element is a supported return key attribute, we
            // need to return it (insert it into the result data set variable)
            if( IsSupportedReturnKeyAttribute( tagseq1.getXTag(), 1 ) )
            {
              newDicomElement( newElement, tagseq1 );
              char *val1 = GetStringValue( elementseq1 );
              newElement->putString( val1 );
              newItem->insert( newElement, OFTrue /*replaceOld*/);
            }
            // If the current element is not a supported return key attribute,
            // insert an empty element into the data set variable.
            else
            {
              newDicomElement( newElement, tagseq1 );
              newItem->insert( newElement, OFTrue /*replaceOld*/);
            }
          }
        }
        // If it is NOT empty
        else
        {
          // Get the first item of the search mask's sequence of items element
          DcmItem *item = sequits->getItem(0);

          // ### wilkens: Hier gehen wir wieder implizit davon aus, daß in einer Sequenz in der
          // ### Suchmaske genau ein Item vorhanden ist. Das ist ja laut Standard auch so vorgesehen
          // ### aber wollen wir nicht noch lieber einmal genau prüfen, ob es auch wirklich so ist?
          // ### Auch wenn wir das in CheckIdentifiers schon mal gemacht haben, sollten
          // ### wir diesen Fall nochmal abfangen und eine entsprechende Warnmeldung ausgeben.

          // Get its cardinality.
          unsigned long cit = item->card();

          // Go through all elements of the first item of the search mask's sequence of items element.
          for( unsigned long k=0 ; k<cit ; k++ )
          {
            // Get the first element.
            DcmElement *elementseq = item->getElement(k);

            // Get the first element's tag.
            DcmTag tagseq( elementseq->getTag() );

            // If the current element is a supported return key attribute, we
            // need to return it (insert it into the result data set variable).
            if( IsSupportedReturnKeyAttribute( tagseq.getXTag(), 1 ) )
            {
              newDicomElement( newElement, tagseq );
              char *val1 = FindStringValue( item1, tagseq );
              newElement->putString( val1 );
              newItem->insert( newElement, OFTrue /*replaceOld*/);
            }
            // If the current element is not a supported return key attribute,
            // insert an empty element into the data set variable.
            // ### wilkens: Nochmal: Wenn wir in der Suchmaske auf ein Element
            // ### treffen, das wir nicht unterstützen, was wollen wir dann machen?
            // ### Wollen wir wirklich ein leeres Element mit dem gegebenen Tag einfügen? Ich glaube das
            // ### würde Sinn machen. Außerdem macht es vielleicht Sinn, an dieser Stelle zusätzlich noch
            // ### eine Warnung auszugeben für den Fall, daß wir alle im aktuellen Standard vorgesehenen
            // ### Return Key Attribute unterstützen und trotzdem auf ein von uns nicht-unterstütztes
            // ### Attribut treffen.
            else
            {
              newDicomElement( newElement, tagseq );
              newItem->insert( newElement, OFTrue /*replaceOld*/);
            }
          }
        }
      }
    }
  }

  // Determine a result value for the out parameter.
  // ### wilkens: Sollte diese Bedingung hier nicht immer wahr sein? Ich glaube schon.
  if( objlist->card() > 0 )
  {
    if( foundUnsupportedOptionalKey )
      status = WLM_PENDING_WARNING;
    else
      status = WLM_PENDING;

    // Remove the current element from objlist.
    objlist->remove();
  }

  // Free memory.
  delete theFile;

  // Set status correspondingly.
  rStatus = status;

  // Return the data set which was created.
  return dsetfile2;
}

// ----------------------------------------------------------------------------

int WlmDataSourceFileSystem::SetReadlock()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function sets a read lock on the LOCKFILE in the directory
//                that is specified through dfPath and calledApplicationEntityTitle.
// Parameters   : none.
// Return Value : 0 - The read lock has been set successfully.
//                1 - The read lock has not been set successfully.
{
#ifndef _WIN32
  struct flock lockdata;
#endif
  int result;

  // if no path or no calledApplicationEntityTitle is specified, return
  if( dfPath == NULL || calledApplicationEntityTitle == NULL )
  {
    CERR << "WlmDataSourceFileSystem::SetReadlock : Path to data source files not specified." << endl;
    return 1;
  }

  // if a read lock has already been set, return
  if( readLockSetOnDataSource )
  {
    CERR << "WlmDataSourceFileSystem::SetReadlock : Nested read locks not allowed!" << endl;
    return 1;
  }

  // assign path to a local variable
  OFString lockname = dfPath;

  // if the given path does not show a PATH_SEPERATOR at the end, append one
  if( lockname.length() > 0 && lockname[lockname.length()-1] != PATH_SEPARATOR )
    lockname += PATH_SEPARATOR;

  // append calledApplicationEntityTitle, another PATH_SEPERATOR,
  // and LOCKFILENAME to the given path (and seperator)
  lockname += calledApplicationEntityTitle;
  lockname += PATH_SEPARATOR;
  lockname += LOCKFILENAME;

  // open corresponding file
  handleToReadLockFile = open( lockname.c_str(), O_RDWR );
  if( handleToReadLockFile == -1 )
  {
    handleToReadLockFile = 0;
    CERR << "WlmDataSourceFileSystem::SetReadlock : Cannot open file " << lockname.c_str() << endl << "return code: " << strerror(errno) << endl;
    return 1;
  }

  // now set a read lock on the corresponding file

#ifdef _WIN32
  // windows does not have fcntl locking, we need to use our own function
  result = dcmtk_flock( handleToReadLockFile, LOCK_SH );
#else
  lockdata.l_type = F_RDLCK;
  lockdata.l_whence=0;
  lockdata.l_start=0;
  lockdata.l_len=0;
#if SIZEOF_VOID_P == SIZEOF_INT
  // some systems, e.g. NeXTStep, need the third argument for fcntl calls to be
  // casted to int. Other systems, e.g. OSF1-Alpha, won't accept this because int
  // and struct flock * have different sizes. The workaround used here is to use a
  // typecast to int if sizeof(void *) == sizeof(int) and leave it away otherwise.
  result = fcntl( handleToReadLockFile, F_SETLKW, (int)(&lockdata) );
#else
  result = fcntl( handleToReadLockFile, F_SETLKW, &lockdata );
#endif
#endif
  if( result == -1 )
  {
    CERR << "WlmDataSourceFileSystem::SetReadlock : Cannot set read lock on file " << lockname.c_str() << endl;
    dcmtk_plockerr("return code");
    close( handleToReadLockFile );
    handleToReadLockFile = 0;
    return 1;
  }

  // update member variable to indicate that a read lock has been set successfully
  readLockSetOnDataSource = OFTrue;

  // return success
  return 0;
}

// ----------------------------------------------------------------------------

int WlmDataSourceFileSystem::ReleaseReadlock()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function releases a read lock on the LOCKFILE in the given directory.
// Parameters   : none.
// Return Value : 0 - The read lock has been released successfully.
//                1 - The read lock has not been released successfully.
{
#ifndef _WIN32
  struct flock lockdata;
#endif
  int result;

  // if no read lock is set, return
  if( !readLockSetOnDataSource )
  {
    CERR << "WlmDataSourceFileSystem::ReleaseReadlock : No readlock to release" << endl;
    return 1;
  }

  // now release read lock on the corresponding file

#ifdef _WIN32
  // windows does not have fcntl locking
  result = dcmtk_flock( handleToReadLockFile, LOCK_UN );
#else
  lockdata.l_type = F_UNLCK;
  lockdata.l_whence=0;
  lockdata.l_start=0;
  lockdata.l_len=0;
#if SIZEOF_VOID_P == SIZEOF_INT
  result = fcntl( handleToReadLockFile, F_SETLKW, (int)(&lockdata) );
#else
  result = fcntl( handleToReadLockFile, F_SETLKW, &lockdata );
#endif
#endif
  if( result == -1 )
  {
    CERR << "WlmDataSourceFileSystem::ReleaseReadlock : Cannot release read lock" << endl;
    dcmtk_plockerr("return code");
    return 1;
  }

  // close read lock file
  close( handleToReadLockFile );
  handleToReadLockFile = 0;

  // update member variable to indicate that no read lock is set
  readLockSetOnDataSource = OFFalse;

  // return success
  return 0;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::IsWorklistFile( const char *fname )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns OFTrue if the given filename refers to a worklist file,
//                i.e. has an extension of ".wl".
// Parameters   : fname - [in] The name of the file.
// Return Value : OFTrue  - The given filename refers to a worklist file.
//                OFFalse - The given filename does not refer to a worklist file.
{
  // If no filename is specified, return OFFalse
  if( fname == NULL )
    return OFFalse;

  // look for an '.' in the filename
  char *s = (char *)strrchr( fname, '.' );

  // if there was no '.' return OFFalse
  if( s == NULL )
    return OFFalse;

  // if the extension is ".wl" return OFTrue
  if( strcmp( s, ".wl" ) == 0 )
    return OFTrue;

  // return OFFalse in all other cases
  return OFFalse;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::IsValidWorklistFile( DcmDataset *dsetfile, const char *fname )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function checks all elements in the data set information which is contained
//                in the worklist file that is currently being processed if these elements refer to
//                attributes which are supported return attributes. If this is the case, the function
//                will return OFTrue, if this is not the case the function will return OFFalse.
// Parameters   : dsetfile - [in] The data set information which is contained in the current worklist file.
//                fname    - [in] Path and filename of the currently processed worklist file .
// Return Value : OFTrue  - All attributes in the given worklist file are supported return key attributes.
//                OFFalse - Not all attributes in the given worklist file are supported return key attributes.
{
// ### wilkens: wieso wird hier anfangs speziell nach der DCM_ScheduledProcedureStepSequence
// ### gesucht? Die ist doch auch ein supported return key attribute! Ich denke, diese Funktion
// ### muß auch umgeschrieben werden, zumindest diesbezüglich. Außerdem: Was ist mit der Einschränkung,
// ### dass das worklist file keine unsupported return key attributes enthalten darf? Diese Einschränkung
// ### ist ziemlich gravierend, auf der anderen Seite natürlich aber nett zu programmieren. In jedem
// ### Fall sollte man das jedoch irgendwo dokumentieren, wenn sie beibehalten werden soll.
// ### Außerdem wird hier für jede Sequenz innerhalb des worklist files nur geprüft, ob die Sequenz
// ### leer ist (das ist ja hier ok, ist ja nicht die Suchmaske), oder es werden nur die Elemente
// ### des ersten Items einer Sequenz auf "supported return key attribute" getestet. Das riecht nach
// ### einer Einschränkung, daß nichtleere Sequenzen nur ein Item enthalten dürfen. Wenn dies tat-
// ### sächlich so ist (das wird sich in der StartFindRequest-Methode zeigen), ist das ebenfalls eine
// ### gravierende Einschränkung, die dokumentiert werden sollte, wenn sie überhaupt beibehalten werden soll.
// ### Außerdem muß diese Methode umgeschrieben werden, so daß mehrfach geschachtelte Sequenzen
// ### behandelt werden können. Bislang kann man nur eine Sequenz innerhalb einer Sequenz behandeln.
  // Initialize return value
  OFBool ok = OFTrue;

  // Search for the Scheduled Procedure Step Sequence data
  // element in the worklist file's data set information
  DcmStack stk;
  dsetfile->search( DCM_ScheduledProcedureStepSequence, stk );
  DcmObject *obj = stk.top();

  // If there is no such data element, we need to return an error; else, do something.
  if( obj == NULL )
  {
    ok = OFFalse;
    CERR << "WlmDataSourceFileSystem::IsValidWorklistFile : Missing ScheduledProcedureStepSequence in worklist database file: " << fname << endl;
  }
  else
  {
    // In case there is such a data element, we need to determine if all elements in the
    // data set information refer to attributes which are supported return attributes.

    // Determine the number of data elements in the data set.
    unsigned long cdset = dsetfile->card();

    // Start a loop and go through all these elements.
    for( unsigned long i=0; i<cdset ; i++ )
    {
      // Determine the current element.
      DcmElement *element = dsetfile->getElement(i);

      // Determine the current element's length
      Uint32 length = element->getLength();

      // Determine the current element's tag
      DcmTag tag( element->getTag() );

      // Figure out if the current element refers to an attribute which is a supported return
      // attribute. Use level 0 for this check. If this is not the case, set ok to OFFalse and
      // terminate the loop; else go ahead.
      if( !IsSupportedReturnKeyAttribute( tag.getXTag(), 0 ) )
      {
        ok = OFFalse;
        CERR << "WlmDataSourceFileSystem::IsValidWorklistFile : Unsupported attribute: " << tag << " " << tag.getTagName() << " in worklist database file " << fname << endl;
        break;
      }

      // When we get to here we might encounter the case that the current element is a (non-empty)
      // sequence. In such a case, we need to check the elements inside the sequence.
      if( element->ident() == EVR_SQ && length != 0 )
      {
        // Consider this element as a sequence of items.
        DcmSequenceOfItems *sequits = (DcmSequenceOfItems*)element;

        // Get the first item.
        DcmItem *item = sequits->getItem(0);

        // Get its cardinality.
        unsigned long cit = item->card();

        // Start a loop and go through all elements of this item.
        for( unsigned long k=0 ; k<cit ; k++ )
        {
          // Get the current element.
          DcmElement *elementseq = item->getElement(k);

          // Determine the current element's tag.
          DcmTag tagseq( elementseq->getTag() );

          // Figure out if the current element refers to an attribute which is a supported return key
          // attribute. Use level 1 for this check. If that is not the case, set ok to OFFalse and
          // terminate the loop
          if( !IsSupportedReturnKeyAttribute( tagseq.getXTag(), 1 ) )
          {
            ok = OFFalse;
            CERR << "WlmDataSourceFileSystem::IsValidWorklistFile : Unsupported attribute: " << tagseq << " " << tagseq.getTagName() << " in worklist database file: " << fname << endl;
            break;
          }
        } //for
      } //if

      // If we encountered an unsupported return key attribute
      // inside a sequence element, terminate the loop.
      if( !ok )
        break;

    }// for
  }// else

  // return result
  return ok;
}

// ----------------------------------------------------------------------------

char *WlmDataSourceFileSystem::FindStringValue( DcmItem *dset, const DcmTagKey &tag )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function looks for a DICOM string element with tag 'tag' in the data set which
//                was passed as first argument and returns the value of this element (attribute).
//                If the element does not refer to a string attribute, a NULL pointer is returned.
// Parameters   : dset - [in] Data set which contains DICOM elements.
//                tag  - [in] Element (attribute) that shall be looked for in dset.
// Return Value : The value of the given DICOM (string) element or NULL.
{
  DcmStack stk;
  char *val = NULL;

  dset->search( tag, stk, ESM_fromHere, OFFalse );
  DcmObject *obj = stk.top();
  if( obj )
    val = GetStringValue((DcmElement*)obj);

  return val;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::ValueMatchesPattern( const char *value, const char *pattern )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function determines if the value which was passed matches the pattern which was
//                passed. The function will return OFTrue if this is the case; if this is not the case
//                it will return OFFalse.
// Parameters   : value   - [in] Value which shall be matched against pattern.
//                pattern - [in] Matching pattern.
// Return Value : OFTrue  - The value matches the pattern.
//                OFFalse - The value does not match the pattern.
{
  // if there is no pattern return OFTrue (everything matches an unknown pattern)
  if( pattern == NULL )
    return OFTrue;

  // copy pattern
  char *pat = new char[ strlen(pattern) + 1 ];
  strcpy( pat, pattern );

  // copy or generate a string value for comparison with the pattern
  char *val;
  if( value == NULL )
  {
    val = new char[ 1 ];
    val[0] = '\0';
  }
  else
  {
    val = new char[ strlen(value) + 1 ];
    strcpy( val, value );
  }

  // intialize return value
  OFBool result = OFFalse;

  // strip trailing spaces
  val = DU_stripTrailingSpaces( val );
  pat = DU_stripTrailingSpaces( pat );

  // if the pattern is an empty string, it is considered to be a match
  if( pattern[0] == '\0')
    result = OFTrue;
  // else if the string value matches the pattern (also taking into account
  // wild card symbols), it is for sure considered to be a match
  else if( WildcardMatch( val, pat ) )
    result = OFTrue;

  // free memory
  delete val;
  delete pat;

  // return result
  return result;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::WildcardMatch( const char *value, const char *pattern )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function determines if the string value which was passed as argument 1 matches
//                the pattern value which was passed as argument 2. (Note that the pattern value might
//                contain wild card symbols.) The function will return OFTrue if this is the case; if
//                this is not the case it will return OFFalse.
// Parameters   : value   - [in] Value which shall be matched against pattern. (non-NULL value expected.)
//                pattern - [in] Matching pattern (may contain wild card symbols). (non-NULL value expected.)
// Return Value : OFTrue  - The value matches the pattern.
//                OFFalse - The value does not match the pattern.
{
  // initialize return value
  OFBool ok = OFFalse;

  // go through both strings character by character as long as
  // a) we do not see a EOS in the pattern AND
  // b) we do not see a '*' in the pattern AND
  // c) the current character in the value is equal to the
  //    current character in the pattern, or the pattern con-
  //    tains a '?' and the value does not yet show an EOS.
  while( *pattern != '\0' &&
         *pattern != '*'  &&
         ( *pattern == *value || ( *pattern == '?' && *value != '\0' ) ) )
  {
    pattern++;
    value++;
  }

  // if the current pattern character equals the star symbol (wild card symbol) we need
  // to call another function. If this is not the case, ok will be set to OFTrue or
  // OFFalse, depending on if we got to the end of both strings in the above loop.
  if( *pattern == '*' )
    ok = MatchStarSymbol( value, pattern+1 );
  else
  {
    if( *pattern == '\0' && *value == '\0' )
      ok = OFTrue;
    else
      ok = OFFalse;
  }
  // return result
  return ok;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::MatchStarSymbol( const char *value, const char *pattern )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function is called, if the search pattern contains a star symbol. It determines
//                if the string value which was passed as argument 1 still matches the pattern value
//                which was passed as argument 2. This function takes the star symbol in wc into account.
//                (Note that the pattern value might contain more wild card symbols.) The function will
//                return OFTrue if there is a match; if there is not a match it will return OFFalse.
// Parameters   : value   - [in] Value which shall be matched against pattern. (non-NULL value expected.)
//                pattern - [in] Matching pattern (may contain wild card symbols). (non-NULL value expected.)
// Return Value : OFTrue  - The value matches the pattern.
//                OFFalse - The value does not match the pattern.
{
  // initialize result value
  OFBool ok = OFFalse;

  // move pointer one char to the right as long as it points to a star symbol
  while( *pattern == '*' )
    pattern++;

  // if we got to the end of the matching pattern, return OFTrue
  if( *pattern == '\0' )
    return OFTrue;

  // if there is something else at the end of the matching pattern,
  // we need to go ahead and compare the rest of the two strings

  // as long as ok equals OFFalse and we did not get to the end of the string
  while( !ok && *value != '\0' )
  {
    // if the pattern reveals a '?' or if both pointers refer to the same
    // character, we need to call WildcardMatch again, to determine a result
    if( *pattern == '?' || *value == *pattern )
      ok = WildcardMatch( value+1, pattern+1 );

    // if ok still equals OFFalse, set pointer one character to the right
    if( !ok )
      value++;
  }

  // return result
  return ok;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::MatchTime( const char *value, const char *pattern )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function determines if the time value which was passed as a first argument matches
//                the time/timerange pattern which was passed as a second argument. The function will return
//                OFTrue if this is the case; if this is not the case it will return OFFalse.
// Parameters   : value   - [in] Value which shall be matched against pattern. (Always represents a point in time.)
//                pattern - [in] Pattern. (Might represent a time range.)
// Return Value : OFTrue  - The time value matches the time/timerange pattern.
//                OFFalse - The time value does not match the time/timerange pattern.
{
  OFBool ok = OFFalse;
  char *pat = NULL, *val = NULL;

  // Make a copy of the pattern if there is one and remove its trailing spaces
  if( pattern != NULL )
  {
    char *pat = new char[ strlen( pattern ) + 1 ];
    strcpy( pat, pattern );
    pat = DU_stripTrailingSpaces( pat );
  }

  // Make a copy of the value if there is one and remove its trailing spaces
  if( value != NULL )
  {
    char *val = new char[ strlen( value ) + 1 ];
    strcpy( val, value );
    val = DU_stripTrailingSpaces( val );
  }

  // If there is no or only an empty pattern, this is considered to be a match.
  if( pat == NULL || pat[0] == '\0' )
    ok = OFTrue;
  // Else if the value which shall be compared to pattern is NULL or empty,
  // this is considered to be NO match since the pattern has contents.
  else if( val == NULL || val[0] == '\0' )
    ok = OFFalse;
  // in all other cases do the following
  else
  {
    // Create a standard string which represents the value which shall be compared to the
    // pattern. (Since this value comes out of some data source, we expect it to be of a
    // correct format so that StandardizeTime() will always return a valid value.)
    char *stdValue = StandardizeTime( val );

    // If a '-' character cannot be found in the pattern, it does not represent a time range
    // but a specific point in time. In this case we want to compare the two points in time.
    if( !strchr( pat,'-' ) )
    {
      // Create a standard string which represents the pattern
      char *stdPattern = StandardizeTime( pat );

      // If the standardized pattern is valid and it equals the standardized
      // value, ok will be set to OFTrue, else ok will be set to OFFalse
      if( stdPattern != NULL )
        ok = ( strcmp( stdValue, stdPattern ) == 0 );
      else
        ok = OFFalse;

      // free memory
      delete stdPattern;
    }
    // Else if a '-' character CAN be found in the pattern, the pattern represents a time range
    // and we need to call another function to determine the comparison's result.
    else
      ok = RangematchTime( stdValue, pat );

    // free memory
    delete stdValue;
  }

  // free memory
  if( pat ) delete pat;
  if( val ) delete val;

  // return result
  return( ok );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::MatchDate( const char *value, const char *pattern )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function determines if the (date) value which was passed as a first argument matches
//                the (date) pattern which was passed as a second argument. The function will return OFTrue
//                if this is the case; if this is not the case it will return OFFalse.
// Parameters   : value   - [in] Value which shall be matched against pattern.
//                pattern - [in] Matching pattern.
// Return Value : OFTrue  - .
//                OFFalse - .
{
  OFBool ok = OFFalse;
  char *pat = NULL, *val = NULL;

  // Make a copy of the pattern if there is one and remove its trailing spaces.
  if( pattern != NULL )
  {
    char *pat = new char[ strlen( pattern ) + 1 ];
    strcpy( pat, pattern );
    pat = DU_stripTrailingSpaces( pat );
  }

  // Make a copy of the value if there is one and remove its trailing spaces.
  if( value != NULL )
  {
    char *val = new char[ strlen( value ) + 1 ];
    strcpy( val, value );
    val = DU_stripTrailingSpaces( val );
  }

  // If there is no or only an empty pattern, this is considered to be a match.
  if( pat == NULL || pat[0] == '\0' )
    ok = OFTrue;
  // Else if the value which shall be compared to pattern is NULL or empty,
  // this is considered to be NO match since the pattern has contents.
  else if( val == NULL || val[0] == '\0' )
    ok = OFFalse;
  // In all other cases do the following:
  else
  {
    // Create a standard string which represents the value which shall be compared to the
    // pattern. (Since this value comes out of some data source, we expect it to be of a
    // correct format so that StandardizeDate() will always return a valid value.)
    char *stdValue = StandardizeDate( val );

    // If a '-' character cannot be found in the pattern, it does not represent a date range
    // but a specific date. In this case we want to compare the two dates.
    if( !strchr( pat,'-' ) )
    {
      // Create a standard string which represents the pattern.
      char *stdPattern = StandardizeDate( pat );

      // If the standardized pattern is valid and it equals the standardized
      // value, ok will be set to OFTrue, else ok will be set to OFFalse
      if( stdPattern != NULL )
        ok = ( strcmp( stdValue, stdPattern ) == 0 );
      else
        ok = OFFalse;

      // free memory
      delete stdPattern;
    }
    // Else if a '-' character CAN be found in the pattern, the pattern represents a date range
    // and we need to call another function to determine the comparison's result.
    else
      ok = RangematchDate( stdValue, pat );

    // free memory
    delete stdValue;
  }

  // free memory
  if( pat ) delete pat;
  if( val ) delete val;

  // return result
  return( ok );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::RangematchTime( const char *timeValue, const char *timeRange )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns OFTrue if the time value which was passed as a first argument
//                lies whithin the time range which was passed as a second argument. Otherwise, OFFalse
//                will be returned.
// Parameters   : timeValue - [in] Time value, already standardized with StandardizeTime.
//                timeRange - [in] Time range, not yet standardized.
// Return Value : OFTrue - The time value lies whithin the time range.
//                OFFalse - The time value does not ly whithin the time range (or some error occurred).
{
  OFBool ok = OFFalse;

  // If there is no time value or no time range, return OFFalse
  if( timeValue == NULL || timeRange == NULL )
    return OFFalse;

  // Copy range into new variable
  int len = strlen( timeRange );
  char *range = new char[ len + 1 ];
  strcpy( range, timeRange );

  // Determine a pointer to the first occurrence of the hyphen in the time range string.
  char *hyphen = strchr( range, '-' );

  // If the hyphen can be found at all in the time range string, do something.
  if( strchr( range, '-' ) )
  {
    // If the hyphen is at the first position, the time value is considered to match
    // the time range if it specifies a point in time which is earlier than or at the
    // same time as the (standardized) ending time of the time range.
    if( range[0] == '-' )
    {
      char *n1 = StandardizeTime( &range[1] );
      if( n1 != NULL )
      {
        if( strcmp( timeValue, n1 ) <= 0 )
          ok = OFTrue;
        else
          ok = OFFalse;
      }
      else
        ok = OFFalse;
      delete n1;
    }
    // If the hyphen is at the last position, the time value is considered to match
    // the time range if it specifies a point in time which is later than or at the
    // same time as the (standardized) beginning time of the time range.
    else if( range[len-1] == '-' && range[len] == '\0' )
    {
      range[len-1] = '\0';
      char *n1 =  StandardizeTime( range );
      if( n1 != NULL )
      {
        if( strcmp( timeValue, n1 ) >= 0 )
          ok = OFTrue;
        else
          ok = OFFalse;
      }
      else
        ok = OFFalse;
      delete n1;
    }
    // If the hyphen is neither at the beginning nor at the end, it must be in the middle
    // between 2 specified points in time. In such a case, the time value is considered to
    // match the time range if it specifies a point in time which is between (or at one of)
    // the two specified (standardized) starting and ending times of the time range.
    else
    {
      *hyphen = '\0';
      hyphen++;
      char *n1 = StandardizeTime( range );
      char *n2 = StandardizeTime( hyphen );
      if( n1 != NULL && n2 != NULL )
      {
        if( strcmp( timeValue, n1 ) >= 0 && strcmp( timeValue, n2 ) <= 0 )
          ok = OFTrue;
        else
          ok = OFFalse;
      }
      else
        ok = OFFalse;
      delete n1;
      delete n2;
    }
  }
  // If the hyphen cannot be found, something is wrong, return OFFalse.
  else
    ok = OFFalse;

  // Free memory
  delete range;

  // return result
  return( ok );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::RangematchDate( const char *dateValue, const char *dateRange )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns OFTrue if the date value which was passed as a first argument
//                lies whithin the date range which was passed as a second argument. Otherwise, OFFalse
//                will be returned.
// Parameters   : dateValue - [in] Date value, already standardized with StandardizeDate.
//                dateRange - [in] Date range, not yet standardized.
// Return Value : OFTrue - The date value lies whithin the date range.
//                OFFalse - The date value does not ly whithin the date range (or some error occurred).
{
  OFBool ok = OFFalse;

  // If there is no date value or no date range, return OFFalse
  if( dateValue == NULL || dateRange == NULL )
    return( OFFalse );

  // Copy range into new variable
  int len = strlen( dateRange );
  char *range = new char[ len + 1 ];
  strcpy( range, dateRange );

  // Determine a pointer to the first occurrence of the hyphen in the date range string.
  char *hyphen = strchr( range, '-' );

  // If the hyphen can be found at all in the date range string, do something.
  if( strchr( range, '-' ) )
  {
    // If the hyphen is at the first position, the date value is considered to match
    // the date range if it specifies a day which is earlier than or on the
    // same day as the (standardized) ending day of the day range.
    if( range[0] == '-' )
    {
      char *n1 = StandardizeDate( &range[1] );
      if( n1 != NULL )
      {
        if( strcmp( dateValue, n1 ) <= 0 )
          ok = OFTrue;
        else
          ok = OFFalse;
      }
      else
        ok = OFFalse;
      delete n1;
    }
    // If the hyphen is at the last position, the date value is considered to match
    // the date range if it specifies day which is later than or on the
    // same day as the (standardized) beginning day of the time range.
    else if( range[len-1] == '-' && range[len] == '\0' )
    {
      range[len-1] = '\0';
      char *n1 =  StandardizeDate( range );
      if( n1 != NULL )
      {
        if( strcmp( dateValue, n1 ) >= 0 )
          ok = OFTrue;
        else
          ok = OFFalse;
      }
      else
        ok = OFFalse;
      delete n1;
    }
    // If the hyphen is neither at the beginning nor at the end, it must be in the middle
    // between 2 specified days. In such a case, the date value is considered to
    // match the date range if it specifies a day which is between (or on one of)
    // the two specified (standardized) starting and ending days of the date range.
    else
    {
      *hyphen = '\0';
      hyphen++;
      char *n1 = StandardizeDate( range );
      char *n2 = StandardizeDate( hyphen );
      if( n1 != NULL && n2 != NULL )
      {
        if( strcmp( dateValue, n1 ) >= 0 && strcmp( dateValue, n2 ) <= 0 )
          ok = OFTrue;
        else
          ok = OFFalse;
      }
      else
        ok = OFFalse;
      delete n1;
      delete n2;
    }
  }
  // If the hyphen cannot be found, something is wrong, return OFFalse.
  else
    ok = OFFalse;

  // Free memory
  delete range;

  // return result
  return( ok );
}

// ----------------------------------------------------------------------------

char *WlmDataSourceFileSystem::StandardizeTime( const char *timeString )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function creates a standard string which represents a certain time value
//                and returns this string. If an error occured or an invalid parameter was passed,
//                the NULL pointer will be returned.
// Parameters   : timeString - [in] Time value which shall be standardized.
//                             This timeString can be of format "hhmmss.fracxx" or "hh:mm:ss.fracxx"
//                             according to the 2001 specification of the DICOM standard, part 5, Table 6.2-1.
//                             Note that one or more of the components mm, ss, or fracxx may be unspecified
//                             (missing) as long as every component to the right of an unspecified component
//                             is also unspecified.
// Return Value : A (newly created) string of the format "hhmmss.fracxx" (all components specified), or
//                NULL if an error occurred.
{
  // if there is no time string, return NULL
  if( !timeString )
    return( NULL );

  // according to the 2001 specification of the DICOM standard, part 5, Table 6.2-1, a time
  // string that has less than 2 characters is not allowed. In this case, return NULL.
  if( strlen( timeString ) < 2 )
    return( NULL );

  // Create and initialize result standard time string (13 characters wide plus one for EOS).
  char *stdTime = new char[14];
  stdTime[13]= '\0';

  // Go through the given time string and create a standard representation "hhmmss.fracxx"
  // (one or more of the components mm, ss, or fracxx may be missing). Neglect superfluous
  // information. Set error indicator variable if something goes wrong.
  int i = 0;
  OFBool error = OFFalse;
  while( *timeString && !error )
  {
    if( ( *timeString >= '0' && *timeString <='9' ) ||
        ( *timeString == '.' && i == 6 ) )
    {
      if( i<13 )
        stdTime[i] = *timeString;
      i++;
    }
    else if( ( *timeString != ':' || ( i!=2 && i!=4 ) ) && *timeString != ' ' )
      error = OFTrue;

    timeString++;
  }

  // Check if the resulting string has less than 2 characters, or if it is
  // odd and at the same time has less than 6 characters. If this is the case
  // something is wrong; hence, set the error indicator variable to true.
  if( i<2 || ( ( i & 1 ) && i<6 ) )
    error = OFTrue;

  // If there is no error, we might want to modify the string even further
  // (because of missing components).
  if( !error )
  {
    // If the string has less than 6 characters, we want append '0's until it has 6 characters.
    if( i<6 )
      for( ; i<6 ; i++ )
        stdTime[i] = '0';

    // If the string has exaactly 6 characters, we want to add a '.' at the current position.
    if( i==6 )
      stdTime[i++] = '.';

    // If the string has less than 13 characters, we want append '0's until it has 13 characters.
    if( i<13 )
      for( ; i<13 ; i++ )
        stdTime[i] = '0';
  }

  // If an error occurred, we want to free the allocated memory and return NULL.
  if( error )
  {
    delete stdTime;
    return( NULL );
  }

  // Return resulting standard time representation.
  return( stdTime );
}

// ----------------------------------------------------------------------------

char *WlmDataSourceFileSystem::StandardizeDate( const char *dateString )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function creates a standard string which represents a certain date value
//                and returns this string. If an error occured or an invalid parameter was passed,
//                the NULL pointer will be returned.
// Parameters   : dateString - [in] Date value which shall be standardized.
//                             This dateString can be of format "yyyymmdd" or "yyyy.mm.dd"
//                             according to the 2001 specification of the DICOM standard, part 5, Table 6.2-1.
// Return Value : A (newly created) string of the format "yyyymmdd", or NULL if an error occurred.
{
  // if there is no date string, return NULL
  if( !dateString )
    return( NULL );

  // according to the 2001 specification of the DICOM standard, part 5, Table 6.2-1, a date
  // string that has less than 8 characters is not allowed. In this case, return NULL.
  if( strlen( dateString ) < 8 )
    return( NULL );

  // Create and initialize result standard time string (8 characters wide plus one for EOS).
  char *stdDate = new char[9];
  stdDate[8] = '\0';

  // Go through the given date string and create a standard representation "yyyymmdd"
  // Neglect superfluous information. Set error indicator variable if something goes wrong.
  int i = 0;
  OFBool error = OFFalse;
  while( *dateString && !error )
  {
    if( *dateString >= '0' && *dateString <='9' )
    {
      if( i<8 )
        stdDate[i] = *dateString;
      i++;
    }
    else if( ( *dateString != '.' || ( i!=4 && i!=6 ) ) && *dateString != ' ' )
      error = OFTrue;

    dateString++;
  }

  // If an error occurred or if the length of the created string does not equal 8
  // we want to free the allocated memory and return NULL.
  if( error || i!=8 )
  {
    delete stdDate;
    return( NULL );
  }

  // Return resulting standard date representation.
  return( stdDate );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::IsSupportedMatchingKeyAttribute( const DcmTagKey &key, int level )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function checks if the given tag refers to an attribute which is a supported
//                matching key attribute. Note that there are 2 levels of supported matching key
//                attributes:
//                - Level 0 supports the attributes DCM_PatientsName, DCM_PatientID, and
//                  DCM_ScheduledProcedureStepSequence.
//                - Level 1 supports the attributes DCM_ScheduledStationAETitle,
//                  DCM_ScheduledProcedureStepStartDate, DCM_ScheduledProcedureStepStartTime,
//                  DCM_Modality, DCM_ScheduledPerformingPhysiciansName, DCM_ScheduledStationName,
//                  DCM_ScheduledProcedureStepLocation, and DCM_CommentsOnTheScheduledProcedureStep.
// Parameters   : key   - [in] The tag that shall be checked.
//                level - [in] Level on which shall be checked.
// Return Value : OFTrue  - The given tag is a supported matching key attribute.
//                OFFalse - The given tag is not a supported matching key attribute.
{
// ### wilkens: Ich glaube das mit den beiden Levels müssen wir abschaffen
  // initialize result variable
  OFBool ok = OFFalse;

  // depending on the level, conduct the checking
  if( level == 0 )
  {
    ok = (key == DCM_PatientsName) ||
         (key == DCM_PatientID) ||
         (key == DCM_ScheduledProcedureStepSequence);
  }
  else if( level == 1 )
  {
    ok = (key == DCM_ScheduledStationAETitle) ||
         (key == DCM_ScheduledProcedureStepStartDate) ||
         (key == DCM_ScheduledProcedureStepStartTime) ||
         (key == DCM_Modality) ||
         (key == DCM_ScheduledPerformingPhysiciansName) ||
         (key == DCM_ScheduledStationName) ||
         (key == DCM_ScheduledProcedureStepLocation) ||
         (key == DCM_CommentsOnTheScheduledProcedureStep);
  }

  // return result
  return ok;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSourceFileSystem::IsSupportedReturnKeyAttribute( const DcmTagKey &key, int level )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function checks if the given tag refers to an attribute which is a supported
//                return key attribute. Note that there are 2 levels of supported return key attributes:
//                - Level 0 supports the attributes DCM_PatientsName, DCM_PatientID, and
//                  DCM_ScheduledProcedureStepSequence (accounted for in IsSupportedMatchingKeyAttribute(...))
//                  plus the following attributes:
//                  DCM_SpecificCharacterSet, DCM_MedicalAlerts, DCM_ContrastAllergies,
//                  DCM_RequestedProcedureID, DCM_RequestedProcedureDescription, DCM_StudyInstanceUID,
//                  DCM_RequestedProcedurePriority, DCM_AccessionNumber, DCM_RequestingPhysician,
//                  DCM_PatientsBirthDate, DCM_PatientsSex, DCM_PatientTransportArrangements,
//                  DCM_ReferringPhysiciansName, DCM_AdmissionID, DCM_CurrentPatientLocation,
//                  DCM_PatientsWeight, DCM_ConfidentialityConstraintOnPatientDataDescription,
//                  DCM_PatientState, DCM_PregnancyStatus, and DCM_SpecialNeeds.
//                - Level 1 supports the attributes DCM_ScheduledStationAETitle,
//                  DCM_ScheduledProcedureStepStartDate, DCM_ScheduledProcedureStepStartTime,
//                  DCM_Modality, DCM_ScheduledPerformingPhysiciansName, DCM_ScheduledStationName,
//                  DCM_ScheduledProcedureStepLocation, and DCM_CommentsOnTheScheduledProcedureStep
//                  (accounted for in isaSupportedMatchingKey(...)) plus the following attributes:
//                  DCM_ScheduledProcedureStepDescription, DCM_PreMedication, DCM_ScheduledProcedureStepID,
//                  and DCM_RequestedContrastAgent.
// Parameters   : key   - [in] The tag that shall be checked.
//                level - [in] Level on which shall be checked.
// Return Value : OFTrue  - The given tag is a supported return key attribute.
//                OFFalse - The given tag is not a supported return key attribute.
{
// ### wilkens: Ich glaube das mit den beiden Levels müssen wir abschaffen
  // initialize result variable
  OFBool ok = OFFalse;

  // depending on the level, conduct the checking
  if( level == 0 )
  {
    ok = IsSupportedMatchingKeyAttribute(key, level) ||
         (key == DCM_SpecificCharacterSet) ||
         (key == DCM_MedicalAlerts) ||
         (key == DCM_ContrastAllergies) ||
         (key == DCM_RequestedProcedureID) ||
         (key == DCM_RequestedProcedureDescription) ||
         (key == DCM_StudyInstanceUID) ||
         (key == DCM_RequestedProcedurePriority) ||
         (key == DCM_AccessionNumber) ||
         (key == DCM_RequestingPhysician) ||
         (key == DCM_PatientsBirthDate) ||
         (key == DCM_PatientsSex) ||
         (key == DCM_PatientTransportArrangements) ||
         (key == DCM_ReferringPhysiciansName) ||
         (key == DCM_AdmissionID) ||
         (key == DCM_CurrentPatientLocation) ||
         (key == DCM_PatientsWeight) ||
         (key == DCM_ConfidentialityConstraintOnPatientDataDescription) ||
         (key == DCM_PatientState) ||
         (key == DCM_PregnancyStatus) ||
         (key == DCM_SpecialNeeds);
  }
  else if (level == 1)
  {
    ok = IsSupportedMatchingKeyAttribute(key, level) ||
         (key == DCM_ScheduledProcedureStepDescription) ||
         (key == DCM_PreMedication) ||
         (key == DCM_ScheduledProcedureStepID) ||
         (key == DCM_RequestedContrastAgent);
  }

  // return result
  return ok;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


/*
** CVS Log
** $Log: wldsfs.cc,v $
** Revision 1.1  2002-04-18 10:26:48  wilkens
** Performed split between db-variant and pki-variant.
**
** Revision 1.6  2002/04/18 10:15:49  wilkens
** Corrected recognition of non-standard characters, added new supported return
** key attributes, updated checking the search mask.
**
** Revision 1.5  2002/04/11 13:07:28  joergr
** Use the new standard file system routines like fileExists() etc.
** Added support for MT-safe system routines (cuserid, getlogin, readdir, ...).
**
** Revision 1.4  2002/01/08 19:14:53  joergr
** Minor adaptations to keep the gcc compiler on Linux and Solaris happy.
** Currently only the "file version" of the worklist SCP is supported on
** Unix systems.
**
** Revision 1.3  2002/01/08 17:46:04  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 16:59:06  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:32:47  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/

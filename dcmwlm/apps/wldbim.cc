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
*  Author:  Thomas Wilkens, Marcel Claus
*
*  Purpose: Class for managing database interaction.
*
*  Last Update:      $Author: wilkens $
*  Update Date:      $Date: 2002-07-17 13:10:13 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/apps/Attic/wldbim.cc,v $
*  CVS/RCS Revision: $Revision: 1.3 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

// ----------------------------------------------------------------------------

#include "osconfig.h"
#include "ofconsol.h"
#include "ofstd.h"
#include "dicom.h"
#include "wltypdef.h"
#include "dctk.h"
#include <stdio.h>
#define OTL_ORA7 // Compile OTL 3.1/OCI7
#include <otlv32.h> // include the OTL 3.2 header file

#include "wldbim.h"

// ----------------------------------------------------------------------------

WlmDatabaseInteractionManager::WlmDatabaseInteractionManager()
// Date         : December 18, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : logStream( NULL ), verboseMode( OFFalse ), myDcmTagAnker( NULL )
{
  strcpy( uidPrefix, "" );
  strcpy( paStmt, "" );
  strcpy( wlmCountStmt, "" );
  strcpy( wlmIDStmt, "" );
  strcpy( wlmIDInsertStmt, "" );
  strcpy( wlmIDUpdateStmt, "" );
  strcpy( connectStr, "" );
}

// ----------------------------------------------------------------------------

WlmDatabaseInteractionManager::~WlmDatabaseInteractionManager()
// Date         : December 18, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::SetLogStream( OFConsole *value )
// Date         : March 15, 2002
// Author       : Thomas Wilkens
// Task         : Set value in member variable.
// Parameters   : value - [in] The value to set.
// Return Value : none.
{
  logStream = value;
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::SetVerbose( OFBool value )
// Date         : March 15, 2002
// Author       : Thomas Wilkens
// Task         : Set value in member variable.
// Parameters   : value - [in] The value to set.
// Return Value : none.
{
  verboseMode = value;
}

// ----------------------------------------------------------------------------

OFCondition WlmDatabaseInteractionManager::ConnectToDatabase( char *dbDsn, char *dbUserName, char *dbUserPassword, int serialNumber, char *cfgFileMatchRecords, char *cfgFileSelectValues, WlmDatabaseType databaseType )
// Date         : March 15, 2002
// Author       : Thomas Wilkens
// Task         : Connects to the database.
// Parameters   : dbDsn               - [in] Data Source Name
//                dbUserName          - [in] Database user name
//                dbUserPassword      - [in] Database user password
//                serialNumber        - [in] Serial number
//                cfgFileMatchRecords - [in] Name of configuration file for determination of matching records.
//                cfgFileSelectValues - [in] Name of configuration file for selection of attribute values.
//                databaseType        - [in] Type of database.
// Return Value : Indicates if the connection could be established or not.
{
  // check parameters
  if( dbUserName == NULL || dbUserPassword == NULL || serialNumber == NULL || cfgFileMatchRecords == NULL || cfgFileSelectValues == NULL || databaseType == DATABASE_TYPE_UNKNOWN )
  {
    DumpMessage("Invalid parameters, cannot connect to database...");
    return( WLM_EC_CannotConnectToDataSource );
  }

  // add serial number to OFFIS GO-Kard UID root
  sprintf( uidPrefix, "1.2.276.0.7230010.8.%d", serialNumber );

  // check if database statement configuration files are existent
  if( !OFStandard::fileExists( OFString( cfgFileMatchRecords ) ) || !OFStandard::fileExists( OFString( cfgFileSelectValues ) ) )
    return( WLM_EC_DatabaseStatementConfigFilesNotExistent );

  // create connect string
  if( databaseType == DATABASE_ORACLE )
  {
    if( dbDsn[0] )
      sprintf( connectStr, "%s/%s@%s", dbUserName, dbUserPassword, dbDsn );
    else
      sprintf( connectStr, "%s/%s", dbUserName, dbUserPassword );
  }

  // initialize OCI environment
  otl_connect::otl_initialize();

  // logon to database
  int error = LogonToDatabase( 0, 10000 );
  if( error == -1 )
    return( WLM_EC_CannotConnectToDataSource );

  // read statements in configuration files
  ReadSearchStmt( cfgFileMatchRecords );
  ReadDcmTags( cfgFileSelectValues );

  // return success
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

OFCondition WlmDatabaseInteractionManager::DisconnectFromDatabase()
// Date         : March 15, 2002
// Author       : Thomas Wilkens
// Task         : Disconnects from the database.
// Parameters   : none.
// Return Value : Indicates if the connection was disconnected successfully.
{
  // free list of tags and select statements for supported attributes
  myDcmTag *hdt;
  hdt = myDcmTagAnker;
  myDcmTag *hdt2;
  while( hdt )
  {
    hdt2 = hdt;
    hdt = hdt->next;
    free( hdt2 );
  }

  // disconnect from Oracle
  db.logoff();

  // return success
  return( EC_Normal );
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::ExtractRange( const char *theSource, const char *noValueMin, const char *noValueMax, const char *rangeValueMin, const char *rangeValueMax, char *targetMin, char *targetMax )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function extracts the range from theSource , where theSource is in DICOM-Notation with "-".
//                The range will be given back in targetMin and targetMax
// Parameters   : theSource     - [in] The original string from which the range will be extracted.
//                noValueMin    - [in] minvalue to be copied in targetMin when theSource is empty
//                noValueMax    - [in] maxvalue to be copied in targetMax when theSource is empty
//                rangeValueMin - [in] minvalue to be copied in targetMin when theSource has no lower bound
//                rangeValueMax - [in] maxvalue to be copied in targetMax when theSource has no upper bound
//                targetMin     - [out] minvalue of range
//                targetMax     - [out] maxvalue of range
// Return Value : none.
{
  if( theSource != NULL && theSource[0] != 0 )
  {
    char *rangeChar;
    if( rangeChar = strstr( theSource, "-" ) )
    {
      // if source has no lower bound
      if( theSource[0] == '-' )
      {
        strcpy( targetMin, rangeValueMin );
        strcpy( targetMax, rangeChar + 1 );
      }
      else
      {
        // if source has no upper bound
        if( theSource[ strlen(theSource) - 1 ] == '-' )
        {
          strcpy( targetMin, theSource );
          targetMin[ strlen(theSource) - 1 ] = 0;
          strcpy( targetMax, rangeValueMax );
        }
        // if source has lower and upper bound
        else
        {
          strcpy( targetMax, rangeChar + 1 );
          strcpy( targetMin, theSource );
          rangeChar = strstr( targetMin, "-" );
          rangeChar[0] = 0;
        }
      }
    }
    // if source has no range
    else
    {
      strcpy( targetMin, theSource );
      strcpy( targetMax, theSource );
    }
  }
  // if source is empty use default values in noValueMin and noValueMax
  else
  {
    strcpy( targetMin, noValueMin );
    strcpy( targetMax, noValueMax );
  }
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::DoReplace( char *theStr, const char *oldStr, const char *newStr )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function replaces in theStr all occurences of oldStr with newStr.
//                The replacement is done directly on theStr: NO COPY WILL BE MADE!!!!
// Parameters   : theStr - [in] The original string which will be modified.
//                oldStr - [in] String which shall be replaced
//                newStr - [in] String which shall be the replacement
// Return Value : none.
{
  int oldLen, newLen;
  char *p, *q;
  oldLen = strlen(oldStr);
  newLen = strlen(newStr);

  // as long as we have something to replace...
  while( NULL != ( p = strstr( theStr, oldStr ) ) )
  {
    memmove( q = p + newLen, p + oldLen, strlen( p + oldLen ) + 1 );
    memcpy( p, newStr, newLen );
  }
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::ReadDcmTags( char *iniFile )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function reads a list of DCMTags and select-statements from the file iniFile and
//                stores all tags and statements in a list to which myDcmTagAnker is in the end an anchor.
// Parameters   : iniFile - [in] Name of the file the tags and statements will be read from
// Return Value : none.
{
  FILE *theDat;
  myDcmTag *hdt;
  char hStr[1500];
  int mode;           // indicates if stmt oder Tag is read
  int len;
  myDcmTagAnker = 0;  // this variable is used as an anchor to the list of tags and statements of supported attributes
  mode = 0;           // start reading attribute tags

  // open file
  if( theDat = fopen( iniFile, "r" ) )
  {
    // as long as we can still read one line from the file, do so
    while( fgets( hStr, 1500, theDat ) )
    {
      // if the line which was read is neither empty nor a comment (which starts with a '#')
      if( hStr[0] && hStr[0] != '#' )
      {
        // if we currently deal with reading select statements (mode == 1)
        if( mode )
        {
          // copy the read line into the current list element, get rid of CR/LF and append a '\0'
          strcpy( hdt->selectStmt, hStr );
          len = strlen( hdt->selectStmt );
          if( hdt->selectStmt[len-1] == 10 ) len--;
          if( hdt->selectStmt[len-1] == 13 ) len--;
          hdt->selectStmt[len] = 0;

          // next we want to read an attribute tag again.
          mode = 0;
        }
        // if we currently deal with reading attribute tags (mode == 0)
        else
        {
          // initialize the list's head at the beginning
          if( myDcmTagAnker == 0 )
          {
            myDcmTagAnker = (myDcmTag *) malloc( sizeof( myDcmTag ) );
            hdt = myDcmTagAnker;
            hdt->next = 0;
          }
          // if there is a head, initialize a new list element
          else
          {
            hdt->next = (myDcmTag *) malloc( sizeof( myDcmTag ) );
            hdt = hdt->next;
            hdt->next = 0;
          }
          // copy the read line into the current list element, get rid of CR/LF and append a '\0'
          strcpy( hdt->tag, hStr );
          len = strlen( hdt->tag );
          if( hdt->tag[len-1] == 10 ) len--;
          if( hdt->tag[len-1] == 13 ) len--;
          hdt->tag[len] = 0;

          // next we want to read a select statement again.
          mode = 1;
        }
      }
    }

    // close file
    fclose(theDat);
  }

  // dump information if required
  if( verboseMode && logStream != NULL )
  {
    // we can only dump information if there is some
    if( myDcmTagAnker )
    {
      hdt = myDcmTagAnker;
      while( hdt )
      {
        logStream->lockCout() << endl << "DCMT: " << hdt->tag << endl << "Stmt: " << hdt->selectStmt << endl;
        logStream->unlockCout();
        hdt = hdt->next;
      }
    }
  }
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::ReadOneStatement( const char *readLine, char *theStmt )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function transforms a line which was read from a file into a string. In detail,
//                carriage return and line feed characters (if there are any) are cut off at the end
//                of the later returned string, and a '\0' character is appended at the end.
// Parameters   : readLine - [in] The string as it was read from the file.
//                theStmt  - [inout] The string without CR/LF and with \0.
// Return Value : none.
{
  int len;
	strcpy( theStmt, readLine );
	len = strlen( theStmt );
	if( theStmt[len-1] == 10 ) len--;
	if( theStmt[len-1] == 13 ) len--;
	theStmt[len] = 0;
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::ReadSearchStmt( char *iniFile )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function reads a list of SQL statements from the file iniFile.
// Parameters   : iniFile - [in] Name of the file the SQL statements will be read from
// Return Value : none.
{
  FILE *theDat;
  char hStr[1500];
  int goon, stmtNr;

  //initialize variables
  goon = 1;
  paStmt[0] = 0;
  wlmCountStmt[0] = 0;
  wlmIDStmt[0] = 0;
  wlmIDInsertStmt[0] = 0;
  wlmIDUpdateStmt[0] = 0;

  // open file
  if( theDat = fopen(iniFile, "r") )
  {
		stmtNr = 0;

    // as long as we want to go on and still another line can be read from the file
    while( goon && fgets( hStr, 1500, theDat ) )
    {
      // if line is not empty or not a comment (which begins with '#')
      if( hStr[0] && hStr[0] != '#' )
      {
        // one after the other we want to read (each in a new line) an SQL statement for
        //   case 0 - selecting ids of records of which attributes shall be requested during worklist management requests
        //   case 1 - counting the number of actual database records for a certain record id
        //   case 2 - selecting a wlm record id
        //   case 3 - inserting a wlm record id
        //   case 4 - updating a wlm record id
				switch( stmtNr )
        {
					case 0: ReadOneStatement( hStr, paStmt );
						stmtNr = 1;
						break;
					case 1: ReadOneStatement( hStr, wlmCountStmt );
						stmtNr = 2;
						break;
					case 2: ReadOneStatement( hStr, wlmIDStmt );
						stmtNr = 3;
						break;
					case 3: ReadOneStatement( hStr, wlmIDInsertStmt );
						stmtNr = 4;
						break;
					case 4: ReadOneStatement( hStr, wlmIDUpdateStmt );
						goon = 0;
						break;
				}
      }
    }

    // close file
    fclose( theDat );
  }

  // dump information which was read
  if( verboseMode && logStream != NULL )
  {
    logStream->lockCout() << endl << "PA-Statement: " << paStmt << endl;
    logStream->lockCout() << endl << "Count-Statement: " << wlmCountStmt << endl;
    logStream->lockCout() << endl << "ID-Statement: " << wlmIDStmt << endl;
    logStream->lockCout() << endl << "Insert-Statement: " << wlmIDInsertStmt << endl;
    logStream->lockCout() << endl << "Update-Statement: " << wlmIDUpdateStmt << endl;
    logStream->unlockCout();
  }
}

// ----------------------------------------------------------------------------

int WlmDatabaseInteractionManager::LogonToDatabase( short doWait, long waitTime )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function tries to logon to the database.
// Parameters   : doWait    - [in] if doWait == 1 then (after a failure) try again to connect after waitTime seconds
//                waitTime  - [in] time (in seconds) the application is supposed to wait before a connect is tried again
// Return Value : none.
{
  char msg[200];

  // initialize return value with error indicator
  int returnValue = -1;

  // if connect-information is available
  if( connectStr[0] != 0 )
  {
    // dump some information if required
    if( verboseMode )
      DumpMessage("Trying to connect to database.");

    short tryToConnect = 1;           // indicates if we want to try to connect to the database (1) or not (0)
    long trials = 0;                   // counts how many times a database connection could not be established
    long cummulativeWaitTime;

    // try to connect to Oracle as long as tryToConnect is valid
    while( tryToConnect )
    {
      try
      {
        // connect to Oracle; if this was not successful, an exception will be thrown
        db.rlogon( connectStr );

        // if we get to here the connection was successfully established
        // dump some information if required
        if( verboseMode )
          DumpMessage("Database connection established.");

        // indicate that we dont need to try to connect any more
        tryToConnect = 0;

        // return success
        returnValue = 0;
      }
      catch( otl_exception& p )
      {
        // in case we caught an OTL exception

        // increase counter
        trials++;

        // calculate waitTime (cummulative), we'll give the DB a litte more time to recover in each iteration
        cummulativeWaitTime = trials * waitTime;

        // up to a maximum of 5 Minutes (= 5 x 60 x 1000 = 300000 msec.)
        if( cummulativeWaitTime > 300000 ) cummulativeWaitTime = 300000;

        // dump some information about the error on stderr
        DumpMessage( (const char*)p.msg );      // print out error message
        DumpMessage( p.stm_text );              // print out SQL that caused the error
        DumpMessage( p.var_info );              // print out the variable that caused the error

        // if doWait is valid wait waitTime sec.
        if( doWait )
        {
          // dump some information if required
          if( verboseMode )
          {
            sprintf( msg, "Could not connect to database. Will retry in %d milliseconds.", cummulativeWaitTime );
            DumpMessage( msg );
          }

          // wait a certain number of seconds
          WaitForSingleObject( GetCurrentProcess(), cummulativeWaitTime );
        }
          // otherwise exit with error
        else
        {
          // dump some information if required
          if( verboseMode )
            DumpMessage("Could not connect to database. Will not retry.");

          // exit function with error
          tryToConnect = 0;
        }
      }
    }
  }

  // return result
  return( returnValue );
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::DumpMessage( const char *message )
// Date         : December 20, 2001
// Author       : Thomas Wilkens
// Task         : This function dumps the given information on a stream.
//                Used for dumping information in normal, debug and verbose mode.
// Parameters   : message - [in] The message to dump.
// Return Value : none.
{
  if( logStream != NULL && message != NULL )
  {
    logStream->lockCout();
    logStream->getCout() << message << endl;
    logStream->unlockCout();
  }
}

// ----------------------------------------------------------------------------

OFBool WlmDatabaseInteractionManager::IsCalledApplicationEntityTitleSupported( char *calledApplicationEntityTitle )
// Date         : December 19, 2001
// Author       : Thomas Wilkens
// Task         : Checks if the given called application entity title is supported. If this is the case,
//                OFTrue will be returned, else OFFalse.
// Parameters   : calledApplicationEntityTitle - [in] The application entity title which shall be checked
//                                               for support. Valid pointer expected.
// Return Value : OFTrue  - The called application entity title is supported.
//                OFFalse - The called application entity title is not supported or it is not given.
{
  // Check if calledApplicationEntityTitle is supported
  // At the moment we support everything: make sure SQL-Statements are configured the correct way!
  return( OFTrue );
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::ReplaceWildcards( char *target, const char *source )
// Date         : December 19, 2001
// Author       : Marcel Claus
// Task         : This function copies source into target and replaces all wildcards in DICOM-notation with
//                wildcards in SQL-notation: '*' -> '%' and '?' -> '_'
// Parameters   : target - [inout] The result string.
//                source - [in] The source string.
// Return Value : none.
{
  if( source != NULL )
  {
    strcpy( target, source );
    DoReplace( target, "*", "%" );
    DoReplace( target, "?", "_" );
  }
  else
  {
    strcpy( target, "" );
  }
}

// ----------------------------------------------------------------------------

// Date         : December 19, 1001
// Author       : Marcel Claus
// Task         : Datatype which is used in below function. List of long values.
typedef struct pa
{
  char value[100];
  struct pa *next;
} pa;

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::GetMatchingRecordIDs( DcmDataset *searchMask, char **&matchingRecordIDs, unsigned long &numOfMatchingRecordIDs )
// Date         : December 19, 1001
// Author       : Marcel Claus
// Task         : This function determines the ids of the database records that match the search mask.
//                The result value of this function is an array of ids that can be used to identify the
//                database records which match the given attribute values.
// Parameters   : searchMask             - [in] The search mask.
//                matchingRecordIDs      - [out] Newly created array of database record ids.
//                numOfMatchingRecordIDs - [out] Number of array fields in the above array matchingRecordIDs.
// Return Value : none.
{
  const char **matchingKeyAttrValues = NULL;
  char theStmt[2000], hStr[200];
  char tmpMin[200], tmpMax[200];
  char **ergArray;
  unsigned long i;

  ergArray = 0;
  i = 0;

  // determine the values of the matching key attributes in the search mask.
  DetermineMatchingKeyAttributeValues( searchMask, matchingKeyAttrValues );

  // the first part here will prepare the statement which will be used to select the records ids
  strcpy( theStmt, paStmt );

  // extract modality, aetitle and scheduled performing physician's name values;
  // note that the values for modality and aetitle shall be matched with single
  // value matching, the name value may or may not contain wildard symbols;
  DoReplace( theStmt, ":MODALITY", ( matchingKeyAttrValues[3] != NULL ? matchingKeyAttrValues[3] : "" ) );
  DoReplace( theStmt, ":AETITLE", ( matchingKeyAttrValues[0] != NULL ? matchingKeyAttrValues[0] : "" ) );
  ReplaceWildcards( hStr, matchingKeyAttrValues[4] );
  DoReplace( theStmt, ":PHYSICIAN", hStr );

  // extract date values; note that in this implementation,
  // date values are always subject to range matching
  ExtractRange( matchingKeyAttrValues[1], "to_char(sysdate, 'YYYYMMDD')", "to_char(sysdate, 'YYYYMMDD')", "19000101", "39991231", tmpMin, tmpMax );
  DoReplace( theStmt, ":STARTDATE", tmpMin );
  DoReplace( theStmt, ":STOPDATE", tmpMax );

  // extract time values; note that in this implementation,
  // time values are always subject to range matching
  ExtractRange( matchingKeyAttrValues[2], "to_char(sysdate, 'HH24MISS')", "to_char(sysdate, 'HH24MISS')", "000000", "235959", tmpMin, tmpMax );
  DoReplace( theStmt, ":STARTTIME", tmpMin );
  DoReplace( theStmt, ":STOPTIME", tmpMax );

  // if time/date values do not exist the default values will be taken. In this case
  // it will happen that the function to_char(...) is used. Now the problem is that
  // we do compare a value with the returnvalue of this function and not with a string
  // so we must eliminate the single quotas in the select statement.
  DoReplace( theStmt, "'to_char(sysdate, 'YYYYMMDD')'", "to_char(sysdate, 'YYYYMMDD')" );
  DoReplace( theStmt, "'to_char(sysdate, 'HH24MISS')'", "to_char(sysdate, 'HH24MISS')" );

  // extract patient name value; note that the patient
  // name value may or may not contain wildcard symbols
  ReplaceWildcards( hStr, matchingKeyAttrValues[5] );
  DoReplace( theStmt, ":PATIENT", hStr );

  // extract patient id value; this value shall be matched with single value matching
  DoReplace( theStmt, ":PATIENTID", ( matchingKeyAttrValues[6] != NULL ? matchingKeyAttrValues[6] : "" ) );

  // dump some information if required
  if( verboseMode && logStream != NULL )
  {
    logStream->lockCout() << "getPAs " << theStmt << endl;
    logStream->unlockCout();
  }

  // now the preparation of the select statement is completed

  // specify statement which shall be used to select records ids
  otl_stream oStream( /* buffer size */ 2100, /* SELECT statement */ theStmt, /* connect object */ db );

  // execute select and store values in datastructure pa
  char thePA[100];
  pa *paAnker;
  pa *hpa;
  paAnker = 0;
  while( !oStream.eof() )
  {
    oStream >> thePA;         // fetch next row
    i++;
    if( paAnker == 0 )
    {
      paAnker = (pa *) malloc(sizeof(pa));
      hpa = paAnker;
    }
    else
    {
      hpa->next = (pa *) malloc(sizeof(pa));
      hpa = hpa->next;
    }
    hpa->next = 0;
    strcpy(hpa->value, thePA);
  }

  // convert list into array of OFString (result value)
  if( paAnker && i > 0 )
  {
    hpa = paAnker;
    pa *hpa2;
    unsigned long j = 0;
    ergArray = new char*[i];
    while( hpa && j < i )
    {
      ergArray[j] = new char[ strlen( hpa->value ) + 1 ];
      strcpy( ergArray[j], hpa->value );
      if( verboseMode && logStream != NULL )
      {
        logStream->lockCout() << j << ". PA " << ergArray[j] << endl;
        logStream->unlockCout();
      }
      j++;
      hpa2 = hpa;
      hpa = hpa->next;
      free(hpa2);
    }
    matchingRecordIDs = ergArray;
  }
  numOfMatchingRecordIDs = i;

  // free memory, reset variables
  delete matchingKeyAttrValues;
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::GetDbValue( char *target, char *thePA, const char *tagName )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function gets the value of an attibute for a given PA (recordID).
// Parameters   : target  - [out] If a value is found it will be copied into target.
//                thePA   - [in] PA (recordID) of the record for which a certain value shall be selected.
//                tagName - [in] Identifies the attribute for which a value shall be selected.
// Return Value : none.
{
  myDcmTag *hdt;

  // initialize return value
  target[0] = 0;

  // if attribute and record ID are given
  if( tagName[0] && thePA[0] )
  {
    // and if the list of select statements for supported attributes is given
    if( myDcmTagAnker )
    {
      // try to find current attribute in list of tags and select statements for supported attributes
      int found = 0;
      hdt = myDcmTagAnker;
      while( hdt && found == 0 )
      {
        if( strcmp( hdt->tag, tagName ) == 0 ) // if tag found...
          found = 1;
        else
          hdt = hdt->next;
      }

      // if the given attribute was found, we can use the corresponding select statement to
      // select a value from the database
      if( found )
      {
        char theStmt[1600];
        strcpy( theStmt, hdt->selectStmt );
        DoReplace( theStmt, ":PA", thePA );
        otl_stream oStream( /* buffer size */ 2000, /* SELECT statement */ theStmt, /* connect object */ db );
        oStream >> target;
      }
    }
  }
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::CheckForExistingStudyInstanceUid( char *dcmStudyInstanceUID, char *thePA )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function looks for an existing StudyInstanceUID in the database.
//                If one exists for the dataset with the PA recordID, it will be returned in dcmStudyInstanceUID
//                otherwise dcmStudyInstanceUID[0] will be 0 (empty string).
// Parameters   : dcmStudyInstanceUID - [out] Result study instance UID.
//                thePA               - [in] The id of the record which shall be taken into account.
// Return Value : none.
{
  dcmStudyInstanceUID[0] = 0;

  if( thePA[0] )
  {
    char theStmt[1600];
    strcpy( theStmt, wlmIDStmt );
    DoReplace( theStmt, ":PA", thePA );
    otl_stream oStream( /* buffer size */ 2000, /* SELECT statement */ theStmt, /* connect object */ db );
    oStream >> dcmStudyInstanceUID;

    char msg[500];
    sprintf( msg, "found DCM-WLM-ID: %s for PA: %s", dcmStudyInstanceUID, thePA );
    DumpMessage( msg );
  }
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::StoreStudyInstanceUid( const char *theStudyInstanceUID, char *thePA )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function stores theStudyInstanceUID for a certain dataset with PA recordID in the database.
// Parameters   : theStudyInstanceUID - [in] The study instance uid which shall be stored.
//                thePA               - [in] The record id of the record for which the study instance uid shall be stored.
// Return Value : none.
{
  long anz;

  if( thePA[0] )
  {
    char theStmt[1600];
    strcpy(theStmt, wlmCountStmt);
    DoReplace(theStmt, ":PA", thePA);
    otl_stream oStream( /* buffer size */ 2000, /* SELECT statement */ theStmt, /* connect object */ db );
    oStream >> anz;
    if( anz > 0 )
    {
      strcpy( theStmt, wlmIDUpdateStmt );
    }
    else
    {
      strcpy( theStmt, wlmIDInsertStmt );
    }
    DoReplace( theStmt, ":PA", thePA );
    DoReplace( theStmt, ":DCMWLMID", theStudyInstanceUID );
    otl_cursor::direct_exec( db, theStmt );
    otl_cursor::direct_exec( db, "commit" );
  }
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::GetAttributeValueForMatchingRecord( DcmTagKey tag, char *recordID, char *&value )
// Date         : December 20, 2001
// Author       : Marcel Claus
// Task         : This function determines an attribute value of a certain (matching) record in the database
//                and returns this value in a newly created string to the caller.
// Parameters   : tag      - [in] Attribute tag. Specifies which attribute's value shall be returned.
//                recordID - [in] Identifies the record from which the attribute value shall be retrieved.
//                value    - [out] Pointer to a newly created string that contains the requested value.
// Return Value : none.
{
  char tmpStr[2000];

  // dump some information in verbose mode
  if( verboseMode && logStream != NULL )
  {
    logStream->lockCout() << "GetDbValue PA: " << recordID << ", TagName: " << DcmTag(tag).getTagName() << endl;
    logStream->unlockCout();
  }

  // if we are to select a value for the Study Instance UID attribute
  if( tag == DCM_StudyInstanceUID )
  {
    // Check if this dataset has been sent before,
    // if so the previously sent StudyInstanceUID will be return in tmpStr
		CheckForExistingStudyInstanceUid( tmpStr, recordID );

    // if no StudyInstanceUID has been generated
		if( tmpStr[0] == 0 )
    {
      // generate a new Study Instance UID
      dcmGenerateUniqueIdentifier( tmpStr, uidPrefix );

      // and store it in the database for this dataset
			StoreStudyInstanceUid( tmpStr,  recordID );
		}
  }
  // if we are dealing with some other attribute
  else
  {
    // select a value from the database
    GetDbValue( tmpStr, recordID, DcmTag(tag).getTagName() );
  }

  // copy generated/selected value into out parameter
  value = new char[ strlen(tmpStr) + 1 ];
  strcpy( value, tmpStr );
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::DetermineMatchingKeyAttributeValues( DcmDataset *dataset, const char **&matchingKeyAttrValues )
// Date         : July 12, 2002
// Author       : Thomas Wilkens
// Task         : This function determines the values of the matching key attributes in the given dataset.
// Parameters   : dataset               - [in] Dataset from which the values shall be extracted.
//                matchingKeyAttrValues - [out] Contains in the end the values of the matching key
//                                        attributes in the search mask. Is an array of pointers.
// Return Value : none.
{
  // Initialize array of strings because all currently
  // supported matching key attributes are strings
  matchingKeyAttrValues = new const char*[ NUMBER_OF_SUPPORTED_MATCHING_KEY_ATTRIBUTES ];

  // find matching key attributes in the dataset and remember their values.
  for( unsigned long i=0 ; i<NUMBER_OF_SUPPORTED_MATCHING_KEY_ATTRIBUTES ; i++ )
  {
    // initialize array field
    matchingKeyAttrValues[i] = NULL;

    // determine which matching key attribute we want to find
    DcmTagKey tag;
    switch( i )
    {
      case 0 : tag = DCM_ScheduledStationAETitle           ; break;
      case 1 : tag = DCM_ScheduledProcedureStepStartDate   ; break;
      case 2 : tag = DCM_ScheduledProcedureStepStartTime   ; break;
      case 3 : tag = DCM_Modality                          ; break;
      case 4 : tag = DCM_ScheduledPerformingPhysiciansName ; break;
      case 5 : tag = DCM_PatientsName                      ; break;
      case 6 : tag = DCM_PatientID                         ; break;
      default:                                               break;
    }

    // try to find matching key attribute in the dataset
    dataset->findAndGetString( tag, matchingKeyAttrValues[i], OFTrue );
  }
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wldbim.cc,v $
** Revision 1.3  2002-07-17 13:10:13  wilkens
** Corrected some minor logical errors in the wlmscpdb sources and completely
** updated the wlmscpfs so that it does not use the original wlistctn sources
** any more but standard wlm sources which are now used by all three variants
** of wlmscps.
**
** Revision 1.2  2002/04/18 14:19:49  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.4  2002/01/08 17:46:02  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.3  2002/01/08 17:33:15  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/

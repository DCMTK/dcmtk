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
 *  Update Date:      $Date: 2002-06-10 11:25:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/wlds.cc,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"  // specific configuration for operating system

#include "dicom.h"     // for DIC_NODENAME etc. used in "wltypdef.h"
#include "wltypdef.h"  // for type definitions
#include "oftypes.h"   // for OFBool
#include "dcdatset.h"  // for DcmDataset
#include "dcvrat.h"    // for DcmAttributTag
#include "dcvrlo.h"    // for DcmLongString
#include "dcdict.h"    // for global variable dcmDataDict
#include "dcdeftag.h"  // for DCM_OffendingElement, ...
#include "dcsequen.h"  // for DcmSequenceOfItems

#include "wlds.h"

// ----------------------------------------------------------------------------

WlmDataSource::WlmDataSource()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : none.
// Return Value : none.
  : failOnInvalidQuery( OFTrue ), calledApplicationEntityTitle( NULL ),
    verbose( OFFalse ), identifiers( NULL ), objlist( NULL ), errorElements( NULL ), offendingElements( NULL ),
    errorComment( NULL ), foundUnsupportedOptionalKey( OFFalse ), readLockSetOnDataSource( OFFalse ),
    logStream( NULL )
{
  char msg[200];

  // Make sure data dictionary is loaded.
  if( !dcmDataDict.isDictionaryLoaded() )
  {
    sprintf( msg, "Warning: no data dictionary loaded, check environment variable: %s\n", DCM_DICT_ENVIRONMENT_VARIABLE );
    DumpMessage( msg );
  }

  // Initialize member variables.
  objlist = new DcmList();
  identifiers = new DcmDataset();
  offendingElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
  errorComment = new DcmLongString( DCM_ErrorComment, 0 );
}

// ----------------------------------------------------------------------------

WlmDataSource::~WlmDataSource()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // free memory
  if( calledApplicationEntityTitle != NULL ) delete calledApplicationEntityTitle;
  ClearObjectList();
  delete objlist;
  ClearDataset(identifiers);
  delete identifiers;
  delete offendingElements;
  delete errorElements;
  delete errorComment;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetCalledApplicationEntityTitle( char *value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Sets the member variable that specifies called application entity title.
// Parameters   : value - Value for calledApplicationEntityTitle.
// Return Value : none.
{
  if( value != NULL )
  {
    if( calledApplicationEntityTitle != NULL ) delete calledApplicationEntityTitle;
    calledApplicationEntityTitle = new char[ strlen( value ) + 1 ];
    strcpy( calledApplicationEntityTitle, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetFailOnInvalidQuery( OFBool value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable that determines if a call should fail on an invalid query.
// Parameters   : value - Value for failOnInvalidQuery.
// Return Value : none.
{
  failOnInvalidQuery = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetLogStream( OFConsole *value )
// Date         : March 14, 2002
// Author       : Thomas Wilkens
// Task         : Set member variable.
// Parameters   : value - Value for member variable.
// Return Value : none.
{
  logStream = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetVerbose( OFBool value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable that determines if information shall be dumped at processing time or not.
// Parameters   : value - Value for verbose.
// Return Value : none.
{
  verbose = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::ClearObjectList()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function removes all elements from member variable objlist. This variable
//                will be used to capture all records that match the specified search mask.
// Parameters   : none.
// Return Value : none.
{
  // If the list is defined and there are elements in the list
  if( objlist && objlist->card() )
  {
    // go through the list of elements, starting at the first element
    DcmObject *removeObj = NULL;
    objlist->seek(ELP_first);
    for( unsigned long i=0; i<objlist->card(); i++ )
    {
      // and remove all elements
      removeObj = objlist->remove();
      if( removeObj )
        delete removeObj;
    }
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::ClearDataset( DcmDataset *idents )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function removes all elements from the given DcmDataset object.
// Parameters   : idents - [in] pointer to object which shall be cleared.
// Return Value : none.
{
  // If the given pointer is valid and refers to a non-empty structure
  if( idents && (idents->card()>0) )
  {
    // clear structure
    idents->clear();
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::PutOffendingElements( DcmTagKey &tag )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function inserts the tag of an offending element into the
//                corresponding member variable, unless this tag is already con-
//                tained in this variable.
// Parameters   : tag - [in] The tag that shall be inserted.
// Return Value : none.
{
  DcmTagKey errortag;

  // determine how many offending elements there have been so far
  unsigned long d = offendingElements->getVM();

  // if this is the first one, insert it at position 0
  if( d==0 )
  {
    offendingElements->putTagVal(tag, 0);
  }
  // if this is not the first one, insert it at the end
  // but only if it is not yet contained.
  else
  {
    OFBool tagFound = OFFalse;
    for( unsigned long j=0 ; j<d && !tagFound ; j++ )
    {
      offendingElements->getTagVal( errortag, j );
      if( errortag == tag )
        tagFound = OFTrue;
    }

    if( !tagFound )
      offendingElements->putTagVal( tag, d );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::PutErrorElements( DcmTagKey &tag )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function inserts the tag of an error element into the
//                corresponding member variable, without checking if it is already
//                contained in this variable.
// Parameters   : tag - [in] The tag that shall be inserted.
// Return Value : none.
{
  // insert tag
  errorElements->putTagVal( tag, errorElements->getVM() );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::CheckMatchingKey( DcmElement *elem )
// Date         : March 18, 2002
// Author       : Thomas Wilkens (updated Andrew Hewett's old version)
// Task         : This function checks if the passed matching key's value only uses characters
//                which are part of its data type's character repertoire. Note that at the moment
//                this application supports the following matching key attributes:
//                  - DCM_ScheduledProcedureStepSequence         (0040,0100)  SQ  R  1
//                     > DCM_ScheduledStationAETitle             (0040,0001)  AE  R  1
//                     > DCM_ScheduledProcedureStepStartDate     (0040,0002)  DA  R  1
//                     > DCM_ScheduledProcedureStepStartTime     (0040,0003)  TM  R  1
//                     > DCM_Modality                            (0008,0060)  CS  R  1
//                     > DCM_ScheduledPerformingPhysiciansName   (0040,0006)  PN  R  2
//                  - DCM_PatientsName                           (0010,0010)  PN  R  1
//                  - DCM_PatientID                              (0010,0020)  LO  R  1
//                As a result, the following data types have to be supported in this function:
//                AE, DA, TM, CS, PN and LO. For the correct specification of these datatypes
//                2001 DICOM standard, part 5, section 6.2, table 6.2-1.
// Parameters   : elem - [in] Element which shall be checked.
// Return Value : OFTrue  - The given element's value only uses characters which are part of
//                          the element's data type's character repertoire.
//                OFFalse - The given element's value does not only use characters which are part of
//                          the element's data type's character repertoire.
{
  OFBool ok = OFTrue;
  char *val = NULL;

  switch( elem->ident() )
  {
    case EVR_DA:
      // get string value
      val = GetStringValue( elem );
      // if there is a value and if the value is not a date or a date range, return invalid value
      if( val != NULL && !IsValidDateOrDateRange( val ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid value for an attribute of datatype DA");
        ok = OFFalse;
      }
      break;

    case EVR_TM:
      // get string value
      val = GetStringValue( elem );
      // if there is a value and if the value is not a time or a time range, return invalid value
      if( val != NULL && !IsValidTimeOrTimeRange( val ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid value for an attribute of datatype TM");
        ok = OFFalse;
      }
      break;

    case EVR_CS:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, "*?ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype CS");
        ok = OFFalse;
      }
      break;

    case EVR_AE:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, "*? !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype AE");
        ok = OFFalse;
      }
      break;

    case EVR_PN:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, "*? !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\033" ) )  // ESC=\033
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype PN");
        ok = OFFalse;
      }
      break;

    case EVR_LO:
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( val != NULL && !ContainsOnlyValidCharacters( val, " !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\033\012\014\015" ) )  // ESC=\033, LF=\012, FF=\014, CR=\015
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for datatype LO");
        ok = OFFalse;
      }
      break;

// ### Die Datentypen AS, DS, IS, UI werden in dieser Methode auch geprüft, aber die unterstützten Matching key
// ### attribute haben gar nicht diese Datentypen. Insgesamt müssen diese Prüfungen für den Moment drin bleiben,
// ### weil der wlmscpfs (der alte wlistctn) diese Methode (unsinnigerweise) für alle Attribute in der Suchmaske
// ### aufruft, und nicht nur für Matching Key Attribute. Wenn der wlmscpfs umgeschrieben ist, so daß er konform
// ### zum wlmscpdb ist, können diese 4 Prüfungen weg. Sie machen dann keinen Sinn mehr.
    case EVR_AS:  //### irrelevant, wenn der wlmscpfs an den wlmscpdb angepasst wurde
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( !ContainsOnlyValidCharacters( val, "*.0123456789DWMY " ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for AS");
        ok = OFFalse;
      }
      break;

    case EVR_DS:  //### irrelevant, wenn der wlmscpfs an den wlmscpdb angepasst wurde
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( !ContainsOnlyValidCharacters( val, "0123456789+-Ee. " ) )
      {
        DcmTag tag(elem->getTag());
        PutOffendingElements(tag);
        errorComment->putString("Invalid Character Repertoire for DS");
        ok = OFFalse;
      }
      break;

    case EVR_IS:  //### irrelevant, wenn der wlmscpfs an den wlmscpdb angepasst wurde
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( !ContainsOnlyValidCharacters( val, "0123456789+-. " ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for IS");
        ok = OFFalse;
      }
      break;

    case EVR_UI:  //### irrelevant, wenn der wlmscpfs an den wlmscpdb angepasst wurde
      // get string value
      val = GetStringValue( elem );
      // check if value contains only valid characters
      if( !ContainsOnlyValidCharacters( val, "0123456789.\\" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoire for UI");
        ok = OFFalse;
      }
      break;

    default:
      break;
  }

  return( ok );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidDateOrDateRange( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given value is a valid date or date range.
// Parameters   : value - [in] The value which shall be checked.
// Return Value : OFTrue  - The given value is a valid date or date range.
//                OFFalse - The given value is not a valid date or date range.
{
  char *tmp;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *dateRange = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( dateRange ) == 0 )
  {
    delete dateRange;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( dateRange, "0123456789.-" ) )
  {
    delete dateRange;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidDateRange = OFFalse;

  // Determine if a hyphen occurs in the date range
  char *hyphen = strchr( dateRange, '-' );
  if( hyphen != NULL )
  {
    // determine if two date values are given or not
    if( dateRange[0] == '-' )
    {
      // if the hyphen occurs at the beginning, there is just one date value which has to be checked for validity
      tmp = dateRange;
      tmp++;
      isValidDateRange = IsValidDate( tmp );
    }
    else if( dateRange[ strlen( dateRange ) - 1 ] == '-' )
    {
      // if the hyphen occurs at the end, there is just one date value which has to be checked for validity
      char *newDateRange = new char[ strlen( dateRange ) - 1 + 1 ];
      strncpy( newDateRange, dateRange, strlen( dateRange ) - 1 );
      newDateRange[ strlen( dateRange ) - 1 ] = '\0';
      isValidDateRange = IsValidDate( newDateRange );
      delete newDateRange;
    }
    else
    {
      // in this case the hyphen occurs somewhere in between beginning and end; hence there are two date values
      // which have to be checked for validity. Determine where the hyphen occurs exactly
      int indexval = hyphen - dateRange;

      // determine the first date
      char *date1 = new char[ indexval + 1 ];
      strncpy( date1, dateRange, indexval );
      date1[indexval] = '\0';

      // determine the second date
      tmp = hyphen;
      tmp++;
      char *date2 = new char[ strlen( tmp ) + 1 ];
      strcpy( date2, tmp );

      // check both dates for validity
      if( IsValidDate( date1 ) && IsValidDate( date2 ) )
        isValidDateRange = OFTrue;

      // free memory
      delete date1;
      delete date2;
    }
  }
  else
  {
    // if there is no hyphen, there is just one date value which has to be checked for validity
    isValidDateRange = IsValidDate( dateRange );
  }

  // free memory
  delete dateRange;

  // return result
  return( isValidDateRange );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidDate( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given date value is valid.
//                According to the 2001 DICOM standard, part 5, Table 6.2-1, a date
//                value is either in format "yyyymmdd" or in format "yyyy.mm.dd",
//                so that e.g. "19840822" represents August 22, 1984.
// Parameters   : value - [in] The value which shall be checked.
// Return Value : OFTrue  - Date is valid.
//                OFFalse - Date is not valid.
{
  int year=0, month=0, day=0;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *date = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( date ) == 0 )
  {
    delete date;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( date, "0123456789." ) )
  {
    delete date;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidDate = OFFalse;

  // check which of the two formats applies to the given string
  if( strlen( date ) == 8 )
  {
    // scan given date string
    sscanf( date, "%4d%2d%2d", &year, &month, &day );
    if( year > 0 && month >= 1 && month <= 12 && day >= 1 && day <= 31 )
      isValidDate = OFTrue;
  }
  else if( strlen( date ) == 10 )
  {
    // scan given date string
    sscanf( date, "%4d.%2d.%2d", &year, &month, &day );
    if( year > 0 && month >= 1 && month <= 12 && day >= 1 && day <= 31 )
      isValidDate = OFTrue;
  }

  // free memory
  delete date;

  // return result
  return( isValidDate );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidTimeOrTimeRange( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given value is a valid time or time range.
// Parameters   : timeRange - [in] The value which shall be checked.
// Return Value : OFTrue  - The given value is a valid time or time range.
//                OFFalse - The given value is not a valid time or time range.
{
  char *tmp;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *timeRange = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( timeRange ) == 0 )
  {
    delete timeRange;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( timeRange, "0123456789.:-" ) )
  {
    delete timeRange;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidTimeRange = OFFalse;

  // Determine if a hyphen occurs in the time range
  char *hyphen = strchr( timeRange, '-' );
  if( hyphen != NULL )
  {
    // determine if two time values are given or not
    if( timeRange[0] == '-' )
    {
      // if the hyphen occurs at the beginning, there is just one time value which has to be checked for validity
      tmp = timeRange;
      tmp++;
      isValidTimeRange = IsValidTime( tmp );
    }
    else if( timeRange[ strlen( timeRange ) - 1 ] == '-' )
    {
      // if the hyphen occurs at the end, there is just one time value which has to be checked for validity
      char *newTimeRange = new char[ strlen( timeRange ) - 1 + 1 ];
      strncpy( newTimeRange, timeRange, strlen( timeRange ) - 1 );
      newTimeRange[ strlen( timeRange ) - 1 ] = '\0';
      isValidTimeRange = IsValidTime( newTimeRange );
      delete newTimeRange;
    }
    else
    {
      // in this case the hyphen occurs somewhere in between beginning and end; hence there are two time values
      // which have to be checked for validity. Determine where the hyphen occurs exactly
      int indexval = hyphen - timeRange;

      // determine the first time
      char *time1 = new char[ indexval + 1 ];
      strncpy( time1, timeRange, indexval );
      time1[indexval] = '\0';

      // determine the second time
      tmp = hyphen;
      tmp++;
      char *time2 = new char[ strlen( tmp ) + 1 ];
      strcpy( time2, tmp );

      // check both times for validity
      if( IsValidTime( time1 ) && IsValidTime( time2 ) )
        isValidTimeRange = OFTrue;

      // free memory
      delete time1;
      delete time2;
    }
  }
  else
  {
    // if there is no hyphen, there is just one date value which has to be checked for validity
    isValidTimeRange = IsValidTime( timeRange );
  }

  // free memory
  delete timeRange;

  // return result
  return( isValidTimeRange );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsValidTime( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function checks if the given time value is valid.
//                According to the 2001 DICOM standard, part 5, Table 6.2-1, a time
//                value is either in format "hhmmss.fracxx" or "hh:mm:ss.fracxx" where
//                 - hh represents the hour (0-23)
//                 - mm represents the minutes (0-59)
//                 - ss represents the seconds (0-59)
//                 - fracxx represents the fraction of a second in millionths of seconds (000000-999999)
//                Note that one or more of the components mm, ss, or fracxx may be missing as
//                long as every component to the right of a missing component is also missing.
//                If fracxx is missing, the "." character in front of fracxx is also missing.
// Parameters   : value - [in] The value which shall be checked.
// Return Value : OFTrue  - Time is valid.
//                OFFalse - Time is not valid.
{
  int hour=0, min=0, sec=0, frac=0, fieldsRead=0;

  // check parameter
  if( value == NULL )
    return( OFFalse );

  // create new string without leading or trailing blanks
  char *timeval = DeleteLeadingAndTrailingBlanks( value );

  // check if string is empty now
  if( strlen( timeval ) == 0 )
  {
    delete timeval;
    return( OFFalse );
  }

  // check if only allowed characters occur in the string
  if( !ContainsOnlyValidCharacters( timeval, "0123456789.:" ) )
  {
    delete timeval;
    return( OFFalse );
  }

  // initialize return value
  OFBool isValidTime = OFFalse;

  // check which of the two formats applies to the given string
  char *colon = strchr( timeval, ':' );
  if( colon != NULL )
  {
    // time format is "hh:mm:ss.fracxx"

    // check which components are missing
    if( strlen( timeval ) == 5 )
    {
      // scan given time string "hh:mm"
      fieldsRead = sscanf( timeval, "%2d:%2d", &hour, &min );
      if( fieldsRead == 2 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timeval ) == 8 )
    {
      // scan given time string "hh:mm:ss"
      fieldsRead = sscanf( timeval, "%2d:%2d:%2d", &hour, &min, &sec );
      if( fieldsRead == 3 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timeval ) > 8 && strlen( timeval ) < 16 )
    {
      // scan given time string "hh:mm:ss.fracxx"
      fieldsRead = sscanf( timeval, "%2d:%2d:%2d.%6d", &hour, &min, &sec, &frac );
      if( fieldsRead == 4 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 && frac >= 0 && frac <= 999999 )
        isValidTime = OFTrue;
    }
  }
  else
  {
    // time format is "hhmmss.fracxx"

    // check which components are missing
    if( strlen( timeval ) == 4 )
    {
      // scan given time string "hhmm"
      fieldsRead = sscanf( timeval, "%2d%2d", &hour, &min );
      if( fieldsRead == 2 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timeval ) == 6 )
    {
      // scan given time string "hhmmss"
      fieldsRead = sscanf( timeval, "%2d%2d%2d", &hour, &min, &sec );
      if( fieldsRead == 3 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 )
        isValidTime = OFTrue;
    }
    else if( strlen( timeval ) > 6 && strlen( timeval ) < 14 )
    {
      // scan given time string "hhmmss.fracxx"
      fieldsRead = sscanf( timeval, "%2d%2d%2d.%6d", &hour, &min, &sec, &frac );
      if( fieldsRead == 4 && hour >= 0 && hour <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59 && frac >= 0 && frac <= 999999 )
        isValidTime = OFTrue;
    }
  }

  // free memory
  delete timeval;

  // return result
  return( isValidTime );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::ContainsOnlyValidCharacters( const char *s, const char *charset )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns OFTrue if all the characters of s can be found
//                in the string charset.
// Parameters   : s       - [in] String which shall be checked.
//                charset - [in] Possible character set for s. (valid pointer expected.)
// Return Value : This function returns OFTrue if all the characters of s can be found
//                in the string charset. If s equals NULL, OFTrue will be returned.
{
  OFBool result = OFTrue;

  // check parameter
  if( s == NULL )
  {
    return OFTrue;
  }

  // return OFTrue if all the characters of s can be found in the string charset.
  int s_len = strlen( s );
  int charset_len = strlen( charset );
  for( int i=0 ; i<s_len && result ; i++ )
  {
    OFBool isSetMember = OFFalse;
    for( int j=0 ; !isSetMember && j<charset_len ; j++ )
    {
      if( s[i] == charset[j] )
        isSetMember = OFTrue;
    }

    if( !isSetMember )
      result = OFFalse;
  }

  return( result );
}

// ----------------------------------------------------------------------------

char *WlmDataSource::DeleteLeadingAndTrailingBlanks( const char *value )
// Date         : March 19, 2002
// Author       : Thomas Wilkens
// Task         : This function makes a copy of value without leading and trailing blanks.
// Parameters   : value - [in] The source string.
// Return Value : A copy of the given string without leading and trailing blanks.
{
  char *returnValue;

  unsigned int i;
  OFBool stop = OFFalse;
  for( i=0 ; i<strlen(value) && !stop ; )
  {
    if( value[i] == ' ' )
      value++;
    else
      stop = OFTrue;
  }
  returnValue = new char[ strlen( value ) + 1 ];
  strcpy( returnValue, value );

  if( strlen( returnValue ) > 0 )
  {
    stop = OFFalse;
    for( i=strlen(returnValue)-1 ; i>0 && !stop ; i-- )
    {
      if( returnValue[i] == ' ' )
        returnValue[i] = '\0';
      else
        stop = OFTrue;
    }
  }

  return( returnValue );
}

// ----------------------------------------------------------------------------

char *WlmDataSource::GetStringValue( DcmElement *elem )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns the value of the given DICOM string element (attribute).
//                If the element does not refer to a string attribute, a NULL pointer is returned.
// Parameters   : elem - [in] The DICOM element.
// Return Value : The value of the given DICOM (string) element or NULL.
{
  char *val = NULL;

  if( elem && elem->isaString() )
    elem->getString( val );

  return val;
}

// ----------------------------------------------------------------------------

DcmAttributeTag *WlmDataSource::GetOffendingElements()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns offending elements from member variable offendingElements
//                if there are any.
// Parameters   : none.
// Return Value : Pointer to offending elements or NULL if there are none.
{
  if( offendingElements->getLength() == 0 )
    return NULL;
  else
    return offendingElements;
}

// ----------------------------------------------------------------------------

DcmLongString *WlmDataSource::GetErrorComments()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns error comments from member variable errorComment
//                if there are any.
// Parameters   : none.
// Return Value : Pointer to error comments or NULL if there are none.
{
  if( errorComment->getLength() == 0 )
    return NULL;
  else
    return errorComment;
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSource::CancelFindRequest()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function handles a C-CANCEL Request during the processing of a C-FIND Request.
//                In detail, the member variable objlist is cleared.
// Parameters   : none.
// Return Value : WLM_CANCEL.
{
  // clear objlist
  ClearObjectList();

  // return WLM_CANCEL
  return WLM_CANCEL;
}

// ----------------------------------------------------------------------------

void WlmDataSource::DumpMessage( const char *message )
// Date         : 1995
// Author       : Jörg Riesmeier
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

/*
** CVS Log
** $Log: wlds.cc,v $
** Revision 1.5  2002-06-10 11:25:10  wilkens
** Made some corrections to keep gcc 2.95.3 quiet.
**
** Revision 1.4  2002/04/18 14:20:23  wilkens
** Modified Makefiles. Updated latest changes again. These are the latest
** sources. Added configure file.
**
** Revision 1.3  2002/01/08 17:46:03  joergr
** Reformatted source files (replaced Windows newlines by Unix ones, replaced
** tabulator characters by spaces, etc.)
**
** Revision 1.2  2002/01/08 17:02:55  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:32:46  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/

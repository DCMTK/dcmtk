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
*  Last Update:      $Author: joergr $
*  Update Date:      $Date: 2002-01-08 17:33:15 $
*  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/Attic/wldbim.cc,v $
*  CVS/RCS Revision: $Revision: 1.3 $
*  Status:           $State: Exp $
*
*  CVS/RCS Log at end of file
*
*/

// ----------------------------------------------------------------------------

#include "osconfig.h"
#include "ofconsol.h"
#include "dicom.h"
#include "wltypdef.h"
#include "dctk.h"


#include <stdio.h>
#include <io.h>


#define OTL_ORA7 // Compile OTL 3.1/OCI7
#include <otlv32.h> // include the OTL 3.2 header file

#include "wldbim.h"

static const db_timeout = 5000;

typedef struct pa {
  long value;
  struct pa *next;
} pa;

typedef struct myDcmTag {
  char tag[90];
  char selectStmt[1000];
  struct myDcmTag *next;
} myDcmTag;

myDcmTag *myDcmTagAnker;
otl_connect db; // db-connect object
char paStmt[1500], connectStr[200];

// Extracts from theSource the range, where theSource is in DICOM-Notation with "-"
// the range will be given back in targetMin and targetMax
// the other parameters define default values:
// noValueMin = minvalue to be copied in targetMin when theSource is empty
// noValueMax = maxvalue to be copied in targetMax when theSource is empty
// rangeValueMin = minvalue to be copied in targetMin when theSource has no lower bound
// rangeValueMax = maxvalue to be copied in targetMax when theSource has no upper bound
static void extractRange(const char *theSource,
				  const char *noValueMin, const char *noValueMax, 
				  const char *rangeValueMin, const char *rangeValueMax, 
				  char *targetMin, char *targetMax) {
	if ((theSource != NULL) && (theSource[0] != 0)) {
		char *rangeChar;
		if (rangeChar = strstr(theSource, "-")) {	// if source has range
			if (theSource[0] == '-') {	// if source has no lower bound
				strcpy(targetMin, rangeValueMin);
				strcpy(targetMax, rangeChar + 1);
			} else {	// if source has no upper bound
				if (theSource[strlen(theSource) - 1] == '-') {
					strcpy(targetMin, theSource);
					targetMin[strlen(theSource) - 1] = 0;
					strcpy(targetMax, rangeValueMax);
				} else {	// if source has lower and upper bound
					strcpy(targetMax, rangeChar + 1);
					strcpy(targetMin, theSource);
					rangeChar = strstr(targetMin, "-");
					rangeChar[0] = 0;
				}
			}
		} else {	// if source has no range
			strcpy(targetMin, theSource);
			strcpy(targetMax, theSource);
		}
	} else {	// if source is empty use default values in noValueMin and noValueMax
		strcpy(targetMin, noValueMin);
		strcpy(targetMax, noValueMax);
	}
}

// Replaces in theStr all occurences of oldStr with newStr.
// The replacement is done directly on theStr: NO COPY WILL BE MADE!!!!
static void doReplace(char *theStr, const char *oldStr, const char *newStr) {
int oldLen, newLen;
char *p, *q;
    oldLen = strlen(oldStr);
    newLen = strlen(newStr);
	while (NULL != (p = strstr(theStr, oldStr))) {	// as long as we have something to replace...
	    memmove(q = p+newLen, p+oldLen, strlen(p+oldLen)+1);
		memcpy(p, newStr, newLen);
	}
}

/* 
*  Reads a list of DCMTags and select-statements from the file iniFile
*/
static void readDCMTags (char *iniFile, const OFBool verbose, OFConsole *stream) {
FILE *theDat;
myDcmTag *hdt;
char hStr[1500];
int mode;	// mode: Detrmine if stmt oder Tag is read
int len;
	myDcmTagAnker = 0;
	mode = 0;
	if (theDat = fopen(iniFile, "r")) {
  		while (fgets(hStr, 1500, theDat)) {
			if (hStr[0] && hStr[0] != '#') {
				if (mode) {
					strcpy(hdt->selectStmt, hStr);
					len = strlen(hdt->selectStmt);
					if (hdt->selectStmt[len-1] == 10) len--;
					if (hdt->selectStmt[len-1] == 13) len--;
					hdt->selectStmt[len] = 0;  // eliminate trailing carriage-return
					mode = 0;
				} else {
					if (myDcmTagAnker == 0) {
		  				myDcmTagAnker = (myDcmTag *) malloc(sizeof(myDcmTag));
	 					hdt = myDcmTagAnker;
  						hdt->next = 0;
					} else {
		  				hdt->next = (myDcmTag *) malloc(sizeof(myDcmTag));
  						hdt = hdt->next;
  						hdt->next = 0;
					}
					strcpy(hdt->tag, hStr);
					len = strlen(hdt->tag);
					if (hdt->tag[len-1] == 10) len--;
					if (hdt->tag[len-1] == 13) len--;
					hdt->tag[len] = 0;  // eliminate trailing carriage-return
					mode = 1;
				}
			}
  		}
		fclose(theDat);
	}
	
	if (verbose && (stream != NULL)) {
		if (myDcmTagAnker) {
			hdt = myDcmTagAnker;
			while (hdt) {                
				stream->lockCout() << endl << "DCMT: " << hdt->tag << endl << "Stmt: " << hdt->selectStmt << endl;
                stream->unlockCout();
				hdt = hdt->next;
			}
		}
	}
}

/* 
*  Reads a list of DCMTags and select-statements from the file iniFile
*/
static void readSearchStmt (char *iniFile, const OFBool verbose, OFConsole *stream) {
FILE *theDat;
char hStr[1500];
int goon, len;
	goon = 1;
	paStmt[0] = 0;
	if (theDat = fopen(iniFile, "r")) {
  		while (goon && fgets(hStr, 1500, theDat)) {
			if (hStr[0] && hStr[0] != '#') {
				strcpy(paStmt, hStr);
				len = strlen(paStmt);
				if (paStmt[len-1] == 10) len--;
				if (paStmt[len-1] == 13) len--;
				paStmt[len] = 0;  // eliminate trailing carriage-return
				goon = 0;
			}
  		}
		fclose(theDat);
	}
	
	if (verbose && (stream != NULL)) {
		stream->lockCout() << endl << "PA-Statement: " << paStmt << endl;
        stream->unlockCout();
	}
}

// try to logon to database,
// if do_wait == 1: on failure wait wait_time sec. and try again
static int my_do_logon (short do_wait, long wait_time, const char *connectStr, const OFBool verbose, OFConsole *stream) {
	if (connectStr[0] != 0) {	// if connect-information is available
		
    	if (verbose && (stream != NULL))
        {
		    stream->lockCout() << "Trying to connect to database...";
            stream->unlockCout();
        }
		
		short try_to_connect;
		long trials;
		long cummulative_wait_time;
		trials = 0;
		try_to_connect = 1;

		while (try_to_connect) {
			try {
				db.rlogon(connectStr); // connect to Oracle
				if (verbose && (stream != NULL))
                {
                    stream->lockCout() << " connected..." << endl;
                    stream->unlockCout();
                }
				try_to_connect = 0;
				return 0;	// successfull, return 0
			}
			catch(otl_exception& p){ // intercept OTL exceptions
				trials++;
				cummulative_wait_time = trials * wait_time;	// calculate wait_time (cummulative), we'll give the DB everytime we loop through a bit more time to recover
				if (cummulative_wait_time > 300000) cummulative_wait_time = 300000; // up to a maximum of 5 Minutes (= 5 x 60 x 1000 = 300000 msec.)
				if (verbose && (stream != NULL))
                {
                    stream->lockCout() << " error... will try again in " << cummulative_wait_time << " msec." << endl;
                    stream->unlockCout();
                }
				cerr<<p.msg<<endl; // print out error message
				cerr<<p.stm_text<<endl; // print out SQL that caused the error
				cerr<<p.var_info<<endl; // print out the variable that caused the error
				if (do_wait)	// if not successfull, wait wait_time sec.
					WaitForSingleObject(GetCurrentProcess(), cummulative_wait_time);
				else {			// otherwise exit with status -1
					try_to_connect = 0;
					return -1;
				}
			}
		}
	}
	return -1;	// not successfull, return -1
}

// ----------------------------------------------------------------------------

WlmDatabaseInteractionManager::WlmDatabaseInteractionManager( OFConsole *logStreamv,
			const OFBool verbosev, char *dbDsnv, char *dbUserNamev, char *dbUserPasswordv, const int serialNumberv)
// Date         : December 18, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : logStreamv      - [in] Stream that can be used to dump a message to.
//                verbosev        - [in] Verbose mode
//                dbDsnv          - [in] The data source name of the database that shall be used.
//                dbUserNamev     - [in] The database user name that shall be used for querying information.
//                dbUserPasswordv - [in] The password that belongs to the database user name.
// Return Value : none.
  : logStream( logStreamv ), verboseMode( verbosev ), objectStatus( WLM_STATUS_OK ), opt_serialNumber ( serialNumberv )
{
	useDBConnect = OFTrue;
	myDcmTagAnker = 0;
    sprintf(uidPrefix, "1.2.276.0.7230010.8.%d", serialNumberv);
	if (strcmp( dbDsnv, "text" ) != 0) {
		if (dbDsnv[0])
			sprintf(connectStr, "%s/%s@%s", dbUserNamev, dbUserPasswordv, dbDsnv);
		else
			sprintf(connectStr, "%s/%s", dbUserNamev, dbUserPasswordv);
	
		otl_connect::otl_initialize(); // initialize OCI environment
		if (my_do_logon (0, 10000, connectStr, verboseMode, logStream) == -1) {
			objectStatus = WLM_STATUS_INIT_FAILED;	// No db-connection available, exit with error
			if (logStream != NULL) {
				logStream->lockCerr() << "Unable to connect to database..." << endl;
				logStream->unlockCerr();
			}
		} else {
			readSearchStmt ("searchStmt.txt", verboseMode, logStream);
			readDCMTags ("dcmTagsStmt.txt", verboseMode, logStream);
		}
	} else {
		if (dbUserNamev != NULL) {
			strcpy(connectStr, dbUserNamev);
			useDBConnect = OFFalse;
		} else {
			objectStatus = WLM_STATUS_INIT_FAILED;	// No filename given, exit with error
			if (logStream != NULL) {
				logStream->lockCerr() << "Unable to open PKI-inifile, set username to valid filename..." << endl;
				logStream->unlockCerr();
			}
		}
	}
}

// ----------------------------------------------------------------------------

WlmDatabaseInteractionManager::~WlmDatabaseInteractionManager()
// Date         : December 18, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
/*
  // terminate database connection if there is one
*/
	myDcmTag *hdt;
	hdt = myDcmTagAnker;
	myDcmTag *hdt2;
	while (hdt) {
		hdt2 = hdt;
		hdt = hdt->next;
		free(hdt2);
	}
	db.logoff();
}

// ----------------------------------------------------------------------------

void WlmDatabaseInteractionManager::DumpMessage( const char *message )
// Date         : 1936
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

OFBool WlmDatabaseInteractionManager::IsObjectStatusOk()
// Date         : December 19, 2005
// Author       : Thomas Wilkens
// Task         : Returns OFTrue if the object status is okay.
// Parameters   : none.
// Return Value : OFTrue - The object status is ok.
//                OFFalse - The object status is not ok.
{
  if( objectStatus == WLM_STATUS_OK )
    return OFTrue;
  else
    return OFFalse;
}

// ----------------------------------------------------------------------------

OFBool WlmDatabaseInteractionManager::IsCalledApplicationEntityTitleSupported
    ( char *calledApplicationEntityTitle )
// Date         : December 19, 2001
// Author       : Thomas Wühlkens
// Task         : Checks if the given called application entity title is supported. If this is the case,
//                OFTrue will be returned, else OFFalse.
// Parameters   : calledApplicationEntityTitle - [in] The application entity title which shall be checked
//                for support. Valid pointer expected.
// Return Value : OFTrue  - The called application entity title is supported.
//                OFFalse - The called application entity title is not supported or it is not given.
{
  // Check if calledApplicationEntityTitle is supported
  // At the moment we support everything: make sure SQL-Statements are configured the correct way!
    return OFTrue;
}

// ----------------------------------------------------------------------------

// copies source into target and replaces all wildcards in DICOM-notation with
// wildcards in SQL-notation:
// * -> %
// ? -> _
void replaceWildcards (char *target, const char *source) {
    if (source != NULL)
    {
    	strcpy(target, source);
	    doReplace(target, "*", "%");
	    doReplace(target, "?", "_");
    } else {
        strcpy(target, "");
    }
}

void WlmDatabaseInteractionManager::GetMatchingRecordIDs( char **matchingKeyAttrValues,
			unsigned long numOfMatchingKeyAttrValues, long *&matchingRecordIDs,
			unsigned long &numOfMatchingRecordIDs )
// Date         : December 19, 1001
// Author       : Marcel Claus
// Task         : This function determines the ids of the database records that match the values which
//                are passed in the matchingKeyAttrValues array. In the current implementation, this
//                array shouls always contain 7 array fields that refer to the following attributes:
//                 - matchingKeyAttrValues[0] refers to attribute ScheduledStationAETitle.
//                 - matchingKeyAttrValues[1] refers to attribute ScheduledProcedureStepStartDate.
//                 - matchingKeyAttrValues[2] refers to attribute ScheduledProcedureStepStartTime.
//                 - matchingKeyAttrValues[3] refers to attribute Modality.
//                 - matchingKeyAttrValues[4] refers to attribute ScheduledPerformingPhysiciansName.
//                 - matchingKeyAttrValues[5] refers to attribute PatientsName.
//                 - matchingKeyAttrValues[6] refers to attribute PatientID.
//                The result value of this function is an array of ids that can be used to identify the
//                database records which match the given attribute values.
// Parameters   : matchingKeyAttrValues      - [in] Contains the matching key attribute values.
//                numOfMatchingKeyAttrValues - [in] Number of matching key attribute values.
//                matchingRecordIDs          - [out] Newly created array of database record ids.
//                numOfMatchingRecordIDs     - [out] Number of array fields in the above array matchingRecordIDs.
// Return Value : none.
{
	
	char theStmt[2000], hStr[200];	
	char tmpMin[200], tmpMax[200];
	long *ergArray;
	long i;

	ergArray = 0;
	i = 0;

	if (useDBConnect) {
		strcpy(theStmt, paStmt);		// prepare statement

		replaceWildcards(hStr, matchingKeyAttrValues[3]);
		doReplace(theStmt, ":MODALITY", hStr);
		replaceWildcards(hStr, matchingKeyAttrValues[0]);
		doReplace(theStmt, ":AETITLE", hStr);
		replaceWildcards(hStr, matchingKeyAttrValues[4]);
		doReplace(theStmt, ":PHYSICIAN", hStr);

		extractRange(matchingKeyAttrValues[1],			// extract date values
					 "to_char(sysdate, 'YYYYMMDD')", "to_char(sysdate, 'YYYYMMDD')",
					 "19000101", "39991231", 
					 tmpMin, tmpMax);
		doReplace(theStmt, ":STARTDATE", tmpMin);
		doReplace(theStmt, ":STOPDATE", tmpMax);

		extractRange(matchingKeyAttrValues[2],			// extract time values
					 "to_char(sysdate, 'HH24MISS')", "to_char(sysdate, 'HH24MISS')",
					 "000000", "235959", 
					 tmpMin, tmpMax);
		doReplace(theStmt, ":STARTTIME", tmpMin);
		doReplace(theStmt, ":STOPTIME", tmpMax);

		extractRange(matchingKeyAttrValues[5],			// extract patgient name values
					 " ", "}",
					 " ", "}", 
					 tmpMin, tmpMax);
		doReplace(theStmt, ":PATIENT_MIN", tmpMin);
		doReplace(theStmt, ":PATIENT_MAX", tmpMax);

		extractRange(matchingKeyAttrValues[6],			// extract patgient name values
					 "0", "999999999999",
					 "0", "999999999999", 
					 tmpMin, tmpMax);
		doReplace(theStmt, ":PATIENTID_MIN", tmpMin);
		doReplace(theStmt, ":PATIENTID_MAX", tmpMax);

		if (verboseMode && (logStream != NULL))
		{
			logStream->lockCout() << "getPAs " << theStmt << endl;
			logStream->unlockCout();
		}
		otl_stream oStream(2100, // buffer size
						   theStmt, // SELECT statement
						   db // connect object
						  );

		// execute select and store values in datastructure pa
		long thePA;
		pa *paAnker;
		pa *hpa;
		paAnker = 0;
		while (!oStream.eof()) {
			oStream>>thePA;			// fetch next row
			i++;
			if (paAnker == 0) {
				paAnker = (pa *) malloc(sizeof(pa));
	 			hpa = paAnker;
			} else {
				hpa->next = (pa *) malloc(sizeof(pa));
  				hpa = hpa->next;
			}
  			hpa->next = 0;
			hpa->value = thePA;
		}

		// Thomas will unbedingt ein array of long, also müssen wir unsere schöne verkettete Liste darin umwandeln
		if (paAnker && i > 0) {	// if data found
			hpa = paAnker;
			pa *hpa2;
			long j = 0;
			ergArray = (long *) malloc(i*sizeof(long));
			while (hpa && j < i) {
				ergArray[j] = hpa->value;
        		if (verboseMode && (logStream != NULL))
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

	} else {	// if PKI-inifile should be used

		numOfMatchingRecordIDs = 1;
		matchingRecordIDs = (long *) malloc(sizeof(long));
		matchingRecordIDs[0] = -1;
        if (verboseMode && (logStream != NULL)) {
			logStream->lockCout() << "using PKI-file, only one PA " << matchingRecordIDs[0] << endl;
			logStream->unlockCout();
		}

	}
}

// ----------------------------------------------------------------------------

// get the value of one DCMTag depending on  PA
// target: if a value is found it will be copied in target
// thePA: value of PA
// tagName: name of the DCMTag
static void get1Value (char *target, long numPA, const char *tagName) {
	myDcmTag *hdt;
	char thePA[20];
	
	sprintf(thePA, "%d", numPA);
	target[0] = 0;
	if (tagName[0] && thePA[0]) {
		if (myDcmTagAnker) {
			int found = 0;
			hdt = myDcmTagAnker;
			while (hdt && found == 0) {
				if ( strcmp( hdt->tag, tagName ) == 0 )	// if tag found...
					found = 1;
				else
					hdt = hdt->next;
			}
			if (found) {
				char theStmt[1600];
				strcpy(theStmt, hdt->selectStmt);
				doReplace(theStmt, ":PA", thePA);
				otl_stream oStream(2000, // buffer size
								   theStmt, // SELECT statement
								   db // connect object
								  );
				oStream>>target;
			}
		}
	}
}

void extractPKIvalues (char *name, char *value, const char *source) {
	int i, status, nameIndex, valueIndex;
	status = 0;
	i = 0;
	name[0] = 0;
	nameIndex = 0;
	value[0] = 0;
	valueIndex = 0;
	while (source[i]) {
		if ( (source[i] == '<') && ((status == 1) || (status == 0)) ) {
			status = 2;
		} else {
			if ((source[i] == ' ') && (status == 0)) {
				status = 1;
			} else {
				if ((source[i] == '>') && (status == 2)) {
					status = 3;
				} else {

					switch (status) {
						case 0: name[nameIndex] = source[i]; nameIndex++; break;
						case 2: value[valueIndex] = source[i]; valueIndex++; break;
					}

				}
			}
		}
		i++;
	}
	value[valueIndex] = 0;
	name[nameIndex] = 0;
}

void getFileValue (char *target, const char *pkiUpperName, const char *pkiLowerName, const char *fileName) {
FILE *theDat;
char hStr[500], pkiTagName[100], pkiTagValue[100];
short goon;
	if (theDat = fopen(fileName, "r")) {
		goon = 1;
  		while (fgets(hStr, 500, theDat) && goon) {
			if (hStr[0] && hStr[0] != '#') {
				extractPKIvalues(pkiTagName, pkiTagValue, hStr);
				if ((strcmp(pkiTagName, pkiUpperName) == 0) || (strcmp(pkiTagName, pkiLowerName) == 0)) {
					strcpy(target, pkiTagValue);
					goon = 0;
				}
			}
  		}
		fclose(theDat);
	}
}


/* read corresponding value from pki-inifile "fileName" for dcmTag with name
 * "tagName" and return it in "target". If nothing is found target[0] will be 0
 * (empty string).
 */
void getPkiValue(char *target, const char *dcmName, const char *fileName) {
	char valueStr1[200], valueStr2[200];
	target[0] = 0;
	if (strcmp(dcmName, "PatientID") == 0) {
		getFileValue (target, "PATIENT_ID", "patient_id", fileName);
		return;
	}
	if (strcmp(dcmName, "PatientsName") == 0) {
		getFileValue (valueStr1, "PATIENT_LAST_NAME", "patient_last_name", fileName);
		getFileValue (valueStr2, "PATIENT_FORE_NAME", "patient_fore_name", fileName);
		/* sprintf(target, "%s^%s", valueStr1, valueStr2); */ // this ist standard conform and will be supported sometime by the new software from philips
		sprintf(target, "%s, %s", valueStr1, valueStr2);
		return;
	}
	if (strcmp(dcmName, "AccessionNumber") == 0) {
		// if AccessionNumber is requested the value in PATIENT_UID is returned
		// this is a bug in the format of the PKI-file!
		getFileValue (target, "PATIENT_UID", "patient_uid", fileName);
		//getFileValue (target, "ACCESSION_NUMBER", "accession_number", fileName);
		return;
	}
	if (strcmp(dcmName, "ScheduledProcedureStepID") == 0) {
		getFileValue (target, "ACCESSION_NUMBER", "accession_number", fileName);
		return;
	}
	if (strcmp(dcmName, "RequestedProcedureID") == 0) {
		getFileValue (target, "PATIENT_UID", "patient_uid", fileName);
		return;
	}
	if (strcmp(dcmName, "PatientsBirthDate") == 0) {
		getFileValue (target, "PATIENT_BIRTHDAY", "patient_birthday", fileName);
		return;
	}
	if (strcmp(dcmName, "ScheduledProcedureStepStartTime") == 0) {
		getFileValue (target, "EXAM_TIME", "exam_time", fileName);
		return;
	}
	if (strcmp(dcmName, "PatientsSex") == 0) {
		getFileValue (valueStr1, "PATIENT_GENDER", "patient_gender", fileName);
		if ((valueStr1[0] == 'F') || (valueStr1[0] == 'f') || (valueStr1[0] == 'W') || (valueStr1[0] == 'w')) {
			target[0] = 'F';
		} else {
			target[0] = 'M';
		}
		target[1] = 0;
		return;
	}
	if (strcmp(dcmName, "ScheduledProcedureStepStartDate") == 0) {
		getFileValue (target, "EXAM_DATE", "exam_date", fileName);
		return;
	}
	if (strcmp(dcmName, "ScheduledPerformingPhysiciansName") == 0) {
		getFileValue (target, "PHYSICIAN_CODE", "physician_code", fileName);
		return;
	}
}


void WlmDatabaseInteractionManager::GetAttributeValueForMatchingRecord( DcmTagKey tag, long recordID, char *&value )
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
    if (verboseMode && (logStream != NULL)) {
        logStream->lockCout() << "get1Value PA: " << recordID << ", TagName: " << DcmTag(tag).getTagName() << endl;
        logStream->unlockCout();
    }

    if (tag == DCM_StudyInstanceUID) {
        dcmGenerateUniqueIdentifier(tmpStr, uidPrefix);
    } else {
    	if (useDBConnect) {
	        get1Value (tmpStr, recordID, DcmTag(tag).getTagName());
	    } else {
		    getPkiValue (tmpStr, DcmTag(tag).getTagName(), connectStr);
        }
    }
    value = new char[strlen(tmpStr) + 1];
    strcpy(value, tmpStr);

}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wldbim.cc,v $
** Revision 1.3  2002-01-08 17:33:15  joergr
** Reworked database support after trials at the hospital (modfied by MC/JR on
** 2002-01-08).
**
**
*/

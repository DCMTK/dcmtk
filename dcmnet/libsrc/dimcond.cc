/*
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**  
**  This software and supporting documentation were
**  developed by 
**  
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**    
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ossietzky Universitaet Oldenburg 
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**    
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**  
**  for CEN/TC251/WG4 as a contribution to the Radiological 
**  Society of North America (RSNA) 1993 Digital Imaging and 
**  Communications in Medicine (DICOM) Demonstration.
**  
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING 
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR 
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER 
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE 
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE   
**  IS WITH THE USER. 
**  
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice. 
** 
*/
/*
** 
** Author: Andrew Hewett		Created: 09-06-93 
** 
** Module: dimcond 
** 
** Purpose: 	
** This file contains routines for generating DIMSE condition messages.
** 
** Module Prefix: DIMSE_ 
** 
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-06-29 12:14:32 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/Attic/dimcond.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include "dimse.h"
#include "dimcond.h"


typedef struct dimcond_vector {
    CONDITION cond;
    const char *message;
} DIMCOND_VECTOR;

static DIMCOND_VECTOR messageVector[] = {
	{DIMSE_NORMAL, "Normal return from DIMSE routine"},
	{DIMSE_NETWORKERROR, "DIMSE Network layer error"},
	{DIMSE_NULLKEY, "DIMSE Caller passed in a NULL key"},
	{DIMSE_ILLEGALASSOCIATION, "DIMSE Caller passed in an illegal association"},
	{DIMSE_MALLOCERROR, "DIMSE Failed to malloc a buffer"},
	{DIMSE_PEERREQUESTEDRELEASE, "DIMSE Peer requested release"},
	{DIMSE_READPDVFAILED, "DIMSE Read PDV failed"},
	{DIMSE_BUILDFAILED, "DIMSE Failed to build an DICOM object"},
	{DIMSE_UNEXPECTEDPDVTYPE, "DIMSE Unexpected PDV type"},
	{DIMSE_PEERABORTEDASSOCIATION, "DIMSE Peer aborted association"},
	{DIMSE_NOVALIDPRESENTATIONCONTEXTID, "DIMSE No valid presentation Context ID (send)"},
	{DIMSE_BADMESSAGE, "DIMSE Badly formed message"},
	{DIMSE_BADDATA, "DIMSE Inappropriate data for message"},
	{DIMSE_PARSEFAILED, "DIMSE Failed to parse received message"},
	{DIMSE_RECEIVEFAILED, "DIMSE Failed to receive message"},
	{DIMSE_OBJECTTOOLARGE, "DIMSE Object too large"},
	{DIMSE_INVALIDPRESENTATIONCONTEXTID, "DIMSE Invalid presentation Context ID"},
	{DIMSE_BADCOMMANDTYPE, "DIMSE Bad command type"},
	{DIMSE_SENDFAILED, "DIMSE Failed to send message"},
	{DIMSE_UNSUPPORTEDTRANSFERSYNTAX, "DIMSE Unsupported transfer syntax: %s"},
	{DIMSE_OUTOFRESOURCES, "DIMSE Out of resources"},
	{DIMSE_NODATAAVAILABLE, "DIMSE No data available (timout in non-blocking mode)"},
	{DIMSE_UNKNOWNCMDELEMENT, "DIMSE Unknown command element encountered"},
	{DIMSE_UNEXPECTEDRESPONSE, "DIMSE Unexpected response received"},
	{DIMSE_UNEXPECTEDREQUEST, "DIMSE Unexpected request received"},
	{0, NULL}
};

const char *
DIMSE_Message(CONDITION condition)
{
    int l_index;

    for (l_index = 0; messageVector[l_index].message != NULL; l_index++)
	if (condition == messageVector[l_index].cond)
	    return messageVector[l_index].message;

    return NULL;
}

/*
** CVS Log
** $Log: dimcond.cc,v $
** Revision 1.3  1998-06-29 12:14:32  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.2  1997/02/06 12:20:33  hewett
** Renamed vector to be dimcond_vector to avoid clash with other vector types.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/

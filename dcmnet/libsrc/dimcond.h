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
** Update Date:		$Date: 1998-06-29 12:14:29 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/Attic/dimcond.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
**
*/


#ifndef DIMCOND_H
#define DIMCOND_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dicom.h"
#include "cond.h"

const char *
DIMSE_Message(CONDITION condition);


#endif

/*
** CVS Log
** $Log: dimcond.h,v $
** Revision 1.2  1998-06-29 12:14:29  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/

/*
          Copyright (C) 1993, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993 DICOM Central Test Node project for, and
          under contract with, the Radiological Society of North America.

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER RSNA NOR
          WASHINGTON UNIVERSITY MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          jointly owned by RSNA and Washington University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include the three paragraphs of
          the copyright notice.
*/
/*			condition.c
**
**  This file contains functions implementing a simple error facility.
**  It was first written by Stephen Moore (smm@wuerl.wustl.edu) to
**  support PACS development at the Mallinckrodt Institute of Radiology.
**  The function names have been modified to have a slightly more
**  generic name, but the basic model and concepts are the same.
**
**  The condition package maintains a stack of <condition, message>
**  pairs that callers can push or pop.  When a routine returns an
**  abnormal value, it should push a condition onto the stack so that
**  the caller can examine the value at a later time.  Nested routines
**  may push a number of conditions onto the stack providing more
**  detailed information about why a routine did not return normally.
**
**  The stack is maintained as a simple stack array.  If it overflows,
**  we dump the stack to stdout and reset it.
**
**  Public routines:
**  CONDITION COND_PushCondition(CONDITION code, char *controlstring, var args)
**	Push a condition onto the stack.  CODE is a "defined" CONDITION
**	value that should be defined for a package or could be internally
**	defined for an application.  It will consist of fields for
**	facility, value and severity.  CONTROLSTRING is a control string
**	used by the standard C run time library "printf" for formatting
**	an output string.  The caller passes a variable number of arguments
**	after CONTROLSTRING to match the conversion specifications in
**	CONTROLSTRING.  This function returns the CONDITION passed to it.
**  CONDITION COND_ExtractConditions(int *callback())
**	Step through the stack and extract the conditions and messages on
**	the stack.  For each condition, call the user's CALLBACK routine
**	with arguments (CONDITION, char *) so that the user can examine
**	the CONDITION value and the ASCIZ message.  The callback routine
**	returns 0 if no more conditions are to be returned and a nonzero
**	value otherwise.
**  CONDITION COND_TopCondition(CONDITION *code, int *text, int maxlength)
**	Examine the stack and return the top condition and corresponding
**	message in the users's CODE and TEXT areas.  The caller specifies
**	the maximum length of the text area with MAXLENGTH.   The function
**	also uses the top condition as its return value.  If the stack is
**	empty, the function returns COND_NORMAL;
**  CONDITION COND_PopCondition(int clearstack)
**	Pop one or all values off the stack.  This function returns the
**	top condition from the stack.  If CLEARSTACK is zero, the top
**	condition is removed from the stack.  If CLEARSTACK is nonzero,
**	the entire stack is cleared.
**
**  AUTHORS:
**
**      Stephen M. Moore (smm@wuerl.wustl.edu)
**	Electronic Radiology Laboratory
**	Mallinckrodt Institute of Radiology
**	Washington University School of Medicine
**
**  CREATION DATE:  January 30, 1991
**
**
**  MODIFICATION HISTORY:
**	smm	23-May-1992
**	Change functions names and drop some functions to be used
**	with the DICOM project.
**
**  hewett 8-June-1993
**  Added function to dump the contents of the condition stack (needed)
**  for new release of DUL code from MIR.
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-06-29 12:14:32 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/cond.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

/*
**
**  INCLUDE FILES
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#include "dicom.h"
#include "cond.h"

typedef struct {
  CONDITION statusCode;
  char statusText[256];
  } EDB;

#define MAXEDB  100

static EDB EDBStack[MAXEDB];
static int stackPtr = -1;
static void (*ErrorCallback)(CONDITION cond, char *str) = NULL;
static void dumpstack();


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      COND_PushCondition
**	This routine is used to log a condition on the stack.  The user
**	passes an error code (currently uninterpreted), a format string
**	and optional format arguments.  We use the vsprintf routine to
**	interpret the user's format string and arguments, and we place the
**	error condition and resultant string on the stack.
**
**  FORMAL PARAMETERS:
**
**      code:
**          The condition code of the error/warning.
**       
**      controlString:
**          format string for vsprintf statement
**       
**      [varargs]:
**          variable arguments to be used with controlString
**       
**  RETURN VALUE:
**
**      code (as passed in by the user)
**
**  SIDE EFFECTS:
**
**      Places a new entry on the stack.  If the stack
**	fills up, drop the last condition.
**	Calls a user-established callback just before return.
**
*/
CONDITION COND_PushCondition(CONDITION cond, const char *controlString, ...)
{
  va_list
    args;
  char
    buffer[1024];

    va_start(args, controlString);
    if (controlString == NULL)
	controlString = "NULL Control string passedto PushCondition";
    (void)vsprintf(buffer, controlString, args);
    va_end(args);

    stackPtr++;
    EDBStack[stackPtr].statusCode = cond;
    buffer[256] = '\0';
    (void)strcpy(EDBStack[stackPtr].statusText, buffer);
    if (ErrorCallback != NULL)
	ErrorCallback(EDBStack[stackPtr].statusCode,
			EDBStack[stackPtr].statusText);

    if (stackPtr == MAXEDB - 1) {
	dumpstack();
	fprintf(stderr, "CONDITION Stack overflow\n");
	stackPtr = 0;
    }
    return cond;

}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**  COND_ExtractConditions
**	This routine walks through the stack and passes the condition
**	codes and text back to the user.  The caller supplies a
**	callback routine.  We start at the top of the stack and
**	call the user's callback for each message on the stack.  The
**	user can terminate the process at any time by returning
**	a zero from his callback.
**
**  FORMAL PARAMETERS:
**
**      callback:
**          User routine to call for each message on the stack.
**       
**  RETURN VALUE:
**
**      1
**
**  SIDE EFFECTS:
**
**
**  DESIGN:
**
**      None
**--
*/

CONDITION COND_ExtractConditions(OFBool (*callback)(CONDITION cond, char *str)) 
{
  int
    l_index,
    returnflag;

    for (l_index = stackPtr, returnflag = 1; l_index >= 0 && returnflag != 0;
	l_index--) {
	returnflag = callback(EDBStack[l_index].statusCode,
				EDBStack[l_index].statusText);
    }
    return COND_NORMAL;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      COND_TopCondition
**	This routine is used to look at the top condition message on
**	the stack.  The user passes pointers to areas to place
**	the error message.  The function also returns the code
**	for the top error message.  If the stack is empty, the
**	success code (0) is returned.
**
**  FORMAL PARAMETERS:
**
**      code:
**          Pointer to the user's area to hold the error code
**       
**      text
**          Pointer to the user's area to hold the error text
**       
**      maxlength
**          Maximum buffer length in the user's text area
**       
**  RETURN VALUE:
**
**      top error code on the stack
**
**  SIDE EFFECTS:
**
**
*/

CONDITION COND_TopCondition(CONDITION *code,char *text,unsigned long maxlength)
{
    if (stackPtr >= 0) {
	*code = EDBStack[stackPtr].statusCode;
	(void)strncpy(text, EDBStack[stackPtr].statusText, size_t(maxlength-1));
	text[maxlength - 1] = '\0';
	return EDBStack[stackPtr].statusCode;
    } else {
	*code = COND_NORMAL;
	*text = '\0';
	return COND_NORMAL;
    }
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      COND_PopCondition
**	This routine pops one or all conditions off the stack.
**	The user passes a flag which indicates the operation.
**	After the clear, the current top error code is returned.
**	If the stack is empty at this point, the success code (0)
**	is returned.
**
**  FORMAL PARAMETERS:
**
**      clearstack:
**          Flag which indicates if the entire stack is to be cleared.
**		0	    Just pop the top error
**		non zero    Clear the entire stack
**       
**  RETURN VALUE:
**
**      The new top error code.  0 if the stack is empty
**
**  SIDE EFFECTS:
**
**
*/

CONDITION COND_PopCondition(OFBool clearstack)
{
  CONDITION
    value;

    if (stackPtr >= 0)
	value = EDBStack[stackPtr].statusCode;
    else
	value = COND_NORMAL;

    if (clearstack) {
	stackPtr = -1;
    } else if (stackPtr <= 0) {
	stackPtr = -1;
    } else {
	stackPtr--;
    }
    return value;
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      COND_EstablishCallback
**	Establishes a callback routine to be called whenever a
**	new condition is placed on the stack.  There is no stack
**	mechanism for these callbacks, so each new callback routine
**	completely supersedes the previous one.
**
**  FORMAL PARAMETERS:
**
**      callback:
**          The new callback routine.  If NULL, this will
**	    disable callbacks.
**       
**  RETURN VALUE:
**
**      0
**
**  SIDE EFFECTS:
**
**
*/

CONDITION COND_EstablishCallback(void (* callback)(CONDITION cond, char *str))
{
    ErrorCallback = callback;
    return COND_NORMAL;
}


static void dumpstack()
{
  int
    l_index;

    for (l_index = 0; l_index <= stackPtr; l_index++)
	fprintf(stderr, "%8lx %s\n", EDBStack[l_index].statusCode,
				    EDBStack[l_index].statusText);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      COND_DumpConditions
**	
**	Dumps the conditions onto stderr.
**	
**  FORMAL PARAMETERS:
**
**		void
**       
**  RETURN VALUE:
**
**      void
**
**  SIDE EFFECTS:
**
**		none
**
*/

void COND_DumpConditions(void)
{
	dumpstack();
}

/*
** CVS Log
** $Log: cond.cc,v $
** Revision 1.4  1998-06-29 12:14:32  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.3  1997/07/24 13:10:57  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.2  1997/07/21 08:47:15  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/

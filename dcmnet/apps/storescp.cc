/*
**
**  Copyright (C) 1996, OFFIS
**
**  This software and supporting documentation were developed by
**
**    Kuratorium OFFIS e.V.
**    Forschungsbereich 2: Kommunikationssysteme
**    Escherweg 2
**    D-26121 Oldenburg, Germany
**
**  for CEN/TC251/WG4 as a contribution to the Computer Assisted Radiology
**  (CAR) 1996 DICOM Demonstration.
**
**  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
**  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
**  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
**  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
**
**  Copyright of the software  and  supporting  documentation  is,  unless
**  otherwise stated, owned by OFFIS, and free access is hereby granted as
**  a license to  use  this  software,  copy  this  software  and  prepare
**  derivative works based upon this software.  However, any  distribution
**  of this software source code or supporting documentation or derivative
**  works  (source code and  supporting documentation)  must  include  the
**  three paragraphs of this copyright notice.
**
*/

/*
** Simple Service Class Provider Example Program
**
** Author: Andrew Hewett
**		Kuratorium OFFIS e.V., Oldenburg, Germany
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1997-05-27 15:44:53 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/storescp.cc,v $
** CVS/RCS Revision:	$Revision: 1.11 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dimse.h"
#include "diutil.h"
#include "dcfilefo.h"
#include "dcdebug.h"
#include "dcuid.h"
#include "dcdict.h"
#include "cmdlnarg.h"

#define PATHSEPARATOR PATH_SEPARATOR	/* via osconfig.h" */
#define APPLICATIONTITLE "STORESCP"     /* our application entity title */

static void usage()
{
  fprintf(stderr, 
"usage: storescp [options] port\n"
"parameters are:\n"
"    port      tcp/ip port number to listen on\n"
"options are:\n"
"  network options:\n"
"      -r      refuse association\n"
"      +R      reject association if implementation class UID is missing\n"
"      -s n    sleep n seconds after store (default: 0)\n"
"      +s n    sleep n seconds during store (default: 0)\n"
"      -b n    set max receive pdu to n bytes (%lu-%lu, default: %lu)\n"
"      -t=     prefer local byte ordering, explicit transfer syntaxes (default)\n"
"      -te     prefer little-endian explicit transfer syntax\n"
"      -tb     prefer big-endian explicit transfer syntax\n"
"      -ti     use little-endian implicit transfer syntax only\n"
"  output options:\n"
"    DICOM fileformat (Sup. 1) support:\n"
"      -F      write file without metaheader\n"
"      +F      write file with metaheader (default)\n"
"    output transfer syntax (not with +B):\n"
"      +t=     write with same transfer syntax as input (default)\n"
"      +te     write with little-endian explicit transfer syntax\n"
"      +tb     write with big-endian explicit transfer syntax\n"
"      +ti     write with little-endian implicit transfer syntax\n"
"    group length encoding (not with +B):\n"
"      +g      write with group lengths\n"
"      +g=     recalculate group lengths (default)\n"
"      -g      write without group lengths\n"
"    length encoding in sequences and items (not with +B):\n"
"      +e      write with explicit lengths (default)\n"
"      -e      write with undefined lengths\n"
"    padding (not with +B or -F):\n"
"      -p      no padding (default)\n"
"      +p n m  pad file to x*n bytes and items to y*m bytes\n"
"    unknown VR (not with +B):\n"
"      -u      disable generation of unknown VR (UN)\n"
"      +u      enable generation of unknown VR (UN) (default)\n"
"  other options:\n"
"      -h      print this usage string\n"
"      +V      verbose mode, print actions\n"
"      +d      debug mode\n"
"      +B      bypass dcmdata, save C-STORE data directly to file\n"
"      -i      ignore store data, receive but do not save to disk\n",
  (unsigned long)ASC_MINIMUMPDUSIZE, 
  (unsigned long)ASC_MAXIMUMPDUSIZE, 
  (unsigned long)ASC_DEFAULTMAXPDU);
}

int extract_unsigned_long(unsigned long& result, int argc, char *argv[], int index)
{
  unsigned long l;
  
  if (index >= argc ) return 0;
  if (1 == sscanf(argv[index], "%lu", &l))
  {
    result = l;
    return 1;
  } else return 0;
}

static CONDITION processCommands(T_ASC_Association *assoc);
static CONDITION acceptAssociation(T_ASC_Network *net);
static CONDITION echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID);
static CONDITION storeSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID);
	
unsigned long     opt_port = 0;
int               opt_refuseAssociation = 0;
int               opt_rejectWithoutImplementationUID = 0;
unsigned long     opt_sleepAfter = 0;
unsigned long     opt_sleepDuring = 0;
unsigned long     opt_maxPDU = ASC_DEFAULTMAXPDU;
int               opt_useMetaheader = 1;
E_TransferSyntax  opt_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;
E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;
E_EncodingType    opt_sequenceType = EET_ExplicitLength;
E_PaddingEncoding opt_paddingType = EPD_withoutPadding;
char *            opt_paddingUsed = NULL; /* checked by -F */
unsigned long     opt_padlen = 0;
unsigned long     opt_subPadlen = 0; 
int               opt_helpString = 0;
int               opt_verbose = 0;
int               opt_debug = 0;
int               opt_bypass = 0;
char *            opt_bypassDisabled = NULL; /* option which disallows use of +B */
int               opt_ignore = 0;


int main(int argc, char *argv[])
{
    CONDITION cond;
    T_ASC_Network *net;
    int i;

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.0 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 0 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

  prepareCmdLineArgs(argc, argv, "storescp");

  if (argc < 2)
  {
    usage();
    return 1;
  }
  
  dcmEnableUnknownVRGeneration = TRUE;
  char *arg = NULL;
  for (i=1; i<argc; i++)
  {
    arg = argv[i];
    if (arg[0] == '-' || arg[0] == '+')
    {
      if (strlen(arg) < 2) 
      {
        fprintf(stderr, "unknown argument: %s\n", arg);
        usage();
        return 1;
      }
      switch (arg[1])
      {

         case 'r': /* -r */
           if ((arg[0]=='-')&&(arg[2]=='\0'))
           {
             opt_refuseAssociation = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'R': /* +R */
           if ((arg[0]=='+')&&(arg[2]=='\0'))
           {
             opt_rejectWithoutImplementationUID = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'h': /* -h */
           if ((arg[0]=='-')&&(arg[2]=='\0'))
           {
             opt_helpString = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'i': /* -i */
           if ((arg[0]=='-')&&(arg[2]=='\0'))
           {
             opt_ignore = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'V': /* +V */
           if ((arg[0]=='+')&&(arg[2]=='\0'))
           {
             opt_verbose = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'd': /* +d */
           if ((arg[0]=='+')&&(arg[2]=='\0'))
           {
             opt_debug = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'B': /* +B */
           if ((arg[0]=='+')&&(arg[2]=='\0'))
           {
             if (opt_bypassDisabled)
             {
               fprintf(stderr, "options %s and +B are mutually exclusive\n",opt_bypassDisabled);
               return 1;
             } else {    
               opt_bypass = 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'F': /* +F, -F */
           if (arg[2] == '\0')
           {
             if (arg[0] == '-') 
             {
                if (opt_paddingUsed)
                {
                  fprintf(stderr, "options -F and %s are mutually exclusive\n", opt_paddingUsed);
                  return 1;
                } else opt_useMetaheader=0; 
             } else opt_useMetaheader=1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 's': /* -s n */
           if (arg[2]=='\0')
           {
             if (arg[0]=='-')
             {
               if (extract_unsigned_long(opt_sleepAfter, argc, argv, i+1)) i++;
               else
               {
                 fprintf(stderr, "illegal parameter in %s option\n", arg);
                 return 1;
               }
             } else {
               if (extract_unsigned_long(opt_sleepDuring, argc, argv, i+1)) i++;
               else
               {
                 fprintf(stderr, "illegal parameter in %s option\n", arg);
                 return 1;
               }
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'b': /* -b n */
           if ((arg[0]=='-')&&(arg[2]=='\0')&&
               (extract_unsigned_long(opt_maxPDU, argc, argv, i+1)))
           {
             if ((opt_maxPDU<ASC_MINIMUMPDUSIZE)||(opt_maxPDU>ASC_MAXIMUMPDUSIZE))
             {
               fprintf(stderr, "option %s: PDU size out of range (%lu-%lu)\n", 
                       arg, (unsigned long)ASC_MINIMUMPDUSIZE, (unsigned long)ASC_MAXIMUMPDUSIZE);
               return 1;
             } else i++;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'u': /* +u, -u */
           if (arg[2] == '\0')
           {
             if (opt_bypass)
             {
               fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
               return 1;
             } else {
               if (arg[0] == '-') dcmEnableUnknownVRGeneration = FALSE;
               else dcmEnableUnknownVRGeneration = TRUE;
               opt_bypassDisabled = arg;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'e': /* +e, -e */
           if (arg[2] == '\0')
           {
             if (opt_bypass)
             {
               fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
               return 1;
             } else {
               if (arg[0] == '-') opt_sequenceType=EET_UndefinedLength; else opt_sequenceType=EET_ExplicitLength;
               opt_bypassDisabled = arg;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 't': /* +t=, +ti, +te, +tb, -t=, -ti, -te, -tb */ 
           if ((strlen(arg)==3)&&(arg[0]=='+'))
           {
             switch (arg[2])
             {
               case '=':
                 if (opt_bypass)
                 {
                   fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
                   return 1;
                 } else {
                   opt_writeTransferSyntax = EXS_Unknown;
                   opt_bypassDisabled = arg;
                 }
                 break;
               case 'i':
                 if (opt_bypass)
                 {
                   fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
                   return 1;
                 } else {
                   opt_writeTransferSyntax = EXS_LittleEndianImplicit;
                   opt_bypassDisabled = arg;
                 }
                 break;
               case 'e':
                 if (opt_bypass)
                 {
                   fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
                   return 1;
                 } else {
                   opt_writeTransferSyntax = EXS_LittleEndianExplicit;
                   opt_bypassDisabled = arg;
                 }
                 break;
               case 'b':
                 if (opt_bypass)
                 {
                   fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
                   return 1;
                 } else {
                   opt_writeTransferSyntax = EXS_BigEndianExplicit;
                   opt_bypassDisabled = arg;
                 }
                 break;
               default:
                 fprintf(stderr, "unknown option: %s\n", arg);
                 return 1;
             }
           }
           else if ((strlen(arg)==3)&&(arg[0]=='-'))
           {
             switch (arg[2])
             {
               case '=':
                 opt_networkTransferSyntax = EXS_Unknown;
                 break;
               case 'i':
                 opt_networkTransferSyntax = EXS_LittleEndianImplicit;
                 break;
               case 'e':
                 opt_networkTransferSyntax = EXS_LittleEndianExplicit;
                 break;
               case 'b':
                 if (opt_bypass)
                 opt_networkTransferSyntax = EXS_BigEndianExplicit;
                 break;
               default:
                 fprintf(stderr, "unknown option: %s\n", arg);
                 return 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'g': /* +g, +g=, -g */
           if (strcmp(arg,"+g")==0)
           {
             if (opt_bypass)
             {
               fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
               return 1;
             } else {
               opt_groupLength = EGL_withGL;
               opt_bypassDisabled = arg;
             }          
           } else if (strcmp(arg,"+g=")==0)
           {
             if (opt_bypass)
             {
               fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
               return 1;
             } else {
               opt_groupLength = EGL_recalcGL;
               opt_bypassDisabled = arg;
             }          
           } else if (strcmp(arg,"-g")==0)
           {
             if (opt_bypass)
             {
               fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
               return 1;
             } else {
               opt_groupLength = EGL_withoutGL;
               opt_bypassDisabled = arg;
             }          
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'p': /* -p, +p n m */
           if (arg[2] == '\0')
           {
             if (arg[0] == '-')         
             {
               if (opt_bypass)
               {
                 fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
                 return 1;
               } else if (! opt_useMetaheader)
               {
                 fprintf(stderr, "options -F and %s are mutually exclusive\n",arg);
                 return 1;
               } else {
                 opt_paddingType = EPD_withoutPadding;
                 opt_paddingUsed = arg;
               }          
             } 
             else
             {
               if (opt_bypass)
               {
                 fprintf(stderr, "options +B and %s are mutually exclusive\n",arg);
                 return 1;
               } else if (! opt_useMetaheader)
               {
                 fprintf(stderr, "options -F and %s are mutually exclusive\n",arg);
                 return 1;
               } else {
                 opt_paddingType = EPD_withPadding;
                 opt_paddingUsed = arg;
                 if ((extract_unsigned_long(opt_padlen, argc, argv, i+1))
                   &&(extract_unsigned_long(opt_subPadlen, argc, argv, i+2)))
                 {
                   i += 2;
                 } else {
                   fprintf(stderr, "illegal parameter in %s option\n", arg);
                   return 1;
                 }
               }
             }          
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
    
         default:
           fprintf(stderr, "unknown option: %s\n", arg);
           return 1;
       } /* end switch */
       
     } else if (opt_port == 0)
     {
       if ((! extract_unsigned_long(opt_port, argc, argv, i)) ||
           (opt_port > 0xFFFF) || (opt_port == 0))
       {
         fprintf(stderr, "illegal port number: %s\n", argv[i]);
         return 1;
       }
     } else {
       fprintf(stderr, "too many arguments: %s\n", arg);
       usage();
       return 1;
     }
  }

  if ( opt_port == 0 )
  {
    fprintf(stderr, "missing port number\n");
    return 1;
  }
    
  if (opt_helpString)
  {
    usage();
    return 0;
  }

  DUL_Debug(opt_debug);
  DIMSE_debug(opt_debug);
  SetDebugLevel(((opt_debug)?3:0));	/* dcmdata debugging */

#ifdef HAVE_GETEUID
  /* if port is privileged we must be as well */
  if (opt_port < 1024)
  {
    if (geteuid() != 0)
    {
	  fprintf(stderr, "storescp: cannot listen on port %lu, insufficient privileges\n", opt_port);
      return 1;
    }
  }
#endif

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
	  DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  cond = ASC_initializeNetwork(NET_ACCEPTOR, (int)opt_port, 1000, &net);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    return 1;
  }

#ifdef HAVE_GETUID
  /* return to normal uid so that we can't do too much damage in case
   * things go very wrong.   Only does someting if the program is setuid 
   * root, and run by another user.  Running as root user may be
   * potentially disasterous if this program screws up badly.
   */
  setuid(getuid());
#endif

  while (SUCCESS(cond)) cond = acceptAssociation(net);

  cond = ASC_dropNetwork(&net);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    return 1;
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  return 0;
}

static CONDITION 
acceptAssociation(T_ASC_Network * net)
{
  CONDITION cond;
  char buf[BUFSIZ];
  T_ASC_Association *assoc;

  const char* knownAbstractSyntaxes[] =
  {
    UID_VerificationSOPClass
  };
  
  const char* transferSyntaxes[] = { NULL, NULL, NULL };
  unsigned long numTransferSyntaxes = 0;
  
  cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU);
  if (!SUCCESS(cond))
  {
    if (opt_verbose) COND_DumpConditions();
    goto cleanup;
  }
  if (opt_verbose) printf("Association Received\n");

  if (opt_debug)
  {
    printf("Parameters:\n");
    ASC_dumpParameters(assoc->params);
  }

  if (opt_refuseAssociation)
  {
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    if (opt_verbose) printf("Refusing Association (forced via command line)\n");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond != ASC_NORMAL)
    {
      printf("Association Reject Failed:\n");
      COND_DumpConditions();
    }
    goto cleanup;
  }

  switch (opt_networkTransferSyntax)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 2;
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 2;
      break;
    default:
      /* We prefer explicit transfer syntaxes. 
       * If we are running on a Little Endian machine we prefer 
       * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
       */
      if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
      {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      } else {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      }
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
  }

  /* accept the Verification SOP Class if presented */
  cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
    assoc->params, 
    knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
    transferSyntaxes, numTransferSyntaxes);
  if (!SUCCESS(cond))
  {
    if (opt_verbose) COND_DumpConditions();
    goto cleanup;
  }

  /* the array of Storage SOP Class UIDs comes from dcuid.h */
  cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
    assoc->params, 
    dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs,
    transferSyntaxes, numTransferSyntaxes);
  if (!SUCCESS(cond))
  {
    if (opt_verbose) COND_DumpConditions();
    goto cleanup;
  }

  /* set our app title */
  ASC_setAPTitles(assoc->params, NULL, NULL, APPLICATIONTITLE);

  /* acknowledge or reject this association */
  if ((cond = ASC_getApplicationContextName(assoc->params, buf) != 
    ASC_NORMAL) || strcmp(buf, UID_StandardApplicationContext) != 0)
  {
    /* reject: the application context name is not supported */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
    };

    if (opt_verbose) printf("Association Rejected: bad appliciation context name: %s\n", buf);
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond != ASC_NORMAL)
    {
      if (opt_verbose) COND_DumpConditions();
    }
    goto cleanup;

  } else if (opt_rejectWithoutImplementationUID && 
      strlen(assoc->params->theirImplementationClassUID) == 0)
  {
    /* reject: the no implementation Class UID provided */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    if (opt_verbose) printf("Association Rejected: No Implementation Class UID provided\n");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond != ASC_NORMAL)
    {
      if (opt_verbose) COND_DumpConditions();
    }
    goto cleanup;
  } else {
    cond = ASC_acknowledgeAssociation(assoc);
    if (cond != ASC_NORMAL)
    {
      COND_DumpConditions();
      goto cleanup;
    }
    if (opt_verbose)
    {
      printf("Association Acknowledged (Max Send PDV: %lu)\n", assoc->sendPDVLength);
      if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
        printf("    (but no valid presentation contexts)\n");
      if (opt_debug) ASC_dumpParameters(assoc->params);
    }
  }

  /* now do the real work */
  cond = processCommands(assoc);

  if (cond == DIMSE_PEERREQUESTEDRELEASE)
  {
    COND_PopCondition(FALSE); 
      /* pop only the peer requested
       * release condition from the stack */
    if (opt_verbose) printf("Association Release\n");
    cond = ASC_acknowledgeRelease(assoc);
  } 
  else if (cond == DIMSE_PEERABORTEDASSOCIATION)
  {
    COND_PopCondition(FALSE);  /* pop DIMSE abort */
    COND_PopCondition(FALSE);  /* pop DUL abort */
    if (opt_verbose) printf("Association Aborted\n");
  } else {
    fprintf(stderr, "storescp: DIMSE Failure (aborting association):\n");
    COND_DumpConditions();
    /* some kind of error so abort the association */
    cond = ASC_abortAssociation(assoc);
  }

cleanup:
  cond = ASC_dropAssociation(assoc);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    exit(1);
  }
  cond = ASC_destroyAssociation(&assoc);
  if (!SUCCESS(cond))
  {
    COND_DumpConditions();
    exit(1);
  }

  if (opt_verbose)
  {
    /* are there any conditions sitting on the condition stack? */
    char buf[BUFSIZ];
    CONDITION c;

    if (COND_TopCondition(&c, buf, BUFSIZ) != COND_NORMAL)
    {
      fprintf(stderr, "CONDITIONS Remaining\n");
      COND_DumpConditions();
    }
  }
  return cond;
}


static CONDITION
processCommands(T_ASC_Association * assoc)
{
  CONDITION cond = DIMSE_NORMAL;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID;
  DcmDataset *statusDetail = NULL;

  while (cond == DIMSE_NORMAL)
  {
    cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID,
        &msg, &statusDetail);
    if (statusDetail != NULL)
    {
      printf("Extra Status Detail: \n");
      statusDetail->print();
      delete statusDetail;
    }

    /* did peer release, abort, or do we have a valid message ? */
    switch (cond)
    {
      case DIMSE_NORMAL:
        /* process command */
        switch (msg.CommandField)
        {
          case DIMSE_C_ECHO_RQ:
            cond = echoSCP(assoc, &msg, presID);
            break;
          case DIMSE_C_STORE_RQ:
            cond = storeSCP(assoc, &msg, presID);
            break;
          default:
            /* we cannot handle this kind of message */
            cond = DIMSE_BADCOMMANDTYPE;
            fprintf(stderr, "storescp: Cannot handle command: 0x%x\n", (unsigned)msg.CommandField);
            break;
        }
        break;
      case DIMSE_PEERREQUESTEDRELEASE:
      case DIMSE_PEERABORTEDASSOCIATION:
        /* association gone */
        break;
      default:
        break;
    }
  }
  return cond;
}


static CONDITION echoSCP(
  T_ASC_Association * assoc,
  T_DIMSE_Message * msg,
  T_ASC_PresentationContextID presID)
{
  CONDITION cond;

  if (opt_verbose)
  {
    printf("Received ");
    DIMSE_printCEchoRQ(stdout, &msg->msg.CEchoRQ);
  }

  /* the echo succeeded !! */
  cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);
  if (!SUCCESS(cond))
  {
    fprintf(stderr, "storescp: Echo SCP Failed:\n");
    COND_DumpConditions();
  }
  return cond;
}

struct StoreCallbackData
{
  char* imageFileName;
  DcmFileFormat* dcmff;
};

static void 
storeSCPCallback(
    /* in */
    void *callbackData, 
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName, DcmDataset **imageDataSet, /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if (opt_sleepDuring > 0)
    {
      sleep((unsigned int)opt_sleepDuring);
    }
    
    if (opt_verbose)
    {
      switch (progress->state)
      {
        case DIMSE_StoreBegin:    
          printf("RECV:");
          break;
        case DIMSE_StoreEnd:
          printf("\n");
          break;
        default:
          putchar('.');
          break;
      }
      fflush(stdout);
    }
    
    if (progress->state == DIMSE_StoreEnd)
    {
       *statusDetail = NULL;    /* no status detail */

       /* could save the image somewhere else, put it in database, etc */
       rsp->Status = STATUS_Success;

       if ((imageDataSet)&&(*imageDataSet)&&(!opt_bypass)&&(!opt_ignore))
       {
         StoreCallbackData *cbdata = (StoreCallbackData*) callbackData;
         const char* fileName = cbdata->imageFileName;

         DcmFileStream outf(fileName, DCM_WriteMode);
         if (outf.Fail())
         {
           fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
           rsp->Status = STATUS_STORE_Refused_OutOfResources;
         } else {
           E_TransferSyntax xfer = opt_writeTransferSyntax;
           if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

           if (opt_useMetaheader)
           {
             /* write as fileformat */
             cbdata->dcmff->transferInit();
             cbdata->dcmff->write(outf, xfer, opt_sequenceType, opt_groupLength,
               opt_paddingType, (Uint32)opt_padlen, (Uint32)opt_subPadlen);
             cbdata->dcmff->transferEnd();
             if (cbdata->dcmff->error() != EC_Normal)
             {
               fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
               rsp->Status = STATUS_STORE_Refused_OutOfResources;
             }
           } else {
             /* write as dataset */
             (*imageDataSet)->transferInit();
             (*imageDataSet)->write(outf, xfer, opt_sequenceType, opt_groupLength,
               opt_paddingType, (Uint32)opt_padlen, (Uint32)opt_subPadlen);
             (*imageDataSet)->transferEnd();
             if ((*imageDataSet)->error() != EC_Normal)
             {
               fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
               rsp->Status = STATUS_STORE_Refused_OutOfResources;
             }
           }
         }
      
        /* should really check the image to make sure it is consistent,
         * that its sopClass and sopInstance correspond with those in
         * the request.
         */
        if ((rsp->Status == STATUS_Success)&&(!opt_ignore))
        {
          /* which SOP class and SOP instance ? */
          if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance))
          {
             fprintf(stderr, "storescp: Bad image file: %s\n", imageFileName);
             rsp->Status = STATUS_STORE_Error_CannotUnderstand;
          }
          else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
          {
            rsp->Status = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
          else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
          {
            rsp->Status = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
          }
        }
      }
    }
    return;
}

static CONDITION storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_Message *msg,
  T_ASC_PresentationContextID presID)
{
    CONDITION cond;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[2048];

    req = &msg->msg.CStoreRQ;
    if (opt_ignore)
    {
      strcpy(imageFileName, "/dev/null");
    } else {
      sprintf(imageFileName, "%s.%s", 
        DU_sopClassToModality(req->AffectedSOPClassUID),
        req->AffectedSOPInstanceUID);
    }

    if (opt_verbose)
    {
      printf("Received ");
      DIMSE_printCStoreRQ(stdout, req);
    }

    StoreCallbackData callbackData;
    callbackData.imageFileName = imageFileName;
    DcmFileFormat dcmff;
    callbackData.dcmff = &dcmff;

    DcmDataset *dset = dcmff.getDataset();

    if (opt_bypass)
    {
      cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader,
        NULL, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
    } else {
      cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL, opt_useMetaheader,
        &dset, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
    }

    if (!SUCCESS(cond))
    {
      fprintf(stderr, "storescp: Store SCP Failed:\n");
      COND_DumpConditions();
      /* remove file */
      if (!opt_ignore)
      {
        unlink(imageFileName);
      }
    }

    if (opt_sleepAfter > 0)
    {
      sleep((unsigned int)opt_sleepAfter);
    }
    return cond;
}


/*
** CVS Log
** $Log: storescp.cc,v $
** Revision 1.11  1997-05-27 15:44:53  meichel
** Corrected typo in storescp help texts.
**
** Revision 1.10  1997/05/23 10:44:20  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes to interfaces of some DIMSE functions.
**
** Revision 1.9  1997/05/22 13:29:59  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.8  1997/05/16 08:31:34  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.7  1997/03/27 16:11:27  hewett
** Added command line switches allowing generation of UN to
** be disabled (it is enabled by default).
**
** Revision 1.6  1996/12/16 15:14:00  hewett
** Added bugfix for WINSOCK support.  The required WINSOCK version
** number was being incorrectly set to version 0.1.  The fixed
** WINSOCK initialisation now uses the MAKEWORD macro to correctly
** set the required version number. This bugfix was contributed
** by Dr. Yongjian Bao of Innomed GmbH, Germany.
**
** Revision 1.5  1996/09/27 14:05:05  hewett
** Added calls to initialise WINSOCK library for Win32 environment.  Only
** compiled in if HAVE_WINSOCK_H
**
** Revision 1.4  1996/09/27 08:24:30  hewett
** System header files now enclosed with BEGIN_EXTERN_C/END_EXTERN_C
**
** Revision 1.3  1996/09/24 16:20:32  hewett
** Now uses global table of Storage SOP Class UIDs (from dcuid.h).
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.2  1996/09/03 11:39:03  hewett
** Added copyright information.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
**
*/

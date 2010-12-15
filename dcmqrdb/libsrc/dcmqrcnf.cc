/*
 *
 *  Copyright (C) 1993-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg / Ralph Meyer
 *
 *  Purpose: class DcmQueryRetrieveConfig
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-12-15 13:59:58 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrcnf.h"

/* includes */
#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#define INCLUDE_CSTDARG
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofcmdln.h"

OFLogger DCM_dcmqrdbGetLogger()
{
    static OFLogger DCM_dcmqrdbLogger = OFLog::getLogger("dcmtk.dcmqrdb");
    return DCM_dcmqrdbLogger;
}

int DcmQueryRetrieveConfig::aeTitlesForPeer(const char *hostName, const char *** aeTitleList) const
{
    int n = 0;
    int i, j, k;
    const int chunkSize = 1;
    int maxAlloc = 0;
    const char *hname;
    const char *aetitle;
    int found;

    *aeTitleList = (const char**)malloc(chunkSize*sizeof(const char*));
    maxAlloc = chunkSize;

    /* collect up titles for peer, search in host table */
    for (i=0; i<CNF_HETable.noOfHostEntries; i++) {
        for (j=0; j<CNF_HETable.HostEntries[i].noOfPeers; j++) {
            hname = CNF_HETable.HostEntries[i].Peers[j].HostName;
            aetitle = CNF_HETable.HostEntries[i].Peers[j].ApplicationTitle;
#ifdef HAVE_PROTOTYPE_STRCASECMP
            if (strcasecmp(hname, hostName) == 0) {  /* DNS is not case-sensitive */
#elif defined(HAVE_PROTOTYPE__STRICMP)
            if (_stricmp(hname, hostName) == 0) {
#else
            if (strcmp(hname, hostName) == 0) {  /* fallback if case insensitive compare is unavailable */
#endif
                /* found an entry for peer host */
                /* make sure its not already in list */
                found = 0;
                for (k=0; !found && k<n; k++) {
                    found = (strcmp((*aeTitleList)[k], aetitle) == 0);
                }
                if (!found) {
                    if (n >= maxAlloc) {
                        *aeTitleList = (const char**)realloc(*aeTitleList,
                                      (maxAlloc + chunkSize)*sizeof(const char*));
                        maxAlloc += chunkSize;
                    }
                    (*aeTitleList)[n] = aetitle;

                    n++;
                }
            }
        }
    }
    /* collect up titles for peer, search in AE table */
    for (i=0; i<CNF_Config.noOfAEEntries; i++) {
        for (j=0; j<CNF_Config.AEEntries[i].noOfPeers; j++) {
            hname = CNF_Config.AEEntries[i].Peers[j].HostName;
            aetitle = CNF_Config.AEEntries[i].Peers[j].ApplicationTitle;

#ifdef HAVE_PROTOTYPE_STRCASECMP
            if (strcasecmp(hname, hostName) == 0) {  /* DNS is not case-sensitive */
#elif defined(HAVE_PROTOTYPE__STRICMP)
            if (_stricmp(hname, hostName) == 0) {
#else
            if (strcmp(hname, hostName) == 0) {  /* fallback if case insensitive compare is unavailable */
#endif
                /* found an entry for peer host */
                /* make sure its not already in list */
                found = 0;
                for (k=0; !found && k<n; k++) {
                    found = (strcmp((*aeTitleList)[k], aetitle) == 0);
                }
                if (!found) {
                    if (n >= maxAlloc) {
                        *aeTitleList = (const char**)realloc(*aeTitleList,
                                        (maxAlloc + chunkSize)*sizeof(const char*));
                        maxAlloc += chunkSize;
                    }
                    (*aeTitleList)[n] = aetitle;

                    n++;
                }
            }
        }
    }

    if (n == 0) {
        free(*aeTitleList);
        *aeTitleList = NULL;
    }
    return n;
}


int DcmQueryRetrieveConfig::ctnTitles(const char *** ctnTitleList) const
{
    int i;
    int n = 0;

    n = CNF_Config.noOfAEEntries;
    *ctnTitleList = (const char**)malloc(n * sizeof(const char*));

    for (i=0; i<n; i++) {
        (*ctnTitleList)[i] = CNF_Config.AEEntries[i].ApplicationTitle;
    }
    return n;
}


int DcmQueryRetrieveConfig::aeTitlesForSymbolicName(const char *symbolicName, const char *** aeTitleList) const
{
    int i = 0;
    int j = 0;
    int n = 0;

    for (i=0; i<CNF_HETable.noOfHostEntries; i++) {
        if (strcmp(symbolicName, CNF_HETable.HostEntries[i].SymbolicName)==0) {
            n = CNF_HETable.HostEntries[i].noOfPeers;
            *aeTitleList = (const char**)malloc(n * sizeof(const char*));
            for (j=0; j<n; j++) {
                (*aeTitleList)[j] =
                    CNF_HETable.HostEntries[i].Peers[j].ApplicationTitle;
            }
            return n;
        }
    }
    return 0;

}

const char *DcmQueryRetrieveConfig::vendorForPeerAETitle(const char *peerAETitle) const
{
    int i = 0;
    int j = 0;

    for (i=0; i<CNF_VendorTable.noOfHostEntries; i++) {
        for (j=0; j<CNF_VendorTable.HostEntries[i].noOfPeers; j++) {
            if (strcmp(peerAETitle,
                CNF_VendorTable.HostEntries[i].Peers[j].ApplicationTitle)==0) {
                return CNF_VendorTable.HostEntries[i].SymbolicName;
            }
        }
    }
    return NULL;
}

int DcmQueryRetrieveConfig::countCtnTitles() const
{
    return CNF_Config.noOfAEEntries;
}


void DcmQueryRetrieveConfig::initConfigStruct()
{
   UserName_ = "";
   GroupName_ = "";
   networkTCPPort_ = 104;
   maxPDUSize_ = 16384;
   maxAssociations_ = 16;
   CNF_Config.noOfAEEntries = 0;
   CNF_HETable.noOfHostEntries = 0;
   CNF_VendorTable.noOfHostEntries = 0;
}


void DcmQueryRetrieveConfig::panic(const char *fmt, ...)
{
   va_list  ap;
   va_start(ap, fmt);

#if defined(HAVE_VSNPRINTF) && defined(HAVE_PROTOTYPE_VSNPRINTF)
   char buf[4096];

#ifdef HAVE_PROTOTYPE_STD__VSNPRINTF
   std::vsnprintf(buf, sizeof(buf), fmt, ap);
#else
   vsnprintf(buf, sizeof(buf), fmt, ap);
#endif

   // Since we can't do anything about a too small buffer for vsnprintf(), we
   // ignore it. But we do make sure the buffer is null-terminated!
   buf[4095] = '\0';

   DCMQRDB_ERROR("CONFIG Error: " << buf << "!");
#else
   fprintf(stderr, "CONFIG Error: ");
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "!\n");
#endif
   va_end(ap);
}


int DcmQueryRetrieveConfig::readConfigLines(FILE *cnffp)
{
   int  lineno = 0,       /* line counter */
        error = 0;        /* error flag */
   char rcline[512],      /* line in configuration file */
        mnemonic[64],     /* mnemonic in line */
        value[256],       /* parameter value */
        *valueptr;        /* pointer to value list */
   char *c;

   while (!feof(cnffp)) {
      fgets(rcline, sizeof(rcline), cnffp); /* read line in configuration file */
      lineno++;
      if (feof(cnffp)) continue;
      if (rcline[0] == '#' || rcline[0] == 10 || rcline[0] == 13)
         continue;        /* comment or blank line */

      if (sscanf(rcline, "%s", mnemonic) != 1)
        continue;  /* ignore lines containing only whitespace */

      valueptr = skipmnemonic(rcline);

      if (!strcmp("ApplicationTitle", mnemonic)) {
        // ignore this entry which was used (really?) in previous versions
      }
      else if (!strcmp("ApplicationContext", mnemonic)) {
        // ignore this entry which was used (really?) in previous versions
      }
      else if (!strcmp("ImplementationClass", mnemonic)) {
        // ignore this entry which was used (really?) in previous versions
      }
      else if (!strcmp("ImplementationVersion", mnemonic)) {
        // ignore this entry which was used (really?) in previous versions
      }
      else if (!strcmp("NetworkType", mnemonic)) {
        // ignore this entry which was used (really?) in previous versions
      }
      else if (!strcmp("UserName", mnemonic)) {
         c = parsevalues(&valueptr);
         UserName_ = c;
         free(c);
      }
      else if (!strcmp("GroupName", mnemonic)) {
         c = parsevalues(&valueptr);
         GroupName_ = c;
         free(c);
      }
      else if (!strcmp("NetworkTCPPort", mnemonic)) {
         sscanf(valueptr, "%d", &networkTCPPort_);
      }
      else if (!strcmp("MaxPDUSize", mnemonic)) {
         unsigned long ul = 0;
         sscanf(valueptr, "%lu", &ul);
         maxPDUSize_ = OFstatic_cast(Uint32, ul);
      }
      else if (!strcmp("MaxAssociations", mnemonic)) {
         sscanf(valueptr, "%d", &maxAssociations_);
      }
      else if (!strcmp("Display", mnemonic))
      {
        // ignore this entry which was needed for ctndisp
      }
      else if (!strcmp("DisplayPort", mnemonic))
      {
        // ignore this entry which was needed for ctndisp
      }
      else if (!strcmp("HostTable", mnemonic)) {
         sscanf(valueptr, "%s", value);
         if (!strcmp("BEGIN", value)) {
            if (!readHostTable(cnffp, &lineno))
               error = 1;
         }
         else if (!strcmp("END", value)) {
            panic("No \"HostTable BEGIN\" before END in configuration file, line %d", lineno);
            error = 1;
         }
         else {
            panic("Unknown HostTable status \"%s\" in configuartion file, line %d", value, lineno);
            error = 1;
         }
      }
      else if (!strcmp("VendorTable", mnemonic)) {
         sscanf(valueptr, "%s", value);
         if (!strcmp("BEGIN", value)) {
            if (!readVendorTable(cnffp, &lineno))
               error = 1;
         }
         else if (!strcmp("END", value)) {
            panic("No \"VendorTable BEGIN\" before END in configuration file, line %d", lineno);
            error = 1;
         }
         else {
            panic("Unknown VendorTable status \"%s\" in configuartion file, line %d", value, lineno);
            error = 1;
         }
      }
      else if (!strcmp("AETable", mnemonic)) {
         sscanf(valueptr, "%s", value);
         if (!strcmp("BEGIN", value)) {
            if (!readAETable(cnffp, &lineno))
               error = 1;
         }
         else if (!strcmp("END", value)) {
            panic("No \"AETable BEGIN\" before END in configuration file, line %d", lineno);
            error = 1;
         }
         else {
            panic("Unknown AETable status \"%s\" in configuartion file, line %d", value, lineno);
            error = 1;
         }
      }
      else {
         panic("Unknown mnemonic \"%s\" in configuration file, line %d", mnemonic, lineno);
         error = 1;
      }
   }

   return(error ? 0 : 1);
}


int DcmQueryRetrieveConfig::readHostTable(FILE *cnffp, int *lineno)
{
   int  error = 0,        /* error flag */
        end = 0,          /* end flag */
        noOfPeers;        /* number of peers for entry */
   char rcline[512],      /* line in configuration file */
        mnemonic[64],     /* mnemonic in line */
        value[256],       /* parameter value */
        *lineptr;         /* pointer to line */
   DcmQueryRetrieveConfigHostEntry *helpentry;

   while (!feof(cnffp)) {
      fgets(rcline, sizeof(rcline), cnffp); /* read line in configuration file */
      (*lineno)++;
      if (feof(cnffp)) continue;
      if (rcline[0] == '#' || rcline[0] == 10 || rcline[0] == 13)
         continue;        /* comment or blank line */

      sscanf(rcline, "%s %s", mnemonic, value);
      if (!strcmp("HostTable", mnemonic)) {
         if (!strcmp("END", value)) {
            end = 1;
            break;
         }
         else {
            panic("Illegal HostTable status \"%s\" in configuration file, line %d", value, *lineno);
            error = 1;
            break;
         }
      }

      lineptr = rcline;
      CNF_HETable.noOfHostEntries++;
      if ((helpentry = (DcmQueryRetrieveConfigHostEntry *)malloc(CNF_HETable.noOfHostEntries * sizeof(DcmQueryRetrieveConfigHostEntry))) == NULL)
         panic("Memory allocation 1 (%d)", CNF_HETable.noOfHostEntries);
      if (CNF_HETable.noOfHostEntries - 1) {
         memcpy((char*)helpentry, (char*)CNF_HETable.HostEntries, (CNF_HETable.noOfHostEntries - 1) *sizeof(DcmQueryRetrieveConfigHostEntry));
         free(CNF_HETable.HostEntries);
      }
      CNF_HETable.HostEntries = helpentry;

      CNF_HETable.HostEntries[CNF_HETable.noOfHostEntries - 1].SymbolicName = parsevalues(&lineptr);
      CNF_HETable.HostEntries[CNF_HETable.noOfHostEntries - 1].Peers = readPeerList(&lineptr, &noOfPeers);
      CNF_HETable.HostEntries[CNF_HETable.noOfHostEntries - 1].noOfPeers = noOfPeers;
      if (!noOfPeers)
         error = 1;
   }

   if (!end) {
      error = 1;
      panic("No \"HostTable END\" in configuration file, line %d", *lineno);
    }
   return(error ? 0 : 1);
}


int DcmQueryRetrieveConfig::readVendorTable(FILE *cnffp, int *lineno)
{
   int  error = 0,        /* error flag */
        end = 0,          /* end flag */
        noOfPeers;        /* number of peers for entry */
   char rcline[512],      /* line in configuration file */
        mnemonic[64],     /* mnemonic in line */
        value[256],       /* parameter value */
        *lineptr;         /* pointer to line */
   DcmQueryRetrieveConfigHostEntry *helpentry;

   while (!feof(cnffp)) {
      fgets(rcline, sizeof(rcline), cnffp); /* read line in configuration file */
      (*lineno)++;
      if (feof(cnffp)) continue;
      if (rcline[0] == '#' || rcline[0] == 10 || rcline[0] == 13)
         continue;        /* comment or blank line */

      sscanf(rcline, "%s %s", mnemonic, value);
      if (!strcmp("VendorTable", mnemonic)) {
         if (!strcmp("END", value)) {
            end = 1;
            break;
         }
         else {
            panic("Illegal VendorTable status \"%s\" in configuration file, line %d", value, *lineno);
            error = 1;
            break;
         }
      }

      lineptr = rcline;
      CNF_VendorTable.noOfHostEntries++;
      if ((helpentry = (DcmQueryRetrieveConfigHostEntry *)malloc(CNF_VendorTable.noOfHostEntries * sizeof(DcmQueryRetrieveConfigHostEntry))) == NULL)
         panic("Memory allocation 2 (%d)", CNF_VendorTable.noOfHostEntries);
      if (CNF_VendorTable.noOfHostEntries - 1) {
         memcpy((char*)helpentry, (char*)CNF_VendorTable.HostEntries, (CNF_VendorTable.noOfHostEntries - 1) *sizeof(DcmQueryRetrieveConfigHostEntry));
         free(CNF_VendorTable.HostEntries);
      }
      CNF_VendorTable.HostEntries = helpentry;

      CNF_VendorTable.HostEntries[CNF_VendorTable.noOfHostEntries - 1].SymbolicName = parsevalues(&lineptr);
      CNF_VendorTable.HostEntries[CNF_VendorTable.noOfHostEntries - 1].Peers = readPeerList(&lineptr, &noOfPeers);
      CNF_VendorTable.HostEntries[CNF_VendorTable.noOfHostEntries - 1].noOfPeers = noOfPeers;
      if (!noOfPeers)
         error = 1;
   }

   if (!end) {
      error = 1;
      panic("No \"VendorTable END\" in configuration file, line %d", *lineno);
    }
   return(error ? 0 : 1);
}


int DcmQueryRetrieveConfig::readAETable(FILE *cnffp, int *lineno)
{
   int  error = 0,          /* error flag */
        end = 0,            /* end flag */
        noOfAEEntries = 0;  /* number of AE entries */
   char rcline[512],        /* line in configuration file */
        mnemonic[64],       /* mnemonic in line */
        value[256],         /* parameter value */
        *lineptr;           /* pointer to line */
   DcmQueryRetrieveConfigAEEntry *helpentry;

   while (!feof(cnffp)) {
      fgets(rcline, sizeof(rcline), cnffp); /* read line in configuration file */
      (*lineno)++;
      if (feof(cnffp)) continue;
      if (rcline[0] == '#' || rcline[0] == 10 || rcline[0] == 13)
         continue;        /* comment or blank line */

      sscanf(rcline, "%s %s", mnemonic, value);
      if (!strcmp("AETable", mnemonic)) {
         if (!strcmp("END", value)) {
            end = 1;
            break;
         }
         else {
            panic("Illegal AETable status \"%s\" in configuration file, line %d", value, *lineno);
            error = 1;
            break;
         }
      }

      lineptr = rcline;
      noOfAEEntries++;
      if ((helpentry = (DcmQueryRetrieveConfigAEEntry *)malloc(noOfAEEntries * sizeof(DcmQueryRetrieveConfigAEEntry))) == NULL)
         panic("Memory allocation 3 (%d)", noOfAEEntries);
      if (noOfAEEntries - 1) {
         memcpy((char*)helpentry, (char*)CNF_Config.AEEntries, (noOfAEEntries - 1) *sizeof(DcmQueryRetrieveConfigAEEntry));
         free(CNF_Config.AEEntries);
      }
      CNF_Config.AEEntries = helpentry;

      CNF_Config.AEEntries[noOfAEEntries - 1].ApplicationTitle = parsevalues(&lineptr);
      CNF_Config.AEEntries[noOfAEEntries - 1].StorageArea = parsevalues(&lineptr);
      CNF_Config.AEEntries[noOfAEEntries - 1].Access = parsevalues(&lineptr);
      CNF_Config.AEEntries[noOfAEEntries - 1].StorageQuota = parseQuota(&lineptr);
      CNF_Config.AEEntries[noOfAEEntries - 1].Peers = parsePeers(&lineptr, &CNF_Config.AEEntries[noOfAEEntries - 1].noOfPeers);
      if (!CNF_Config.AEEntries[noOfAEEntries - 1].noOfPeers)
         error = 1;
   }

   if (!end) {
      error = 1;
      panic("No \"AETable END\" in configuration file, line %d", *lineno);
    }
   CNF_Config.noOfAEEntries = noOfAEEntries;
   return(error ? 0 : 1);
}


DcmQueryRetrieveConfigQuota *DcmQueryRetrieveConfig::parseQuota(char **valuehandle)
{
   int  studies;
   char *helpvalue,
        helpval[20];
   DcmQueryRetrieveConfigQuota *helpquota;

   if ((helpquota = (DcmQueryRetrieveConfigQuota *)malloc(sizeof(DcmQueryRetrieveConfigQuota))) == NULL)
      panic("Memory allocation 4");
   helpvalue = parsevalues(valuehandle);
   sscanf(helpvalue, "%d , %s", &studies, helpval);
   helpquota->maxStudies = studies;
   helpquota->maxBytesPerStudy = quota(helpval);
   free(helpvalue);

   return(helpquota);
}


DcmQueryRetrieveConfigPeer *DcmQueryRetrieveConfig::parsePeers(char **valuehandle, int *peers)
{
   char *helpvalue;
   char *valueptr = *valuehandle;

   helpvalue = parsevalues(valuehandle);
   if (!strcmp("ANY", helpvalue)) {     /* keywork ANY used */
      free(helpvalue);
      *peers = -1;
      return((DcmQueryRetrieveConfigPeer *) 0);
   }

   free(helpvalue);         /* regular peer list */
   return(readPeerList(&valueptr, peers));
}


DcmQueryRetrieveConfigPeer *DcmQueryRetrieveConfig::readPeerList(char **valuehandle, int *peers)
{
   int  i,
   found,
   noOfPeers = 0;
   char *helpvalue;
   DcmQueryRetrieveConfigPeer *helppeer,
   *peerlist = NULL;

   while((helpvalue = parsevalues(valuehandle)) != NULL) {
      found = 0;
      if (strchr(helpvalue, ',') == NULL) {   /* symbolic name */
         if (!CNF_HETable.noOfHostEntries) {
            panic("No symbolic names defined");
            *peers = 0;
            free(helpvalue);
            return((DcmQueryRetrieveConfigPeer *) 0);
         }
         for(i = 0; i < CNF_HETable.noOfHostEntries; i++) {
            if (!strcmp(CNF_HETable.HostEntries[i].SymbolicName, helpvalue)) {
               found = 1;
               break;
            }
         }
         if (!found) {
            panic("Symbolic name \"%s\" not defined", helpvalue);
            *peers = 0;
            free(helpvalue);
            return((DcmQueryRetrieveConfigPeer *) 0);
         }

         noOfPeers += CNF_HETable.HostEntries[i].noOfPeers;
        if ((helppeer = (DcmQueryRetrieveConfigPeer *)malloc(noOfPeers * sizeof(DcmQueryRetrieveConfigPeer))) == NULL)
            panic("Memory allocation 5 (%d)", noOfPeers);
        if (noOfPeers - CNF_HETable.HostEntries[i].noOfPeers) {
            memcpy((char*)helppeer, (char*)peerlist, (noOfPeers - CNF_HETable.HostEntries[i].noOfPeers) * sizeof(DcmQueryRetrieveConfigPeer));
            free(peerlist);
         }
         peerlist = helppeer;
         memcpy((char*)(peerlist + (noOfPeers - CNF_HETable.HostEntries[i].noOfPeers)), (char*)CNF_HETable.HostEntries[i].Peers, CNF_HETable.HostEntries[i].noOfPeers * sizeof(DcmQueryRetrieveConfigPeer));
      }

      else {            /* peer */
         noOfPeers++;
         if ((helppeer = (DcmQueryRetrieveConfigPeer *)malloc(noOfPeers * sizeof(DcmQueryRetrieveConfigPeer))) == NULL)
            panic("Memory allocation 6 (%d)", noOfPeers);
         if (noOfPeers - 1) {
            memcpy((char*)helppeer, (char*)peerlist, (noOfPeers - 1) *sizeof(DcmQueryRetrieveConfigPeer));
            free(peerlist);
         }
         peerlist = helppeer;

         char *tempvalue = helpvalue;
         peerlist[noOfPeers - 1].ApplicationTitle = parsevalues(&helpvalue);
         peerlist[noOfPeers - 1].HostName = parsevalues(&helpvalue);
         peerlist[noOfPeers - 1].PortNumber = atoi(helpvalue);
         helpvalue = tempvalue;
      }
      free(helpvalue);
   }
   *peers = noOfPeers;
   return(peerlist);
}


char *DcmQueryRetrieveConfig::skipmnemonic (char *rcline)
{
   char *help = rcline;

   while(*help != '\0') {                       /* leading spaces */
      if (isgap(*help)) help++;
      else break;
    }
   while(*help != '\0') {
      if (!isspace(OFstatic_cast(unsigned char, *help))) help++;    /* Mnemonic */
      else break;
   }
   while(*help != '\0') {
      if (isgap(*help)) help++;     /* Gap */
      else break;
   }
   return(help);
}


int DcmQueryRetrieveConfig::isgap (char gap)
{
   if (isspace(OFstatic_cast(unsigned char, gap)))
      return(1);
   if (gap == '=' || gap == ',' || gap == 10 || gap == 13)
      return(1);
   else
      return(0);
}


int DcmQueryRetrieveConfig::isquote (char quote)
{
   if (quote == '"' || quote == '\'' || quote == '(' || quote == ')')
      return(1);
   else
      return(0);
}


char *DcmQueryRetrieveConfig::parsevalues (char **valuehandle)
{
   int i,
       inquotes = 0,
       count = 0;
   char *value = NULL;
   const char *help,
   *valueptr = *valuehandle;

   if (isquote(*valueptr)) {
      inquotes = 1;
      valueptr++;
   }

   help = valueptr;

   while(*help != '\0') {
      if (inquotes) {
         if (isquote(*help)) {
            if ((value = (char*)malloc(count * sizeof(char) + 1)) == NULL)
               panic("Memory allocation 7 (%d)", count);
            for(i = 0; i < count; i++)
               value[i] = valueptr[i];
            value[count] = '\0';
            count++;
            help++;
            while (*help != '\0') {
               if (isgap(*help)) {
                  count++;
                  help++;
               }
               else
                  break;
            }
            *valuehandle += (count + 1);
            break;
         }
         else {
            count++;
            help++;
         }
      }
      else {
         if (isgap(*help)) {
            if ((value = (char*)malloc(count * sizeof(char) + 1)) == NULL)
               panic("Memory allocation 8 (%d)", count);
            for(i = 0; i < count; i++)
               value[i] = valueptr[i];
            value[count] = '\0';
            while (*help != '\0') {
               if (isgap(*help)) {
                  count++;
                  help++;
               }
               else
                  break;
            }
            *valuehandle += count;
            break;
         }
         else {
           count++;
           help++;
         }
      } /* inquotes */
   } /* while */

   return(value);
}


long DcmQueryRetrieveConfig::quota (const char *value)
{
   int  number;
   long factor;
   char last = *(value + strlen(value) - 1),  /* last character */
   mult = *(value + strlen(value) - 2);       /* multiplier */

   if (last == 'b' || last == 'B') {
      if (mult == 'k' || mult == 'K') factor = 1024;
      else if (mult == 'm' || mult == 'M') factor = 1024 * 1024;
      else if (mult == 'g' || mult == 'G') factor = 1024 * 1024 * 1024;
      else factor = 1;
   }
   else return(-1L);

   number = atoi(value);
   return(number * factor);
}


int DcmQueryRetrieveConfig::init(const char *ConfigurationFile)
{
   int  error = 0;        /* error flag */
   FILE *cnffp;         /* configuration file pointer */

   if ((cnffp = fopen(ConfigurationFile, "r")) == NULL) {
      panic("Unable to open configuration file \"%s\"", ConfigurationFile);
      return(0);
   }

   initConfigStruct();

   if (!readConfigLines(cnffp)) {
      panic("Reading configuration file \"%s\" with errors", ConfigurationFile);
      error = 1;
   }

   fclose(cnffp);

   return(error ? 0 : 1);
}


void DcmQueryRetrieveConfig::printConfig()
{
   int i,j;

   DCMQRDB_INFO("\nHostTable: " <<  CNF_HETable.noOfHostEntries);
   for(i = 0; i < CNF_HETable.noOfHostEntries; i++) {
      DCMQRDB_INFO(CNF_HETable.HostEntries[i].SymbolicName << " " << CNF_HETable.HostEntries[i].noOfPeers);
      for(j = 0; j < CNF_HETable.HostEntries[i].noOfPeers; j++) {
         DCMQRDB_INFO(CNF_HETable.HostEntries[i].Peers[j].ApplicationTitle << " " <<
            CNF_HETable.HostEntries[i].Peers[j].HostName << " " << CNF_HETable.HostEntries[i].Peers[j].PortNumber);
      }
   }
   DCMQRDB_INFO("\nVendorTable: " << CNF_VendorTable.noOfHostEntries);
   for(i = 0; i < CNF_VendorTable.noOfHostEntries; i++) {
      DCMQRDB_INFO(CNF_VendorTable.HostEntries[i].SymbolicName << " " << CNF_VendorTable.HostEntries[i].noOfPeers);
      for(j = 0; j < CNF_VendorTable.HostEntries[i].noOfPeers; j++) {
         DCMQRDB_INFO(CNF_VendorTable.HostEntries[i].Peers[j].ApplicationTitle << " " <<
            CNF_VendorTable.HostEntries[i].Peers[j].HostName << " " << CNF_VendorTable.HostEntries[i].Peers[j].PortNumber);
      }
   }
   DCMQRDB_INFO("\nGlobal Parameters:\n" << networkTCPPort_ << "\n" << OFstatic_cast(unsigned long, maxPDUSize_)
      << "\n" << maxAssociations_);
   DCMQRDB_INFO("\nAEEntries: " << CNF_Config.noOfAEEntries);
   for(i = 0; i < CNF_Config.noOfAEEntries; i++) {
      DCMQRDB_INFO(CNF_Config.AEEntries[i].ApplicationTitle << "\n" << CNF_Config.AEEntries[i].StorageArea
         << "\n" << CNF_Config.AEEntries[i].Access << "\n" << CNF_Config.AEEntries[i].StorageQuota->maxStudies
         << ", " << CNF_Config.AEEntries[i].StorageQuota->maxBytesPerStudy);
      if (CNF_Config.AEEntries[i].noOfPeers == -1)
         DCMQRDB_INFO("Peers: ANY");
      else {
         DCMQRDB_INFO("Peers: " << CNF_Config.AEEntries[i].noOfPeers);
         for(j = 0; j < CNF_Config.AEEntries[i].noOfPeers; j++) {
            DCMQRDB_INFO(CNF_Config.AEEntries[i].Peers[j].ApplicationTitle << " " <<
               CNF_Config.AEEntries[i].Peers[j].HostName << " " << CNF_Config.AEEntries[i].Peers[j].PortNumber);
         }
      }
      DCMQRDB_INFO("----------------------------------\n");
   }
}


int DcmQueryRetrieveConfig::getNetworkTCPPort() const
{
   return(networkTCPPort_);
}


OFCmdUnsignedInt DcmQueryRetrieveConfig::getMaxPDUSize() const
{
   return(maxPDUSize_);
}


int DcmQueryRetrieveConfig::getMaxAssociations() const
{
   return(maxAssociations_);
}


const char *DcmQueryRetrieveConfig::getStorageArea(const char *AETitle) const
{
   int  i;

   for(i = 0; i < CNF_Config.noOfAEEntries; i++) {
      if (!strcmp(AETitle, CNF_Config.AEEntries[i].ApplicationTitle))
         return(CNF_Config.AEEntries[i].StorageArea);
   }
   return(NULL);        /* AETitle not found */
}


const char *DcmQueryRetrieveConfig::getAccess(const char *AETitle) const
{
   int  i;

   for(i = 0; i < CNF_Config.noOfAEEntries; i++) {
      if (!strcmp(AETitle, CNF_Config.AEEntries[i].ApplicationTitle))
         return(CNF_Config.AEEntries[i].Access);
   }
   return(NULL);        /* AETitle not found */
}


int DcmQueryRetrieveConfig::getMaxStudies(const char *AETitle) const
{
   int  i;

   for(i = 0; i < CNF_Config.noOfAEEntries; i++) {
      if (!strcmp(AETitle, CNF_Config.AEEntries[i].ApplicationTitle))
         return(CNF_Config.AEEntries[i].StorageQuota->maxStudies);
   }
   return(0);       /* AETitle not found */
}

long DcmQueryRetrieveConfig::getMaxBytesPerStudy(const char *AETitle) const
{
   int  i;

   for(i = 0; i < CNF_Config.noOfAEEntries; i++) {
      if (!strcmp(AETitle, CNF_Config.AEEntries[i].ApplicationTitle))
         return(CNF_Config.AEEntries[i].StorageQuota->maxBytesPerStudy);
   }
   return(0);       /* AETitle not found */
}


int DcmQueryRetrieveConfig::peerInAETitle(const char *calledAETitle, const char *callingAETitle, const char *HostName) const
{
   int  i,
    j;

   for(i = 0; i < CNF_Config.noOfAEEntries; i++) {
      if (!strcmp(calledAETitle, CNF_Config.AEEntries[i].ApplicationTitle)) {
         if (CNF_Config.AEEntries[i].noOfPeers == -1) /* ANY Peer allowed */
            return(1);
         for(j = 0; j < CNF_Config.AEEntries[i].noOfPeers; j++) {
            if (!strcmp(callingAETitle, CNF_Config.AEEntries[i].Peers[j].ApplicationTitle) &&
#ifdef HAVE_PROTOTYPE_STRCASECMP
                /* DNS is not case-sensitive */
                !strcasecmp(HostName, CNF_Config.AEEntries[i].Peers[j].HostName))
#elif defined(HAVE_PROTOTYPE__STRICMP)
                !_stricmp(HostName, CNF_Config.AEEntries[i].Peers[j].HostName))
#else
                /* fallback solution is to do case sensitive comparison on systems
                   which do not implement strcasecmp or _stricmp */
                !strcmp(HostName, CNF_Config.AEEntries[i].Peers[j].HostName))
#endif
               return(1);       /* Peer found */
         }
      }
   }
   return(0);           /* Peer not found */
}


int DcmQueryRetrieveConfig::peerForAETitle(const char *AETitle, const char **HostName, int *PortNumber) const
{
   int  i,
    j;

   for(i = 0; i < CNF_Config.noOfAEEntries; i++) {
      for(j = 0; j < CNF_Config.AEEntries[i].noOfPeers; j++) {
         if (!strcmp(AETitle, CNF_Config.AEEntries[i].Peers[j].ApplicationTitle)) {
            *HostName = CNF_Config.AEEntries[i].Peers[j].HostName;
            *PortNumber = CNF_Config.AEEntries[i].Peers[j].PortNumber;
            return(1);        /* Peer found in AETable */
         }
      }
   }

   for(i = 0; i < CNF_HETable.noOfHostEntries; i++) {
      for(j = 0; j < CNF_HETable.HostEntries[i].noOfPeers; j++) {
         if (!strcmp(AETitle, CNF_HETable.HostEntries[i].Peers[j].ApplicationTitle)) {
            *HostName = CNF_HETable.HostEntries[i].Peers[j].HostName;
            *PortNumber = CNF_HETable.HostEntries[i].Peers[j].PortNumber;
            return(2);        /* Peer found in HostTable */
         }
      }
   }

   return(0);         /* Peer not found */
}


int DcmQueryRetrieveConfig::checkForSameVendor(const char *AETitle1, const char *AETitle2) const
{
   int  i,
    j,
    k,
    found = 0;

   for(i = 0; i < CNF_VendorTable.noOfHostEntries; i++) {
      for(j = 0; j < CNF_VendorTable.HostEntries[i].noOfPeers; j++) {
         if (!strcmp(AETitle1, CNF_VendorTable.HostEntries[i].Peers[j].ApplicationTitle)) {
            for(k = 0; k < CNF_VendorTable.HostEntries[i].noOfPeers; k++) {
               if (!strcmp(AETitle2, CNF_VendorTable.HostEntries[i].Peers[k].ApplicationTitle))
                  found = 1;
            }
         }
      }
   }

   return(found);
}


int DcmQueryRetrieveConfig::HostNamesForVendor(const char *Vendor, const char ***HostNameArray) const
{
   int  i, j,
    found = 0;

   for(i = 0; i < CNF_VendorTable.noOfHostEntries; i++) {
      if (!strcmp(CNF_VendorTable.HostEntries[i].SymbolicName, Vendor)) {
         found = 1;
         break;
      }
   }

   if (!found)
      return(0);

   if ((*HostNameArray = (const char**)malloc(CNF_VendorTable.HostEntries[i].noOfPeers * sizeof(const char *))) == NULL) {
      panic("Memory allocation A (%d)", CNF_VendorTable.HostEntries[i].noOfPeers);
      return(0);
   }
   for(j = 0; j < CNF_VendorTable.HostEntries[i].noOfPeers; j++)
      (*HostNameArray)[j] = CNF_VendorTable.HostEntries[i].Peers[j].HostName;

   return(CNF_VendorTable.HostEntries[i].noOfPeers);
}

OFBool DcmQueryRetrieveConfig::writableStorageArea(const char *aeTitle) const
{
    const char *axs = getAccess((char*)aeTitle);
    if (strcmp(axs, "RW") == 0) return OFTrue;
    if (strcmp(axs, "WR") == 0) return OFTrue;
    if (strcmp(axs, "W") == 0) return OFTrue;
    return OFFalse;
}

const char *DcmQueryRetrieveConfig::getUserName() const
{
   return UserName_.c_str();
}

const char *DcmQueryRetrieveConfig::getGroupName() const
{
   return GroupName_.c_str();
}

/*
 * CVS Log
 * $Log: dcmqrcnf.cc,v $
 * Revision 1.14  2010-12-15 13:59:58  uli
 * Fixed a problem with a missing prototype for vsnprintf on HP-UX.
 *
 * Revision 1.13  2010-10-20 07:41:36  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.12  2010-10-14 13:14:35  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.11  2010-09-09 17:20:42  joergr
 * Removed unused (or never used?) configuration entries.
 *
 * Revision 1.10  2010-09-09 16:54:32  joergr
 * Further code clean-up and minor changes to log messages.
 *
 * Revision 1.9  2010-06-25 09:15:19  uli
 * Fixed issues with compiling with HAVE_STD_STRING.
 *
 * Revision 1.8  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.7  2009-08-21 09:54:11  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.6  2005/12/16 13:10:24  meichel
 * Added type safety code for 64bit platforms
 *
 * Revision 1.5  2005/12/14 14:29:42  joergr
 * Including ctype if present, needed for Solaris.
 *
 * Revision 1.4  2005/12/08 15:47:08  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2005/04/07 14:38:22  joergr
 * Initialize member variables for user and group name.
 *
 * Revision 1.2  2005/04/04 13:15:13  meichel
 * Added username/groupname configuration option that allows to start the
 *   image database as root and let it call setuid/setgid to execute under an
 *   unprivileged account once the listen socket has been opened.
 *
 * Revision 1.1  2005/03/30 13:34:53  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */

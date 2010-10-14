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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:41 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMQRCNF_H
#define DCMQRCNF_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/oflog/oflog.h"

OFLogger DCM_dcmqrdbGetLogger();

#define DCMQRDB_TRACE(msg) OFLOG_TRACE(DCM_dcmqrdbGetLogger(), msg)
#define DCMQRDB_DEBUG(msg) OFLOG_DEBUG(DCM_dcmqrdbGetLogger(), msg)
#define DCMQRDB_INFO(msg)  OFLOG_INFO(DCM_dcmqrdbGetLogger(), msg)
#define DCMQRDB_WARN(msg)  OFLOG_WARN(DCM_dcmqrdbGetLogger(), msg)
#define DCMQRDB_ERROR(msg) OFLOG_ERROR(DCM_dcmqrdbGetLogger(), msg)
#define DCMQRDB_FATAL(msg) OFLOG_FATAL(DCM_dcmqrdbGetLogger(), msg)

/** this class describes configuration settings for the quota of a storage area
 */
struct DcmQueryRetrieveConfigQuota
{
    /// maximum number of studies
    int  maxStudies;
    /// maximum number of bytes per study
    long maxBytesPerStudy;
};

/** this class describes configuration settings for a remote SCP peer
 */
struct DcmQueryRetrieveConfigPeer
{
    /// remote peer AE title
    const char *ApplicationTitle;

    /// remote peer host name
    const char *HostName;

    /// remote peer port number
    int PortNumber;
};

/** this class describes configuration settings for a single storage area
 */
struct DcmQueryRetrieveConfigAEEntry
{
    /// application entity title
    const char *ApplicationTitle;

    /// name of storage area
    const char *StorageArea;

    /// access type (read-only, read/write)
    const char *Access;

    /// quota setting for this storage area
    DcmQueryRetrieveConfigQuota *StorageQuota;

    /// number of peer entries
    int noOfPeers;

    /// array of peer entries describing remote SCP peers
    DcmQueryRetrieveConfigPeer *Peers;
};

/** this class describes configuration settings for a list of storage areas
 */
struct DcmQueryRetrieveConfigConfiguration
{
    /// number of storage areas (aetitles)
    int noOfAEEntries;

    /// array of entries for each storage area
    DcmQueryRetrieveConfigAEEntry *AEEntries;
};

/** this class describes configuration settings for one symbolic host or vendor
 */
struct DcmQueryRetrieveConfigHostEntry
{
    /// symbolic name of host
    const char *SymbolicName;

    /// number of peer entries
    int noOfPeers;

    /// array of peer entries describing remote SCP peers
    DcmQueryRetrieveConfigPeer *Peers;
};

/** this class describes configuration settings for a list of symbolic hosts or vendors
 */
struct DcmQueryRetrieveConfigHostTable
{
    /// number of entries
    int noOfHostEntries;

    /// array of entries
    DcmQueryRetrieveConfigHostEntry   *HostEntries;
};

/** this class describes configuration settings for a Query/Retrieve SCP Service
 */
class DcmQueryRetrieveConfig
{

public:

  /*
   *  read configuration file and initialize the
   *  intern configuration structure
   *  Input : configuration file name
   *  Return : 1 - ok
   *     0 - error
   */
  int init(const char *ConfigurationFile);

  /*
   *  search for peer with AETitle
   *  Input : AETitle
   *  Ouput : Host Name, Port Number
   *  Return : 1 - found in AETable
   *     2 - found in HostTable
   *     0 - not found
   */
  int peerForAETitle(const char *AETitle, const char **HostName, int *PortNumber) const;

  /*
   *  check if given AETitles exist in same
   *  Vendor Group
   *  Input : two AETitles
   *  Return : 1 - same group
   *     0 - else
   */
  int checkForSameVendor(const char *AETitle1, const char *AETitle2) const;

  /*
   *  get Storage Area for AETitle
   *  Input : AETitle
   *  Return : Storage Area
   */
  const char *getStorageArea(const char *AETitle) const;

  /*
   *  get Number of Maximal Studies
   *  Input : AETitle
   *  Return : Number of Maximal Studies
   */
  int getMaxStudies(const char *AETitle) const;

  /*
   *  get Number of maximal Bytes per Study
   *  Input : AETitle
   *  Return : Number of maximal Bytes per Study
   */
  long getMaxBytesPerStudy(const char *AETitle) const;

  /*
   *  get Max Associations
   *  Input :
   *  Return : Max Associations
   */
  int getMaxAssociations() const;

  /*
   *  get Network TCP Port
   *  Input :
   *  Return : Network TCP Port
   */
  int getNetworkTCPPort() const;

  /*
   *  get Max PDU Size
   *  Input :
   *  Return : Max PDU Size
   */
  OFCmdUnsignedInt getMaxPDUSize() const;

  /*
   *  check if there is an peer with calling AETitle
   *  on HostName
   *  Input : called AETitle, calling AETitle, Host Name
   *  Return : 1 -- yes
   *     0 -- no
   */
  int peerInAETitle(const char *calledAETitle, const char *callingAETitle, const char *HostName) const;

  /*
   *  get Access mode
   *  Input : AETitle
   *  Return : Access mode
   */
  const char *getAccess(const char *AETitle) const;

  /*
   *  check if given storage area is read/write
   *  Input : AETitle
   *  Return : true if storage area is writable
   */
  OFBool writableStorageArea(const char *aeTitle) const;

  // methods only used by TI

  /*
   *  searches in the host table for all AE titles
   *  known for peer hostName.  Creates an array of string pointers
   *  containing the known AE titles.  The AE titles contained
   *  in the array are privately owned by the config facility (you
   *  may not free them).  You may free the array when no longer needed.
   *
   *  Input Parameter: peer host name
   *  Output Parameter: malloc'ed array of private string pointers.
   *  Returns : number of entries in the malloced array.
   *      0 if no entries found.
   */

  int aeTitlesForPeer(const char *hostName, const char *** aeTitleList) const;

  /*
   *  Creates an array of string pointers
   *  containing the known AE titles for CTN storage areas.
   *  The AE titles contained in the array are privately owned
   *  by the config facility (you may not free them).  You may
   *  free the array when no longer needed.
   *
   *  Output Parameter: malloc'ed array of private string pointers.
   *  Returns : number of entries in the malloced array.
   *      0 if no entries exist.
   */

  int ctnTitles(const char *** ctnTitleList) const;

  /*
   *  Creates an array of string pointers
   *  containing the kown Host Names for given Vendor Name.
   *  The Host Names contained in the array are privately owned
   *  by the config facility (you may not free them). You may
   *  free the array when no longer needed.
   *  Input : Vendor Name
   *  Ouput : array of string pointers
   *  Return : number of entries in array
   *     0 if no entries exist
   */
  int HostNamesForVendor(const char *Vendor, const char ***HostNameArray) const;

  /*
   *  searches in the host table for all AE titles
   *  known for a symbolic name.  Creates an array of string pointers
   *  containing the known AE titles.  The AE titles contained
   *  in the array are privately owned by the config facility (you
   *  may not free them).  You may free the array when no longer needed.
   *
   *  Input Parameter: symbolic name
   *  Output Parameter: malloc'ed array of private string pointers.
   *  Returns : number of entries in the malloced array.
   *      0 if no entries found.
   */
  int aeTitlesForSymbolicName(const char *symbolicName, const char ***aeTitleList) const;

  /*
   *  printf contents of configuration stucture
   *  to stdout
   */
  void printConfig();

  /*
   *  get User Name
   *  Input :
   *  Return : User Name
   */
  const char *getUserName() const;

  /*
   *  get Group Name
   *  Input :
   *  Return : Group Name
   */
  const char *getGroupName() const;

private:

  const char* vendorForPeerAETitle(const char *peerAETitle) const;

  int countCtnTitles() const;


  /*
   *  initialize configuration storage structure
   */
  void initConfigStruct();

  /*
   *  read configuration file line by line
   *  Input : configuration file pointer
   *  Return : 1 - ok
   *     0 - error
   */
  int readConfigLines(FILE *cnffp);

  /*
   *  read HostTable in configuration file
   *  Input : configuration file pointer, line number
   *  Output : line number
   *  Return : 1 - ok
   *     0 - error
   */
  int readHostTable(FILE *cnffp, int *lineno);

  /*
   *  read VendorTable in configuration file
   *  Input : configuration file pointer, line number
   *  Output : line number
   *  Return : 1 - ok
   *     0 - error
   */
  int readVendorTable(FILE *cnffp, int *lineno);

  /*
   *  read AETable in configuration file
   *  Input : configuration file pointer, line number
   *  Output : line number
   *  Return : 1 - ok
   *     0 - error
   */
  int readAETable(FILE *cnffp, int *lineno);

  /*
   *  separate the peer list from value list
   *  Input : pointer to value list
   *  Output : number of peers
   *  Return : pointer to peer list
   */
  DcmQueryRetrieveConfigPeer *parsePeers(char **valuehandle, int *peers);

  /*
   *  extract peers from peer list
   *  Input : pointer to value list
   *  Output : number of peers
   *  Return : pointer to peer list
   */
  DcmQueryRetrieveConfigPeer *readPeerList(char **valuehandle, int *peers);

  /*
   *  separate a quota from value list
   *  Input : pointer to value list
   *  Return : pointer to quota structure
   */
  static DcmQueryRetrieveConfigQuota *parseQuota(char **valuehandle);

  /*
   *  check if character is white space or separator
   *  Input : character
   *  Return : 1 - yes
   *     0 - no
   */
  static int isgap (char gap);

  /*
   *  check if character is quote
   *  Input : character
   *  Return : 1 - yes
   *     0 - no
   */
  static int isquote (char quote);

  /*
   *  print a panic message to stderr
   *  Input : variable
   */
  static void panic(const char *fmt, ...);

  /*
   *  convert string to long
   *  Input : parameter string value
   *  Return : parameter as long
   *     -1 on error
   */
  static long quota (const char *value);

  /*
   *  skip mnemonic and first gap in rc line
   *  Input : rc line
   *  Return : pointer to value list
   */
  static char *skipmnemonic (char *rcline);

  /*
   *  separate on value from value list
   *  Input : pointer to value list
   *  Return : pointer to next value
   */
  static char *parsevalues (char **valuehandle);

  /* Configuration Parameters */
  OFString UserName_;
  OFString GroupName_;
  int networkTCPPort_;
  Uint32 maxPDUSize_;
  int maxAssociations_;
  DcmQueryRetrieveConfigConfiguration CNF_Config;   /* configuration file contents */
  DcmQueryRetrieveConfigHostTable CNF_HETable;      /* HostEntries Table */
  DcmQueryRetrieveConfigHostTable CNF_VendorTable;  /* Vendor Table */

};


#endif

/*
 * CVS Log
 * $Log: dcmqrcnf.h,v $
 * Revision 1.7  2010-10-14 13:16:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2010-09-09 17:20:33  joergr
 * Removed unused (or never used?) configuration entries.
 *
 * Revision 1.5  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.4  2009-08-21 09:50:07  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.3  2005/12/08 16:04:20  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2005/04/04 13:15:13  meichel
 * Added username/groupname configuration option that allows to start the
 *   image database as root and let it call setuid/setgid to execute under an
 *   unprivileged account once the listen socket has been opened.
 *
 * Revision 1.1  2005/03/30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */

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
 *  Author:  Andrew Hewett
 *
 *  Purpose: TI Common Constants, Types, Globals and Functions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:41 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMQRTIS_H
#define DCMQRTIS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmqrdb/dcmqrcnf.h"

class DcmQueryRetrieveDatabaseHandle;

/*
 * Constants
 */

#define TI_MAXPEERS       100
#define TI_MAXDATABASES   100
#define TI_MAXSTUDIES    1000
#define TI_MAXSERIES      500
#define TI_MAXIMAGES     1000

/*
 * Type definitions
 */

struct TI_ImageEntry
{
    DIC_UI  sopInstanceUID;
    DIC_IS  imageNumber;
    int   intImageNumber;
} ;

struct TI_SeriesEntry
{
    DIC_UI  seriesInstanceUID;
    DIC_IS  seriesNumber;
    int   intSeriesNumber;
    DIC_CS  modality;
    TI_ImageEntry *images[TI_MAXIMAGES];  /* array of image pointers */
    int     imageCount;

    time_t lastQueryTime; /* time we last queried db */
};

struct TI_StudyEntry
{
    DIC_UI  studyInstanceUID;
    DIC_CS  studyID;
    DIC_PN  patientName;
    DIC_LO  patientID;
    TI_SeriesEntry  *series[TI_MAXSERIES];  /* array of series pointers */
    int     seriesCount;
    time_t lastQueryTime; /* time we last queried db */
};


struct TI_DBEntry
{
    const char *title;  /* the CTN AE Title associated with this DB */

    const char **peerTitles;  /* peer titles which can read this database
       * and thus we can comminicate with */
    int peerTitleCount; /* number of peer titles */

    DcmQueryRetrieveDatabaseHandle *dbHandle;  /* handle to current db */

    TI_StudyEntry *studies[TI_MAXSTUDIES]; /* array of study pointers */
    int     studyCount;

    int currentStudy; /* index of current study */
    int currentSeries;  /* index of current series in current study */
    int currentImage; /* index of current image in current study */

    time_t lastQueryTime; /* time we last queried db */

    OFBool isRemoteDB;  /* true if DB is remote */
};

struct TI_GenericCallbackStruct
{
    TI_DBEntry *db;
    TI_StudyEntry *study;
    TI_SeriesEntry *series;
};

typedef OFBool (*TI_GenericEntryCallbackFunction)(TI_GenericCallbackStruct *cbstruct, DcmDataset *reply);

/** this class provides the functionality of the telnet initiator application
 */
class DcmQueryRetrieveTelnetInitiator
{
public:

    /** constructor
     *  @param cfg configuration facility
     */
    DcmQueryRetrieveTelnetInitiator(DcmQueryRetrieveConfig &cfg);

    /** main entry point for console-based user interface
     */
    void TI_userInput();

    /** add remote peer to list of peers
     *  @param peerName name of peer
     *  @param configFileName name of configuration file from which peer was read
     */
    OFBool addPeerName(const char *peerName, const char *configFileName);

    /** print TI configuration to stdout
     */
    void printConfig();

    /** detach current association
     *  @param abortFlag if true, abort association instead of releasing it
     */
    OFBool TI_detachAssociation(OFBool abortFlag);

    /** set local aetitle
     *  @param ae aetitle
     */
    void setAETitle(const char *ae)
    {
      myAETitle = ae;
    }

    /** set max receive PDU
     *  @param pdu max receive PDU size
     */
    void setMaxPDU(OFCmdUnsignedInt pdu)
    {
      maxReceivePDULength = pdu;
    }

    /** activate first peer in list of peers
     */
    void activateFirstPeer()
    {
      peerHostName = peerNames[0];
    }

    /** provide read/write access to network structure maintained by this object.
     *  Yes, this is ugly.
     *  @return pointer to pointer to network structure
     */
    T_ASC_Network **accessNet()
    {
      return &net;
    }

    /// return number of databases
    int getdbCount() const
    {
      return dbCount;
    }

    /** create configuration entries for remote databases
     *  @param configFileName name of configuration file
     *  @param remoteDBTitlesCount number of remote DB titles
     *  @param remoteDBTitles list of remote DB titles
     */
    void createConfigEntries(
      const char *configFileName,
      int remoteDBTitlesCount,
      const char **remoteDBTitles);

    /** set the network transfer syntax
     *  @param xfer new network transfer syntax
     */
    void setXferSyntax(E_TransferSyntax xfer) { networkTransferSyntax = xfer; }

    /** set blocking mode and timeout for DIMSE operations
     *  @param blockMode blocking mode for DIMSE operations
     *  @param timeout timeout for DIMSE operations
     */
    void setBlockMode(T_DIMSE_BlockingMode blockMode, int timeout)
    {
      blockMode_ = blockMode;
      dimse_timeout_ = timeout;
    }

private:

    OFBool TI_attachAssociation();
    OFBool TI_changeAssociation();
    OFBool TI_sendEcho();
    OFBool TI_storeImage(char *sopClass, char *sopInstance, char * imgFile);
    OFBool TI_remoteFindQuery(
        TI_DBEntry *db, DcmDataset *query,
        TI_GenericEntryCallbackFunction callbackFunction,
        TI_GenericCallbackStruct *callbackData);
    OFBool TI_title(int arg, const char * /*cmdbuf*/ );
    OFBool TI_attachDB(TI_DBEntry *db);
    OFBool TI_database(int arg, const char * /*cmdbuf*/ );
    OFBool TI_echo(int arg, const char * /*cmdbuf*/ );
    OFBool TI_quit(int arg, const char * /*cmdbuf*/ );
    OFBool TI_actualizeStudies();
    OFBool TI_study(int arg, const char * /*cmdbuf*/ );
    OFBool TI_actualizeSeries();
    OFBool TI_series(int arg, const char * /*cmdbuf*/ );
    OFBool TI_actualizeImages();
    OFBool TI_image(int arg, const char * /*cmdbuf*/ );
    OFBool TI_buildStudies(TI_DBEntry *db);
    OFBool TI_buildSeries(TI_DBEntry *db, TI_StudyEntry *study);
    OFBool TI_buildRemoteImages(TI_DBEntry *db, TI_StudyEntry *study, TI_SeriesEntry *series);
    OFBool TI_buildImages(TI_DBEntry *db, TI_StudyEntry *study, TI_SeriesEntry *series);
    OFBool TI_sendStudy(int arg, const char * /*cmdbuf*/ );
    OFBool TI_sendSeries(int arg, const char * /*cmdbuf*/ );
    OFBool TI_sendImage(int arg, const char * /*cmdbuf*/ );
    OFBool TI_send(int /*arg*/, const char *cmdbuf);
    OFBool TI_shortHelp(int /*arg*/ , const char * /*cmdbuf*/ );
    OFBool TI_help(int arg, const char * /*cmdbuf*/ );
    OFBool TI_buildRemoteStudies(TI_DBEntry *db);
    OFBool TI_buildRemoteSeries(TI_DBEntry *db, TI_StudyEntry *study);
    OFBool TI_dbReadable(const char *dbTitle);
    time_t TI_dbModifyTime(const char *dbTitle);
    OFCondition addPresentationContexts(T_ASC_Parameters *params);

    OFBool findDBPeerTitles(
      const char *configFileName,
      TI_DBEntry *dbEntry,
      const char *peer);

    /// the CTN databases we know
    TI_DBEntry **dbEntries;

    /// number of entries in databases we know
    int dbCount;

    /// current peer to talk to
    const char *peerHostName;

    /// list of peer names
    const char *peerNames[TI_MAXPEERS];

    /// number of peer names in list
    int peerNamesCount;

    /// my application entity title
    const char *myAETitle;

    /// active network
    T_ASC_Network *net;

    /// currently active association
    T_ASC_Association *assoc;

    /// number of bytes per PDU we can receive
    OFCmdUnsignedInt maxReceivePDULength;

    /// current database index
    int currentdb;

    /// current peer title
    const char *currentPeerTitle;

    /// configuration facility
    DcmQueryRetrieveConfig& config;

    /// network transfer syntax
    E_TransferSyntax networkTransferSyntax;

    /// blocking mode for DIMSE operations
    T_DIMSE_BlockingMode blockMode_;

    /// timeout for DIMSE operations
    int dimse_timeout_;

};


#endif

/*
 * CVS Log
 * $Log: dcmqrtis.h,v $
 * Revision 1.8  2010-10-14 13:16:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2010-08-09 13:23:00  joergr
 * Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 * the official "keyword" is used for the attribute name which results in a
 * number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 * Revision 1.6  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.5  2009-08-21 09:50:07  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.4  2005/12/08 16:04:28  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2005/11/17 13:44:37  meichel
 * Added command line options for DIMSE and ACSE timeouts
 *
 * Revision 1.2  2005/06/16 08:03:51  meichel
 * Fixed typo in method name
 *
 * Revision 1.1  2005/03/30 13:34:50  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */

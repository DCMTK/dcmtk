/*
 *
 *  Copyright (C) 1994-2007, OFFIS
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg / Andrew Hewett
 *
 *  Purpose: Classes for Query/Retrieve Service Class User (C-FIND operation)
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2009-07-08 16:14:32 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dfindscu.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dfindscu.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/ofstd/ofconapp.h"

/* ---------------- static functions ---------------- */

#define OFFIS_CONSOLE_APPLICATION "findscu"

static void progressCallback(
  void *callbackData,
  T_DIMSE_C_FindRQ *request,
  int responseCount,
  T_DIMSE_C_FindRSP *rsp,
  DcmDataset *responseIdentifiers)
{
  DcmFindSCUDefaultCallback *callback = OFreinterpret_cast(DcmFindSCUDefaultCallback *, callbackData);
  if (callback) callback->callback(request, responseCount, rsp, responseIdentifiers);
}

/* ---------------- class DcmFindSCUCallback ---------------- */

DcmFindSCUCallback::DcmFindSCUCallback()
: assoc_(NULL)
, presId_(0)
{
}
  
void DcmFindSCUCallback::setAssociation(T_ASC_Association *assoc)
{
  assoc_ = assoc;
}

void DcmFindSCUCallback::setPresentationContextID(T_ASC_PresentationContextID presId)
{
  presId_ = presId;
}

/* ---------------- class DcmFindSCUCallback ---------------- */

DcmFindSCUDefaultCallback::DcmFindSCUDefaultCallback(
    OFBool extractResponsesToFile,
    int cancelAfterNResponses,
    OFBool verbose)	
: DcmFindSCUCallback()
, extractResponsesToFile_(extractResponsesToFile)
, cancelAfterNResponses_(cancelAfterNResponses)
, verbose_(verbose)
{
}

void DcmFindSCUDefaultCallback::callback(
        T_DIMSE_C_FindRQ *request,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers)
 {
    /* dump response number */
    STD_NAMESPACE ostream& mycout = ofConsole.lockCout();
    mycout << "RESPONSE: " << responseCount << " (" << DU_cfindStatusString(rsp->DimseStatus) << ")\n";

    /* dump data set which was received */
    responseIdentifiers->print(mycout);
   
    /* dump delimiter */
    mycout << "--------" << OFendl;
    ofConsole.unlockCout();
    
    /* in case extractResponsesToFile is set the responses shall be extracted to a certain file */
    if (extractResponsesToFile_) {
        char rspIdsFileName[1024];
        sprintf(rspIdsFileName, "rsp%04d.dcm", responseCount);
        DcmFindSCU::writeToFile(rspIdsFileName, responseIdentifiers);
    }

    /* should we send a cancel back ?? */
    if (cancelAfterNResponses_ == responseCount)
    {
        if (verbose_)
        {
            ofConsole.lockCout() << "Sending Cancel RQ, MsgId: " << request->MessageID << ", PresId: " << presId_ << OFendl;
            ofConsole.unlockCout();
        }
        OFCondition cond = DIMSE_sendCancelRequest(assoc_, presId_, request->MessageID);
        if (cond.bad())
        {
            ofConsole.lockCerr() << "Cancel RQ Failed:" << OFendl;
            ofConsole.unlockCerr();
            DimseCondition::dump(cond);
        }
    }
}

/* ---------------- class DcmFindSCU ---------------- */


DcmFindSCU::DcmFindSCU(OFBool verboseMode, OFBool debugMode)
: net_(NULL), verbose_(verboseMode), debug_(debugMode)
{
}

DcmFindSCU::~DcmFindSCU()
{ 
  dropNetwork(); 
}



OFCondition DcmFindSCU::addOverrideKey(DcmDataset *dataset, 
                                       const OFString& pathParam)
{
  if (dataset == NULL) return EC_IllegalCall;
  if (pathParam.empty()) return EC_Normal;
  OFString path = pathParam;
  OFString value;
  OFBool valueSpecified = OFFalse;
  size_t pos = path.find('=');
  // separate tag from value if there is one
  if (pos != OFString_npos)
  {
    value = path.substr(pos+1); // value now contains value
    path.erase(pos);            // pure path without value
    valueSpecified = OFTrue;
  }
  DcmPathProcessor proc;
  /* disable item wildcards since they don't make sense for Q/R or worklist
     where always a single item is sent in the query. Further, do not check
     for private reservations in query dataset.
   */
  proc.setItemWildcardSupport(OFFalse);
  proc.checkPrivateReservations(OFFalse);
  // create path
  OFCondition result = proc.findOrCreatePath(dataset, path, OFTrue /* create if necessary */);
  if (result.bad())
    return result;
  OFList<DcmPath*> pathResults;
  // check for results (there must be some when creating the path successfully)
  Uint32 numResults = proc.getResults(pathResults);
  if (numResults == 0)
  {
    return EC_IllegalCall;
  }
  // if no value is specified, work is already done at this point
  if (!valueSpecified)
    return EC_Normal;

  // if value is specified, be sure path does not end with item
  OFListIterator(DcmPath*) it = pathResults.begin();
  DcmPathNode *last = (*it)->back();
  if (last == NULL) return EC_IllegalCall;
  if (! (last->m_obj->isLeaf()) )
    return makeOFCondition(OFM_dcmdata, 25, OF_error, "Cannot insert value into path ending with item or sequence");
  OFListConstIterator(DcmPath*) endList = pathResults.end();

  // Insert value into each element affected by path
  while (it != endList)
  {
    last = (*it)->back();
    if (last == NULL) return EC_IllegalCall;
    DcmElement *elem = OFstatic_cast(DcmElement*, last->m_obj);
    if (elem == NULL) return EC_IllegalCall;
    result = elem->putString(value.c_str());
    if (result.bad())
      break;
    it++;
  }
  return result;
}


OFCondition DcmFindSCU::initializeNetwork(int acse_timeout)
{
  return ASC_initializeNetwork(NET_REQUESTOR, 0, acse_timeout, &net_);
}

OFCondition DcmFindSCU::setTransportLayer(DcmTransportLayer *tLayer)
{
  return ASC_setTransportLayer(net_, tLayer, 0);
}

OFCondition DcmFindSCU::dropNetwork()
{
  if (net_) return ASC_dropNetwork(&net_); else return EC_Normal;
}

OFCondition DcmFindSCU::performQuery(
    const char *peer,
    unsigned int port,
    const char *ourTitle,
    const char *peerTitle,
    const char *abstractSyntax,
    E_TransferSyntax preferredTransferSyntax,
    T_DIMSE_BlockingMode blockMode,
    int dimse_timeout,
    Uint32 maxReceivePDULength,
    OFBool secureConnection,
    OFBool abortAssociation,
    unsigned int repeatCount,
    OFBool extractResponsesToFile,
    int cancelAfterNResponses,
    OFList<OFString> *overrideKeys,
    DcmFindSCUCallback *callback,
    OFList<OFString> *fileNameList)
{
    T_ASC_Association *assoc = NULL;
    T_ASC_Parameters *params = NULL;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;

    /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
    OFCondition cond = ASC_createAssociationParameters(&params, maxReceivePDULength);
    if (cond.bad()) return cond;

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, ourTitle, peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* structure. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, secureConnection);
    if (cond.bad()) return cond;

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", peer, (int)port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    cond = addPresentationContext(params, abstractSyntax, preferredTransferSyntax);

    if (cond.bad()) return cond;

    /* dump presentation contexts if required */
    if (debug_) {
        STD_NAMESPACE ostream& mycout = ofConsole.lockCout();
        mycout << "Request Parameters:\n";
        ASC_dumpParameters(params, mycout);
        ofConsole.unlockCout();
    }

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    if (verbose_)
        printf("Requesting Association\n");

    cond = ASC_requestAssociation(net_, params, &assoc);

    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters(params, &rej);

            STD_NAMESPACE ostream& mycerr = ofConsole.lockCerr();
            mycerr << "Association Rejected:" << OFendl;
            ASC_printRejectParameters(mycerr, &rej);
            ofConsole.unlockCerr();
            return cond;
        } else {
            ofConsole.lockCerr() << "Association Request Failed:" << OFendl;
            ofConsole.unlockCerr();
            return cond;
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    if (debug_) {
        STD_NAMESPACE ostream& mycout = ofConsole.lockCout();
        mycout << "Association Parameters Negotiated:\n";
        ASC_dumpParameters(params, mycout);
        ofConsole.unlockCout();
    }

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        ofConsole.lockCerr() << "No Acceptable Presentation Contexts" << OFendl;
        ofConsole.unlockCerr();
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* dump general information concerning the establishment of the network connection if required */
    if (verbose_) {
        printf("Association Accepted (Max Send PDV: %lu)\n", assoc->sendPDVLength);
    }

    /* do the real work, i.e. for all files which were specified in the command line, send a */
    /* C-FIND-RQ to the other DICOM application and receive corresponding response messages. */
    cond = EC_Normal;
    if ((fileNameList == NULL) || fileNameList->empty())
    {
        /* no files provided on command line */
        cond = findSCU(assoc, NULL, repeatCount, abstractSyntax, blockMode, dimse_timeout, extractResponsesToFile, cancelAfterNResponses, overrideKeys, callback);
    } else {
      OFListIterator(OFString) iter = fileNameList->begin();
      OFListIterator(OFString) enditer = fileNameList->end();
      while ((iter != enditer) && (cond.good())) // compare with EC_Normal since DUL_PEERREQUESTEDRELEASE is also good()
      {
          cond = findSCU(assoc, (*iter).c_str(), repeatCount, abstractSyntax, blockMode, dimse_timeout, extractResponsesToFile, cancelAfterNResponses, overrideKeys, callback);
          ++iter;
      }
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (abortAssociation) {
            if (verbose_)
                printf("Aborting Association\n");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad()) {
                ofConsole.lockCerr() << "Association Abort Failed:" << OFendl;
                ofConsole.unlockCerr();
                return cond;
            }
        } else {
            /* release association */
            if (verbose_)
                printf("Releasing Association\n");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                ofConsole.lockCerr() << "Association Release Failed:" << OFendl;
                ofConsole.unlockCerr();
                return cond;
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        ofConsole.lockCerr() << "Protocol Error: peer requested release (Aborting)" << OFendl;
        ofConsole.unlockCerr();
        if (verbose_)
            printf("Aborting Association\n");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            ofConsole.lockCerr() << "Association Abort Failed:" << OFendl;
            ofConsole.unlockCerr();
            return cond;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        if (verbose_) printf("Peer Aborted Association\n");
    }
    else
    {
        ofConsole.lockCerr() << "SCU Failed:" << OFendl;
        ofConsole.unlockCerr();
        DimseCondition::dump(cond);
        if (verbose_)
            printf("Aborting Association\n");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            ofConsole.lockCerr() << "Association Abort Failed:" << OFendl;
            ofConsole.unlockCerr();
            return cond;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    return cond;
}

OFCondition DcmFindSCU::addPresentationContext(
  T_ASC_Parameters *params, 
  const char *abstractSyntax,
  E_TransferSyntax preferredTransferSyntax)
{
    /*
    ** We prefer to use Explicitly encoded transfer syntaxes.
    ** If we are running on a Little Endian machine we prefer
    ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    ** Some SCP implementations will just select the first transfer
    ** syntax they support (this is not part of the standard) so
    ** organise the proposed transfer syntaxes to take advantage
    ** of such behaviour.
    **
    ** The presentation contexts proposed here are only used for
    ** C-FIND and C-MOVE, so there is no need to support compressed
    ** transmission.
    */

    const char* transferSyntaxes[] = { NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (preferredTransferSyntax) {
    case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
    case EXS_LittleEndianExplicit:
        /* we prefer Little Endian Explicit */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    case EXS_BigEndianExplicit:
        /* we prefer Big Endian Explicit */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
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

    return ASC_addPresentationContext(
        params, 1, abstractSyntax,
        transferSyntaxes, numTransferSyntaxes);
}


OFBool DcmFindSCU::writeToFile(const char* ofname, DcmDataset *dataset)
{
    /* write out as a file format */

    DcmFileFormat fileformat(dataset); // copies dataset
    OFCondition ec = fileformat.error();
    if (ec.bad()) {
        ofConsole.lockCerr() << "error writing file: " << ofname << ": " << ec.text() << OFendl;
        ofConsole.unlockCerr();
        return OFFalse;
    }

    ec = fileformat.saveFile(ofname, dataset->getOriginalXfer());
    if (ec.bad()) {
        ofConsole.lockCerr() << "error writing file: " << ofname << ": " << ec.text() << OFendl;
        ofConsole.unlockCerr();
        return OFFalse;
    }

    return OFTrue;
}


OFCondition DcmFindSCU::findSCU(
  T_ASC_Association * assoc, 
  const char *fname, 
  int repeatCount,  
  const char *abstractSyntax,
  T_DIMSE_BlockingMode blockMode, 
  int dimse_timeout, 
  OFBool extractResponsesToFile,
  int cancelAfterNResponses,
  OFList<OFString> *overrideKeys,
  DcmFindSCUCallback *callback) const
    /*
     * This function will read all the information from the given file
     * (this information specifies a search mask), figure out a corresponding
     * presentation context which will be used to transmit a C-FIND-RQ message
     * over the network to the SCP, and it will finally initiate the transmission
     * of data to the SCP.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     *   fname - [in] Name of the file which shall be processed.
     */
{
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmFileFormat dcmff;

    /* if there is a valid filename */
    OFCondition cond;
    if (fname != NULL) {

        /* read information from file (this information specifies a search mask). After the */
        /* call to DcmFileFormat::read(...) the information which is encapsulated in the file */
        /* will be available through the DcmFileFormat object. In detail, it will be available */
        /* through calls to DcmFileFormat::getMetaInfo() (for meta header information) and */
        /* DcmFileFormat::getDataset() (for data set information). */
        cond = dcmff.loadFile(fname);

        /* figure out if an error occured while the file was read*/
        if (cond.bad()) {
            ofConsole.lockCerr() << "Bad DICOM file: " << fname << ": " << cond.text() << OFendl;
            ofConsole.unlockCerr();
            return cond;
        }
    }

    /* replace specific keys by those in overrideKeys */
    OFListIterator(OFString) path = overrideKeys->begin();
    OFListConstIterator(OFString) endOfList = overrideKeys->end();
    DcmDataset* dset = dcmff.getDataset();
    while (path != endOfList)
    {
        cond = addOverrideKey(dset, *path);
        if (cond.bad()) {
            ofConsole.lockCerr() << "Bad override key/path: " << *path << ": " << cond.text() << OFendl;
            ofConsole.unlockCerr();
            return cond;
        }
        path++;
    }

    /* figure out which of the accepted presentation contexts should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, abstractSyntax);

    if (presId == 0) {
        ofConsole.lockCerr() << "No presentation context" << OFendl;
        ofConsole.unlockCerr();
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* repeatCount specifies how many times a certain file shall be processed */
    int n = repeatCount;

    /* prepare C-FIND-RQ message */
    bzero((char*)&req, sizeof(req));
    strcpy(req.AffectedSOPClassUID, abstractSyntax);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    /* prepare the callback data */
    DcmFindSCUDefaultCallback defaultCallback(extractResponsesToFile, cancelAfterNResponses, verbose_);
    if (callback == NULL) callback = &defaultCallback;
    callback->setAssociation(assoc);
    callback->setPresentationContextID(presId);

    /* as long as no error occured and the counter does not equal 0 */
    cond = EC_Normal;
    while (cond.good() && n--) 
    {
        DcmDataset *statusDetail = NULL;

        /* complete preparation of C-FIND-RQ message */
        req.MessageID = assoc->nextMsgID++;
        
        /* if required, dump some more general information */
        if (verbose_) {
            STD_NAMESPACE ostream& mycout = ofConsole.lockCout();
            mycout << "Find SCU RQ: MsgID " << req.MessageID << "\nREQUEST:\n";
            dcmff.getDataset()->print(mycout);
            mycout << "--------" << OFendl;
            ofConsole.unlockCout();
        }
        
        /* finally conduct transmission of data */
        OFCondition cond = DIMSE_findUser(assoc, presId, &req, dcmff.getDataset(),
            progressCallback, callback, blockMode, dimse_timeout,
            &rsp, &statusDetail);
        
        /* dump some more general information */
        if (cond.good()) {
            if (verbose_) {
                DIMSE_printCFindRSP(stdout, &rsp);
            } else {
                if (rsp.DimseStatus != STATUS_Success) {
                    printf("Response: %s\n", DU_cfindStatusString(rsp.DimseStatus));
                }
            }
        } else {
            if (fname) {
                ofConsole.lockCerr() << "Find Failed, file: " << fname << ":" << OFendl;
                ofConsole.unlockCerr();
            } else {
                STD_NAMESPACE ostream& mycerr = ofConsole.lockCerr();
                mycerr << "Find Failed, query keys" << OFendl;
                dcmff.getDataset()->print(mycerr);
                ofConsole.unlockCerr();                
            }
            DimseCondition::dump(cond);
        }
        
        /* dump status detail information if there is some */
        if (statusDetail != NULL) {
            STD_NAMESPACE ostream& mycout = ofConsole.lockCout();
            mycout << "  Status Detail:\n";
            statusDetail->print(mycout);
            ofConsole.unlockCout();
            delete statusDetail;
        }
    }

    /* return */
    return cond;
}


/*
 * CVS Log
 * $Log: dfindscu.cc,v $
 * Revision 1.4  2009-07-08 16:14:32  onken
 * Added support for specifying tag paths as override keys.
 *
 * Revision 1.3  2007-10-19 10:56:33  onken
 * Fixed bug in addOverrideKey() that caused  problems when parsing a value in a
 * tag-value combination if the value contained whitespace characters.
 *
 * Revision 1.2  2007/10/18 16:14:34  onken
 * - Fixed bug in addOverrideKey() that caused  problems when parsing a value in a
 * tag-value combination if the value contained whitespace characters.
 *
 * Revision 1.1  2007/02/19 13:13:28  meichel
 * Refactored findscu code into class DcmFindSCU, which is now part of the dcmnet
 *   library, and a short command line tool that only evaluates command line
 *   parameters and then makes use of this class. This facilitates re-use of the
 *   findscu code in other applications.
 *
 *
 */

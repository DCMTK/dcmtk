/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVSignatureHandler
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-01-26 10:43:14 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvsighdl.h"
#include "dcdeftag.h"
#include "dcmsign.h"
#include "dcobject.h"
#include "dcsequen.h"
#include "dcvrat.h"
#include "dvpscf.h"
#include "sicert.h"
#include "sitypes.h"

#ifdef HAVE_STRSTREAM_H
#include <strstream.h>
#endif

#ifdef HAVE_STRSTREA_H
#include <strstrea.h>
#endif

#ifdef WITH_OPENSSL
BEGIN_EXTERN_C
#include <openssl/x509.h>
END_EXTERN_C
#endif

DVSignatureHandler::DVSignatureHandler(DVConfiguration& cfg)
: htmlSR("<html><head><title>Structured Report</title></head><body>No structured report has been loaded/created yet.</body></html>\n")
, htmlImage("<html><head><title>Image</title></head><body>No image has been loaded yet.</body></html>\n")
, htmlPState("<html><head><title>Presentation State</title></head><body>No presentation state has been loaded/created yet.</body></html>\n")
, htmlOverview()
, correctSignaturesSR(0)
, corruptSignaturesSR(0)
, untrustSignaturesSR(0)
, correctSignaturesImage(0)
, corruptSignaturesImage(0)
, untrustSignaturesImage(0)
, correctSignaturesPState(0)
, corruptSignaturesPState(0)
, untrustSignaturesPState(0)
, certVerifier()
{
  int fileFormat = cfg.getTLSPEMFormat() ? X509_FILETYPE_PEM : X509_FILETYPE_ASN1;
  const char *tlsCACertificateFolder = cfg.getTLSCACertificateFolder();
  if (tlsCACertificateFolder) certVerifier.addTrustedCertificateDir(tlsCACertificateFolder, fileFormat);
  updateSignatureValidationOverview();
}


DVSignatureHandler::~DVSignatureHandler()
{  
}


void DVSignatureHandler::replaceString(DVPSObjectType objtype, const char *str)
{
  switch (objtype)
  {
    case DVPSS_structuredReport:
      if (str) htmlSR=str; else htmlSR.clear();
      break;
    case DVPSS_image:
      if (str) htmlImage=str; else htmlImage.clear();
      break;
    case DVPSS_presentationState:
      if (str) htmlPState=str; else htmlPState.clear();
      break;  
  }
}

void DVSignatureHandler::printSignatureItemPosition(DcmStack& stack, ostream& os)
{
  DcmObject *elem = NULL;
  DcmSequenceOfItems *sq = NULL;
  unsigned long sqCard=0;
  const char *tagname = NULL;
  unsigned long m=0;
  char buf[20];
  OFBool printed = OFFalse;
  
  if (stack.card() > 2)
  {      
    // signature is located within a sequence
    for (unsigned long l=stack.card()-2; l>0; --l) // loop over all elements except the stack top and bottom
    {
      elem = stack.elem(l);
      if (elem)
      {
        if ((elem->ident() == EVR_item) && sq)
        {
          sqCard = sq->card();
          for (m=0; m<sqCard; m++)
          {
            if (sq->getItem(m) == elem)
            {
              sprintf(buf, "[%lu]", m);
              os << buf;
              printed = OFTrue;
            }
          }
        }
        else 
        {
          if (printed) os << ". ";
          sq = (DcmSequenceOfItems *)elem;
          DcmTag currentTag(elem->getTag());
          tagname = currentTag.getTagName();
          if (tagname) os << tagname; else
          {
            sprintf(buf, "(%04x,%04x)", elem->getTag().getGroup(), elem->getTag().getElement());
            os << buf;
            printed = OFTrue;
          }
          if (elem->ident() == EVR_SQ) sq = (DcmSequenceOfItems *)elem; else sq = NULL;
        }
      }
    }
  } else {
    // signature is located in the main dataset
    os << "Main Dataset";
  }
}

void DVSignatureHandler::updateDigitalSignatureInformation(DcmItem& dataset, DVPSObjectType objtype, OFBool /* onRead */)
{
  SI_E_Condition sicond = SI_EC_Normal;
  DcmStack stack;
  DcmSignature signer;
  OFString aString;
  unsigned long counter = 0;
  unsigned long corrupt_counter = 0;
  unsigned long untrustworthy_counter = 0;
  unsigned long numSignatures = 0;
  unsigned long l=0;
  Uint16 macID = 0;
  DcmAttributeTag at(DCM_DataElementsSigned);  
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(dataset, stack);
  DcmTagKey tagkey;
  DcmTag tag;
  const char *tagName = NULL;
  ostrstream os;

  const char *htmlHead     = NULL;
  const char *htmlFoot     = "</body></html>\n\n";
  const char *htmlEndl     = "</td></tr>\n";
  const char *htmlTitle    = "<tr><td colspan=\"4\">";
  const char *htmlVfyOK    = "<tr><td colspan=\"4\" bgcolor=\"#50ff50\">";
  const char *htmlVfyCA    = "<tr><td colspan=\"4\" bgcolor=\"yellow\">";  
  const char *htmlVfyErr   = "<tr><td colspan=\"4\" bgcolor=\"#FF5050\">";  
  const char *htmlLine1    = "<tr><td width=\"20\" nowrap>&nbsp;</td><td colspan=\"2\" nowrap>";
  const char *htmlLine2    = "<tr><td colspan=\"3\" nowrap>&nbsp;</td><td>";
  const char *htmlLine3    = "<tr><td colspan=\"2\" nowrap>&nbsp;</td><td nowrap>";
  const char *htmlLine4    = "<tr><td width=\"20\" nowrap>&nbsp;</td><td width=\"20\" nowrap>&nbsp;</td><td>";
  const char *htmlNext     = "</td><td>";
  const char *htmlTableOK  = "<p><table cellspacing=\"0\" bgcolor=\"#D0FFD0\">\n";
  const char *htmlTableCA  = "<p><table cellspacing=\"0\" bgcolor=\"#FFF8DC\">\n";
  const char *htmlTableErr = "<p><table cellspacing=\"0\" bgcolor=\"#FFD0D0\">\n";
  const char *htmlTableE   = "</table></p>\n\n";

  switch (objtype)
  {
    case DVPSS_structuredReport:
      htmlHead = "<html>\n<head><title>Structured Report</title></head><body>\n";
      break;
    case DVPSS_image:
      htmlHead = "<html>\n<head><title>Image</title></head><body>\n";
      break;
    case DVPSS_presentationState:
      htmlHead = "<html>\n<head><title>Presentation State</title></head><body>\n";
      break;  
  }
  os << htmlHead;
  OFBool nextline;
  while (sigItem)
  {
    signer.attach(sigItem);
    numSignatures = signer.numberOfSignatures();
    for (l=0; l<numSignatures; l++)
    {
      if (SI_EC_Normal == signer.selectSignature(l))
      {
        ++counter;
        sicond = signer.verifyCurrent();
        SiCertificate *cert = signer.getCurrentCertificate();
        if ((sicond == SI_EC_Normal) && cert)
        {
          sicond = certVerifier.verifyCertificate(*cert);
        }
        
        if (sicond == SI_EC_Normal) os << htmlTableOK; 
        else if (sicond == SI_EC_VerificationFailed_NoTrust) os << htmlTableCA; 
        else os << htmlTableErr;

        os << htmlTitle <<   "<b>Signature #" << counter << " UID=";
        if (SI_EC_Normal == signer.getCurrentSignatureUID(aString)) os << aString.c_str(); else os << "(unknown)";
        os << "</b>" << htmlEndl;

        os << htmlLine1 << "Location" << htmlNext;
        printSignatureItemPosition(stack, os);        
        os << htmlEndl;

        os << htmlLine1 << "MAC ID" << htmlNext;
        if (SI_EC_Normal == signer.getCurrentMacID(macID)) os << macID; else os << "(unknown)";
        os << htmlEndl;

        os << htmlLine1 << "MAC algorithm" << htmlNext;
        if (SI_EC_Normal == signer.getCurrentMacName(aString)) os << aString.c_str(); else os << "(unknown)";
        os << htmlEndl;

        os << htmlLine1 << "MAC calculation xfer syntax" << htmlNext;
        if (SI_EC_Normal == signer.getCurrentMacXferSyntaxName(aString)) os << aString.c_str(); else os << "(unknown)";
        os << htmlEndl;

        os << htmlLine1 << "Data elements signed" << htmlNext;
        nextline = OFFalse;
        if (SI_EC_Normal == signer.getCurrentDataElementsSigned(at))
        {
          unsigned long atVM = at.getVM();
          for (unsigned long n=0; n<atVM; n++)
          {
            if (EC_Normal == at.getTagVal(tagkey, n))
            {
            	if (nextline) os << htmlLine2; else nextline=OFTrue;
            	os << tagkey << " ";
            	tag = tagkey;
            	tagName = tag.getTagName();
            	if (tagName) os << tagName;
            	os << htmlEndl;
            }
          }
        } else os << "all elements" << htmlEndl;

        os << htmlLine1 << "Signature date/time" << htmlNext;
        if (SI_EC_Normal == signer.getCurrentSignatureDateTime(aString)) os << aString.c_str(); else os << "(unknown)";
        os << htmlEndl
           << htmlLine1 << "Certificate of signer" << htmlNext;
        if ((cert == NULL)||(cert->getKeyType()==EKT_none)) os << "none" << htmlEndl; else
        {
          os << "X.509v" << cert->getX509Version() << htmlEndl;
          cert->getCertSubjectName(aString);
          os << htmlLine3 << "Subject" << htmlNext << aString.c_str() << htmlEndl;
          cert->getCertIssuerName(aString);
          os << htmlLine3 << "Issued by" << htmlNext << aString.c_str() << htmlEndl
             << htmlLine3 << "Serial no." << htmlNext << cert->getCertSerialNo() << htmlEndl
             << htmlLine3 << "Validity" << htmlNext << "not before ";
          cert->getCertValidityNotBefore(aString);
          os << aString.c_str() << ", not after ";
          cert->getCertValidityNotAfter(aString);
          os << aString.c_str() << htmlEndl
             << htmlLine4 << "Public key" << htmlNext;
          switch (cert->getKeyType())
          {
            case EKT_RSA:
              os << "RSA, " << cert->getCertKeyBits() << " bits" << htmlEndl;
              break;
            case EKT_DSA:
              os << "DSA, " << cert->getCertKeyBits() << " bits" << htmlEndl;
              break;
            case EKT_DH:
              os << "DH, " << cert->getCertKeyBits() << " bits" << htmlEndl;
              break;
            case EKT_none: // should never happen
              os << "none" << htmlEndl;
              break;
          }             
        }

        switch (sicond)
        {
          case SI_EC_Normal:
            os << htmlVfyOK << "Verification: OK" << htmlEndl;
            break;
          case SI_EC_VerificationFailed_NoTrust:
            untrustworthy_counter++;
            os << htmlVfyCA << "Verification: Signature is valid but certificate could not be verified: " 
               << certVerifier.lastError() << htmlEndl;
            break;
          default: 
            corrupt_counter++;
            os << htmlVfyErr << "Verification: ";
            os << siErrorConditionToString(sicond) << htmlEndl;
            break;
        }
        os << htmlTableE;
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(dataset, stack);
  }
  
  switch (objtype)
  {
    case DVPSS_structuredReport:
      if (counter == 0) os << "The current structured report does not contain any digital signature." << endl;
      corruptSignaturesSR = corrupt_counter;
      untrustSignaturesSR = untrustworthy_counter;  
      correctSignaturesSR = counter - corrupt_counter - untrustworthy_counter;
      break;
    case DVPSS_image:
      if (counter == 0) os << "The current image does not contain any digital signature." << endl;
      corruptSignaturesImage = corrupt_counter;
      untrustSignaturesImage = untrustworthy_counter;  
      correctSignaturesImage = counter - corrupt_counter - untrustworthy_counter;
      break;
    case DVPSS_presentationState:
      if (counter == 0) os << "The current presentation state does not contain any digital signature." << endl;
      corruptSignaturesPState = corrupt_counter;
      untrustSignaturesPState = untrustworthy_counter;  
      correctSignaturesPState = counter - corrupt_counter - untrustworthy_counter;
      break;  
  }
  os << htmlFoot;
  char *newText = os.str();
  replaceString(objtype, newText); // copies newText into OFString
  delete[] newText;
  updateSignatureValidationOverview();
  return;
}


void DVSignatureHandler::disableDigitalSignatureInformation(DVPSObjectType objtype)
{
  switch (objtype)
  {
    case DVPSS_structuredReport:
      htmlSR = "<html><head><title>Structured Report</title></head><body>The current structured report does not contain any digital signature.</body></html>\n";
      break;
    case DVPSS_image:
      htmlImage = "<html><head><title>Image</title></head><body>The current image does not contain any digital signature.</body></html>\n";
      break;
    case DVPSS_presentationState:
      htmlPState = "<html><head><title>Presentation State</title></head><body>The current presentation state does not contain any digital signature.</body></html>\n";
      break;  
  }
  updateSignatureValidationOverview();
}


DVPSSignatureStatus DVSignatureHandler::getCurrentSignatureStatus(DVPSObjectType objtype) const
{
  switch (objtype)
  {
    case DVPSS_structuredReport:
      if ((correctSignaturesSR + corruptSignaturesSR + untrustSignaturesSR) == 0) return DVPSW_unsigned;
      if ((corruptSignaturesSR + untrustSignaturesSR) == 0) return DVPSW_signed_OK;
      if (corruptSignaturesSR == 0) return DVPSW_signed_unknownCA;
      break;
    case DVPSS_image:
      if ((correctSignaturesImage + corruptSignaturesImage + untrustSignaturesImage) == 0) return DVPSW_unsigned;
      if ((corruptSignaturesImage + untrustSignaturesImage) == 0) return DVPSW_signed_OK;
      if (corruptSignaturesImage == 0) return DVPSW_signed_unknownCA;
      break;
    case DVPSS_presentationState:
      if ((correctSignaturesPState + corruptSignaturesPState + untrustSignaturesPState) == 0) return DVPSW_unsigned;
      if ((corruptSignaturesPState + untrustSignaturesPState) == 0) return DVPSW_signed_OK;
      if (corruptSignaturesPState == 0) return DVPSW_signed_unknownCA;
      break;
  }
  return DVPSW_signed_corrupt;
}

DVPSSignatureStatus DVSignatureHandler::getCombinedImagePStateSignatureStatus() const
{
  DVPSSignatureStatus statImage = getCurrentSignatureStatus(DVPSS_image);
  DVPSSignatureStatus statPState = getCurrentSignatureStatus(DVPSS_presentationState);
  if ((statImage == DVPSW_signed_corrupt)||(statPState == DVPSW_signed_corrupt)) return DVPSW_signed_corrupt;
  if ((statImage == DVPSW_signed_unknownCA)||(statPState == DVPSW_signed_unknownCA)) return DVPSW_signed_unknownCA;
  if ((statImage == DVPSW_signed_OK)&&(statPState == DVPSW_signed_OK)) return DVPSW_signed_OK;
  return DVPSW_unsigned;
}

const char *DVSignatureHandler::getCurrentSignatureValidationHTML(DVPSObjectType objtype) const
{
  const char *result = "";
  switch (objtype)
  {
    case DVPSS_structuredReport:
      result = htmlSR.c_str();
      break;
    case DVPSS_image:
      result = htmlImage.c_str();
      break;
    case DVPSS_presentationState:
      result = htmlPState.c_str();
      break;
  }
  return result;
}

void DVSignatureHandler::updateSignatureValidationOverview()
{
  const char *htmlHead     = "<html>\n<head><title>Overview</title></head><body>\n";
  const char *htmlFoot     = "</body></html>\n\n";
  const char *htmlEndl     = "</td></tr>\n";
  const char *htmlTitle    = "<tr><td colspan=\"2\">";
  const char *htmlVfyUns   = "<tr><td colspan=\"2\" bgcolor=\"#A0A0A0\">";
  const char *htmlVfySig   = "<tr><td colspan=\"2\" bgcolor=\"#50ff50\">";
  const char *htmlVfyCA    = "<tr><td colspan=\"2\" bgcolor=\"yellow\">";
  const char *htmlVfyErr   = "<tr><td colspan=\"2\" bgcolor=\"#FF5050\">";  
  const char *htmlLine1    = "<tr><td width=\"20\" nowrap>&nbsp;</td><td nowrap>";
  const char *htmlNext     = "</td><td>";
  const char *htmlTableUns = "<p><table cellspacing=\"0\" bgcolor=\"#E0E0E0\">\n";
  const char *htmlTableSig = "<p><table cellspacing=\"0\" bgcolor=\"#FFD0D0\">\n";
  const char *htmlTableCA  = "<p><table cellspacing=\"0\" bgcolor=\"#FFF8DC\">\n";
  const char *htmlTableErr = "<p><table cellspacing=\"0\" bgcolor=\"#FFD0D0\">\n";
  const char *htmlTableE   = "</table></p>\n\n";

  ostrstream os;
  DVPSSignatureStatus status;  
  os << htmlHead;

  // Structured Report
  status = getCurrentSignatureStatus(DVPSS_structuredReport);
  switch (status)
  {
    case DVPSW_unsigned:
      os << htmlTableUns;
      break;
    case DVPSW_signed_OK:
      os << htmlTableSig;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlVfyCA;
      break;
    case DVPSW_signed_corrupt:
      os << htmlTableErr;
      break;
  }
  os << htmlTitle << "<b>Structured Report</b>"<< htmlEndl;
  os << htmlLine1 << "Number of correct signatures" << htmlNext << correctSignaturesSR << htmlEndl;
  os << htmlLine1 << "Number of corrupt signatures" << htmlNext << corruptSignaturesSR << htmlEndl;
  os << htmlLine1 << "Number of untrusted signatures" << htmlNext << untrustSignaturesSR << htmlEndl;
  switch (status)
  {
    case DVPSW_unsigned:
      os << htmlVfyUns << "Status: unsigned" << htmlEndl;
      break;
    case DVPSW_signed_OK:
      os << htmlVfySig << "Status: signed" << htmlEndl;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlVfyCA  << "Status: signed but untrustworthy: certificate could not be verified" << htmlEndl;
      break;
    case DVPSW_signed_corrupt:
      os << htmlVfyErr << "Status: contains corrupt signatures" << htmlEndl;
      break;
  }
  os << htmlTableE;

  // Image
  status = getCurrentSignatureStatus(DVPSS_image);
  switch (status)
  {
    case DVPSW_unsigned:
      os << htmlTableUns;
      break;
    case DVPSW_signed_OK:
      os << htmlTableSig;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlTableCA;
      break;
    case DVPSW_signed_corrupt:
      os << htmlTableErr;
      break;
  }
  os << htmlTitle << "<b>Image</b>"<< htmlEndl;
  os << htmlLine1 << "Number of correct signatures" << htmlNext << correctSignaturesImage << htmlEndl;
  os << htmlLine1 << "Number of corrupt signatures" << htmlNext << corruptSignaturesImage << htmlEndl;
  os << htmlLine1 << "Number of untrusted signatures" << htmlNext << untrustSignaturesImage << htmlEndl;
  switch (status)
  {
    case DVPSW_unsigned:
      os << htmlVfyUns << "Status: unsigned" << htmlEndl;
      break;
    case DVPSW_signed_OK:
      os << htmlVfySig << "Status: signed" << htmlEndl;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlVfyCA  << "Status: signed but untrustworthy: certificate could not be verified" << htmlEndl;
      break;
    case DVPSW_signed_corrupt:
      os << htmlVfyErr << "Status: contains corrupt signatures" << htmlEndl;
      break;
  }
  os << htmlTableE;
  
  // Presentation State
  status = getCurrentSignatureStatus(DVPSS_presentationState);
  switch (status)
  {
    case DVPSW_unsigned:
      os << htmlTableUns;
      break;
    case DVPSW_signed_OK:
      os << htmlTableSig;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlTableCA;
      break;
    case DVPSW_signed_corrupt:
      os << htmlTableErr;
      break;
  }
  os << htmlTitle << "<b>Structured Report</b>"<< htmlEndl;
  os << htmlLine1 << "Number of correct signatures" << htmlNext << correctSignaturesPState << htmlEndl;
  os << htmlLine1 << "Number of corrupt signatures" << htmlNext << corruptSignaturesPState << htmlEndl;
  os << htmlLine1 << "Number of untrusted signatures" << htmlNext << untrustSignaturesPState << htmlEndl;
  switch (status)
  {
    case DVPSW_unsigned:
      os << htmlVfyUns << "Status: unsigned" << htmlEndl;
      break;
    case DVPSW_signed_OK:
      os << htmlVfySig << "Status: signed" << htmlEndl;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlVfyCA  << "Status: signed but untrustworthy: certificate could not be verified" << htmlEndl;
      break;
    case DVPSW_signed_corrupt:
      os << htmlVfyErr << "Status: contains corrupt signatures" << htmlEndl;
      break;
  }
  os << htmlTableE;
  os << htmlFoot;

  char *newText = os.str();
  htmlOverview = newText;
  delete[] newText;
  return;  
}

const char *DVSignatureHandler::getCurrentSignatureValidationOverview() const
{
  return htmlOverview.c_str();
}

/*
 *  $Log: dvsighdl.cc,v $
 *  Revision 1.2  2001-01-26 10:43:14  meichel
 *  Introduced additional (fourth) status flag for signature validation
 *    describing signatures that are valid but untrustworthy (unknown CA).
 *
 *  Revision 1.1  2001/01/25 15:18:10  meichel
 *  Added initial support for verification of digital signatures
 *    in presentation states, images and structured reports to module dcmpstat.
 *
 *
 */


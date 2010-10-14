/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVSignatureHandler
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:33 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvsighdl.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmsign/dcsignat.h"
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmpstat/dvpscf.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmsign/sitypes.h"
#include "dcmtk/dcmsign/sinullpr.h"
#include "dcmtk/dcmsign/siprivat.h"
#include "dcmtk/dcmsign/siripemd.h"

#include "dcmtk/ofstd/ofstream.h"

#ifdef WITH_OPENSSL
BEGIN_EXTERN_C
#include <openssl/x509.h>
END_EXTERN_C

/// the signature profile class we're using with DICOMscope 
class DVSignatureHandlerSignatureProfile: public SiNullProfile
{
public:

  /// default constructor
  DVSignatureHandlerSignatureProfile(DcmAttributeTag& at)
  : SiNullProfile()
  , notToSign(at)
  , vmNotToSign(notToSign.getVM())
  { 
  }

  /// destructor
  virtual ~DVSignatureHandlerSignatureProfile() { }

  /** checks whether the given tag is in the list of tags not to sign
   *  @param tag tag to check
   *  @return true if in list, false otherwise
   */
  OFBool tagInList(const DcmTagKey& tag) const
  {  
    DcmAttributeTag tempList(notToSign); // required because getTagVal() is not const
    DcmTagKey tagkey;
    for (unsigned long n=0; n<vmNotToSign; n++)
    {
      if ((EC_Normal == tempList.getTagVal(tagkey, n)) && (tag == tagkey)) return OFTrue;
    }
    return OFFalse;  
  }

  /** checks whether an attribute with the given tag must not be signed
   *  for the current security profile.
   *  @param key tag key to be checked
   *  @return true if attribute must not be signed, false otherwise.
   */
  virtual OFBool attributeForbidden(const DcmTagKey& key) const
  {
    return tagInList(key);
  }

private:

  /// list of attributes not to sign  
  DcmAttributeTag& notToSign;

  /// number of entries in notToSign
  unsigned long vmNotToSign;
};

#endif


DVSignatureHandler::DVSignatureHandler(DVConfiguration& cfg)
: htmlSR("<html><head><title>Structured Report</title></head><body>No structured report is currently active.</body></html>\n")
, htmlImage("<html><head><title>Image</title></head><body>No image is currently active.</body></html>\n")
, htmlPState("<html><head><title>Presentation State</title></head><body>No presentation state is currently active.</body></html>\n")
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
#ifdef WITH_OPENSSL
, certVerifier()
#endif
, config(cfg)
{
#ifdef WITH_OPENSSL
  int fileFormat = config.getTLSPEMFormat() ? X509_FILETYPE_PEM : X509_FILETYPE_ASN1;
  const char *tlsCACertificateFolder = config.getTLSCACertificateFolder();
  if (tlsCACertificateFolder) certVerifier.addTrustedCertificateDir(tlsCACertificateFolder, fileFormat);
#endif
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

void DVSignatureHandler::printSignatureItemPosition(DcmStack& stack, STD_NAMESPACE ostream& os)
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

#ifdef WITH_OPENSSL
void DVSignatureHandler::updateDigitalSignatureInformation(DcmItem& dataset, DVPSObjectType objtype, OFBool /* onRead */)
#else
void DVSignatureHandler::updateDigitalSignatureInformation(DcmItem& /*dataset*/, DVPSObjectType objtype, OFBool /* onRead */)
#endif
{
  OFOStringStream os;
  unsigned long counter = 0;
  unsigned long corrupt_counter = 0;
  unsigned long untrustworthy_counter = 0;
  const char *htmlHead     = NULL;
  const char *htmlFoot     = "</body></html>\n\n";

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

#ifdef WITH_OPENSSL
  DcmStack stack;
  OFString aString;
  DcmAttributeTag at(DCM_DataElementsSigned);  
  DcmTag tag;
  unsigned long numSignatures = 0;
  unsigned long l=0;
  Uint16 macID = 0;
  DcmTagKey tagkey;
  const char *tagName = NULL;
  OFBool nextline;

  const char *htmlEndl     = "</td></tr>\n";
  const char *htmlVfyOK    = "<tr><td colspan=\"4\" bgcolor=\"#50ff50\">";
  const char *htmlVfyCA    = "<tr><td colspan=\"4\" bgcolor=\"yellow\">";  
  const char *htmlVfyErr   = "<tr><td colspan=\"4\" bgcolor=\"#FF5050\">";  
  const char *htmlLine1    = "<tr><td width=\"20\" nowrap>&nbsp;</td><td colspan=\"2\" nowrap>";
  const char *htmlLine2    = "<tr><td colspan=\"3\" nowrap>&nbsp;</td><td>";
  const char *htmlLine3    = "<tr><td colspan=\"2\" nowrap>&nbsp;</td><td nowrap>";
  const char *htmlLine4    = "<tr><td width=\"20\" nowrap>&nbsp;</td><td width=\"20\" nowrap>&nbsp;</td><td>";
  const char *htmlNext     = "</td><td>";
  const char *htmlTableOK  = "<table cellspacing=\"0\" bgcolor=\"#D0FFD0\">\n";
  const char *htmlTableCA  = "<table cellspacing=\"0\" bgcolor=\"#FFF8DC\">\n";
  const char *htmlTableErr = "<table cellspacing=\"0\" bgcolor=\"#FFD0D0\">\n";
  const char *htmlTableE   = "</table><p>\n\n";

  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(dataset, stack);
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  while (sigItem)
  {
    signer.attach(sigItem);
    numSignatures = signer.numberOfSignatures();
    for (l=0; l<numSignatures; l++)
    {
      if (EC_Normal == signer.selectSignature(l))
      {
        ++counter;
        sicond = signer.verifyCurrent();
        SiCertificate *cert = signer.getCurrentCertificate();
        if ((sicond == EC_Normal) && cert)
        {
          sicond = certVerifier.verifyCertificate(*cert);
        }
        
        if (sicond == EC_Normal)
          os << htmlTableOK  << htmlVfyOK; 
        else if (sicond == SI_EC_VerificationFailed_NoTrust) 
          os << htmlTableCA  << htmlVfyCA; 
        else
          os << htmlTableErr << htmlVfyErr;

        os << "<b>Signature #" << counter << " UID=";
        if (EC_Normal == signer.getCurrentSignatureUID(aString)) os << aString.c_str(); else os << "(unknown)";
        os << "</b>" << htmlEndl;

        os << htmlLine1 << "Location" << htmlNext;
        printSignatureItemPosition(stack, os);        
        os << htmlEndl;

        os << htmlLine1 << "MAC ID" << htmlNext;
        if (EC_Normal == signer.getCurrentMacID(macID)) os << macID; else os << "(unknown)";
        os << htmlEndl;

        os << htmlLine1 << "MAC algorithm" << htmlNext;
        if (EC_Normal == signer.getCurrentMacName(aString)) os << aString.c_str(); else os << "(unknown)";
        os << htmlEndl;

        os << htmlLine1 << "MAC calculation xfer syntax" << htmlNext;
        if (EC_Normal == signer.getCurrentMacXferSyntaxName(aString)) os << aString.c_str(); else os << "(unknown)";
        os << htmlEndl;

        os << htmlLine1 << "Data elements signed" << htmlNext;
        nextline = OFFalse;
        if (EC_Normal == signer.getCurrentDataElementsSigned(at))
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
        if (EC_Normal == signer.getCurrentSignatureDateTime(aString)) os << aString.c_str(); else os << "(unknown)";
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

        if (sicond.good())
        {
          os << htmlVfyOK << "<b>Verification: OK</b>" << htmlEndl;
        } 
        else if (sicond == SI_EC_VerificationFailed_NoTrust)
        {
          untrustworthy_counter++;
          os << htmlVfyCA << "<b>Verification: Signature is valid but certificate could not be verified: " 
             << certVerifier.lastError() << "</b>" << htmlEndl ;
        }
        else
        {
          corrupt_counter++;
          os << htmlVfyErr << "<b>Verification: ";
          os << sicond.text() << "</b>" << htmlEndl;
        }
        os << htmlTableE;
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(dataset, stack);
  }
#endif  

  switch (objtype)
  {
    case DVPSS_structuredReport:
      if (counter == 0) os << "The current structured report does not contain any digital signature." << OFendl;
      corruptSignaturesSR = corrupt_counter;
      untrustSignaturesSR = untrustworthy_counter;  
      correctSignaturesSR = counter - corrupt_counter - untrustworthy_counter;
      break;
    case DVPSS_image:
      if (counter == 0) os << "The current image does not contain any digital signature." << OFendl;
      corruptSignaturesImage = corrupt_counter;
      untrustSignaturesImage = untrustworthy_counter;  
      correctSignaturesImage = counter - corrupt_counter - untrustworthy_counter;
      break;
    case DVPSS_presentationState:
      if (counter == 0) os << "The current presentation state does not contain any digital signature." << OFendl;
      corruptSignaturesPState = corrupt_counter;
      untrustSignaturesPState = untrustworthy_counter;  
      correctSignaturesPState = counter - corrupt_counter - untrustworthy_counter;
      break;  
  }
  os << htmlFoot << OFStringStream_ends;
  OFSTRINGSTREAM_GETSTR(os, newText)
  replaceString(objtype, newText); // copies newText into OFString
  OFSTRINGSTREAM_FREESTR(newText)
  updateSignatureValidationOverview();
  return;
}


void DVSignatureHandler::disableDigitalSignatureInformation(DVPSObjectType objtype)
{
  switch (objtype)
  {
    case DVPSS_structuredReport:
      htmlSR = "<html><head><title>Structured Report</title></head><body>The current structured report does not contain any digital signature.</body></html>\n";
      corruptSignaturesSR = 0;
      untrustSignaturesSR = 0;  
      correctSignaturesSR = 0;
      break;
    case DVPSS_image:
      corruptSignaturesImage = 0;
      untrustSignaturesImage = 0;  
      correctSignaturesImage = 0;
      htmlImage = "<html><head><title>Image</title></head><body>The current image does not contain any digital signature.</body></html>\n";
      break;
    case DVPSS_presentationState:
      corruptSignaturesPState = 0;
      untrustSignaturesPState = 0;  
      correctSignaturesPState = 0;
      htmlPState = "<html><head><title>Presentation State</title></head><body>The current presentation state does not contain any digital signature.</body></html>\n";
      break;  
  }
  updateSignatureValidationOverview();
}

void DVSignatureHandler::disableImageAndPState()
{
  corruptSignaturesImage = 0;
  untrustSignaturesImage = 0;  
  correctSignaturesImage = 0;
  htmlImage = "<html><head><title>Image</title></head><body>No image is currently active.</body></html>\n";
  corruptSignaturesPState = 0;
  untrustSignaturesPState = 0;  
  correctSignaturesPState = 0;
  htmlPState = "<html><head><title>Presentation State</title></head><body>No presentation state is currently active.</body></html>\n";
  updateSignatureValidationOverview();
  return;
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
  const char *htmlTitle    = "<tr><td colspan=\"3\">";
  const char *htmlVfyUns   = "<tr><td colspan=\"3\" bgcolor=\"#A0A0A0\">";
  const char *htmlVfySig   = "<tr><td colspan=\"3\" bgcolor=\"#50ff50\">";
  const char *htmlVfyCA    = "<tr><td colspan=\"3\" bgcolor=\"yellow\">";
  const char *htmlVfyErr   = "<tr><td colspan=\"3\" bgcolor=\"#FF5050\">";  
  const char *htmlLine1    = "<tr><td width=\"20\" nowrap>&nbsp;</td><td nowrap>";
  const char *htmlNext     = "</td><td>";
  const char *htmlTableUns = "<p><table cellspacing=\"0\" bgcolor=\"#E0E0E0\">\n";
  const char *htmlTableSig = "<p><table cellspacing=\"0\" bgcolor=\"#FFD0D0\">\n";
  const char *htmlTableCA  = "<p><table cellspacing=\"0\" bgcolor=\"#FFF8DC\">\n";
  const char *htmlTableErr = "<p><table cellspacing=\"0\" bgcolor=\"#FFD0D0\">\n";
  const char *htmlTableE   = "</table></p>\n\n";

  OFOStringStream os;
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
      os << htmlVfyUns << "<b>Status: unsigned</b>" << htmlEndl;
      break;
    case DVPSW_signed_OK:
      os << htmlVfySig << "<b>Status: signed</b>" << htmlEndl;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlVfyCA  << "<b>Status: signed but untrustworthy: certificate could not be verified</b>" << htmlEndl;
      break;
    case DVPSW_signed_corrupt:
      os << htmlVfyErr << "<b>Status: contains corrupt signatures</b>" << htmlEndl;
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
      os << htmlVfyUns << "<b>Status: unsigned</b>" << htmlEndl;
      break;
    case DVPSW_signed_OK:
      os << htmlVfySig << "<b>Status: signed</b>" << htmlEndl;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlVfyCA  << "<b>Status: signed but untrustworthy: certificate could not be verified</b>" << htmlEndl;
      break;
    case DVPSW_signed_corrupt:
      os << htmlVfyErr << "<b>Status: contains corrupt signatures</b>" << htmlEndl;
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
  os << htmlTitle << "<b>Presentation State</b>"<< htmlEndl;
  os << htmlLine1 << "Number of correct signatures" << htmlNext << correctSignaturesPState << htmlEndl;
  os << htmlLine1 << "Number of corrupt signatures" << htmlNext << corruptSignaturesPState << htmlEndl;
  os << htmlLine1 << "Number of untrusted signatures" << htmlNext << untrustSignaturesPState << htmlEndl;
  switch (status)
  {
    case DVPSW_unsigned:
      os << htmlVfyUns << "<b>Status: unsigned</b>" << htmlEndl;
      break;
    case DVPSW_signed_OK:
      os << htmlVfySig << "<b>Status: signed</b>" << htmlEndl;
      break;
    case DVPSW_signed_unknownCA:
      os << htmlVfyCA  << "<b>Status: signed but untrustworthy: certificate could not be verified</b>" << htmlEndl;
      break;
    case DVPSW_signed_corrupt:
      os << htmlVfyErr << "<b>Status: contains corrupt signatures</b>" << htmlEndl;
      break;
  }
  os << htmlTableE;
  os << htmlFoot << OFStringStream_ends;

  OFSTRINGSTREAM_GETSTR(os, newText)
  htmlOverview = newText;
  OFSTRINGSTREAM_FREESTR(newText)
  return;  
}

const char *DVSignatureHandler::getCurrentSignatureValidationOverview() const
{
  return htmlOverview.c_str();
}

unsigned long DVSignatureHandler::getNumberOfCorrectSignatures(DVPSObjectType objtype) const
{
  unsigned long result = 0;
  switch (objtype)
  {
    case DVPSS_structuredReport:
      result = correctSignaturesSR;
      break;
    case DVPSS_image:
      result = correctSignaturesImage;
      break;
    case DVPSS_presentationState:
      result = correctSignaturesPState;
      break;
  }
  return result;
}


unsigned long DVSignatureHandler::getNumberOfUntrustworthySignatures(DVPSObjectType objtype) const
{
  unsigned long result = 0;
  switch (objtype)
  {
    case DVPSS_structuredReport:
      result = untrustSignaturesSR;
      break;
    case DVPSS_image:
      result = untrustSignaturesImage;
      break;
    case DVPSS_presentationState:
      result = untrustSignaturesPState;
      break;
  }
  return result;
}


unsigned long DVSignatureHandler::getNumberOfCorruptSignatures(DVPSObjectType objtype) const
{
  unsigned long result = 0;
  switch (objtype)
  {
    case DVPSS_structuredReport:
      result = corruptSignaturesSR;
      break;
    case DVPSS_image:
      result = corruptSignaturesImage;
      break;
    case DVPSS_presentationState:
      result = corruptSignaturesPState;
      break;
  }
  return result;
}

#ifdef WITH_OPENSSL

OFBool DVSignatureHandler::attributesSigned(DcmItem& item, DcmAttributeTag& tagList) const
{
  DcmStack stack;
  DcmAttributeTag at(DCM_DataElementsSigned);  
  DcmTagKey tagkey;
  DcmSignature signer;
  unsigned long numSignatures;
  unsigned long l;
  DVSignatureHandlerSignatureProfile sigProfile(tagList);
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(item, stack);
  while (sigItem)
  {
    if (sigItem == &item) 
    {
      // signatures on main level - check attributes signed
      signer.attach(sigItem);
      numSignatures = signer.numberOfSignatures();
      for (l=0; l<numSignatures; l++)
      {
        if (EC_Normal == signer.selectSignature(l))
        {
          // printSignatureItemPosition(stack, os);        
          if (EC_Normal == signer.getCurrentDataElementsSigned(at))
          {
            unsigned long atVM = at.getVM();
            for (unsigned long n=0; n<atVM; n++)
            {
              if (EC_Normal == at.getTagVal(tagkey, n))
              {
                if (sigProfile.tagInList(tagkey)) return OFTrue; // one of the elements in tagList is signed
              }
            }
          } else return OFTrue; // all elements are signed
        }
      }
      signer.detach();
    }
    else
    {
      // signatures on lower level - check whether attribute on main level is in list
      unsigned long scard = stack.card();
      if (scard > 1) // should always be true
      {
        DcmObject *obj = stack.elem(scard-2);
        if (obj) // should always be true
        {
          if (sigProfile.tagInList(obj->getTag())) return OFTrue; // one of the elements in tagList contains a signature
        }
      }
    }
    sigItem = DcmSignature::findNextSignatureItem(item, stack);
  }
  return OFFalse;  
}

#else

OFBool DVSignatureHandler::attributesSigned(DcmItem& /* item */, DcmAttributeTag& /* tagList */) const
{
  return OFFalse;  
}

#endif


#ifdef WITH_OPENSSL

OFCondition DVSignatureHandler::createSignature(
    DcmItem& mainDataset,
    const DcmStack& itemStack,
    DcmAttributeTag& attributesNotToSignInMainDataset,
    const char *userID,
    const char *passwd)
{
  if (userID == NULL) return EC_IllegalCall;

  // get user settings
  int fileformat = config.getTLSPEMFormat() ? X509_FILETYPE_PEM : X509_FILETYPE_ASN1;
  const char *userDir  = config.getUserCertificateFolder();
  const char *userKey  = config.getUserPrivateKey(userID);
  if (userKey == NULL) return EC_IllegalCall;
  const char *userCert = config.getUserCertificate(userID);
  if (userCert == NULL) return EC_IllegalCall;
  
  // load private key
  SiPrivateKey key;
  OFString filename;
  if (userDir)
  {
    filename = userDir;
    filename += PATH_SEPARATOR;
  }
  filename += userKey;
  if (passwd) key.setPrivateKeyPasswd(passwd);
  OFCondition sicond = key.loadPrivateKey(filename.c_str(), fileformat);
  if (sicond != EC_Normal) return EC_IllegalCall; // unable to load private key

  // load certificate
  SiCertificate cert;
  filename.clear();
  if (userDir)
  {
    filename = userDir;
    filename += PATH_SEPARATOR;
  }
  filename += userCert;
  sicond = cert.loadCertificate(filename.c_str(), fileformat);
  if (sicond != EC_Normal) return EC_IllegalCall; // unable to load certificate
  if (! key.matchesCertificate(cert)) return EC_IllegalCall; // private key does not match certificate

  DcmSignature signer;
  SiRIPEMD160 mac;
  SiNullProfile nullProfile;
  DVSignatureHandlerSignatureProfile mainProfile(attributesNotToSignInMainDataset);
  
  DcmObject *current;
  DcmItem *currentItem;
  DcmStack workStack(itemStack);
  while (! workStack.empty())
  {
    current = workStack.pop();
    if ((current->ident() != EVR_dataset) && (current->ident() != EVR_item)) return EC_IllegalCall; // wrong type on stack
    currentItem = (DcmItem *)current;
    signer.attach(currentItem);
    if (currentItem == &mainDataset)
    {
      // we're creating a signature in the main dataset
      // we have to establish an explicit tag list, otherwise the profile does not work!
      DcmAttributeTag tagList(DCM_DataElementsSigned);
      unsigned long numAttributes = currentItem->card();
      for (unsigned long l=0; l<numAttributes; l++)
      {
        tagList.putTagVal(currentItem->getElement(l)->getTag(),l);
      }
      sicond = signer.createSignature(key, cert, mac, mainProfile, EXS_LittleEndianExplicit, &tagList);
      if (sicond != EC_Normal) return EC_IllegalCall; // error while creating signature
    }
    else     
    {
      // we're creating a signature in a sequence item
      sicond = signer.createSignature(key, cert, mac, nullProfile, EXS_LittleEndianExplicit);
      if (sicond != EC_Normal) return EC_IllegalCall; // error while creating signature
    }
    signer.detach();
  }
  return EC_Normal;  
}

#else

OFCondition DVSignatureHandler::createSignature(
    DcmItem& /* mainDataset */,
    const DcmStack& /* itemStack */,
    DcmAttributeTag& /* attributesNotToSignInMainDataset */,
    const char * /* userID */,
    const char * /* passwd */)
{
  return EC_IllegalCall;
}

#endif


/*
 *  $Log: dvsighdl.cc,v $
 *  Revision 1.17  2010-10-14 13:14:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2010-03-01 09:08:49  uli
 *  Removed some unnecessary include directives in the headers.
 *
 *  Revision 1.15  2006-08-15 16:57:02  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.14  2005/12/08 15:46:59  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.13  2004/02/04 15:57:49  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.12  2002/12/20 14:52:00  wilkens
 *  Modified name clash resulting in a compiler error on Solaris 2.5.1 using
 *  compiler SC 2.0.1.
 *
 *  Revision 1.11  2002/05/02 14:10:22  joergr
 *  Added support for standard and non-standard string streams (which one is
 *  supported is detected automatically via the configure mechanism).
 *
 *  Revision 1.10  2002/04/16 14:02:23  joergr
 *  Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *  Revision 1.9  2001/12/04 18:02:25  meichel
 *  Adapted dcmpstat signature code to changed interface in dcmsign
 *
 *  Revision 1.8  2001/09/26 15:36:36  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.7  2001/06/05 10:30:56  joergr
 *  Replaced some #ifdef _WIN32 statements by #ifdef HAVE_WINDOWS_H or #ifdef
 *  __CYGWIN__ respectively to reflect the fact that the latest Cygwin/gcc
 *  version does not define _WIN32 any more.
 *
 *  Revision 1.6  2001/05/25 10:07:35  meichel
 *  Modified dcmpstat signature handler to also compile without OpenSSL
 *
 *  Revision 1.5  2001/02/13 09:55:45  joergr
 *  Minor purifications in "signature validation overview" HTML page.
 *
 *  Revision 1.4  2001/01/29 17:34:01  joergr
 *  Fixed bug in createSignature method.
 *
 *  Revision 1.3  2001/01/29 14:55:47  meichel
 *  Added new methods for creating signatures and checking the signature
 *    status in module dcmpstat.
 *
 *  Revision 1.2  2001/01/26 10:43:14  meichel
 *  Introduced additional (fourth) status flag for signature validation
 *    describing signatures that are valid but untrustworthy (unknown CA).
 *
 *  Revision 1.1  2001/01/25 15:18:10  meichel
 *  Added initial support for verification of digital signatures
 *    in presentation states, images and structured reports to module dcmpstat.
 *
 *
 */

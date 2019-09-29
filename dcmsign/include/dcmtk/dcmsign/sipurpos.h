/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiSignaturePurpose
 *
 */

#ifndef SIPURPOS_H
#define SIPURPOS_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sidefine.h"
#include "dcmtk/ofstd/ofstream.h"

class DcmItem;
class OFCondition;

/** Helper class for the Digital Signature Purpose Code Sequence.
 *  @remark this class is only available if DCMTK is compiled with
 *  OpenSSL support enabled.
 */
class DCMTK_DCMSIGN_EXPORT SiSignaturePurpose
{
public:

    /** purpose of digital signature as defined in DICOM BCID 7007.
     *  @remark this enum is only available if DCMTK is compiled with
     *  OpenSSL support enabled.
     */
    enum E_SignaturePurposeType
    {
      /// no signature purpose specified
      ESP_none = 0,

      /// Author's Signature
      ESP_AuthorsSignature = 1,

      /// Coauthor's Signature
      ESP_CoauthorsSignature = 2,

      /// Co-participant's Signature
      ESP_CoparticipantsSignature = 3,

      /// Transcriptionist/Recorder Signature
      ESP_TranscriptionistSignature = 4,

      /// Verification Signature
      ESP_VerificationSignature = 5,

      /// Validation Signature
      ESP_ValidationSignature = 6,

      /// Consent Signature
      ESP_ConsentSignature = 7,

      /// Signature Witness Signature
      ESP_SignatureWitnessSignature = 8,

      /// Event Witness Signature
      ESP_EventWitnessSignature = 9,

      /// Identity Witness Signature
      ESP_IdentityWitnessSignature = 10,

      /// Consent Witness Signature
      ESP_ConsentWitnessSignature = 11,

      /// Interpreter Signature
      ESP_InterpreterSignature = 12,

      /// Review Signature
      ESP_ReviewSignature = 13,

      /// Source Signature
      ESP_SourceSignature = 14,

      /// Addendum Signature
      ESP_AddendumSignature = 15,

      /// Modification Signature
      ESP_ModificationSignature = 16,

      /// Administrative (Error/Edit) Signature
      ESP_AdministrativeSignature = 17,

      /// Timestamp Signature
      ESP_TimestampSignature = 18

    };

    /** return the code value for the given signature purpose
     *  @return code value for the given signature purpose, NULL for ESP_none
     */
    static const char *getCodeValue(E_SignaturePurposeType purpose);

    /** return the code meaning for the given signature purpose
     *  @return code meaning for the given signature purpose, NULL for ESP_none
     */
    static const char *getCodeMeaning(E_SignaturePurposeType purpose);

    /** return the coding scheme designator for the given signature purpose
     *  @return coding scheme designator for the given signature purpose
     */
    static const char *getCodingSchemeDesignator(E_SignaturePurposeType purpose);

    /** insert a digital signature purpose code sequence into the given DICOM item
     *  @param seqItem item into which the sequence is inserted. This should be an item
     *    of the DigitalSignaturesSequence.
     *  @param sigPurpose signature purpose. If the purpose is ESP_none, nothing will be 
     *    inserted and this method will immediately return with EC_Normal
     *  @return EC_Normal if successful, an error code otherwise
     */
    static OFCondition insertDigitalSignaturePurposeCodeSQ(DcmItem& seqItem, E_SignaturePurposeType sigPurpose);

    /** determine the signature purpose to be used, and prints a warning to the logger if
     *  an override required by the signature profile causes the user selection to be ignored.
     *  @param currentPurpose signature purpose selected by the user
     *  @param overridePurpose override signature purpose required by the signature profile
     *  @return signature purpose to be used for the digital signature purpose code sequence
     */
    static E_SignaturePurposeType determineOverridePurpose(E_SignaturePurposeType currentPurpose, E_SignaturePurposeType overridePurpose);

    /** look-up signature purpose enum by number
     *  @param num number, 0 for ESP_none, 1 for ESP_AuthorsSignature etc.
     *  @return signature purpose enum or ESP_none if number too high.
     */
    static E_SignaturePurposeType lookup(size_t num);

    /** print a list of all signature purpose codes supported by this helper class.
     *  This is used by dcmsign --list-purposes.
     *  @param out output stream to print to
     */
    static void printSignatureCodes(STD_NAMESPACE ostream& out);

};

#endif
#endif

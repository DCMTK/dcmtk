/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Michael Onken
 *
 *  Purpose: tests for parseAssociate() error-path cleanup of the
 *           extended negotiation sub-item list (extNegList) and of a
 *           partially parsed presentation context.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dcasccfg.h"
#include "dcmtk/dcmnet/dulstruc.h"
#include "dcmtk/dcmnet/extneg.h"
#include "dcmtk/dcmnet/dcuserid.h"

#include <cstring>                    /* for strlen() */

/* parseAssociate is declared in the private dcmnet header. The test reaches
 * into libsrc/ deliberately to whitebox-test the PDU parser without going
 * through a real TCP association. */
#include "../libsrc/dulpriv.h"

/* ---------------------------------------------------------------------------
 *  PDU layout constants
 *
 *  An A-ASSOCIATE-RQ PDU is structured as:
 *      offset 0   : PDU type            (1 byte,  0x01)
 *      offset 1   : reserved            (1 byte,  0x00)
 *      offset 2   : PDU length          (4 bytes, big-endian) - counts only
 *                                        the bytes AFTER this field, i.e. it
 *                                        excludes the 6-byte preamble above.
 *      offset 6   : protocol version    (2 bytes)
 *      offset 8   : reserved            (2 bytes)
 *      offset 10  : called AE title     (16 bytes, padded with spaces)
 *      offset 26  : calling AE title    (16 bytes, padded with spaces)
 *      offset 42  : reserved            (32 bytes, zeros)
 *      offset 74  : variable items follow (presentation contexts, user info, ...)
 *
 *  Variable items (including the User Info sub-PDU and each sub-item inside
 *  it) share a 4-byte sub-PDU header:
 *      byte 0     : item type           (1 byte)
 *      byte 1     : reserved            (1 byte, 0x00)
 *      byte 2-3   : item length         (2 bytes, big-endian) - counts only
 *                                        the bytes AFTER this field.
 *
 *  A SOP Class Extended Negotiation sub-item (item type 0x56) has, after that
 *  4-byte header, a mandatory 2-byte sopClassUIDLength field; with an empty
 *  UID and no serviceClassAppInfo bytes that yields a 6-byte minimum.
 * ------------------------------------------------------------------------- */

static const unsigned long  ASSOC_RQ_HEADER_BYTES        = 74; // fixed header up to offset 74
static const unsigned long  PDU_PREAMBLE_BYTES           = 6;  // type + rsv + 4-byte length
static const unsigned long  SUBPDU_HEADER_BYTES          = 4;  // type + rsv + 2-byte length
static const unsigned short EXT_NEG_MIN_BODY_BYTES       = 2;  // sopClassUIDLength field only
static const unsigned long  EXT_NEG_MIN_SUBITEM_BYTES    = SUBPDU_HEADER_BYTES + EXT_NEG_MIN_BODY_BYTES; // 6


/* Write a 16-bit big-endian value into buf and advance the cursor. */
static void put_u16_be(unsigned char *&p, unsigned short v)
{
    *p++ = OFstatic_cast(unsigned char, (v >> 8) & 0xff);
    *p++ = OFstatic_cast(unsigned char, v & 0xff);
}

/* Write a 32-bit big-endian value into buf and advance the cursor. */
static void put_u32_be(unsigned char *&p, unsigned long v)
{
    *p++ = OFstatic_cast(unsigned char, (v >> 24) & 0xff);
    *p++ = OFstatic_cast(unsigned char, (v >> 16) & 0xff);
    *p++ = OFstatic_cast(unsigned char, (v >> 8) & 0xff);
    *p++ = OFstatic_cast(unsigned char, v & 0xff);
}

/* Write the fixed-size A-ASSOCIATE-RQ header (ASSOC_RQ_HEADER_BYTES = 74,
 * see the layout comment above). Variable items follow at offset 74.
 * Returns the cursor positioned at the first variable-item byte.
 * `pduPayloadLen` is the value placed into the PDU length field, which by the
 * spec excludes the first PDU_PREAMBLE_BYTES (= 6) of the PDU. */
static unsigned char *write_assoc_rq_header(unsigned char *buf, unsigned long pduPayloadLen)
{
    const int AE_TITLE_BYTES          = 16; // called/calling AET fields (each)
    const int TRAILING_RESERVED_BYTES = 32; // reserved block after the AETs
    // Each AE-title literal is exactly AE_TITLE_BYTES (16) characters so no
    // padding is required and the field contents look like a realistic
    // upper-case ASCII AET rather than an all-spaces placeholder.
    const char *calledAE  = "DCMNETTESTCALLED";
    const char *callingAE = "DCMNETTESTCALLER";

    unsigned char *p = buf;
    *p++ = 0x01;                              // PDU type: A-ASSOCIATE-RQ
    *p++ = 0x00;                              // reserved
    put_u32_be(p, pduPayloadLen);        // PDU length (excludes PDU_PREAMBLE_BYTES)
    put_u16_be(p, DUL_PROTOCOL);              // protocol version
    *p++ = 0x00; *p++ = 0x00;                 // reserved
    for (int i = 0; i < AE_TITLE_BYTES; ++i) *p++ = OFstatic_cast(unsigned char, calledAE[i]);
    for (int i = 0; i < AE_TITLE_BYTES; ++i) *p++ = OFstatic_cast(unsigned char, callingAE[i]);
    for (int i = 0; i < TRAILING_RESERVED_BYTES; ++i) *p++ = 0x00;  // reserved
    return p;
}


/* Regression test for the primary leak from DCMTK issue #1216:
 *   destroyUserInformationLists() freed only the OFList container, orphaning
 *   the SOPClassExtendedNegotiationSubItem* members already pushed into
 *   userInfo->extNegList when parseExtNeg() failed late in the user-info loop.
 *
 * Payload: N valid 6-byte 0x56 sub-items followed by one truncated sub-item
 * (5 bytes), which makes parseExtNeg() reject the last entry at availData < 6.
 * Pre-fix this leaks all N already-parsed items + their serviceClassAppInfo
 * buffers on every malicious association. Functional assertion below catches
 * a regression in the cleanup logic (e.g. accidental double-free); the leak
 * itself is only directly observable under LeakSanitizer (e.g. by building with
 * DCMTK_WITH_SANITIZERS=ON).
 */
OFTEST(dcmnet_parseAssociate_extNeg_truncated)
{
    const int validItems = 10;
    // Each valid sub-item is EXT_NEG_MIN_SUBITEM_BYTES (6) on the wire; the
    // truncated one is intentionally one byte short to trip parseExtNeg's
    // availData < 6 check.
    const unsigned long validBytes = OFstatic_cast(unsigned long, validItems) * EXT_NEG_MIN_SUBITEM_BYTES;
    const unsigned long truncBytes = EXT_NEG_MIN_SUBITEM_BYTES - 1;
    const unsigned short userInfoPayload = OFstatic_cast(unsigned short, validBytes + truncBytes);
    // totalLen = fixed A-ASSOCIATE-RQ header + User Info sub-PDU header + payload.
    const unsigned long totalLen = ASSOC_RQ_HEADER_BYTES + SUBPDU_HEADER_BYTES + userInfoPayload;
    // The PDU length field excludes the leading PDU_PREAMBLE_BYTES.
    const unsigned long pduPayloadLen = totalLen - PDU_PREAMBLE_BYTES;

    unsigned char *buf = new unsigned char[totalLen];
    unsigned char *p = write_assoc_rq_header(buf, pduPayloadLen);

    // User Info sub-PDU header: type + reserved + 2-byte item length = SUBPDU_HEADER_BYTES.
    *p++ = DUL_TYPEUSERINFO;
    *p++ = 0x00;
    put_u16_be(p, userInfoPayload);

    // N valid extended-negotiation sub-items: itemLength = EXT_NEG_MIN_BODY_BYTES
    // (just the 2-byte sopClassUIDLength field), sopClassUIDLength = 0; so each
    // sub-item occupies exactly EXT_NEG_MIN_SUBITEM_BYTES on the wire.
    for (int i = 0; i < validItems; ++i) {
        *p++ = DUL_TYPESOPCLASSEXTENDEDNEGOTIATION;
        *p++ = 0x00;
        put_u16_be(p, EXT_NEG_MIN_BODY_BYTES);   // itemLength
        put_u16_be(p, 0);                        // sopClassUIDLength
    }

    // One truncated sub-item: claims itemLength = EXT_NEG_MIN_BODY_BYTES but
    // delivers only EXT_NEG_MIN_SUBITEM_BYTES - 1 bytes, so parseExtNeg trips
    // availData < 6 AFTER the outer loop has already pushed `validItems`
    // complete sub-items into extNegList - that is the leaked set.
    *p++ = DUL_TYPESOPCLASSEXTENDEDNEGOTIATION;  // type    (1)
    *p++ = 0x00;                                 // reserved (1)
    *p++ = 0x00; *p++ = 0x02;                    // itemLength = 2 (2)
    *p++ = 0x00;                                 // sopClassUIDLength: missing
                                                 // one byte to deliberately truncate

    OFCHECK_EQUAL(OFstatic_cast(unsigned long, p - buf), totalLen);

    PRV_ASSOCIATEPDU assoc;
    OFCondition cond = parseAssociate(buf, pduPayloadLen, &assoc);

    // Must reject the malformed PDU.
    OFCHECK(cond.bad());
    // Cleanup ran: list container was freed and pointer cleared.
    OFCHECK(assoc.userInfo.extNegList == NULL);

    delete[] buf;
}


/* Regression test for another leak tracked as DCMTK issue #TODO:
 *   parseUserInfo() allocates a SOPClassExtendedNegotiationSubItem* stub
 *   before invoking parseExtNeg(). When parseExtNeg() returned an error the
 *   stub was never pushed to extNegList and not freed either.
 *
 * Payload: a single 6-byte 0x56 sub-item with itemLength=1, which trips the
 * "itemLength < 2" check inside parseExtNeg() AFTER the stub allocation but
 * BEFORE serviceClassAppInfo is allocated. Before fixing the code leaked one
 * stub per call. Functional assertions below only catch a regression in the
 * cleanup logic (e.g. accidental double-free); the leak itself is only
 * directly observable under LeakSanitizer (build with
 * DCMTK_WITH_SANITIZERS=ON on Linux).
 */
OFTEST(dcmnet_parseAssociate_extNeg_malformed_itemLength)
{
    // The User Info payload is one minimum-size extended-negotiation sub-item
    // (EXT_NEG_MIN_SUBITEM_BYTES on the wire); the payload value is a uint16
    // because that is what the sub-PDU length field carries.
    const unsigned short userInfoPayload = OFstatic_cast(unsigned short, EXT_NEG_MIN_SUBITEM_BYTES);
    const unsigned long totalLen = ASSOC_RQ_HEADER_BYTES + SUBPDU_HEADER_BYTES + userInfoPayload;
    const unsigned long pduPayloadLen = totalLen - PDU_PREAMBLE_BYTES;

    unsigned char *buf = new unsigned char[totalLen];
    unsigned char *p = write_assoc_rq_header(buf, pduPayloadLen);

    *p++ = DUL_TYPEUSERINFO;
    *p++ = 0x00;
    put_u16_be(p, userInfoPayload);

    // Malformed extended-negotiation sub-item: itemLength = 1 violates the
    // (itemLength >= EXT_NEG_MIN_BODY_BYTES) invariant in parseExtNeg.
    *p++ = DUL_TYPESOPCLASSEXTENDEDNEGOTIATION;
    *p++ = 0x00;
    put_u16_be(p, 1);                            // itemLength = 1 (invalid; < EXT_NEG_MIN_BODY_BYTES)
    put_u16_be(p, 0);                            // sopClassUIDLength

    OFCHECK_EQUAL(OFstatic_cast(unsigned long, p - buf), totalLen);

    PRV_ASSOCIATEPDU assoc;
    OFCondition cond = parseAssociate(buf, pduPayloadLen, &assoc);

    OFCHECK(cond.bad());
    // The malformed item never reached the list, so it stays NULL.
    OFCHECK(assoc.userInfo.extNegList == NULL);

    delete[] buf;
}


/* Regression test for the pre-authentication memory leak caused by a duplicate
 * User Identity Negotiation sub-item (item type 0x58) in an A-ASSOCIATE-RQ.
 * DICOM permits this sub-item at most once per association, but
 * parseUserInfo() stored each parsed sub-item in the single userInfo->usrIdent
 * pointer. A second sub-item silently overwrote (and leaked) the first, since
 * destroyUserInformationLists() can only free whichever pointer is stored last.
 * An unauthenticated peer could pack many sub-items into one PDU to exhaust
 * server memory.
 *
 * Payload: two minimal, well-formed 0x58 sub-items. The parser must now reject
 * the PDU when it sees the second one. The functional assertion below also
 * confirms the error-path cleanup cleared userInfo->usrIdent (no leak, no
 * double-free); the leak itself is only directly observable under
 * tooling like LeakSanitizer.
 */
OFTEST(dcmnet_parseAssociate_duplicate_userIdentity)
{
    // A minimal User Identity Negotiation RQ sub-item is 10 bytes on the wire:
    // the 4-byte sub-PDU header plus a 6-byte mandatory body (identity type(1),
    // positive-response-requested(1), primary-field length(2), secondary-field
    // length(2)) with both variable fields empty.
    const unsigned short USER_IDENT_BODY_BYTES    = 6;
    const unsigned long  USER_IDENT_SUBITEM_BYTES = SUBPDU_HEADER_BYTES + USER_IDENT_BODY_BYTES; // 10
    const int            numSubItems              = 2;

    const unsigned short userInfoPayload = OFstatic_cast(unsigned short,
        OFstatic_cast(unsigned long, numSubItems) * USER_IDENT_SUBITEM_BYTES);
    const unsigned long totalLen = ASSOC_RQ_HEADER_BYTES + SUBPDU_HEADER_BYTES + userInfoPayload;
    const unsigned long pduPayloadLen = totalLen - PDU_PREAMBLE_BYTES;

    unsigned char *buf = new unsigned char[totalLen];
    unsigned char *p = write_assoc_rq_header(buf, pduPayloadLen);

    // User Info sub-PDU header.
    *p++ = DUL_TYPEUSERINFO;
    *p++ = 0x00;
    put_u16_be(p, userInfoPayload);

    // Two identical, well-formed User Identity Negotiation sub-items. Each one
    // parses successfully on its own; it is the duplicate that must be rejected.
    for (int i = 0; i < numSubItems; ++i) {
        *p++ = DUL_TYPENEGOTIATIONOFUSERIDENTITY_REQ;  // item type 0x58
        *p++ = 0x00;                                   // reserved
        put_u16_be(p, USER_IDENT_BODY_BYTES);          // item length = 6
        *p++ = 1;                                      // user identity type = 1 (username)
        *p++ = 0x00;                                   // positive response requested = no
        put_u16_be(p, 0);                              // primary-field length   = 0
        put_u16_be(p, 0);                              // secondary-field length = 0
    }

    OFCHECK_EQUAL(OFstatic_cast(unsigned long, p - buf), totalLen);

    PRV_ASSOCIATEPDU assoc;
    OFCondition cond = parseAssociate(buf, pduPayloadLen, &assoc);

    // The duplicate sub-item must be rejected.
    OFCHECK(cond.bad());
    // Error-path cleanup ran: the first sub-item was freed and the pointer cleared.
    OFCHECK(assoc.userInfo.usrIdent == NULL);

    delete[] buf;
}


/* Write a sub-item (Abstract/Transfer Syntax) into buf and advance the cursor:
 * 4-byte sub-PDU header (type, reserved, 2-byte length) followed by `uid`. */
static void put_syntax_subitem(unsigned char *&p, unsigned char type, const char *uid)
{
    const unsigned short uidLen = OFstatic_cast(unsigned short, strlen(uid));
    *p++ = type;
    *p++ = 0x00;                                 // reserved
    put_u16_be(p, uidLen);                       // sub-item length
    for (unsigned short i = 0; i < uidLen; ++i)
        *p++ = OFstatic_cast(unsigned char, uid[i]);
}


/* Regression test for the remotely-triggerable, unauthenticated memory leak in
 * parseAssociate()'s presentation-context error branch.
 * parsePresentationContext() creates context->transferSyntaxList via
 * LST_Create() and enqueues a DUL_SUBITEM for every Transfer Syntax it parses
 * before a later, malformed sub-item makes it fail. The old error path ran a
 * plain free(context), releasing only the struct and leaking the list head plus
 * every already-parsed transfer-syntax sub-item. As the trigger is reached
 * during A-ASSOCIATE handling -- before any AE-title/access-control check -- a
 * remote peer could drive unbounded heap growth (DoS).
 *
 * Payload: one A-ASSOCIATE-RQ presentation context holding a valid Abstract
 * Syntax and N valid Transfer Syntaxes, followed by a Transfer Syntax sub-item
 * whose declared length exceeds the bytes remaining in the context. That makes
 * parseSubItem() return DULC_ILLEGALPDULENGTH AFTER the N transfer syntaxes have
 * been enqueued -- exactly the leaked set. The functional assertions below catch
 * a regression in the cleanup logic (e.g. accidental double-free or crash); the
 * leak itself is only directly observable under tools like LeakSanitizer.
 */
OFTEST(dcmnet_parseAssociate_presCtx_malformed_transferSyntax)
{
    const char *abstractSyntax = "1.2.840.10008.1.1";   // Verification SOP Class
    const char *transferSyntax = "1.2.840.10008.1.2";   // Implicit VR Little Endian
    const int   validTS        = 5;                     // transfer syntaxes leaked pre-fix

    // Presentation-context header counted inside context->length:
    // contextID(1) + reserved(1) + result(1) + reserved(1).
    const unsigned long PRES_CTX_SUBHEADER_BYTES = 4;
    const unsigned long asBytes    = SUBPDU_HEADER_BYTES + OFstatic_cast(unsigned long, strlen(abstractSyntax));
    const unsigned long tsBytes    = SUBPDU_HEADER_BYTES + OFstatic_cast(unsigned long, strlen(transferSyntax));
    // The trailing malformed Transfer Syntax contributes only its 4-byte header
    // on the wire; its length field then claims more data than remains.
    const unsigned long badTsBytes = SUBPDU_HEADER_BYTES;

    // context->length covers everything after the 2-byte presentation-context
    // length field: the 4-byte sub-header, the abstract syntax, the N valid
    // transfer syntaxes and the malformed transfer-syntax header.
    const unsigned short presCtxLength = OFstatic_cast(unsigned short,
        PRES_CTX_SUBHEADER_BYTES + asBytes
        + OFstatic_cast(unsigned long, validTS) * tsBytes + badTsBytes);

    // The whole presentation-context item adds its own type+reserved+length
    // (SUBPDU_HEADER_BYTES) in front of context->length.
    const unsigned long presCtxItemBytes = SUBPDU_HEADER_BYTES + presCtxLength;
    const unsigned long totalLen = ASSOC_RQ_HEADER_BYTES + presCtxItemBytes;
    const unsigned long pduPayloadLen = totalLen - PDU_PREAMBLE_BYTES;

    unsigned char *buf = new unsigned char[totalLen];
    unsigned char *p = write_assoc_rq_header(buf, pduPayloadLen);

    // Presentation Context item header.
    *p++ = DUL_TYPEPRESENTATIONCONTEXTRQ;
    *p++ = 0x00;                                 // reserved
    put_u16_be(p, presCtxLength);                // presentation-context length
    *p++ = 0x01;                                 // presentation context ID
    *p++ = 0x00;                                 // reserved
    *p++ = 0x00;                                 // result/reason (ignored for RQ)
    *p++ = 0x00;                                 // reserved

    // One Abstract Syntax, then N well-formed Transfer Syntaxes that are parsed
    // and enqueued into context->transferSyntaxList before the failure.
    put_syntax_subitem(p, DUL_TYPEABSTRACTSYNTAX, abstractSyntax);
    for (int i = 0; i < validTS; ++i)
        put_syntax_subitem(p, DUL_TYPETRANSFERSYNTAX, transferSyntax);

    // Malformed trailing Transfer Syntax: only the 4-byte header is present, but
    // the length field claims a full UID, so parseSubItem() trips its
    // "subitem claims to be larger than the containing PDU" check and returns
    // an error -- after the N transfer syntaxes above were already enqueued.
    *p++ = DUL_TYPETRANSFERSYNTAX;
    *p++ = 0x00;                                 // reserved
    put_u16_be(p, OFstatic_cast(unsigned short, strlen(transferSyntax)));

    OFCHECK_EQUAL(OFstatic_cast(unsigned long, p - buf), totalLen);

    PRV_ASSOCIATEPDU assoc;
    OFCondition cond = parseAssociate(buf, pduPayloadLen, &assoc);

    // The malformed presentation context must be rejected.
    OFCHECK(cond.bad());
    // Error-path cleanup ran without crashing: the presentation context list was
    // destroyed and its head pointer cleared.
    OFCHECK(assoc.presentationContextList == NULL);

    delete[] buf;
}


/* Regression test for the parser desynchronisation in
 * UserIdentityNegotiationSubItemRQ::parseFromBuffer().
 *
 * For every identity type the sub-item on the wire carries a secondary-field
 * length field plus that many secondary-field bytes. The value is only stored
 * for USERNAME+PASSWORD (type 2), but the declared bytes are always present in
 * the item. parseFromBuffer() used to add the secondary-field length to its
 * returned bytesRead only inside the password branch; for any other identity
 * type with a non-zero secondary field it under-reported bytesRead by exactly
 * the secondary-field length. The caller (parseUserInfo) advances by bytesRead,
 * so the trailing secondary-field bytes were re-parsed as a bogus following
 * User-Info sub-item.
 *
 * This whitebox test calls parseFromBuffer() directly on a single, well-formed
 * type-1 (username) sub-item with a non-empty primary AND secondary field and
 * asserts that the whole item is consumed (bytesRead == availData). Pre-fix,
 * bytesRead is short by the secondary-field length.
 */
OFTEST(dcmnet_parseUserIdentity_secondaryField_bytesRead)
{
    const unsigned short primLen = 3;
    const unsigned short secLen  = 5;
    // item body = identity type(1) + pos-response(1) + prim-length(2)
    //             + primField + sec-length(2) + secField
    const unsigned short itemLength = OFstatic_cast(unsigned short, 6 + primLen + secLen);
    // whole sub-item on the wire = 4-byte sub-PDU header + item body
    const unsigned long availData = SUBPDU_HEADER_BYTES + itemLength;

    unsigned char *buf = new unsigned char[availData];
    unsigned char *p = buf;
    *p++ = DUL_TYPENEGOTIATIONOFUSERIDENTITY_REQ;   // item type 0x58
    *p++ = 0x00;                                    // reserved
    put_u16_be(p, itemLength);                      // item length
    *p++ = ASC_USER_IDENTITY_USER;                  // identity type 1 (username, no password)
    *p++ = 0x00;                                    // positive response requested = no
    put_u16_be(p, primLen);                         // primary-field length
    for (unsigned short i = 0; i < primLen; ++i) *p++ = OFstatic_cast(unsigned char, 'A' + i);
    put_u16_be(p, secLen);                          // secondary-field length (non-zero!)
    for (unsigned short i = 0; i < secLen;  ++i) *p++ = OFstatic_cast(unsigned char, 'a' + i);

    OFCHECK_EQUAL(OFstatic_cast(unsigned long, p - buf), availData);

    UserIdentityNegotiationSubItemRQ item;
    unsigned long bytesRead = 0;
    OFCondition cond = item.parseFromBuffer(buf, bytesRead, availData);

    // The item is well-formed and must parse successfully...
    OFCHECK(cond.good());
    // ...and the parser must report the ENTIRE sub-item as consumed, including
    // the secondary field, so the caller does not re-parse the trailing bytes.
    OFCHECK_EQUAL(bytesRead, availData);

    delete[] buf;
}


/* Regression test for DcmAssociationConfiguration copy constructor and copy
 * assignment operator copying alwaysAcceptDefaultRole_.
 *
 * Pre-fix, the copy constructor left alwaysAcceptDefaultRole_ uninitialized
 * (triggering UBSan "load of value ..., which is not a valid value for type
 * 'OFBool'" during evaluateAssociationParameters), and operator=() reset
 * alwaysAcceptDefaultRole_ to OFFalse via clear() without copying arg's value.
 */
static void check_assoc_cfg_default_role_behavior(
    const DcmAssociationConfiguration& cfg,
    const OFBool expectAccepted)
{
    const char *tsList[] = { UID_LittleEndianImplicitTransferSyntax };
    T_ASC_Association assoc;
    std::memset(&assoc, 0, sizeof(assoc));

    OFCHECK(ASC_createAssociationParameters(&assoc.params, ASC_DEFAULTMAXPDU, 10).good());
    OFCHECK(ASC_addPresentationContext(
        assoc.params, 1, UID_VerificationSOPClass, tsList, 1, ASC_SC_ROLE_DEFAULT).good());
    OFCHECK(cfg.evaluateAssociationParameters("DEFAULT", assoc).good());

    if (expectAccepted)
    {
        OFCHECK_EQUAL(ASC_countAcceptedPresentationContexts(assoc.params), 1);
        OFCHECK_EQUAL(ASC_findAcceptedPresentationContextID(&assoc, UID_VerificationSOPClass), 1);
    }
    else
    {
        OFCHECK_EQUAL(ASC_countAcceptedPresentationContexts(assoc.params), 0);
        OFCHECK_EQUAL(ASC_findAcceptedPresentationContextID(&assoc, UID_VerificationSOPClass), 0);
    }

    OFCHECK(ASC_destroyAssociationParameters(&assoc.params).good());
}

OFTEST(dcmnet_assoc_cfg_copy_alwaysAcceptDefaultRole)
{
    DcmAssociationConfiguration defaultCfg;
    OFCHECK(defaultCfg.addTransferSyntax("TS_DEFAULT", UID_LittleEndianImplicitTransferSyntax).good());
    OFCHECK(defaultCfg.addPresentationContext("CTX_DEFAULT", UID_VerificationSOPClass, "TS_DEFAULT", OFFalse).good());
    OFCHECK(defaultCfg.addRole("ROLE_SCP", UID_VerificationSOPClass, ASC_SC_ROLE_SCP).good());
    OFCHECK(defaultCfg.addProfile("DEFAULT", "CTX_DEFAULT", "ROLE_SCP").good());

    // Default-constructed OFFalse must be initialized and preserved across copy/assign
    DcmAssociationConfiguration defaultCopy(defaultCfg);
    check_assoc_cfg_default_role_behavior(defaultCopy, OFFalse);

    DcmAssociationConfiguration defaultAssign;
    defaultAssign.setAlwaysAcceptDefaultRole(OFTrue);
    defaultAssign = defaultCfg;
    check_assoc_cfg_default_role_behavior(defaultAssign, OFFalse);

    // Explicit OFTrue must be preserved across copy/assign
    DcmAssociationConfiguration acceptCfg(defaultCfg);
    acceptCfg.setAlwaysAcceptDefaultRole(OFTrue);

    DcmAssociationConfiguration acceptCopy(acceptCfg);
    check_assoc_cfg_default_role_behavior(acceptCopy, OFTrue);

    DcmAssociationConfiguration acceptAssign;
    acceptAssign = acceptCfg;
    check_assoc_cfg_default_role_behavior(acceptAssign, OFTrue);
}

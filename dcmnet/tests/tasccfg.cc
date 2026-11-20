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
 *  Purpose: tests for class DcmAssociationConfiguration
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmnet/dcasccfg.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmdata/dcuid.h"

#include <cstring>                    /* for memset() */
#include <new>                        /* for placement new */


/* Evaluate a request that proposes Verification in Default role against
 * profile "DEFAULT" of the given configuration, which only supports the
 * SCP role. The presentation context is accepted only if the configuration
 * has "always accept Default role" enabled. Returns the number of accepted
 * presentation contexts (0 or 1).
 */
static int countAcceptedContexts(const DcmAssociationConfiguration& cfg)
{
    const char *tsList[] = { UID_LittleEndianImplicitTransferSyntax };
    T_ASC_Association assoc;
    memset(&assoc, 0, sizeof(assoc));
    OFCHECK(ASC_createAssociationParameters(&assoc.params, ASC_DEFAULTMAXPDU, dcmConnectionTimeout.get()).good());
    OFCHECK(ASC_addPresentationContext(assoc.params, 1, UID_VerificationSOPClass, tsList, 1, ASC_SC_ROLE_DEFAULT).good());
    OFCHECK(cfg.evaluateAssociationParameters("DEFAULT", assoc).good());
    const int result = ASC_countAcceptedPresentationContexts(assoc.params);
    OFCHECK(ASC_destroyAssociationParameters(&assoc.params).good());
    return result;
}

/* Copy-construct a configuration into storage pre-filled with the given byte,
 * so that a member which the copy constructor forgets to initialize does not
 * hold the expected value by chance, and evaluate it.
 */
static int countAcceptedContextsOfCopy(const DcmAssociationConfiguration& src, const unsigned char fill)
{
    unsigned char *buf = new unsigned char[sizeof(DcmAssociationConfiguration)];
    memset(buf, fill, sizeof(DcmAssociationConfiguration));
    DcmAssociationConfiguration *copy = new (buf) DcmAssociationConfiguration(src);
    const int result = countAcceptedContexts(*copy);
    copy->~DcmAssociationConfiguration();
    delete[] buf;
    return result;
}

OFTEST(dcmnet_assoc_cfg_copy_alwaysAcceptDefaultRole)
{
    DcmAssociationConfiguration cfg;
    OFCHECK(cfg.addTransferSyntax("TS", UID_LittleEndianImplicitTransferSyntax).good());
    OFCHECK(cfg.addPresentationContext("PC", UID_VerificationSOPClass, "TS", OFFalse).good());
    OFCHECK(cfg.addRole("ROLE", UID_VerificationSOPClass, ASC_SC_ROLE_SCP).good());
    OFCHECK(cfg.addProfile("DEFAULT", "PC", "ROLE").good());
    OFCHECK_EQUAL(countAcceptedContexts(cfg), 0);

    DcmAssociationConfiguration acceptCfg(cfg);
    acceptCfg.setAlwaysAcceptDefaultRole(OFTrue);
    OFCHECK_EQUAL(countAcceptedContexts(acceptCfg), 1);

    // copy constructor
    OFCHECK_EQUAL(countAcceptedContextsOfCopy(cfg, 0xff), 0);
    OFCHECK_EQUAL(countAcceptedContextsOfCopy(acceptCfg, 0x00), 1);

    // copy assignment
    DcmAssociationConfiguration assigned;
    assigned = acceptCfg;
    OFCHECK_EQUAL(countAcceptedContexts(assigned), 1);
    assigned.setAlwaysAcceptDefaultRole(OFTrue);
    assigned = cfg;
    OFCHECK_EQUAL(countAcceptedContexts(assigned), 0);
}

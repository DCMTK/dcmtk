/*
 *
 *  Copyright (C) 2011-2020, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: test program for the data dictionary
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcdicent.h"

OFTEST(dcmdata_readingDataDictionary)
{
    // Does loading the global data dictionary work?
    if (!dcmDataDict.isDictionaryLoaded())
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
}

OFTEST(dcmdata_usingDataDictionary)
{
    // All tests run with an DCMDICTPATH external dictionary defined. This test
    // expects an empty dictionary though, so we disable the DCMDICTPATH
    // dictionary in this test by unsetting the environment variable.
#ifdef _WIN32
    _putenv_s("DCMDICTPATH", "");
#else
    setenv("DCMDICTPATH","", 1 /* overwrite */);
#endif

    // This dictionary will only contain the skeleton entries
    DcmDataDictionary localDict(OFFalse, OFFalse);
    DcmDictEntry *entry1;
    DcmDictEntry *entry2;
    const char *name = "TestEntry";
    const char *creator = "Test";
    DcmTagKey key1(2, 2);
    DcmTagKey key2(4, 5);
    int entries;

#define checkDictionary(num_entries, e_name, key1_c, key2_c, key1_nc, key2_nc) \
    entries  = localDict.numberOfNormalTagEntries();                           \
    entries += localDict.numberOfRepeatingTagEntries();                        \
    OFCHECK_EQUAL(localDict.numberOfSkeletonEntries() + num_entries, entries); \
    OFCHECK_EQUAL(localDict.numberOfEntries(), num_entries);                   \
    OFCHECK(!localDict.isDictionaryLoaded());                                  \
    OFCHECK(localDict.findEntry("UnexistingEntry") == NULL);                   \
    OFCHECK(localDict.findEntry(name) == e_name);                              \
    OFCHECK(localDict.findEntry(key1, creator) == key1_c);                     \
    OFCHECK(localDict.findEntry(key2, creator) == key2_c);                     \
    OFCHECK(localDict.findEntry(key1, NULL) == key1_nc);                       \
    OFCHECK(localDict.findEntry(key2, NULL) == key2_nc);

    checkDictionary(0, NULL, NULL, NULL, NULL, NULL);

    entry1 = new DcmDictEntry(2, 2, DcmVR(EVR_PN), name, 0, 1,
            "test", OFTrue, "Test");
    entry2 = new DcmDictEntry(4, 4, 4, 6, DcmVR(EVR_PN), name, 0, 1,
            "test", OFTrue, "Test");
    localDict.addEntry(entry1);
    localDict.addEntry(entry2);

    checkDictionary(2, entry1, entry1, entry2, NULL, NULL);

    // This also gets rid of the skeleton entries!
    localDict.clear();

    OFCHECK_EQUAL(localDict.numberOfNormalTagEntries(), 0);
    OFCHECK_EQUAL(localDict.numberOfRepeatingTagEntries(), 0);
    OFCHECK_EQUAL(localDict.numberOfSkeletonEntries(), 0);
    OFCHECK_EQUAL(localDict.numberOfEntries(), 0);

    checkDictionary(0, NULL, NULL, NULL, NULL, NULL);

    // Load the skeleton dictionary again
    OFCHECK(localDict.reloadDictionaries(OFFalse, OFFalse));

    checkDictionary(0, NULL, NULL, NULL, NULL, NULL);

    entry1 = new DcmDictEntry(2, 2, DcmVR(EVR_PN), name, 0, 1,
            "test", OFTrue, NULL);
    entry2 = new DcmDictEntry(4, 4, 4, 6, DcmVR(EVR_PN), name, 0, 1,
            "test", OFTrue, NULL);
    localDict.addEntry(entry1);
    localDict.addEntry(entry2);

    checkDictionary(2, entry1, NULL, NULL, entry1, entry2);

    // Now we will overwrite entries
    entry1 = new DcmDictEntry(2, 2, DcmVR(EVR_PN), "overwritten", 0, 1,
            "test", OFTrue, NULL);
    entry2 = new DcmDictEntry(4, 4, 4, 6, DcmVR(EVR_PN), "overwritten", 0, 1,
            "test", OFTrue, NULL);
    localDict.addEntry(entry1);
    localDict.addEntry(entry2);

    checkDictionary(2, NULL, NULL, NULL, entry1, entry2);

#undef checkDictionary
}

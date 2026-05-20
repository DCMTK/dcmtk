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
 *  Module:  dcmwlm
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for the AE title validation used by the wlmscpfs
 *           filesystem worklist SCP to guard against path traversal
 *           through the Called Application Entity Title.
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmwlm/wlfsim.h"


// Helper: build an OFString that contains a single NUL byte in the middle
// of an otherwise printable AE title. OFString tolerates embedded NUL bytes
// when constructed from a (pointer, length) pair, which is how a NUL byte
// could enter wlmscpfs from the wire.
static OFString aeTitleWithEmbeddedNul()
{
    const char raw[] = { 'A', 'B', '\0', 'C', 'D' };
    return OFString(raw, sizeof(raw));
}


OFTEST(dcmwlm_aetitle_validation)
{
    // ----- Path traversal payloads from the original bug report. -----
    // All of these must be rejected; if any of them is accepted, the
    // CVE-2026-... path traversal hole is open again.

    // 16-byte payload from the proof of concept ("../secret/VICTIM").
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("../secret/VICTIM"));

    // Multi-AET demo ("../CARDIOLOGY").
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("../CARDIOLOGY"));

    // Plain parent reference and self reference.
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem(".."));
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("."));

    // Windows-style separator.
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("..\\secret"));

    // Leading-dot variants and dots in the middle. None of these are
    // path-traversal payloads on their own, but we reject any AE title
    // that contains a dot, see the documentation of
    // IsValidAETitleForFilesystem() for the rationale.
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem(".foo"));
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("foo."));
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("foo..bar"));
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("MY.AE"));

    // ----- Other unsafe input. -----

    // Empty AE title.
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem(""));

    // Too long (17 bytes; the DICOM AE VR allows at most 16).
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("ABCDEFGHIJKLMNOPQ"));

    // Embedded NUL byte.
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem(aeTitleWithEmbeddedNul()));

    // Control character (tab).
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("AB\tCD"));

    // High-range byte.
    {
        OFString withHigh;
        withHigh.append("AE", 2);
        withHigh.append(1, '\xff');
        OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem(withHigh));
    }

    // Shell metacharacters: rejected because
    // OFStandard::sanitizeAETitle() would replace them.
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("AE;rm"));
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("AE|cat"));
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("AE`id`"));
    OFCHECK(!WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("AE$VAR"));

    // ----- Legitimate AE titles must still be accepted. -----

    // Plain alphanumeric, mixed case.
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("WORKLIST"));
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("CARDIOLOGY"));
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("MyAE"));

    // Single character (minimum length).
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("A"));

    // Maximum length (16 bytes).
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("ABCDEFGHIJKLMNOP"));

    // Conformant punctuation that is also safe as a path component.
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("MY-AE_1"));
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("AE@HOST"));
    OFCHECK(WlmFileSystemInteractionManager::IsValidAETitleForFilesystem("AE:1"));
}

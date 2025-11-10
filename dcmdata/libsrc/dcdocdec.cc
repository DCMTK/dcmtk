/*
 *
 *  Copyright (C) 2007-2025, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, Tingyan Xu
 *
 *  Purpose: Helper class for extracting encapsulated file from a DICOM encapsulated storage object
 *
 */

//make sure OS specific configuration is included first
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdocdec.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"

BEGIN_EXTERN_C
#include <fcntl.h>                       /*  for O_BINARY */
#ifdef HAVE_IO_H
#include <io.h>                          /* for setmode() on Windows */
#endif
END_EXTERN_C


DcmDocumentDecapsulator::DcmDocumentDecapsulator()
: readMode_(ERM_autoDetect)
, inputXfer_(EXS_Unknown)
, execString_(NULL)
, inputFname_(NULL)
, outputFname_(NULL)
, dicomFile_()
{
}


DcmDocumentDecapsulator::~DcmDocumentDecapsulator()
{
}


OFCondition DcmDocumentDecapsulator::loadDICOMFile()
{
    // check filename
    if ((inputFname_ == NULL) || (strlen(inputFname_) == 0))
    {
        DCMDATA_ERROR("invalid input filename: <empty string>");
        return EC_InvalidFilename;
    }

    // read DICOM file
    DCMDATA_INFO("open input file " << inputFname_);
    OFCondition cond = dicomFile_.loadFile(inputFname_, inputXfer_, EGL_noChange, DCM_MaxReadLength, readMode_);
    if (cond.bad())
    {
        DCMDATA_ERROR(cond.text() << ": reading file: " << inputFname_);
        return cond;
    }

    return EC_Normal;
}


OFCondition DcmDocumentDecapsulator::writeEncapsulatedContentToFile()
{
    // check filename
    if ((outputFname_ == NULL) || (strlen(outputFname_) == 0))
    {
        DCMDATA_ERROR("invalid output filename: <empty string>");
        return EC_InvalidFilename;
    }

    // get encapsulated document
    DcmDataset* dataset = dicomFile_.getDataset();
    DcmElement *delem = NULL;
    OFCondition cond = dataset->findAndGetElement(DCM_EncapsulatedDocument, delem);
    if (cond.bad() || delem == NULL)
    {
        DCMDATA_ERROR("encapsulated document missing");
        return EC_MissingAttribute;
    }

    Uint32 len = delem->getLength();
    Uint8 *encapDocment = NULL;
    cond = delem->getUint8Array(encapDocment);
    if (cond.bad() || encapDocment == NULL || len == 0)
    {
        DCMDATA_ERROR("encapsulated Document empty or wrong VR");
        return EC_MissingAttribute;
    }

    // get and check element encapsulated document length
    Uint32 lenElem;
    cond = dataset->findAndGetUint32(DCM_EncapsulatedDocumentLength, lenElem);

    // EncapsulatedDocumentLength Element is invalid or
    // it does not fit the length of the encapsulated document
    // (it has to be equal or equal to EncapsulatedDocumentLength -1)
    if (cond.bad() || (lenElem != len && lenElem != len - 1))
    {
        DCMDATA_DEBUG("EncapsulatedDocumentLength missing or invalid, using length of EncapsulatedDocument");
        lenElem = len;

        // try to retrieve the SOP Class UID, ignore errors
        OFString sopClass;
        (void) dataset->findAndGetOFString(DCM_SOPClassUID, sopClass);

        // try to determine if we need to strip a pad byte at the end of the element
        if (sopClass == UID_EncapsulatedPDFStorage)
        {
            // The PDF format expects files to end with %%EOF followed by CR/LF
            // (although in some cases the CR/LF may be missing or you might only find CR or LF).
            // If the last character of the file is not a CR or LF, and not the
            // letter 'F', we assume it is either trailing garbage or a pad byte, and remove it.
            if (encapDocment[lenElem-1] != 10 && encapDocment[lenElem-1] != 13 && encapDocment[lenElem-1] != 'F')
            {
                DCMDATA_DEBUG("removing pad byte at end of encapsulated document");
                --lenElem;
            }
        }
        else if (sopClass == UID_EncapsulatedCDAStorage)
        {
            // CDA documents end with a closing XML tag, optionally followed by whitespace.
            // If the last character of the file is not a CR ('\r', 13) or LF ('\n', 10), and not the
            // letter '>', we assume it is either trailing garbage or a pad byte, and remove it.
            if (encapDocment[lenElem - 1] != '\n' && encapDocment[lenElem - 1] != '\r' && encapDocment[lenElem - 1] != '>')
            {
                DCMDATA_DEBUG("removing pad byte at end of encapsulated document");
                --lenElem;
            }
        }
        else if (sopClass == UID_EncapsulatedSTLStorage || sopClass == UID_EncapsulatedOBJStorage || sopClass == UID_EncapsulatedMTLStorage)
        {
            // STL, OBJ and MTL are text formats. We remove a trailing null byte, but nothing else.
            if (encapDocment[lenElem - 1] == '\0')
            {
                DCMDATA_DEBUG("removing pad byte at end of encapsulated document");
                --lenElem;
            }
        }
        else
        {
            DCMDATA_DEBUG("Unknown encapsulated document SOP class, not removing any pad byte");
        }
    }

    DCMDATA_INFO( "writing encapsulated document to " << outputFname_);

    if (strcmp(outputFname_, "-") == 0)
    {
#ifdef _WIN32
        // Set "stdout" to binary mode
        if (setmode(fileno(stdout), O_BINARY) == -1)
        {
            DCMDATA_ERROR("Failed to switch stdout to binary mode");
            return makeOFCondition(OFM_dcmdata, 19, OF_error, "file write error");
        }
#endif

        // write encapsulated document to stdout
        if (lenElem != fwrite(encapDocment, 1, lenElem, stdout))
        {
            DCMDATA_ERROR("write error to stdout");
            return makeOFCondition(OFM_dcmdata, 19, OF_error, "file write error");
        }
    }
    else
    {
        // write encapsulated document to file
        OFFile encapfile;
        if (!encapfile.fopen(outputFname_, "wb"))
        {
            DCMDATA_ERROR("unable to create file " << outputFname_);
            return makeOFCondition(OFM_dcmdata, 19, OF_error, "file open error");
        }

        if (lenElem != encapfile.fwrite(encapDocment, 1, lenElem))
        {
            DCMDATA_ERROR("write error in file " << outputFname_);
            (void) encapfile.fclose();
            return makeOFCondition(OFM_dcmdata, 19, OF_error, "file write error");
        }

        if (encapfile.fclose())
        {
            DCMDATA_ERROR("write error in file " << outputFname_);
            return makeOFCondition(OFM_dcmdata, 19, OF_error, "file close error");
        }
    }

    DCMDATA_INFO( "conversion successful");
    return EC_Normal;
}


/** Replace all occurrences of pattern in srcstr with substitute and return
 *  the result as a new OFString variable.
 *  @param srcstr source string
 *  @param pattern pattern string to be substituted.
 *  @param substitute substitute for occurences of pattern in srcstr
 *  @return string with patterns replaced
 */
static OFString replaceChars(const OFString &srcstr, const OFString &pattern, const OFString &substitute)
{
    OFString result = srcstr;
    size_t pos = 0;

    while (pos != OFString_npos)
    {
        pos = result.find(pattern, pos);

        if (pos != OFString_npos)
        {
            result.replace(pos, pattern.size(), substitute);
            pos += substitute.size();
        }
    }

    return result;
}


#define FILENAME_PLACEHOLDER "#f"

OFCondition DcmDocumentDecapsulator::executeCommand()
{
    if (execString_)
    {
        OFString cmdStr = replaceChars(execString_, OFString(FILENAME_PLACEHOLDER), outputFname_);

        // Execute command and return result
        if (system(cmdStr.c_str()) != 0) return EC_CommandLineFailed;
    }
    return EC_Normal;
}

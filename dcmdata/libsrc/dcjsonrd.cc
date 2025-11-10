/*
 *
 *  Copyright (C) 2024-2025, OFFIS e.V.
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
 *  Author:  Tingyan Xu, Marco Eichelberg
 *
 *  Purpose: Class for converting JSON DICOM documents to binary DICOM files
 *
 */

#include "dcmtk/config/osconfig.h"      /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcjsonrd.h"
#include "dcmtk/ofstd/ofcond.h"         /* for class OFCondition */
#include "dcmtk/ofstd/offile.h"         /* for class OFFile */
#include "dcmtk/dcmdata/dcerror.h"      /* for dcmdata logger macros */
#include "dcmtk/dcmdata/dcfilefo.h"     /* for class DcmFileFormat */
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdeftag.h"     /* for tag keys */
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcvrov.h"       /* for DcmOther64bitVeryLong */

#include <climits>                      /* for INT_MAX */

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

// Private creator identification string for files
// containing a list of datasets as a private sequence
#define JSON2DCM_PRIVATE_RESERVATION "JSON2DCM_LIST_OF_DATASETS"

// Begin with a 1 MByte buffer when reading from stdin.
// Buffer size will increase if necessary.
#define JSON2DCM_STDIN_BLOCKSIZE 1048576


DcmJSONReader::DcmJSONReader()
: jsonDataset_(NULL)
, jsonDatasetLen_(0)
, tokenArray_(NULL)
, tokenNumber_(0)
, ignoreBulkdataURIPolicy_(OFFalse)
, stopOnErrorPolicy_(OFTrue)
, ignoreMetaInfoPolicy_(OFFalse)
, arrayHandlingPolicy_(-1)
, xferSyntax_(EXS_LittleEndianExplicit)
, permittedBulkdataDirs_()
{
}


DcmJSONReader::~DcmJSONReader()
{
    clear();
}


void DcmJSONReader::clear()
{
    delete[] jsonDataset_;
    jsonDataset_ = NULL;
    delete[] tokenArray_;
    tokenArray_ = NULL;
    jsonDatasetLen_ = 0;
    tokenNumber_ = 0;
}


OFCondition DcmJSONReader::readJSONFile(const char *ifname)
{
    if (NULL == ifname) return EC_IllegalParameter;

    OFFile jsonFile;
    if (! jsonFile.fopen(ifname, "rb"))
    {
        OFString s("(unknown error code)");
        jsonFile.getLastErrorString(s);
        DCMDATA_ERROR("failed to open JSON file '" << ifname << "': " << s);
        return makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
    }

    // obtain file size
    const size_t len = OFStandard::getFileSize(ifname);
    char *jsonString = new (std::nothrow) char[len + 1];
    if (jsonString == NULL)
    {
        DCMDATA_ERROR("out of memory: failed to allocate buffer for JSON file");
        return EC_MemoryExhausted;
    }

    // read the whole file to buffer
    jsonString[len] = '\0';
    size_t res = jsonFile.fread(jsonString, 1, len);
    jsonFile.fclose();
    if (res != len)
    {
        OFString s("(unknown error code)");
        jsonFile.getLastErrorString(s);
        delete[] jsonString;
        return makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
    }

    // file was successfully read into buffer. Store the result, and in the process,
    // clear the token array which is now invalid
    clear();
    jsonDataset_ = jsonString;
    jsonDatasetLen_ = len;

    return EC_Normal;
}


OFCondition DcmJSONReader::readJSONFromStdin()
{
    size_t bufSize = JSON2DCM_STDIN_BLOCKSIZE;
    size_t bufFree;
    size_t bytesInBuffer = 0;
    size_t readResult;
    char *jsonString = NULL;;
    char *oldBuf = NULL;
    char *writePtr;
    do
    {
        if (jsonString == NULL) jsonString = new (std::nothrow) char[bufSize+1];
        if (jsonString == NULL)
        {
            DCMDATA_ERROR("out of memory: failed to allocate buffer for reading JSON dataset from stdin");
            delete[] oldBuf;
            return EC_MemoryExhausted;
        }
        if (oldBuf)
        {
            memmove(jsonString, oldBuf, bytesInBuffer);
            delete[] oldBuf;
            oldBuf = NULL;
        }
        writePtr = jsonString + bytesInBuffer;
        bufFree = bufSize - bytesInBuffer;
        readResult = fread(writePtr, 1, bufFree, stdin);
        bytesInBuffer += readResult;
        bufFree -= readResult;
        if ((! feof(stdin)) && (bufFree == 0))
        {
            // we need more buffer. Double the size.
            oldBuf = jsonString;
            jsonString = NULL;
            bufSize *= 2;
        }
        else if (feof(stdin))
        {
            jsonString[bytesInBuffer] = '\0';

            // dataset was successfully read into buffer. Store the result, and in the process,
            // clear the token array which is now invalid
            clear();
            jsonDataset_ = jsonString;
            jsonDatasetLen_ = bytesInBuffer;
            return EC_Normal;
        }
    } while (OFTrue);
}


OFCondition DcmJSONReader::reserveTokens()
{
    if ((NULL == jsonDataset_) || (jsonDatasetLen_ < 2))
        return EC_IllegalCall;

    OFJsmnParser jsmnParser;
    jsmn_init(&jsmnParser);
    int tokenNum = jsmn_parse(&jsmnParser, jsonDataset_, jsonDatasetLen_, NULL, 0);

    if (tokenNum <= 0)
        return EC_InvalidJSONContent;

    // delete old token array, if still present
    tokenNumber_ = 0;
    delete[] tokenArray_;

    // allocate one additional token
    tokenArray_ = new (std::nothrow) OFJsmnToken[tokenNum+1];
    if (NULL == tokenArray_)
    {
        DCMDATA_ERROR("out of memory: failed to allocate JSON token array");
        return EC_MemoryExhausted;
    }

    // initialize token array with zeroes
    memset(tokenArray_, 0, tokenNum * sizeof(OFJsmnToken));

    // fill dummy token at the end of the array with values
    // that allow the loop in parseElement() to reliably terminate
    tokenArray_[tokenNum].start = INT_MAX;
    tokenArray_[tokenNum].end = INT_MAX;
    tokenArray_[tokenNum].size = 0;

    // finally, store number of tokens
    tokenNumber_ = tokenNum;

    DCMDATA_TRACE("JSMN tokens reserved: " << tokenNum);
    return EC_Normal;
}


void DcmJSONReader::getTokenContent(OFString& value, OFJsmnTokenPtr t)
{
    int size = t->end - t->start;

    // remember the character immediately following the token
    char c = jsonDataset_[t->start+size];

    // convert the token string to a null terminated C string
    jsonDataset_[t->start+size] = '\0';

    // copy token string into output parameter
    value = jsonDataset_ + t->start;

    // restore array
    jsonDataset_[t->start+size] = c;
    return;
}


OFCondition DcmJSONReader::dumpJSONTokenArray()
{
    if ((NULL == tokenArray_) || (tokenNumber_ < 1))
        return EC_IllegalCall;

    const char *json_type;
    OFString json_value;
    fprintf(stderr, "============================== BEGIN JSON DUMP ==============================\n");
    for (int i=0; i < tokenNumber_; ++i)
    {
        switch (tokenArray_[i].type)
        {
            case JSMN_OBJECT:
              json_type = "object,    size=";
              break;
            case JSMN_ARRAY:
              json_type = "array,     size=";
              break;
            case JSMN_STRING:
              json_type = "string,    size=";
              break;
            case JSMN_PRIMITIVE:
              json_type = "primitive, size=";
              break;
            case JSMN_UNDEFINED:
            default:
              json_type = "undefined, size=";
              break;
        }

        getTokenContent(json_value, &tokenArray_[i]);
        fprintf(stderr, "%06d: type=%s%04d,  value=%s\n", i+1, json_type, tokenArray_[i].size, json_value.c_str());
    }
    fprintf(stderr, "=============================== END JSON DUMP ===============================\n");
    return EC_Normal;
}


OFCondition DcmJSONReader::parseJSON()
{
    if ((NULL == tokenArray_) || (tokenNumber_ < 1))
        return EC_IllegalCall;

    OFJsmnParser jsmnParser;
    jsmn_init(&jsmnParser);

    int parsRes = jsmn_parse(&jsmnParser, jsonDataset_, jsonDatasetLen_, tokenArray_, tokenNumber_);

    if (parsRes < 0)
    {
       // a parse error in jsmn_parse occurred
        DCMDATA_ERROR("parse error in JSON file");
        if (parsRes == JSMN_ERROR_INVAL)
            return EC_InvalidCharacter;
            else return EC_InvalidJSONContent;
    }

    return EC_Normal;
}


OFCondition DcmJSONReader::storeInlineBinaryValue(
    DcmElement& element,
    Uint8 *data,
    size_t length)
{
    OFCondition result = EC_Normal;
    if (NULL == data) length = 0;
    DcmEVR evr = element.getVR();
    if (evr == EVR_OW)
    {
        /* Base64 decoder produces little endian output data, convert to local byte order */
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Uint16));
        result = element.putUint16Array(OFreinterpret_cast(Uint16 *, data), OFstatic_cast(Uint32, length / sizeof(Uint16)));
    }
    else if (evr == EVR_OF)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Float32));
        result = element.putFloat32Array(OFreinterpret_cast(Float32 *, data), OFstatic_cast(Uint32, length / sizeof(Float32)));
    }
    else if (evr == EVR_OD)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Float64));
        result = element.putFloat64Array(OFreinterpret_cast(Float64 *, data), OFstatic_cast(Uint32, length / sizeof(Float64)));
    }
    else if (evr == EVR_OL)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Uint32));
        result = element.putUint32Array(OFreinterpret_cast(Uint32 *, data), OFstatic_cast(Uint32, length / sizeof(Uint32)));
    }
    else if (evr == EVR_OV)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Uint64));
        result = element.putUint64Array(OFreinterpret_cast(Uint64 *, data), OFstatic_cast(Uint32, length / sizeof(Uint64)));
    }
    else if (evr == EVR_OB || evr == EVR_UN)
    {
        result = element.putUint8Array(data, OFstatic_cast(Uint32, length));
    }
    else
    {
        DCMDATA_ERROR("invalid VR Type for inline value");
        return EC_InvalidVR;
    }

    if (result.bad())
    {
        DCMDATA_ERROR("failed to store inline binary value for DICOM element "  << element.getTag() << ": " << result.text());
    }
    return result;
}


OFCondition DcmJSONReader::storeBulkValue(
    DcmElement& element,
    Uint8 *data,
    size_t length)
{
    OFCondition result = EC_Normal;
    if (NULL == data) length = 0;
    DcmEVR evr = element.getVR();

    if (evr == EVR_OB || evr == EVR_UN)
    {
        // sequence of bytes, no byte swapping necessary
        result = element.putUint8Array(data, OFstatic_cast(Uint32, length));
    }
    else if (evr == EVR_DS || evr == EVR_IS || evr == EVR_LT || evr == EVR_ST || evr == EVR_UT || evr == EVR_UC)
    {
        // sequence of bytes, no byte swapping necessary
        result = element.putString(OFreinterpret_cast(char *, data), OFstatic_cast(Uint32, length));
    }
    else if (evr == EVR_OW || evr == EVR_US)
    {
        // bulk data is always in little endian, convert to local byte order
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Uint16));
        result = element.putUint16Array(OFreinterpret_cast(Uint16 *, data), OFstatic_cast(Uint32, length / sizeof(Uint16)));
    }
    else if (evr == EVR_SS)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Sint16));
        result = element.putSint16Array(OFreinterpret_cast(Sint16 *, data), OFstatic_cast(Uint32, length / sizeof(Sint16)));
    }
    else if (evr == EVR_OL || evr == EVR_UL)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Uint32));
        result = element.putUint32Array(OFreinterpret_cast(Uint32 *, data), OFstatic_cast(Uint32, length / sizeof(Uint32)));
    }
    else if (evr == EVR_SL)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Sint32));
        result = element.putSint32Array(OFreinterpret_cast(Sint32 *, data), OFstatic_cast(Uint32, length / sizeof(Sint32)));
    }
    else if (evr == EVR_OV || evr == EVR_UV)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Uint64));
        result = element.putUint64Array(OFreinterpret_cast(Uint64 *, data), OFstatic_cast(Uint32, length / sizeof(Uint64)));
    }
    else if (evr == EVR_SV)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Sint64));
        result = element.putSint64Array(OFreinterpret_cast(Sint64 *, data), OFstatic_cast(Uint32, length / sizeof(Sint64)));
    }
    else if (evr == EVR_FL || evr == EVR_OF)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Float32));
        result = element.putFloat32Array(OFreinterpret_cast(Float32 *, data), OFstatic_cast(Uint32, length / sizeof(Float32)));
    }
    else if (evr == EVR_FD || evr == EVR_OD)
    {
        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, OFstatic_cast(Uint32, length), sizeof(Float64));
        result = element.putFloat64Array(OFreinterpret_cast(Float64 *, data), OFstatic_cast(Uint32, length / sizeof(Float64)));
    }
    else
    {
        DCMDATA_ERROR("invalid VR Type for bulk data");
        return EC_InvalidVR;
    }

    if (result.bad())
    {
        DCMDATA_ERROR("failed to store bulk data value for DICOM element "  << element.getTag() << ": " << result.text());
    }
    return result;
}


OFCondition DcmJSONReader::processJSONEscapeCharacters(OFString& value)
{
    /*
        \" represents the quotation mark character (U+0022).
        \\ represents the reverse solidus character (U+005C).
        \/ represents the solidus character (U+002F).
        \b represents the backspace character (U+0008).
        \f represents the form feed character (U+000C).
        \n represents the line feed character (U+000A).
        \r represents the carriage return character (U+000D).
        \t represents the character tabulation character (U+0009).
    */
    OFString escapeSeq = "\"\\/bfnrt";

    // find backslash
    for (size_t backSlash = value.find('\\');
        backSlash < value.length();
        backSlash = value.find('\\', backSlash + 1))
    {
        if (value.length() < backSlash + 2)
        {
            // the given string is shorter than expected. just return the string
            DCMDATA_ERROR("incomplete JSON escape sequence");
            return EC_InvalidJSONContent;
        }
        char sigChar = value[backSlash + 1];
        OFString front = value.substr(0, backSlash);
        OFString escString, back, replacement;
        unsigned int unicodeCodepoint = 0;

        if (escapeSeq.find(sigChar) < escapeSeq.length())
        {
            back = value.substr(backSlash + 2);
            escString = sigChar;

            switch (sigChar)
            {
            case 'b':
                unicodeCodepoint = 0x0008;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 'f':
                unicodeCodepoint = 0x000C;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 'n':
                unicodeCodepoint = 0x000A;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 'r':
                unicodeCodepoint = 0x000D;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            case 't':
                unicodeCodepoint = 0x0009;
                replacement = OFstatic_cast(char, unicodeCodepoint);
                break;
            default: // \ / "
                replacement = sigChar;
                break;
            }
        }
        else
        {
            // does not contain the full pattern of "\uXXXX"
            if (sigChar != 'u' || value.length() < backSlash + 6)
            {
                // unknown escape sequence or incomplete unicode code point
                DCMDATA_ERROR("unknown JSON escape sequence or incomplete Unicode code point");
                return EC_InvalidJSONContent;
            }

            escString = value.substr(backSlash + 2, 4);
            // parse escString to unicodeCodepoint
            if (sscanf(escString.c_str(), "%x", &unicodeCodepoint) != 1)
            {
                // Invalid hex code
                DCMDATA_ERROR("invalid hex code in JSON escape sequence");
                return EC_InvalidJSONContent;
            }

            // convert Unicode codepoint to UTF-8 string
            if (unicodeCodepoint < 0x80)
            {
                // UTF-8 sequence with one byte
                replacement = OFstatic_cast(char, unicodeCodepoint);
                back = value.substr(backSlash + 6);
            }
            else if (unicodeCodepoint < 0x800)
            {
                // UTF-8 sequence with two bytes
                replacement = OFstatic_cast(char, ((unicodeCodepoint >> 6) & 0x1F) | 0xC0);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 0) & 0x3F) | 0x80);
                back = value.substr(backSlash + 6);
            }
            else if ((unicodeCodepoint >= 0xD800) && ((unicodeCodepoint < 0xE000)))
            {
                // UTF-16 surrogate pair consisting two Unicode code points
                if ((value.length() < backSlash + 12) || (value[backSlash + 6] != '\\') || (value[backSlash + 7] != 'u'))
                {
                    DCMDATA_ERROR("invalid JSON UTF-16 surrogate pair escape sequence");
                    return EC_InvalidJSONContent;
                }

                escString = value.substr(backSlash + 8, 4);
                unsigned int unicodeCodepoint2;
                if (sscanf(escString.c_str(), "%x", &unicodeCodepoint2) != 1)
                {
                    // Invalid hex code
                    DCMDATA_ERROR("invalid hex code in JSON escape sequence");
                    return EC_InvalidJSONContent;
                }

                // combine the two UTF-16 surrogates to a single Unicode code point.
                // The first code point is the high surrogate (D800..DBFF), the second one the low surrogate (DC00..DFFF)
                unicodeCodepoint = ((unicodeCodepoint - 0xD800) << 10) + unicodeCodepoint2 - 0xDC00 + 0x10000;
                // UTF-8 sequence with four bytes
                replacement = OFstatic_cast(char, ((unicodeCodepoint >> 18) & 0x07) | 0xF0);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 12) & 0x3F) | 0x80);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 6) & 0x3F) | 0x80);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 0) & 0x3F) | 0x80);
                back = value.substr(backSlash + 12);
            }
            else
            {
                // UTF-8 sequence with three bytes
                replacement = OFstatic_cast(char, ((unicodeCodepoint >> 12) & 0x0F) | 0xE0);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 6) & 0x3F) | 0x80);
                replacement += OFstatic_cast(char, ((unicodeCodepoint >> 0) & 0x3F) | 0x80);
                back = value.substr(backSlash + 6);
            }
        }
        DCMDATA_TRACE("the escaped string [" << escString << "] is parsed to UTF-8:"
            << replacement << " - unicode:" << std::hex << unicodeCodepoint);
        value = front + replacement + back;
    }
    return EC_Normal;
}


OFCondition DcmJSONReader::parseElement(
    DcmItem* dataset,
    DcmItem* metaheader,
    OFJsmnTokenPtr& current)
{
    OFCondition result = EC_Normal;
    DCMDATA_TRACE("element at " << current->start);

    // the key for the element tag has to be a string
    if (current->type != JSMN_STRING)
    {
        DCMDATA_ERROR("not a valid DICOM JSON dataset: element tag must be a JSON string");
        return EC_InvalidJSONType;
    }

    DcmTagKey tagkey;
    result = extractTag(current, tagkey);
    if (result.bad())
    {
        if (stopOnErrorPolicy_) return result; else result = EC_Normal;
    }
    DcmTag dcmTag(tagkey);

    // the next token contains the whole element encapsulated into a JSON object.
    current++;
    if (current->type != JSMN_OBJECT)
    {
        DCMDATA_ERROR("not a valid DICOM JSON dataset: element content must be a JSON object");
        return EC_InvalidJSONType;
    }

    // number of attributes for the element
    int contentSize = current->size;
    DcmElement* newElem = NULL;
    current++;

    // examine the attributes
    OFJsmnTokenPtr vrToken = NULL, valueToken = NULL;
    OFString valueType;
    for (int count = 0; count < contentSize; count++)
    {
        OFString attrName;
        getTokenContent(attrName, current);
        attrName = OFStandard::toLower(attrName);
        DCMDATA_TRACE("attribute '" << attrName << "' token at " << current->start << " - " << current->end);
        current++;

        if (attrName == "vr")
        {
            if (vrToken == NULL)
                vrToken = current;
            else
                DCMDATA_WARN("attribute '" << attrName << " already present in this JSON object. This token will be ignored");
        }
        else
        {
            if (attrName == "bulkdatauri" || attrName == "inlinebinary" || attrName == "value")
            {
                if (valueToken == NULL)
                {
                    valueToken = current;
                    valueType = attrName;
                }
                else
                    DCMDATA_WARN("attribute '" << attrName << " already present in this JSON object. This token will be ignored");
            }
            else
            {
                DCMDATA_ERROR("unknown JSON attribute name \"" << attrName << "\"");
                return EC_InvalidJSONContent;
            }
        }
        // find the next token in the current hierarchie
        OFJsmnTokenPtr tmpToken = current;
        while (current->start < tmpToken->end)
            current++;
    }

    OFString vr = "";
    if (vrToken != NULL)
        getTokenContent(vr, vrToken);

    // create DICOM element
    result = createElement(newElem, dcmTag, vr);
    if (result.bad())
    {
        if (stopOnErrorPolicy_) return result; else result = EC_Normal;
    }

    if (valueToken == NULL)
    {
        // no content following, element value remains empty
        DCMDATA_TRACE("no value token for element " << dcmTag << ", using empty value");
    }

    // bulk data URIs reference a file, download URL or a URN referencing another MIME part in multipart/related structure.
    // This is not yet supported
    else if (valueType == "bulkdatauri")
    {
        if (ignoreBulkdataURIPolicy_)
        {
            // leave the element with BulkdataURI empty
            DCMDATA_INFO("ignoring BulkdataURI for element: " << dcmTag << ", leaving element empty");
        }
        else
        {
            // the URI value has to be a JSON string
            if (valueToken->type != JSMN_STRING)
            {
                DCMDATA_ERROR("not a valid DICOM JSON dataset: BulkdataURI value must be a JSON string");
                delete newElem;
                return EC_InvalidJSONType;
            }

            OFString value;
            getTokenContent(value, valueToken);

            if (isFileURI(value))
            {
                // convert URI to file path
                OFString filePath;
                OFString filePathNormalized;
                size_t offset = 0;
                size_t length = 0;
                result = fileURItoPath(value, filePath, offset, length);
                if (result.bad())
                {
                    delete newElem;
                    return result;
                }

                // normalize file path
                result = normalizePath(filePath, filePathNormalized);
                if (result.bad())
                {
                    delete newElem;
                    return result;
                }

                // check if file path is present in our list of permitted paths
                if (! bulkdataPathPermitted(filePathNormalized))
                {
                    DCMDATA_ERROR("BulkdataURI refers to a directory that is not permitted for bulk data: '" << filePath << "'");
                    delete newElem;
                    return EC_InvalidFilename;
                }

                // read the file content and insert it into the current element
                result = loadBulkdataFile(*newElem, filePathNormalized, offset, length);
                if (result.bad())
                {
                    delete newElem;
                    return result;
                }
            }
            else if (isHttpURI(value))
            {
                DCMDATA_ERROR("loading Bulkdata from http/https BulkDataURI not yet possible");
                delete newElem;
                return EC_UnsupportedURIType;
            }
            else
            {
                DCMDATA_ERROR("Unsupported BulkDataURI URI type: '" << value << "'");
                delete newElem;
                return EC_UnsupportedURIType;
            }
        }
    }

    // inlinebinary - content is base64 encoded
    else if (valueType == "inlinebinary")
    {
        // the base64 value has to be a JSON string
        if (valueToken->type != JSMN_STRING)
        {
            DCMDATA_ERROR("not a valid DICOM JSON dataset: InlineBinary value must be a JSON string");
            delete newElem;
            return EC_InvalidJSONType;
        }

        OFString value;
        getTokenContent(value, valueToken);
        Uint8* data = NULL;
        const size_t length = OFStandard::decodeBase64(value, data);
        DCMDATA_TRACE("parsing inline binary (" << length << "): " << value << " | " << data);
        if (length > 0)
            result = storeInlineBinaryValue(*newElem, data, length);

        /* delete buffer since data is copied into the element */
        delete[] data;
        if (result.bad())
        {
            if (stopOnErrorPolicy_) return result; else result = EC_Normal;
        }
    }
    else if (valueType == "value")
    {
        if (valueToken->type != JSMN_ARRAY)
        {
            // the value of the element has to be an array
            DCMDATA_ERROR("not a valid DICOM JSON dataset: attribute value must be a JSON array");
            delete newElem;
            return EC_InvalidJSONType;
        }

        // Sequence
        if (newElem->ident() == EVR_SQ)
        {
            result = parseSequence(*(OFstatic_cast(DcmSequenceOfItems*, newElem)), valueToken);
        }
        else if (newElem->getTag() == DCM_PixelData)
        {
            // special handling for pixel data
            DCMDATA_ERROR("pixel data must not have a 'value' attribute in the DICOM JSON model");
            delete newElem;
            return EC_InvalidJSONContent;
        }
        else
        {
            // parse the value array
            DCMDATA_TRACE("parsing value array of size " << valueToken->size);
            result = parseElementValueArray(newElem, valueToken);
        }
        if (result.bad())
        {
            if (stopOnErrorPolicy_) return result; else result = EC_Normal;
        }
    }
    else
    {
        DCMDATA_ERROR("unknown JSON attribute name: " << valueType);
        delete newElem;
        return EC_InvalidJSONContent;
    }

    if (result.good())
    {
        // insert the new attribute to the metaheader if the tag is (0002,xxxx), otherwise to the dataset
        if (dcmTag.getGroup() == 0x0002)
        {
            if (ignoreMetaInfoPolicy_ || (metaheader == NULL))
            {
                // we ignore meta info elements
                delete newElem;
            }
            else
            {
                if (dcmTag.getElement() == 0x0010)
                {
                    // this is (0002,0010) TransferSyntaxUID, extract the transfer syntax
                    OFString value;
                    newElem->getOFString(value, 0);
                    xferSyntax_ = DcmXfer(value.c_str()).getXfer();
                }
                result = metaheader->insert(newElem, OFFalse /*replaceOld*/);
            }
        }
        else
        {
            result = dataset->insert(newElem, OFFalse /*replaceOld*/);
        }
        if (result.bad())
        {
            DCMDATA_WARN("element " << dcmTag << " found twice in one data set or item, ignoring second entry");
            delete newElem;
        }
    }
    else // result.bad()
    {
        /* delete element if insertion or putting the value failed */
        delete newElem;
    }
    return result;
}


OFCondition DcmJSONReader::extractTag(
    OFJsmnTokenPtr keyToken,
    DcmTagKey& tagkey)
{
    OFString tagString;
    getTokenContent(tagString, keyToken);
    if (tagString.empty() || tagString.size() != 8)
    {
        DCMDATA_ERROR("not a valid DICOM JSON dataset: expected attribute tag string with 8 characters, found '" << tagString << "'");
        return EC_InvalidTag;
    }
    unsigned long group, element;
    OFString gStr = tagString.substr(0, 4);
    OFString eStr = tagString.substr(4);
    if (sscanf(gStr.c_str(), "%lx", &group) != 1
        || sscanf(eStr.c_str(), "%lx", &element) != 1)
    {
       DCMDATA_ERROR("not a valid DICOM JSON dataset: attribute tag must consist of two 16-bit hex numbers");
       return EC_InvalidTag;
    }

    tagkey.set(OFstatic_cast(Uint16, group), OFstatic_cast(Uint16, element));
    return EC_Normal;
}


OFCondition DcmJSONReader::parseSequence(
    DcmSequenceOfItems& sequence,
    OFJsmnTokenPtr& current)
{
    OFCondition result = EC_Normal;
    int sqSize = current->size;
    int sqStart = current->start;

    DCMDATA_TRACE("sequence start: " << sqStart << " with size: " << sqSize);
    current++;

    // iterate over sequence items
    for (int i = 0; i < sqSize; i++)
    {
        DCMDATA_TRACE("item " << sqStart << ":" << i << " -- " << current->start);

        // sequence items have to be an object
        if (current->type != JSMN_OBJECT)
            return EC_InvalidJSONType;

        // create new sequence item
        DcmItem* newItem = new DcmItem();
        if (newItem != NULL)
        {
            sequence.insert(newItem);

            // proceed parsing the item content
            result = parseDataSet(newItem, NULL, current);
            if (result.bad() && stopOnErrorPolicy_) return result;
        }
        DCMDATA_TRACE("item " << sqStart << ":" << i << " end, next up : " << current->start);
    }
    DCMDATA_TRACE("sequence end: " << sqStart << "; next element: " << current->start);
    return result;
}


OFCondition DcmJSONReader::parseDataSet(
    DcmItem* dataset,
    DcmItem* metaheader,
    OFJsmnTokenPtr& current)
{
    OFCondition result = EC_Normal;

    // we expext a JSON object that encapsulates the DICOM dataset
    if (current->type != JSMN_OBJECT)
    {
        DCMDATA_ERROR("not a valid DICOM JSON dataset: datasets must be encapsulated in a JSON object");
        return EC_InvalidJSONType;
    }
    int dsSize = current->size;
    int dsStart = current->start;

    DCMDATA_TRACE("dataset start " << dsStart << " - size: " << dsSize);
    current++;
    for (int i = 0; i < dsSize; i++)
    {
        // read each entry in the content object as a DICOM element
        result = parseElement(dataset, metaheader, current);
        if (result.bad() && stopOnErrorPolicy_) return result;
    }
    DCMDATA_TRACE("dataset end " << dsStart << "; next element: " << current->start);
    return result;
}




OFCondition DcmJSONReader::parsePersonName(
    OFString& value,
    OFJsmnTokenPtr& current)
{
    static const char *PersonGroupNames[] = { "Alphabetic", "Ideographic", "Phonetic" };

    OFCondition result;
    int size = current->size;
    if (size > 3)
    {
        DCMDATA_ERROR("not a valid DICOM JSON dataset: a person name must have at most three component groups");
        return EC_InvalidJSONType;
    }

    OFVector<OFString> pn(3);
    for (int i = 0; i < size; i++)
    {
        current++;
        if (current->type != JSMN_STRING)
        {
            DCMDATA_ERROR("not a valid DICOM JSON dataset: PN values must be JSON strings");
            return EC_InvalidJSONType;
        }

        OFString key;
        getTokenContent(key, current);
        int idx = -1;
        for (int j = 0; j < 3; j++)
        {
            if (key == PersonGroupNames[j])
            {
                idx = j;
                break;
            }
        }
        if (idx < 0)
        {

            DCMDATA_ERROR("not a valid DICOM JSON dataset: unsupported PN component group type '" << key << "'");
            return EC_InvalidJSONType;
        }

        // if pn[idx] is not empty, it will be overwritten.
        current++;
        if (current->type != JSMN_STRING)
        {
            DCMDATA_ERROR("not a valid DICOM JSON dataset: PN values must be JSON strings");
            return EC_InvalidJSONType;
        }

        getTokenContent(pn[idx], current);

        DCMDATA_TRACE("person name (PN) with " << key << " val " << pn[idx]);
        result = processJSONEscapeCharacters(pn[idx]);
        if (result.bad())
        {
            if (stopOnErrorPolicy_) return result; else result = EC_Normal;
        }
    }

    // PN format is: "alphPN=ideoPN=phonPN"
    if (!pn[0].empty())
        value += pn[0];
    if (!pn[1].empty() || !pn[2].empty())
        value += '=' + pn[1];
    if (!pn[2].empty())
        value += '=' + pn[2];
    DCMDATA_TRACE("PN value " << value);
    return result;
}


OFCondition DcmJSONReader::parseElementValueArray(
    DcmElement*& newElem,
    OFJsmnTokenPtr& current)
{
    OFCondition result;
    OFString vmString;
    OFString value;
    int vm = current->size;

    for (int count = 0; count < vm; count++)
    {
        value.clear();
        current++;
        if (newElem->ident() == EVR_PN)
        {
            // special handling for person names (PN)
            OFString tokenValue;
            getTokenContent(tokenValue, current);
            DCMDATA_TRACE("element value array, parsing PN value: " << tokenValue);
            if (current->type != JSMN_OBJECT)
            {
                if (tokenValue == "null")
                {
                    value = "";
                }
                else
                {
                    DCMDATA_ERROR("not a valid DICOM JSON dataset: PN components must be JSON strings or null");
                    return EC_InvalidJSONType;
                }
            }
            else
            {
                result = parsePersonName(value, current);
                if (result.bad())
                {
                    if (stopOnErrorPolicy_) return result; else result = EC_Normal;
                }
            }
            if (count > 0)
                vmString += '\\';
            vmString += value;
        }
        else if (newElem->ident() == EVR_AT)
        {
            // special handling for attribute tags (AT).
            // DcmAttributeTag::putOFStringArray() expects a format like this: "(0008,0020)\(0008,0030)"
            if (current->type != JSMN_STRING)
            {
                DCMDATA_ERROR("not a valid DICOM JSON dataset: AT values must be JSON strings");
                return EC_InvalidJSONType;
            }

            DcmTagKey tagkey;
            result = extractTag(current, tagkey);
            if (result.bad())
            {
                if (stopOnErrorPolicy_) return result; else result = EC_Normal;
            }
            DCMDATA_TRACE("element value array, parsing AT value: " << tagkey.toString());

            if (count > 0)
                vmString += '\\';
            vmString += tagkey.toString();
        }
        else
        {
            // default for all other VRs (numeric and text)
            if (current->type != JSMN_PRIMITIVE && current->type != JSMN_STRING)
            {
                DCMDATA_ERROR("not a valid DICOM JSON dataset: expected number, string or null");
                return EC_InvalidJSONType;
            }

            getTokenContent(value, current);
            if (current->type == JSMN_STRING)
            {
                // JSMN_STRING
                DCMDATA_TRACE("element value array, parsing string value: " << value);
                result = processJSONEscapeCharacters(value);
                if (result.bad())
                {
                    if (stopOnErrorPolicy_) return result; else result = EC_Normal;
                }
            }
            else
            {
                // JSMN_PRIMITIVE, i.e. null, number, or boolean (which should not occur)
                // Replace "null" by an empty string and keep numbers as they are
                if (value == "null") value = "";
            }
            if (count > 0)
                vmString += '\\';
            vmString += value;
            DCMDATA_TRACE("element value array: all values: " << vmString);
        }
    }

    result = newElem->putOFStringArray(vmString);
    if (result.bad())
    {
       DCMDATA_ERROR("failed to store string value for element " << newElem->getTag() << ": " << result.text());
    }
    return result;
}




OFCondition DcmJSONReader::createElement(
    DcmElement*& newElem,
    DcmTag& dcmTag,
    const OFString& vr)
{
    OFCondition result = EC_Normal;

    DCMDATA_TRACE("parsing VR: " << vr);

    // convert vr string
    const DcmVR jsonVR(vr.c_str());
    if (jsonVR.isUnknown() || jsonVR.isInvalid())
    {
        // check whether "vr" attribute exists
        if (vr.empty() || vr == "")
        {
            DCMDATA_WARN("missing 'vr' attribute for " << dcmTag
                << ", using the tag's VR (" << dcmTag.getVR().getVRName() << ")");
        }
        else {
            DCMDATA_WARN("invalid 'vr' attribute (" << vr << ") for " << dcmTag
                << ", using the tag's VR (" << dcmTag.getVR().getVRName() << ")");
        }
    }
    else
    {
        const DcmEVR dcmEVR = jsonVR.getEVR();
        const DcmEVR dictionaryEVR = dcmTag.getEVR();

        // Check if the VR is correct and print a warning otherwise.
        //
        // Normally, the JSON file and the data dictionary should specify the same
        // VR for a tag (i.e., dictionaryEVR == dcmEVR), or dictionaryEVR is EVR_UNKNOWN
        // if the tag is not present in our data dictionary.

        if ((dictionaryEVR != dcmEVR) && (dictionaryEVR != EVR_UNKNOWN) &&
            // LUTData VR can be US, SS or OW
            ((dcmTag.getTagKey() != DCM_LUTData) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS) && (dcmEVR != EVR_OW))) &&
            // If the dictionary says "EVR_xs", the VR can either be US or SS
            ((dictionaryEVR != EVR_xs) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS))) &&
            // If the dictionary says "EVR_ox" (OB/OW) or "EVR_px" (pixel data), the VR can either be OB or OW
            (((dictionaryEVR != EVR_ox) && (dictionaryEVR != EVR_px)) || ((dcmEVR != EVR_OB) && (dcmEVR != EVR_OW))))
        {
            // there are inconsistencies concerning the VR in the JSON file.
            // print a warning since there may be resulting errors in the DICOM dataset
            DCMDATA_WARN("element " << dcmTag << " has wrong VR (" << jsonVR.getVRName()
                << "), correct is '" << dcmTag.getVR().getVRName() << "'");
        }

        // change the VR to the one specified in the JSON dataset, even if it does not match our dictionary
        dcmTag.setVR(jsonVR);
    }

    // create DICOM element with given tag and VR
    result = DcmItem::newDicomElementWithVR(newElem, dcmTag);
    if (result.bad())
    {
        DCMDATA_ERROR("failed to create DICOM element "  << dcmTag << ": " << result.text());
    }

    // if this is pixel data, create an empty unencapsulated representation
    // to make sure that writing the element with empty value will not fail
    if (dcmTag == DCM_PixelData) newElem->putUint16Array(NULL,0);
    return result;
}


OFCondition DcmJSONReader::readAndConvertJSONFile(
    DcmFileFormat& fileformat,
    const char *ifname)
{
    // clear old buffers, in case this object is re-used
    clear();
    DcmMetaInfo* metaheader = fileformat.getMetaInfo();
    DcmDataset* dataset = fileformat.getDataset();
    OFCondition result;

    // readin the input file to a memory buffer
    OFString stdinName("-");
    if (ifname == stdinName)
        result = readJSONFromStdin();
        else result = readJSONFile(ifname);
    if (result.bad()) return result;

    // calculate tokens needed for the parser and allocate tokens
    result = reserveTokens();
    if (result.bad()) return result;

    // use the JSON library to parse the string and save it to the token array.
    result = parseJSON();
    if (result.bad() && stopOnErrorPolicy_) return result;

    // check if the token array starts with a JSON array or a JSON object
    OFJsmnTokenPtr current = tokenArray_;
    if (current->type == JSMN_ARRAY)
    {
        if (current->size < 1)
        {
            DCMDATA_ERROR("found empty JSON array instead of DICOM JSON dataset");
            return EC_InvalidJSONContent;
        }
        if (current->size == 1)
        {
            // this is a JSON array containing a single DICOM dataset.
            // Silently ignore the array structure and parse the dataset
            DCMDATA_DEBUG("parsing JSON array containing a single dataset");
            current++;
            result = parseDataSet(dataset, metaheader, current);
        }
        else
        {
            // this is a JSON array containing a multiple DICOM datasets.
            if (arrayHandlingPolicy_ < 0)
            {
                // reject multiple datasets
                DCMDATA_ERROR("found JSON array containing " << current->size << " DICOM datasets, rejecting conversion");
                result = EC_InvalidJSONContent;
            }
            else if (arrayHandlingPolicy_ == 0)
            {
                // Store multiple datasets in a private sequence.
                DCMDATA_DEBUG("parsing JSON array containing " << current->size << " DICOM datasets");
                DcmTag private_reservation(0x0009,0x0010, EVR_LO);
                DcmTag private_sequence(0x0009,0x1000, EVR_SQ);
                DcmSequenceOfItems *newSQ = new DcmSequenceOfItems(private_sequence);
                result = dataset->putAndInsertString(private_reservation, JSON2DCM_PRIVATE_RESERVATION);
                if (result.good()) result = dataset->insert(newSQ);
                if (result.good()) result = parseSequence(*newSQ, current);
            }
            else
            {
                // select a single dataset from the array
                if (arrayHandlingPolicy_ > current->size)
                {
                    DCMDATA_ERROR("found JSON array containing " << current->size << " DICOM datasets, cannot store dataset no. " << arrayHandlingPolicy_);
                    result = EC_InvalidJSONContent;
                }
                else
                {
                    DCMDATA_DEBUG("selecting dataset " << arrayHandlingPolicy_ << " from JSON array containing " << current->size << " DICOM datasets");

                    // move "current" to the token representing the first dataset
                    current++;

                    // number of datasets to skip
                    signed long tokensToSkip = arrayHandlingPolicy_ - 1;

                    // recursively skip tokens including their sub-tokens until we are done
                    while (tokensToSkip > 0)
                    {
                      tokensToSkip += current->size;
                      current++;
                      tokensToSkip--;
                    }

                    // extract the single dataset at the target location
                    result = parseDataSet(dataset, metaheader, current);
                }
            }
        }
    }
    else
    {
        // we expect a single dataset here, parseDataSet() will check if it is the right JSON structure
        DCMDATA_DEBUG("parsing single JSON dataset");
        result = parseDataSet(dataset, metaheader, current);
    }

    if (!stopOnErrorPolicy_) result = EC_Normal;
    return result;
}


OFBool DcmJSONReader::isFileURI(const OFString& uri) const
{
    return (uri.substr(0, 6) == "file:/");
}


OFBool DcmJSONReader::isHttpURI(const OFString& uri) const
{
    OFString s = uri.substr(0, 8);
    if (s == "https://") return OFTrue;
    s.erase(7);
    return (s == "http://");
}


OFCondition DcmJSONReader::urlDecode(OFString& uri) const
{
    size_t pos;
    unsigned int val = 0;
    char c;
    while (OFString_npos != (pos = uri.find("%")))
    {
       if (uri.length() + 3 < pos)
       {
            DCMDATA_ERROR("incomplete URL code: " << uri.substr(pos, 3));
            return EC_UnsupportedURIType;

       }
       c = uri[pos+1];
       if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
       {
            DCMDATA_ERROR("invalid URL code: " << uri.substr(pos, 3));
            return EC_UnsupportedURIType;
       }

       c = uri[pos+2];
       if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
       {
            DCMDATA_ERROR("invalid URL code: " << uri.substr(pos, 3));
            return EC_UnsupportedURIType;
       }

       if (1 != sscanf(uri.c_str() + pos, "%%%2x", &val))
       {
            DCMDATA_ERROR("invalid URL code: " << uri.substr(pos, 3));
            return EC_UnsupportedURIType;
       }
       uri[pos] = OFstatic_cast(char, val);
       uri.erase(pos+1, 2);
    }
    return EC_Normal;
}


OFCondition DcmJSONReader::fileURItoPath(const OFString& uri, OFString& filepath, size_t& offset, size_t& length) const
{
    // clear output parameters
    filepath.clear();
    offset = 0;
    length = 0;

    // check if this is a file URI at all
    if ((uri.substr(0, 6) != "file:/") || (uri.length() < 7))
    {
        DCMDATA_ERROR("not a file URI: " << uri);
        return EC_UnsupportedURIType;
    }

    OFString filePath;
    OFString params;

    // check if we have URI parameters. If they are present, store them in a separate string.
    // In any case, remove the "file:" prefix from the path.
    size_t paramStart = uri.find("?");
    if (paramStart == OFString_npos)
    {
        filePath = uri.substr(5);
    }
    else
    {
        params = uri.substr(paramStart+1);
        filePath = uri.substr(5, paramStart-5);
   }

    OFCondition result = urlDecode(params);
    if (result.bad()) return result;

    result = urlDecode(filePath);
    if (result.bad()) return result;

    // check if we have a URI of type "file:/path" or "file://host/path"
    if (filePath[1] == '/') // we have checked earlier that the URI is long enough for this
    {
        // URI type: file://host/path
        if (filePath.substr(1, 3) == "///")
        {
            DCMDATA_ERROR("Access to file URIs for network hosts not supported: " <<  uri);
            return EC_UnsupportedURIType;
        }

        // determine length of hostname
        size_t separator = filePath.find('/', 2);
        if (separator == OFString_npos)
        {
            DCMDATA_ERROR("file URI without path not supported: " <<  uri);
            return EC_UnsupportedURIType;
        }

        // separate hostname and path
        OFString host = filePath.substr(2, separator-2);
        filePath = filePath.substr(separator);
        if ((host != "") && (host != "localhost") && (host != "127.0.0.1") && (host != "ip6-localhost") && (host != "::1"))
        {
            DCMDATA_ERROR("Access to file URIs for network hosts not supported: " <<  uri);
            return EC_UnsupportedURIType;
        }
    }
    if (filePath.length() < 2)
    {
        DCMDATA_ERROR("file URI without path not supported: " <<  uri);
        return EC_UnsupportedURIType;
    }

#ifdef HAVE_WINDOWS_H
    if ((filePath.length() > 2) && (filePath[2] == ':'))
    {
        // first component of the path is a drive name. Remove leading slash.
        // We don't do this on Posix systems because the colon character is permitted there as a filename component.
        filePath.erase(0,1);
    }
#endif

    // replace '/' in the path by the system specific path separator
    size_t l = filePath.size();
    for (size_t i = 0; i < l; ++i)
    {
        if (filePath[i] == '/') filePath[i] = PATH_SEPARATOR;
    }

    // now for the URI parameters...
    size_t paramSep;
    OFString currentParam;
    while (params.length() > 0)
    {
        // separate the next parameter
        paramSep = params.find("&");
        if (paramSep == OFString_npos)
        {
            currentParam = params;
            params = "";
        }
        else
        {
            currentParam = params.substr(0, paramSep);
            params = params.substr(paramSep + 1);
        }
        if (currentParam.substr(0,7) == "offset=")
        {
            // convert offset
            unsigned long ll = 0;
            currentParam.erase(0,7);
            if ((OFString_npos != currentParam.find_first_not_of("0123456789")) || (1 != sscanf(currentParam.c_str(), "%lu", &ll)))
            {
                DCMDATA_ERROR("Invalid value for URI parameter 'offset': " <<  currentParam);
                return EC_UnsupportedURIType;
            }
            offset = OFstatic_cast(size_t, ll);
        }
        else if (currentParam.substr(0,7) == "length=")
        {
            // convert length
            unsigned long ll = 0;
            currentParam.erase(0,7);
            if ((OFString_npos != currentParam.find_first_not_of("0123456789")) || (1 != sscanf(currentParam.c_str(), "%lu", &ll)))
            {
                DCMDATA_ERROR("Invalid value for URI parameter 'length': " <<  currentParam);
                return EC_UnsupportedURIType;
            }
            length = OFstatic_cast(size_t, ll);
        }
        else
        {
          DCMDATA_ERROR("'file' URI with unsupported parameter '" << currentParam << "': " <<   uri);
          return EC_UnsupportedURIType;
        }
    }
    filepath = filePath;
    return EC_Normal;
}


OFCondition DcmJSONReader::normalizePath(const OFString& filepath_in, OFString& filepath_out) const
{
    filepath_out.clear();
#ifdef HAVE_WINDOWS_H
    char buf[32768];

    // resolve a relative path to an absolute path
    DWORD res = GetFullPathNameA(filepath_in.c_str(), 32768, buf, NULL);
    if ((res == 0) || (res > 32768))
    {
        DCMDATA_ERROR("Failed to normalize file path: '" << filepath_in << "'");
        return EC_InvalidFilename;
    }

    // resolve short file and directory name components such as "PROGRA~1"
    // (which is the short 8.3 version for "Program Files") into the long names
    res = GetLongPathNameA(buf, buf, 32768);
    if ((res == 0) || (res > 32768))
    {
        DCMDATA_ERROR("Failed to normalize file path: '" << filepath_in << "'");
        return EC_InvalidFilename;
    }

    // convert all characters to uppercase using a function that (hopefully)
    // uses the same mapping table as the WIN32 file API
    size_t len = strlen(buf);
    if (len != CharUpperBuffA(buf, OFstatic_cast(DWORD, len)))
    {
        DCMDATA_ERROR("Failed to normalize file path: '" << filepath_in << "'");
        return EC_InvalidFilename;
    }
    filepath_out = buf;
#else
    // resolve a relative path to an absolute path without symbolic links
    char *resolved_path = realpath(filepath_in.c_str(), NULL);
    if (resolved_path == NULL)
    {
        DCMDATA_ERROR("Failed to normalize file path: '" << filepath_in << "'");
        return EC_InvalidFilename;
    }
    filepath_out = resolved_path;
    free(resolved_path);
#endif
    return EC_Normal;
}


OFCondition DcmJSONReader::addPermittedBulkdataPath(const OFString& dirpath)
{
    OFString std_dirpath;
    OFCondition result = normalizePath(dirpath, std_dirpath);
    if ((std_dirpath.length() > 0) && (std_dirpath[std_dirpath.length()-1] != PATH_SEPARATOR))
    {
        std_dirpath.append(1, PATH_SEPARATOR);
    }

    if (result.good()) permittedBulkdataDirs_.push_back(std_dirpath);
    return result;

}


OFBool DcmJSONReader::bulkdataPathPermitted(const OFString& filepath) const
{
    OFListConstIterator(OFString) iter = permittedBulkdataDirs_.begin();
    OFListConstIterator(OFString) last = permittedBulkdataDirs_.end();
    while (iter != last)
    {
        if (filepath.substr(0, (*iter).length()) == *iter) return OFTrue;
        ++iter;
    }
    return OFFalse;
}


OFCondition DcmJSONReader::loadBulkdataFile(
    DcmElement& element,
    const OFString& filepath,
    size_t offset,
    size_t length)
{
    // open file for reading
    OFFile file;
    if (! file.fopen(filepath, "rb"))
    {
        OFString s("(unknown error code)");
        file.getLastErrorString(s);
        return makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
    }

    // obtain file size and check if file is large enough
    const size_t filelen = OFStandard::getFileSize(filepath);
    if (0 == length) length = filelen;
    if (offset + length > filelen)
    {
        DCMDATA_ERROR("bulk data file too short: '" << filepath << "', expected " << offset + length << " bytes but only found " << filelen);
        return EC_EndOfStream;
    }

    // allocate buffer
    Uint8 *bulkDataBuffer = new (std::nothrow) Uint8[length];
    if (bulkDataBuffer == NULL)
    {
        DCMDATA_ERROR("out of memory: failed to allocate buffer for bulk data file");
        return EC_MemoryExhausted;
    }

    // seek to the given offset within the file
    if (offset > 0)
    {
        if (0 != file.fseek(offset, SEEK_SET))
        {
            OFString s("(unknown error code)");
            file.getLastErrorString(s);
            delete[] bulkDataBuffer;
            return makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
        }
    }

    // read the bulk data into the buffer
    size_t res = file.fread(bulkDataBuffer, 1, length);

    // we ignore the fclose() return code, which is safe since the file is read-only
    file.fclose();

    // check the number of bytes read
    if (res != length)
    {
        OFString s("(unknown error code)");
        file.getLastErrorString(s);
        delete[] bulkDataBuffer;
        return makeOFCondition(OFM_dcmdata, 18, OF_error, s.c_str());
    }

    // file was successfully read into buffer. Store the result.
    OFCondition result = storeBulkValue(element, bulkDataBuffer, length);
    delete[] bulkDataBuffer;
    return result;
}

/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Author:  Andreas Barth
 *
 *  Purpose: create a Dicom FileFormat or DataSet from an ASCII-dump
 *
 */

/*
 * Input File Description:
 * The input file can be an output of dcmdump.  One element (tag, VR, value)
 * must be written into one line separated by arbitrary spaces or tab characters.
 * A '#' begins a comment that ends at the line end.  Empty lines are allowed.
 * The individual parts of a line have the following syntax:
 * Tag:   (gggg,eeee)
 *        with gggg and eeee are 4 character hexadecimal values representing
 *        group and element tag.  Spaces and tabs can be anywhere in a tag
 *        specification.
 * VR:    Value Representation must be written as 2 characters as in Part 6
 *        of the DICOM standard.  No spaces or tabs are allowed between the
 *        two characters.  If the VR can determined from the tag, this part
 *        of a line is optional.
 * Value: There are several rules for writing values:
 *        1. US, SS, SL, UL, FD, FL are written as decimal strings that can
 *           be read by scanf().
 *        2. AT is written as '(gggg,eeee)' with additional spaces stripped
 *           off automatically and gggg and eeee being decimal strings that
 *           can be read by scanf().
 *        3. OB and OW values are written as byte or word hexadecimal values
 *           separated by '\' character.  Alternatively, OB or OW values can
 *           be read from a separate file by writing the filename prefixed
 *           by a '=' character (e.g. '=largepix.dat').  The contents of the
 *           file will be read as is.  By default, OW data is expected to be
 *           little endian ordered and will be swapped if necessary.  No
 *           checks will be made to ensure that the amount of data is
 *           reasonable in terms of other attributes such as Rows or Columns.
 *           In case of compressed pixel data, the line should start with
 *           '(7fe0,0010) OB (PixelSequence' in order to distinguish from
 *           uncompressed pixel data.
 *        4. UI is written as '=Name' in data dictionary or as unique
 *           identifier string (see 6.) , e.g. '[1.2.840.....]'
 *        5. Strings without () <> [] spaces, tabs and # can be written
 *           directly.
 *        6. Other strings must be surrounded by '[' and ']'.  No bracket
 *           structure is passed.  The value ends at the last ']' in the
 *           line.  Anything after the ']' is interpreted as comment.
 *        7. '(' and '<' are interpreted special and may not be used when
 *           writing an input file by hand as beginning characters of a
 *           string.  Multiple Value are separated by '\'.  The sequence of
 *           lines must not be ordered but they can.  References in DICOM
 *           Directories are not supported.  Semantic errors are not detected.
 *
 * Examples:
 *  (0008,0020) DA  [19921012]          #     8,  1  StudyDate
 *  (0008,0016) UI  =MRImageStorage     #    26,  1  SOPClassUID
 *  (0002,0012) UI  [1.2.276.0.7230010.100.1.1]
 *  (0020,0032) DS  [0.0\0.0]           #     8,  2  ImagePositionPatient
 *  (0028,0009) AT  (3004,000c)         #     4,  1  FrameIncrementPointer
 *  (0028,0010) US  256                 #     4,  1  Rows
 *  (0002,0001) OB  01\00
 *
 */

#include "dcmtk/config/osconfig.h"

// if defined, use createValueFromTempFile() for large binary data files
//#define EXPERIMENTAL_READ_FROM_FILE

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"  /* for dcmZlibCompressionLevel */

#ifdef EXPERIMENTAL_READ_FROM_FILE
#include "dcmtk/dcmdata/dcistrmf.h"  /* for class DcmInputFileStream */
#endif

#ifdef WITH_ZLIB
#include <zlib.h>                    /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dump2dcm"

static OFLogger dump2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define SHORTCOL 3
#define LONGCOL 21

// Maximum Line Size (default)

const unsigned int DCM_DumpMaxLineSize = 4096;

// Global variables

static E_ByteOrder opt_fileContentsByteOrdering = EBO_LittleEndian;

// Field definition separators

#define DCM_DumpCommentChar '#'
#define DCM_DumpTagDelim ')'
#define DCM_DumpOpenString '['
#define DCM_DumpCloseString ']'
#define DCM_DumpOpenDescription '('
#define DCM_DumpOpenFile '<'
#define DCM_DumpCloseFile '>'

#define TO_UCHAR(s) OFstatic_cast(unsigned char, (s))
static void
stripWhitespace(char *s)
{
    char *p;

    if (s == NULL) return;

    p = s;
    while (*s != '\0') {
        if (isspace(TO_UCHAR(*s)) == OFFalse) {
            *p++ = *s;
        }
        s++;
    }
    *p = '\0';
}

static char *
stripTrailingWhitespace(char *s)
{
    int i, n;
    if (s == NULL) return s;

    n = strlen(s);
    for (i = n - 1; i >= 0 && isspace(TO_UCHAR(s[i])); i--)
        s[i] = '\0';
    return s;
}


static char *
stripPrecedingWhitespace(char *s)
{
    char *p;
    if (s == NULL) return s;

    for(p = s; *p && isspace(TO_UCHAR(*p)); p++)
        ;

    return p;
}

static OFBool
onlyWhitespace(const char *s)
{
    int len = strlen(s);
    int charsFound = OFFalse;

    for (int i = 0; (!charsFound) && (i < len); i++) {
        charsFound = !isspace(TO_UCHAR(s[i]));
    }
    return (!charsFound) ? OFTrue : OFFalse;
}

static char *
getLine(char *line, int maxLineLen, FILE *f, const unsigned long lineNumber)
{
    char *s;

    s = fgets(line, maxLineLen, f);

    // if line is too long, throw rest of it away
    if (s && strlen(s) == size_t(maxLineLen - 1) && s[maxLineLen - 2] != '\n')
    {
        int c = fgetc(f);
        while(c != '\n' && c != EOF)
            c = fgetc(f);
        OFLOG_ERROR(dump2dcmLogger, "line " << lineNumber << " too long");
    }

    /* strip any trailing white space */
    stripTrailingWhitespace(s);

    return s;
}

static OFBool
isaCommentLine(const char *s)
{
    OFBool isComment = OFFalse; /* assumption */
    int len = strlen(s);
    int i = 0;
    // skip leading spaces
    for (i = 0; i < len && isspace(TO_UCHAR(s[i])); i++) /* loop with empty body */;
    // check for comment character
    isComment = (s[i] == DCM_DumpCommentChar);
    return isComment;
}

static OFBool
parseTag(char *&s, DcmTagKey &key)
{
    OFBool ok = OFTrue;
    char *p;
    unsigned int g, e;

    // find tag begin
    p = strchr(s, DCM_DumpTagDelim);
    if (p)
    {
        // string all white spaces and read tag
        int len = p - s + 1;
        p = new char[len + 1];
        OFStandard::strlcpy(p, s, len + 1);
        stripWhitespace(p);
        s += len;

        if (sscanf(p, "(%x,%x)", &g, &e) == 2)
            key.set(g, e);
        else
            ok = OFFalse;
        delete[] p;
    }
        else ok = OFFalse;

    return ok;
}


static OFBool
parseVR(char *&s, DcmEVR &vr)
{
    OFBool ok = OFTrue;

    s = stripPrecedingWhitespace(s);

    // Are there two upper characters?
    if (isupper(TO_UCHAR(*s)) && isupper(TO_UCHAR(*(s + 1))))
    {
        char c_vr[3];
        OFStandard::strlcpy(c_vr, s, 3);
        // Convert to VR
        DcmVR dcmVR(c_vr);
        vr = dcmVR.getEVR();
        s += 2;
    }
    else if ((*s == 'p') && (*(s + 1) == 'i'))
    {
        vr = EVR_pixelItem;
        s += 2;
    }
    else if (((*s == 'o') && (*(s + 1) == 'x')) ||
        ((*s == 'x') && (*(s + 1) == 's')) ||
        ((*s == 'n') && (*(s + 1) == 'a')) ||
        ((*s == 'u') && (*(s + 1) == 'p')))
    {
        // swallow internal VRs
        vr = EVR_UNKNOWN;
        s += 2;
    }
    else ok = OFFalse;

    return ok;
}
#undef TO_UCHAR


static int
searchLastClose(char *s, const char closeChar)
{
    // search last close bracket in a line
    // no bracket structure will be considered

    char *found = NULL;
    char *p = s;

    while(p && *p)
    {
        p = strchr(p, closeChar);
        if (p)
        {
            found = p;
            p++;
        }
    }

    if (found)
        return (found - s) + 1;
    else
        return 0;
}


static int
searchCommentOrEol(char *s)
{
    char *comment = strchr(s, DCM_DumpCommentChar);
    if (comment)
        return comment - s;
    else if (s)
        return strlen(s);
    else
        return 0;
}


static char *
convertNewlineCharacters(char *s)
{
    // convert the string "\n" into the \r\n combination required by DICOM
    if (s == NULL || s[0] == '\0') return s;
    int len = strlen(s);
    int i = 0;
    for (i = 0; i < (len - 1); i++)
    {
        if (s[i] == '\\' && s[i + 1] == 'n')
        {
            s[i] = '\r';
            s[i + 1] = '\n';
            i++;
        }
    }

    return s;
}

static OFBool
parseValue(char *&s, char *&value, DcmEVR &vr, const DcmTagKey &tagkey)
{
    OFBool ok = OFTrue;
    int len;
    value = NULL;

    s = stripPrecedingWhitespace(s);

    switch (*s)
    {
        case DCM_DumpOpenString:
            len = searchLastClose(s, DCM_DumpCloseString);
            if (len == 0)
                ok = OFFalse;
            else if (len > 2)
            {
                value = new char[len - 1];
                OFStandard::strlcpy(value, s + 1, len - 1);
                value = convertNewlineCharacters(value);
            }
            else
                value = NULL;
            break;

        case DCM_DumpOpenDescription:
            /* need to distinguish VR=AT from description field */
            /* NB: if the VR is unknown this workaround will not succeed */
            if (vr == EVR_AT)
            {
                len = searchLastClose(s, DCM_DumpTagDelim);
                if (len >= 11)  // (gggg,eeee) allow non-significant spaces
                {
                    char *pv = s;
                    DcmTagKey tag;
                    if (parseTag(pv, tag))   // check for valid tag format
                    {
                        value = new char[len + 1];
                        OFStandard::strlcpy(value, s, len + 1);
                        stripWhitespace(value);
                    } else
                        ok = OFFalse;   // skip description
                }
                else
                    ok = OFFalse;   // skip description
            }
            /* need to distinguish pixel sequence */
            else if ((tagkey == DCM_PixelData) && (vr == EVR_OB))
            {
                if (strncmp(s + 1, "PixelSequence", 13) == 0)
                    vr = EVR_pixelSQ;
            }
            break;

        case DCM_DumpOpenFile:
            ok = OFFalse;  // currently not supported
            break;

        case DCM_DumpCommentChar:
            break;

        default:
            len = searchCommentOrEol(s);
            if (len)
            {
                value = new char[len + 1];
                OFStandard::strlcpy(value, s, len + 1);
                stripTrailingWhitespace(value);
            }
            break;
    }
    return ok;
}

static OFCondition
putFileContentsIntoElement(DcmElement *elem, const char *filename)
{
    OFCondition ec = EC_Normal;
#ifdef EXPERIMENTAL_READ_FROM_FILE
    /* create stream object for binary file */
    DcmInputFileStream fileStream(filename);
    ec = fileStream.status();
    if (ec.good())
    {
        /* NB: if size is odd file will be rejected */
        const size_t fileLen = OFStandard::getFileSize(filename);
        /* read element value from binary file (requires even length) */
        ec = elem->createValueFromTempFile(fileStream.newFactory(), fileLen, EBO_LittleEndian);
        if (ec.bad())
            OFLOG_ERROR(dump2dcmLogger, "cannot process binary data file: " << filename);
    } else {
        OFLOG_ERROR(dump2dcmLogger, "cannot open binary data file: " << filename);
        ec = EC_InvalidTag;
    }
#else
    FILE *f = NULL;
    if ((f = fopen(filename, "rb")) == NULL)
    {
        OFLOG_ERROR(dump2dcmLogger, "cannot open binary data file: " << filename);
        return EC_InvalidTag;
    }

    const size_t len = OFStandard::getFileSize(filename);
    unsigned long buflen = len;
    if (buflen & 1)
        buflen++; /* if odd then make even (DICOM requires even length values) */

    Uint8 *buf = NULL;
    const DcmEVR evr = elem->getVR();
    /* create buffer of OB or OW data */
    if (evr == EVR_OB || evr == EVR_pixelItem)
        ec = elem->createUint8Array(buflen, buf);
    else if (evr == EVR_OW)
    {
        Uint16 *buf16 = NULL;
        ec = elem->createUint16Array(buflen / 2, buf16);
        buf = OFreinterpret_cast(Uint8 *, buf16);
    } else
        ec = EC_IllegalCall;
    if (ec.good())
    {
        /* read binary file into the buffer */
        if (fread(buf, 1, OFstatic_cast(size_t, len), f) != len)
        {
            char errBuf[256];
            OFLOG_ERROR(dump2dcmLogger, "error reading binary data file: " << filename
                << ": " << OFStandard::strerror(errno, errBuf, sizeof(errBuf)));
            ec = EC_CorruptedData;
        }
        else if (evr == EVR_OW)
        {
            /* swap 16 bit OW data (if necessary) */
            swapIfNecessary(gLocalByteOrder, opt_fileContentsByteOrdering, buf, buflen, sizeof(Uint16));
        }
    }
    else if (ec == EC_MemoryExhausted)
        OFLOG_ERROR(dump2dcmLogger, "out of memory reading binary data file: " << filename);
    else
        OFLOG_ERROR(dump2dcmLogger, "illegal call processing binary data file: " << filename);

    fclose(f);
#endif
    return ec;
}

static OFCondition
insertIntoSet(DcmStack &stack, const E_TransferSyntax xfer, const DcmTagKey &tagkey,
              const DcmEVR &vr, const char *value)
{
    // insert new element into dataset or metaheader

    OFCondition l_error = EC_Normal;
    OFCondition newElementError = EC_Normal;

    if (stack.empty())
        l_error = EC_CorruptedData;

    if (l_error == EC_Normal)
    {
        DcmElement *newElement = NULL;
        DcmObject *topOfStack = stack.top();

        // convert tagkey to tag including VR
        DcmTag tag(tagkey);
        DcmVR dcmvr(vr);

        const DcmEVR tagvr = tag.getEVR();
        /* check VR and consider various special cases */
        if (tagvr != vr && vr != EVR_UNKNOWN && tagvr != EVR_UNKNOWN &&
           (tagkey != DCM_LUTData || (vr != EVR_US && vr != EVR_SS && vr != EVR_OW)) &&
           (tagkey != DCM_PixelData || (vr != EVR_OB && vr != EVR_OW && vr != EVR_pixelSQ)) &&
           (tagvr != EVR_xs || (vr != EVR_US && vr != EVR_SS)) &&
           (tagvr != EVR_ox || (vr != EVR_OB && vr != EVR_OW)) &&
           (tagvr != EVR_na || vr != EVR_pixelItem))
        {
            OFLOG_WARN(dump2dcmLogger, "Tag " << tag << " with wrong VR '"
                << dcmvr.getVRName() << "' found, correct is '"
                << tag.getVR().getVRName() << "'");
        }
        else if ((tagvr == EVR_UNKNOWN) && (vr == EVR_UNKNOWN))
        {
            OFLOG_WARN(dump2dcmLogger, "VR of Tag " << tag << " is unknown, "
                << "using 'UN' for this data element");
        }

        if (vr != EVR_UNKNOWN)
            tag.setVR(dcmvr);
        const DcmEVR newTagVR = tag.getEVR();

        // create new element (special handling for pixel sequence and item)
        if (newTagVR == EVR_pixelSQ)
            newElement = new DcmPixelData(tag);
        else if (newTagVR == EVR_pixelItem)
            newElement = new DcmPixelItem(DcmTag(DCM_Item, EVR_OB));
        else
            newElementError = newDicomElement(newElement, tag);

        if (newElementError == EC_Normal)
        {
            // tag describes an element
            if (!newElement)
            {
                // Tag was ambiguous - should never happen according to the current implementation of newDicomElement()
                l_error = EC_InvalidVR;
            }
            else
            {
                // check for uncompressed pixel data (i.e. no pixel sequence present)
                if (tagkey == DCM_PixelData  && (newTagVR == EVR_OB || newTagVR == EVR_OW))
                    OFstatic_cast(DcmPixelData *, newElement)->setNonEncapsulationFlag(OFTrue /*alwaysUnencapsulated*/);
                // fill value
                if (value)
                {
                    if (value[0] == '=' && (newTagVR == EVR_OB || newTagVR == EVR_OW || newTagVR == EVR_pixelItem))
                    {
                        /*
                         * Special case handling for OB, OW and pixel item data.
                         * Allow a value beginning with a '=' character to represent
                         * a file containing data to be used as the attribute value.
                         * A '=' character is not a normal value since OB and OW values
                         * must be written as multivalued hexadecimal (e.g. "00\ff\0d\8f");
                         */
                        l_error = putFileContentsIntoElement(newElement, value + 1);
                    } else {
                        l_error = newElement->putString(value);
                    }
                }

                // insert element into hierarchy
                if (l_error == EC_Normal)
                {
                    switch(topOfStack->ident())
                    {
                      case EVR_item:
                      case EVR_dirRecord:
                      case EVR_dataset:
                      case EVR_metainfo:
                          {
                              DcmItem *item = OFstatic_cast(DcmItem *, topOfStack);
                              item->insert(newElement);
                              // special handling for pixel sequence
                              if (newTagVR == EVR_pixelSQ)
                              {
                                  DcmPixelSequence *pixelSeq = new DcmPixelSequence(DcmTag(DCM_PixelData, EVR_OB));
                                  if (pixelSeq != NULL)
                                  {
                                      OFstatic_cast(DcmPixelData *, newElement)->putOriginalRepresentation(xfer, NULL, pixelSeq);
                                      stack.push(pixelSeq);
                                  }
                              }
                              else if (newElement->ident() == EVR_SQ)
                                  stack.push(newElement);
                          }
                          break;
                      case EVR_pixelSQ:
                          if (newTagVR == EVR_pixelItem)
                          {
                              DcmPixelSequence *pixelSeq = OFstatic_cast(DcmPixelSequence *, topOfStack);
                              pixelSeq->insert(OFstatic_cast(DcmPixelItem *, newElement));
                          } else
                              l_error = EC_InvalidTag;
                          break;
                      default:
                          l_error = EC_InvalidTag;
                      break;
                    }
                }
            }
        }
        else if (newElementError == EC_SequEnd)
        {
            // pop stack if stack object was a sequence
            if (topOfStack->ident() == EVR_SQ || topOfStack->ident() == EVR_pixelSQ)
                stack.pop();
            else
                l_error = EC_InvalidTag;
        }
        else if (newElementError == EC_ItemEnd)
        {
            // pop stack if stack object was an item
            switch (topOfStack->ident())
            {
              case EVR_item:
              case EVR_dirRecord:
              case EVR_dataset:
              case EVR_metainfo:
                  stack.pop();
                  break;

              default:
                  l_error = EC_InvalidTag;
                  break;
            }
        }
        else if (newElementError == EC_InvalidTag)
        {
            if (tag.getXTag() == DCM_Item)
            {
                DcmItem *item = NULL;
                if (topOfStack->getTag().getXTag() == DCM_DirectoryRecordSequence)
                {
                    // an Item must be pushed to the stack
                    item = new DcmDirectoryRecord(tag, 0);
                    OFstatic_cast(DcmSequenceOfItems *, topOfStack)->insert(item);
                    stack.push(item);
                }
                else if (topOfStack->ident() == EVR_SQ)
                {
                    // an item must be pushed to the stack
                    item = new DcmItem(tag);
                    OFstatic_cast(DcmSequenceOfItems *, topOfStack)->insert(item);
                    stack.push(item);
                }
                else
                    l_error = EC_InvalidTag;
            }
            else
                l_error = EC_InvalidTag;
        }
        else
            l_error = EC_InvalidTag;
    }

    return l_error;
}

static OFBool
readDumpFile(DcmMetaInfo *metaheader, DcmDataset *dataset,
             FILE *infile, const char *ifname, E_TransferSyntax &xfer,
             const OFBool stopOnErrors, const unsigned long maxLineLength)
{
    char *lineBuf = new char[maxLineLength];
    int lineNumber = 0;
    OFBool errorOnThisLine = OFFalse;
    char *parse = NULL;
    char *value = NULL;
    DcmEVR vr = EVR_UNKNOWN;
    int errorsEncountered = 0;
    DcmTagKey tagkey;
    DcmStack metaheaderStack;
    DcmStack datasetStack;
    xfer = EXS_Unknown;

    if (metaheader)
        metaheaderStack.push(metaheader);

    datasetStack.push(dataset);

    while (getLine(lineBuf, OFstatic_cast(int, maxLineLength), infile, lineNumber + 1))
    {
        lineNumber++;

        OFLOG_TRACE(dump2dcmLogger, "processing line " << lineNumber << " of the input file");

        // ignore empty lines and comment lines
        if (onlyWhitespace(lineBuf))
            continue;
        if (isaCommentLine(lineBuf))
            continue;

        errorOnThisLine = OFFalse;

        parse = &lineBuf[0];

        // parse tag an the line
        if (!parseTag(parse, tagkey))
        {
            OFLOG_ERROR(dump2dcmLogger, OFFIS_CONSOLE_APPLICATION ": "<< ifname << ": "
                << "no Tag found (line " << lineNumber << ")");
            errorOnThisLine = OFTrue;
        }

        // parse optional VR
        if (!errorOnThisLine && !parseVR(parse, vr))
            vr = EVR_UNKNOWN;

        // parse optional value
        if (!errorOnThisLine && !parseValue(parse, value, vr, tagkey))
        {
            OFLOG_ERROR(dump2dcmLogger, OFFIS_CONSOLE_APPLICATION ": "<< ifname << ": "
                << "incorrect value specification (line " << lineNumber << ")");
            errorOnThisLine = OFTrue;
        }


        // insert new element that consists of tag, VR, and value
        if (!errorOnThisLine)
        {
            OFCondition l_error = EC_Normal;
            if (tagkey.getGroup() == 0x0002)
            {
                if (metaheader)
                {
                    l_error = insertIntoSet(metaheaderStack, xfer, tagkey, vr, value);
                    // check for transfer syntax in meta-header
                    if ((tagkey == DCM_TransferSyntaxUID) && (xfer == EXS_Unknown))
                    {
                        const char *xferUID;
                        // use resolved value (UID)
                        if (metaheader->findAndGetString(DCM_TransferSyntaxUID, xferUID).good())
                            xfer = DcmXfer(xferUID).getXfer();
                    }
                }
            }
            else
                l_error = insertIntoSet(datasetStack, xfer, tagkey, vr, value);

            if (value)
            {
                delete[] value;
                value = NULL;
            }
            if (l_error != EC_Normal)
            {
                errorOnThisLine = OFTrue;
                OFLOG_ERROR(dump2dcmLogger, OFFIS_CONSOLE_APPLICATION ": " << ifname << ": Error in creating Element: "
                    << l_error.text() << " (line " << lineNumber << ")");
            }

        }

        if (errorOnThisLine)
            errorsEncountered++;
    }


    // test blocking structure
    if (metaheader && metaheaderStack.card() != 1)
    {
        OFLOG_ERROR(dump2dcmLogger, OFFIS_CONSOLE_APPLICATION ": " << ifname << ": Block Error in metaheader");
        errorsEncountered++;
    }

    if (datasetStack.card() != 1)
    {
        OFLOG_ERROR(dump2dcmLogger, OFFIS_CONSOLE_APPLICATION ": " << ifname << ": Block Error in dataset");
        errorsEncountered++;
    }

    delete[] lineBuf;

    if (errorsEncountered)
    {
        OFLOG_ERROR(dump2dcmLogger, errorsEncountered << " Errors found in " <<  ifname);
        return !stopOnErrors;
    }
    else
        return OFTrue;
}

// ********************************************

int main(int argc, char *argv[])
{
#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Convert ASCII dump to DICOM file", rcsid);
    OFCommandLine cmd;

    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dumpfile-in", "dump input filename");
    cmd.addParam("dcmfile-out", "DICOM output filename");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                   "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    cmd.addGroup("input options:", LONGCOL, SHORTCOL + 2);
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-meta-info",      "+f",     "read meta information if present (default)");
        cmd.addOption("--ignore-meta-info",    "-f",     "ignore file meta information");
      cmd.addSubGroup("file contents byte ordering:");
        cmd.addOption("--read-file-little",    "+rl",    "read OW data with little endian (default)");
        cmd.addOption("--read-file-big",       "+rb",    "read OW data from file with big endian");
      cmd.addSubGroup("other input options:");
        cmd.addOption("--line",                "+l",  1, "[m]ax-length: integer",
                                                         "maximum line length m (default: 4096)");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("unique identifiers:");
        cmd.addOption("--generate-new-uids",   "+Ug",    "generate new Study/Series/SOP Instance UID");
        cmd.addOption("--dont-overwrite-uids", "-Uo",    "do not overwrite existing UIDs (default)");
        cmd.addOption("--overwrite-uids",      "+Uo",    "overwrite existing UIDs");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output file format:");
        cmd.addOption("--write-file",          "+F",     "write file format (default)");
        cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
        cmd.addOption("--update-meta-info",    "+Fu",    "update particular file meta information");
      cmd.addSubGroup("output transfer syntax:");
        cmd.addOption("--write-xfer-same",     "+t=",    "write with same TS as input (default)");
        cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian");
        cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
        cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
        cmd.addOption("--write-xfer-deflated", "+td",    "write with deflated expl. VR little endian TS");
#endif
      cmd.addSubGroup("error handling:");
        cmd.addOption("--stop-on-error",       "-E",     "do not write if dump is damaged (default)");
        cmd.addOption("--ignore-errors",       "+E",     "attempt to write even if dump is damaged");
      cmd.addSubGroup("post-1993 value representations:");
        cmd.addOption("--enable-new-vr",       "+u",     "enable support for new VRs (UN/UT) (default)");
        cmd.addOption("--disable-new-vr",      "-u",     "disable support for new VRs, convert to OB");
      cmd.addSubGroup("group length encoding:");
        cmd.addOption("--group-length-recalc", "+g=",    "recalculate group lengths if present (default)");
        cmd.addOption("--group-length-create", "+g",     "always write with group length elements");
        cmd.addOption("--group-length-remove", "-g",     "always write without group length elements");
      cmd.addSubGroup("length encoding in sequences and items:");
        cmd.addOption("--length-explicit",     "+e",     "write with explicit lengths (default)");
        cmd.addOption("--length-undefined",    "-e",     "write with undefined lengths");
      cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
        cmd.addOption("--padding-retain",      "-p=",    "do not change padding (default)");
        cmd.addOption("--padding-off",         "-p",     "no padding (implicit if --write-dataset)");
        cmd.addOption("--padding-create",      "+p",  2, "[f]ile-pad [i]tem-pad: integer",
                                                         "align file on multiple of f bytes\nand items on multiple of i bytes");
#ifdef WITH_ZLIB
      cmd.addSubGroup("deflate compression level (only with --write-xfer-deflated):");
        cmd.addOption("--compression-level",   "+cl", 1, "[l]evel: integer (default: 6)",
                                                         "0=uncompressed, 1=fastest, 9=best compression");
#endif

    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
    E_TransferSyntax opt_xfer = EXS_Unknown;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_recalcGL;
    E_PaddingEncoding opt_padenc = EPD_withoutPadding;
    E_FileWriteMode opt_writeMode = EWM_fileformat;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;
    OFCmdUnsignedInt opt_linelength = DCM_DumpMaxLineSize;
    OFBool opt_stopOnErrors = OFTrue;
    OFBool opt_metaInfo = OFTrue;
    OFBool opt_generateUIDs = OFFalse;
    OFBool opt_overwriteUIDs = OFFalse;

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
          if (cmd.findOption("--version"))
          {
              app.printHeader(OFTrue /*print host identifier*/);
              COUT << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
              COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
              COUT << " none" << OFendl;
#endif
              return 0;
          }
      }

      /* command line parameters */

      cmd.getParam(1, opt_ifname);
      cmd.getParam(2, opt_ofname);

      OFLog::configureFromCommandLine(cmd, app);

      /* input options */

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-meta-info")) opt_metaInfo = OFTrue;
      if (cmd.findOption("--ignore-meta-info")) opt_metaInfo = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file-little")) opt_fileContentsByteOrdering = EBO_LittleEndian;
      if (cmd.findOption("--read-file-big")) opt_fileContentsByteOrdering = EBO_BigEndian;
      cmd.endOptionBlock();

      if (cmd.findOption("--line"))
          app.checkValue(cmd.getValueAndCheckMin(opt_linelength, 80));

      /* processing options */

      if (cmd.findOption("--generate-new-uids")) opt_generateUIDs = OFTrue;

      cmd.beginOptionBlock();
      if (cmd.findOption("--dont-overwrite-uids")) opt_overwriteUIDs = OFFalse;
      if (cmd.findOption("--overwrite-uids")) opt_overwriteUIDs = OFTrue;
      cmd.endOptionBlock();

      /* output options */

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-file")) opt_writeMode = EWM_fileformat;
      if (cmd.findOption("--write-dataset")) opt_writeMode = EWM_dataset;
      cmd.endOptionBlock();

      if (cmd.findOption("--update-meta-info"))
      {
          app.checkConflict("--update-meta-info", "--write-dataset", opt_writeMode == EWM_dataset);
          opt_writeMode = EWM_updateMeta;
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-xfer-same")) opt_xfer = EXS_Unknown;;
      if (cmd.findOption("--write-xfer-little")) opt_xfer = EXS_LittleEndianExplicit;
      if (cmd.findOption("--write-xfer-big")) opt_xfer = EXS_BigEndianExplicit;
      if (cmd.findOption("--write-xfer-implicit")) opt_xfer = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
      if (cmd.findOption("--write-xfer-deflated")) opt_xfer = EXS_DeflatedLittleEndianExplicit;
#endif
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--stop-on-error")) opt_stopOnErrors = OFTrue;
      if (cmd.findOption("--ignore-errors")) opt_stopOnErrors = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
          dcmEnableUnknownVRGeneration.set(OFTrue);
          dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
          dcmEnableOtherFloatStringVRGeneration.set(OFTrue);
          dcmEnableOtherDoubleStringVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
          dcmEnableUnknownVRGeneration.set(OFFalse);
          dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
          dcmEnableOtherFloatStringVRGeneration.set(OFFalse);
          dcmEnableOtherDoubleStringVRGeneration.set(OFFalse);
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--group-length-recalc")) opt_glenc = EGL_recalcGL;
      if (cmd.findOption("--group-length-create")) opt_glenc = EGL_withGL;
      if (cmd.findOption("--group-length-remove")) opt_glenc = EGL_withoutGL;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--length-explicit")) opt_enctype = EET_ExplicitLength;
      if (cmd.findOption("--length-undefined")) opt_enctype = EET_UndefinedLength;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--padding-retain"))
      {
          app.checkConflict("--padding-retain", "--write-dataset", opt_writeMode == EWM_dataset);
          opt_padenc = EPD_noChange;
      }
      if (cmd.findOption("--padding-off")) opt_padenc = EPD_withoutPadding;
      if (cmd.findOption("--padding-create"))
      {
          app.checkConflict("--padding-create", "--write-dataset", opt_writeMode == EWM_dataset);
          app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
          app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
          opt_padenc = EPD_withPadding;
      }
      cmd.endOptionBlock();

#ifdef WITH_ZLIB
      if (cmd.findOption("--compression-level"))
      {
          OFCmdUnsignedInt comprLevel = 0;
          app.checkDependence("--compression-level", "--write-xfer-deflated", opt_xfer == EXS_DeflatedLittleEndianExplicit);
          app.checkValue(cmd.getValueAndCheckMinMax(comprLevel, 0, 9));
          dcmZlibCompressionLevel.set(OFstatic_cast(int, comprLevel));
      }
#endif
    }

    /* print resource identifier */
    OFLOG_DEBUG(dump2dcmLogger, rcsid << OFendl);

    DcmFileFormat fileformat;
    DcmMetaInfo *metaheader = (opt_metaInfo) ? fileformat.getMetaInfo() : NULL;
    DcmDataset *dataset = fileformat.getDataset();

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dump2dcmLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    // open input dump file
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_FATAL(dump2dcmLogger, "invalid input filename: <empty string>");
        return 1;
    }
    OFLOG_INFO(dump2dcmLogger, "reading dump file: " << opt_ifname);

    FILE *dumpfile = fopen(opt_ifname, "r");
    if (!dumpfile)
    {
        OFLOG_FATAL(dump2dcmLogger, "input file does not exist: " << opt_ifname);
        return 1;
    }

    int status = 0;
    E_TransferSyntax xfer;
    // read dump file into metaheader and dataset
    if (readDumpFile(metaheader, dataset, dumpfile, opt_ifname, xfer, opt_stopOnErrors,
        OFstatic_cast(unsigned long, opt_linelength)))
    {
        /* generate new UIDs (if required) */
        if (opt_generateUIDs)
        {
            char uid[100];
            if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_StudyInstanceUID))
            {
                OFLOG_INFO(dump2dcmLogger, "generating new Study Instance UID");
                dataset->putAndInsertString(DCM_StudyInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
            }
            if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SeriesInstanceUID))
            {
                OFLOG_INFO(dump2dcmLogger, "generating new Series Instance UID");
                dataset->putAndInsertString(DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
            }
            if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SOPInstanceUID))
            {
                OFLOG_INFO(dump2dcmLogger, "generating new SOP Instance UID");
                dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
                /* make sure that the file meta information is updated correspondingly */
                if (opt_writeMode == EWM_fileformat)
                    opt_writeMode = EWM_updateMeta;
            }
        }

        // write into file format or dataset
        OFLOG_INFO(dump2dcmLogger, "writing DICOM file");

        /* determine transfer syntax to write the file */
        if (opt_xfer == EXS_Unknown)
        {
            opt_xfer = xfer;
            /* check whether output xfer is still unknown */
            if (opt_xfer == EXS_Unknown)
            {
                OFLOG_WARN(dump2dcmLogger, "output transfer syntax unknown, assuming --write-xfer-little");
                opt_xfer = EXS_LittleEndianExplicit;
            }
        }
        /* check whether it is possible to write the file */
        if (fileformat.canWriteXfer(opt_xfer))
        {
            /* check whether pixel data is compressed */
            if ((opt_writeMode == EWM_dataset) && DcmXfer(xfer).isEncapsulated())
            {
                OFLOG_WARN(dump2dcmLogger, "encapsulated pixel data require file format, ignoring --write-dataset");
                opt_writeMode = EWM_fileformat;
            }
            OFCondition l_error = fileformat.saveFile(opt_ofname, opt_xfer, opt_enctype, opt_glenc, opt_padenc,
                OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);

            if (l_error == EC_Normal)
            {
                OFLOG_INFO(dump2dcmLogger, "dump successfully converted");
            } else {
                OFLOG_ERROR(dump2dcmLogger, l_error.text() << ": writing file: "  << opt_ofname);
                status = 1;
            }
        } else {
            OFLOG_ERROR(dump2dcmLogger, "no conversion to transfer syntax " << DcmXfer(opt_xfer).getXferName() << " possible!");
            status = 2;
        }
    }
    fclose(dumpfile);

    return status;
}

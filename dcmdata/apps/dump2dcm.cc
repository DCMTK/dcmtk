/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-20 07:41:34 $
 *  CVS/RCS Revision: $Revision: 1.70 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
 *           file will be read as is.  OW data is expected to be little endian
 *           ordered and will be swapped if necessary.  No checks will be made
 *           to ensure that the amount of data is reasonable in terms of other
 *           attributes such as Rows or Columns.
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
    for (i = 0; i < len && isspace(TO_UCHAR(s[i])); i++) /*loop*/;
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
            swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, buf, buflen, sizeof(Uint16));
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
                         * must be written as multivalued hexidecimal (e.g. "00\ff\0d\8f");
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
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
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
      }
      if (cmd.findOption("--disable-new-vr"))
      {
          dcmEnableUnknownVRGeneration.set(OFFalse);
          dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
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


/*
** CVS/RCS Log:
** $Log: dump2dcm.cc,v $
** Revision 1.70  2010-10-20 07:41:34  uli
** Made sure isalpha() & friends are only called with valid arguments.
**
** Revision 1.69  2010-10-14 13:13:30  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.68  2010-06-03 10:30:17  joergr
** Replaced calls to strerror() by new helper function OFStandard::strerror()
** which results in using the thread safe version of strerror() if available.
**
** Revision 1.67  2010-05-20 09:23:45  joergr
** Use new OFStandard::getFileSize() method where appropriate.
**
** Revision 1.66  2009-11-13 13:20:23  joergr
** Fixed minor issues in log output.
**
** Revision 1.65  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.64  2009-08-21 09:25:13  joergr
** Added parameter 'writeMode' to save/write methods which allows for specifying
** whether to write a dataset or fileformat as well as whether to update the
** file meta information or to create a new file meta information header.
**
** Revision 1.63  2009-05-07 09:08:03  joergr
** Added new command line options that allow for generating new Study/Series/SOP
** Instance UIDs (incl. an option for overwriting existing values).
** Added new command line options that allow for ignoring the file meta
** information and for updating particular file meta information.
**
** Revision 1.62  2008-11-26 12:07:42  joergr
** Updated documentation of newDicomElement() in order to reflect the current
** implementation.
**
** Revision 1.61  2008-11-03 16:42:59  joergr
** Added experimental support for importing very large binary files (e.g. pixel
** data) using new createValueFromTempFile() method.
** Added ZLIB related output options --write-xfer-deflated, --compression-level.
**
** Revision 1.60  2008-09-25 14:38:48  joergr
** Moved output of resource identifier in order to avoid printing the same
** information twice.
**
** Revision 1.59  2008-09-25 11:19:48  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.58  2007/06/07 09:06:01  joergr
** Enhanced support for very large binary data elements (by reducing the overall
** memory consumption).
**
** Revision 1.57  2007/03/09 15:01:02  joergr
** Updated documentation of dump input format.
**
** Revision 1.56  2007/03/07 12:30:13  joergr
** Fixed issue with uncompressed icon images in DICOM images with encapsulated
** transfer syntax
**
** Revision 1.55  2007/02/22 13:07:42  joergr
** Added support for compressed pixel data.
** Added new command line option --write-xfer-same.
**
** Revision 1.54  2006/08/15 15:50:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.53  2006/07/27 13:52:42  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
** Option "--help" is no longer an exclusive option by default.
**
** Revision 1.52  2006/03/23 17:16:21  joergr
** Added missing fclose() statement at the end of main().
**
** Revision 1.51  2005/12/16 09:07:03  onken
** Added variable initialization to avoid compiler warning
**
** Revision 1.50  2005/12/08 15:40:50  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.49  2004/07/13 09:43:10  meichel
** Fixed memory leak occuring when raw data is read from file.
**
** Revision 1.48  2004/03/05 09:59:00  joergr
** Avoid wrong warning for LUTData (0028,3006) having a VR of US or SS.
** Added initial "hooks" for (compressed) pixel items.
** Added "ignore errors" option (similar to dcmdump).
**
** Revision 1.47  2004/01/16 10:53:16  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.46  2003/11/05 16:15:27  meichel
** Removed useless "--write-xfer-same" command line option
**
** Revision 1.45  2002/12/05 13:59:29  joergr
** Fixed typo.
**
** Revision 1.44  2002/11/27 12:07:18  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.43  2002/11/26 08:43:02  meichel
** Replaced all includes for "zlib.h" with <zlib.h>
**   to avoid inclusion of zlib.h in the makefile dependencies.
**
** Revision 1.42  2002/09/23 17:52:04  joergr
** Prepared code for future support of 'config.guess' host identifiers.
**
** Revision 1.41  2002/09/23 13:50:42  joergr
** Added new command line option "--version" which prints the name and version
** number of external libraries used.
**
** Revision 1.40  2002/08/21 10:14:16  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.39  2002/04/16 13:38:55  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.38  2001/12/11 14:00:39  joergr
** Fixed bug in 'dump2dcm' parser causing AT attribute values to be ignored.
**
** Revision 1.37  2001/11/09 15:50:53  joergr
** Renamed some of the getValue/getParam methods to avoid ambiguities reported
** by certain compilers.
**
** Revision 1.36  2001/09/25 17:21:01  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.35  2001/06/01 15:48:30  meichel
** Updated copyright header
**
** Revision 1.34  2000/04/14 15:42:54  meichel
** Global VR generation flags are now derived from OFGlobal and, thus,
**   safe for use in multi-thread applications.
**
** Revision 1.33  2000/03/08 16:26:06  meichel
** Updated copyright header.
**
** Revision 1.32  2000/03/06 18:09:38  joergr
** Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
**
** Revision 1.31  2000/03/03 14:05:16  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.30  2000/02/29 11:48:51  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.29  2000/02/23 15:11:36  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.28  2000/02/10 16:02:51  joergr
** Enhanced handling of PixelData/Item element. Externally stored raw data is
** now always imported as little endian and swapped if necessary. This change
** reflects the new 'export' feature of dcmdump.
**
** Revision 1.27  2000/02/01 10:11:58  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.26  1999/05/03 14:13:40  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.25  1999/04/27 17:50:53  joergr
** Adapted console applications to new OFCommandLine and OFConsoleApplication
** functionality.
**
** Revision 1.24  1999/04/27 12:23:27  meichel
** Prevented dcmdata applications from opening a file with empty filename,
**   leads to application crash on Win32.
**
** Revision 1.23  1999/03/31 09:24:23  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.22  1999/03/29 10:14:15  meichel
** Adapted command line options of dcmdata applications to new scheme.
**
** Revision 1.21  1999/03/22 16:16:01  meichel
** dump2dcm now allows to include the contents of binary files
**   as OB/OW values while converting a dump to a DICOM file.
**
** Revision 1.20  1999/01/07 14:13:12  meichel
** Corrected bug in dump2dcm that prevented the correct processing of
**   dumps created with dcmdump if they contained the "internal" VR markers
**   "xs" (US or SS) or "ox" (OB or OW).
**
** Revision 1.19  1998/01/27 10:51:27  meichel
** Removed some unused variables, meaningless const modifiers
**   and unreached statements.
**
** Revision 1.18  1998/01/14 14:41:15  hewett
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.17  1997/08/05 07:34:54  andreas
** Corrected Error handling of SQ in dump2dcm
**
** Revision 1.16  1997/07/21 07:59:02  andreas
** - Deleted support for DcmPixelItems and DcmPixelSequences in dump2dcm
**   ToDo: New support should be added in the future compatible to
**   the new DcmPixel class.
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.15  1997/07/03 15:09:40  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.14  1997/05/30 06:44:57  andreas
** - fixed scanf format problem leading to warnings on 64 bit machines.
**
** Revision 1.13  1997/05/29 15:52:52  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.12  1997/05/22 13:26:25  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.11  1997/05/20 07:57:12  andreas
** - Removed obsolete applications file2ds and ds2file. The functionality of these
**   applications is now peformed by dcmconv. Unified calling parameters
**   are implemented in dump2dcm, dcmdump and dcmconv.
**
** Revision 1.10  1997/05/16 08:31:06  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.9  1997/04/18 08:06:56  andreas
** - Minor corrections: correct some warnings of the SUN-C++ Compiler
**   concerning the assignments of wrong types and inline compiler
**   errors
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.8  1997/03/27 15:47:25  hewett
** Added command line switche to allow generation of UN to be
** disabled (it is enabled by default).
**
** Revision 1.7  1996/09/24 16:13:51  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.6  1996/05/02 17:00:23  hewett
** Corrected program name in usage description.
**
** Revision 1.5  1996/05/02 15:55:11  hewett
** Stopped whitespace being stripped from inside value strings when
** no [] delimiter present.  Now only leading and trailing whitespace
** is stripped.
**
** Revision 1.4  1996/04/27 12:13:01  hewett
** Corrected bug in last bug-fix.  A tag value [some text] was being
** parsed as an empty string.  Now both [] and [some text] appear to
** work as intended.
**
** Revision 1.3  1996/03/22 12:38:44  andreas
** Correct some mistakes: handling [] as empty string (no value field)
**                        handling =Name correct if Name is not correct
**
** Revision 1.2  1996/03/12 15:11:39  hewett
** Added call to prepareCmdLineArgs to enable command line arguments
** in environments which do not provide them.
**
** Revision 1.1  1996/01/29 13:36:38  andreas
** dump2dcm added convert ASCII descriptions into DICOM files
**
**
*/

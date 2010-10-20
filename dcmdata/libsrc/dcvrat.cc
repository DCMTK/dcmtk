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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmAttributeTag
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.33 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmAttributeTag::DcmAttributeTag(const DcmTag &tag,
                                 const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmAttributeTag::DcmAttributeTag(const DcmAttributeTag &old)
  : DcmElement(old)
{
}


DcmAttributeTag::~DcmAttributeTag()
{
}


DcmAttributeTag &DcmAttributeTag::operator=(const DcmAttributeTag &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


OFCondition DcmAttributeTag::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmAttributeTag &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmAttributeTag::ident() const
{
    return EVR_AT;
}


OFCondition DcmAttributeTag::checkValue(const OFString &vm,
                                       const OFBool /*oldFormat*/)
{
    /* check VM only, further checks on the attribute tags could be added later */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmAttributeTag::getVM()
{
    /* attribute tags store pairs of 16 bit values */
    return OFstatic_cast(unsigned long, getLengthField() / (2 * sizeof(Uint16)));
}


// ********************************


void DcmAttributeTag::print(STD_NAMESPACE ostream& out,
                            const size_t flags,
                            const int level,
                            const char * /*pixelFileName*/,
                            size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get unsigned integer data */
        Uint16 *uintVals;
        errorFlag = getUint16Array(uintVals);
        const unsigned long count = getVM();
        if ((uintVals != NULL) && (count > 0))
        {
            /* determine number of values to be printed */
            unsigned long expectedLength = count * (11 + 1) - 1;
            const unsigned long printCount =
                ((expectedLength > DCM_OptPrintLineLength) && (flags & DCMTypes::PF_shortenLongTagValues)) ?
                (DCM_OptPrintLineLength - 3 /* for "..." */ + 1) / (11 /* (gggg,eeee) */ + 1 /* for "\" */) : count;
            unsigned long printedLength = printCount * (11 + 1) - 1;
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            if (printCount > 0)
            {
                out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0');
                /* print tag values (group,element) in hex mode */
                out << '(' << STD_NAMESPACE setw(4) << (*(uintVals++));
                out << ',' << STD_NAMESPACE setw(4) << (*(uintVals++)) << ')';
                for (unsigned long i = 1; i < printCount; i++)
                {
                    out << "\\" << '(' << STD_NAMESPACE setw(4) << (*(uintVals++));
                    out << ',' << STD_NAMESPACE setw(4) << (*(uintVals++)) << ')';
                }
                /* reset i/o manipulators */
                out << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
            }
            /* print trailing "..." if data has been truncated */
            if (printCount < count)
            {
                out << "...";
                printedLength += 3;
            }
            /* print line end with length, VM and tag name */
            printInfoLineEnd(out, flags, printedLength);
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmAttributeTag::getTagVal(DcmTagKey &tagVal,
                                       const unsigned long pos)
{
    /* get unsigned integer data */
    Uint16 *uintValues;
    errorFlag = getUint16Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            tagVal.set(uintValues[2 * pos] /*group*/, uintValues[2 * pos + 1] /*element*/);
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        tagVal = DcmTagKey();
    return errorFlag;
}


OFCondition DcmAttributeTag::getUint16Array(Uint16 *&uintVals)
{
    uintVals = OFstatic_cast(Uint16 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::getOFString(OFString &stringVal,
                                         const unsigned long pos,
                                         OFBool /*normalize*/)
{
    DcmTagKey tagVal;
    /* get the specified tag value */
    errorFlag = getTagVal(tagVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "(%4.4x,%4.4x)", tagVal.getGroup(), tagVal.getElement());
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::putTagVal(const DcmTagKey &tagVal,
                                       const unsigned long pos)
{
    /* create tag data */
    Uint16 uintVals[2];
    uintVals[0] = tagVal.getGroup();
    uintVals[1] = tagVal.getElement();
    /* change element value */
    errorFlag = changeValue(uintVals, 2 * sizeof(Uint16) * OFstatic_cast(Uint32, pos), 2 * sizeof(Uint16));
    return errorFlag;
}


OFCondition DcmAttributeTag::putUint16Array(const Uint16 *uintVals,
                                            const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints > 0)
    {
        /* check for valid data */
        if (uintVals != NULL)
            errorFlag = putValue(uintVals, 2 * sizeof(Uint16) * Uint32(numUints));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Uint16 * field = new Uint16[2 * vm];
            const char *s = stringVal;
            char *value;
            /* retrieve attribute tag data from character string */
            for (unsigned long i = 0; (i < 2 * vm) && errorFlag.good(); i += 2)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if ((value == NULL) || sscanf(value, "(%hx,%hx)", &field[i], &field[i + 1]) != 2)
                    errorFlag = EC_CorruptedData;
                delete[] value;
            }
            /* set binary data as the element value */
            if (errorFlag.good())
                errorFlag = putUint16Array(field, vm);
            /* delete temporary buffer */
            delete[] field;
        } else
            putValue(NULL, 0);
    } else
        putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (2 * sizeof(Uint16)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % (2* sizeof(Uint16))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::checkStringValue(const OFString &value,
                                              const OFString &vm)
{
    return DcmElement::checkVM(getVMFromString(value.c_str()), vm);
}


/*
** CVS/RCS Log:
** $Log: dcvrat.cc,v $
** Revision 1.33  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.32  2010-10-14 13:14:09  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.31  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.30  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.29  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.28  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.27  2005/12/08 15:41:47  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.26  2004/02/04 16:48:43  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.25  2003/10/09 13:51:52  joergr
** Fixed issue with the order of group and element number in print method.
**
** Revision 1.24  2003/03/25 17:11:34  joergr
** Fixed bug in print method: wrong position of setw() operators.
**
** Revision 1.23  2002/12/06 13:12:37  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.22  2002/11/27 12:06:55  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.21  2002/04/25 10:27:43  joergr
** Added getOFString() implementation.
**
** Revision 1.20  2002/04/16 13:43:23  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.19  2001/09/25 17:19:55  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.18  2001/06/01 15:49:14  meichel
** Updated copyright header
**
** Revision 1.17  2000/04/14 15:55:08  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.16  2000/03/08 16:26:45  meichel
** Updated copyright header.
**
** Revision 1.15  2000/03/03 14:05:38  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.14  2000/02/10 10:52:23  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.13  2000/02/02 14:32:55  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.12  1999/03/31 09:25:47  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1997/07/21 08:25:32  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.10  1997/07/03 15:10:08  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.9  1997/04/18 08:17:19  andreas
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
** Revision 1.8  1996/08/05 08:46:18  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:50  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/16 16:04:04  andreas
** - new put parameter DcmTagKey for DcmAttributeTag elements
** - better support for NULL element value
**
** Revision 1.5  1996/03/26 09:59:34  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:31  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:46  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/

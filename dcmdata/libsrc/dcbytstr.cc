/*
 *
 *  Copyright (C) 1994-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmByteString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-10 10:52:16 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcbytstr.cc,v $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB_4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB_4
END_EXTERN_C
#endif
#endif

#include <string.h>
#include <stdio.h>
#include <iostream.h>


#include "ofstring.h"
#include "dcbytstr.h"
#include "dcvr.h"
#include "dcdebug.h"


// ********************************


DcmByteString::DcmByteString(const DcmTag &tag,
                             const Uint32 len)
  : DcmElement(tag, len),
    realLength(len),
    fStringMode(DCM_UnknownString),
    paddingChar(' '),
    maxLength(DCM_UndefinedLength)
{
}


// ********************************


DcmByteString::DcmByteString(const DcmByteString& old)
  : DcmElement(old),
    realLength(old.realLength),
    fStringMode(old.fStringMode),
    paddingChar(old.paddingChar),
    maxLength(old.maxLength)
{
}


// ********************************

DcmByteString::~DcmByteString(void)
{
}

// ********************************

DcmByteString& DcmByteString::operator=(const DcmByteString& obj)
{
  DcmElement::operator=(obj);
  realLength=obj.realLength;
  fStringMode=obj.fStringMode;
  paddingChar=obj.paddingChar;
  maxLength=obj.maxLength;
  return *this;
}

// ********************************


void DcmByteString::print(ostream & out, const OFBool showFullData,
                          const int level, const char * /*pixelFileName*/,
                          size_t * /*pixelCounter*/)
{
    if (this -> valueLoaded())
    {
        char * byteStringValue = NULL; 
        this -> getString(byteStringValue);
        if (byteStringValue)
        {
            Uint32 printLength;
            if (showFullData)
                printLength = realLength;
            else
                printLength = DCM_OptPrintLineLength-5 < realLength ? 
                    DCM_OptPrintLineLength : realLength;

            char *tmp = new char[printLength + 5];
            tmp[0] = '[';
            strncpy( tmp+1, byteStringValue, size_t(printLength));
            if (showFullData || printLength == realLength)
            {
                tmp[printLength + 1 ] = ']';
                tmp[printLength + 2 ] = '\0';
            }
            else
            {
                tmp[printLength+1] = '\0';
                strcat(tmp, "...");
            }
                
            printInfoLine(out, showFullData, level, tmp);
            delete[] tmp;
        }
        else
            printInfoLine(out, showFullData, level, "(no value available)" );
    }
    else
        printInfoLine(out, showFullData, level, "(not loaded)" );
}


// ********************************

Uint32 
DcmByteString::getLength(
    const E_TransferSyntax /*xfer*/,
    const E_EncodingType /*enctype*/)
{
    makeDicomByteString();
    return Length;
}


    
// ********************************

E_Condition
DcmByteString::getOFString(OFString & str,
                           const unsigned long pos,
                           OFBool /*normalize*/)
{
    errorFlag = EC_Normal;
    if (pos >= getVM())
        errorFlag = EC_IllegalCall;
    else
    {
        char * s = (char *)getValue();
        if (errorFlag == EC_Normal) errorFlag = getStringPart(str, s, pos);
    }
    return errorFlag;
}

// ********************************

E_Condition 
DcmByteString::getOFStringArray(
    OFString & str,
    OFBool /*normalize*/)
{
    const char * s = (char *)getValue();
    if (s) str = s;
    else s = "";		// meaning ?? (perhaps 'str = ""')
    return errorFlag;
}


// ********************************


E_Condition DcmByteString::getString(char * & byteStringValue)
{
    byteStringValue = (char *)this -> getValue();

    if (byteStringValue && fStringMode != DCM_MachineString)
        this -> makeMachineByteString();

    return errorFlag;
}


// ********************************


Uint32 DcmByteString::getRealLength(void)
{
    switch (fStringMode)
    {
    case DCM_MachineString:
        break;

    case DCM_DicomString:
    case DCM_UnknownString:
        this -> makeMachineByteString();
    }

    return realLength;
}


// ********************************


unsigned long DcmByteString::getVM()
{
    char * s = NULL;
    this -> getString(s);
    unsigned long vm = 0;
    if (s == NULL || Length == 0)
        vm = 0;
    else
    {
        vm = 1;
        char c;
        while ((c = *s++) != 0)
            if ( c == '\\' )
                vm++;
    }
    return vm;
}


// ********************************

E_Condition DcmByteString::makeDicomByteString(void)
{
    char * value = NULL;
    errorFlag = this -> getString(value);

    if (value) 
    {
        if (realLength & 1)
        {
            Length = realLength + 1;
            value[realLength] = paddingChar;
        }
        else if (realLength < Length)
            Length = realLength;

        value[Length] = '\0';
    }
    fStringMode = DCM_DicomString;
    return errorFlag;
}

// ********************************

E_Condition DcmByteString::makeMachineByteString(void)
{
    errorFlag = EC_Normal;
    char * value = NULL;
    value = (char *)this -> getValue();
    if (value)
        realLength = strlen(value);
    else
        realLength = 0;

    if (dcmEnableAutomaticInputDataCorrection) {
        /* 
        ** This code removes extra padding chars at the end of
        ** a ByteString.  Trailing padding can cause problems
        ** when comparing strings.
        */
        if (realLength) {
            size_t i = 0;
            for(i = size_t(realLength);
                i > 0 && (value[i-1] == paddingChar);
                i--) {
                value[i-1] = '\0';
            }
            realLength = (Uint32)i;
        }
    }
    fStringMode = DCM_MachineString;
    return errorFlag;
}


// ********************************

Uint8 * DcmByteString::newValueField(void)
{
    // This is correct since the relation Length >= realLength is true.

    Uint8 * value = NULL;
    if (Length & 1)
    {
        value = new Uint8[Length+2]; // protocol error: odd value length 
        if (value)
            value[Length] = 0;
        Length++;               // make value length even
    }
    else
        value = new Uint8[Length+1];

    if (value)
        value[Length] = 0;

    return value;
}

// ********************************

void DcmByteString::postLoadValue(void)
{
    fStringMode = DCM_UnknownString;
}

// ********************************


E_Condition 
DcmByteString::putString(const char * byteStringValue)
{
    errorFlag = EC_Normal;

    if (byteStringValue && byteStringValue[0] != '\0')
        this -> putValue(byteStringValue, strlen(byteStringValue));
    else
        this -> putValue(NULL, 0);

    fStringMode = DCM_UnknownString;
    this -> makeMachineByteString();

    return errorFlag;
}

// ********************************

// sets the value of a complete (possibly multi-valued) string attribute.
E_Condition 
DcmByteString::putOFStringArray(const OFString& stringValue)
{
    return this -> putString(stringValue.c_str());
}


// ********************************

E_Condition DcmByteString::verify(const OFBool autocorrect)
{
    char * value = NULL;
    errorFlag = this -> getString(value);
    if (value != NULL && realLength != 0 )
    {
        char *tempstr = new char[ realLength + 1 ];
        unsigned long field = 0;
        unsigned long num = getVM();
        unsigned long pos = 0;
        unsigned long temppos = 0;
        char c;
        while (field < num )
        {
            unsigned long fieldlen = 0;
            while ((c = value[pos++]) != 0 && c != '\\')
            {
                if (fieldlen < maxLength && autocorrect)
                    tempstr[temppos++] = c;
                fieldlen++;
            }
            if (fieldlen >= maxLength)
                errorFlag = EC_CorruptedData;

            if (autocorrect)
                tempstr[temppos++] = c;      // c ist entweder '\\' oder NULL

            field++;
            if (pos > Length)
                break;
        }
        if (autocorrect)
            this -> putString(tempstr);
        delete[] tempstr;
    }

    Cdebug(3, errorFlag!=EC_Normal,
            ("DcmByteString::verify: Illegal values in Tag=(0x%4.4x,0x%4.4x) VM=%d",
            getGTag(), getETag(), getVM() ));

    return errorFlag;
}

// ********************************

E_Condition DcmByteString::clear(void)
{
    errorFlag = DcmElement::clear();
    fStringMode = DCM_UnknownString;
    return errorFlag;
}


// ********************************

E_Condition DcmByteString::write(DcmStream & outStream,
                                 const E_TransferSyntax oxfer,
                                 const E_EncodingType enctype)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        if (fTransferState == ERW_init)
            this -> makeDicomByteString();

        errorFlag = DcmElement::write(outStream, oxfer, enctype);
    }
    return errorFlag;
}



/**** Function to get Part of a DICOM String ****/

E_Condition 
getStringPart(
    OFString & result,
    char * orgStr,
    const unsigned long which)
{
    E_Condition l_error = EC_Normal;
    if (!orgStr) l_error = EC_IllegalCall;
    else
    {
        unsigned long i=0;
        while (i<which && *orgStr != '\0') if (*orgStr++ == '\\') i++;
        if (i==which)
        {
            char *t = orgStr;
            while (*t != '\0' && *t != '\\') t++;
            if (t - orgStr >0) result.assign(orgStr, t-orgStr);
            else result = "";
        } 
        else l_error = EC_IllegalCall;
    }
    return l_error;
}

/*** general Function for Normalizing of DICOM STRINGS ** */

void 
normalizeString(
    OFString & str,
    const OFBool multiPart,
    const OFBool leading,
    const OFBool trailing)
{
    if (!str.empty())
    {
        size_t partindex = 0;
        size_t offset = 0;
        size_t len = str.length();
        while (partindex < len)
        {
            // remove leading spaces in every part of the string
            if (leading)
            {
                offset = 0;
                while(partindex+offset < len && str[partindex+offset] == ' ') 
                    offset++;
                if (offset) str.erase(partindex, offset);
            }

            len = str.length();
            // compute begin to the next separator index!
            if (multiPart)
            {
                partindex = str.find('\\', partindex);
                if (partindex == OFString_npos) partindex = len;
            }
            else partindex = len;

            // remove trailing spaces in every part of the string
            if (trailing && partindex)
            {
                offset = partindex-1;
                while(offset && str[offset] == ' ') offset--;
                if (offset != partindex-1)
                {
                    if (str[offset] == ' ')
                    {
                        str.erase(offset, partindex-offset);
                        partindex = offset;
                    }
                    else
                    {
                        str.erase(offset+1, partindex-offset-1);
                        partindex = offset+1;
                    }
                }
            }

            len = str.length();
            if (partindex != len) ++partindex;
        }
    }
}



/*
** CVS/RCS Log:
** $Log: dcbytstr.cc,v $
** Revision 1.24  2000-02-10 10:52:16  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.23  2000/02/02 14:32:47  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.22  2000/02/01 10:12:04  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.21  1999/03/31 09:25:16  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.20  1998/11/12 16:48:13  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.19  1998/07/15 15:51:46  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.18  1997/10/13 11:33:48  hewett
** Fixed bug in DcmByteString::getOFString due to inverse logic causing
** a string to be retrieved for all illegal values of pos while the errorFlag
** was set to EC_IllegalCall for all legal values of pos (thanks to Phil Liao
** <phil@eeg.com> for suggesting the bugfix).
**
** Revision 1.17  1997/09/11 15:18:16  hewett
** Added a putOFStringArray method.
**
** Revision 1.16  1997/08/29 08:32:53  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.15  1997/07/24 13:10:50  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.14  1997/07/21 07:56:39  andreas
** - Corrected error in length computation of DcmItem for strings in
**   items.
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.13  1997/07/03 15:09:52  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.12  1997/05/16 08:31:27  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.11  1997/04/18 08:17:13  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.10  1997/03/26 17:05:51  hewett
** Added global flag for disabling the automatic correction of small errors.
** Such behaviour is undesirable when performing data validation.
**
** Revision 1.9  1996/08/05 08:46:07  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.8  1996/05/31 09:09:08  hewett
** The stripping of trailing padding characters has been restored (without
** the 8bit char removal bug).  Trailing padding characters are insignificant
** and if they are not removed problems arise with string comparisons since
** the dicom encoding rules require the addition of a padding character for
** odd length strings.
**
** Revision 1.7  1996/05/30 17:17:49  hewett
** Disabled erroneous code to strip trailing padding characters.
**
** Revision 1.6  1996/04/16 16:05:22  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/11 13:17:23  hewett
** Removed get function for unsigned char*
**
** Revision 1.4  1996/01/09 11:06:42  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:32  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/


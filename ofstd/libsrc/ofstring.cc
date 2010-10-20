/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: A simple string class
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-20 07:41:37 $
 *  CVS/RCS Revision: $Revision: 1.33 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


/*
** A simple string class
** - for OFFIS projects when an ANSI string class is not always available
** - based on the ANSI-C++ specifications
** - this impementation is intended to be slow but reliable
** - it is known to be slow but is it reliable?
*/

#include "dcmtk/config/osconfig.h"     /* include OS specific configuration first */

#ifndef HAVE_STD_STRING

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofbmanip.h"

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

static const char* verify_string(const char *s)
{
    if (s == NULL)
    {
#ifdef DEBUG
        fprintf(stderr, "WARNING: OFString constructed from NULL, this is deprecated.\n");
#endif
#ifdef USE_NULL_SAFE_OFSTRING
        s = "";
#endif
    }
    return s;
}

/*
** Constructors
*/

OFString::OFString()
    : theCString(NULL), theSize(0), theCapacity(0)
{
    reserve(1);
}

OFString::OFString(const OFString& str, size_t pos, size_t n)
    : theCString(NULL), theSize(0), theCapacity(0)
{
    this->assign(str, pos, n);
}

OFString::OFString (const char* s, size_t n)
    : theCString(NULL), theSize(0), theCapacity(0)
{
    s = verify_string(s);
    if (n == OFString_npos) {
        n = strlen(s);
    }
    reserve(n);
    OFBitmanipTemplate<char>::copyMem(s, this->theCString, n);
    this->theCString[n] = '\0';
    this->theSize = n;
}

OFString::OFString (const char* s)
    : theCString(NULL), theSize(0), theCapacity(0)
{
    s = verify_string(s);
    const size_t n = strlen(s);
    reserve(n);
    // Because we used strlen() to figure out the length we can use strcpy()
    // since there won't be any '\0' bytes in the string.
    strcpy(this->theCString, s);
    this->theSize = n;
}


OFString::OFString (size_t rep, char c)
    : theCString(NULL), theSize(0), theCapacity(0)
{
    reserve(rep);
    for (size_t i = 0; i < rep; i++) {
        this->theCString[i] = c;
    }
    this->theCString[rep] = '\0';
    this->theSize = rep;
}

/*
** Destructor
*/

OFString::~OFString()
{
    if (theCString) {
        delete[] theCString;
        theCString = NULL;
    }
}

/*
** Operator =
*/

OFString&
OFString::operator= (const OFString& rhs)
{
    this->assign(rhs);
    return *this;
}

OFString&
OFString::operator= (const char* s)
{
    this->assign(s);
    return *this;
}

OFString&
OFString::operator= (char s)
{
    this->assign(1, s);
    return *this;
}

/*
** Operator +=
*/

OFString&
OFString::operator+= (const OFString& rhs)
{
    return this->append(rhs);
}

OFString&
OFString::operator+= (const char* s)
{
    return this->append(s);
}

OFString&
OFString::operator+= (char s)
{
    return this->append(1, s);
}

/*
** Append
*/

OFString&
OFString::append (const OFString& str, size_t pos, size_t n)
{
    OFString b(str, pos, n);
    this->reserve(this->size() + b.size());
    // We can't use strcat() because some string could contain NULL bytes
    // We need size() + 1 so that the EOS mark is copied over, too
    OFBitmanipTemplate<char>::copyMem(b.theCString, this->theCString + this->size(), b.size() + 1);
    this->theSize += b.size();

    return *this;
}

OFString&
OFString::append (const char* s, size_t n)
{
    OFString str(s, n);
    return this->append(str);
}

OFString&
OFString::append (const char* s)
{
    OFString str(s);
    return this->append(str);
}

OFString&
OFString::append (size_t rep, char c)
{
    OFString str(rep, c);
    return this->append(str);
}

/*
** Assign
*/

OFString&
OFString::assign (const OFString& str, size_t pos, size_t n)
{
    OFSTRING_OUTOFRANGE(pos > str.size());
    const size_t remain = (str.size() - pos);
    if ((n == OFString_npos) || (n > remain)) {
        n = remain;
    }
    if (n > 0) {
        this->reserve(n);
        // Someone could try to assign a string to itself, but strncpy() must
        // not be called on overlapping memory areas, therefore, we use moveMem().
        OFBitmanipTemplate<char>::moveMem(str.theCString + pos, this->theCString, n);
        this->theCString[n] = '\0';
        this->theSize = n;
    } else {
        this->reserve(1);
        /* assign an empty string */
        this->theCString[0] = '\0';
        this->theSize = 0;
    }
    return *this;
}

OFString&
OFString::assign (const OFString& str)
{
    return assign(str, 0, OFString_npos);
}

OFString&
OFString::assign (const char* s, size_t n)
{
    OFString str(s, n);
    return this->assign(str);
}

OFString&
OFString::assign (const char* s)
{
    OFString str(s);
    return this->assign(str);
}

OFString&
OFString::assign (size_t rep, char c)
{
    OFString str(rep, c);
    return this->assign(str);
}

/*
** Insert
*/

OFString&
OFString::insert (size_t pos1, const OFString& str, size_t pos2, size_t n)
{
    OFString i(str, pos2, n);
    OFString a(*this, OFstatic_cast(size_t, 0), pos1);
    OFString b(*this, pos1);
    return this->assign(a).append(i).append(b);
}

OFString&
OFString::insert (size_t pos, const char* s, size_t n)
{
    OFString str(s, n);
    return this->insert(pos, str);
}

OFString&
OFString::insert (size_t pos, const char* s)
{
    OFString str(s);
    return this->insert(pos, str);
}

OFString&
OFString::insert (size_t pos, size_t rep, char s)
{
    OFString str(rep, s);
    return this->insert(pos, str);
}

/*
** Erase
*/
OFString&
OFString::erase (size_t pos, size_t n)
{
    OFString a(*this, 0, pos);
    OFString b;
    if (n != OFString_npos) {
        b.assign(*this, pos + n, OFString_npos);
    }
    return this->assign(a).append(b);
}

/*
** Replace
*/

OFString&
OFString::replace (size_t pos1, size_t n1, const OFString& str,
                   size_t pos2, size_t n2)
{
    OFString a(*this, OFstatic_cast(size_t, 0), pos1);
    OFString b;
    if ((n1 < OFString_npos) && ((pos1 + n1) < this->size())) {
        b.assign(*this, pos1 + n1, OFString_npos);
    }
    OFString i(str, pos2, n2);
    return this->assign(a).append(i).append(b);
}

OFString&
OFString::replace (size_t pos, size_t n, const char* s, size_t n2)
{
    OFString str(s, n2);
    return this->replace(pos, n, str);
}

OFString&
OFString::replace (size_t pos, size_t n, const char* s)
{
    OFString str(s);
    return this->replace(pos, n, str);
}

OFString&
OFString::replace (size_t pos, size_t n, size_t rep, char s)
{
    OFString str(rep, s);
    return this->replace(pos, n, str);
}


/*
** Data
*/

const char*
OFString::data () const
{
    return (this->size() != 0) ? this->c_str() : "";
}


/*
** Resize
*/

void
OFString::resize (size_t n, char c)
{
    OFSTRING_LENGTHERROR(n == OFString_npos);

    reserve(n);
    const size_t len = this->size();
    if (n <= len) {
        for (size_t i = n; i < len; i++) {
            this->theCString[i] = '\0';
        }
    } else {
        for (size_t i = len; i < n; i++) {
            this->theCString[i] = c;
        }
        this->theCString[n] = '\0';
    }
    this->theSize = n;
}

/*
** Reserve
*/

void
OFString::reserve (size_t res_arg)
{
    if (res_arg == OFString_npos) {
        res_arg = 0; /* let at least space for eos get reserved */
    }
    res_arg++; /* add space for eos */
    if (this->theCapacity < res_arg) {
        char* newstr = new char[res_arg];
        if (newstr) {
            size_t usedSpace = 0;
            this->theCapacity = res_arg - 1; /* not the eos */
            if (this->size() > 0) {
                const size_t len = size();
                // copyMem() because theCString could have null bytes
                OFBitmanipTemplate<char>::copyMem(this->theCString, newstr, len);
                usedSpace = len;
            }
            OFBitmanipTemplate<char>::zeroMem(newstr + usedSpace, res_arg - usedSpace);
            char* oldstr = this->theCString;
            this->theCString = newstr;
            delete[] oldstr;
        } else {
            OFSTRING_MEMORYALLOCERROR(newstr);
        }
    }
}

/*
** Copy
*/

size_t
OFString::copy (char* s, size_t n, size_t pos) const
{
    OFString sub(this->substr(pos, n));
    // String length *including* the trailing EOS
    const size_t result = sub.size() + 1;

    // The string could have NULL bytes so no strncpy()
    OFBitmanipTemplate<char>::copyMem(this->theCString, s, result);
    return result;
}

/*
** Substr
*/

OFString
OFString::substr (size_t pos, size_t n) const
{
    OFString sub;
    return sub.assign(*this, pos, n);
}

/*
** Swap
*/

void
OFString::swap(OFString& s)
{
    char* tmpCString = s.theCString;
    s.theCString = this->theCString;
    this->theCString = tmpCString;

    size_t tmpSize = s.theSize;
    s.theSize = this->theSize;
    this->theSize = tmpSize;

    size_t tmpCapacity = s.theCapacity;
    s.theCapacity = this->theCapacity;
    this->theCapacity = tmpCapacity;
}

/*
** Compare
*/

int
OFString::compare (const OFString& str) const
{
    const size_t this_size = this->size();
    const size_t str_size = str.size();
    const size_t rlen = (this_size < str_size) ? this_size : str_size;
    // Our string could contain null bytes and thus we can't use strncmp()
    int result = memcmp(this->theCString, str.theCString, rlen);
    if (result == 0) {
        result = (this_size - str_size);
    }
    return result;
}

int
OFString::compare (size_t pos1, size_t n1, const OFString& str) const
{
    return OFString(*this, pos1, n1).compare(str);
}

int
OFString::compare (size_t pos1, size_t n1, const OFString& str,
                   size_t pos2, size_t n2) const
{
    return OFString(*this, pos1, n1).compare(OFString(str, pos2, n2));
}

int
OFString::compare (const char* s) const
{
    return this->compare(OFString(s));
}

int
OFString::compare (size_t pos1, size_t n1,
                   const char* s, size_t n2) const
{
    return OFString(*this, pos1, n1).compare(OFString(s, n2));
}

/*
** Find
*/

size_t
OFString::find (const OFString& pattern, size_t pos) const
{
    /* determine string length only once */
    const size_t this_size = this->size();
    const size_t pattern_size = pattern.size();
    if ((this_size == 0) || (pattern_size == 0) || (pos == OFString_npos)) {
        return OFString_npos;
    }
    for (size_t i = pos; i < this_size; i++) {
        /* is there enought space for the pattern? */
        if ((i + pattern_size) > this_size) {
            return OFString_npos;
        }
        int match = 1; /* assume there is a match */
        for (size_t j = 0; (j < pattern_size) && match; j++) {
            if (this->at(i + j) != pattern[j]) {
                match = 0;
            }
        }
        if (match) {
            return i;
        }
    }
    return OFString_npos;
}

size_t
OFString::find (const char* pattern, size_t pos, size_t n) const
{
    OFString str(pattern, n);
    return this->find(str, pos);
}

size_t
OFString::find (const char* pattern, size_t pos) const
{
    OFString str(pattern);
    return this->find(str, pos);
}

size_t
OFString::find (char pattern, size_t pos) const
{
    size_t i = pos;
    const size_t this_size = this->size();
    while ((i < this_size) && (this->at(i) != pattern))
        i++;
    return (i < this_size) ? i : OFString_npos;
}

/*
** Rfind
*/

size_t
OFString::rfind (const OFString& pattern, size_t pos) const
{
    /* determine string length only once */
    const size_t this_size = this->size();
    const size_t pattern_size = pattern.size();
    if ((this_size == 0) || (pattern_size == 0) || (this_size < pattern_size)) {
        return OFString_npos;
    }
    int above = ((this_size - pattern_size) < pos) ? (this_size - pattern_size) : pos;
    for (int i = above; i >= 0; i--) {
        int match = 1; /* assume there is a match */
        for (size_t j = 0; (j < pattern_size) && match; j++) {
            if (this->at(i + j) != pattern[j]) {
                match = 0;
            }
        }
        if (match) {
            return i;
        }
    }
    return OFString_npos;
}

size_t
OFString::rfind (const char* pattern, size_t pos, size_t n) const
{
    OFString str(pattern, n);
    return this->rfind(str, pos);
}

size_t
OFString::rfind (const char* pattern, size_t pos) const
{
    OFString str(pattern);
    return this->rfind(str, pos);
}

size_t
OFString::rfind (char pattern, size_t pos) const
{
    OFString str(1, pattern);
    return this->rfind(str, pos);
}

/*
** Find_first_of
*/

size_t
OFString::find_first_of (const OFString& str, size_t pos) const
{
    /* determine string length only once */
    const size_t this_size = this->size();
    const size_t str_size = str.size();
    if ((this_size == 0) || (str_size == 0) || (pos == OFString_npos)) {
        return OFString_npos;
    }
    for (size_t i = pos; i < this_size; i++) {
        for (size_t j = 0; j < str_size; j++) {
            if (this->at(i) == str[j]) {
                return i;
            }
        }
    }
    return OFString_npos;
}

size_t
OFString::find_first_of (const char* s, size_t pos, size_t n) const
{
    OFString str(s, n);
    return this->find_first_of(str, pos);
}

size_t
OFString::find_first_of (const char* s, size_t pos) const
{
    OFString str(s);
    return this->find_first_of(str, pos);
}

size_t
OFString::find_first_of (char s, size_t pos) const
{
    OFString str(1, s);
    return this->find_first_of(str, pos);
}

/*
** Find_last_of
*/

size_t
OFString::find_last_of (const OFString& str, size_t pos) const
{
    /* determine string length only once */
    const size_t this_size = this->size();
    const size_t str_size = str.size();
    if ((this_size == 0) || (str_size == 0)) {
        return OFString_npos;
    }
    if ((pos == OFString_npos) || (pos > this_size)) {
        pos = this_size;
    }
    for (int i = OFstatic_cast(int, pos - 1); i >= 0; i--) {
        for (size_t j = 0; j < str_size; j++) {
            if (this->at(i) == str[j]) {
                return i;
            }
        }
    }
    return OFString_npos;
}

size_t
OFString::find_last_of (const char* s, size_t pos, size_t n) const
{
    OFString str(s, n);
    return this->find_last_of(str, pos);
}

size_t
OFString::find_last_of (const char* s, size_t pos) const
{
    OFString str(s);
    return this->find_last_of(str, pos);
}

size_t
OFString::find_last_of (char s, size_t pos) const
{
    OFString str(1, s);
    return this->find_last_of(str, pos);
}

/*
** Find_first_not_of
*/

size_t
OFString::find_first_not_of (const OFString& str, size_t pos) const
{
    /* determine string length only once */
    const size_t this_size = this->size();
    const size_t str_size = str.size();
    if ((this_size == 0) || (str_size == 0) || (pos == OFString_npos)) {
        return OFString_npos;
    }
    for (size_t i = pos; i < this_size; i++) {
        if (str.find(this->at(i)) == OFString_npos)
            return i;
    }
    return OFString_npos;
}

size_t
OFString::find_first_not_of (const char* s, size_t pos, size_t n) const
{
    OFString str(s, n);
    return this->find_first_not_of(str, pos);
}

size_t
OFString::find_first_not_of (const char* s, size_t pos) const
{
    OFString str(s);
    return this->find_first_not_of(str, pos);
}

size_t
OFString::find_first_not_of (char s, size_t pos) const
{
    OFString str(1, s);
    return this->find_first_not_of(str, pos);
}

/*
** Find_last_not_of
*/

size_t
OFString::find_last_not_of (const OFString& str, size_t pos) const
{
    /* determine string length only once */
    const size_t this_size = this->size();
    const size_t str_size = str.size();
    if ((this_size == 0) || (str_size == 0)) {
        return OFString_npos;
    }
    if (pos == OFString_npos) {
        pos = this_size;
    }
    for (int i = OFstatic_cast(int, pos - 1); i >= 0; i--) {
        if (str.find(this->at(i)) == OFString_npos)
            return i;
    }
    return OFString_npos;
}

size_t
OFString::find_last_not_of (const char* s, size_t pos, size_t n) const
{
    OFString str(s, n);
    return this->find_last_not_of(str, pos);
}

size_t
OFString::find_last_not_of (const char* s, size_t pos) const
{
    OFString str(s);
    return this->find_last_not_of(str, pos);
}

size_t
OFString::find_last_not_of (char s, size_t pos) const
{
    OFString str(1, s);
    return this->find_last_not_of(str, pos);
}

/*
** Operator <<
*/

STD_NAMESPACE ostream& operator<< (STD_NAMESPACE ostream& o, const OFString& s)
{
    return o.write (s.c_str(), s.size());
}

/*
** Operator >>
*/

STD_NAMESPACE istream& operator>> (STD_NAMESPACE istream& i, OFString& s)
{
    s.resize(0);
    char c = '\0';
    size_t n = s.max_size();
    if (i.width() > 0) {
        n = OFstatic_cast(size_t, i.width());
    }
    // skip white space before word
    i.get(c);
    while (i.good() && isspace(OFstatic_cast(unsigned char, c))) {
        i.get(c);
    }
    // get the word
    while (i.good() && !isspace(OFstatic_cast(unsigned char, c)) && n--) {
        s += c;
        i.get(c);
    }
    if (isspace(OFstatic_cast(unsigned char, c))) {
        i.putback(c);
    }
    i.width(0);
    return i;
}

/*
** Operator +
*/

OFString operator+ (const OFString& lhs, const OFString& rhs)
{
    OFString s(lhs);
    s += rhs;
    return s;
}

OFString operator+ (const char* lhs, const OFString& rhs)
{
    OFString s(lhs);
    s += rhs;
    return s;
}

OFString operator+ (char lhs, const OFString& rhs)
{
    OFString s(1, lhs);
    s += rhs;
    return s;
}

OFString operator+ (const OFString& lhs, const char* rhs)
{
    OFString s(lhs);
    s += rhs;
    return s;
}

OFString operator+ (const OFString& lhs, char rhs)
{
    OFString s(lhs);
    s += rhs;
    return s;
}

/*
** Operator ==
*/

OFBool operator== (const OFString& lhs, const OFString& rhs)
{
    return (lhs.compare(rhs) == 0) ? OFTrue : OFFalse;
}

OFBool operator== (const char* lhs, const OFString& rhs)
{
    OFString slhs(lhs);
    return (slhs == rhs);
}

OFBool operator== (char lhs, const OFString& rhs)
{
    OFString slhs(1, lhs);
    return (slhs == rhs);
}

OFBool operator== (const OFString& lhs, const char* rhs)
{
    OFString srhs(rhs);
    return (lhs == srhs);
}

OFBool operator== (const OFString& lhs, char rhs)
{
    OFString srhs(1, rhs);
    return (lhs == srhs);
}

/*
** Operator <
*/

OFBool operator< (const OFString& lhs, const OFString& rhs)
{
    return (lhs.compare(rhs) < 0) ? OFTrue : OFFalse;
}

OFBool operator< (const char* lhs, const OFString& rhs)
{
    OFString slhs(lhs);
    return (slhs < rhs);
}

OFBool operator< (char lhs, const OFString& rhs)
{
    OFString slhs(1, lhs);
    return (slhs < rhs);
}

OFBool operator< (const OFString& lhs, const char* rhs)
{
    OFString srhs(rhs);
    return (lhs < srhs);
}

OFBool operator< (const OFString& lhs, char rhs)
{
    OFString srhs(1, rhs);
    return (lhs < srhs);
}

/*
** Operator <=
*/

OFBool operator<= (const OFString& lhs, const OFString& rhs)
{
    return (!(rhs < lhs));
}

OFBool operator<= (const char* lhs, const OFString& rhs)
{
    return (!(rhs < lhs));
}

OFBool operator<= (char lhs, const OFString& rhs)
{
    return (!(rhs < lhs));
}

OFBool operator<= (const OFString& lhs, const char* rhs)
{
    return (!(rhs < lhs));
}

OFBool operator<= (const OFString& lhs, char rhs)
{
    return (!(rhs < lhs));
}

/*
** Operator !=
*/

OFBool operator!= (const OFString& lhs, const OFString& rhs)
{
    return (!(lhs == rhs));
}

OFBool operator!= (const char* lhs, const OFString& rhs)
{
    return (!(lhs == rhs));
}

OFBool operator!= (char lhs, const OFString& rhs)
{
    return (!(lhs == rhs));
}

OFBool operator!= (const OFString& lhs, const char* rhs)
{
    return (!(lhs == rhs));
}

OFBool operator!= (const OFString& lhs, char rhs)
{
    return (!(lhs == rhs));
}

/*
** Operator >
*/

OFBool operator> (const OFString& lhs, const OFString& rhs)
{
    return (rhs < lhs);
}

OFBool operator> (const char* lhs, const OFString& rhs)
{
    return (rhs < lhs);
}

OFBool operator> (char lhs, const OFString& rhs)
{
    return (rhs < lhs);
}

OFBool operator> (const OFString& lhs, const char* rhs)
{
    return (rhs < lhs);
}

OFBool operator> (const OFString& lhs, char rhs)
{
    return (rhs < lhs);
}

/*
** Operator >=
*/

OFBool operator>= (const OFString& lhs, const OFString& rhs)
{
    return (!(lhs < rhs));
}

OFBool operator>= (const char* lhs, const OFString& rhs)
{
    return (!(lhs < rhs));
}

OFBool operator>= (char lhs, const OFString& rhs)
{
    return (!(lhs < rhs));
}

OFBool operator>= (const OFString& lhs, const char* rhs)
{
    return (!(lhs < rhs));
}

OFBool operator>= (const OFString& lhs, char rhs)
{
    return (!(lhs < rhs));
}

#else /* HAVE_STD_STRING */

int ofstring_cc_dummy_to_keep_linker_from_moaning = 0;

#endif


/*
** CVS/RCS Log:
** $Log: ofstring.cc,v $
** Revision 1.33  2010-10-20 07:41:37  uli
** Made sure isalpha() & friends are only called with valid arguments.
**
** Revision 1.32  2010-10-14 13:14:53  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.31  2010-10-08 10:52:28  uli
** Make OFString(NULL) construct an empty string. Previously, this was the
** default, but it was changed to match the behavior of std::string.
** This adds a new define USE_NULL_SAFE_OFSTRING which is always defined by
** our Makefiles. Passing NULL to OFString now also generates a warning in
** DEBUG builds.
**
** Revision 1.30  2010-08-19 12:07:55  uli
** Made OFString follow the C++ standard for std::string::assign().
**
** Revision 1.29  2010-06-25 09:43:23  uli
** Don't allow OFString(NULL) because it is also forbidden for std::string.
**
** Revision 1.28  2010-05-03 07:50:18  joergr
** Added type cast to integer variable in order to avoid compiler warnings
** reported by VisualStudio 2010.
**
** Revision 1.27  2010-04-09 09:48:23  uli
** Don't initialize our string twice in OFString::reserve().
**
** Revision 1.26  2010-01-05 14:05:34  uli
** Made sure OFString always null-terminates its C-Strings.
**
** Revision 1.25  2009-09-28 14:18:36  joergr
** Introduced new member variable that stores the current length of the string.
** This yields in a significant performance improvement when compiled in debug
** mode.
** Added support for strings that contain null bytes ('\0') in order to be more
** compliant with the standard C++ string class.
**
** Revision 1.24  2009-09-25 09:46:52  joergr
** Fixed issue in assign() method with overlapping memory areas (e.g. when using
** self-assignment of a string or copying a sub-string to itself).
**
** Revision 1.23  2009-08-07 14:30:32  joergr
** Fixed incorrect implementation of find_first_not_of() and find_last_not_of().
**
** Revision 1.22  2006/08/14 16:42:46  meichel
** Updated all code in module ofstd to correctly compile if the standard
**   namespace has not included into the global one with a "using" directive.
**
** Revision 1.21  2005/12/08 15:49:01  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.20  2003/07/09 13:58:04  meichel
** Adapted type casts to new-style typecast operators defined in ofcast.h
**
** Revision 1.19  2003/07/04 13:31:52  meichel
** Fixed issues with compiling with HAVE_STD_STRING
**
** Revision 1.18  2003/05/14 13:22:29  joergr
** Fixed bug in OFString::find_first_of().
**
** Revision 1.17  2003/04/17 15:55:17  joergr
** Enhanced performance of find routines.
**
** Revision 1.16  2002/11/27 11:23:11  meichel
** Adapted module ofstd to use of new header file ofstdinc.h
**
** Revision 1.15  2001/12/04 16:48:18  meichel
** Completed doc++ documentation, fixed bug in OFString::copy.
**
** Revision 1.14  2001/11/26 16:43:20  joergr
** Fixed bug in OFString constructor.
**
** Revision 1.13  2001/06/01 15:51:39  meichel
** Updated copyright header
**
** Revision 1.12  2000/04/14 15:21:33  meichel
** Minor change to make OFString thread safe.
**
** Revision 1.11  2000/03/08 16:36:06  meichel
** Updated copyright header.
**
** Revision 1.10  2000/02/02 14:30:17  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.9  1999/09/09 14:22:38  thiel
** bug in rfind: patternsize greater than string
**
** Revision 1.8  1998/11/27 12:43:54  joergr
** Added copyright message to source files and changed CVS header.
**
** Revision 1.7  1998/06/29 12:09:26  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.6  1997/10/06 11:31:13  hewett
** Fixed OFString::operator<< handling of leading whitespace.  Leading
** whitespace is now skipped.
**
** Revision 1.5  1997/10/01 11:53:57  hewett
** Fixed segmentation fault for OFString's find_ methods when current
** string is empty.
**
** Revision 1.4  1997/09/11 15:39:18  hewett
** Fixed OFString bug associated with the assign method
** when n == OFString_npos.  Since OFString_npos is represented
** by -1 the assign method  was reserving zero space for the string.
** This case is now explicitly handled.
**
** Revision 1.3  1997/09/01 10:00:20  hewett
** Added absent $ terminator to RCS/CVS Revision keyword in header.
**
** Revision 1.2  1997/07/07 14:05:33  hewett
** Renamed the constant OFnpos to OFString_npos to look more like
** the real ANSI constant string::npos.
**
** Revision 1.1  1997/07/07 11:52:22  meichel
** Added string class OFString to ofstd library.
** This class implements a subset of the ANSI C++ "string" class.
**
**
*/

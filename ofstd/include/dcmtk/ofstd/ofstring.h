/*
 *
 *  Copyright (C) 1997-2012, OFFIS e.V.
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
 */

#ifndef OFSTRING_H
#define OFSTRING_H

#include "dcmtk/config/osconfig.h"     /* include OS specific configuration first */

#include "dcmtk/ofstd/oftypes.h"       /* for OFBool */
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofdefine.h"


// makes sure that resulting C string is never NULL
#define OFSTRING_GUARD(c_string) ((c_string != NULL) ? (c_string) : "")


#ifdef HAVE_STD_STRING
/*
** Use the ANSI Standard string class
*/

#include <string>

#define OFString std::string
#define OFString_npos std::string::npos

#else /* not HAVE_STD_STRING */

/*
** Declare our own string class
*/

#define INCLUDE_CASSERT
#define INCLUDE_CSTRING
#define INCLUDE_CSTDLIB
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/oftypes.h"

/*
** Error macros
*/
#define OFSTRING_OUTOFRANGE(cond) assert (!(cond))
#define OFSTRING_LENGTHERROR(cond) assert (!(cond))
#define OFSTRING_MEMORYALLOCERROR(cond) assert (!(cond))

/** OFString_npos is a value larger than any "reasonable" string and is
 *  used to denote "until the end" when a length is required.
 *  Normally string::npos is defined as a static const member
 *  but some C++ compilers are too primitive.
 */
static const size_t OFString_npos = (OFstatic_cast(size_t, -1));


/** a simple string class that implements a subset of std::string.
 *  It does not implement iterators or traits and is not optimized for speed.
 */
class DCMTK_OFSTD_EXPORT OFString
{
public:
    /*
     * The SunOS C++ 2.0.1 does not allow static const members.
     * We would like to define:
     *    static const size_t npos = ((size_t)-1);
     * but cannot so an alternative OFString_npos is defined outside
     * the class (see above).
     */

    /** Default constructor. Constructs an empty string.
     */
    OFString();

    /** Constructs a string from the given input string str. The effective
     *  length rlen of the constructed string is the smaller of n and
     *  str.size() - pos, and the string is constructed by copying rlen
     *  characters starting at position pos of the input string str. The
     *  function throws an out-of-range error if pos > str.size().
     *  @param str string to copy from
     *  @param pos position to start copying from
     *  @param n maximum number of characters to copy
     */
    OFString(const OFString& str, size_t pos = 0, size_t n = OFString_npos);

    /** This constructor copies n characters starting at s, and constructs a
     *  string object initialized with the corresponding characters.
     *  NOTE: If n > length(s), then junk characters are appended
     *  to the end of the string. i.e. n characters are copied regardless of
     *  the exact length of the array pointed to by the input pointer s.
     *  @param s pointer to an array of char of length n. Must not be NULL.
     *  @param n number of characters in array
     */
    OFString(const char* s, size_t n);

    /** Constructs a string object from the array pointed to by the input
     *  pointer s. It is assumed that s is not a null pointer.
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     */
    OFString(const char* s);

    /** Constructs a string object with the character c repeated rep times.
     *  Reports a length error if rep equals npos.
     *  @param rep number of repetitions
     *  @param c character to construct from
     */
    OFString(size_t rep, char c);

    /** destructor
     */
    ~OFString();

    /** assigns the input string to the current string.
     *  @param rhs string to copy from
     *  @return reference to this object
     */
    OFString& operator=(const OFString& rhs);

    /** constructs a temporary string from the input s and assigns it to the current string.
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     *  @return reference to this object
     */
    OFString& operator=(const char* s);

    /** constructs a temporary string from the input s and assigns it to the current string.
     *  @param s character
     *  @return reference to this object
     */
    OFString& operator=(char s);

    /** Appends the input string to the current string.
     *  @param rhs string to append from
     *  @return reference to this object
     */
    OFString& operator+=(const OFString& rhs);

    /** constructs a temporary string from the input s and appends it to the current string.
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     *  @return reference to this object
     */
    OFString& operator+=(const char* s);

    /** constructs a temporary string from the input s and appends it to the current string.
     *  @param s character
     *  @return reference to this object
     */
    OFString& operator+=(char s);

    /** Appends characters from the input string str to the current string
     *  object. At most n characters, starting at position pos of str, are
     *  appended.
     *  The function reports an out-of-range error if pos > str.size().
     *  @param str string to append from
     *  @param pos position to start copying from
     *  @param n maximum number of characters to copy
     *  @return reference to this object
     */
    OFString& append(const OFString& str, size_t pos = 0, size_t n = OFString_npos);

    /** constructs a temporary string from the input and appends it to the current string.
     *  @param s pointer to an array of char of length n. Must not be NULL.
     *  @param n number of characters in array
     *  @return reference to this object
     */
    OFString& append(const char* s, size_t n);

    /** constructs a temporary string from the input and appends it to the current string.
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     *  @return reference to this object
     */
    OFString& append(const char* s);

    /** constructs a temporary string from the input and appends it to the current string.
     *  @param rep number of repetitions
     *  @param c character to construct from
     *  @return reference to this object
     */
    OFString& append(size_t rep, char c);

    /** Assigns characters from the input string str to the current string
     *  object. At most n characters, starting at position pos of str, are
     *  appended.
     *  The function reports an out-of-range error if pos > str.size().
     *  @param str string to append from
     *  @param pos position to start copying from
     *  @param n maximum number of characters to copy, can be OFString_npos
     *  @return reference to this object
     */
    OFString& assign(const OFString& str, size_t pos, size_t n);

    /** Assigns input string str to the current string object.
     *  @param str string to copy
     *  @return reference to this object
     */
    OFString& assign(const OFString& str);

    /** constructs a temporary string from the input and assigns it to the current string.
     *  @param s pointer to an array of char of length n. Must not be NULL.
     *  @param n number of characters in array
     *  @return reference to this object
     */
    OFString& assign(const char* s, size_t n);

    /** constructs a temporary string from the input and assigns it to the current string.
     *  @param s pointer to an array of char. Must not be NULL.
     *  @param e pointer to the first element of the array that should not be included.
     *  @return reference to this object
     */
    OFString& assign(const char* s, const char* e);

    /** constructs a temporary string from the input and assigns it to the current string.
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     *  @return reference to this object
     */
    OFString& assign(const char* s);

    /** constructs a temporary string from the input and assigns it to the current string.
     *  @param rep number of repetitions
     *  @param c character to construct from
     *  @return reference to this object
     */
    OFString& assign(size_t rep, char c);

    /** Inserts at most n characters, starting at position pos2 of the input
     *  string str, into the current string. The characters are inserted
     *  starting at position pos1 in the current string.
     *  The function reports an out-of-range error if pos > str.size().
     *  @param pos1 position to insert at
     *  @param str string to copy from
     *  @param pos2 position to start copying from
     *  @param n maximum number of characters to copy
     *  @return reference to this object
     */
    OFString& insert(size_t pos1, const OFString& str,
                     size_t pos2 = 0, size_t n = OFString_npos);

    /** constructs a temporary string from the input and inserts
     *  it into the current string.
     *  @param pos position to insert at
     *  @param s pointer to an array of char of length n. Must not be NULL.
     *  @param n number of characters in array
     *  @return reference to this object
     */
    OFString& insert(size_t pos, const char* s, size_t n);

    /** constructs a temporary string from the input and inserts
     *  it into the current string.
     *  @param pos position to insert at
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     *  @return reference to this object
     */
    OFString& insert(size_t pos, const char* s);

    /** constructs a temporary string from the input and inserts
     *  it into the current string.
     *  @param pos position to insert at
     *  @param rep number of repetitions
     *  @param c character to construct from
     *  @return reference to this object
     */
    OFString& insert(size_t pos, size_t rep, char c);

    /** Removes up to n characters from the string starting from position pos.
     *  @param pos position to start from
     *  @param n number of characters to remove
     *  @return reference to this object
     */
    OFString& erase(size_t pos = 0, size_t n = OFString_npos);

    /** replaces a range of characters in the current string
     *  with a range of characters taken from the input string str. The
     *  range to be replaced starts at position pos1 in the current string,
     *  and extends for n1 characters, or up to the end of the string,
     *  whichever comes first.
     *  The range of characters inserted starts at position pos2 of the
     *  input string str, and extends for n2 characters, or up to the end of
     *  the string str, whichever comes first.
     *  @param pos1 position to insert at
     *  @param n1 number of characters to replace
     *  @param str string to copy from
     *  @param pos2 position to start copying from
     *  @param n2 maximum number of characters to copy
     *  @return reference to this object
     */
    OFString& replace(size_t pos1, size_t n1, const OFString& str,
                      size_t pos2 = 0, size_t n2 = OFString_npos);

    /** constructs a temporary string from the input and replaces the range [pos, n]
     *  in the current string with the constructed string.
     *  @param pos position to replace at
     *  @param n number of characters to be replaced
     *  @param s pointer to an array of char of length n. Must not be NULL.
     *  @param n2 number of characters in array
     *  @return reference to this object
     */
    OFString& replace(size_t pos, size_t n, const char* s, size_t n2);

    /** constructs a temporary string from the input and replaces the range [pos, n]
     *  in the current string with the constructed string.
     *  @param pos position to replace at
     *  @param n number of characters to be replaced
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     *  @return reference to this object
     */
    OFString& replace(size_t pos, size_t n, const char* s);

    /** constructs a temporary string from the input and replaces the range [pos, n]
     *  in the current string with the constructed string.
     *  @param pos position to replace at
     *  @param n number of characters to be replaced
     *  @param rep number of repetitions
     *  @param s character to construct from
     *  @return reference to this object
     */
    OFString& replace(size_t pos, size_t n, size_t rep, char s);

    /** returns a constant reference to the character at
     *  position pos of the current string.
     *  if pos >= size(), throws out_of_range exception.
     *  @param pos position in string
     *  @return const reference to character in string at pos
     */
    const char& at(size_t pos) const
    {
        OFSTRING_OUTOFRANGE (pos >= this->size());
        return this->theCString[pos];
    }

    /** returns a non-const reference to the character at
     *  position pos of the current string.
     *  if pos >= size(), throws out_of_range exception.
     *  @param pos position in string
     *  @return non-const reference to character in string at pos
     */
    char& at(size_t pos)
    {
        OFSTRING_OUTOFRANGE (pos >= this->size());
        return this->theCString[pos];
    }

    /** returns the element at position pos of the current string. Returns
     *  '@\0' if pos == size().
     *  @param pos position in string
     *  @return character in string at pos (by value)
     */
    char operator[] (size_t pos) const
    {
        if (pos == this->size()) return '\0';
        else
        {
          OFSTRING_OUTOFRANGE (pos > this->size());
          return this->theCString[pos];
        }
    }

    /** returns the element at position pos of the current string.
     *  The reference returned is invalid after a subsequent call to any
     *  non-const member function for the object.
     *  @param pos position in string, must be < size().
     *  @return character in string at pos (by reference)
     */
    char& operator[] (size_t pos)
    {
        OFSTRING_OUTOFRANGE (pos >= this->size());
        return this->theCString[pos];
    }

    /** returns a pointer to the initial element of an array of length
     *  size()+1 whose first size() elements equal the corresponding elements
     *  of the current string and whose last element is a null character.
     *  @return C string for this string
     */
    const char* c_str() const
    {
        return (this->theCString)?(this->theCString):("");
    }

    /** if size() is nonzero, this function returns a pointer to the initial
     *  element of an array whose first size() elements equal the
     *  corresponding elements of the string controlled by *this.
     *  If size() is zero, the member returns a non-null pointer that is
     *  copyable and can have zero added to it.
     *  @return pointer to char array for this string
     */
    const char* data() const;

    /** returns a count of the number of char-like objects currently in
     *  the string.
     *  @return number of char-like objects currently in string
     */
    size_t size() const
    {
        return this->theSize;
    }

    /** returns a count of the number of char-like objects currently in
     *  the string.
     *  @return number of char-like objects currently in string
     */
    size_t length() const
    {
        return this->size();
    }

    /** return true if the string is empty, false otherwise.
     *  @return true if the string is empty, false otherwise.
     */
    OFBool empty() const
    {
        return (this->size() == 0)?(OFTrue):(OFFalse);
    }

    /** if n <= size(), truncates the string to length n else it pads the
     *  extra locations with c. Reports a length error if n equals OFString_npos.
     *  @param n length to truncate string to
     *  @param c character to pad extra locations with
     */
    void resize(size_t n, char c = '\0');

    /** returns the size of the allocated storage in the string.
     *  @return size of the allocated storage in the string
     */
    size_t capacity() const
    {
        return this->theCapacity;
    }

    /** returns the maximum size of a string which could possibly by allocated.
     *  @return maximum size of a string which could possibly by allocated.
     */
    size_t max_size() const
    {
        return ((OFString_npos - 1)/sizeof(char));
    }

    /** empty the string of all contents
     */
    void clear()
    {
        this->erase();
    }

    /** directive that informs a string of a planned
     *  change in size, so that it can manage the storage allocation
     *  accordingly.  Reallocation of a string happens if and only if
     *  the current capacity is less than res_arg. After this call,
     *  capacity() is greater than or equal to res_arg if reallocation
     *  happens and equal to the previous value of capacity() otherwise.
     *  @param res_arg string capacity to reserve
     */
    void reserve(size_t res_arg);

    /** replaces the string designated by s with a copy of a
     *  range of characters from the current string. The range copied begins
     *  at position pos of the current string and extends for n characters or
     *  up to the end of the current string, whichever comes first.
     *  Does not append a null object to the end of the string designated by s.
     *  @param s character array to copy to
     *  @param n size of character array
     *  @param pos position in string to start copying from
     *  @return number of characters copied
     */
    size_t copy(char* s, size_t n, size_t pos = 0) const;

    /** returns a copy the substring consisting of at most n
     *  characters starting at position pos of the current string.
     *  @param pos position in string to start copying from
     *  @param n number of characters to copy
     */
    OFString substr(size_t pos = 0, size_t n = OFString_npos) const;

    /** swaps the contents of the two strings. The time complexity of this
     *  function is constant.
     *  @param s string to swap with
     */
    void swap(OFString& s);

    /** determines the effective length rlen of the strings to compare as
     *  the smallest of size() and str.size(). The function then compares
     *  the two strings by calling strcmp(data(), str.data(), rlen).
     *  Returns: the nonzero result if the result of the comparison is
     *  nonzero. Otherwise, returns a value < 0 if size() < str.size(),
     *  a value of 0 if size() == str.size(), or a value > 0 if
     *  size() > str.size().
     *  @param str string to compare to
     *  @return comparison result as described above
     */
    int compare(const OFString& str) const;

    /** constructs a temporary string from this object and compares
     *  it with the input string
     *  @param pos1 position to start copying from this object
     *  @param n1 maximum number of characters to copy from this object
     *  @param str string to compare to
     *  @return comparison result
     */
    int compare(size_t pos1, size_t n1, const OFString& str) const;

    /** constructs a temporary string from this object and another
     *  temporary from the input and compares the two temporaries
     *  @param pos1 position to start copying from this object
     *  @param n1 maximum number of characters to copy from this object
     *  @param str string to create second temporary from
     *  @param pos2 position to start copying from in str
     *  @param n2 maximum number of characters to copy from str
     *  @return comparison result
     */
    int compare(size_t pos1, size_t n1, const OFString& str, size_t pos2, size_t n2) const;

    /** constructs a temporary string from the input and compares
     *  it with the current string
     *  @param s pointer to a zero-terminated C string. Must not be NULL.
     *  @return comparison result
     */
    int compare(const char* s) const;

    /** constructs a temporary string from this object and another
     *  temporary from the input and compares the two temporaries
     *  @param pos1 position to start copying from this object
     *  @param n1 maximum number of characters to copy from this object
     *  @param s pointer to an array of char of length n. Must not be NULL.
     *  @param n2 number of characters in array s
     *  @return comparison result
     */
    int compare(size_t pos1, size_t n1, const char* s, size_t n2 = OFString_npos) const;

    /** determines the earliest occurrence of the
     *  input pattern in the current string object, starting from position
     *  pos in the current string. If find can determine such an occurrence,
     *  it returns the starting index of pattern in the current string.
     *  Otherwise, it returns string::npos.
     *  @param pattern pattern to find
     *  @param pos position to start searching from
     *  @return index of pattern as described above
     */
    size_t find(const OFString& pattern, size_t pos = 0) const;

    /** creates a pattern string from the input and determines the earliest
     *  occurrence of the pattern in the current string object, starting from position
     *  pos in the current string. If find can determine such an occurrence,
     *  it returns the starting index of pattern in the current string.
     *  Otherwise, it returns string::npos.
     *  @param pattern pointer to an array of char of length n. Must not be NULL.
     *  @param pos position to start searching from
     *  @param n number of characters in array
     *  @return index of pattern in string
     */
    size_t find(const char* pattern, size_t pos, size_t n) const;

    /** creates a pattern string from the input and determines the earliest
     *  occurrence of the pattern in the current string object, starting from position
     *  pos in the current string. If find can determine such an occurrence,
     *  it returns the starting index of pattern in the current string.
     *  Otherwise, it returns string::npos.
     *  @param pattern pointer to a zero-terminated C string. Must not be NULL.
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t find(const char* pattern, size_t pos = 0) const;

    /** creates a pattern string from the input and determines the earliest
     *  occurrence of the pattern in the current string object, starting from position
     *  pos in the current string. If find can determine such an occurrence,
     *  it returns the starting index of pattern in the current string.
     *  Otherwise, it returns string::npos.
     *  @param pattern character
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t find(char pattern, size_t pos = 0) const;

    /** scans the current string backwards, and finds the first
     *  occurrence of pattern in the string (from the back). The starting
     *  index of the matched position in the current string should be less
     *  than or equal to the parameter pos. If a match is found, the starting
     *  index is returned; otherwise, the function returns string::npos.
     *  @param pattern pattern to find
     *  @param pos position to start searching from
     *  @return index of pattern as described above
     */
    size_t rfind(const OFString& pattern, size_t pos = OFString_npos) const;

    /** scans the current string backwards, and finds the first
     *  occurrence of pattern in the string (from the back). The starting
     *  index of the matched position in the current string should be less
     *  than or equal to the parameter pos. If a match is found, the starting
     *  index is returned; otherwise, the function returns string::npos.
     *  @param pattern pointer to an array of char of length n. Must not be NULL.
     *  @param pos position to start searching from
     *  @param n number of characters in array
     *  @return index of pattern in string
     */
    size_t rfind(const char* pattern, size_t pos, size_t n) const;

    /** scans the current string backwards, and finds the first
     *  occurrence of pattern in the string (from the back). The starting
     *  index of the matched position in the current string should be less
     *  than or equal to the parameter pos. If a match is found, the starting
     *  index is returned; otherwise, the function returns string::npos.
     *  @param pattern pointer to a zero-terminated C string. Must not be NULL.
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t rfind(const char* pattern, size_t pos = OFString_npos) const;

    /** scans the current string backwards, and finds the first
     *  occurrence of pattern in the string (from the back). The starting
     *  index of the matched position in the current string should be less
     *  than or equal to the parameter pos. If a match is found, the starting
     *  index is returned; otherwise, the function returns string::npos.
     *  @param pattern character
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t rfind(char pattern, size_t pos = OFString_npos) const;

    /** determines the first location, loc, between pos and
     *  the end of the current string, such that the character at loc matches
     *  at least one character from the set of characters. If such a
     *  location can be determined, it is returned. Otherwise, the function
     *  returns string::npos.
     *  @param str set of characters to find
     *  @param pos position to start searching from
     *  @return index of character as described above
     */
    size_t find_first_of(const OFString& str, size_t pos = 0) const;

    /** determines the first location, loc, between pos and
     *  the end of the current string, such that the character at loc matches
     *  at least one character from the set of characters. If such a
     *  location can be determined, it is returned. Otherwise, the function
     *  returns string::npos.
     *  @param s set of characters to find, pointer to an array of char of length n. Must not be NULL.
     *  @param pos position to start searching from
     *  @param n number of characters in array
     *  @return index of pattern in string
     */
    size_t find_first_of(const char* s, size_t pos, size_t n) const;

    /** determines the first location, loc, between pos and
     *  the end of the current string, such that the character at loc matches
     *  at least one character from the set of characters. If such a
     *  location can be determined, it is returned. Otherwise, the function
     *  returns string::npos.
     *  @param s set of characters to find, pointer to a zero-terminated C string. Must not be NULL.
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t find_first_of(const char* s, size_t pos = 0) const;

    /** determines the first location, loc, between pos and
     *  the end of the current string, such that the character at loc matches
     *  the given character. If such a
     *  location can be determined, it is returned. Otherwise, the function
     *  returns string::npos.
     *  @param s character to find
     *  @param pos position to start searching from
     *  @return index of character
     */
    size_t find_first_of(char s, size_t pos = 0) const;

    /** determines the highest location, loc, up to pos, such
     *  that the character at loc matches at least one character from the
     *  set of characters. If such a location can be determined, it is
     *  returned. Otherwise, the function returns string::npos.
     *  @param str set of characters to find
     *  @param pos position to start searching from
     *  @return index of character as described above
     */
    size_t find_last_of(const OFString& str, size_t pos = OFString_npos) const;

    /** determines the highest location, loc, up to pos, such
     *  that the character at loc matches at least one character from the
     *  set of characters. If such a location can be determined, it is
     *  returned. Otherwise, the function returns string::npos.
     *  @param s set of characters to find, pointer to an array of char of length n. Must not be NULL.
     *  @param pos position to start searching from
     *  @param n number of characters in array
     *  @return index of pattern in string
     */
    size_t find_last_of(const char* s, size_t pos, size_t n) const;

    /** determines the highest location, loc, up to pos, such
     *  that the character at loc matches at least one character from the
     *  set of characters. If such a location can be determined, it is
     *  returned. Otherwise, the function returns string::npos.
     *  @param s set of characters to find, pointer to a zero-terminated C string. Must not be NULL.
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t find_last_of(const char* s, size_t pos = OFString_npos) const;

    /** determines the highest location, loc, up to pos, such
     *  that the character at loc matches the given character.
     *  If such a location can be determined, it is
     *  returned. Otherwise, the function returns string::npos.
     *  @param s character to find
     *  @param pos position to start searching from
     *  @return index of character
     */
    size_t find_last_of(char s, size_t pos = OFString_npos) const;

    /** determines the first location loc, between pos and
     *  the end of the current string, such that the character at loc does not
     *  match any character from the set of characters. If such a location
     *  is found, it is returned. Otherwise, the function returns string::npos.
     *  @param str set of characters to find
     *  @param pos position to start searching from
     *  @return index of character as described above
     */
    size_t find_first_not_of(const OFString& str, size_t pos = 0) const;

    /** determines the first location loc, between pos and
     *  the end of the current string, such that the character at loc does not
     *  match any character from the set of characters. If such a location
     *  is found, it is returned. Otherwise, the function returns string::npos.
     *  @param s set of characters to find, pointer to an array of char of length n. Must not be NULL.
     *  @param pos position to start searching from
     *  @param n number of characters in array
     *  @return index of pattern in string
     */
    size_t find_first_not_of(const char* s, size_t pos, size_t n) const;

    /** determines the first location loc, between pos and
     *  the end of the current string, such that the character at loc does not
     *  match any character from the set of characters. If such a location
     *  is found, it is returned. Otherwise, the function returns string::npos.
     *  @param s set of characters to find, pointer to a zero-terminated C string. Must not be NULL.
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t find_first_not_of(const char* s, size_t pos = 0) const;

    /** determines the first location loc, between pos and
     *  the end of the current string, such that the character at loc does not
     *  match the given character. If such a location
     *  is found, it is returned. Otherwise, the function returns string::npos.
     *  @param c character to find
     *  @param pos position to start searching from
     *  @return index of character
     */
    size_t find_first_not_of(char c, size_t pos = 0) const;

    /** scans the current string up to the position pos and
     *  determines the highest location, loc, such that the character at loc
     *  does not match any character from the set of characters. If such
     *  a location is found, it is returned. Otherwise, the function returns
     *  string::npos.
     *  @param str set of characters to find
     *  @param pos position to start searching from
     *  @return index of character as described above
     */
    size_t find_last_not_of(const OFString& str, size_t pos = OFString_npos) const;

    /** scans the current string up to the position pos and
     *  determines the highest location, loc, such that the character at loc
     *  does not match any character from the set of characters. If such
     *  a location is found, it is returned. Otherwise, the function returns
     *  string::npos.
     *  @param s set of characters to find, pointer to an array of char of length n. Must not be NULL.
     *  @param pos position to start searching from
     *  @param n number of characters in array
     *  @return index of pattern in string
     */
    size_t find_last_not_of(const char* s, size_t pos, size_t n) const;

    /** scans the current string up to the position pos and
     *  determines the highest location, loc, such that the character at loc
     *  does not match any character from the set of characters. If such
     *  a location is found, it is returned. Otherwise, the function returns
     *  string::npos.
     *  @param s set of characters to find, pointer to a zero-terminated C string. Must not be NULL.
     *  @param pos position to start searching from
     *  @return index of pattern in string
     */
    size_t find_last_not_of(const char* s, size_t pos = OFString_npos) const;

    /** scans the current string up to the position pos and
     *  determines the highest location, loc, such that the character at loc
     *  does not match the given character. If such
     *  a location is found, it is returned. Otherwise, the function returns
     *  string::npos.
     *  @param c character to find
     *  @param pos position to start searching from
     *  @return index of character
     */
    size_t find_last_not_of(char c, size_t pos = OFString_npos) const;

    /** type that is used for lengths and offsets */
    typedef size_t size_type;

    /** type that is contained in this */
    typedef char value_type;

    /** this typedef can be used to iterate over an string.
     *  Note: Normally you are allowed to modify items through an iterator,
     *  we do not allow this!
     */
    typedef const char* iterator;

    /** this is just an alias for iterator since iterator is already "const" */
    typedef iterator const_iterator;

    /** returns a constant iterator that points to the beginning of the string
     *  @return iterator to the beginning of the string
     */
    iterator begin() const { return theCString; }

    /** returns a constant iterator that points after the last element of the string
     *  @return iterator after the last element of the string
     */
    iterator end() const { return begin() + length(); }

private:
    /// the "C" string pointer
    char* theCString;

    /// the length of theCString
    size_t theSize;

    /// the capacity of str
    size_t theCapacity;

};


/** puts the string s on the output stream o.
 *  @param o output stream
 *  @param s string to print
 *  @return reference to stream
 */
DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream& operator<< (STD_NAMESPACE ostream& o, const OFString& s);

/** reads a string of characters from input stream i into s. Any
 *  whitespace is treated as a string terminator.
 *  @param i input stream
 *  @param s string to print
 *  @return reference to stream
 */
DCMTK_OFSTD_EXPORT STD_NAMESPACE istream& operator>> (STD_NAMESPACE istream& i, OFString& s);

/** appends the string rhs to lhs.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string
 *  @return concatenated string, by value
 */
DCMTK_OFSTD_EXPORT OFString operator+ (const OFString& lhs, const OFString& rhs);

/** appends the string rhs to lhs.
 *  @param lhs left-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @param rhs right-hand side string
 *  @return concatenated string, by value
 */
DCMTK_OFSTD_EXPORT OFString operator+ (const char* lhs, const OFString& rhs);

/** appends the string rhs to lhs.
 *  @param lhs left-hand side string, single character
 *  @param rhs right-hand side string
 *  @return concatenated string, by value
 */
DCMTK_OFSTD_EXPORT OFString operator+ (char lhs, const OFString& rhs);

/** appends the string rhs to lhs.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @return concatenated string, by value
 */
DCMTK_OFSTD_EXPORT OFString operator+ (const OFString& lhs, const char* rhs);

/** appends the string rhs to lhs.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, single character
 *  @return concatenated string, by value
 */
DCMTK_OFSTD_EXPORT OFString operator+ (const OFString& lhs, char rhs);

/** returns true if lhs.compare(rhs) is zero, otherwise false.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string
 *  @return true if strings are equal, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator== (const OFString& lhs, const OFString& rhs);

/** returns true if lhs.compare(rhs) is zero, otherwise false.
 *  @param lhs left-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @param rhs right-hand side string
 *  @return true if strings are equal, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator== (const char* lhs, const OFString& rhs);

/** returns true if lhs.compare(rhs) is zero, otherwise false.
 *  @param lhs left-hand side string, single character
 *  @param rhs right-hand side string
 *  @return true if strings are equal, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator== (char lhs, const OFString& rhs);

/** returns true if lhs.compare(rhs) is zero, otherwise false.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @return true if strings are equal, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator== (const OFString& lhs, const char* rhs);

/** returns true if lhs.compare(rhs) is zero, otherwise false.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, single character
 *  @return true if strings are equal, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator== (const OFString& lhs, char rhs);

/** returns true if lhs.compare(rhs) < 0, otherwise false.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string
 *  @return true if lhs < rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator< (const OFString& lhs, const OFString& rhs);

/** returns true if lhs.compare(rhs) < 0, otherwise false.
 *  @param lhs left-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @param rhs right-hand side string
 *  @return true if lhs < rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator< (const char* lhs, const OFString& rhs);

/** returns true if lhs.compare(rhs) < 0, otherwise false.
 *  @param lhs left-hand side string, single character
 *  @param rhs right-hand side string
 *  @return true if lhs < rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator< (char lhs, const OFString& rhs);

/** returns true if lhs.compare(rhs) < 0, otherwise false.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @return true if lhs < rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator< (const OFString& lhs, const char* rhs);

/** returns true if lhs.compare(rhs) < 0, otherwise false.
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, single character
 *  @return true if lhs < rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator< (const OFString& lhs, char rhs);

/** returns !(rhs < lhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string
 *  @return true if lhs <= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator<= (const OFString& lhs, const OFString& rhs);

/** returns !(rhs < lhs)
 *  @param lhs left-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @param rhs right-hand side string
 *  @return true if lhs <= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator<= (const char* lhs, const OFString& rhs);

/** returns !(rhs < lhs)
 *  @param lhs left-hand side string, single character
 *  @param rhs right-hand side string
 *  @return true if lhs <= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator<= (char lhs, const OFString& rhs);

/** returns !(rhs < lhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @return true if lhs <= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator<= (const OFString& lhs, const char* rhs);

/** returns !(rhs < lhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, single character
 *  @return true if lhs <= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator<= (const OFString& lhs, char rhs);

/** returns !(lhs == rhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string
 *  @return true if lhs != rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator!= (const OFString& lhs, const OFString& rhs);

/** returns !(lhs == rhs)
 *  @param lhs left-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @param rhs right-hand side string
 *  @return true if lhs != rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator!= (const char* lhs, const OFString& rhs);

/** returns !(lhs == rhs)
 *  @param lhs left-hand side string, single character
 *  @param rhs right-hand side string
 *  @return true if lhs != rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator!= (char lhs, const OFString& rhs);

/** returns !(lhs == rhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @return true if lhs != rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator!= (const OFString& lhs, const char* rhs);

/** returns !(lhs == rhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, single character
 *  @return true if lhs != rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator!= (const OFString& lhs, char rhs);

/** returns (rhs < lhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string
 *  @return true if lhs > rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator> (const OFString& lhs, const OFString& rhs);

/** returns (rhs < lhs)
 *  @param lhs left-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @param rhs right-hand side string
 *  @return true if lhs > rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator> (const char* lhs, const OFString& rhs);

/** returns (rhs < lhs)
 *  @param lhs left-hand side string, single character
 *  @param rhs right-hand side string
 *  @return true if lhs > rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator> (char lhs, const OFString& rhs);

/** returns (rhs < lhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @return true if lhs > rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator> (const OFString& lhs, const char* rhs);

/** returns (rhs < lhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, single character
 *  @return true if lhs > rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator> (const OFString& lhs, char rhs);

/** returns !(lhs < rhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string
 *  @return true if lhs >= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator>= (const OFString& lhs, const OFString& rhs);

/** returns !(lhs < rhs)
 *  @param lhs left-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @param rhs right-hand side string
 *  @return true if lhs >= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator>= (const char* lhs, const OFString& rhs);

/** returns !(lhs < rhs)
 *  @param lhs left-hand side string, single character
 *  @param rhs right-hand side string
 *  @return true if lhs >= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator>= (char lhs, const OFString& rhs);

/** returns !(lhs < rhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, pointer to a zero-terminated C string. Must not be NULL.
 *  @return true if lhs >= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator>= (const OFString& lhs, const char* rhs);

/** returns !(lhs < rhs)
 *  @param lhs left-hand side string
 *  @param rhs right-hand side string, single character
 *  @return true if lhs >= rhs, false otherwise
 */
DCMTK_OFSTD_EXPORT OFBool operator>= (const OFString& lhs, char rhs);

#endif /* HAVE_STD_STRING */

#endif /* OFSTRING_H */

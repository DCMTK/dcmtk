/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Andrew Hewett
 *				
 *  Purpose: A simple string class
 * 
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 12:42:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstring.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFSTRING_H
#define OFSTRING_H

/*
** A simple string class
** - for OFFIS projects when an ANSI string class is not always available
** - based on the ANSI-C++ specifications (????)
** - does not implement iterators or traits
** - this impementation is intended to be slow but reliable
** - it is known to be slow but is it reliable?
*/

#include "osconfig.h"     /* include OS specific configuration first */


#ifdef HAVE_STD_STRING
/*
** Use the ANSI Standard string class
*/

#include <string>

#define OFString string
#define OFString_npos string::npos

#else /* not HAVE_STD_STRING */

/*
** Declare our own string class
*/

BEGIN_EXTERN_C
#ifdef HAVE_STDLIB_H
#include <stdlib.h> /* for NULL */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h> /* for NULL */
#endif
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
END_EXTERN_C

#include <iostream.h>

#include "oftypes.h"

/*
** Error macros
*/
#define OFSTRING_OUTOFRANGE(cond) assert (!(cond))
#define OFSTRING_LENGTHERROR(cond) assert (!(cond))
#define OFSTRING_MEMORYALLOCERROR(cond) assert (!(cond))

/*
** Normally string::npos is defined as a static const member
** but some C++ compilers are too primitive.  
** npos is a value larger than any "reasonable" string and is
** used to denote "until the end" when a length is required.
*/
static const size_t OFString_npos = ((size_t)-1);

/*
** The string class itself
*/

class OFString {
private:
    char* theCString;        /* the "C" string pointer */
    size_t theCapacity;      /* the capacity of str */

public:
    /*
    ** The SunOS C++ 2.0.1 does not allow static const members.
    ** We would like to define:
    **    static const size_t npos = ((size_t)-1);
    ** but cannot so an alternative OFString_npos is defined outside 
    ** the class (see above).
    */

    /*
    ** Default constructor. Constructs an empty string.
    */
    OFString();

    /* 
    ** Constructs a string from the given input string str. The effective
    ** length rlen of the constructed string is the smaller of n and
    ** str.size() - pos, and the string is constructed by copying rlen
    ** characters starting at position pos of the input string str. The 
    ** function throws an out-of-range error if pos > str.size().
    */
    OFString(const OFString& str, size_t pos = 0, size_t n = OFString_npos);

    /*
    ** The input pointer s is assumed to point to an array of char of
    ** length n. It is assumed that s is not a null pointer.
    ** This constructor copies n characters starting at s, and constructs a
    ** string object initialized with the corresponding characters.
    ** NOTE: If n > length(s), then junk characters are appended
    ** to the end of the string. i.e. n characters are copied regardless of
    ** the exact length of the array pointed to by the input pointer s.
    */
    OFString(const char* s, size_t n);

    /*
    ** Constructs a string object from the array pointed to by the input
    ** pointer s. It is assumed that s is not a null pointer.
    */
    OFString (const char* s);

    /*
    ** Constructs a string object with the character c repeated rep times.
    ** Reports a length error if rep equals npos.
    */
    OFString (size_t rep, char c);

    /*
    ** Deallocates the memory referenced by the string object.
    */
    ~OFString();

    /*
    ** Assigns the input string rhs to the current string.
    ** Both the overloaded members assign the string constructed from
    ** the input s to the current string.
    */
    OFString& operator= (const OFString& rhs);
    OFString& operator= (const char* s);
    OFString& operator= (char s);

    /*
    ** Appends the string rhs to the current string.
    ** Both of the overloaded functions construct a string object from the
    ** input s, and append it to the current string.
    */
    OFString& operator+= (const OFString& rhs);
    OFString& operator+= (const char* s);
    OFString& operator+= (char s);


    /*
    ** Appends characters from the input string str to the current string
    ** object. At most n characters, starting at position pos of str, are 
    ** appended.
    ** The function reports an out-of-range error if pos > str.size().
    ** All of the overloaded append functions  construct a string object from 
    ** the input s and append it to the current string.
    */
    OFString& append (const OFString& str, size_t pos = 0, size_t n = OFString_npos);
    OFString& append (const char* s, size_t n);
    OFString& append (const char* s);
    OFString& append (size_t rep, char c);

    /*
    ** Assigns characters from the input string str to the current string
    ** object. At most n characters, starting at position pos of str are 
    ** assigned to the current string. The function reports an out-of-range
    ** error if pos > str.size().
    ** All of the overloaded assign functions construct a string object
    ** from the input s and assign it to the current string.
    */
    OFString& assign (const OFString& str, size_t pos = 0, size_t n = OFString_npos);
    OFString& assign (const char* s, size_t n);
    OFString& assign (const char* s);
    OFString& assign (size_t rep, char c);

    /*
    ** Inserts at most n characters, starting at position pos2 of the input
    ** string str, into the current string. The characters are inserted 
    ** starting at position pos1 in the current string.
    ** All of the overloaded insert functions construct a string object from
    ** the input s and insert it into the current string.
    */
    OFString& insert (size_t pos1, const OFString& str, 
		      size_t pos2 = 0, size_t n = OFString_npos);
    OFString& insert (size_t pos, const char* s, size_t n);
    OFString& insert (size_t pos, const char* s);
    OFString& insert (size_t pos, size_t rep, char s);

    /*
    ** Removes up to n characters from the string starting from position pos.
    */
    OFString& erase (size_t pos = 0, size_t n = OFString_npos);

    /*
    ** This function replaces a range of characters in the current string
    ** with a range of characters taken from the input string str. The
    ** range to be replaced starts at position pos1 in the current string,
    ** and extends for n1 characters, or up to the end of the string, 
    ** whichever comes first.
    ** The range of characters inserted starts at position pos2 of the
    ** input string str, and extends for n2 characters, or up to the end of
    ** the string str, whichever comes first.
    ** All of the overloaded functions construct a string object from the
    ** input s and replace the range [pos, n] in the current string with
    ** the constructed string.
    */
    OFString& replace (size_t pos1, size_t n1, const OFString& str, 
		       size_t pos2 = 0, size_t n2 = OFString_npos);
    OFString& replace (size_t pos, size_t n, const char* s, size_t n2);
    OFString& replace (size_t pos, size_t n, const char* s);
    OFString& replace (size_t pos, size_t n, size_t rep, char s);

    /* 
    ** This function returns a constant reference to the character at 
    ** position pos of the current string.
    ** In the overloaded at() function if pos < size() returns a reference
    ** to the character at position pos, else throws out_of_range
    ** exception.
    */
    const char& at (size_t pos) const
    {
	OFSTRING_OUTOFRANGE (pos >= this->size());
	return this->theCString[pos];
    }
    char& at (size_t pos)
    {
	OFSTRING_OUTOFRANGE (pos >= this->size());
	return this->theCString[pos];
    }

    /*
    ** Returns the element at position pos of the current string. Returns
    ** '\0' if pos == size().
    ** In the overloaded operator[] if pos < size(), returns the element
    ** at position pos of the current string. The reference returned is
    ** invalid after a subsequent call to any non-const member function for
    ** the object.
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
    char& operator[] (size_t pos)
    {
	OFSTRING_OUTOFRANGE (pos >= this->size());
	return this->theCString[pos];
    }

    /*
    ** Returns a pointer to the initial element of an array of length
    ** size()+1 whose first size() elements equal the corresponding elements
    ** of the current string and whose last element is a null character.
    */
    const char* c_str () const
    {
	return (this->theCString)?(this->theCString):("");
    }

    /*
    ** If size() is nonzero, the member returns a pointer to the initial 
    ** element of an array whose first size() elements equal the 
    ** corresponding elements of the string controlled by *this. 
    ** If size() is zero, the member returns a non-null pointer that is 
    ** copyable and can have zero added to it.
    */
    const char* data () const;

    /*
    ** Returns a count of the number of char-like objects currently in 
    ** the string.
    */
    size_t size () const
    {
	return (this->theCString)?(strlen(this->theCString)):(0);
    }
    size_t length () const
    {
	return this->size();
    }

    /*
    ** Return true if the string is empty, false otherwise.
    */
    OFBool empty() const {
	return (this->size() == 0)?(OFTrue):(OFFalse);
    }

    /*
    ** If n <= size(), truncates the string to length n else it pads the
    ** extra locations with c. Reports a length error if n equals OFString_npos.
    */
    void resize (size_t n, char c = '\0');

    /*
    ** Returns the size of the allocated storage in the string.
    */
    size_t capacity () const
    {
	return this->theCapacity;
    }

    /*
    ** Returns the maximum size of a string which could possibly by allocated.
    ** ????
    */
    size_t max_size() const
    {
	return ((OFString_npos - 1)/sizeof(char));
    }

    /*
    ** Empty the string of all contents
    */
    void clear() 
    {
	this->erase();
    }

    /*
    ** This function is a directive that informs a string of a planned
    ** changed in size, so that it can manage the storage allocation 
    ** accordingly.  Reallocation of a string happens if and only if 
    ** the current capacity is less than res_arg. After this call, 
    ** capacity() is greater than or equal to res_arg if reallocation 
    ** happens and equal to the previous value of capacity() otherwise.
    */
    void reserve (size_t res_arg);

    /*
    ** This function replaces the string designated by s with a copy of a
    ** range of characters from the current string. The range copied begins
    ** at position pos of the current string and extends for n characters or
    ** up to the end of the current string, whichever comes first.
    */
    size_t copy (char* s, size_t n, size_t pos = 0) const;

    /*
    ** This function returns a copy the substring consisting of at most n
    ** characters starting at position pos of the current string.
    */
    OFString substr (size_t pos = 0, size_t n = OFString_npos) const;

    /*
    ** This function swaps the contents of the two strings. The time 
    ** complexity of this function is linear.
    */
    void swap (OFString& s);

    /*
    ** Determines the effective length rlen of the strings to compare as 
    ** the smallest of size() and str.size(). The function then compares 
    ** the two strings by calling strcmp(data(), str.data(), rlen).
    ** Returns: the nonzero result if the result of the comparison is 
    ** nonzero. Otherwise, returns a value < 0 if size() < str.size(),
    ** a value of 0 if size() == str.size(), or a value > 0 if 
    ** size() > str.size().
    ** All of the overloaded functions construct a string from the input s,
    ** and compares it with the current string.
    */
    int compare (const OFString& str) const;
    int compare (size_t pos1, size_t n1, const OFString& str) const;
    int compare (size_t pos1, size_t n1, const OFString& str,
		 size_t pos2, size_t n2) const;
    int compare (const char* s) const;
    int compare (size_t pos1, size_t n1,
		 const char* s, size_t n2 = OFString_npos) const;

    /*
    ** This member function determines the earliest occurrence of the
    ** input pattern in the current string object, starting from position
    ** pos in the current string. If find can determine such an occurrence,
    ** it returns the starting index of pattern in the current string. 
    ** Otherwise, it returns string::npos.
    ** All of the overloaded functions construct a string from the input
    ** pattern, and try to find the pattern in the current string.
    */
    size_t find (const OFString& pattern, size_t pos = 0) const;
    size_t find (const char* pattern, size_t pos, size_t n) const;
    size_t find (const char* pattern, size_t pos = 0) const;
    size_t find (char pattern, size_t pos = 0) const;

    /*
    ** This function scans the current string backwards, and finds the first
    ** occurrence of pattern in the string (from the back). The starting
    ** index of the matched position in the current string should be less
    ** than or equal to the parameter pos. If a match is found, the starting
    ** index is returned; otherwise, the function returns string::npos.
    ** All of the overloaded functions construct a string from the input
    ** pattern, and then call rfind on the current string with the 
    ** constructed input.
    */
    size_t rfind (const OFString& pattern, size_t pos = OFString_npos) const;
    size_t rfind (const char* pattern, size_t pos, size_t n) const;
    size_t rfind (const char* pattern, size_t pos = OFString_npos) const;
    size_t rfind (char pattern, size_t pos = OFString_npos) const;

    /*
    ** This function determines the first location, loc, between pos and
    ** the end of the current string, such that the character at loc matches
    ** at least one character from the parameter string str. If such a 
    ** location can be determined, it is returned. Otherwise, the function 
    ** returns string::npos.
    ** All of the overloaded member functions construct a string from the
    ** input s, and then call find_first_of on the current string with
    ** the constructed input.
    */
    size_t find_first_of (const OFString& str, size_t pos = 0) const;
    size_t find_first_of (const char* s, size_t pos, size_t n) const;
    size_t find_first_of (const char* s, size_t pos = 0) const;
    size_t find_first_of (char s, size_t pos = 0) const;

    /*
    ** This function determines the highest location, loc, up to pos, such
    ** that the character at loc matches at least one character from the 
    ** parameter string str. If such a location can be determined, it is 
    ** returned. Otherwise, the function returns string::npos.
    ** All of the overloaded member functions construct a string from the
    ** input s, and then call find_last_of on the current string with the
    ** constructed input.
    */
    size_t find_last_of (const OFString& str, size_t pos = OFString_npos) const;
    size_t find_last_of (const char* s, size_t pos, size_t n) const;
    size_t find_last_of (const char* s, size_t pos = OFString_npos) const;
    size_t find_last_of (char s, size_t pos = OFString_npos) const;

    /*
    ** This function determines the first location loc, between pos and
    ** the end of the current string, such that the character at loc does not
    ** match any character from the parameter string str. If such a location
    ** is found, it is returned. Otherwise, the function returns string::npos.
    ** All of the overloaded member functions construct a string from the
    ** input s, and then call find_first_not_of on the current string
    ** with the constructed input.
    */
    size_t find_first_not_of (const OFString& str, size_t pos = 0) const;
    size_t find_first_not_of (const char* s, size_t pos, size_t n) const;
    size_t find_first_not_of (const char* s, size_t pos = 0) const;
    size_t find_first_not_of (char c, size_t pos = 0) const;

    /*
    ** This function scans the current string up to the position pos and 
    ** determines the highest location, loc, such that the character at loc
    ** does not match any character from the parameter string str. If such
    ** a location is found, it is returned. Otherwise, the function returns
    ** string::npos.
    ** All of these member functions construct a string from the input s,
    ** and then call find_last_not_of on the current string with the
    ** constructed input.
    */
    size_t find_last_not_of (const OFString& str, size_t pos = OFString_npos) const;
    size_t find_last_not_of (const char* s, size_t pos, size_t n) const;
    size_t find_last_not_of (const char* s, size_t pos = OFString_npos) const;
    size_t find_last_not_of (char c, size_t pos = OFString_npos) const;

};


/*
** Puts the string s on the output stream o.
*/
ostream& operator<< (ostream& o, const OFString& s);

/*
** Reads a string of characters from input stream i into s. Any
** whitespace is treated as a string terminator.
*/
istream& operator>> (istream& i, OFString& s);


/*
** Appends the string rhs to lhs.
** All of the overloaded member functions construct strings from lhs
** and rhs, and then append rhs to lhs.
*/
OFString operator+ (const OFString& lhs, const OFString& rhs);
OFString operator+ (const char* lhs, const OFString& rhs);
OFString operator+ (char lhs, const OFString& rhs);
OFString operator+ (const OFString& lhs, const char* rhs);
OFString operator+ (const OFString& lhs, char rhs);

/*
** Returns true if lhs.compare(rhs) is zero, otherwise false.
** All of the overloaded == member functions construct strings from
** lhs and rhs, and then call operator== (OFString&, OFString&)
** for string equality.
*/
OFBool operator== (const OFString& lhs, const OFString& rhs);
OFBool operator== (const char* lhs, const OFString& rhs);
OFBool operator== (char lhs, const OFString& rhs);
OFBool operator== (const OFString& lhs, const char* rhs);
OFBool operator== (const OFString& lhs, char rhs);

/*
** Returns true if lhs.compare(rhs) < 0, otherwise false.
** All of the less than member functions construct strings from lhs
** and rhs, and then call operator< for strings.
*/
OFBool operator< (const OFString& lhs, const OFString& rhs);
OFBool operator< (const char* lhs, const OFString& rhs);
OFBool operator< (char lhs, const OFString& rhs);
OFBool operator< (const OFString& lhs, const char* rhs);
OFBool operator< (const OFString& lhs, char rhs);

/*
** All of the less than equal to member functions construct strings
** from lhs and rhs (if needed), and then return !(rhs < lhs).
*/
OFBool operator<= (const OFString& lhs, const OFString& rhs);
OFBool operator<= (const char* lhs, const OFString& rhs);
OFBool operator<= (char lhs, const OFString& rhs);
OFBool operator<= (const OFString& lhs, const char* rhs);
OFBool operator<= (const OFString& lhs, char rhs);

/*
** All of the not equal to member functions construct strings from lhs
** and rhs (if needed), and then return !(lhs == rhs).
*/
OFBool operator!= (const OFString& lhs, const OFString& rhs);
OFBool operator!= (const char* lhs, const OFString& rhs);
OFBool operator!= (char lhs, const OFString& rhs);
OFBool operator!= (const OFString& lhs, const char* rhs);
OFBool operator!= (const OFString& lhs, char rhs);

/*
** All of the greater than member functions construct strings from lhs
** and rhs (if needed), and then return (rhs < lhs).
*/
OFBool operator> (const OFString& lhs, const OFString& rhs);
OFBool operator> (const char* lhs, const OFString& rhs);
OFBool operator> (char lhs, const OFString& rhs);
OFBool operator> (const OFString& lhs, const char* rhs);
OFBool operator> (const OFString& lhs, char rhs);

/*
** All of the greater than equal to member functions construct strings
** from lhs and rhs (if needed), and then return !(lhs < rhs).
*/
OFBool operator>= (const OFString& lhs, const OFString& rhs);
OFBool operator>= (const char* lhs, const OFString& rhs);
OFBool operator>= (char lhs, const OFString& rhs);
OFBool operator>= (const OFString& lhs, const char* rhs);
OFBool operator>= (const OFString& lhs, char rhs);

#endif /* HAVE_STD_STRING */

#endif /* OFSTRING_H */

/*
** CVS/RCS Log:
** $Log: ofstring.h,v $
** Revision 1.5  1998-11-27 12:42:52  joergr
** Added copyright message to source files and changed CVS header.
**
** Revision 1.4  1997/09/01 10:00:12  hewett
** Added absent $ terminator to RCS/CVS Revision keyword in header.
**
** Revision 1.3  1997/07/14 13:37:31  meichel
** Simplified OFString code to allow compilation with Sun CC 2.0.1
**
** Revision 1.2  1997/07/07 14:05:24  hewett
** Renamed the constant OFnpos to OFString_npos to look more like
** the real ANSI constant string::npos.
**
** Revision 1.1  1997/07/07 11:52:18  meichel
** Added string class OFString to ofstd library.
** This class implements a subset of the ANSI C++ "string" class.
**
**
*/

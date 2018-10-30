/*
 *
 *  Copyright (C) 2018, OFFIS e.V.
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
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Implementing the currently used subset of C++17' std::filesystem
 *
 */

#ifndef OFFILSYS_H
#define OFFILSYS_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofutil.h"

/** @file offilsys.h
 *  Implementing the currently used subset of C++17' std::filesystem,
 *  i.e.\ various classes, free standing functions and global definitions as
 *  can be found in <filesystem>.
 */

// forward declarations
class OFdirectory_entry;
class OFdirectory_iterator;

/** A class representing paths on a filesystem.
 *  OFpath implements a subset of the functionality described as
 *  std::filesystem::path, see http://en.cppreference.com/w/cpp/filesystem/path
 *  for more information.
 *  @warning One major difference of the current OFpath implementation and the
 *    real std::filesystem::path is that std::filesystem::path native string
 *    type is expected to be std::wstring on Windows, whereas OFpath currently
 *    always uses OFString, since something like OFWString does currently not
 *    exist. This will be addressed before making OFpath interchangeable with
 *    std::filesystem::path.
 */
class DCMTK_OFSTD_EXPORT OFpath
{
public:
    /** The path separator used in the native path syntax, i.e.\ '\' on Windows
     *  and '/' everywhere else.
     */
    static const char preferred_separator = PATH_SEPARATOR;

    /** Construct an empty path.
     *  An empty path refers to the same location as the path "." on most
     *  systems.
     */
    OFpath();

    /** Copy construct a path.
     *  @param rhs another OFpath object that this one will be constructed
     *    from.
     */
    OFpath( const OFpath& rhs );

    /** Move construct a path.
     *  @param rhs another OFpath object that this one will be constructed
     *    from.
     *  @remarks The current implementation constructs an empty path and swaps
     *    it with the one provided as rhs in constant time.
     */
    OFpath( OFrvalue_ref(OFpath) rhs );

    /** Construct an OFpath from a const char*.
     *  @param cstr a plain old C character string.
     *  @warning This constructor does not exists in the real
     *    std::filesystem::path, where various character strings (e.g. char*
     *    and wchar_t*) are handled uniformly by one of the template
     *    constructors.
     *  @note Automatic path conversion (e.g. replacing all '/' with '\' on
     *    Windows) is currently not available, which is why this constructor
     *    lacks the second parameter "format" and instead always expects
     *    the argument to be given in the native format.
     */
    OFpath( const char* const cstr );

    /** Constructs an OFpath from an OFString.
     *  @param string an OFString.
     *  @note Automatic path conversion (e.g. replacing all '/' with '\' on
     *    Windows) is currently not available, which is why this constructor
     *    lacks the second parameter "format" and instead always expects
     *    the argument to be given in the native format.
     */
    OFpath( const OFString& string );

    /** Copy-assign another OFpath to this one.
     *  Effectively: this->native() = rhs.native().
     *  @param rhs another OFpath object.
     *  @return *this
     */
    OFpath& operator=( const OFpath& rhs );

    /** Move-assign another OFpath to this one.
     *  @param rhs another OFpath object.
     *  @return *this
     *  @remarks The current implementation clears the path and then swaps it
     *    with the one provided as rhs in constant time.
     */
    OFpath& operator=( OFrvalue_ref(OFpath) rhs );

    /** Test whether the path is empty.
     *  @return effectively: native().empty().
     */
    OFBool empty() const;

    /** Test whether the path is absolute, i.e.\ refers to a certain location
     *  unaffected by which directory is set as the current working directory.
     *  @return OFTrue if the path is absolute, OFFalse otherwise.
     */
    OFBool is_absolute() const;

    /** Test whether the path is relative, e.g.\ needs to be concatenated to
     *  another path before it refers to a filesystem location.
     *  @return effectively: !is_absolute().
     */
    OFBool is_relative() const;

    /** Test whether the path contains a root name.
     *  @return effectively: !root_name().empty().
     */
    OFBool has_root_name() const;

    /** Test whether the path contains a root directory.
     *  @return effectively: !root_directory().empty().
     */
    OFBool has_root_directory() const;

    /** Test whether the path contains a filename.
     *  @return effectively: !filename().empty().
     */
    OFBool has_filename() const;

    /** Test whether the path contains a filename extension.
     *  @return effectively: !extension().empty().
     */
    OFBool has_extension() const;

    /** Retrieve the underlying native string.
     *  @return a const reference to the underlying OFString.
     */
    const OFString& native() const;

    /** Retrieve a native character pointer.
     *  @return effectively: native().c_str().
     */
    const char* c_str() const;

    /** Return the root name contained in the path (if any) or an empty path.
     *  The root name is a concept used in multi root filesystems (Windows)
     *  to identify the root (e.g. C:).
     *  @return an OFpath containing only the root name, e.g given the path
     *    "C:\" it will return "C:" on Windows and "" everywhere else.
     */
    OFrvalue<OFpath> root_name() const;

    /** Return the filename contained in the path (if any) or an empty path.
     *  The filename is the substring after the last path separator, and,
     *  for multi root filesystems also after the root name.
     *  @return an OFpath containing only the filename, e.g. "file" given a
     *    "/path/to/file" or resp. "C:\path\to\file".
     *  @note OFpath( "C:" ).filename() is "" on Windows and "C:" everywhere
     *    else.
     */
    OFrvalue<OFpath> filename() const;

    /** Return the filename extension contained in the path (if any) or an
     *  empty path.
     *  The filename extension is the part of the filename beginning at (and
     *  containing) the last '.', e.g. ".txt" for a file named "readme.txt".
     *  Special filenames like ".", ".." and hidden files like ".hidden" will
     *  be handled in a special way. The names ".", ".." and ".hidden" will
     *  give an empty string result, whereas ".hidden.txt" will give ".txt".
     *  @return an OFpath containing only the extension part of the filename.
     */
    OFrvalue<OFpath> extension() const;

    /** Append another path to this one, handling various special cases.
     *  @param rhs another OFpath object to append to this one.
     *  @return *this
     *  @details
     *  This operator will append another path to the existing one, always
     *  doing the right thing. There are many special cases it accounts for,
     *  for details see (http://en.cppreference.com/w/cpp/filesystem/path/append)
     *  In short: you can trust it to turn "path" / "file" into "path\file"
     *  on Windows and "path/file" everywhere else, while also ensuring
     *  "path/" / "file" will become "path/file" and not "path//file" and many
     *  more.
     */
    OFpath& operator/=( const OFpath& rhs );

private:
#ifndef DOXYGEN
    // helper functions, common ground for the respective has_XXX() and XXX()
    // functions.
    size_t findRootName() const;
    size_t findFilename() const;
    size_t findExtension() const;
    // the actual native string, who would have guessed
    OFString m_NativeString;
#endif
};

/** Append two paths.
 *  @relates OFpath
 *  @param lhs the left hand side path for the operation.
 *  @param rhs the right hand side path for the operation.
 *  @return effectively: OFpath( lhs ) / rhs.
 */
inline OFrvalue<OFpath> operator/( const OFpath& lhs, const OFpath& rhs )
#ifndef DOXYGEN
{
    return OFpath( lhs ) /= rhs;
}
#else
;
#endif

/** Print a path to an output stream.
 *  @relates OFpath
 *  The parameters and return value are as usual for the output stream
 *  operator, the only thing worth mentioning is that the path will be put
 *  into double quotes, i.e. it will print "rhs.native()".
 *  @param out the stream.
 *  @param rhs the path
 *  @return out
 */
inline STD_NAMESPACE ostream& operator<<( STD_NAMESPACE ostream& out, const OFpath& rhs )
#ifndef DOXYGEN
{
    return out << '"' << rhs.native() << '"';
}
#else
;
#endif

/** Lexicographically compare two paths.
 *  @relates OFpath
 *  @param lhs the left hand side path for the operation.
 *  @param rhs the right hand side path for the operation.
 *  @return effectively: lhs.native() < rhs.native()
 *  @note the comparison is purely lexicographic, i.e. the paths will not be
 *    normalized in any way prior to comparing their native strings.
 */
inline OFBool operator<( const OFpath& lhs, const OFpath& rhs )
#ifndef DOXYGEN
{
    return lhs.native() < rhs.native();
}
#else
;
#endif

/** Lexicographically compare two paths.
 *  @relates OFpath
 *  @param lhs the left hand side path for the operation.
 *  @param rhs the right hand side path for the operation.
 *  @return effectively: lhs.native() > rhs.native()
 *  @note the comparison is purely lexicographic, i.e. the paths will not be
 *    normalized in any way prior to comparing their native strings.
 */
inline OFBool operator>( const OFpath& lhs, const OFpath& rhs )
#ifndef DOXYGEN
{
    return lhs.native() > rhs.native();
}
#else
;
#endif

/** Lexicographically compare two paths.
 *  @relates OFpath
 *  @param lhs the left hand side path for the operation.
 *  @param rhs the right hand side path for the operation.
 *  @return effectively: lhs.native() <= rhs.native()
 *  @note the comparison is purely lexicographic, i.e. the paths will not be
 *    normalized in any way prior to comparing their native strings.
 */
inline OFBool operator<=( const OFpath& lhs, const OFpath& rhs )
#ifndef DOXYGEN
{
    return lhs.native() <= rhs.native();
}
#else
;
#endif

/** Lexicographically compare two paths.
 *  @relates OFpath
 *  @param lhs the left hand side path for the operation.
 *  @param rhs the right hand side path for the operation.
 *  @return effectively: lhs.native() >= rhs.native()
 *  @note the comparison is purely lexicographic, i.e. the paths will not be
 *    normalized in any way prior to comparing their native strings.
 */
inline OFBool operator>=( const OFpath& lhs, const OFpath& rhs )
#ifndef DOXYGEN
{
    return lhs.native() >= rhs.native();
}
#else
;
#endif

/** Lexicographically compare two paths.
 *  @relates OFpath
 *  @param lhs the left hand side path for the operation.
 *  @param rhs the right hand side path for the operation.
 *  @return effectively: lhs.native() == rhs.native()
 *  @note the comparison is purely lexicographic, i.e. the paths will not be
 *    normalized in any way prior to comparing their native strings.
 */
inline OFBool operator==( const OFpath& lhs, const OFpath& rhs )
#ifndef DOXYGEN
{
    return lhs.native() == rhs.native();
}
#else
;
#endif

/** Lexicographically compare two paths.
 *  @relates OFpath
 *  @param lhs the left hand side path for the operation.
 *  @param rhs the right hand side path for the operation.
 *  @return effectively: lhs.native() != rhs.native()
 *  @note the comparison is purely lexicographic, i.e. the paths will not be
 *    normalized in any way prior to comparing their native strings.
 */
inline OFBool operator!=( const OFpath& lhs, const OFpath& rhs )
#ifndef DOXYGEN
{
    return lhs.native() != rhs.native();
}
#else
;
#endif

/** A class representing an directory entry.
 *  OFdirectory_entry implements a subset of the functionality described as
 *  std::filesystem::directory_entry, see
 *  http://en.cppreference.com/w/cpp/filesystem/directory_entry for more
 *  information.
 *  @note currently, OFdirectory_entry only contains a single OFpath object
 *    (no file meta information as the real std::filesystem::directory_entry
 *    would). It is currently only a placeholder that exists because
 *    OFdirectory_iterator is expected to refer to an directory entry instead
 *    of directly referring to an OFpath object.
 */
class DCMTK_OFSTD_EXPORT OFdirectory_entry
{
public:
    /** Access the OFpath object the directory entry applies to.
     *  @return a const reference to an OFpath object.
     */
    const OFpath& path() const;

    /** Automatically convert a directory entry to an OFpath if the context of
     *  the evaluation allows it, e.g. for printing an OFdirectory_entry on
     *  an output stream.
     *  @return effectively: this->path().
     */
    operator const OFpath&() const;

private:
#ifndef DOXYGEN
    // OFdirectory_iterator needs write access to the path for updating it
    // during the iteration.
    friend class OFdirectory_iterator;
    OFpath m_Path;
#endif
};

#ifndef DOXYGEN
// This is a helper class hidden from the users.
// It helps implementing the post increment operator of the actual directory
// iterator, which, since it is an input iterator, cannot return a copy of
// itself as a forward iterator would do, but instead only copies the 'value'
// (the OFdirectory_entry part) of the enclosing iterator.
class DCMTK_OFSTD_EXPORT OFdirectory_iterator_proxy
{
public:
    inline OFdirectory_iterator_proxy(OFrvalue_ref(OFdirectory_iterator_proxy) rhs)
    : m_Entry( OFmove( OFrvalue_access( rhs ).m_Entry ) ) {}
    // add operators '->' and '*' to access the 'value', acting like an
    // iterator would although this already is the actual entry object
    inline const OFdirectory_entry* operator->() const { return &m_Entry; }
    inline const OFdirectory_entry& operator*() const { return m_Entry; }

private:
    // only a directory iterator may create instances of this object, others
    // shouldn't even know it exists
    friend class OFdirectory_iterator;
    OFdirectory_iterator_proxy(const OFdirectory_entry&);
    OFdirectory_entry m_Entry;
};
#endif

/** An iterator for iterating over all the files (and folders) contained in
 *  a given filesystem location.
 *  OFdirectory_iterator implements a subset of the functionality described as
 *  std::filesystem::directory_iterator, see
 *  http://en.cppreference.com/w/cpp/filesystem/directory_iterator for more
 *  information.
 */
class DCMTK_OFSTD_EXPORT OFdirectory_iterator
{
public:
    /** A valid OFdirectory_iterator refers to an OFdirectory_entry
     */
    typedef OFdirectory_entry value_type;

    /** The distance between to OFdirectory_iterators would be measured
     *  as an std::ptrdiff_t. Well, it cannot be measured since
     *  OFdirectory_iterator is an input iterator, but some code might
     *  depend on the type being defined, so we define it anyway.
     */
    typedef STD_NAMESPACE ptrdiff_t difference_type;

    /** OFdirectory_iterator's '->' operator will give you a pointer to a
     *  const OFdirectory_entry.
     */
    typedef const OFdirectory_entry* pointer;

    /** OFdirectory_iterator's '*' operator will give you a const reference
     *  to an OFdirectory_entry.
     */
    typedef const OFdirectory_entry& reference;

    // TODO: we probably need a configure test for this, simply writing
    //       STD_NAMESPACE input_iterator_tag without including anything
    //       etc. seems prone to errors.
    // typedef std::input_iterator_tag iterator_category;

    /** Construct a past-the-end OFdirectory_iterator for being used as the
     *  sentinel when iterating.
     */
    OFdirectory_iterator();

    /** Construct an OFdirectory_iterator for iterating over all the files
     *  (and folders) inside the given location (not recursively).
     *  @param path the location to search for files in.
     *  @note the special files "." and ".." will be skipped.
     */
    OFdirectory_iterator( const OFpath& path );

    /** Copy construct an OFdirectory_iterator, referring to the same
     *  directory entry as the given one.
     *  @param rhs the iterator to copy.
     *  @remarks OFdirectory_iterator uses an OFshared_ptr for all information
     *    needed for iterating, copy construction is therefore efficient and
     *    in constant time.
     */
    OFdirectory_iterator( const OFdirectory_iterator& rhs );

    /** Assign a different OFdirectory_iterator to this one.
     *  @param rhs another iterator that will be copied into this one.
     *  @return *this
     *  @remarks OFdirectory_iterator uses an OFshared_ptr for all information
     *    needed for iterating, copy assignment is therefore efficient and
     *    in constant time.
     */
    OFdirectory_iterator& operator=( const OFdirectory_iterator& rhs );

    /** Destroy an OFdirectory_iterator freeing the data needed for iterating.
     */
    ~OFdirectory_iterator();

    /** Compare an OFdirectory_iterator against another one.
     *  This is primarily used for comparing a potentially valid iterator
     *  against the past-the-end iterator OFdirectory_iterator() to determine
     *  whether the iterator refers to an directory entry or not.
     *  @param rhs another iterator to compare this one against.
     *  @return OFTrue if both iterators refer to past-the-end, OFFalse
     *    otherwise unless one of the given iterators was copy constructed
     *    or copy assigned from the other one.
     *  @note this operator explicitly does not compare the iterators
     *    semantically (i.e. whether both refer to the same directory entry).
     */
    OFBool operator==( const OFdirectory_iterator& rhs ) const;

    /** Compare an OFdirectory_iterator against another one.
     *  This is primarily used for comparing a potentially valid iterator
     *  against the past-the-end iterator OFdirectory_iterator() to determine
     *  whether the iterator refers to an directory entry or not.
     *  @param rhs another iterator to compare this one against.
     *  @return effectively: !( *this == rhs )
     *  @note this operator explicitly does not compare the iterators
     *    semantically (i.e. whether both refer to the same directory entry).
     */
    OFBool operator!=( const OFdirectory_iterator& rhs ) const;

    /** Increment the iterator.
     *  Incrementing the iterator will make it refer to the next file in the
     *  originally given location or to path-the-end in case the current one
     *  was the last file.
     *  @pre *this != OFdirectory_iterator()
     *  @return *this
     */
    OFdirectory_iterator& operator++();

#ifndef DOXYGEN
    OFrvalue<OFdirectory_iterator_proxy> operator++(int);
#else
    /** Increment the iterator.
     *  Incrementing the iterator will make it refer to the next file in the
     *  originally given location or to path-the-end in case the current one
     *  was the last file.
     *  @pre *this != OFdirectory_iterator()
     *  @return an object that, when dereferenced, refers to the directory
     *   entry that this iterator referred to before it was incremented.
     *  @note Since OFdirectory_iterator is an input iterator, the iterator
     *    value from before incrementing it cannot be restored afterwards,
     *    such that this post increment operator cannot return an
     *    OFdirectory_iterator object as a normal post increment operator
     *    would.
     */
    unspecified operator++(int);
#endif

    /** Dereference the iterator.
     *  @pre *this != OFdirectory_iterator()
     *  @return a pointer to the const OFdirectory_entry object this iterator
     *    refers to.
     */
    pointer operator->() const;

    /** Dereference the iterator.
     *  @pre *this != OFdirectory_iterator()
     *  @return a const reference the OFdirectory_entry object this iterator
     *    refers to.
     */
    reference operator*() const;

private:
#ifndef DOXYGEN
    // PIMPL style actual implementation
    class NativeDirectoryEntry;
    OFshared_ptr<NativeDirectoryEntry> m_pEntry;
#endif
};

#endif // OFFILSYS_H

/*
 *
 *  Copyright (C) 2006-2019, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose: C++ wrapper class for stdio FILE functions and
 *           wide character filenames
 *
 */

#ifndef OFFILE_H
#define OFFILE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftypes.h"    /* for class OFBool */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */
#include "dcmtk/ofstd/ofstd.h"      /* for class OFStandard */

#define INCLUDE_UNISTD
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
//#define INCLUDE_CWCHAR    /* not yet implemented in "ofstdinc.h" */
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>       /* needed for struct _stati64 on Win32 */
#endif
END_EXTERN_C

#ifdef HAVE_UNIX_H
#include <unix.h>           /* needed for setlinebuf() on QNX */
#endif

/* HP-UX has clearerr both as macro and as a function definition. We have to
 * undef the macro so that we can define a function called "clearerr".
 */
#if defined(__hpux) && defined(clearerr)
#undef clearerr
#endif

/* When using the ISO C++ include files such as <cstdio>, <cstdarg> etc.,
 * all ANSI C functions like fopen() are declared in namespace std,
 * (e.g. we have to use std::fopen()), but non-ANSI Posix functions remain
 * in global namespace, e.g. we have to use ::fopen64().
 * To make things even more difficult, not all compilers really declare
 * ANSI C functions in namespace std in accordance with the C++ standard.
 * Yes, this is ugly.
 */

/* Find out whether to use explicit LFS function calls to handle
 * large file support
 */
#if defined(DCMTK_ENABLE_LFS) && DCMTK_ENABLE_LFS == DCMTK_LFS64
#define EXPLICIT_LFS_64

// Use POSIX 64 bit file position type when available
#ifdef HAVE_FPOS64_T
typedef fpos64_t offile_fpos_t;
#else // Otherwise this should be sufficient
typedef fpos_t offile_fpos_t;
#endif

// Use POSIX 64 bit file offset type when available
#ifdef HAVE_OFF64_T
typedef off64_t offile_off_t;
#elif !defined(OF_NO_SINT64) // Otherwise use a 64 bit integer
typedef Sint64 offile_off_t;
#else // Cry when 64 LFS is required but no 64 bit integer exists
#error \
  Could not find a suitable offset-type for LFS64 support.
#endif

#else // Implicit LFS or no LFS

#if defined(DCMTK_ENABLE_LFS) && DCMTK_ENABLE_LFS == DCMTK_LFS
#if defined(SIZEOF_FPOS_T) && (!defined(SIZEOF_OFF_T) || SIZEOF_FPOS_T > SIZEOF_OFF_T)
// strange Windows LFS where sizeof(fpos_t) == 8 but sizeof(off_t) == 4
#ifndef OF_NO_SINT64 // Use a 64 bit integer
typedef Sint64 offile_off_t;
#else // Cry when LFS is required but no 64 bit integer exists
#error \
  Could not find a suitable offset-type for LFS support.
#endif
#else
typedef off_t offile_off_t;
#endif
#elif defined(HAVE_FSEEKO)
typedef off_t offile_off_t;
#else
typedef long offile_off_t;
#endif
typedef fpos_t offile_fpos_t;

#endif // basic type definitions

// the type we use to store the last error.
typedef int offile_errno_t;


// forward declarations
class OFpath;

/** class for managing filenames consisting either of conventional (8-bit) or
 *  wide (e.g.\ 16-bit) characters.  The wide character support is currently
 *  Windows-specific because most other operating systems use UTF-8, which is
 *  compatible with conventional 8-bit character strings.
 */
class DCMTK_OFSTD_EXPORT OFFilename
{
public:
  /** default constructor
   */
  OFFilename();

  /** constructor expecting a conventional character string
   *  @param filename filename to be stored (8-bit characters, e.g. UTF-8)
   *  @param convert  convert given filename to wide character encoding as an
   *    alternative representation
   */
  OFFilename(const char *filename,
             const OFBool convert = OFFalse);

  /** constructor expecting a character string as an OFString instance
   *  @param filename filename to be stored (8-bit characters, e.g. UTF-8)
   *  @param convert  convert given filename to wide character encoding as an
   *    alternative representation.  Only works on Windows systems.
   */
  OFFilename(const OFString &filename,
             const OFBool convert = OFFalse);

  /** construct an OFFilename from an OFpath.
   *  @param path the OFpath object referring to a file.
   *  Effectively OFFilename(path.native(), OFTrue), but potentially more
   *  efficient.
   */
  OFFilename(const OFpath& path);

#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  /** constructor expecting a wide character string
   *  @remark This constructor is only available if DCMTK is compiled on Windows
   *  Operating Systems with wide chars enabled (defining _WIN32 as well as
   *  WIDE_CHAR_FILE_IO_FUNCTIONS or WIDE_CHAR_MAIN_FUNCTION).
   *  @param filename filename to be stored (e.g. 16-bit characters)
   *  @param convert  convert given filename to UTF-8 encoding as an
   *    alternative representation.  Only works on Windows systems.
   */
  OFFilename(const wchar_t *filename,
             const OFBool convert = OFTrue);
#endif

  /** copy constructor
   *  @param arg filename object to be copied
   */
  OFFilename(const OFFilename &arg);

  /** destructor. Frees memory.
   */
  ~OFFilename();

  /** assignment operator
   *  @param arg filename object to be copied
   *  @return reference to this filename object
   */
  OFFilename &operator=(const OFFilename &arg);

  /** clear currently stored filename
   */
  void clear();

  /** fast, non-throwing swap function. The time complexity of this function
   *  is constant.
   *  @param arg filename object to swap with
   */
  void swap(OFFilename &arg);

  /** checks whether this object stores an empty filename
   *  @return OFTrue if the filename is empty, OFFalse otherwise
   */
  OFBool isEmpty() const;

  /** checks whether this object stores a wide character filename
   *  @return OFTrue if the filename uses wide characters, OFFalse otherwise
   */
  inline OFBool usesWideChars() const
  {
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
    return (wfilename_ != NULL);
#else
    return OFFalse;
#endif
  }

  /** get stored filename consisting of conventional characters
   *  @return filename (might be NULL if none is stored)
   */
  inline const char *getCharPointer() const
  {
    return filename_;
  }

#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  /** get stored filename consisting of wide characters
   *  @remark This method is only available if DCMTK is compiled on Windows
   *  Operating Systems with wide chars enabled (defining _WIN32 as well as
   *  WIDE_CHAR_FILE_IO_FUNCTIONS or WIDE_CHAR_MAIN_FUNCTION).
   *  @return wide char filename (might be NULL if none is stored)
   */
  inline const wchar_t *getWideCharPointer() const
  {
    return wfilename_;
  }
#endif

  /** replace currently stored filename by given value
   *  @param filename filename to be stored (8-bit characters, e.g. UTF-8)
   *  @param convert  convert given filename to wide character encoding as an
   *    alternative representation.  Only works on Windows systems.
   */
  void set(const char *filename,
           const OFBool convert = OFFalse);

  /** replace currently stored filename by given value
   *  @param filename filename to be stored (8-bit characters, e.g. UTF-8)
   *  @param convert  convert given filename to wide character encoding as an
   *    alternative representation).  Only works on Windows systems.
   */
  void set(const OFString &filename,
           const OFBool convert = OFFalse);

#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  /** replace currently stored filename by given value
   *  @remark This method is only available if DCMTK is compiled on Windows
   *  Operating Systems with wide chars enabled (defining _WIN32 as well as
   *  WIDE_CHAR_FILE_IO_FUNCTIONS or WIDE_CHAR_MAIN_FUNCTION).
   *  @param filename filename to be stored (e.g. 16-bit characters)
   *  @param convert  convert given filename to UTF-8 encoding as an alternative
   *    representation.  Only works on Windows systems.
   */
  void set(const wchar_t *filename,
           const OFBool convert = OFTrue);
#endif

private:
  /// filename consisting of conventional characters (8-bit, e.g.\ UTF-8)
  /// @remark This member is only available if DCMTK is compiled on Windows
  /// Operating Systems with wide chars enabled (defining _WIN32 as well as
  /// WIDE_CHAR_FILE_IO_FUNCTIONS or WIDE_CHAR_MAIN_FUNCTION).
  char *filename_;
#if (defined(WIDE_CHAR_FILE_IO_FUNCTIONS) || defined(WIDE_CHAR_MAIN_FUNCTION)) && defined(_WIN32)
  /// filename consisting of wide characters (e.g. 16-bit on Windows)
  wchar_t *wfilename_;
#endif
};

/** swap function for OFFilename class. The time complexity of this function
 *  is constant.
 *  @param lhs left-hand side filename
 *  @param rhs right-hand side filename
 */
inline void swap(OFFilename &lhs, OFFilename &rhs)
{
  lhs.swap(rhs);
}

/** output filename to the given stream.
 *  Only the string of conventional characters (e.g. ASCII or UTF-8) is printed since
 *  we do not expect the output stream (console or logger) to support wide characters.
 *  @param stream output stream
 *  @param filename OFFilename object to print
 *  @return reference to the output stream
 */
DCMTK_OFSTD_EXPORT STD_NAMESPACE ostream &operator<<(STD_NAMESPACE ostream &stream, const OFFilename &filename);


/** this class provides a simple C++ encapsulation layer for stdio FILE pointers.
 *  All stdio functions on files are directly mapped into member functions.
 *  The handling of large files (64 bit file systems) is transparent. Instead
 *  of type off_t, fseek() and ftell() use offile_off_t which is a 64 bit type
 *  if available on the underlying platform. Similarly, getpos() and setpos() use
 *  type offile_fpos_t, which is defined appropriately.
 *  This class provides both fclose() and pclose(), but these are equivalent -
 *  the code always closes pipes with pclose() and files with fclose().
 *  Finally, an abstraction for errno is provided. Error codes should always
 *  be retrieves using methods getLastError() and getLastErrorString() which
 *  on Unix platforms are based on errno and strerror/strerror_r, but may be based
 *  on other mechanisms on platforms where errno does not exist.
 */
class OFFile
{
public:
  /// default constructor, creates an object that is not associated with any file.
  OFFile(): file_(NULL), popened_(OFFalse), lasterror_(0) {}

  /** create object for given stdio FILE
   *  @param f stdio FILE
   */
  OFFile(FILE *f): file_(f), popened_(OFFalse), lasterror_(0) {}

  /// destructor. Closes file if still open.
  ~OFFile()
  {
    if (file_) fclose();
  }

  /** opens the file whose name is the string pointed to by path and associates
   *  a stream with it.
   *  @param filename path to file
   *  @param modes "r", "w" or "a" with possible modifiers "+", "b"
   *  @return true if stream was successfully created, false otherwise, in which
   *    case the error code is set.
   */
  OFBool fopen(const char *filename, const char *modes)
  {
    if (file_) fclose();
#ifdef EXPLICIT_LFS_64
    file_ = :: fopen64(filename, modes);
#else
    file_ = STDIO_NAMESPACE fopen(filename, modes);
#endif
    if (file_) popened_ = OFFalse; else storeLastError();
    return (file_ != NULL);
  }

#if defined(WIDE_CHAR_FILE_IO_FUNCTIONS) && defined(_WIN32)
  /** opens the file whose name is the wide character string pointed to by path and
   *  associates a stream with it.
   *  @remark This member is only available if DCMTK is compiled on Windows
   *  Operating Systems with wide chars enabled (defining _WIN32 as well as
   *  WIDE_CHAR_FILE_IO_FUNCTIONS).
   *  @param filename Unicode filename path to file
   *  @param modes "r", "w" or "a" with possible modifiers "+", "b", as a wide
   *    character string
   *  @return true if stream was successfully created, false otherwise, in which case
   *    the error code is set.
   */
  OFBool wfopen(const wchar_t *filename, const wchar_t *modes)
  {
    if (file_) fclose();
    file_ = _wfopen(filename, modes);
    if (file_) popened_ = OFFalse; else storeLastError();
    return (file_ != NULL);
  }
#endif

  /** opens the file whose name is a conventional or wide character string pointed to
   *  by path and associates. The wide character support is currently Windows-specific.
   *  @param filename object containing the filename path to file
   *  @param modes "r", "w" or "a" with possible modifiers "+", "b"
   *  @return true if stream was successfully created, false otherwise, in which case
   *    the error code is set.
   */
  OFBool fopen(const OFFilename &filename, const char *modes)
  {
    OFBool result = OFFalse;
#if defined(WIDE_CHAR_FILE_IO_FUNCTIONS) && defined(_WIN32)
    if (filename.usesWideChars())
    {
      // convert file mode to wide char string
      const size_t length = strlen(modes) + 1;
      wchar_t *wmodes = new wchar_t[length];
      if (wmodes != NULL)
      {
        for (size_t i = 0; i < length; ++i)
        {
          // conversion of ASCII codes (7-bit) is easy
          wmodes[i] = OFstatic_cast(wchar_t, modes[i]);
        }
        result = wfopen(filename.getWideCharPointer(), wmodes);
      }
      delete[] wmodes;
    } else
#endif
      result = fopen(filename.getCharPointer(), modes);
    return result;
  }

  /** associates a stream with the existing file descriptor 'fd'. The mode of
   *  the stream (one of the values "r", "r+", "w", "w+", "a", "a+") must be
   *  compatible with the mode of the file descriptor. The file position
   *  indicator of the new stream is set to that belonging to 'fd', and the
   *  error and end-of-file indicators are cleared. Modes "w" or "w+" do not
   *  cause truncation of the file. The file descriptor is not dup'ed, and
   *  will be closed when the stream created by fdopen is closed. The result
   *  of applying fdopen to a shared memory object is undefined.
   *  @param fd file descriptor
   *  @param modes "r", "w" or "a" with possible modifiers "+", "b"
   *  @return true if stream was successfully created, false otherwise, in
   *    which case the error code is set.
   */
  OFBool fdopen(int fd, const char *modes)
  {
    if (file_) fclose();
    file_ = :: fdopen(fd, modes);
    if (file_) popened_ = OFFalse; else storeLastError();
    return (file_ != NULL);
  }

  /** opens a process by creating a pipe, forking, and invoking the shell.
   *  Since a pipe is by definition unidirectional, the type argument may
   *  specify only reading or writing, not both; the resulting stream is
   *  correspondingly read-only or write-only. If the object was already
   *  associated with another file or pipe, that one is closed.
   *  @param command shell command line
   *  @param modes "r" or "w"
   *  @return true if pipe was successfully created, false otherwise
   */
  OFBool popen(const char *command, const char *modes)
  {
    if (file_) fclose();
#ifdef HAVE_POPEN
    file_ = :: popen(command, modes);
#else
    file_ = _popen(command, modes);
#endif
    if (file_) popened_ = OFTrue; else storeLastError();
    return (file_ != NULL);
  }

  /** opens the file whose name is the string pointed to by path and associates
   *  the stream pointed maintained by this object with it. The original stream
   *  (if it exists) is closed. The mode argument is used just as in the fopen
   *  function. The primary use of the freopen function is to change the file
   *  associated with a standard text stream (stderr, stdin, or stdout).
   *  @param filename path to file
   *  @param modes "r", "w" or "a" with possible modifiers "+", "b"
   *  @return true if stream was successfully created, false otherwise, in
   *    which case the error code is set.
   */
  OFBool freopen(const char *filename, const char *modes)
  {
#if defined(EXPLICIT_LFS_64) && ! defined(__MINGW32__)
    // MinGW has EXPLICIT_LFS_64 but no freopen64()
    file_ = :: freopen64(filename, modes, file_);
#else
    file_ = STDIO_NAMESPACE freopen(filename, modes, file_);
#endif
    if (file_) popened_ = OFFalse; else storeLastError();
    return (file_ != NULL);
  }

  /** generates a unique temporary filename. The temporary file is then opened
   *  in binary read/write (w+b) mode. The file will be automatically deleted
   *  when it is closed or the program terminates normally.
   *  @return true if stream was successfully created, false otherwise, in
   *    which case the error code is set.
   */
  OFBool tmpfile()
  {
    if (file_) fclose();
#if defined(EXPLICIT_LFS_64) && ! defined(__MINGW32__)
    // MinGW has EXPLICIT_LFS_64 but no tmpfile64()
    file_ = :: tmpfile64();
#else
    file_ = STDIO_NAMESPACE tmpfile();
#endif
    if (file_) popened_ = OFFalse; else storeLastError();
    return (file_ != NULL);
  }

  /** dissociates the named stream from its underlying file or set of functions.
   *  If the stream was being used for output, any buffered data is written
   *  first, using fflush. Independent of the return value of this method,
   *  any further access (including another call to fclose()) to the stream
   *  maintained by this object results in undefined behaviour.
   *  @return 0 upon success, EOF otherwise, in which case the error code is set.
   */
  int fclose()
  {
    int result = 0;
    if (file_)
    {
      if (popened_)
      {
#ifdef HAVE_PCLOSE
        result = :: pclose(file_);
#else
        result = _pclose(file_);
#endif
      }
      else
      {
        result = STDIO_NAMESPACE fclose(file_);
      }
      // After calling fclose() once, the FILE* is gone even if fclose() failed.
      file_ = NULL;
    }
    if (result) storeLastError();
    return result;
  }

  /** waits for the associated process (created with popen) to terminate and
   *  returns the exit status of the command as returned by wait4.
   *  In this implementation, fclose and pclose can be used synonymously.
   *  @return process ID of the child which exited, or -1 on error, in which
   *    case the error code is set
   */
  int pclose() { return fclose(); }

  /** writes n elements of data, each size bytes long, to the stream, obtaining
   *  them from the location given by ptr. Returns the number of items successfully written
   *  (i.e., not the number of characters).  If an error occurs the return value is a short
   *  item count (or zero).
   *  @param ptr pointer to buffer
   *  @param size size of item
   *  @param n number of items
   *  @return number of items written
   */
  size_t fwrite(const void *ptr, size_t size, size_t n)
  {
    return STDIO_NAMESPACE fwrite(ptr, size, n, file_);
  }

  /** reads n elements of data, each size bytes long, from the stream, storing
   *  them at the location given by ptr. Returns the number of items successfully
   *  read (i.e., not the number of characters).  If an error occurs, or the
   *  end-of-file is reached, the return value is a short item count (or zero).
   *  fread does not distinguish between end-of-file and error, and callers must
   *  use feof and ferror to determine which occurred.
   *  @param ptr pointer to buffer
   *  @param size size of item
   *  @param n number of items
   *  @return number of items read
   */
  size_t fread(void *ptr, size_t size, size_t n)
  {
    return STDIO_NAMESPACE fread(ptr, size, n, file_);
  }

  /** forces a write of all user-space buffered data for the given output or
   *  update stream via the stream's underlying write function. The open status
   *  of the stream is unaffected.
   *  @return 0 upon success, EOF otherwise, in which case the error code is set.
   */
  int fflush()
  {
    int result = STDIO_NAMESPACE fflush(file_);
    if (result) storeLastError();
    return result;
  }

  /** reads the next character from stream and returns it as an unsigned char
   *  cast to an int, or EOF on end of file or error.
   *  @return next character from stream or EOF
   */
  int fgetc() { return STDIO_NAMESPACE fgetc(file_); }

  /** The three types of buffering available are unbuffered, block buffered, and
   *  line buffered. When an output stream is unbuffered, information appears on
   *  the destination file or terminal as soon as written; when it is block
   *  buffered many characters are saved up and written as a block; when it is
   *  line buffered characters are saved up until a newline is output or input
   *  is read from any stream attached to a terminal device (typically stdin).
   *  Normally all files are block buffered. if a stream refers to a  terminal
   *  (as stdout normally does) it is line buffered. The standard error  stream
   *  stderr is always unbuffered by default. this function allows to set the
   *  mode of the stream to line buffered.
   *  @return 0 upon success, nonzero otherwise, in which case the error code may be set
   *
   */
  void setlinebuf()
  {
#if defined(_WIN32) || defined(__hpux)
    this->setvbuf(NULL, _IOLBF, 0);
#else
    :: setlinebuf(file_);
#endif
  }

  /** sets the file position indicator for the stream pointed to by stream to
   *  the beginning of the file. This is equivalent to fseek(0, SEEK_SET)
   *  except that the error indicator for the stream is also cleared.
   */
  void rewind()
  {
#if defined(_WIN32) || defined(__CYGWIN__)
    /* On these platforms rewind() fails after reading to the end of file
     * if the file is read-only. Using fseek() instead.
     */
    (void) this->fseek(0L, SEEK_SET);
#else
    STDIO_NAMESPACE rewind(file_);
#endif
  }

  /** clears the end-of-file and error indicators for the stream
   */
  void clearerr() { STDIO_NAMESPACE clearerr(file_); }

  /** tests the end-of-file indicator for the stream, returning non-zero if it
   *  is set. The end-of-file indicator can only be cleared by the function
   *  clearerr. This method is called eof, not feof, because feof() is a macro
   *  on some systems and, therefore, cannot be used as a method name.
   *  @return non-zero if EOF, zero otherwise
   */
  int eof() const
  {
#ifdef feof
    // feof is a macro on some systems. Macros never have namespaces.
    return feof(file_);
#else
    return STDIO_NAMESPACE feof(file_);
#endif
  }

  /** tests the error indicator for the stream, returning non-zero if it is set.
   *  This method is named error, not ferror, because ferror() is a macro
   *  on some systems and, therefore, cannot be used as a method name.
   *  The error indicator can only be reset by the clearerr function.
   *  @return non-zero if error flag is set, zero otherwise
   */
  int error()
  {
#ifdef ferror
    // ferror is a macro on some systems. Macros never have namespaces.
    return ferror(file_);
#else
    return STDIO_NAMESPACE ferror(file_);
#endif
  }

  /** returns the low-level file descriptor associated with the stream.
   *  The spelling of this member function is different from stdio fileno()
   *  because on some systems (such as MinGW) fileno() is a macro
   *  and, therefore, cannot be used as a method name.
   *  @return low-level file descriptor associated with stream
   */
#ifdef fileno
  int fileNo() { return fileno(file_); }
#else
  int fileNo() { return :: fileno(file_); }
#endif

  /** The three types of buffering available are unbuffered, block buffered, and
   *  line buffered. When an output stream is unbuffered, information appears on
   *  the destination file or terminal as soon as written; when it is block
   *  buffered many characters are saved up and written as a block; when it is
   *  line buffered characters are saved up until a newline is output or input
   *  is read from any stream attached to a terminal device (typically stdin).
   *  Normally all files are block buffered. if a stream refers to a  terminal
   *  (as stdout normally does) it is line buffered. The standard error  stream
   *  stderr is always unbuffered by default. This function allows to set the
   *  mode of the stream to unbuffered (if buf is NULL) or block buffered.
   *  @param buf pointer to buffer of size BUFSIZ as declared in cstdio, or NULL
   *  @return 0 upon success, nonzero otherwise, in which case the error code may be set
   */
  void setbuf(char *buf) { STDIO_NAMESPACE setbuf(file_, buf); }

  /** The three types of buffering available are unbuffered, block buffered, and
   *  line buffered. When an output stream is unbuffered, information appears on
   *  the destination file or terminal as soon as written; when it is block
   *  buffered many characters are saved up and written as a block; when it is
   *  line buffered characters are saved up until a newline is output or input
   *  is read from any stream attached to a terminal device (typically stdin).
   *  Normally all files are block buffered. if a stream refers to a  terminal
   *  (as stdout normally does) it is line buffered. The standard error  stream
   *  stderr is always unbuffered by default. This function allows to set the
   *  stream mode.
   *  @param buf pointer to buffer, may be NULL
   *  @param modes _IONBF (unbuffered) _IOLBF (line buffered) or _IOFBF (fully buffered)
   *  @param n size of buffer, in bytes
   *  @return 0 upon success, nonzero otherwise, in which case the error code may be set
   */
  int setvbuf(char * buf, int modes, size_t n)
  {
    int result = STDIO_NAMESPACE setvbuf(file_, buf, modes, n);
    if (result) storeLastError();
    return result;
  }

  /** The three types of buffering available are unbuffered, block buffered, and
   *  line buffered. When an output stream is unbuffered, information appears on
   *  the destination file or terminal as soon as written; when it is block
   *  buffered many characters are saved up and written as a block; when it is
   *  line buffered characters are saved up until a newline is output or input
   *  is read from any stream attached to a terminal device (typically stdin).
   *  Normally all files are block buffered. if a stream refers to a  terminal
   *  (as stdout normally does) it is line buffered. The standard error  stream
   *  stderr is always unbuffered by default. This function allows to set the
   *  mode of the stream to unbuffered (if buf is NULL) or block buffered.
   *  @param buf pointer to buffer
   *  @param size size of buffer, in bytes
   *  @return 0 upon success, nonzero otherwise, in which case the error code may be set
   */
  void setbuffer(char *buf, size_t size)
  {
#if defined(_WIN32) || defined(__hpux)
    this->setvbuf(NULL, buf ? _IOFBF : _IONBF, size);
#else
    :: setbuffer(file_, buf, size);
#endif
  }

  /** writes the character c, cast to an unsigned char, to stream.
   *  @param c character
   *  @return the character written as an unsigned char cast to an int or EOF on error
   */
  int fputc(int c) { return STDIO_NAMESPACE fputc(c, file_); }

  /** reads in at most one less than n characters from stream and stores them
   *  into the buffer pointed to by s. Reading stops after an EOF or a newline.
   *  If a newline is read, it is stored into the buffer. A '@\0' is stored after
   *  the last character in the buffer.
   *  @param s pointer to buffer of size n
   *  @param n buffer size
   *  @return pointer to string
   */
  char *fgets(char *s, int n) { return STDIO_NAMESPACE fgets(s, n, file_); }

  /** writes the string s to stream, without its trailing '@\0'.
   *  @param s string to be written
   *  @return a non-negative number on success, or EOF on error.
   */
  int fputs(const char *s) { return STDIO_NAMESPACE fputs(s, file_); }

  /** pushes c back to stream, cast to unsigned char, where it is available for
   *  subsequent read operations. Pushed - back characters will be returned in
   *  reverse order; only one pushback is guaranteed.
   *  @param c character to push back
   *  @return c on success, or EOF on error.
   */
  int ungetc(int c) { return STDIO_NAMESPACE ungetc(c, file_); }

  /** sets the file position indicator for the stream pointed to by stream. The
   *  new position, measured in bytes, is obtained by adding offset bytes to the
   *  position specified by whence. If whence is set to SEEK_SET, SEEK_CUR, or
   *  SEEK_END, the offset is relative to the start of the file, the current
   *  position indicator, or end-of-file, respectively. A successful call to the
   *  fseek function clears the end-of- file indicator for the stream and undoes
   *  any effects of the ungetc function on the same stream.
   *  @param off offset to seek to
   *  @param whence SEEK_SET, SEEK_CUR, or SEEK_END
   *  @return 0 upon success, -1 otherwise in which case the error code is set.
   */
  int fseek(offile_off_t off, int whence)
  {
    int result;
#ifdef _WIN32
    // Windows does not have a 64-bit fseek.
    // We emulate fseek through fsetpos, which does exist on Windows.
    // fpos_t is (hopefully always) defined as __int64 on this platform
    offile_fpos_t off2 = off;
    fpos_t pos;
    struct _stati64 buf;
    switch (whence)
    {
      case SEEK_END:
        // flush write buffer, if any, so that the file size is correct
        STDIO_NAMESPACE fflush(file_);
#if 0
        // Python implementation based on _lseeki64(). May be unsafe because
        // there is no guarantee that fflush also empties read buffers.
        STDIO_NAMESPACE fflush(file_);
#ifdef fileno
        if (_lseeki64(   fileno(file_), 0, 2) == -1)
#else
        if (_lseeki64(:: fileno(file_), 0, 2) == -1)
#endif
        {
          storeLastError();
          return -1;
        }
        // fall through
#else
        // determine file size (using underlying file descriptor). This should be safe.
#ifdef fileno
        if (_fstati64(   fileno(file_), &buf) == -1)
#else
        if (_fstati64(:: fileno(file_), &buf) == -1)
#endif
        {
          storeLastError();
          return -1;
        }

        // fsetpos position is offset + file size.
        off2 += buf.st_size;
        break;
#endif
      case SEEK_CUR:
        if (STDIO_NAMESPACE fgetpos(file_, &pos) != 0)
        {
          storeLastError();
          return -1;
        }

        off2 += pos;
        break;
      case SEEK_SET:
        /* do nothing */
        break;
    }
    result = this->fsetpos(&off2);
#elif defined(__BEOS__)
    result = :: _fseek(file_, off, whence);
#else
#ifdef HAVE_FSEEKO
#ifdef EXPLICIT_LFS_64
    result = :: fseeko64(file_, off, whence);
#else
    result = :: fseeko(file_, off, whence);
#endif
#else
    result = STDIO_NAMESPACE fseek(file_, off, whence);
#endif
#endif
    if (result) storeLastError();
    return result;
  }

  /** obtains the current value of the file position indicator for the stream pointed to by the stream.
   *  @return current file position
   */
  offile_off_t ftell()
  {
    offile_off_t result;
#ifdef _WIN32
    // Windows does not have a 64-bit ftell, and _telli64 cannot be used
    // because it operates on file descriptors and ignores FILE buffers.
    // We emulate ftell through fgetpos, which does exist on Windows.
    // fpos_t is (hopefully always) defined as __int64 on this platform.
    offile_fpos_t pos;
    if (this->fgetpos(&pos) != 0)
    {
      storeLastError();
      return -1;
    }
    return pos;
#else
#ifdef HAVE_FSEEKO
#ifdef EXPLICIT_LFS_64
    result = :: ftello64(file_);
#else
    result = :: ftello(file_);
#endif
#else
    result = STDIO_NAMESPACE ftell(file_);
#endif
#endif
    if (result < 0) storeLastError();
    return result;
  }

  /** alternate interface equivalent to ftell, storing the current value of the
   *  file offset into the object referenced by pos. On some non-UNIX systems an
   *  fpos_t object may be a complex object and these routines may be the only
   *  way to portably reposition a text stream.
   *  @param pos pointer to offile_fpos_t structure
   *  @return 0 upon success, -1 otherwise in which case the error code is set.
   */
  int fgetpos(offile_fpos_t *pos)
  {
    int result;
#if defined(EXPLICIT_LFS_64) && ! defined(__MINGW32__) && ! defined(__QNX__)
    // MinGW and QNX have EXPLICIT_LFS_64 but no fgetpos64()
    result = :: fgetpos64(file_, pos);
#else
    result = STDIO_NAMESPACE fgetpos(file_, pos);
#endif
    if (result) storeLastError();
    return result;
  }

  /** alternate interface equivalent to fseek (with whence set to SEEK_SET),
   *  setting the current value of the file offset from the object referenced by
   *  pos. On some non-UNIX systems an fpos_t object may be a complex object and
   *  these routines may be the only way to portably reposition a text stream.
   *  @param pos pointer to offile_fpos_t structure
   *  @return 0 upon success, -1 otherwise in which case the error code is set.
   */
  int fsetpos(offile_fpos_t *pos)
  {
    int result;
#if defined(EXPLICIT_LFS_64) && ! defined(__MINGW32__) && ! defined(__QNX__)
    // MinGW and QNX have EXPLICIT_LFS_64 but no fsetpos64()
    result = :: fsetpos64(file_, pos);
#else
    result = STDIO_NAMESPACE fsetpos(file_, pos);
#endif
    if (result) storeLastError();
    return result;
  }

  /** print formatted string into stream, see printf(3)
   *  @param format format string
   *  @param ... further parameters according to format string
   *  @return number of characters printed
   */
  int fprintf(const char *format, ...)
  {
    int result = 0;
    va_list ap;
    va_start(ap, format);
    result = STDIO_NAMESPACE vfprintf(file_, format, ap);
    va_end(ap);
    return result;
  }

  /** print formatted string into stream, see printf(3)
   *  @param format format string
   *  @param arg list of further parameters according to format string
   *  @return number of characters printed
   */
   int vfprintf(const char *format, va_list arg)
   {
     return STDIO_NAMESPACE vfprintf(file_, format, arg);
   }

  // we cannot emulate fscanf because we would need vfscanf for this
  // purpose, which does not exist, e.g. on Win32.

  /** return FILE pointer managed by this object. This allows the user
   *  to call some stdio functions that are not encapsulated in this class
   *  (but possibly should be).
   *  @return pointer to FILE structure managed by this object
   */
  FILE *file() { return file_; }

  /** return true if this object is currently associated with a stream, false otherwise
   *  @return true if this object is currently associated with a stream, false otherwise
   */
  OFBool open() const { return file_ != NULL; }

  /** return last error code for this stream
   *  @return last error code for this stream
   */
  offile_errno_t getLastError() const { return lasterror_; }

  /** return string describing last error code for this stream
   *  @param s string describing last error code for this stream returned in this parameter
   */
  void getLastErrorString(OFString& s) const
  {
    char buf[1000];
    s = OFStandard::strerror(lasterror_, buf, 1000);
  }

// wide character functions (disabled by default, since currently not used within DCMTK)
#ifdef WIDE_CHAR_FILE_IO_FUNCTIONS

  /** When mode is zero, the fwide function determines the current orientation
   *  of stream. It returns a value > 0 if stream is wide-character oriented,
   *  i.e.  if wide character I/O is permitted but char I/O is disallowed. It
   *  returns a  value < 0 if stream is byte oriented, i.e. if char I/O is
   *  permitted but wide  character I/O is disallowed. It returns zero if stream
   *  has no orientation yet;  in this case the next I/O operation might change
   *  the orientation (to byte  oriented if it is a char I/O operation, or to
   *  wide-character oriented if it  is a wide character I/O operation).
   *  Once a stream has an orientation, it cannot be changed and persists until
   *  the stream is closed.
   *  When mode is non-zero, the fwide function first attempts to set stream's
   *  orientation (to wide-character oriented if mode > 0, or to byte oriented
   *  if mode < 0). It then returns a value denoting the current orientation, as
   *  above.
   *  @param mode mode of operation for fwide
   *  @return orientation of stream
   */
  int fwide(int mode)
  {
    return STDIO_NAMESPACE fwide(file_, mode);
  }

  /** reads a wide character from stream and returns it. If the end of stream is
   *  reached, or if ferror(stream) becomes true, it returns WEOF. If a wide
   *  character conversion error occurs, it sets the error code to EILSEQ and returns
   *  WEOF.
   *  @return next character from stream or WEOF
   */
  wint_t fgetwc()
  {
    wint_t result = STDIO_NAMESPACE fgetwc(file_);
    if (result == WEOF) storeLastError();
    return result;
  }

  /** writes the wide character wc to stream. If ferror(stream) becomes true, it returns WEOF.
   *  If a wide character conversion error occurs, it sets the error code to EILSEQ and returns WEOF.
   *  Otherwise it returns wc.
   *  @param wc wide character to write to stream
   *  @return character written or WEOF
   */
  wint_t fputwc(wchar_t wc)
  {
    wint_t result = STDIO_NAMESPACE fputwc(wc, file_);
    if (result == WEOF) storeLastError();
    return result;
  }

  /** pushes back a wide character onto stream and returns it. If wc is WEOF, it
   *  returns WEOF. If wc is an invalid wide character, it sets errno to EILSEQ
   *  and returns WEOF. If wc is a valid wide character, it is pushed back  onto
   *  the stream and thus becomes available for future wide character read
   *  operations. The file-position indicator is decremented by one or more.
   *  The end-of-file indicator is cleared. The backing storage of the file is
   *  not affected. Note: wc need not be the last wide character read from the
   *  stream; it can be any other valid wide character. If the implementation
   *  supports multiple push-back operations in a row, the pushed-back wide
   *  characters will be read in reverse order; however, only one level of
   *  push-back is guaranteed.
   *  @param wc wide character to put back to stream
   *  @return character put back or WEOF
   */
  wint_t ungetwc(wint_t wc)
  {
    wint_t result = STDIO_NAMESPACE ungetwc(wc, file_);
    if (result == WEOF) storeLastError();
    return result;
  }

  /** print formatted wide string into stream, see wprintf(3)
   *  @param format format string
   *  @param ... further parameters according to format string
   *  @return number of characters printed
   */
  int fwprintf(const wchar_t *format, ...)
  {
    int result = 0;
    va_list ap;
    va_start(ap, format);
    result = STDIO_NAMESPACE vfwprintf(file_, format, ap);
    va_end(ap);
    return result;
  }

  /** print formatted wide string into stream, see printf(3)
   *  @param format format string
   *  @param arg list of further parameters according to format string
   *  @return number of characters printed
   */
   int vfwprintf(const wchar_t *format, va_list arg)
   {
     return STDIO_NAMESPACE vfwprintf(file_, format, arg);
   }

  // we cannot emulate fwscanf because we would need vfwscanf for this
  // purpose, which does not exist, e.g. on Win32.

#endif /* WIDE_CHAR_FILE_IO_FUNCTIONS */

private:

  // private undefined copy constructor
  OFFile(const OFFile &arg);

  // private undefined assignment operator
  OFFile &operator=(const OFFile &arg);

  /// the file maintained by this object
  FILE *file_;

  /// a flag indicating whether or not this object was created with popen().
  OFBool popened_;

  /// the last error code for operations of this stream
  offile_errno_t lasterror_;

  /// store last error code. For now we simply store the content of errno.
  inline void storeLastError()
  {
    lasterror_ = errno;
  }

};

#endif

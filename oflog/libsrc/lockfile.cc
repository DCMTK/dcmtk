// -*- C++ -*-
//
//  Copyright (C) 2012, Vaclav Zeman. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modifica-
//  tion, are permitted provided that the following conditions are met:
//
//  1. Redistributions of  source code must  retain the above copyright  notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
//  APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
//  DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
//  OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
//  ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
//  (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_SYS_FILE_H)
#include <sys/file.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_SYS_LOCKING_H)
#include <sys/locking.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_IO_H)
#include <io.h>
#endif
#if defined (_WIN32)
#include <tchar.h>
#include <share.h>
#endif
#include "dcmtk/oflog/config/windowsh.h"

#include <stdexcept>
#include <cerrno>
#include <limits>
#include <cstring>

#include "dcmtk/oflog/helpers/lockfile.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/loglog.h"

#if defined (_WIN32)
#  if _WIN32_WINNT < 0x0501
#    define DCMTK_LOG4CPLUS_USE_WIN32_LOCKING
#  else
#    define DCMTK_LOG4CPLUS_USE_WIN32_LOCKFILEEX
#  endif
#else
#  if defined (O_EXLOCK)
#    define DCMTK_LOG4CPLUS_USE_O_EXLOCK
#  elif defined (DCMTK_LOG4CPLUS_HAVE_FCNTL) && defined (F_SETLKW)
#    define DCMTK_LOG4CPLUS_USE_SETLKW
#  elif defined (DCMTK_LOG4CPLUS_HAVE_LOCKF)
#    define DCMTK_LOG4CPLUS_USE_LOCKF
#  elif defined (DCMTK_LOG4CPLUS_HAVE_FLOCK)
#    define DCMTK_LOG4CPLUS_USE_FLOCK
#  endif
#  if defined (DCMTK_LOG4CPLUS_USE_O_EXLOCK) || defined (DCMTK_LOG4CPLUS_USE_SETLKW) \
    || defined (DCMTK_LOG4CPLUS_USE_LOCKF) || defined (DCMTK_LOG4CPLUS_USE_FLOCK)
#    define DCMTK_LOG4CPLUS_USE_POSIX_LOCKING
#  endif
#endif

#if ! defined (DCMTK_LOG4CPLUS_USE_POSIX_LOCKING) && ! defined (_WIN32)
#error "no usable file locking"
#endif

namespace dcmtk {
namespace log4cplus { namespace helpers {


#if defined (_WIN32)
int const OPEN_FLAGS = _O_RDWR | _O_CREAT /*| _O_TEMPORARY*/ | _O_NOINHERIT;
int const OPEN_SHFLAGS = _SH_DENYNO;
int const OPEN_MODE = _S_IREAD | _S_IWRITE;

namespace
{

static
HANDLE
get_os_HANDLE (int fd, helpers::LogLog & loglog)
{
    HANDLE fh = OFreinterpret_cast(HANDLE, _get_osfhandle (fd));
    if (fh == INVALID_HANDLE_VALUE)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT ("_get_osfhandle() failed: "))
            + convertIntegerToString (errno), true);

    return fh;
}

} // namespace

#elif defined (DCMTK_LOG4CPLUS_USE_POSIX_LOCKING)
int const OPEN_FLAGS = O_RDWR | O_CREAT
#if defined (O_CLOEXEC)
    | O_CLOEXEC
#endif
    ;

mode_t const OPEN_MODE = (S_IRWXU ^ S_IXUSR)
    | (S_IRWXG ^ S_IXGRP)
    | (S_IRWXO ^ S_IXOTH);

#endif

struct LockFile::Impl
{
#if defined (DCMTK_LOG4CPLUS_USE_POSIX_LOCKING) \
    || defined (_WIN32)
    int fd;

#endif
};


//
//
//

LockFile::LockFile (tstring const & lf)
    : lock_file_name (lf)
    , data (new LockFile::Impl)
{
#if defined (DCMTK_LOG4CPLUS_USE_O_EXLOCK)
    data->fd = -1;

#else
    open (OPEN_FLAGS);

#endif
}


LockFile::~LockFile ()
{
    close ();
    delete data;
}


void
LockFile::open (int open_flags) const
{
    LogLog & loglog = getLogLog ();

#if defined (_WIN32)
    data->fd = _sopen (lock_file_name.c_str (), open_flags,
        OPEN_SHFLAGS, OPEN_MODE);
    if (data->fd == -1)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT("could not open or create file "))
            + lock_file_name, true);

#elif defined (DCMTK_LOG4CPLUS_USE_POSIX_LOCKING)
    data->fd = ::open (DCMTK_LOG4CPLUS_TSTRING_TO_STRING (lock_file_name).c_str (),
        open_flags, OPEN_MODE);
    if (data->fd == -1)
        loglog.error (
            tstring (DCMTK_LOG4CPLUS_TEXT ("could not open or create file "))
            + lock_file_name, true);

#if ! defined (O_CLOEXEC) && defined (FD_CLOEXEC)
    int ret = fcntl (data->fd, F_SETFD, FD_CLOEXEC);
    if (ret == -1)
        loglog.warn (
            tstring (DCMTK_LOG4CPLUS_TEXT("could not set FD_CLOEXEC on file "))
            + lock_file_name);

#endif
#endif
}


void
LockFile::close () const
{
#if defined (_WIN32)
    if (data->fd >= 0)
        _close (data->fd);

    data->fd = -1;

#elif defined (DCMTK_LOG4CPLUS_USE_POSIX_LOCKING)
    if (data->fd >= 0)
        ::close (data->fd);

    data->fd = -1;

#endif
}


void
LockFile::lock () const
{
    LogLog & loglog = getLogLog ();
    int ret = 0;
    (void) loglog;
    (void) ret;

#if defined (DCMTK_LOG4CPLUS_USE_WIN32_LOCKFILEEX)
    HANDLE fh = get_os_HANDLE (data->fd, loglog);

    OVERLAPPED overlapped;
    memset (&overlapped, 0, sizeof (overlapped));
    overlapped.hEvent = 0;

    ret = LockFileEx(fh, LOCKFILE_EXCLUSIVE_LOCK, 0,
        (STD_NAMESPACE numeric_limits<DWORD>::max) (),
        (STD_NAMESPACE numeric_limits<DWORD>::max) (), &overlapped);
    if (! ret)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT ("LockFileEx() failed: "))
            + convertIntegerToString (GetLastError ()), true);

#elif defined (DCMTK_LOG4CPLUS_USE_WIN32_LOCKING)
    ret = _locking (data->fd, _LK_LOCK, (STD_NAMESPACE numeric_limits<long>::max) ());
    if (ret != 0)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT ("_locking() failed: "))
            + convertIntegerToString (errno), true);

#elif defined (DCMTK_LOG4CPLUS_USE_O_EXLOCK)
    open (OPEN_FLAGS | O_EXLOCK);

#elif defined (DCMTK_LOG4CPLUS_USE_SETLKW)
    do
    {
        struct flock fl;
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;
        ret = fcntl (data->fd, F_SETLKW, &fl);
        if (ret == -1 && errno != EINTR)
            loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT("fcntl(F_SETLKW) failed: "))
                + convertIntegerToString (errno), true);
    }
    while (ret == -1);

#elif defined (DCMTK_LOG4CPLUS_USE_LOCKF)
    do
    {
        ret = lockf (data->fd, F_LOCK, 0);
        if (ret == -1 && errno != EINTR)
            loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT("lockf() failed: "))
                + convertIntegerToString (errno), true);
    }
    while (ret == -1);

#elif defined (DCMTK_LOG4CPLUS_USE_FLOCK)
    do
    {
        ret = flock (data->fd, LOCK_EX);
        if (ret == -1 && errno != EINTR)
            loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT("flock() failed: "))
                + convertIntegerToString (errno), true);
    }
    while (ret == -1);

#endif
}


void LockFile::unlock () const
{
#if defined (DCMTK_LOG4CPLUS_USE_WIN32_LOCKFILEEX)
    LogLog & loglog = getLogLog ();
    HANDLE fh = get_os_HANDLE (data->fd, loglog);

    int ret = UnlockFile(fh, 0, 0, (STD_NAMESPACE numeric_limits<DWORD>::max) (),
        (STD_NAMESPACE numeric_limits<DWORD>::max) ());
    if (! ret)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT ("UnlockFile() failed: "))
            + convertIntegerToString (GetLastError ()), true);

#elif defined (DCMTK_LOG4CPLUS_USE_WIN32_LOCKING)
    LogLog & loglog = getLogLog ();
    int ret = _locking (data->fd, _LK_UNLCK, (STD_NAMESPACE numeric_limits<long>::max) ());
    if (ret != 0)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT ("_locking() failed: "))
            + convertIntegerToString (errno), true);

#elif defined (DCMTK_LOG4CPLUS_USE_O_EXLOCK)
    close ();

#elif defined (DCMTK_LOG4CPLUS_USE_SETLKW)
    LogLog & loglog = getLogLog ();
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    int ret = fcntl (data->fd, F_SETLKW, &fl);
    if (ret != 0)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT("fcntl(F_SETLKW) failed: "))
            + convertIntegerToString (errno), true);

#elif defined (DCMTK_LOG4CPLUS_USE_LOCKF)
    LogLog & loglog = getLogLog ();
    int ret = lockf (data->fd, F_ULOCK, 0);
    if (ret != 0)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT("lockf() failed: "))
            + convertIntegerToString (errno), true);

#elif defined (DCMTK_LOG4CPLUS_USE_FLOCK)
    LogLog & loglog = getLogLog ();
    int ret = flock (data->fd, LOCK_UN);
    if (ret != 0)
        loglog.error (tstring (DCMTK_LOG4CPLUS_TEXT("flock() failed: "))
            + convertIntegerToString (errno), true);

#endif

}



} } // namespace log4cplus { namespace helpers {
} // end namespace dcmtk

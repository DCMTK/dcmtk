//  Copyright (C) 2010, Vaclav Haisman. All rights reserved.
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

#include "dcmtk/oflog/helpers/snprintf.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/internal/internal.h"
#include <cstdarg>
#include <cstdio>
#if defined (DCMTK_OFLOG_UNICODE)
#include <cwchar>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_STDARG_H)
#  include <stdarg.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_WCHAR_H)
#  include <wchar.h>
#endif
#if defined (DCMTK_LOG4CPLUS_HAVE_STDIO_H)
#  include <stdio.h>
#endif


namespace dcmtk {
namespace log4cplus { namespace helpers {


size_t const START_BUF_SIZE = 512;

#if defined (DCMTK_LOG4CPLUS_USE_POOR_MANS_SNPRINTF)

#if defined (_WIN32)
char const NULL_FILE[] = "NUL";
#else
char const NULL_FILE[] = "/dev/null";
#endif

#endif

namespace
{


#if defined (DCMTK_LOG4CPLUS_USE_POOR_MANS_SNPRINTF)

static inline int vftprintf (STD_NAMESPACE FILE * file, tchar const * fmt,
    va_list args) DCMTK_LOG4CPLUS_FORMAT_ATTRIBUTE (__printf__, 2, 0);

static inline
int
vftprintf (STD_NAMESPACE FILE * file, tchar const * fmt, va_list args)
{
#if defined (DCMTK_OFLOG_UNICODE)
#  if defined (DCMTK_LOG4CPLUS_HAVE_VFWPRINTF_S)
    return vfwprintf_s (file, fmt, args);
#  else
    return STD_NAMESPACE vfwprintf (file, fmt, args);
#  endif
#else
#  if defined (DCMTK_LOG4CPLUS_HAVE_VFPRINTF_S)
    return vfprintf_s (file, fmt, args);
#  else
    return STD_NAMESPACE vfprintf (file, fmt, args);
#  endif
#endif
}


static inline int vstprintf (tchar * dest, size_t dest_size,
    tchar const * fmt, va_list args)
    DCMTK_LOG4CPLUS_FORMAT_ATTRIBUTE (__printf__, 3, 0);

static inline
int
vstprintf (tchar * dest, size_t dest_size, tchar const * fmt,
    va_list args)
{
    int ret;
#if defined (DCMTK_OFLOG_UNICODE)
#  if defined (DCMTK_LOG4CPLUS_HAVE_VSWPRINTF_S)
    ret = vswprintf_s (dest, dest_size, fmt, args);
#  else
    ret = STD_NAMESPACE vswprintf (dest, dest_size, fmt, args);
#  endif
#else
#  if defined (DCMTK_LOG4CPLUS_HAVE_VSPRINTF_S)
    ret = vsprintf_s (dest, dest_size, fmt, args);
#  else
    ret = STD_NAMESPACE vsprintf (dest, fmt, args);
#  endif
#endif

    if (ret >= 0)
        assert (OFstatic_cast(size_t, ret) <= dest_size);
    
    return ret;
}

#else /* ?defined (DCMTK_LOG4CPLUS_USE_POOR_MANS_SNPRINTF) */

static inline int vsntprintf (tchar * dest, size_t dest_size,
    tchar const * fmt, va_list args)
    DCMTK_LOG4CPLUS_FORMAT_ATTRIBUTE (__printf__, 3, 0);

static inline
int
vsntprintf (tchar * dest, size_t dest_size, tchar const * fmt,
    va_list args)
{
    int ret;

#if defined (DCMTK_OFLOG_UNICODE)
#  if defined (DCMTK_LOG4CPLUS_HAVE__VSNWPRINTF_S) && defined (_TRUNCATE)
    ret = _vsnwprintf_s (dest, dest_size, _TRUNCATE, fmt, args);
#  else
    ret = STD_NAMESPACE vswprintf (dest, dest_size, fmt, args);
#  endif
#else
#  if defined (DCMTK_LOG4CPLUS_HAVE_VSNPRINTF_S) && defined (_TRUNCATE)
    ret = vsnprintf_s (dest, dest_size, _TRUNCATE, fmt, args);
#  elif defined (DCMTK_LOG4CPLUS_HAVE__VSNPRINTF_S) && defined (_TRUNCATE)
    ret = _vsnprintf_s (dest, dest_size, _TRUNCATE, fmt, args);
#  elif defined (DCMTK_LOG4CPLUS_HAVE_VSNPRINTF)
    ret = vsnprintf (dest, dest_size, fmt, args);
#  elif defined (DCMTK_LOG4CPLUS_HAVE__VSNPRINTF)
    ret = _vsnprintf (dest, dest_size, fmt, args);
#  else
#    error "None of vsnprintf_s, _vsnprintf_s, vsnprintf or _vsnprintf is available."
#  endif
#endif
    
    return ret;
}
#endif


}


snprintf_buf::snprintf_buf ()
    : buf (START_BUF_SIZE)
{ }


tchar const *
snprintf_buf::print (tchar const * fmt, ...)
{
    assert (fmt);

    va_list args;
    va_start (args, fmt);
    tchar const * ret = print_va_list (fmt, args);
    va_end (args);
    return ret;
}


tchar const *
snprintf_buf::print_va_list(tchar const * fmt, va_list args)
{
    int printed;
    size_t const fmt_len = STD_NAMESPACE char_traits<tchar>::length (fmt);
    size_t buf_size = buf.size ();
    size_t const output_estimate = fmt_len + fmt_len / 2 + 1;
    if (output_estimate > buf_size)
        buf.resize (output_estimate);

#if defined (DCMTK_LOG4CPLUS_USE_POOR_MANS_SNPRINTF)
    STD_NAMESPACE FILE * & fnull = internal::get_ptd ()->fnull;
    if (! fnull)
    {
        fnull = STD_NAMESPACE fopen (NULL_FILE, "wb");
        if (! fnull)
        {
            LogLog::getLogLog ()->error (
                DCMTK_LOG4CPLUS_TEXT ("Could not open NULL_FILE."));
            buf.clear ();
            buf.push_back (0);
            return &buf[0];
        }
    }
   
    printed = vftprintf (fnull, fmt, args);
    if (printed == -1)
    {
        LogLog::getLogLog ()->error (
            DCMTK_LOG4CPLUS_TEXT ("Error printing into NULL_FILE."));
        buf.clear ();
        buf.push_back (0);
        return &buf[0];
    }

    buf.resize (printed + 1);
    int sprinted = vstprintf (&buf[0], buf.size (), fmt, args);
    if (sprinted == -1)
    {
        LogLog::getLogLog ()->error (
            DCMTK_LOG4CPLUS_TEXT ("Error printing into string."));
        buf.clear ();
        buf.push_back (0);
        return &buf[0];
    }
    assert (printed == sprinted);
    
    buf[sprinted] = 0;

#else
    do
    {
        printed = vsntprintf (&buf[0], buf_size - 1, fmt, args);
        if (printed == -1)
        {
            buf_size *= 2;
            buf.resize (buf_size);
        }
        else
            buf[printed] = 0;
    }
    while (printed == -1);

#endif

    return &buf[0];
}


} } // namespace log4cplus { namespace helpers
} // end namespace dcmtk

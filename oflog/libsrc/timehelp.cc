// Module:  Log4CPLUS
// File:    timehelper.cxx
// Created: 4/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2010 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "dcmtk/oflog/helpers/timehelp.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/internal/internal.h"

#include <algorithm>
#include "dcmtk/ofstd/ofvector.h"
#include <iomanip>
#include <cassert>
#include <cerrno>
#if defined (DCMTK_OFLOG_UNICODE)
#include <cwchar>
#endif

#if defined (DCMTK_LOG4CPLUS_HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#if defined(DCMTK_LOG4CPLUS_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#if defined(DCMTK_LOG4CPLUS_HAVE_SYS_TIMEB_H) && defined(DCMTK_LOG4CPLUS_HAVE_FTIME) && !defined (DCMTK_LOG4CPLUS_HAVE_CLOCK_GETTIME) && !defined(DCMTK_LOG4CPLUS_HAVE_GETTIMEOFDAY) && !defined(_WIN32)
#include <sys/timeb.h>
#endif

#if defined(DCMTK_LOG4CPLUS_HAVE_GMTIME_R) && !defined(DCMTK_LOG4CPLUS_SINGLE_THREADED)
#define DCMTK_LOG4CPLUS_NEED_GMTIME_R
#endif

#if defined(DCMTK_LOG4CPLUS_HAVE_LOCALTIME_R) && !defined(DCMTK_LOG4CPLUS_SINGLE_THREADED)
#define DCMTK_LOG4CPLUS_NEED_LOCALTIME_R
#endif

#ifdef MAX
#undef MAX
#endif

#define MAX(a, b) ((a) < (b) ? (b) : (a))


namespace dcmtk {
namespace log4cplus { namespace helpers {

const int ONE_SEC_IN_USEC = 1000000;

#if 0
using STD_NAMESPACE mktime;
using STD_NAMESPACE gmtime;
using STD_NAMESPACE localtime;
#if defined (DCMTK_OFLOG_UNICODE)
using STD_NAMESPACE wcsftime;
#else
using STD_NAMESPACE strftime;
#endif
#endif


//////////////////////////////////////////////////////////////////////////////
// Time ctors
//////////////////////////////////////////////////////////////////////////////

Time::Time()
    : tv_sec(0)
    , tv_usec(0)
{
}


Time::Time(time_t tv_sec_, long tv_usec_)
    : tv_sec(tv_sec_)
    , tv_usec(tv_usec_)
{
    assert (tv_usec < ONE_SEC_IN_USEC);
}


Time::Time(time_t time)
    : tv_sec(time)
    , tv_usec(0)
{
}


Time
Time::gettimeofday()
{
#if defined (DCMTK_LOG4CPLUS_HAVE_CLOCK_GETTIME)
    struct timespec ts;
    int res = clock_gettime (CLOCK_REALTIME, &ts);
    assert (res == 0);
    if (res != 0)
        LogLog::getLogLog ()->error (
            DCMTK_LOG4CPLUS_TEXT("clock_gettime() has failed"), true);

    return Time (ts.tv_sec, ts.tv_nsec / 1000);

#elif defined(DCMTK_LOG4CPLUS_HAVE_GETTIMEOFDAY)
    struct timeval tp;
    ::gettimeofday(&tp, 0);

    return Time(tp.tv_sec, tp.tv_usec);

#elif defined (_WIN32)
    FILETIME ft;
    GetSystemTimeAsFileTime (&ft);

    typedef unsigned __int64 uint64_type;
    uint64_type st100ns
        = uint64_type (ft.dwHighDateTime) << 32
        | ft.dwLowDateTime;

    // Number of 100-ns intervals between UNIX epoch and Windows system time
    // is 116444736000000000.
    uint64_type const offset = uint64_type (116444736) * 1000 * 1000 * 1000;
    uint64_type fixed_time = st100ns - offset;

    return Time (fixed_time / (10 * 1000 * 1000),
        OFstatic_cast(long, fixed_time % (10 * 1000 * 1000) / 10));

#elif defined(DCMTK_LOG4CPLUS_HAVE_FTIME)
    struct timeb tp;
    ftime(&tp);

    return Time(tp.time, tp.millitm * 1000);

#else
#warning "Time::gettimeofday()- low resolution timer: gettimeofday and ftime unavailable"
    return Time(::time(0), 0);
#endif
}


//////////////////////////////////////////////////////////////////////////////
// Time methods
//////////////////////////////////////////////////////////////////////////////

time_t
Time::setTime(tm* t)
{
    time_t time = mktime(t);
    if (time != -1)
        tv_sec = time;

    return time;
}


time_t
Time::getTime() const
{
    return tv_sec;
}


void
Time::gmtime(tm* t) const
{
    time_t clock = tv_sec;
#if defined (DCMTK_LOG4CPLUS_HAVE_GMTIME_S) && defined (_MSC_VER) && _MSC_VER > 1200
    gmtime_s (t, &clock);
#elif defined (DCMTK_LOG4CPLUS_HAVE_GMTIME_S) && defined (__BORLANDC__)
    gmtime_s (&clock, t);
#elif defined (DCMTK_LOG4CPLUS_NEED_GMTIME_R)
    gmtime_r (&clock, t);
#else
    tm* tmp = ::gmtime(&clock);
    *t = *tmp;
#endif
}


void
Time::localtime(tm* t) const
{
    time_t clock = tv_sec;
#ifdef DCMTK_LOG4CPLUS_NEED_LOCALTIME_R
    ::localtime_r(&clock, t);
#else
    tm* tmp = ::localtime(&clock);
    *t = *tmp;
#endif
}


namespace
{


static log4cplus::tstring const padding_zeros[4] =
{
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT("000")),
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT("00")),
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT("0")),
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT(""))
};


static log4cplus::tstring const uc_q_padding_zeros[4] =
{
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT(".000")),
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT(".00")),
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT(".0")),
    log4cplus::tstring (DCMTK_LOG4CPLUS_TEXT("."))
};


static
void
build_q_value (log4cplus::tstring & q_str, long tv_usec)
{
    convertIntegerToString(q_str, tv_usec / 1000);
    size_t const len = q_str.length();
    if (len <= 2)
        q_str.insert (0, padding_zeros[q_str.length()]);
}


static
void
build_uc_q_value (log4cplus::tstring & uc_q_str, long tv_usec,
    log4cplus::tstring & tmp)
{
    build_q_value (uc_q_str, tv_usec);

    convertIntegerToString(tmp, tv_usec % 1000);
    size_t const usecs_len = tmp.length();
    tmp.insert (0, usecs_len <= 3
        ? uc_q_padding_zeros[usecs_len] : uc_q_padding_zeros[3]);
    uc_q_str.append (tmp);
}


} // namespace


log4cplus::tstring
Time::getFormattedTime(const log4cplus::tstring& fmt_orig, bool use_gmtime) const
{
    if (fmt_orig.empty () || fmt_orig[0] == 0)
        return log4cplus::tstring ();

    tm time = {};

    if (use_gmtime)
        gmtime(&time);
    else
        localtime(&time);

    enum State
    {
        TEXT,
        PERCENT_SIGN
    };

    internal::gft_scratch_pad & gft_sp = internal::get_gft_scratch_pad ();
    gft_sp.reset ();

    gft_sp.fmt.assign (fmt_orig);
    gft_sp.ret.reserve (OFstatic_cast(size_t, OFstatic_cast(double, gft_sp.fmt.size ()) * 1.35));
    State state = TEXT;

    // Walk the format string and process all occurrences of %q and %Q.

    for (log4cplus::tstring::const_iterator fmt_it = gft_sp.fmt.begin ();
         fmt_it != gft_sp.fmt.end (); ++fmt_it)
    {
        switch (state)
        {
        case TEXT:
        {
            if (*fmt_it == DCMTK_LOG4CPLUS_TEXT ('%'))
                state = PERCENT_SIGN;
            else
                gft_sp.ret.append(1, *fmt_it);
        }
        break;

        case PERCENT_SIGN:
        {
            switch (*fmt_it)
            {
            case DCMTK_LOG4CPLUS_TEXT ('q'):
            {
                if (! gft_sp.q_str_valid)
                {
                    build_q_value (gft_sp.q_str, tv_usec);
                    gft_sp.q_str_valid = true;
                }
                gft_sp.ret.append (gft_sp.q_str);
                state = TEXT;
            }
            break;

            case DCMTK_LOG4CPLUS_TEXT ('Q'):
            {
                if (! gft_sp.uc_q_str_valid)
                {
                    build_uc_q_value (gft_sp.uc_q_str, tv_usec, gft_sp.tmp);
                    gft_sp.uc_q_str_valid = true;
                }
                gft_sp.ret.append (gft_sp.uc_q_str);
                state = TEXT;
            }
            break;

            // Windows do not support %s format specifier
            // (seconds since epoch).
            case DCMTK_LOG4CPLUS_TEXT ('s'):
            {
                if (! gft_sp.s_str_valid)
                {
                    convertIntegerToString (gft_sp.s_str, tv_sec);
                    gft_sp.s_str_valid = true;
                }
                gft_sp.ret.append (gft_sp.s_str);
                state = TEXT;
            }
            break;

            default:
            {
                gft_sp.ret.append (1, DCMTK_LOG4CPLUS_TEXT ('%'));
                gft_sp.ret.append (1, *fmt_it);
                state = TEXT;
            }
            }
        }
        break;
        }
    }

    // Finally call strftime/wcsftime to format the rest of the string.

    gft_sp.ret.swap (gft_sp.fmt);
    size_t buffer_size = gft_sp.fmt.size () + 1;
    size_t len;

    // Limit how far can the buffer grow. This is necessary so that we
    // catch bad format string. Some implementations of strftime() signal
    // both too small buffer and invalid format string by returning 0
    // without changing errno.
    size_t const buffer_size_max
        = MAX(OFstatic_cast(size_t, 1024), buffer_size * 16);

    do
    {
        gft_sp.buffer.resize (buffer_size);
        errno = 0;
#ifdef DCMTK_OFLOG_UNICODE
        len = wcsftime(&gft_sp.buffer[0], buffer_size,
            gft_sp.fmt.c_str(), &time);
#else
        len = strftime(&gft_sp.buffer[0], buffer_size,
            gft_sp.fmt.c_str(), &time);
#endif
        if (len == 0)
        {
            int const eno = errno;
            buffer_size *= 2;
            if (buffer_size > buffer_size_max)
            {
                LogLog::getLogLog ()->error (
                    DCMTK_LOG4CPLUS_TEXT("Error in strftime(): ")
                    + convertIntegerToString (eno), true);
            }
        }
    }
    while (len == 0);

    return tstring (&*gft_sp.buffer.begin (), len);
}


Time&
Time::operator+=(const Time& rhs)
{
    tv_sec += rhs.tv_sec;
    tv_usec += rhs.tv_usec;

    if(tv_usec > ONE_SEC_IN_USEC) {
        ++tv_sec;
        tv_usec -= ONE_SEC_IN_USEC;
    }

    return *this;
}


Time&
Time::operator-=(const Time& rhs)
{
    tv_sec -= rhs.tv_sec;
    tv_usec -= rhs.tv_usec;

    if(tv_usec < 0) {
        --tv_sec;
        tv_usec += ONE_SEC_IN_USEC;
    }

    return *this;
}


Time&
Time::operator/=(long rhs)
{
    long rem_secs = OFstatic_cast(long, tv_sec % rhs);
    tv_sec /= rhs;

    tv_usec /= rhs;
    tv_usec += OFstatic_cast(long, (rem_secs * ONE_SEC_IN_USEC) / rhs);

    return *this;
}


Time&
Time::operator*=(long rhs)
{
    long new_usec = tv_usec * rhs;
    long overflow_sec = new_usec / ONE_SEC_IN_USEC;
    tv_usec = new_usec % ONE_SEC_IN_USEC;

    tv_sec *= rhs;
    tv_sec += overflow_sec;

    return *this;
}


//////////////////////////////////////////////////////////////////////////////
// Time globals
//////////////////////////////////////////////////////////////////////////////


const Time
operator+(const Time& lhs, const Time& rhs)
{
    return Time(lhs) += rhs;
}


const Time
operator-(const Time& lhs, const Time& rhs)
{
    return Time(lhs) -= rhs;
}


const Time
operator/(const Time& lhs, long rhs)
{
    return Time(lhs) /= rhs;
}


const Time
operator*(const Time& lhs, long rhs)
{
    return Time(lhs) *= rhs;
}


bool
operator<(const Time& lhs, const Time& rhs)
{
    return (   (lhs.sec() < rhs.sec())
            || (   (lhs.sec() == rhs.sec())
                && (lhs.usec() < rhs.usec())) );
}


bool
operator<=(const Time& lhs, const Time& rhs)
{
    return ((lhs < rhs) || (lhs == rhs));
}


bool
operator>(const Time& lhs, const Time& rhs)
{
    return (   (lhs.sec() > rhs.sec())
            || (   (lhs.sec() == rhs.sec())
                && (lhs.usec() > rhs.usec())) );
}


bool
operator>=(const Time& lhs, const Time& rhs)
{
    return ((lhs > rhs) || (lhs == rhs));
}


bool
operator==(const Time& lhs, const Time& rhs)
{
    return (   lhs.sec() == rhs.sec()
            && lhs.usec() == rhs.usec());
}


bool
operator!=(const Time& lhs, const Time& rhs)
{
    return !(lhs == rhs);
}


} } // namespace log4cplus { namespace helpers {
} // end namespace dcmtk

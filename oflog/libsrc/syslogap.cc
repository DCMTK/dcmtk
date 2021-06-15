// Module:  Log4CPLUS
// File:    syslogappender.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
// Copyright 2001-2010 Tad E. Smith
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

#include "dcmtk/oflog/syslogap.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/logmacro.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/internal/internal.h"
#include "dcmtk/oflog/internal/env.h"
#include "dcmtk/oflog/thread/syncpub.h"
#include <cstring>

#if defined (DCMTK_LOG4CPLUS_HAVE_SYSLOG_H)
#include <syslog.h>

#else // DCMTK_LOG4CPLUS_HAVE_SYSLOG_H

// The following bits were derived from SUSv4 documentation and
// RFC5424 document.

// priority codes
#define LOG_EMERG   0
#define LOG_ALERT   1
#define LOG_CRIT    2
#define LOG_ERR     3
#define LOG_WARNING 4
#define LOG_NOTICE  5
#define LOG_INFO    6
#define LOG_DEBUG   7

// facility codes
#define LOG_KERN      (0 << 3)
#define LOG_USER      (1 << 3)
#define LOG_MAIL      (2 << 3)
#define LOG_DAEMON    (3 << 3)
#define LOG_AUTH      (4 << 3)
#define LOG_SYSLOG    (5 << 3)
#define LOG_LPR       (6 << 3)
#define LOG_NEWS      (7 << 3)
#define LOG_UUCP      (8 << 3)
#define LOG_CRON      (9 << 3)
#define LOG_AUTHPRIV (10 << 3)
#define LOG_FTP      (11 << 3)
#define LOG_NTP      (12 << 3)
#define LOG_SECURITY (13 << 3)
#define LOG_CONSOLE  (14 << 3)
// (15 << 3) is missing here
#define LOG_LOCAL0   (16 << 3)
#define LOG_LOCAL1   (17 << 3)
#define LOG_LOCAL2   (18 << 3)
#define LOG_LOCAL3   (19 << 3)
#define LOG_LOCAL4   (20 << 3)
#define LOG_LOCAL5   (21 << 3)
#define LOG_LOCAL6   (22 << 3)
#define LOG_LOCAL7   (23 << 3)

#endif // DCMTK_LOG4CPLUS_HAVE_SYSLOG_H


namespace dcmtk
{
namespace log4cplus
{

namespace
{

#ifdef LOG_USER
int const fallback_facility = LOG_USER;

#else
int const fallback_facility = 0;

#endif


static
int
parseFacility (const tstring& text)
{
    if (text.empty ())
        return fallback_facility;
#ifdef LOG_AUTH
    else if (text == DCMTK_LOG4CPLUS_TEXT ("auth"))
        return LOG_AUTH;
#endif
#ifdef LOG_AUTHPRIV
    else if (text == DCMTK_LOG4CPLUS_TEXT ("authpriv"))
        return LOG_AUTHPRIV;
#endif
#ifdef LOG_CONSOLE
    else if (text == DCMTK_LOG4CPLUS_TEXT ("console"))
        return LOG_CONSOLE;
#endif
#ifdef LOG_CRON
    else if (text == DCMTK_LOG4CPLUS_TEXT ("cron"))
        return LOG_CRON;
#endif
#ifdef LOG_DAEMON
    else if (text == DCMTK_LOG4CPLUS_TEXT ("daemon"))
        return LOG_DAEMON;
#endif
#ifdef LOG_FTP
    else if (text == DCMTK_LOG4CPLUS_TEXT ("ftp"))
        return LOG_FTP;
#endif
#ifdef LOG_KERN
    else if (text == DCMTK_LOG4CPLUS_TEXT ("kern"))
        return LOG_KERN;
#endif
#ifdef LOG_LOCAL0
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local0"))
        return LOG_LOCAL0;
#endif
#ifdef LOG_LOCAL1
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local1"))
        return LOG_LOCAL1;
#endif
#ifdef LOG_LOCAL2
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local2"))
        return LOG_LOCAL2;
#endif
#ifdef LOG_LOCAL3
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local3"))
        return LOG_LOCAL3;
#endif
#ifdef LOG_LOCAL4
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local4"))
        return LOG_LOCAL4;
#endif
#ifdef LOG_LOCAL5
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local5"))
        return LOG_LOCAL5;
#endif
#ifdef LOG_LOCAL6
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local6"))
        return LOG_LOCAL6;
#endif
#ifdef LOG_LOCAL7
    else if (text == DCMTK_LOG4CPLUS_TEXT ("local7"))
        return LOG_LOCAL7;
#endif
#ifdef LOG_LPR
    else if (text == DCMTK_LOG4CPLUS_TEXT ("lpr"))
        return LOG_LPR;
#endif
#ifdef LOG_MAIL
    else if (text == DCMTK_LOG4CPLUS_TEXT ("mail"))
        return LOG_MAIL;
#endif
#ifdef LOG_NEWS
    else if (text == DCMTK_LOG4CPLUS_TEXT ("news"))
        return LOG_NEWS;
#endif
#ifdef LOG_NTP
    else if (text == DCMTK_LOG4CPLUS_TEXT ("ntp"))
        return LOG_NTP;
#endif
#ifdef LOG_SECURITY
    else if (text == DCMTK_LOG4CPLUS_TEXT ("security"))
        return LOG_SECURITY;
#endif
#ifdef LOG_SYSLOG
    else if (text == DCMTK_LOG4CPLUS_TEXT ("syslog"))
        return LOG_SYSLOG;
#endif
#ifdef LOG_USER
    else if (text == DCMTK_LOG4CPLUS_TEXT ("user"))
        return LOG_USER;
#endif
#ifdef LOG_UUCP
    else if (text == DCMTK_LOG4CPLUS_TEXT ("uucp"))
        return LOG_UUCP;
#endif
    else
    {
        // Unknown facility.
        tstring msg (DCMTK_LOG4CPLUS_TEXT ("Unknown syslog facility: "));
        msg += text;
        helpers::getLogLog ().error (msg);

        return fallback_facility;
    }
}


} // namespace


///////////////////////////////////////////////////////////////////////////////
// SysLogAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

#if defined (DCMTK_LOG4CPLUS_HAVE_SYSLOG_H)
static const char*
useIdent (const tstring& string)
{
    return string.empty() ? 0 : string.c_str();
}

SysLogAppender::SysLogAppender(const tstring& id)
    : ident(id)
    , appendFunc (&SysLogAppender::appendLocal)
    , facility (0)
    , host ()
    , port (0)
    , syslogSocket ()
    // Store STD_NAMESPACE string form of ident as member of SysLogAppender so
    // the address of the c_str() result remains stable for openlog &
    // co to use even if we use wstrings.
    , identStr(DCMTK_LOG4CPLUS_TSTRING_TO_STRING (id) )
    , hostname (helpers::getHostname (true))
{
    ::openlog(useIdent(identStr), 0, 0);
}

#endif


SysLogAppender::SysLogAppender(const helpers::Properties & properties)
    : Appender(properties)
    , ident ()
    , appendFunc (0)
    , facility (0)
    , host ()
    , port (0)
    , syslogSocket ()
    , identStr ()
    , hostname (helpers::getHostname (true))
{
    ident = properties.getProperty( DCMTK_LOG4CPLUS_TEXT("ident") );
    facility = parseFacility (
        helpers::toLower (
            properties.getProperty (DCMTK_LOG4CPLUS_TEXT ("facility"))));
    identStr = DCMTK_LOG4CPLUS_TSTRING_TO_STRING (ident);

    host = properties.getProperty (DCMTK_LOG4CPLUS_TEXT ("host"));
    if (host.empty ())
    {
#if defined (DCMTK_LOG4CPLUS_HAVE_SYSLOG_H)
        appendFunc = &SysLogAppender::appendLocal;
        ::openlog(useIdent(identStr), 0, 0);

#else
        helpers::getLogLog ().error (
            DCMTK_LOG4CPLUS_TEXT ("SysLogAppender")
            DCMTK_LOG4CPLUS_TEXT ("- local syslog not available"), true);

#endif
    }
    else
    {
        if (! properties.getInt (port, DCMTK_LOG4CPLUS_TEXT ("port")))
            port = 514;

        appendFunc = &SysLogAppender::appendRemote;
        syslogSocket = helpers::Socket (host, OFstatic_cast(unsigned short, port), true);
    }
}


SysLogAppender::SysLogAppender(const tstring& id, const tstring & h,
    int p, const tstring & f)
    : ident (id)
    , appendFunc (&SysLogAppender::appendRemote)
    , facility (parseFacility (helpers::toLower (f)))
    , host (h)
    , port (p)
    , syslogSocket (host, OFstatic_cast(unsigned short, port), true)
    // Store STD_NAMESPACE string form of ident as member of SysLogAppender so
    // the address of the c_str() result remains stable for openlog &
    // co to use even if we use wstrings.
    , identStr(DCMTK_LOG4CPLUS_TSTRING_TO_STRING (id) )
    , hostname (helpers::getHostname (true))
{ }


SysLogAppender::~SysLogAppender()
{
    destructorImpl();
}



///////////////////////////////////////////////////////////////////////////////
// SysLogAppender public methods
///////////////////////////////////////////////////////////////////////////////

void
SysLogAppender::close()
{
    helpers::getLogLog().debug(
        DCMTK_LOG4CPLUS_TEXT("Entering SysLogAppender::close()..."));
    thread::MutexGuard guard (access_mutex);

    if (host.empty ())
    {
#if defined (DCMTK_LOG4CPLUS_HAVE_SYSLOG_H)
        ::closelog();
#endif
    }
    else
        syslogSocket.close ();

    closed = true;
}



///////////////////////////////////////////////////////////////////////////////
// SysLogAppender protected methods
///////////////////////////////////////////////////////////////////////////////

int
SysLogAppender::getSysLogLevel(const LogLevel& ll) const
{
    if(ll < INFO_LOG_LEVEL /* || ll < DEBUG_LOG_LEVEL*/) {
        return LOG_DEBUG;
    }
    else if(ll < WARN_LOG_LEVEL) {
        return LOG_INFO;
    }
    else if(ll < ERROR_LOG_LEVEL) {
        return LOG_WARNING;
    }
    else if(ll < FATAL_LOG_LEVEL) {
        return LOG_ERR;
    }
    else if(ll == FATAL_LOG_LEVEL) {
        return LOG_CRIT;
    }

    return LOG_ALERT;  // ll > FATAL_LOG_LEVEL
}


// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void
SysLogAppender::append(const spi::InternalLoggingEvent& event)
{
    (this->*appendFunc) (event);
}


#if defined (DCMTK_LOG4CPLUS_HAVE_SYSLOG_H)
void
SysLogAppender::appendLocal(const spi::InternalLoggingEvent& event)
{
    int const level = getSysLogLevel(event.getLogLevel());
    internal::appender_sratch_pad & appender_sp = internal::get_appender_sp ();
    detail::clear_tostringstream (appender_sp.oss);
    layout->formatAndAppend(appender_sp.oss, event);
    appender_sp.str = OFString(appender_sp.oss.str().c_str(), appender_sp.oss.str().length());
    ::syslog(facility | level, "%s",
        DCMTK_LOG4CPLUS_TSTRING_TO_STRING(appender_sp.str).c_str());
}

#endif


tstring const SysLogAppender::remoteTimeFormat (
    DCMTK_LOG4CPLUS_TEXT ("%Y-%m-%dT%H:%M:%S.%qZ"));


void
SysLogAppender::appendRemote(const spi::InternalLoggingEvent& event)
{
    int const level = getSysLogLevel(event.getLogLevel());
    internal::appender_sratch_pad & appender_sp = internal::get_appender_sp ();
    detail::clear_tostringstream (appender_sp.oss);

    appender_sp.oss
        // PRI
        << DCMTK_LOG4CPLUS_TEXT ('<') << (level | facility) << DCMTK_LOG4CPLUS_TEXT ('>')
        // VERSION
        << 1
        // TIMESTAMP
        << DCMTK_LOG4CPLUS_TEXT (' ')
        << event.getTimestamp ().getFormattedTime (remoteTimeFormat, true)
        // HOSTNAME
        << DCMTK_LOG4CPLUS_TEXT (' ') << hostname
        // APP-NAME
        << DCMTK_LOG4CPLUS_TEXT (' ') << ident
        // PROCID
        << DCMTK_LOG4CPLUS_TEXT (' ') << internal::get_process_id ()
        // MSGID
        << DCMTK_LOG4CPLUS_TEXT (' ') << event.getLoggerName ()
        // STRUCTURED-DATA
        // no structured data, it could be whole MDC
        << DCMTK_LOG4CPLUS_TEXT (" - ");

    // MSG
    layout->formatAndAppend (appender_sp.oss, event);

    DCMTK_LOG4CPLUS_TSTRING_TO_STRING (appender_sp.oss.str ())
        .swap (appender_sp.chstr);
    
    bool ret = syslogSocket.write (appender_sp.chstr);
    if (! ret)
    {
        helpers::getLogLog ().warn (
            DCMTK_LOG4CPLUS_TEXT ("SysLogAppender::appendRemote")
            DCMTK_LOG4CPLUS_TEXT ("- socket write failed"));
        syslogSocket = helpers::Socket (host, OFstatic_cast(unsigned short, port), true);
    }
}


} // namespace log4cplus
} // end namespace dcmtk

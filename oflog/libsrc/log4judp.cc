// Module:  Log4CPLUS
// File:    log4judpappender.cxx
// Created: 7/2012
// Author:  Siva Chandran P
//
//
// Copyright 2012 Siva Chandran P
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

#include "dcmtk/oflog/log4judp.h"
#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/logmacro.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/internal/internal.h"
#include "dcmtk/oflog/thread/syncpub.h"
#include <iomanip>
#include <cstring>
#if defined (DCMTK_OFLOG_UNICODE)
#include <cwctype>
#else
#include <cctype>
#endif


namespace dcmtk
{
namespace log4cplus
{

namespace
{


static inline bool
is_control (tchar ch)
{
#if defined (DCMTK_OFLOG_UNICODE)
    return !! STD_NAMESPACE iswcntrl (STD_NAMESPACE char_traits<tchar>::to_int_type (ch));
#elif defined (_MSC_VER) && _MSC_VER <= 1200 /* MSC6 and older */
    return !! iscntrl (STD_NAMESPACE char_traits<tchar>::to_int_type (ch));
#else
    return !! STD_NAMESPACE iscntrl (STD_NAMESPACE char_traits<tchar>::to_int_type (ch));
#endif
}


//! Outputs str with reserved XML characters escaped.
static
void
output_xml_escaped (tostream & os, tstring const & str)
{
    for (tstring::const_iterator it = str.begin (); it != str.end (); ++it)
    {
        tchar const & ch = *it;
        switch (ch)
        {
        case DCMTK_LOG4CPLUS_TEXT ('<'):
            os << DCMTK_LOG4CPLUS_TEXT ("&lt;");
            break;

        case DCMTK_LOG4CPLUS_TEXT ('>'):
            os << DCMTK_LOG4CPLUS_TEXT ("&gt;");
            break;

        case DCMTK_LOG4CPLUS_TEXT ('&'):
            os << DCMTK_LOG4CPLUS_TEXT ("&amp;");
            break;

        case DCMTK_LOG4CPLUS_TEXT ('\''):
            os << DCMTK_LOG4CPLUS_TEXT ("&apos;");
            break;

        case DCMTK_LOG4CPLUS_TEXT ('"'):
            os << DCMTK_LOG4CPLUS_TEXT ("&quot;");
            break;

        default:
            if (is_control (ch))
            {
                tchar const prev_fill = os.fill ();
                STD_NAMESPACE ios_base::fmtflags const prev_flags = os.flags ();
                os.flags (STD_NAMESPACE ios_base::hex | STD_NAMESPACE ios_base::right);
                os.fill (DCMTK_LOG4CPLUS_TEXT ('0'));

                os << STD_NAMESPACE setw (0) << DCMTK_LOG4CPLUS_TEXT ("&#x")
                    << STD_NAMESPACE setw (2) << STD_NAMESPACE char_traits<tchar>::to_int_type (ch)
                    << STD_NAMESPACE setw (0) << DCMTK_LOG4CPLUS_TEXT (";");

                os.fill (prev_fill);
                os.flags (prev_flags);
            }
            else
                os.put (ch);
        }
    }
}


//! Helper manipulator like class for escaped XML output.
struct outputXMLEscaped
{
    outputXMLEscaped (tstring const & s)
        : str (s)
    { }

    tstring const & str;
};


//! Overload stream insertion for outputXMLEscaped.
static
tostream &
operator << (tostream & os, outputXMLEscaped const & x)
{
    output_xml_escaped (os, x.str);
    return os;
}


} // namespace


//////////////////////////////////////////////////////////////////////////////
// Log4jUdpAppender ctors and dtor
//////////////////////////////////////////////////////////////////////////////

Log4jUdpAppender::Log4jUdpAppender(const tstring& host_, int port_)
    : socket()
    , host(host_)
    , port(port_)
{
    layout.reset (new PatternLayout (DCMTK_LOG4CPLUS_TEXT ("%m")));
    openSocket();
}



Log4jUdpAppender::Log4jUdpAppender(const helpers::Properties & properties)
    : Appender(properties)
    , socket()
    , host()
    , port(5000)
{
    host = properties.getProperty( DCMTK_LOG4CPLUS_TEXT("host"),
        DCMTK_LOG4CPLUS_TEXT ("localhost") );
    properties.getInt (port, DCMTK_LOG4CPLUS_TEXT ("port"));

    openSocket();
}



Log4jUdpAppender::~Log4jUdpAppender()
{
    destructorImpl();
}



//////////////////////////////////////////////////////////////////////////////
// Log4jUdpAppender public methods
//////////////////////////////////////////////////////////////////////////////

void
Log4jUdpAppender::close()
{
    helpers::getLogLog().debug(
        DCMTK_LOG4CPLUS_TEXT("Entering Log4jUdpAppender::close()..."));

    socket.close();
    closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// Log4jUdpAppender protected methods
//////////////////////////////////////////////////////////////////////////////

void
Log4jUdpAppender::openSocket()
{
    if(!socket.isOpen()) {
        socket = helpers::Socket(host, OFstatic_cast(unsigned short, port), true);
    }
}

void
Log4jUdpAppender::append(const spi::InternalLoggingEvent& event)
{
    if(!socket.isOpen()) {
        openSocket();
        if(!socket.isOpen()) {
            helpers::getLogLog().error(
                DCMTK_LOG4CPLUS_TEXT("Log4jUdpAppender::append()- Cannot connect to server"));
            return;
        }
    }

    tstring & str = formatEvent (event);

    internal::appender_sratch_pad & appender_sp
        = internal::get_appender_sp ();
    tostringstream & buffer = appender_sp.oss;
    detail::clear_tostringstream (buffer);

    buffer << DCMTK_LOG4CPLUS_TEXT("<log4j:event logger=\"")
           << outputXMLEscaped (event.getLoggerName())
           << DCMTK_LOG4CPLUS_TEXT("\" level=\"")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (getLogLevelManager().toString(event.getLogLevel()))
           << DCMTK_LOG4CPLUS_TEXT("\" timestamp=\"")
           << event.getTimestamp().getFormattedTime(DCMTK_LOG4CPLUS_TEXT("%s%q"))
           << DCMTK_LOG4CPLUS_TEXT("\" thread=\"") << event.getThread()
           << DCMTK_LOG4CPLUS_TEXT("\">")

           << DCMTK_LOG4CPLUS_TEXT("<log4j:message>")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (str)
           << DCMTK_LOG4CPLUS_TEXT("</log4j:message>")

           << DCMTK_LOG4CPLUS_TEXT("<log4j:NDC>")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (event.getNDC())
           << DCMTK_LOG4CPLUS_TEXT("</log4j:NDC>")

           << DCMTK_LOG4CPLUS_TEXT("<log4j:locationInfo class=\"\" file=\"")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (event.getFile())
           << DCMTK_LOG4CPLUS_TEXT("\" method=\"")
           << outputXMLEscaped (event.getFunction())
           << DCMTK_LOG4CPLUS_TEXT("\" line=\"")
           << event.getLine()
           << DCMTK_LOG4CPLUS_TEXT("\"/>")
           << DCMTK_LOG4CPLUS_TEXT("</log4j:event>");

    DCMTK_LOG4CPLUS_TSTRING_TO_STRING (buffer.str ()).swap (appender_sp.chstr);

    bool ret = socket.write(appender_sp.chstr);
    if (!ret)
    {
        helpers::getLogLog().error(
            DCMTK_LOG4CPLUS_TEXT(
                "Log4jUdpAppender::append()- Cannot write to server"));
    }
}

} // namespace log4cplus
} // end namespace dcmtk

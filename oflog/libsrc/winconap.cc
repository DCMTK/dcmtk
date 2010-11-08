//   Copyright (C) 2009, Vaclav Haisman. All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without modifica-
//   tion, are permitted provided that the following conditions are met:
//
//   1. Redistributions of  source code must  retain the above copyright  notice,
//      this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright notice,
//      this list of conditions and the following disclaimer in the documentation
//      and/or other materials provided with the distribution.
//
//   THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//   FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
//   APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
//   INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
//   DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
//   OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
//   ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
//   (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
//   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "dcmtk/oflog/winconap.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/strhelp.h"


#if defined(_WIN32) && defined (LOG4CPLUS_HAVE_WIN32_CONSOLE)
namespace log4cplus
{


Win32ConsoleAppender::Win32ConsoleAppender (bool allocConsole)
    : alloc_console (allocConsole)
{ }


Win32ConsoleAppender::Win32ConsoleAppender (
    helpers::Properties const & properties, tstring& error)
    : Appender (properties)
    , alloc_console (true)
{
    tstring tmp = properties.getProperty (LOG4CPLUS_TEXT ("AllocConsole"),
        LOG4CPLUS_TEXT ("true"));
    alloc_console = helpers::toLower (tmp) == LOG4CPLUS_TEXT ("true");
}


Win32ConsoleAppender::~Win32ConsoleAppender ()
{
    destructorImpl();
}


void
Win32ConsoleAppender::close ()
{
    closed = true;
}


void
Win32ConsoleAppender::append (spi::InternalLoggingEvent const & event)
{
    if (alloc_console)
        // We ignore the return value here. If we already have a console,
        // it will fail.
        AllocConsole ();

    HANDLE const console_out = GetStdHandle (STD_OUTPUT_HANDLE);
    if (console_out == INVALID_HANDLE_VALUE)
    {
        getLogLog ().error (
            LOG4CPLUS_TEXT ("Win32ConsoleAppender::append")
            LOG4CPLUS_TEXT ("- Unable to get STD_OUTPUT_HANDLE."));
        return;
    }

    DWORD const handle_type = GetFileType (console_out);
    if (handle_type == FILE_TYPE_UNKNOWN && GetLastError () != NO_ERROR)
    {
        getLogLog ().error (
            LOG4CPLUS_TEXT ("Win32ConsoleAppender::append")
            LOG4CPLUS_TEXT ("- Error retrieving handle type."));
        return;
    }

    tostringstream oss;
    layout->formatAndAppend (oss, event);
    OFSTRINGSTREAM_GETOFSTRING(oss, str);
    size_t const str_len = str.size ();
    tchar const * s = str.c_str ();

    if (handle_type == FILE_TYPE_CHAR)
        // It seems that we have real console handle here. We can use
        // WriteConsole() directly.
        write_console (console_out, s, str_len);
    else
        // It seems that console is redirected.
        write_handle (console_out, s, str_len);
}


void
Win32ConsoleAppender::write_handle (HANDLE out, tchar const * s, size_t str_len)
{
    char const * const cstr = s;

    DWORD const total_to_write = OFstatic_cast(DWORD, str_len);
    DWORD total_written = 0;

    do
    {
        DWORD const to_write = total_to_write - total_written;
        DWORD written = 0;

        BOOL ret = WriteFile (out, cstr + total_written, to_write, &written,
            0);
        if (! ret)
        {
            getLogLog ().error (
                LOG4CPLUS_TEXT ("Win32ConsoleAppender::write_handle")
                LOG4CPLUS_TEXT ("- WriteFile has failed."));
            return;
        }

        total_written += written;
    }
    while (total_written != total_to_write);
}


void
Win32ConsoleAppender::write_console (HANDLE console_out, tchar const * s,
    size_t str_len)
{
    DWORD const total_to_write = OFstatic_cast(DWORD, str_len);
    DWORD total_written = 0;

    do
    {
        DWORD to_write = total_to_write - total_written;
        DWORD const to_write_max = 64*1024 - 1;
        DWORD written = 0;

        if (to_write > to_write_max)
            to_write = to_write_max;

        BOOL ret = WriteConsole (console_out, s + total_written, to_write,
            &written, 0);
        if (! ret)
        {
            getLogLog ().error (
                LOG4CPLUS_TEXT ("Win32ConsoleAppender::write_console")
                LOG4CPLUS_TEXT ("- WriteConsole has failed."));
            return;
        }

        total_written += written;
    }
    while (total_written != total_to_write);
}



} // namespace log4cplus

#endif

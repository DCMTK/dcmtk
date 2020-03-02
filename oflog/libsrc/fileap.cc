// Module:  Log4CPLUS
// File:    fileappender.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
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

#include "dcmtk/oflog/fileap.h"
#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/timehelp.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/helpers/fileinfo.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/spi/factory.h"
#include "dcmtk/oflog/thread/syncpub.h"
#include "dcmtk/oflog/internal/internal.h"
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <stdexcept>

#if defined (__BORLANDC__)
// For _wrename() and _wremove() on Windows.
#  include <stdio.h>
#endif
#include <cerrno>
#ifdef DCMTK_LOG4CPLUS_HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef MAX
#undef MAX
#endif

#define MAX(a, b) ((a) < (b) ? (b) : (a))

namespace dcmtk
{
namespace log4cplus
{

using helpers::Properties;
using helpers::Time;


const long DEFAULT_ROLLING_LOG_SIZE = 10 * 1024 * 1024L;
const long MINIMUM_ROLLING_LOG_SIZE = 200*1024L;


///////////////////////////////////////////////////////////////////////////////
// File LOCAL definitions
///////////////////////////////////////////////////////////////////////////////

namespace
{

long const DCMTK_LOG4CPLUS_FILE_NOT_FOUND = ENOENT;


static 
long
file_rename (tstring const & src, tstring const & target)
{
#if defined (DCMTK_OFLOG_UNICODE) && defined (_WIN32)
    if (_wrename (src.c_str (), target.c_str ()) == 0)
        return 0;
    else
        return errno;

#else
    if (rename (DCMTK_LOG4CPLUS_TSTRING_TO_STRING (src).c_str (),
            DCMTK_LOG4CPLUS_TSTRING_TO_STRING (target).c_str ()) == 0)
        return 0;
    else
        return errno;

#endif
}


static
long
file_remove (tstring const & src)
{
#if defined (DCMTK_OFLOG_UNICODE) && defined (_WIN32)
    if (_wremove (src.c_str ()) == 0)
        return 0;
    else
        return errno;

#else
    if (remove (DCMTK_LOG4CPLUS_TSTRING_TO_STRING (src).c_str ()) == 0)
        return 0;
    else
        return errno;

#endif
}


static
void
loglog_renaming_result (helpers::LogLog & loglog, tstring const & src,
    tstring const & target, long ret)
{
    if (ret == 0)
    {
        loglog.debug (
            DCMTK_LOG4CPLUS_TEXT("Renamed file ") 
            + src 
            + DCMTK_LOG4CPLUS_TEXT(" to ")
            + target);
    }
    else if (ret != DCMTK_LOG4CPLUS_FILE_NOT_FOUND)
    {
        tostringstream oss;
        oss << DCMTK_LOG4CPLUS_TEXT("Failed to rename file from ")
            << src
            << DCMTK_LOG4CPLUS_TEXT(" to ")
            << target
            << DCMTK_LOG4CPLUS_TEXT("; error ")
            << ret;
        loglog.error (OFString(oss.str().c_str(), oss.str().length()));
    }
}


static
void
loglog_opening_result (helpers::LogLog & loglog,
    log4cplus::tostream const & os, tstring const & filename)
{
    if (! os)
    {
        loglog.error (
            DCMTK_LOG4CPLUS_TEXT("Failed to open file ") 
            + filename);
    }
}


static
void
rolloverFiles(const tstring& filename, unsigned int maxBackupIndex)
{
    helpers::LogLog * loglog = helpers::LogLog::getLogLog();

    // Delete the oldest file
    tostringstream buffer;
    buffer << filename << DCMTK_LOG4CPLUS_TEXT(".") << maxBackupIndex;
    long ret = file_remove (OFString(buffer.str().c_str(), buffer.str().length()));

    tostringstream source_oss;
    tostringstream target_oss;

    // Map {(maxBackupIndex - 1), ..., 2, 1} to {maxBackupIndex, ..., 3, 2}
    for (int i = maxBackupIndex - 1; i >= 1; --i)
    {
        source_oss.str(DCMTK_LOG4CPLUS_TEXT(""));
        target_oss.str(DCMTK_LOG4CPLUS_TEXT(""));

        source_oss << filename << DCMTK_LOG4CPLUS_TEXT(".") << i;
        target_oss << filename << DCMTK_LOG4CPLUS_TEXT(".") << (i+1);

        tstring const source (OFString(source_oss.str().c_str(), source_oss.str().length()));
        tstring const target (OFString(target_oss.str().c_str(), target_oss.str().length()));

#if defined (_WIN32)
        // Try to remove the target first. It seems it is not
        // possible to rename over existing file.
        ret = file_remove (target);
#endif

        ret = file_rename (source, target);
        loglog_renaming_result (*loglog, source, target, ret);
    }
} // end rolloverFiles()


static
STD_NAMESPACE locale
get_locale_by_name (tstring const & locale_name)
{
    try {
        spi::LocaleFactoryRegistry & reg = spi::getLocaleFactoryRegistry ();
        spi::LocaleFactory * fact = reg.get (locale_name);
        if (fact)
        {
            helpers::Properties props;
            props.setProperty (DCMTK_LOG4CPLUS_TEXT ("Locale"), locale_name);
            return fact->createObject (props);
        }
        else
            return STD_NAMESPACE locale (DCMTK_LOG4CPLUS_TSTRING_TO_STRING (locale_name).c_str ());
    }
    catch (STD_NAMESPACE runtime_error const &)
    {
        helpers::getLogLog ().error (
            DCMTK_LOG4CPLUS_TEXT ("Failed to create locale " + locale_name));
        return STD_NAMESPACE locale ();
    }
}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// FileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

FileAppender::FileAppender(const tstring& filename_, 
    STD_NAMESPACE ios_base::openmode mode_, bool immediateFlush_)
    : immediateFlush(immediateFlush_)
    , reopenDelay(1)
    , bufferSize (0)
    , buffer (0)
    , out ()
    , filename ()
    , localeName (DCMTK_LOG4CPLUS_TEXT ("DEFAULT"))
    , reopen_time ()
{
    init(filename_, mode_, internal::empty_str);
}


FileAppender::FileAppender(const Properties& props, 
                           STD_NAMESPACE ios_base::openmode mode_)
    : Appender(props)
    , immediateFlush(true)
    , reopenDelay(1)
    , bufferSize (0)
    , buffer (0)
    , out ()
    , filename ()
    , localeName ()
    , reopen_time ()
{
    bool app = (mode_ == STD_NAMESPACE ios::app);
    tstring const & fn = props.getProperty( DCMTK_LOG4CPLUS_TEXT("File") );
    if (fn.empty())
    {
        getErrorHandler()->error( DCMTK_LOG4CPLUS_TEXT("Invalid filename") );
        return;
    }

    props.getBool (immediateFlush, DCMTK_LOG4CPLUS_TEXT("ImmediateFlush"));
    props.getBool (app, DCMTK_LOG4CPLUS_TEXT("Append"));
    props.getInt (reopenDelay, DCMTK_LOG4CPLUS_TEXT("ReopenDelay"));
    props.getULong (bufferSize, DCMTK_LOG4CPLUS_TEXT("BufferSize"));

    tstring lockFileName = props.getProperty (DCMTK_LOG4CPLUS_TEXT ("LockFile"));
    if (useLockFile && lockFileName.empty ())
    {
        lockFileName = fn;
        lockFileName += DCMTK_LOG4CPLUS_TEXT(".lock");
    }

    localeName = props.getProperty (DCMTK_LOG4CPLUS_TEXT ("Locale"),
        DCMTK_LOG4CPLUS_TEXT ("DEFAULT"));

    init(fn, (app ? STD_NAMESPACE ios::app : STD_NAMESPACE ios::trunc), lockFileName);
}



void
FileAppender::init(const tstring& filename_, 
                   STD_NAMESPACE ios_base::openmode mode_,
                   const log4cplus::tstring& lockFileName_)
{
    filename = filename_;

    if (bufferSize != 0)
    {
        delete[] buffer;
        buffer = new tchar[bufferSize];
        out.rdbuf ()->pubsetbuf (buffer, bufferSize);
    }

    helpers::LockFileGuard guard;
    if (useLockFile && ! lockFile.get ())
    {
        try
        {
            lockFile.reset (new helpers::LockFile (lockFileName_));
            guard.attach_and_lock (*lockFile);
        }
        catch (STD_NAMESPACE runtime_error const &)
        {
            // We do not need to do any logging here as the internals
            // of LockFile already use LogLog to report the failure.
            return;
        }
    }

    open(mode_);
    imbue (get_locale_by_name (localeName));

    if(!out.good()) {
        getErrorHandler()->error(  DCMTK_LOG4CPLUS_TEXT("Unable to open file: ") 
                                 + filename);
        return;
    }
    helpers::getLogLog().debug(DCMTK_LOG4CPLUS_TEXT("Just opened file: ") + filename);
}



FileAppender::~FileAppender()
{
    destructorImpl();
}



///////////////////////////////////////////////////////////////////////////////
// FileAppender public methods
///////////////////////////////////////////////////////////////////////////////

void 
FileAppender::close()
{
    thread::MutexGuard guard (access_mutex);

    out.close();
    delete[] buffer;
    buffer = 0;
    closed = true;
}


STD_NAMESPACE locale
FileAppender::imbue(STD_NAMESPACE locale const& loc)
{
    return out.imbue (loc);
}


STD_NAMESPACE locale
FileAppender::getloc () const
{
    return out.getloc ();
}


///////////////////////////////////////////////////////////////////////////////
// FileAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void
FileAppender::append(const spi::InternalLoggingEvent& event)
{
    if(!out.good()) {
        if(!reopen()) {
            getErrorHandler()->error(  DCMTK_LOG4CPLUS_TEXT("file is not open: ") 
                                     + filename);
            return;
        }
        // Resets the error handler to make it 
        // ready to handle a future append error.
        else
            getErrorHandler()->reset();
    }

    if (useLockFile)
        out.seekp (0, STD_NAMESPACE ios_base::end);

    layout->formatAndAppend(out, event);

    if(immediateFlush || useLockFile)
        out.flush();
}

void
FileAppender::open(STD_NAMESPACE ios::openmode mode)
{
    out.open(DCMTK_LOG4CPLUS_FSTREAM_PREFERED_FILE_NAME(filename).c_str(), mode);
}

bool
FileAppender::reopen()
{
    // When append never failed and the file re-open attempt must
    // be delayed, set the time when reopen should take place.
    if (reopen_time == log4cplus::helpers::Time () && reopenDelay != 0)
        reopen_time = log4cplus::helpers::Time::gettimeofday()
            + log4cplus::helpers::Time(reopenDelay);
    else
    {
        // Otherwise, check for end of the delay (or absence of delay)
        // to re-open the file.
        if (reopen_time <= log4cplus::helpers::Time::gettimeofday()
            || reopenDelay == 0)
        {
            // Close the current file
            out.close();
            out.clear(); // reset flags since the C++ standard specified that all the
                         // flags should remain unchanged on a close

            // Re-open the file.
            open(STD_NAMESPACE ios_base::out | STD_NAMESPACE ios_base::ate);

            // Reset last fail time.
            reopen_time = log4cplus::helpers::Time ();

            // Succeed if no errors are found.
            if(out.good())
                return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// RollingFileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

RollingFileAppender::RollingFileAppender(const tstring& filename_,
    long maxFileSize_, int maxBackupIndex_, bool immediateFlush_)
    : FileAppender(filename_, STD_NAMESPACE ios::app, immediateFlush_), maxFileSize (), maxBackupIndex ()
{
    init(maxFileSize_, maxBackupIndex_);
}


RollingFileAppender::RollingFileAppender(const Properties& properties)
    : FileAppender(properties, STD_NAMESPACE ios::app), maxFileSize (), maxBackupIndex ()
{
    long tmpMaxFileSize = DEFAULT_ROLLING_LOG_SIZE;
    int tmpMaxBackupIndex = 1;
    tstring tmp (
        helpers::toUpper (
            properties.getProperty (DCMTK_LOG4CPLUS_TEXT ("MaxFileSize"))));
    if (! tmp.empty ())
    {
        tmpMaxFileSize = atoi(DCMTK_LOG4CPLUS_TSTRING_TO_STRING(tmp).c_str());
        if (tmpMaxFileSize != 0)
        {
            tstring::size_type const len = tmp.length();
            if (len > 2
                && tmp.compare (len - 2, 2, DCMTK_LOG4CPLUS_TEXT("MB")) == 0)
                tmpMaxFileSize *= (1024 * 1024); // convert to megabytes
            else if (len > 2
                && tmp.compare (len - 2, 2, DCMTK_LOG4CPLUS_TEXT("KB")) == 0)
                tmpMaxFileSize *= 1024; // convert to kilobytes
        }
        tmpMaxFileSize = MAX(tmpMaxFileSize, MINIMUM_ROLLING_LOG_SIZE);
    }

    properties.getInt (tmpMaxBackupIndex, DCMTK_LOG4CPLUS_TEXT("MaxBackupIndex"));

    init(tmpMaxFileSize, tmpMaxBackupIndex);
}


void
RollingFileAppender::init(long maxFileSize_, int maxBackupIndex_)
{
    if (maxFileSize_ < MINIMUM_ROLLING_LOG_SIZE)
    {
        tostringstream oss;
        oss << DCMTK_LOG4CPLUS_TEXT ("RollingFileAppender: MaxFileSize property")
            DCMTK_LOG4CPLUS_TEXT (" value is too small. Resetting to ")
            << MINIMUM_ROLLING_LOG_SIZE << ".";
        helpers::getLogLog ().warn (OFString(oss.str().c_str(), oss.str().length()));
        maxFileSize_ = MINIMUM_ROLLING_LOG_SIZE;
    }

    maxFileSize = maxFileSize_;
    maxBackupIndex = MAX(maxBackupIndex_, 1);
}


RollingFileAppender::~RollingFileAppender()
{
    destructorImpl();
}


///////////////////////////////////////////////////////////////////////////////
// RollingFileAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void
RollingFileAppender::append(const spi::InternalLoggingEvent& event)
{
    FileAppender::append(event);

    if(out.tellp() > maxFileSize) {
        rollover(true);
    }
}


void
RollingFileAppender::rollover(bool alreadyLocked)
{
    helpers::LogLog & loglog = helpers::getLogLog();
    helpers::LockFileGuard guard;

    // Close the current file
    out.close();
    // Reset flags since the C++ standard specified that all the flags
    // should remain unchanged on a close.
    out.clear(); 

    if (useLockFile)
    {
        if (! alreadyLocked)
        {
            try
            {
                guard.attach_and_lock (*lockFile);
            }
            catch (STD_NAMESPACE runtime_error const &)
            {
                return;
            }
        }

        // Recheck the condition as there is a window where another
        // process can rollover the file before us.

        helpers::FileInfo fi;
        if (helpers::getFileInfo (&fi, filename) == -1
            || fi.size < maxFileSize)
        {
            // The file has already been rolled by another
            // process. Just reopen with the new file.

            // Open it up again.
            open (STD_NAMESPACE ios::out | STD_NAMESPACE ios::ate);
            loglog_opening_result (loglog, out, filename);

            return;
        }
    }

    // If maxBackups <= 0, then there is no file renaming to be done.
    if (maxBackupIndex > 0)
    {
        rolloverFiles(filename, maxBackupIndex);

        // Rename fileName to fileName.1
        tstring target = filename + DCMTK_LOG4CPLUS_TEXT(".1");

        long ret;

#if defined (_WIN32)
        // Try to remove the target first. It seems it is not
        // possible to rename over existing file.
        ret = file_remove (target);
#endif

        loglog.debug (
            DCMTK_LOG4CPLUS_TEXT("Renaming file ") 
            + filename 
            + DCMTK_LOG4CPLUS_TEXT(" to ")
            + target);
        ret = file_rename (filename, target);
        loglog_renaming_result (loglog, filename, target, ret);
    }
    else
    {
        loglog.debug (filename + DCMTK_LOG4CPLUS_TEXT(" has no backups specified"));
    }

    // Open it up again in truncation mode
    open(STD_NAMESPACE ios::out | STD_NAMESPACE ios::trunc);
    loglog_opening_result (loglog, out, filename);
}


///////////////////////////////////////////////////////////////////////////////
// DailyRollingFileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

DailyRollingFileAppender::DailyRollingFileAppender(
    const tstring& filename_, DailyRollingFileSchedule schedule_,
    bool immediateFlush_, int maxBackupIndex_)
    : FileAppender(filename_, STD_NAMESPACE ios::app, immediateFlush_)
    , schedule()
    , scheduledFilename()
    , nextRolloverTime()
    , maxBackupIndex(maxBackupIndex_)
{
    init(schedule_);
}



DailyRollingFileAppender::DailyRollingFileAppender(
    const Properties& properties)
    : FileAppender(properties, STD_NAMESPACE ios::app)
    , schedule()
    , scheduledFilename()
    , nextRolloverTime()
    , maxBackupIndex(10)
{
    DailyRollingFileSchedule theSchedule = DAILY;
    tstring scheduleStr (helpers::toUpper (
        properties.getProperty (DCMTK_LOG4CPLUS_TEXT ("Schedule"))));

    if(scheduleStr == DCMTK_LOG4CPLUS_TEXT("MONTHLY"))
        theSchedule = MONTHLY;
    else if(scheduleStr == DCMTK_LOG4CPLUS_TEXT("WEEKLY"))
        theSchedule = WEEKLY;
    else if(scheduleStr == DCMTK_LOG4CPLUS_TEXT("DAILY"))
        theSchedule = DAILY;
    else if(scheduleStr == DCMTK_LOG4CPLUS_TEXT("TWICE_DAILY"))
        theSchedule = TWICE_DAILY;
    else if(scheduleStr == DCMTK_LOG4CPLUS_TEXT("HOURLY"))
        theSchedule = HOURLY;
    else if(scheduleStr == DCMTK_LOG4CPLUS_TEXT("MINUTELY"))
        theSchedule = MINUTELY;
    else {
        helpers::getLogLog().warn(
            DCMTK_LOG4CPLUS_TEXT("DailyRollingFileAppender::ctor()")
            DCMTK_LOG4CPLUS_TEXT("- \"Schedule\" not valid: ")
            + properties.getProperty(DCMTK_LOG4CPLUS_TEXT("Schedule")));
        theSchedule = DAILY;
    }
    
    properties.getInt (maxBackupIndex, DCMTK_LOG4CPLUS_TEXT("MaxBackupIndex"));

    init(theSchedule);
}



void
DailyRollingFileAppender::init(DailyRollingFileSchedule sch)
{
    this->schedule = sch;

    Time now = Time::gettimeofday();
    now.usec(0);
    struct tm time;
    now.localtime(&time);

    time.tm_sec = 0;
    switch (schedule)
    {
    case MONTHLY:
        time.tm_mday = 1;
        time.tm_hour = 0;
        time.tm_min = 0;
        break;

    case WEEKLY:
        time.tm_mday -= (time.tm_wday % 7);
        time.tm_hour = 0;
        time.tm_min = 0;
        break;

    case DAILY:
        time.tm_hour = 0;
        time.tm_min = 0;
        break;

    case TWICE_DAILY:
        if(time.tm_hour >= 12) {
            time.tm_hour = 12;
        }
        else {
            time.tm_hour = 0;
        }
        time.tm_min = 0;
        break;

    case HOURLY:
        time.tm_min = 0;
        break;

    case MINUTELY:
        break;
    };
    now.setTime(&time);

    scheduledFilename = getFilename(now);
    nextRolloverTime = calculateNextRolloverTime(now);
}



DailyRollingFileAppender::~DailyRollingFileAppender()
{
    destructorImpl();
}




///////////////////////////////////////////////////////////////////////////////
// DailyRollingFileAppender public methods
///////////////////////////////////////////////////////////////////////////////

void
DailyRollingFileAppender::close()
{
    rollover();
    FileAppender::close();
}



///////////////////////////////////////////////////////////////////////////////
// DailyRollingFileAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void
DailyRollingFileAppender::append(const spi::InternalLoggingEvent& event)
{
    if(event.getTimestamp() >= nextRolloverTime) {
        rollover(true);
    }

    FileAppender::append(event);
}



void
DailyRollingFileAppender::rollover(bool alreadyLocked)
{
    helpers::LockFileGuard guard;

    if (useLockFile && ! alreadyLocked)
    {
        try
        {
            guard.attach_and_lock (*lockFile);
        }
        catch (STD_NAMESPACE runtime_error const &)
        {
            return;
        }
    }

    // Close the current file
    out.close();
    out.clear(); // reset flags since the C++ standard specified that all the
                 // flags should remain unchanged on a close

    // If we've already rolled over this time period, we'll make sure that we
    // don't overwrite any of those previous files.
    // E.g. if "log.2009-11-07.1" already exists we rename it
    // to "log.2009-11-07.2", etc.
    rolloverFiles(scheduledFilename, maxBackupIndex);

    // Do not overwriet the newest file either, e.g. if "log.2009-11-07"
    // already exists rename it to "log.2009-11-07.1"
    tostringstream backup_target_oss;
    backup_target_oss << scheduledFilename << DCMTK_LOG4CPLUS_TEXT(".") << 1;
    tstring backupTarget(backup_target_oss.str().c_str(), backup_target_oss.str().length());

    helpers::LogLog & loglog = helpers::getLogLog();
    long ret;

#if defined (_WIN32)
    // Try to remove the target first. It seems it is not
    // possible to rename over existing file, e.g. "log.2009-11-07.1".
    ret = file_remove (backupTarget);
#endif

    // Rename e.g. "log.2009-11-07" to "log.2009-11-07.1".
    ret = file_rename (scheduledFilename, backupTarget);
    loglog_renaming_result (loglog, scheduledFilename, backupTarget, ret);

#if defined (_WIN32)
    // Try to remove the target first. It seems it is not
    // possible to rename over existing file, e.g. "log.2009-11-07".
    ret = file_remove (scheduledFilename);
#endif
   
    // Rename filename to scheduledFilename,
    // e.g. rename "log" to "log.2009-11-07".
    loglog.debug(
        DCMTK_LOG4CPLUS_TEXT("Renaming file ")
        + filename 
        + DCMTK_LOG4CPLUS_TEXT(" to ")
        + scheduledFilename);
    ret = file_rename (filename, scheduledFilename);
    loglog_renaming_result (loglog, filename, scheduledFilename, ret);

    // Open a new file, e.g. "log".
    open(STD_NAMESPACE ios::out | STD_NAMESPACE ios::trunc);
    loglog_opening_result (loglog, out, filename);

    // Calculate the next rollover time
    log4cplus::helpers::Time now = Time::gettimeofday();
    if (now >= nextRolloverTime)
    {
        scheduledFilename = getFilename(now);
        nextRolloverTime = calculateNextRolloverTime(now);
    }
}



Time
DailyRollingFileAppender::calculateNextRolloverTime(const Time& t) const
{
    switch(schedule)
    {
    case MONTHLY: 
    {
        struct tm nextMonthTime;
        t.localtime(&nextMonthTime);
        nextMonthTime.tm_mon += 1;
        nextMonthTime.tm_isdst = 0;

        Time ret;
        if(ret.setTime(&nextMonthTime) == -1) {
            helpers::getLogLog().error(
                DCMTK_LOG4CPLUS_TEXT("DailyRollingFileAppender::calculateNextRolloverTime()-")
                DCMTK_LOG4CPLUS_TEXT(" setTime() returned error"));
            // Set next rollover to 31 days in future.
            ret = (t + Time(2678400));
        }

        return ret;
    }

    case WEEKLY:
        return (t + Time(7 * 24 * 60 * 60));

    default:
        helpers::getLogLog ().error (
            DCMTK_LOG4CPLUS_TEXT ("DailyRollingFileAppender::calculateNextRolloverTime()-")
            DCMTK_LOG4CPLUS_TEXT (" invalid schedule value"));
        // Fall through.

    case DAILY:
        return (t + Time(24 * 60 * 60));

    case TWICE_DAILY:
        return (t + Time(12 * 60 * 60));

    case HOURLY:
        return (t + Time(60 * 60));

    case MINUTELY:
        return (t + Time(60));
    };
}



tstring
DailyRollingFileAppender::getFilename(const Time& t) const
{
    tchar const * pattern = 0;
    switch (schedule)
    {
    case MONTHLY:
        pattern = DCMTK_LOG4CPLUS_TEXT("%Y-%m");
        break;

    case WEEKLY:
        pattern = DCMTK_LOG4CPLUS_TEXT("%Y-%W");
        break;

    default:
        helpers::getLogLog ().error (
            DCMTK_LOG4CPLUS_TEXT ("DailyRollingFileAppender::getFilename()-")
            DCMTK_LOG4CPLUS_TEXT (" invalid schedule value"));
        // Fall through.

    case DAILY:
        pattern = DCMTK_LOG4CPLUS_TEXT("%Y-%m-%d");
        break;

    case TWICE_DAILY:
        pattern = DCMTK_LOG4CPLUS_TEXT("%Y-%m-%d-%p");
        break;

    case HOURLY:
        pattern = DCMTK_LOG4CPLUS_TEXT("%Y-%m-%d-%H");
        break;

    case MINUTELY:
        pattern = DCMTK_LOG4CPLUS_TEXT("%Y-%m-%d-%H-%M");
        break;
    };

    tstring result (filename);
    result += DCMTK_LOG4CPLUS_TEXT(".");
    result += t.getFormattedTime(pattern, false);
    return result;
}

} // namespace log4cplus
} // end namespace dcmtk

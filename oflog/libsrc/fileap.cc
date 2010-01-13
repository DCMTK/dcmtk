// Module:  Log4CPLUS
// File:    fileappender.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//

#include "dcmtk/oflog/fileap.h"
#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/timehelp.h"
#include "dcmtk/oflog/spi/logevent.h"
//#include <algorithm>

#if defined (__BORLANDC__)
// For _wrename() and _wremove() on Windows.
#  include <stdio.h>
#endif

//#include <cstdio>
#define INCLUDE_CSTDIO
//#include <cerrno>
#define INCLUDE_CERRNO
//#include <cstdlib>
#define INCLUDE_CSTDLIB

#include "dcmtk/ofstd/ofstdinc.h"


namespace log4cplus
{

using helpers::Properties;
using helpers::Time;


const long MINIMUM_ROLLING_LOG_SIZE = 200*1024L;


///////////////////////////////////////////////////////////////////////////////
// File LOCAL definitions
///////////////////////////////////////////////////////////////////////////////

namespace
{

static
int
file_rename (tstring const & src, tstring const & target)
{
    return STD_NAMESPACE rename (LOG4CPLUS_TSTRING_TO_STRING (src).c_str (),
        LOG4CPLUS_TSTRING_TO_STRING (target).c_str ()) == 0 ? 0 : -1;
}


static
int
file_remove (tstring const & src)
{
    return STD_NAMESPACE remove (LOG4CPLUS_TSTRING_TO_STRING (src).c_str ()) == 0
        ? 0 : -1;
}


static
void
loglog_renaming_result (helpers::LogLog & loglog, tstring const & src,
    tstring const & target, int ret)
{
    if (ret == 0)
    {
        loglog.debug (
            LOG4CPLUS_TEXT("Renamed file ")
            + src
            + LOG4CPLUS_TEXT(" to ")
            + target);
    }
    else if (ret == -1 && errno != ENOENT)
    {
        loglog.error (
            LOG4CPLUS_TEXT("Failed to rename file from ")
            + target
            + LOG4CPLUS_TEXT(" to ")
            + target);
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
            LOG4CPLUS_TEXT("Failed to open file ")
            + filename);
    }
}


static
void
rolloverFiles(const tstring& filename, unsigned int maxBackupIndex)
{
    helpers::SharedObjectPtr<helpers::LogLog> loglog
        = helpers::LogLog::getLogLog();

    // Delete the oldest file
    tostringstream buffer;
    buffer << filename << LOG4CPLUS_TEXT(".") << maxBackupIndex;
    OFSTRINGSTREAM_GETOFSTRING(buffer, buffer_str)
    int ret = file_remove (buffer_str);

    tostringstream source_oss;
    tostringstream target_oss;

    // Map {(maxBackupIndex - 1), ..., 2, 1} to {maxBackupIndex, ..., 3, 2}
    for (int i = maxBackupIndex - 1; i >= 1; --i)
    {
        source_oss.str(LOG4CPLUS_TEXT(""));
        target_oss.str(LOG4CPLUS_TEXT(""));

        source_oss << filename << LOG4CPLUS_TEXT(".") << i;
        target_oss << filename << LOG4CPLUS_TEXT(".") << (i+1);

        OFSTRINGSTREAM_GETOFSTRING(source_oss, source)
        OFSTRINGSTREAM_GETOFSTRING(target_oss, target)

#if defined (WIN32)
        // Try to remove the target first. It seems it is not
        // possible to rename over existing file.
        ret = file_remove (target);
#endif

        ret = file_rename (source, target);
        loglog_renaming_result (*loglog, source, target, ret);
    }
} // end rolloverFiles()

}


///////////////////////////////////////////////////////////////////////////////
// FileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

FileAppender::FileAppender(const tstring& filename_,
    LOG4CPLUS_OPEN_MODE_TYPE mode, bool immediateFlush)
    : immediateFlush(immediateFlush)
{
    init(filename_, mode);
}


FileAppender::FileAppender(const Properties& properties,
                           LOG4CPLUS_OPEN_MODE_TYPE mode)
    : Appender(properties)
    , immediateFlush(true)
{
    bool append = (mode == STD_NAMESPACE ios::app);
    tstring filename_ = properties.getProperty( LOG4CPLUS_TEXT("File") );
    if (filename_.empty())
    {
        getErrorHandler()->error( LOG4CPLUS_TEXT("Invalid filename") );
        return;
    }
    if(properties.exists( LOG4CPLUS_TEXT("ImmediateFlush") )) {
        tstring tmp = properties.getProperty( LOG4CPLUS_TEXT("ImmediateFlush") );
        immediateFlush = (helpers::toLower(tmp) == LOG4CPLUS_TEXT("true"));
    }
    if(properties.exists( LOG4CPLUS_TEXT("Append") )) {
        tstring tmp = properties.getProperty( LOG4CPLUS_TEXT("Append") );
        append = (helpers::toLower(tmp) == LOG4CPLUS_TEXT("true"));
    }

    init(filename_, (append ? STD_NAMESPACE ios::app : STD_NAMESPACE ios::trunc));
}



void
FileAppender::init(const tstring& filename_,
                   LOG4CPLUS_OPEN_MODE_TYPE mode)
{
    this->filename = filename_;
    out.open(LOG4CPLUS_TSTRING_TO_STRING(filename).c_str(), mode);

    if(!out.good()) {
        getErrorHandler()->error(  LOG4CPLUS_TEXT("Unable to open file: ")
                                 + filename);
        return;
    }
    getLogLog().debug(LOG4CPLUS_TEXT("Just opened file: ") + filename);
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
    LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( access_mutex )
        out.close();
        closed = true;
    LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX;
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
        getErrorHandler()->error(  LOG4CPLUS_TEXT("file is not open: ")
                                 + filename);
        return;
    }

    layout->formatAndAppend(out, event);
    if(immediateFlush) {
        out.flush();
    }
}


///////////////////////////////////////////////////////////////////////////////
// RollingFileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

RollingFileAppender::RollingFileAppender(const tstring& filename,
    long maxFileSize_, int maxBackupIndex_, bool immediateFlush)
    : FileAppender(filename, STD_NAMESPACE ios::app, immediateFlush)
{
    init(maxFileSize_, maxBackupIndex_);
}


RollingFileAppender::RollingFileAppender(const Properties& properties)
    : FileAppender(properties, STD_NAMESPACE ios::app)
{
    int maxFileSize_ = 10*1024*1024;
    int maxBackupIndex_ = 1;
    if(properties.exists( LOG4CPLUS_TEXT("MaxFileSize") )) {
        tstring tmp = properties.getProperty( LOG4CPLUS_TEXT("MaxFileSize") );
        tmp = helpers::toUpper(tmp);
        maxFileSize_ = STD_NAMESPACE atoi(LOG4CPLUS_TSTRING_TO_STRING(tmp).c_str());
        if(tmp.find( LOG4CPLUS_TEXT("MB") ) == (tmp.length() - 2)) {
            maxFileSize_ *= (1024 * 1024); // convert to megabytes
        }
        if(tmp.find( LOG4CPLUS_TEXT("KB") ) == (tmp.length() - 2)) {
            maxFileSize_ *= 1024; // convert to kilobytes
        }
    }

    if(properties.exists( LOG4CPLUS_TEXT("MaxBackupIndex") )) {
        tstring tmp = properties.getProperty(LOG4CPLUS_TEXT("MaxBackupIndex"));
        maxBackupIndex_ = STD_NAMESPACE atoi(LOG4CPLUS_TSTRING_TO_STRING(tmp).c_str());
    }

    init(maxFileSize_, maxBackupIndex_);
}

void
RollingFileAppender::init(long maxFileSize_, int maxBackupIndex_)
{
    this->maxFileSize = maxFileSize_;
    if (this->maxFileSize < MINIMUM_ROLLING_LOG_SIZE)
        this->maxFileSize = MINIMUM_ROLLING_LOG_SIZE;
    this->maxBackupIndex = maxBackupIndex_;
    if (this->maxBackupIndex < 1)
        this->maxBackupIndex = 1;
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
    if(!out.good()) {
        getErrorHandler()->error(  LOG4CPLUS_TEXT("file is not open: ")
                                 + filename);
        return;
    }

    layout->formatAndAppend(out, event);
    if(immediateFlush) {
        out.flush();
    }

    if(out.tellp() > maxFileSize) {
        rollover();
    }
}


void
RollingFileAppender::rollover()
{
    helpers::LogLog & loglog = getLogLog();

    // Close the current file
    out.close();
    out.clear(); // reset flags since the C++ standard specified that all the
                 // flags should remain unchanged on a close

    // If maxBackups <= 0, then there is no file renaming to be done.
    if (maxBackupIndex > 0)
    {
        rolloverFiles(filename, maxBackupIndex);

        // Rename fileName to fileName.1
        tstring target = filename + LOG4CPLUS_TEXT(".1");

        int ret;

#if defined (WIN32)
        // Try to remove the target first. It seems it is not
        // possible to rename over existing file.
        ret = file_remove (target);
#endif

        loglog.debug (
            LOG4CPLUS_TEXT("Renaming file ")
            + filename
            + LOG4CPLUS_TEXT(" to ")
            + target);
        ret = file_rename (filename, target);
        loglog_renaming_result (loglog, filename, target, ret);
    }
    else
    {
        loglog.debug (filename + LOG4CPLUS_TEXT(" has no backups specified"));
    }

    // Open it up again in truncation mode
    out.open(LOG4CPLUS_TSTRING_TO_STRING(filename).c_str(),
        STD_NAMESPACE ios::out | STD_NAMESPACE ios::trunc);
    loglog_opening_result (loglog, out, filename);
}


///////////////////////////////////////////////////////////////////////////////
// DailyRollingFileAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

DailyRollingFileAppender::DailyRollingFileAppender(
    const tstring& filename, DailyRollingFileSchedule schedule_,
    bool immediateFlush, int maxBackupIndex)
    : FileAppender(filename, STD_NAMESPACE ios::app, immediateFlush)
    , maxBackupIndex(maxBackupIndex)
{
    init(schedule_);
}



DailyRollingFileAppender::DailyRollingFileAppender(
    const Properties& properties)
    : FileAppender(properties, STD_NAMESPACE ios::app)
    , maxBackupIndex(10)
{
    DailyRollingFileSchedule theSchedule = DAILY;
    tstring scheduleStr = properties.getProperty(LOG4CPLUS_TEXT("Schedule"));
    scheduleStr = helpers::toUpper(scheduleStr);

    if(scheduleStr == LOG4CPLUS_TEXT("MONTHLY"))
        theSchedule = MONTHLY;
    else if(scheduleStr == LOG4CPLUS_TEXT("WEEKLY"))
        theSchedule = WEEKLY;
    else if(scheduleStr == LOG4CPLUS_TEXT("DAILY"))
        theSchedule = DAILY;
    else if(scheduleStr == LOG4CPLUS_TEXT("TWICE_DAILY"))
        theSchedule = TWICE_DAILY;
    else if(scheduleStr == LOG4CPLUS_TEXT("HOURLY"))
        theSchedule = HOURLY;
    else if(scheduleStr == LOG4CPLUS_TEXT("MINUTELY"))
        theSchedule = MINUTELY;
    else {
        getLogLog().warn(  LOG4CPLUS_TEXT("DailyRollingFileAppender::ctor()- \"Schedule\" not valid: ")
                         + properties.getProperty(LOG4CPLUS_TEXT("Schedule")));
        theSchedule = DAILY;
    }

    if(properties.exists( LOG4CPLUS_TEXT("MaxBackupIndex") )) {
        tstring tmp = properties.getProperty(LOG4CPLUS_TEXT("MaxBackupIndex"));
        maxBackupIndex = STD_NAMESPACE atoi(LOG4CPLUS_TSTRING_TO_STRING(tmp).c_str());
    }

    init(theSchedule);
}



void
DailyRollingFileAppender::init(DailyRollingFileSchedule schedule_)
{
    this->schedule = schedule_;

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
    if(!out.good()) {
        getErrorHandler()->error(  LOG4CPLUS_TEXT("file is not open: ")
                                 + filename);
        return;
    }

    if(event.getTimestamp() >= nextRolloverTime) {
        rollover();
    }

    layout->formatAndAppend(out, event);
    if(immediateFlush) {
        out.flush();
    }
}



void
DailyRollingFileAppender::rollover()
{
    // Close the current file
    out.close();
    out.clear(); // reset flags since the C++ standard specified that all the
                 // flags should remain unchanged on a close

    // If we've already rolled over this time period, we'll make sure that we
    // don't overwrite any of those previous files.
    rolloverFiles(scheduledFilename, maxBackupIndex);
    tostringstream backup_target_oss;
    backup_target_oss << scheduledFilename << LOG4CPLUS_TEXT(".") << 1;
    OFSTRINGSTREAM_GETOFSTRING(backup_target_oss, backupTarget)

    helpers::LogLog & loglog = getLogLog();
    int ret;

#if defined (WIN32)
    // Try to remove the target first. It seems it is not
    // possible to rename over existing file.
    ret = file_remove (backupTarget);
#endif

    ret = file_rename (scheduledFilename, backupTarget);
    loglog_renaming_result (loglog, scheduledFilename, backupTarget, ret);

    // Rename filename to scheduledFilename
    loglog.debug(
        LOG4CPLUS_TEXT("Renaming file ")
        + filename
        + LOG4CPLUS_TEXT(" to ")
        + scheduledFilename);
    ret = file_rename (filename, scheduledFilename);
    loglog_renaming_result (loglog, filename, scheduledFilename, ret);

    // Open a new file
    out.open(LOG4CPLUS_TSTRING_TO_STRING(filename).c_str(),
        STD_NAMESPACE ios::out | STD_NAMESPACE ios::trunc);
    loglog_opening_result (loglog, out, filename);

    // Calculate the next rollover time
    if (Time::gettimeofday() >= nextRolloverTime)
    {
        scheduledFilename = getFilename(nextRolloverTime);
        nextRolloverTime = calculateNextRolloverTime(nextRolloverTime);
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
            getLogLog().error(LOG4CPLUS_TEXT("DailyRollingFileAppender::calculateNextRolloverTime()- setTime() returned error"));
            ret = (t + Time(2678400));
        }

        return ret;
    }

    case WEEKLY:
        return (t + Time(604800)); // 7 * 24 * 60 * 60 seconds

    default:
        getLogLog ().error (
            LOG4CPLUS_TEXT ("DailyRollingFileAppender::calculateNextRolloverTime()-")
            LOG4CPLUS_TEXT (" invalid schedule value"));
        // Fall through.

    case DAILY:
        return (t + Time(86400)); //      24 * 60 * 60 seconds

    case TWICE_DAILY:
        return (t + Time(43200)); //      12 * 60 * 60 seconds

    case HOURLY:
        return (t + Time(3600));  //           60 * 60 seconds

    case MINUTELY:
        return (t + Time(60));    //                60 seconds
    };
}



tstring
DailyRollingFileAppender::getFilename(const Time& t) const
{
    tchar const * pattern = 0;
    switch (schedule)
    {
    case MONTHLY:
        pattern = LOG4CPLUS_TEXT("%Y-%m");
        break;

    case WEEKLY:
        pattern = LOG4CPLUS_TEXT("%Y-%W");
        break;

    default:
        getLogLog ().error (
            LOG4CPLUS_TEXT ("DailyRollingFileAppender::getFilename()-")
            LOG4CPLUS_TEXT (" invalid schedule value"));
        // Fall through.

    case DAILY:
        pattern = LOG4CPLUS_TEXT("%Y-%m-%d");
        break;

    case TWICE_DAILY:
        pattern = LOG4CPLUS_TEXT("%Y-%m-%d-%p");
        break;

    case HOURLY:
        pattern = LOG4CPLUS_TEXT("%Y-%m-%d-%H");
        break;

    case MINUTELY:
        pattern = LOG4CPLUS_TEXT("%Y-%m-%d-%H-%M");
        break;
    };

    return filename + LOG4CPLUS_TEXT(".") + t.getFormattedTime(pattern, false);
}

} // namespace log4cplus

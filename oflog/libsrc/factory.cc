// Module:  Log4CPLUS
// File:    factory.cxx
// Created: 2/2002
// Author:  Tad E. Smith
//
//
// Copyright 2002-2010 Tad E. Smith
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

#include "dcmtk/oflog/spi/factory.h"
#include "dcmtk/oflog/spi/logfact.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/threadcf.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/asyncap.h"
#include "dcmtk/oflog/consap.h"
#include "dcmtk/oflog/fileap.h"
#include "dcmtk/oflog/ntelogap.h"
#include "dcmtk/oflog/nullap.h"
#include "dcmtk/oflog/socketap.h"
#include "dcmtk/oflog/syslogap.h"
#include "dcmtk/oflog/windebap.h"
#include "dcmtk/oflog/winconap.h"
#include "dcmtk/oflog/log4judp.h"


///////////////////////////////////////////////////////////////////////////////
// LOCAL file class definitions
///////////////////////////////////////////////////////////////////////////////

namespace dcmtk {
namespace log4cplus {
    
namespace spi {

BaseFactory::~BaseFactory()
{ }


AppenderFactory::AppenderFactory()
{ }

AppenderFactory::~AppenderFactory()
{ }


LayoutFactory::LayoutFactory()
{ }

LayoutFactory::~LayoutFactory()
{ }


FilterFactory::FilterFactory()
{ }

FilterFactory::~FilterFactory()
{ }


LocaleFactory::LocaleFactory()
{ }

LocaleFactory::~LocaleFactory()
{ }


LoggerFactory::~LoggerFactory()
{ }


namespace
{

class GlobalLocale
    : public LocalFactoryBase<LocaleFactory>
{
public:
    GlobalLocale (tchar const * n)
        : LocalFactoryBase<LocaleFactory> (n)
    { }

    virtual
    ProductPtr
    createObject (const log4cplus::helpers::Properties &)
    {
        return STD_NAMESPACE locale ();
    }
};


class UserLocale
    : public LocalFactoryBase<LocaleFactory>
{
public:
    UserLocale (tchar const * n)
        : LocalFactoryBase<LocaleFactory> (n)
    { }

    virtual
    ProductPtr
    createObject (const log4cplus::helpers::Properties &)
    {
        return STD_NAMESPACE locale ("");
    }
};


class ClassicLocale
    : public LocalFactoryBase<LocaleFactory>
{
public:
    ClassicLocale (tchar const * n)
        : LocalFactoryBase<LocaleFactory> (n)
    { }

    virtual
    ProductPtr
    createObject (const log4cplus::helpers::Properties &)
    {
        return STD_NAMESPACE locale::classic ();
    }
};


} // namespace




} // namespace spi


void initializeFactoryRegistry();
void initializeFactoryRegistry()
{
    spi::AppenderFactoryRegistry& reg = spi::getAppenderFactoryRegistry();
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, ConsoleAppender);
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, NullAppender);
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, FileAppender);
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, RollingFileAppender);
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, DailyRollingFileAppender);
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, SocketAppender);
#if defined(_WIN32)
#  if defined(DCMTK_LOG4CPLUS_HAVE_NT_EVENT_LOG)
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, NTEventLogAppender);
#  endif
#  if defined(DCMTK_LOG4CPLUS_HAVE_WIN32_CONSOLE)
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, Win32ConsoleAppender);
#  endif
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, Win32DebugAppender);
#endif
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, SysLogAppender);
#ifndef DCMTK_LOG4CPLUS_SINGLE_THREADED
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, AsyncAppender);
#endif
    DCMTK_LOG4CPLUS_REG_APPENDER (reg, Log4jUdpAppender);

    spi::LayoutFactoryRegistry& reg2 = spi::getLayoutFactoryRegistry();
    DCMTK_LOG4CPLUS_REG_LAYOUT (reg2, SimpleLayout);
    DCMTK_LOG4CPLUS_REG_LAYOUT (reg2, TTCCLayout);
    DCMTK_LOG4CPLUS_REG_LAYOUT (reg2, PatternLayout);

    spi::FilterFactoryRegistry& reg3 = spi::getFilterFactoryRegistry();
    DCMTK_LOG4CPLUS_REG_FILTER (reg3, DenyAllFilter);
    DCMTK_LOG4CPLUS_REG_FILTER (reg3, LogLevelMatchFilter);
    DCMTK_LOG4CPLUS_REG_FILTER (reg3, LogLevelRangeFilter);
    DCMTK_LOG4CPLUS_REG_FILTER (reg3, StringMatchFilter);

    spi::LocaleFactoryRegistry& reg4 = spi::getLocaleFactoryRegistry();
    DCMTK_LOG4CPLUS_REG_LOCALE (reg4, DCMTK_LOG4CPLUS_TEXT("GLOBAL"), spi::GlobalLocale);
    DCMTK_LOG4CPLUS_REG_LOCALE (reg4, DCMTK_LOG4CPLUS_TEXT("DEFAULT"), spi::GlobalLocale);
    DCMTK_LOG4CPLUS_REG_LOCALE (reg4, DCMTK_LOG4CPLUS_TEXT("USER"), spi::UserLocale);
    DCMTK_LOG4CPLUS_REG_LOCALE (reg4, DCMTK_LOG4CPLUS_TEXT("CLASSIC"), spi::ClassicLocale);
}


} // namespace log4cplus
} // end namespace dcmtk

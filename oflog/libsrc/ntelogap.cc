// Module:  LOG4CPLUS
// File:    nteventlogappender.cxx
// Created: 4/2003
// Author:  Michael CATANZARITI
//
// Copyright 2003-2010 Michael CATANZARITI
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

#include "dcmtk/oflog/config.h"
#if defined (DCMTK_LOG4CPLUS_HAVE_NT_EVENT_LOG)

#include "dcmtk/oflog/ntelogap.h"
#include "dcmtk/oflog/loglevel.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/internal/internal.h"
#include "dcmtk/oflog/thread/syncpub.h"
#include <sstream>
#include <cstdlib>


namespace dcmtk
{
namespace log4cplus
{


//////////////////////////////////////////////////////////////////////////////
// File LOCAL methods
//////////////////////////////////////////////////////////////////////////////

namespace {

    static
    bool
    copySID(SID** ppDstSid, SID* pSrcSid)
    {
        DWORD dwLength = ::GetLengthSid(pSrcSid);

        SID * pDstSid = OFreinterpret_cast(SID*, calloc(1, dwLength));
        if (! pDstSid)
            return false;

        if (CopySid(dwLength, pDstSid, pSrcSid))
        {
            *ppDstSid = pDstSid;
            return true;
        }
        else
        {
            free (pDstSid);
            return false;
        }
    }


    static
    bool
    GetCurrentUserSID(SID** ppSid)
    {
        bool bSuccess = false;
        TOKEN_USER * ptu = 0;
        DWORD tusize = 0;
        HANDLE hProcess = ::GetCurrentProcess();
        HANDLE hToken = 0;

        if (! ::OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
            goto finish;

        // Get the required size
        if (! GetTokenInformation(hToken, TokenUser, NULL, 0, &tusize))
            goto finish;

        ptu = OFreinterpret_cast(TOKEN_USER*, calloc(1, tusize));
        if (! ptu)
            goto finish;

        if (GetTokenInformation(hToken, TokenUser, OFreinterpret_cast(LPVOID, ptu), tusize, &tusize))
            bSuccess = copySID (ppSid, OFreinterpret_cast(SID*, ptu->User.Sid));

    finish:;
        if (hToken)
            CloseHandle (hToken);

        free (ptu);

        return bSuccess;
    }


    static
    HKEY
    regGetKey(const tstring& subkey, DWORD* disposition)
    {
        HKEY hkey = 0;
#if defined (DCMTK_OFLOG_UNICODE)
        RegCreateKeyEx(HKEY_LOCAL_MACHINE,
#else
        RegCreateKeyExA(HKEY_LOCAL_MACHINE,
#endif
                       subkey.c_str(),
                       0,
                       NULL,
                       REG_OPTION_NON_VOLATILE,
                       KEY_SET_VALUE,
                       NULL,
                       &hkey,
                       disposition);
        return hkey;
    }


    static
    void
    regSetString(HKEY hkey, const tstring& name, const tstring& value)
    {
#if defined (DCMTK_OFLOG_UNICODE)
        RegSetValueEx(hkey,
#else
        RegSetValueExA(hkey,
#endif
                      name.c_str(),
                      0,
                      REG_SZ,
                      OFreinterpret_cast(BYTE const *, value.c_str()),
                      OFstatic_cast(DWORD, value.length() * sizeof(tchar)));
    }


    static
    void
    regSetDword(HKEY hkey, const tstring& name, DWORD value)
    {
#if defined (DCMTK_OFLOG_UNICODE)
        RegSetValueEx(hkey,
#else
        RegSetValueExA(hkey,
#endif
                      name.c_str(),
                      0,
                      REG_DWORD,
                      OFreinterpret_cast(LPBYTE, &value),
                      OFstatic_cast(DWORD, sizeof(DWORD)));
    }

}



//////////////////////////////////////////////////////////////////////////////
// NTEventLogAppender ctor and dtor
//////////////////////////////////////////////////////////////////////////////

NTEventLogAppender::NTEventLogAppender(const tstring& server,
                                       const tstring& log,
                                       const tstring& source)
: server(server),
  log(log),
  source(source),
  hEventLog(NULL),
  pCurrentUserSID(NULL)
{
    init();
}



NTEventLogAppender::NTEventLogAppender(const helpers::Properties & properties)
: Appender(properties),
  server(properties.getProperty( DCMTK_LOG4CPLUS_TEXT("server") )),
  log(properties.getProperty( DCMTK_LOG4CPLUS_TEXT("log") )),
  source(properties.getProperty( DCMTK_LOG4CPLUS_TEXT("source") )),
  hEventLog(NULL),
  pCurrentUserSID(NULL)
{
    init();
}



void
NTEventLogAppender::init()
{
    if(source.empty()) {
        helpers::getLogLog().warn(
            DCMTK_LOG4CPLUS_TEXT("Source option not set for appender [")
            + name
            + DCMTK_LOG4CPLUS_TEXT("]."));
        return;
    }

    if(log.empty ()) {
        log = DCMTK_LOG4CPLUS_TEXT("Application");
    }

    // current user security identifier
    GetCurrentUserSID(&pCurrentUserSID);

    addRegistryInfo();

#if defined (DCMTK_OFLOG_UNICODE)
    hEventLog = ::RegisterEventSource(server.empty () ? 0 : server.c_str(),
        source.c_str());
#else
    hEventLog = ::RegisterEventSourceA(server.empty () ? 0 : server.c_str(),
        source.c_str());
#endif
    if (! hEventLog || hEventLog == HANDLE(ERROR_INVALID_HANDLE))
        helpers::getLogLog().warn (
            DCMTK_LOG4CPLUS_TEXT("Event source registration failed."));
}



NTEventLogAppender::~NTEventLogAppender()
{
    destructorImpl();

    if(pCurrentUserSID != NULL) {
        free (pCurrentUserSID);
        pCurrentUserSID = NULL;
    }
}



//////////////////////////////////////////////////////////////////////////////
// NTEventLogAppender public methods
//////////////////////////////////////////////////////////////////////////////

void
NTEventLogAppender::close()
{
    if(hEventLog != NULL) {
        ::DeregisterEventSource(hEventLog);
        hEventLog = NULL;
    }
    closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// NTEventLogAppender protected methods
//////////////////////////////////////////////////////////////////////////////

void
NTEventLogAppender::append(const spi::InternalLoggingEvent& event)
{
    if(hEventLog == NULL) {
        helpers::getLogLog().warn(DCMTK_LOG4CPLUS_TEXT("NT EventLog not opened."));
        return;
    }

    tstring & str = formatEvent (event);

    // From MSDN documentation for ReportEvent():
    // Each string is limited to 31,839 characters.
    if (str.size () > 31839)
        str.resize (31839);

    const tchar * s = str.c_str ();
#if defined (DCMTK_OFLOG_UNICODE)
    BOOL bSuccess = ::ReportEvent(hEventLog,
#else
    BOOL bSuccess = ::ReportEventA(hEventLog,
#endif
                                  getEventType(event),
                                  getEventCategory(event),
                                  0x1000,
                                  pCurrentUserSID,
                                  1,
                                  0,
                                  &s,
                                  NULL);

    if(!bSuccess) {
        helpers::getLogLog().error(
            DCMTK_LOG4CPLUS_TEXT("Cannot report event in NT EventLog."));
    }
}




WORD
NTEventLogAppender::getEventType(const spi::InternalLoggingEvent& event)
{
    WORD ret_val;
    LogLevel const ll = event.getLogLevel();

    if (ll >= ERROR_LOG_LEVEL) // or FATAL_LOG_LEVEL
        ret_val = EVENTLOG_ERROR_TYPE;
    else if (ll >= WARN_LOG_LEVEL)
        ret_val = EVENTLOG_WARNING_TYPE;
    else // INFO_LOG_LEVEL or DEBUG_LOG_LEVEL or TRACE_LOG_LEVEL
        ret_val = EVENTLOG_INFORMATION_TYPE;

    return ret_val;
}



WORD
NTEventLogAppender::getEventCategory(const spi::InternalLoggingEvent& event)
{
    WORD ret_val;
    LogLevel const ll = event.getLogLevel();

    if (ll >= FATAL_LOG_LEVEL)
        ret_val = 1;
    else if (ll >= ERROR_LOG_LEVEL)
        ret_val = 2;
    else if (ll >= WARN_LOG_LEVEL)
        ret_val = 3;
    else if (ll >= INFO_LOG_LEVEL)
        ret_val = 4;
    else if (ll >= DEBUG_LOG_LEVEL)
        ret_val = 5;
    else // TRACE_LOG_LEVEL
        ret_val = 6;

    return ret_val;
}


// Add this source with appropriate configuration keys to the registry.
void
NTEventLogAppender::addRegistryInfo()
{
    DWORD disposition;
    HKEY hkey = 0;
    tstring subkey =   DCMTK_LOG4CPLUS_TEXT("SYSTEM\\CurrentControlSet\\Services\\EventLog\\")
                     + log
                     + DCMTK_LOG4CPLUS_TEXT("\\")
                     + source;

    hkey = regGetKey(subkey, &disposition);
    if(disposition == REG_CREATED_NEW_KEY) {
        regSetString(hkey,
                     DCMTK_LOG4CPLUS_TEXT("EventMessageFile"),
                     DCMTK_LOG4CPLUS_TEXT("NTEventLogAppender.dll"));
        regSetString(hkey,
                     DCMTK_LOG4CPLUS_TEXT("CategoryMessageFile"),
                     DCMTK_LOG4CPLUS_TEXT("NTEventLogAppender.dll"));
        regSetDword(hkey, DCMTK_LOG4CPLUS_TEXT("TypesSupported"), OFstatic_cast(DWORD, 7));
        regSetDword(hkey, DCMTK_LOG4CPLUS_TEXT("CategoryCount"), OFstatic_cast(DWORD, 5));
    }

    RegCloseKey(hkey);
    return;
}


} // namespace log4cplus
} // end namespace dcmtk

#else

int oflog_ntelogap_cc_dummy_to_keep_linker_from_moaning = 0;

#endif // DCMTK_LOG4CPLUS_HAVE_NT_EVENT_LOG

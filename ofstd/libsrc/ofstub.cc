/*
 *
 *  Copyright (C) 2024-2025, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Main function for a proxy stub that simply calls another
 *   command line tool and forwards environment, command line arguments,
 *   standard I/O streams, and return code.
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstub.h"
#include "dcmtk/ofstd/ofwhere.h"
#include "dcmtk/ofstd/ofstd.h"

#ifdef DCMTK_USE_OFLOG_LOGGER_IN_STUB
#include "dcmtk/oflog/oflog.h"
#endif

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif /* HAVE_WINDOWS_H */

#define EXITCODE_CANNOT_DETERMINE_DIR        90
#define EXITCODE_EXEC_FAILED                 91
#define EXITCODE_ILLEGAL_PARAMS              92

#ifdef _WIN32

// The function argvToCommandLine() is derived from an implementation
// with the following copyright statement:
//
// Copyright (c) 2011-2015 Ryan Prichard
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

static OFString argvToCommandLine(int argc, char** argv)
{
    OFString result;
    for (int i = 0; i < argc; ++i)
    {
        if (i > 0)
            result.append(1, ' ');
        const char *arg = argv[i];
        OFBool quote_argument =
            strchr(arg, ' ') != NULL ||
            strchr(arg, '\t') != NULL ||
            *arg == '\0';
        if (quote_argument)
            result.append(1, '\"');
        int bsCount = 0;
        for (const char *p = arg; *p != '\0'; ++p)
        {
            if (*p == '\\')
            {
                bsCount++;
            }
            else if (*p == '\"')
            {
                result.append(bsCount * 2 + 1, '\\');
                result.append(1, '\"');
                bsCount = 0;
            }
            else
            {
                if (bsCount > 0) result.append(bsCount, '\\');
                bsCount = 0;
                result.append(1, *p);
            }
        }
        if (quote_argument)
        {
            if (bsCount > 0) result.append(bsCount * 2, '\\');
            result.append(1, '\"');
        }
        else
        {
            if (bsCount > 0) result.append(bsCount, '\\');
        }
    }
    return result;
}

//
// The function  GetLastErrorStdStr() is derived from an implementation
// by Orjan Westin published under the BSD license.
//
static OFString getLastErrorString()
{
  DWORD error = GetLastError();
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    if (bufLen)
    {
      LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
      OFString result(lpMsgStr, bufLen);
      LocalFree(lpMsgBuf);
      return result;
    }
  }
  return OFString();
}

#elif defined(__APPLE__)

#include <crt_externs.h>
#define environ (*_NSGetEnviron())

#else /* _WIN32 */

extern char** environ; // required to exist by the Single Unix Specification

#endif /* _WIN32 */


int OFstub_main(int argc, char** argv, const char *stubName, const char *appName)
{
    if ((argv==NULL) || (stubName==NULL) || (appName==NULL))
    {
      fprintf(stderr, "F: Illegal NULL parameters passed to OFstub_main\n");
      return EXITCODE_ILLEGAL_PARAMS;
    }

    long len = 43 - OFstatic_cast(long, strlen(stubName)) - OFstatic_cast(long, strlen(appName));
    if (len < 0) len = 0;

#ifdef DCMTK_USE_OFLOG_LOGGER_IN_STUB
    OFString logger_name = "dcmtk.apps.";
    logger_name += stubName;
    OFLogger logger = OFLog::getLogger(logger_name.c_str());

    OFString output =
        "##########################################################################\n"
        "#                                                                        #\n#";
    output.append(len/2, ' ');
    output.append(stubName);
    output.append(" is deprecated, use ");
    output.append(appName);
    output.append(" instead!");
    output.append(len - (len/2), ' ');
    output.append("#\n"
        "#                                                                        #\n"
        "##########################################################################\n");
      OFLOG_WARN(logger, output);
#else
    OFString output =
        "W: ##########################################################################\n"
        "W: #                                                                        #\nW: #";
    output.append(len/2, ' ');
    output.append(stubName);
    output.append(" is deprecated, use ");
    output.append(appName);
    output.append(" instead!");
    output.append(len - (len/2), ' ');
    output.append("#\n"
        "W: #                                                                        #\n"
        "W: ##########################################################################\n\n");

    fprintf(stderr, "%s", output.c_str());
#endif

    // get real path in which the stub executable is located
    int dirname_length = 0;
    int length = OFgetExecutablePath(NULL, 0, &dirname_length);
    if (length == 0)
    {
#ifdef DCMTK_USE_OFLOG_LOGGER_IN_STUB
      OFLOG_FATAL(logger, "Cannot determine location of " << stubName << " executable");
#else
      fprintf(stderr, "F: Cannot determine location of %s executable\n", stubName);
#endif
      return EXITCODE_CANNOT_DETERMINE_DIR;
    }

    char *path = new char[length + 1];
    OFgetExecutablePath(path, length, &dirname_length);
    path[length] = '\0';
    path[dirname_length] = '\0';
    OFString newpath = path;
    delete[] path;

    // add path separator and name of application to call
    newpath += PATH_SEPARATOR;
    newpath += appName;

#ifdef _WIN32

    // add ".exe" extension and quote command if necessary
    if (newpath.find_first_of(" \t") != OFString_npos)
        newpath = "\"" + newpath + ".exe\"";
        else newpath += ".exe";

    if (argc > 1)
    {
        // add arguments to new command line
        newpath += " " + argvToCommandLine(argc-1, argv+1);
    }

    // prepare data for CreateProcessA
    STARTUPINFOA siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));
    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION piProcInfo;

    // CreateProcess expects a writable char buffer as command line
    size_t pathlen = newpath.length() + 1;
    char *pathbuf = new char[pathlen];
    OFStandard::strlcpy(pathbuf, newpath.c_str(), pathlen);

    // Create the child process
    BOOL success = CreateProcessA(
       NULL, /* lpApplicationName */
       pathbuf,
       NULL, /* lpProcessAttributes */
       NULL, /* lpThreadAttributes */
       TRUE, /* bInheritHandles */
       0,    /* dwCreationFlags */
       NULL, /* lpEnvironment, NULL for inherit */
       NULL, /* lpCurrentDirectory, NULL for inherit */
       &siStartInfo,
       &piProcInfo);

    delete[] pathbuf;

    if (! success)
    {
        OFString message = getLastErrorString();
#ifdef DCMTK_USE_OFLOG_LOGGER_IN_STUB
        OFLOG_FATAL(logger, "Cannot execute " << newpath << ": " << message);
#else
        fprintf(stderr, "F: Cannot execute %s: %s\n", newpath.c_str(), message.c_str());
#endif
        return EXITCODE_EXEC_FAILED;
    }

    // Wait for the child process to finish, then terminate
    WaitForSingleObject( piProcInfo.hProcess, INFINITE );
    DWORD exit_code = 0;
    GetExitCodeProcess( piProcInfo.hProcess, &exit_code);

    // Close process and thread handles.
    CloseHandle( piProcInfo.hProcess );
    CloseHandle( piProcInfo.hThread );
    return OFstatic_cast(int, exit_code);

#else /* _WIN32 */

    // call the application for which we are a stub
    (void) execve(newpath.c_str(), argv, environ);

    // execv() only returns in case of an error, which is then stored in errno.
    char buf[256];
    const char *message = OFStandard::strerror(errno, buf, sizeof(buf));
    if (message == NULL) message = "(unknown error)";

#ifdef DCMTK_USE_OFLOG_LOGGER_IN_STUB
    OFLOG_FATAL(logger, "Cannot execute " << newpath << ": " << message);
#else
    fprintf(stderr, "F: Cannot execute %s: %s\n", newpath.c_str(), message);
#endif

  // if we have reached this point, then execve() has failed
  return EXITCODE_EXEC_FAILED;

#endif /* _WIN32 */
}

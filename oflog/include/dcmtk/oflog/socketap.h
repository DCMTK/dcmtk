// -*- C++ -*-
// Module:  LOG4CPLUS
// File:    socketap.h
// Created: 5/2003
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

/** @file */

#ifndef DCMTK_LOG4CPLUS_SOCKET_APPENDER_HEADER_
#define DCMTK_LOG4CPLUS_SOCKET_APPENDER_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/appender.h"
#include "dcmtk/oflog/helpers/socket.h"
#include "dcmtk/oflog/thread/syncprim.h"
#include "dcmtk/oflog/thread/threads.h"


namespace dcmtk
{
namespace log4cplus
{
 

#ifndef DCMTK_OFLOG_UNICODE
    size_t const DCMTK_LOG4CPLUS_MAX_MESSAGE_SIZE = 8*1024;
#else
    size_t const DCMTK_LOG4CPLUS_MAX_MESSAGE_SIZE = 2*8*1024;
#endif


    /**
     * Sends {@link spi::InternalLoggingEvent} objects to a remote a log server.
     *
     * The SocketAppender has the following properties:
     *
     * <ul>
     *
     *   <li>Remote logging is non-intrusive as far as the log event
     *   is concerned. In other words, the event will be logged with
     *   the same time stamp, NDC, location info as if it were logged
     *   locally by the client.
     *
     *   <li>SocketAppenders do not use a layout.
     *
     *   <li>Remote logging uses the TCP protocol. Consequently, if
     *   the server is reachable, then log events will eventually arrive
     *   at the server.
     *
     *   <li>If the remote server is down, the logging requests are
     *   simply dropped. However, if and when the server comes back up,
     *   then event transmission is resumed transparently. This
     *   transparent reconneciton is performed by a <em>connector</em>
     *   thread which periodically attempts to connect to the server.
     *
     *   <li>Logging events are automatically <em>buffered</em> by the
     *   native TCP implementation. This means that if the link to server
     *   is slow but still faster than the rate of (log) event production
     *   by the client, the client will not be affected by the slow
     *   network connection. However, if the network connection is slower
     *   then the rate of event production, then the client can only
     *   progress at the network rate. In particular, if the network link
     *   to the the server is down, the client will be blocked.
     * 
     *   <li>On the other hand, if the network link is up, but the server
     *   is down, the client will not be blocked when making log requests
     *   but the log events will be lost due to server unavailability.
     * </ul>
     *
     * <h3>Properties</h3>
     * <dl>
     * <dt><tt>host</tt></dt>
     * <dd>Remote host name to connect and send events to.</dd>
     *
     * <dt><tt>port</tt></dt>
     * <dd>Port on remote host to send events to.</dd>
     *
     * <dt><tt>ServerName</tt></dt>
     * <dd>Host name of event's origin prepended to each event.</dd>
     *
     * </dl>
     */
    class DCMTK_LOG4CPLUS_EXPORT SocketAppender : public Appender {
    public:
      // Ctors
        SocketAppender(const log4cplus::tstring& host, unsigned short port, 
                       const log4cplus::tstring& serverName = tstring());
        SocketAppender(const log4cplus::helpers::Properties & properties);

      // Dtor
        ~SocketAppender();

      // Methods
        virtual void close();

    protected:
        void openSocket();
        void initConnector ();
        virtual void append(const spi::InternalLoggingEvent& event);

      // Data
        log4cplus::helpers::Socket socket;
        log4cplus::tstring host;
        unsigned int port;
        log4cplus::tstring serverName;

#if ! defined (DCMTK_LOG4CPLUS_SINGLE_THREADED)
        class DCMTK_LOG4CPLUS_EXPORT ConnectorThread;
        friend class ConnectorThread;

        class DCMTK_LOG4CPLUS_EXPORT ConnectorThread
            : public thread::AbstractThread
        {
        public:
            ConnectorThread (SocketAppender &);
            virtual ~ConnectorThread ();

            virtual void run();

            void terminate ();
            void trigger ();

        protected:
            SocketAppender & sa;
            thread::ManualResetEvent trigger_ev;
            bool exit_flag;
        };

        volatile bool connected;
        helpers::SharedObjectPtr<ConnectorThread> connector;
#endif

    private:
      // Disallow copying of instances of this class
        SocketAppender(const SocketAppender&);
        SocketAppender& operator=(const SocketAppender&);
    };

    namespace helpers {
        DCMTK_LOG4CPLUS_EXPORT
        void convertToBuffer (SocketBuffer & buffer,
            const log4cplus::spi::InternalLoggingEvent& event,
            const log4cplus::tstring& serverName);

        DCMTK_LOG4CPLUS_EXPORT
        log4cplus::spi::InternalLoggingEvent readFromBuffer(SocketBuffer& buffer);
    } // end namespace helpers

} // end namespace log4cplus
} // end namespace dcmtk

#endif // DCMTK_LOG4CPLUS_SOCKET_APPENDER_HEADER_


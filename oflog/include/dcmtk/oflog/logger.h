// Module:  Log4CPLUS
// File:    logger.h
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2009 Tad E. Smith
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

/** @file
 * This header defines the Logger class and the logging macros. */

#ifndef DCMTK__LOG4CPLUS_LOGGERHEADER_
#define DCMTK__LOG4CPLUS_LOGGERHEADER_

#include "dcmtk/oflog/config.h"
#include "dcmtk/oflog/loglevel.h"
#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/streams.h"
#include "dcmtk/oflog/spi/apndatch.h"
#include "dcmtk/oflog/spi/logfact.h"

//#include <vector>


namespace dcmtk
{
namespace log4cplus
{
    // Forward declarations

    class Appender;
    class Hierarchy;
    class HierarchyLocker;
    class DefaultLoggerFactory;

    namespace spi
    {

        class LoggerImpl;

    }


    /**
     * This is the central class in the log4cplus package. One of the
     * distintive features of log4cplus are hierarchical loggers and their
     * evaluation.
     *
     * See the <a href="../../../../manual.html">user manual</a> for an
     * introduction on this class.
     */
    class DCMTK_LOG4CPLUS_EXPORT Logger
        : public spi::AppenderAttachable
    {
    public:
      // Static Methods
        /**
         * Returns <code>true </code>if the named logger exists
         * (in the default hierarchy).
         *
         * @param name The name of the logger to search for.
         */
        static bool exists(const tstring& name);

        /*
         * Returns all the currently defined loggers in the default
         * hierarchy.
         *
         * The root logger is <em>not</em> included in the returned
         * list.
        */
        static OFList<Logger> getCurrentLoggers();

        /**
         * Return the default Hierarchy instance.
         */
        static Hierarchy& getDefaultHierarchy();

        /**
         * Retrieve a logger with name <code>name</code>.  If the named
         * logger already exists, then the existing instance will be returned.
         * Otherwise, a new instance is created.
         *
         * By default, loggers do not have a set LogLevel but inherit
         * it from the hierarchy. This is one of the central features of
         * log4cplus.
         *
         * @param name The name of the logger to retrieve.
         */
        static Logger getInstance(const tstring& name);

        /**
         * Like getInstance() except that the type of logger
         * instantiated depends on the type returned by the {@link
         * spi::LoggerFactory#makeNewLoggerInstance} method of the
         * <code>factory</code> parameter.
         *
         * This method is intended to be used by sub-classes.
         *
         * @param name The name of the logger to retrieve.
         * @param factory A {@link spi::LoggerFactory} implementation that will
         * actually create a new Instance.
         */
        static Logger getInstance(const tstring& name, spi::LoggerFactory& factory);

        /**
         * Return the root of the default logger hierrachy.
         *
         * The root logger is always instantiated and available. It's
         * name is "root".
         *
         * Nevertheless, calling {@link #getInstance
         * Logger.getInstance("root")} does not retrieve the root logger
         * but a logger just under root named "root".
         */
        static Logger getRoot();

        /**
         * Calling this method will <em>safely</em> close and remove all
         * appenders in all the loggers including root contained in the
         * default hierachy.
         *
         * Some appenders such as SocketAppender need to be closed before the
         * application exits. Otherwise, pending logging events might be
         * lost.
         *
         * The <code>shutdown</code> method is careful to close nested
         * appenders before closing regular appenders. This is allows
         * configurations where a regular appender is attached to a logger
         * and again to a nested appender.
         */
        static void shutdown();

      // Non-Static Methods
        /**
         * If <code>assertionVal</code> parameter is <code>false</code>, then
         * logs <code>msg</code> with FATAL_LOG_LEVEL log level.
         *
         * @param assertionVal Truth value of assertion condition.
         * @param msg The message to print if <code>assertion</code> is
         * false.
         */
        void assertion(bool assertionVal, const tstring& msg) const;

        /**
         * Close all attached appenders implementing the AppenderAttachable
         * interface.
         */
        void closeNestedAppenders() const;

        /**
         * Check whether this logger is enabled for a given
         * LogLevel passed as parameter.
         *
         * @return boolean True if this logger is enabled for <code>ll</code>.
         */
        bool isEnabledFor(LogLevel ll) const;

        /**
         * This generic form is intended to be used by wrappers.
         */
        void log(LogLevel ll, const tstring& message,
                 const char* file=NULL, int line=-1,
                 const char* function=NULL) const;

        /**
         * This method creates a new logging event and logs the event
         * without further checks.
         */
        void forcedLog(LogLevel ll, const tstring& message,
                       const char* file=NULL, int line=-1,
                       const char* function=NULL) const;

        /**
         * Call the appenders in the hierrachy starting at
         * <code>this</code>.  If no appenders could be found, emit a
         * warning.
         *
         * This method calls all the appenders inherited from the
         * hierarchy circumventing any evaluation of whether to log or not
         * to log the particular log request.
         *
         * @param event the event to log.
         */
        void callAppenders(const spi::InternalLoggingEvent& event) const;

        /**
         * Starting from this logger, search the logger hierarchy for a
         * "set" LogLevel and return it. Otherwise, return the LogLevel of the
         * root logger.
         *
         * The Logger class is designed so that this method executes as
         * quickly as possible.
         */
        LogLevel getChainedLogLevel() const;

        /**
         * Returns the assigned LogLevel, if any, for this Logger.
         *
         * @return LogLevel - the assigned LogLevel, can be <code>NOT_SET_LOG_LEVEL</code>.
         */
        LogLevel getLogLevel() const;

        /**
         * Set the LogLevel of this Logger.
         */
        void setLogLevel(LogLevel ll);

        /**
         * Return the the {@link Hierarchy} where this <code>Logger</code> instance is
         * attached.
         */
        Hierarchy& getHierarchy() const;

        /**
         * Return the logger name.
         */
        tstring getName() const;

        /**
         * Get the additivity flag for this Logger instance.
         */
        bool getAdditivity() const;

        /**
         * Set the additivity flag for this Logger instance.
         */
        void setAdditivity(bool additive);

      // AppenderAttachable Methods
        virtual void addAppender(SharedAppenderPtr newAppender);

        virtual SharedAppenderPtrList getAllAppenders();

        virtual SharedAppenderPtr getAppender(const tstring& name);

        virtual void removeAllAppenders();

        virtual void removeAppender(SharedAppenderPtr appender);

        virtual void removeAppender(const tstring& name);

        Logger ();
        Logger(const Logger& rhs);
        Logger& operator=(const Logger& rhs);
        virtual ~Logger();

        void swap (Logger &);

        /**
         * Used to retrieve the parent of this Logger in the
         * Logger tree.
         */
        Logger getParent() const;

    protected:
      // Data
        /** This is a pointer to the implementation class. */
        spi::LoggerImpl * value;

    private:
      // Ctors
        /**
         * This constructor created a new <code>Logger</code> instance
         * with a pointer to a Logger implementation.
         *
         * You should not create loggers directly.
         *
         * @param ptr A pointer to the Logger implementation.  This value
         *            cannot be NULL.
         */
        Logger(spi::LoggerImpl * ptr);

      // Friends
        friend class spi::LoggerImpl;
        friend class Hierarchy;
        friend class HierarchyLocker;
        friend class DefaultLoggerFactory;
    };


    /** \typedef OFList<Logger> LoggerList
     * This is a list of {@link Logger Loggers}. */
    typedef OFList<Logger> LoggerList;
    typedef OFListIterator(Logger) LoggerListIterator;


    /**
     * This class is used to create the default implementation of
     * the Logger class
     */
    class DCMTK_LOG4CPLUS_EXPORT DefaultLoggerFactory : public spi::LoggerFactory {
    public:
        Logger makeNewLoggerInstance(const tstring& name, Hierarchy& h);
    };



    /**
     * This class is used to produce "Trace" logging.  When an instance of
     * this class is created, it will log a <code>"ENTER: " + msg</code>
     * log message if TRACE_LOG_LEVEL is enabled for <code>logger</code>.
     * When an instance of this class is destroyed, it will log a
     * <code>"ENTER: " + msg</code> log message if TRACE_LOG_LEVEL is enabled
     * for <code>logger</code>.
     *
     * @see DCMTK_LOG4CPLUS_TRACE
     */
    class TraceLogger
    {
    public:
        TraceLogger(const Logger& l, const tstring& _msg,
                    const char* _file=NULL, int _line=-1)
          : logger(l), msg(_msg), file(_file), line(_line)
        { if(logger.isEnabledFor(TRACE_LOG_LEVEL))
              logger.forcedLog(TRACE_LOG_LEVEL, DCMTK_LOG4CPLUS_TEXT("ENTER: ") + msg, file, line);
        }

        ~TraceLogger()
        { if(logger.isEnabledFor(TRACE_LOG_LEVEL))
              logger.forcedLog(TRACE_LOG_LEVEL, DCMTK_LOG4CPLUS_TEXT("EXIT:  ") + msg, file, line);
        }

    private:
        TraceLogger(const TraceLogger&);
        TraceLogger& operator=(const TraceLogger&);
        Logger logger;
        tstring msg;
        const char* file;
        int line;
    };

} // end namespace log4cplus
} // end namespace dcmtk


#include "dcmtk/oflog/logmacro.h"


#endif // DCMTK__LOG4CPLUS_LOGGERHEADER_


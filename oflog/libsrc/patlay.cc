// Module:  Log4CPLUS
// File:    patternlayout.cxx
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

#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/logmacro.h"
#include "dcmtk/oflog/helpers/loglog.h"
#include "dcmtk/oflog/helpers/timehelp.h"
#include "dcmtk/oflog/helpers/strhelp.h"
#include "dcmtk/oflog/helpers/socket.h"
#include "dcmtk/oflog/helpers/property.h"
#include "dcmtk/oflog/spi/logevent.h"
#include "dcmtk/oflog/internal/internal.h"
#include "dcmtk/oflog/internal/env.h"
#include <cstdlib>


namespace
{


static
dcmtk::log4cplus::tstring
get_basename (const dcmtk::log4cplus::tstring& filename)
{
#if defined(_WIN32)
    dcmtk::log4cplus::tchar const dir_sep(DCMTK_LOG4CPLUS_TEXT('\\'));
#else
    dcmtk::log4cplus::tchar const dir_sep(DCMTK_LOG4CPLUS_TEXT('/'));
#endif

    dcmtk::log4cplus::tstring::size_type pos = filename.rfind(dir_sep);
    if (pos != OFString_npos)
        return filename.substr(pos+1);
    else
        return filename;
}


} // namespace


namespace dcmtk
{
namespace log4cplus
{

static tchar const ESCAPE_CHAR = DCMTK_LOG4CPLUS_TEXT('%');

extern void formatRelativeTimestamp (log4cplus::tostream & output,
    log4cplus::spi::InternalLoggingEvent const & event);


namespace pattern
{


/**
 * This is used by PatternConverter class to inform them how to format
 * their output.
 */
struct FormattingInfo {
    int minLen;
    size_t maxLen;
    bool leftAlign;
    FormattingInfo()
        : minLen(), maxLen(), leftAlign()
    { reset(); }

    void reset();
    void dump(helpers::LogLog&);
};



/**
 * This is the base class of all "Converter" classes that format a
 * field of InternalLoggingEvent objects.  In fact, the PatternLayout
 * class simply uses an array of PatternConverter objects to format
 * and append a logging event.
 */
class PatternConverter
{
public:
    explicit PatternConverter(const FormattingInfo& info);
    virtual ~PatternConverter() {}
    void formatAndAppend(tostream& output,
        const spi::InternalLoggingEvent& event);

    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event) = 0;

private:
    int minLen;
    size_t maxLen;
    bool leftAlign;
};


typedef OFVector<pattern::PatternConverter*> PatternConverterList;


/**
 * This PatternConverter returns a constant string.
 */
class LiteralPatternConverter : public PatternConverter
{
public:
    LiteralPatternConverter(const tstring& str);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent&)
    {
        result = str;
    }

private:
    tstring str;
};


/**
 * This PatternConverter is used to format most of the "simple" fields
 * found in the InternalLoggingEvent object.
 */
class BasicPatternConverter
    : public PatternConverter
{
public:
    enum Type { THREAD_CONVERTER,
                THREAD2_CONVERTER,
                PROCESS_CONVERTER,
                LOGLEVEL_CONVERTER,
                LOGLEVEL_PREFIX_CONVERTER,
                NDC_CONVERTER,
                MESSAGE_CONVERTER,
                NEWLINE_CONVERTER,
                BASENAME_CONVERTER,
                FILE_CONVERTER,
                LINE_CONVERTER,
                FULL_LOCATION_CONVERTER,
                FUNCTION_CONVERTER };
    BasicPatternConverter(const FormattingInfo& info, Type type);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
  // Disable copy
    BasicPatternConverter(const BasicPatternConverter&);
    BasicPatternConverter& operator=(BasicPatternConverter&);

    LogLevelManager& llmCache;
    Type type;
};



/**
 * This PatternConverter is used to format the Logger field found in
 * the InternalLoggingEvent object.
 */
class LoggerPatternConverter : public PatternConverter {
public:
    LoggerPatternConverter(const FormattingInfo& info, int precision);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    int precision;
};



/**
 * This PatternConverter is used to format the timestamp field found in
 * the InternalLoggingEvent object.  It will be formatted according to
 * the specified "pattern".
 */
class DatePatternConverter : public PatternConverter {
public:
    DatePatternConverter(const FormattingInfo& info,
                         const tstring& pattern,
                         bool use_gmtime);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    bool use_gmtime;
    tstring format;
};


//! This pattern is used to format milliseconds since process start.
class RelativeTimestampConverter: public PatternConverter {
public:
    RelativeTimestampConverter(const FormattingInfo& info);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);
};


/**
 * This PatternConverter is used to format the hostname field.
 */
class HostnamePatternConverter : public PatternConverter {
public:
    HostnamePatternConverter(const FormattingInfo& info, bool fqdn);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    tstring hostname_;
};


/**
 * This PatternConverter is used to format the MDC field found in
 * the InternalLoggingEvent object, optionally limited to
 * \c k Mapped diagnostic context key.
 */
class MDCPatternConverter
    : public PatternConverter
{
public:
    MDCPatternConverter(const FormattingInfo& info, tstring const & k);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    tstring key;
};


/**
 * This PatternConverter is used to format the NDC field found in
 * the InternalLoggingEvent object, optionally limited to
 * \c precision levels (using space to separate levels).
 */
class NDCPatternConverter : public PatternConverter {
public:
    NDCPatternConverter(const FormattingInfo& info, int precision);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    int precision;
};



/**
 * This class parses a "pattern" string into an array of
 * PatternConverter objects.
 * <p>
 * @see PatternLayout for the formatting of the "pattern" string.
 */
class PatternParser
{
public:
    PatternParser(const tstring& pattern, unsigned ndcMaxDepth);
    OFVector<PatternConverter*> parse();

private:
  // Types
    enum ParserState { LITERAL_STATE,
                       CONVERTER_STATE,
                       DOT_STATE,
                       MIN_STATE,
                       MAX_STATE };

  // Methods
    tstring extractOption();
    int extractPrecisionOption();
    void finalizeConverter(tchar c);

  // Data
    tstring pattern;
    FormattingInfo formattingInfo;
    OFVector<PatternConverter*> list;
    ParserState state;
    tstring::size_type pos;
    tstring currentLiteral;
    unsigned ndcMaxDepth;
};


////////////////////////////////////////////////
// FormattingInfo methods:
////////////////////////////////////////////////

void
FormattingInfo::reset() {
    minLen = -1;
    maxLen = 0x7FFFFFFF;
    leftAlign = false;
}


void
FormattingInfo::dump(helpers::LogLog& loglog) {
    tostringstream buf;
    buf << DCMTK_LOG4CPLUS_TEXT("min=") << minLen
        << DCMTK_LOG4CPLUS_TEXT(", max=") << maxLen
        << DCMTK_LOG4CPLUS_TEXT(", leftAlign=") << STD_NAMESPACE boolalpha << leftAlign;
    loglog.debug(OFString(buf.str().c_str(), buf.str().length()));
}




////////////////////////////////////////////////
// PatternConverter methods:
////////////////////////////////////////////////

PatternConverter::PatternConverter(const FormattingInfo& i)
    : minLen(i.minLen)
    , maxLen(i.maxLen)
    , leftAlign(i.leftAlign)
{
}



void
PatternConverter::formatAndAppend(
    tostream& output, const spi::InternalLoggingEvent& event)
{
    tstring & s = internal::get_ptd ()->faa_str;
    convert (s, event);
    size_t len = s.length();

    if (len > maxLen)
        output << s.substr(len - maxLen);
    else if (OFstatic_cast(int, len) < minLen)
    {
/*
        STD_NAMESPACE ios_base::fmtflags const original_flags = output.flags ();
        tchar const fill = output.fill (DCMTK_LOG4CPLUS_TEXT(' '));
        output.setf (leftAlign ? STD_NAMESPACE ios_base::left : STD_NAMESPACE ios_base::right,
            STD_NAMESPACE ios_base::adjustfield);
        output.width (minLen);
        output << s;
        output.fill (fill);
        output.flags (original_flags);
*/
        // use implementation from log4cplus 1.0.x since the above code does not work correctly
        if(leftAlign) {
            output << s;
            output << tstring(minLen - len, DCMTK_LOG4CPLUS_TEXT(' '));
        }
        else {
            output << tstring(minLen - len, DCMTK_LOG4CPLUS_TEXT(' '));
            output << s;
        }
    }
    else
        output << s;
}



////////////////////////////////////////////////
// LiteralPatternConverter methods:
////////////////////////////////////////////////

LiteralPatternConverter::LiteralPatternConverter(
    const tstring& str_)
    : PatternConverter(FormattingInfo())
    , str(str_)
{
}



////////////////////////////////////////////////
// BasicPatternConverter methods:
////////////////////////////////////////////////

BasicPatternConverter::BasicPatternConverter(
    const FormattingInfo& info, Type type_)
    : PatternConverter(info)
    , llmCache(getLogLevelManager())
    , type(type_)
{
}



void
BasicPatternConverter::convert(tstring & result,
    const spi::InternalLoggingEvent& event)
{
    switch(type)
    {
    case LOGLEVEL_CONVERTER:
        result = llmCache.toString(event.getLogLevel());
        return;

    case LOGLEVEL_PREFIX_CONVERTER:
        result = llmCache.toString(event.getLogLevel()).substr(0, 1);
        return;

    case BASENAME_CONVERTER:
        result = get_basename(event.getFile());
        return;

    case PROCESS_CONVERTER:
        helpers::convertIntegerToString(result, internal::get_process_id ());
        return;

    case NDC_CONVERTER:
        result = event.getNDC();
        return;

    case MESSAGE_CONVERTER:
        result = event.getMessage();
        return;

    case NEWLINE_CONVERTER:
        result = DCMTK_LOG4CPLUS_TEXT("\n");
        return;

    case FILE_CONVERTER:
        result = event.getFile();
        return;

    case THREAD_CONVERTER:
        result = event.getThread();
        return;

    case THREAD2_CONVERTER:
        result = event.getThread2();
        return;

    case LINE_CONVERTER:
        {
            if(event.getLine() != -1)
                helpers::convertIntegerToString(result, event.getLine());
            else
                result.clear ();
            return;
        }

    case FULL_LOCATION_CONVERTER:
        {
            tstring const & file = event.getFile();
            if (! file.empty ())
            {
                result = file;
                result += DCMTK_LOG4CPLUS_TEXT(":");
                result += helpers::convertIntegerToString(event.getLine());
            }
            else
                result = DCMTK_LOG4CPLUS_TEXT(":");
            return;
        }

    case FUNCTION_CONVERTER:
        result = event.getFunction ();
        return;
    }

    result = DCMTK_LOG4CPLUS_TEXT("INTERNAL LOG4CPLUS ERROR");
}



////////////////////////////////////////////////
// LoggerPatternConverter methods:
////////////////////////////////////////////////

LoggerPatternConverter::LoggerPatternConverter(
    const FormattingInfo& info, int prec)
    : PatternConverter(info)
    , precision(prec)
{
}



void
LoggerPatternConverter::convert(tstring & result,
    const spi::InternalLoggingEvent& event)
{
    const tstring& name = event.getLoggerName();
    if (precision <= 0) {
        result = name;
    }
    else {
        size_t len = name.length();

        // We subtract 1 from 'len' when assigning to 'end' to avoid out of
        // bounds exception in return r.substring(end+1, len). This can happen
        // if precision is 1 and the logger name ends with a dot.
        tstring::size_type end = len - 1;
        for (int i = precision; i > 0; --i)
        {
            end = name.rfind(DCMTK_LOG4CPLUS_TEXT('.'), end - 1);
            if(end == OFString_npos) {
                result = name;
                return;
            }
        }
        result = name.substr(end + 1);
    }
}



////////////////////////////////////////////////
// DatePatternConverter methods:
////////////////////////////////////////////////


DatePatternConverter::DatePatternConverter(
    const FormattingInfo& info, const tstring& pattern,
    bool use_gmtime_)
    : PatternConverter(info)
    , use_gmtime(use_gmtime_)
    , format(pattern)
{
}



void
DatePatternConverter::convert(tstring & result,
    const spi::InternalLoggingEvent& event)
{
    result = event.getTimestamp().getFormattedTime(format, use_gmtime);
}


//
//
//

RelativeTimestampConverter::RelativeTimestampConverter (FormattingInfo const & info)
    : PatternConverter (info)
{ }


void
RelativeTimestampConverter::convert (tstring & result,
    spi::InternalLoggingEvent const & event)
{
    tostringstream & oss = internal::get_ptd ()->layout_oss;
    detail::clear_tostringstream (oss);
    formatRelativeTimestamp (oss, event);
    // oss.str ().swap (result);
    result = OFString(oss.str().c_str(), oss.str().length());
}


////////////////////////////////////////////////
// HostnamePatternConverter methods:
////////////////////////////////////////////////

HostnamePatternConverter::HostnamePatternConverter (
    const FormattingInfo& info, bool fqdn)
    : PatternConverter(info)
    , hostname_ (helpers::getHostname (fqdn))
{ }


void
HostnamePatternConverter::convert (
    tstring & result, const spi::InternalLoggingEvent&)
{
    result = hostname_;
}



////////////////////////////////////////////////
// MDCPatternConverter methods:
////////////////////////////////////////////////

log4cplus::pattern::MDCPatternConverter::MDCPatternConverter (
    const FormattingInfo& info, tstring const & k)
    : PatternConverter(info)
    , key (k)
{ }


void
log4cplus::pattern::MDCPatternConverter::convert (tstring & result,
    const spi::InternalLoggingEvent& event)
{
    result = event.getMDC (key);
}


////////////////////////////////////////////////
// NDCPatternConverter methods:
////////////////////////////////////////////////

log4cplus::pattern::NDCPatternConverter::NDCPatternConverter (
    const FormattingInfo& info, int precision_)
    : PatternConverter(info)
    , precision(precision_)
{ }


void
log4cplus::pattern::NDCPatternConverter::convert (tstring & result,
    const spi::InternalLoggingEvent& event)
{
    const log4cplus::tstring& text = event.getNDC();
    if (precision <= 0)
        result = text;
    else
    {
        tstring::size_type p = text.find(DCMTK_LOG4CPLUS_TEXT(' '));
        for (int i = 1; i < precision && p != OFString_npos; ++i)
            p = text.find(DCMTK_LOG4CPLUS_TEXT(' '), p + 1);

        result = text.substr(0, p);
    }
}



////////////////////////////////////////////////
// PatternParser methods:
////////////////////////////////////////////////

PatternParser::PatternParser(
    const tstring& pattern_, unsigned ndcMaxDepth_)
    : pattern(pattern_)
    , formattingInfo()
    , list()
    , state(LITERAL_STATE)
    , pos(0)
    , currentLiteral()
    , ndcMaxDepth (ndcMaxDepth_)
{
}



tstring
PatternParser::extractOption()
{
    if (   (pos < pattern.length())
        && (pattern[pos] == DCMTK_LOG4CPLUS_TEXT('{')))
    {
        tstring::size_type end = pattern.find_first_of(DCMTK_LOG4CPLUS_TEXT('}'), pos);
        if (end != OFString_npos) {
            tstring r = pattern.substr(pos + 1, end - pos - 1);
            pos = end + 1;
            return r;
        }
        else {
            log4cplus::tostringstream buf;
            buf << DCMTK_LOG4CPLUS_TEXT("No matching '}' found in conversion pattern string \"")
                << pattern
                << DCMTK_LOG4CPLUS_TEXT("\"");
            helpers::getLogLog().error(OFString(buf.str().c_str(), buf.str().length()));
            pos = pattern.length();
        }
    }

    return DCMTK_LOG4CPLUS_TEXT("");
}


int
PatternParser::extractPrecisionOption()
{
    tstring opt = extractOption();
    int r = 0;
    if (! opt.empty ())
        r = atoi(DCMTK_LOG4CPLUS_TSTRING_TO_STRING(opt).c_str());

    return r;
}



PatternConverterList
PatternParser::parse()
{
    tchar c;
    pos = 0;
    while(pos < pattern.length()) {
        c = pattern[pos++];
        switch (state) {
        case LITERAL_STATE :
            // In literal state, the last char is always a literal.
            if(pos == pattern.length()) {
                currentLiteral += c;
                continue;
            }
            if(c == ESCAPE_CHAR) {
                // peek at the next char.
                switch (pattern[pos]) {
                case ESCAPE_CHAR:
                    currentLiteral += c;
                    pos++; // move pointer
                    break;
                default:
                    if(! currentLiteral.empty ()) {
                        list.push_back
                             (new LiteralPatternConverter(currentLiteral));
                        //getLogLog().debug("Parsed LITERAL converter: \""
                        //                  +currentLiteral+"\".");
                    }
                    currentLiteral.resize(0);
                    currentLiteral += c; // append %
                    state = CONVERTER_STATE;
                    formattingInfo.reset();
                }
            }
            else {
                currentLiteral += c;
            }
            break;

        case CONVERTER_STATE:
            currentLiteral += c;
            switch (c) {
            case DCMTK_LOG4CPLUS_TEXT('-'):
                formattingInfo.leftAlign = true;
                break;
            case DCMTK_LOG4CPLUS_TEXT('.'):
                state = DOT_STATE;
                break;
            default:
                if(c >= DCMTK_LOG4CPLUS_TEXT('0') && c <= DCMTK_LOG4CPLUS_TEXT('9')) {
                    formattingInfo.minLen = c - DCMTK_LOG4CPLUS_TEXT('0');
                    state = MIN_STATE;
                }
                else {
                    finalizeConverter(c);
                }
            } // switch
            break;

        case MIN_STATE:
            currentLiteral += c;
            if (c >= DCMTK_LOG4CPLUS_TEXT('0') && c <= DCMTK_LOG4CPLUS_TEXT('9')) {
                formattingInfo.minLen = formattingInfo.minLen * 10 + (c - DCMTK_LOG4CPLUS_TEXT('0'));
            }
            else if(c == DCMTK_LOG4CPLUS_TEXT('.')) {
                state = DOT_STATE;
            }
            else {
                finalizeConverter(c);
            }
            break;

        case DOT_STATE:
            currentLiteral += c;
            if(c >= DCMTK_LOG4CPLUS_TEXT('0') && c <= DCMTK_LOG4CPLUS_TEXT('9')) {
                formattingInfo.maxLen = c - DCMTK_LOG4CPLUS_TEXT('0');
                state = MAX_STATE;
            }
            else {
                tostringstream buf;
                buf << DCMTK_LOG4CPLUS_TEXT("Error occurred in position ")
                    << pos
                    << DCMTK_LOG4CPLUS_TEXT(".\n Was expecting digit, instead got char \"")
                    << c
                    << DCMTK_LOG4CPLUS_TEXT("\".");
                helpers::getLogLog().error(OFString(buf.str().c_str(), buf.str().length()));
                state = LITERAL_STATE;
            }
            break;

         case MAX_STATE:
            currentLiteral += c;
            if (c >= DCMTK_LOG4CPLUS_TEXT('0') && c <= DCMTK_LOG4CPLUS_TEXT('9'))
                formattingInfo.maxLen = formattingInfo.maxLen * 10 + (c - DCMTK_LOG4CPLUS_TEXT('0'));
            else {
                finalizeConverter(c);
                state = LITERAL_STATE;
            }
            break;
        } // end switch
    } // end while

    if(! currentLiteral.empty ()) {
        list.push_back(new LiteralPatternConverter(currentLiteral));
      //getLogLog().debug("Parsed LITERAL converter: \""+currentLiteral+"\".");
    }

    return list;
}



void
PatternParser::finalizeConverter(tchar c)
{
    PatternConverter* pc = 0;
    switch (c) {
        case DCMTK_LOG4CPLUS_TEXT('b'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::BASENAME_CONVERTER);
            //getLogLog().debug("BASENAME converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('c'):
            pc = new LoggerPatternConverter(formattingInfo,
                                            extractPrecisionOption());
            //getLogLog().debug( DCMTK_LOG4CPLUS_TEXT("LOGGER converter.") );
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('d'):
        case DCMTK_LOG4CPLUS_TEXT('D'):
            {
                tstring dOpt = extractOption();
                if(dOpt.empty ()) {
                    dOpt = DCMTK_LOG4CPLUS_TEXT("%Y-%m-%d %H:%M:%S");
                }
                bool use_gmtime = c == DCMTK_LOG4CPLUS_TEXT('d');
                pc = new DatePatternConverter(formattingInfo, dOpt, use_gmtime);
                //if(use_gmtime) {
                //    getLogLog().debug("GMT DATE converter.");
                //}
                //else {
                //    getLogLog().debug("LOCAL DATE converter.");
                //}
                //formattingInfo.dump(getLogLog());
            }
            break;

        case DCMTK_LOG4CPLUS_TEXT('F'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::FILE_CONVERTER);
            //getLogLog().debug("FILE NAME converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('h'):
        case DCMTK_LOG4CPLUS_TEXT('H'):
            {
                bool fqdn = (c == DCMTK_LOG4CPLUS_TEXT('H'));
                pc = new HostnamePatternConverter(formattingInfo, fqdn);
                // getLogLog().debug( DCMTK_LOG4CPLUS_TEXT("HOSTNAME converter.") );
                // formattingInfo.dump(getLogLog());
            }
            break;

        case DCMTK_LOG4CPLUS_TEXT('i'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::PROCESS_CONVERTER);
            //getLogLog().debug("PROCESS_CONVERTER converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('l'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::FULL_LOCATION_CONVERTER);
            //getLogLog().debug("FULL LOCATION converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('L'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::LINE_CONVERTER);
            //getLogLog().debug("LINE NUMBER converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('m'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::MESSAGE_CONVERTER);
            //getLogLog().debug("MESSAGE converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('M'):
            pc = new BasicPatternConverter (
                formattingInfo, BasicPatternConverter::FUNCTION_CONVERTER);
            //getLogLog().debug("METHOD (function name) converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('n'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::NEWLINE_CONVERTER);
            //getLogLog().debug("MESSAGE converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('p'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::LOGLEVEL_CONVERTER);
            //getLogLog().debug("LOGLEVEL converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('P'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::LOGLEVEL_PREFIX_CONVERTER);
            //getLogLog().debug("LOGLEVEL converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('r'):
            pc = new RelativeTimestampConverter (formattingInfo);
            //getLogLog().debug("RELATIVE converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('t'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::THREAD_CONVERTER);
            //getLogLog().debug("THREAD converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('T'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::THREAD2_CONVERTER);
            //getLogLog().debug("THREAD2 converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case DCMTK_LOG4CPLUS_TEXT('x'):
            pc = new NDCPatternConverter (formattingInfo, ndcMaxDepth);
            //getLogLog().debug("NDC converter.");
            break;

        case DCMTK_LOG4CPLUS_TEXT('X'):
            pc = new MDCPatternConverter (formattingInfo, extractOption ());
            //getLogLog().debug("MDC converter.");
            break;

        default:
            tostringstream buf;
            buf << DCMTK_LOG4CPLUS_TEXT("Unexpected char [")
                << c
                << DCMTK_LOG4CPLUS_TEXT("] at position ")
                << pos
                << DCMTK_LOG4CPLUS_TEXT(" in conversion pattern.");
            helpers::getLogLog().error(OFString(buf.str().c_str(), buf.str().length()));
            pc = new LiteralPatternConverter(currentLiteral);
    }

    list.push_back(pc);
    currentLiteral.resize(0);
    state = LITERAL_STATE;
    formattingInfo.reset();
}


} // namespace pattern


typedef pattern::PatternConverterList PatternConverterList;


////////////////////////////////////////////////
// PatternLayout methods:
////////////////////////////////////////////////

PatternLayout::PatternLayout(const tstring& pattern_, bool formatEachLine_)
    : pattern()
    , formatEachLine()
    , parsedPattern()
{
    init(pattern_, formatEachLine_, 0);
}


PatternLayout::PatternLayout(const helpers::Properties& properties)
    : pattern()
    , formatEachLine()
    , parsedPattern()
{
    unsigned ndcMaxDepth = 0;
    bool formatEachLine_ = true;
    properties.getUInt (ndcMaxDepth, DCMTK_LOG4CPLUS_TEXT ("NDCMaxDepth"));
    properties.getBool(formatEachLine_, DCMTK_LOG4CPLUS_TEXT("FormatEachLine"));

    bool hasPattern = properties.exists( DCMTK_LOG4CPLUS_TEXT("Pattern") );
    bool hasConversionPattern = properties.exists( DCMTK_LOG4CPLUS_TEXT("ConversionPattern") );

    if(hasPattern) {
        helpers::getLogLog().warn(
            DCMTK_LOG4CPLUS_TEXT("PatternLayout- the \"Pattern\" property has been")
            DCMTK_LOG4CPLUS_TEXT(" deprecated.  Use \"ConversionPattern\" instead."));
    }

    if(hasConversionPattern) {
        init(properties.getProperty( DCMTK_LOG4CPLUS_TEXT("ConversionPattern") ), formatEachLine_,
            ndcMaxDepth);
    }
    else if(hasPattern) {
        init(properties.getProperty( DCMTK_LOG4CPLUS_TEXT("Pattern") ), formatEachLine_, ndcMaxDepth);
    }
    else {
        helpers::getLogLog().error(
            DCMTK_LOG4CPLUS_TEXT ("ConversionPattern not specified in properties"),
            true);
    }

}


void
PatternLayout::init(const tstring& pattern_, bool formatEachLine_, unsigned ndcMaxDepth)
{
    pattern = pattern_;
    formatEachLine = formatEachLine_;
    parsedPattern = pattern::PatternParser(pattern, ndcMaxDepth).parse();

    // Let's validate that our parser didn't give us any NULLs.  If it did,
    // we will convert them to a valid PatternConverter that does nothing so
    // at least we don't core.
    for(PatternConverterList::iterator it=parsedPattern.begin();
        it!=parsedPattern.end();
        ++it)
    {
        if( (*it) == 0 ) {
            helpers::getLogLog().error(
                DCMTK_LOG4CPLUS_TEXT("Parsed Pattern created a NULL PatternConverter"));
            (*it) = new pattern::LiteralPatternConverter( DCMTK_LOG4CPLUS_TEXT("") );
        }
    }
    if(parsedPattern.empty ()) {
        helpers::getLogLog().warn(
            DCMTK_LOG4CPLUS_TEXT("PatternLayout pattern is empty.  Using default..."));
        parsedPattern.push_back (
            new pattern::BasicPatternConverter(pattern::FormattingInfo(),
            pattern::BasicPatternConverter::MESSAGE_CONVERTER));
    }
}



PatternLayout::~PatternLayout()
{
    for(PatternConverterList::iterator it=parsedPattern.begin();
        it!=parsedPattern.end();
        ++it)
    {
        delete (*it);
    }
}



void
PatternLayout::formatAndAppend(tostream& output,
                               const spi::InternalLoggingEvent& event)
{
    if (formatEachLine && event.getMessage().find('\n') != OFString_npos)
    {
        size_t pos = 0;
        size_t last_pos = 0;

        while (pos != OFString_npos)
        {
            pos = event.getMessage().find('\n', last_pos);

            // Create a substring from just this single line
            tstring tmp_message(event.getMessage().substr(last_pos, (pos == OFString_npos) ? pos : pos - last_pos));

            // Then create a temporary InternalLoggingEvent for this one line
            spi::InternalLoggingEvent tmp_event(event.getLoggerName(), event.getLogLevel(),
                event.getNDC(), event.getMDCCopy(), tmp_message, event.getThread(),
                event.getTimestamp(), event.getFile(), event.getLine());
            tmp_event.setFunction(event.getFunction());

            // And finally, log this single line
            formatAndAppend(output, tmp_event);

            // Skip the "\n"
            last_pos = pos + 1;
        }
    }
    else
    {
        for(PatternConverterList::iterator it=parsedPattern.begin();
            it!=parsedPattern.end();
            ++it)
        {
            (*it)->formatAndAppend(output, event);
        }
    }
}


} // namespace log4cplus
} // end namespace dcmtk

/*
 *
 *  Copyright (C) 2011-2025, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This code is inspired by quicktest.
 *    Copyright (C) 2005-2008
 *    Tyler Streeter (http://www.tylerstreeter.net)
 *      http://quicktest.sourceforge.net
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: Provide a test framework for the toolkit
 *
 */


#ifndef OFTEST_H
#define OFTEST_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofconapp.h"   /* for OFCommandLine */
#include "dcmtk/ofstd/ofconsol.h"   /* for CERR */
#include "dcmtk/ofstd/oflist.h"     /* for class OFList */
#include "dcmtk/ofstd/ofstream.h"   /* for class OFOStringStream */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */
#include "dcmtk/ofstd/oftypes.h"    /* for OFBool */

// use this macro if the tests are run in a module where the logger from oflog
// and the macros from dcmdata are not accessible, i.e. ofstd and oficonv.
#ifdef OFTEST_OFSTD_ONLY

#define OFTEST_LOG_VERBOSE(msg) do { \
    if (verbose_) \
        COUT << msg << OFendl; \
} while (0)

#ifdef OFTEST_INCLUDE_IPC_OPTION
extern const char *appName;
extern void ipc_client(Uint32 port);
#endif

#else

#include "dcmtk/dcmdata/dcuid.h"    /* for dcmtk version name */
#include "dcmtk/oflog/oflog.h"

static OFLogger testLogger = OFLog::getLogger("dcmtk.test");

#define OFTEST_LOG_VERBOSE(msg) OFLOG_INFO(testLogger, msg)

#endif

/** @file oftest.h
 *  A simple framework for writing and running test cases.
 */

/** A single test case which can be run */
class OFTestTest
{
public:
    /// This is the type used for test results.
    typedef OFList<OFString> TestResult;

    /** Special flags that a test can have. The flags for a test are the result
     *  of a bitwise or of these individual flags.
     */
    enum E_Flags {
        EF_None = 0x0,
        /// Slow test which should only be run in exhaustive mode.
        EF_Slow = 0x1
    };

    /** Constructor
     *  @param testName the name of this test case
     */
    OFTestTest(const OFString& testName, int flag)
      : testName_(testName)
      , results_()
      , flags_(flag)
    {
    }

    /// Destructor
    virtual ~OFTestTest()
    {
    }

    /// @return the flags of this test case
    int flags() const { return flags_; }

    /// @return the name of this test case
    const OFString& getTestName() const { return testName_; }

    /** Execute this test case.
     *  @return Reference to list of errors.
     */
    const TestResult& runAndReturn()
    {
        results_.clear();
        run();
        return results_;
    }

    /** Execute this test case.
     *  @see #OFCHECK(condition)
     */
    virtual void run() = 0;

    /** Add a new failure to the result set.
     *  @param file filename for this failure
     *  @param line line number for this failure
     *  @param message error description.
     */
    void recordFailure(const OFString& file, unsigned long int line, const OFString& message)
    {
        OFOStringStream oss;
        oss << "FAILED test '" << testName_ << "' at " << file << ":" << line
            << ": " << message << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(oss, str)
        results_.push_back(str);
    }

    /** Add a new failure to the result set.
     *  @param file filename for this failure
     *  @param line line number for this failure
     *  @param message error description.
     *  @param errorDetails details of what went wrong
     */
    void recordFailure(const OFString& file, unsigned long int line, const OFString& message, const OFString& errorDetails)
    {
        OFOStringStream oss;
        oss << "FAILED test '" << testName_ << "' at " << file << ":" << line
            << ": " << message;
        if (!errorDetails.empty())
            oss << ", details: " << errorDetails;
        oss << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(oss, str)
            results_.push_back(str);
    }

private:
    /// The unique name of this test.
    OFString testName_;

    /// The test results, empty for success.
    TestResult results_;

    /// Flags that this test has.
    const int flags_;
};

/** The test manager singleton manages the list of available test cases
 *  and executes them.
 */
class OFTestManager
{
public:
    /// @return the only instance of the test manager
    static OFTestManager& instance()
    {
        static OFTestManager manager;
        return manager;
    }

    /// @return the currently running test case
    OFTestTest& currentTest()
    {
        if (!curTest_)
            abort();
        return *curTest_;
    }

    /** Register a test with this test manager.
     *  @param test the test to register
     */
    void addTest(OFTestTest* test)
    {
        tests_.push_back(test);
    }

    /** Run a list of test cases. The results will be printed on the console.
     *  @param tests tests to execute
     */
    int runTests(const OFList<OFTestTest*>& tests, const char *module)
    {
        unsigned int numFailed = 0;
        OFListConstIterator(OFTestTest*) it;
        OFString mod_str = module ? " for module '" + OFString(module) + "'" : "";

        OFTEST_LOG_VERBOSE("Running " << tests.size() << " tests" << mod_str << ":");

        for (it = tests.begin(); it != tests.end(); ++it)
        {
            OFTEST_LOG_VERBOSE("  Running test '" << (*it)->getTestName() << "'...");

            curTest_ = *it;
            const OFTestTest::TestResult& result = (*it)->runAndReturn();
            curTest_ = NULL;

            if (!result.empty())
            {
                numFailed++;
                OFListConstIterator(OFString) rit;
                for (rit = result.begin(); rit != result.end(); ++rit)
                {
                    // recordFailure() already formatted the message
                    CERR << *rit << OFendl;
                }
            }
        }

        COUT << "Test results" << mod_str << ": "
            << tests.size() - numFailed << " succeeded, "
            << numFailed << " failed." << OFendl;

        /* Only the lowest 8 bit of the exit code can be used! */
        if (numFailed > 254)
        {
            CERR << "WARNING: More than 254 tests failed!" << OFendl;
            return 254;
        }

        return OFstatic_cast(int, numFailed);
    }

    /** Handle the given arguments and run the requested test case. This
     *  function should be used for implementing main().
     *  @param argc number of arguments
     *  @param argv list of arguments
     *  @param module name of the module that we are testing for
     *  @return 0 in case of success, else an error value.
     */
    int run(int argc, char* argv[], const char* module)
    {
        OFList<OFTestTest*> testsToRun;
        OFBool listOnly = OFFalse;
        OFString rcsid;

#ifdef OFTEST_OFSTD_ONLY
#ifdef OFTEST_INCLUDE_IPC_OPTION
        appName = argv[0];
#endif
        // No proper rcsid because the necessary defines are in dcmdata
        if (module != NULL)
            rcsid = "$dcmtk: " + OFString(module) + " $";
#else
        rcsid = "$dcmtk: ";
        rcsid += OFSTRING_GUARD(module);
        rcsid += " v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";
#endif

        OFConsoleApplication app("tests", "Run the test suite", rcsid.c_str());
        OFCommandLine cmd;
        cmd.setParamColumn(13);

        cmd.addParam("tests-to-run", "names of tests to run (default: all)", OFCmdParam::PM_MultiOptional);

        cmd.addGroup("general options:");
          cmd.addOption("--help",       "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
          cmd.addOption("--list",       "-l", "list available tests and exit", OFCommandLine::AF_Exclusive);
          cmd.addOption("--exhaustive", "-x", "also run extensive and slow tests");
#ifdef OFTEST_OFSTD_ONLY
          cmd.addOption("--verbose",    "-v", "verbose mode, print processing details");
#ifdef OFTEST_INCLUDE_IPC_OPTION
          cmd.addOption("--ipc",           1, "run as IPC message queue client process",
                                              "[p]rocess ID: int",  OFCommandLine::AF_Internal);
#endif
#else
          OFLog::addOptions(cmd);
#endif

        /* evaluate command line */
        if (app.parseCommandLine(cmd, argc, argv))
        {
            /* check exclusive options first */
        }

#ifdef OFTEST_OFSTD_ONLY
        if (cmd.findOption("--verbose")) verbose_ = OFTrue;
#ifdef OFTEST_INCLUDE_IPC_OPTION
        if (cmd.findOption("--ipc"))
        {
            // act as the client process for the IPC test case
            OFCmdUnsignedInt ipc_port = 0;
            app.checkValue(cmd.getValueAndCheckMin(ipc_port, 65536));
            ipc_client(ipc_port);
            exit(0);
        }
#endif
#else
        /* We disable warnings and errors by default since some tests cause
         * such messages by testing corner cases. */
        OFLog::configureFromCommandLine(cmd, app, OFLogger::FATAL_LOG_LEVEL);
#endif
        if (cmd.findOption("--exhaustive")) exhaustive_ = OFTrue;
        if (cmd.findOption("--list")) listOnly = OFTrue;

        if (!buildTestsToRun(cmd, testsToRun))
            return -1;

        if (testsToRun.empty())
        {
            CERR << "No tests to run!" << OFendl;
            return 0;
        }

        if (listOnly)
        {
            OFListIterator(OFTestTest*) it;
            COUT << "There are " << testsToRun.size() << " tests";
            if (module)
                COUT << " for module '" << module << "'";
            COUT << ":" << OFendl;
            for (it = testsToRun.begin(); it != testsToRun.end(); ++it)
            {
                COUT << "  " << (*it)->getTestName() << "\n";
            }
            return 0;
        }

        return runTests(testsToRun, module);
    }

private:
    /// Private constructor, this is a singleton!
    OFTestManager()
      : tests_()
      , curTest_(NULL)
      , exhaustive_(OFFalse)
#ifdef OFTEST_OFSTD_ONLY
      , verbose_(OFFalse)
#endif
    {
    }

    /// Private undefined copy constructor
    OFTestManager(const OFTestManager& obj);

    /// Private undefined assignment operator
    OFTestManager& operator=(const OFTestManager& obj);

    /** Build a list of tests which should be executed from the command line.
     *  @param cmd command line arguments which should be parsed
     *  @param tests will be set to the list of tests to run
     *  @return OFFalse if the command line could not be handled.
     */
    OFBool buildTestsToRun(OFCommandLine& cmd, OFList<OFTestTest*>& tests) const
    {
        const int paramCount = cmd.getParamCount();
        OFString paramString;
        OFBool result = OFTrue;

        if (paramCount == 0)
        {
            // If no arguments are given, run all possible tests
            tests = tests_;
        }
        else
        {
            for (int i = 1; i <= paramCount; i++)
            {
                cmd.getParam(i, paramString);

                // Find all tests matching this argument
                OFBool found = OFFalse;
                OFListConstIterator(OFTestTest*) it;
                for (it = tests_.begin(); it != tests_.end(); ++it)
                {
                    if (testMatches(*it, paramString))
                    {
                        tests.push_back(*it);
                        found = OFTrue;
                    }
                }

                if (!found)
                {
                    CERR << "Error: No test matches '" << paramString << "'" << OFendl;
                    result = OFFalse;
                }
            }
        }

        // If we are not in exhaustive mode, remove all slow tests
        if (!exhaustive_)
        {
            OFListIterator(OFTestTest*) it = tests.begin();
            while (it != tests.end())
            {
                if ((*it)->flags() & OFTestTest::EF_Slow)
                    it = tests.erase(it);
                else
                    ++it;
            }
        }

        return result;
    }

    /** Test if the test name matches the given name. This function supports '?'
     *  and '*' for wildcards. However, '*' can only be used at the end of string.
     *  @param test test to check against
     *  @param str the string describing the tests
     *  @return OFTrue if we found a match, else OFFalse
     */
    OFBool testMatches(const OFTestTest* test, const OFString& str) const
    {
        const char* testName = test->getTestName().c_str();
        const char* string = str.c_str();

        for (; *testName != '\0' && *string != '\0'; testName++, string++)
        {
            // Does the string still match?
            if (string[0] != '?' && testName[0] != string[0])
                break;
        }

        // Is this a wildcard?
        // So far we only support '*' at the end of the string
        if (string[0] == '*' && string[1] == '\0')
            return OFTrue;

        // If both strings reached their end, we have a match
        if (testName[0] == '\0' && string[0] == '\0')
            return OFTrue;
        return OFFalse;
    }

    /// List of tests. Statically allocated, so don't have to be freed.
    OFList<OFTestTest*> tests_;

    /// Currently running test.
    OFTestTest* curTest_;

    /// Should slow tests be run, too?
    OFBool exhaustive_;

#ifdef OFTEST_OFSTD_ONLY
    /// Are we running in verbose mode? Only used if oflog is not available.
    OFBool verbose_;
#endif
};


/** Implement a main() function for running tests. The main function will return
 *  the number of failed tests or -1 if an invalid test name was given.
 *  @param module the name of the module which is under test.
 */
#define OFTEST_MAIN(module) \
int main(int argc, char* argv[]) \
{ \
    return OFTestManager::instance().run(argc, argv, module); \
}

/// Internal macro for generating a class definition, don't use yourself!
#define OFTEST_CLASS(testName) \
class OFTest ## testName : public OFTestTest \
{ \
public: \
    OFTest ## testName(); \
    void run(); \
}

/** Register a test to the test manager. Normally you should use
 *  OFTEST_REGISTER instead, but that macro doesn't work if OFTEST and
 *  OFTEST_REGISTER are called in the same source file.
 *  @param testName name of the test to register
 */
#define OFTEST_REGISTER_INT(testName) \
    OFTest ## testName OFTest ## testName ## instance

/** Register a test to the test manager.
 *  @param testName name of the test to register
 */
#define OFTEST_REGISTER(testName) \
    OFTEST_CLASS(testName); \
    OFTEST_REGISTER_INT(testName)

/** Macro to define a new test case. Internally this defines a new class
 *  inheriting from OFTest.
 *  This is equivalent to OFTEST_FLAGS(testName, EF_None).
 *  @param testName name describing the test
 *  @see OFTEST_FLAGS
 */
#define OFTEST(testName) OFTEST_FLAGS(testName, EF_None)

/** Macro to define a new test case. Internally this defines a new class
 *  inheriting from OFTest.
 *  @param flags flags that should be set for this test
 *  @param testName name describing the test
 *  @see OFTEST
 */
#define OFTEST_FLAGS(testName, flags) \
    OFTEST_CLASS(testName); \
    OFTest ## testName::OFTest ## testName() \
    : OFTestTest(#testName, flags) \
    { \
        OFTestManager::instance().addTest(this); \
    } \
    void OFTest ## testName ::run()

/** @name macros for checking conditions in tests
 *  These macros can be used for doing various checks in test cases. In case
 *  their check fails, they emit a descriptive message explaining the problem.
 */
///@{

/** Check if a condition is true. Can only be used inside OFTEST().
 *  @param condition condition to check
 */
#define OFCHECK(condition) \
    do { \
        if (!(condition)) \
            OFTestManager::instance().currentTest().recordFailure(__FILE__, __LINE__, #condition); \
    } while (0)

 /** Check if a condition is true. Can only be used inside OFTEST().
  *  @param condition condition to check
  *  @param errorDetails additional error details (string) provided by caller
  */
#define OFCHECK_MSG(condition, errorDetails) \
    do { \
        if (!(condition)) \
            OFTestManager::instance().currentTest().recordFailure(__FILE__, __LINE__, #condition, errorDetails); \
    } while (0)

/** Check if two values are equal. Can only be used inside OFTEST(). Both
 *  arguments must be compatible with OFOStringStream's operator<<.
 *  @param val1 first value to compare
 *  @param val2 second value to compare
 */
#define OFCHECK_EQUAL(val1, val2) \
    do { \
        if ((val1) != (val2)) { \
            OFOStringStream oss___; \
            oss___ << "(" << (val1) << ") should equal (" << (val2) << ")" << OFStringStream_ends; \
            OFSTRINGSTREAM_GETOFSTRING(oss___, str___) \
            OFTestManager::instance().currentTest().recordFailure(__FILE__, __LINE__, str___); \
        } \
    } while (0)

/** Unconditionally add a failure
 *  @param message string describing the failure
 */
#define OFCHECK_FAIL(message) \
    do { \
        OFOStringStream oss___; \
        oss___ << message << OFStringStream_ends; \
        OFSTRINGSTREAM_GETOFSTRING(oss___, str___) \
        OFTestManager::instance().currentTest().recordFailure(__FILE__, __LINE__, str___); \
    } while (0)

///@}

#endif

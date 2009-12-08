#ifndef LOG4CPLUS_CONFIG_HXX
#define LOG4CPLUS_CONFIG_HXX

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#if defined (_WIN32)
#  include "dcmtk/oflog/config/win32.h"
#elif (defined(__MWERKS__) && defined(__MACOS__))
#  include "dcmtk/oflog/config/macosx.h"
#else
#  include "dcmtk/oflog/config/defines.h"
#endif

#if !defined(_WIN32)
#  if !defined(LOG4CPLUS_SINGLE_THREADED)
#    define LOG4CPLUS_USE_PTHREADS
#  endif
#  if defined (INSIDE_LOG4CPLUS)
#    define LOG4CPLUS_EXPORT LOG4CPLUS_DECLSPEC_EXPORT
#  else
#    define LOG4CPLUS_EXPORT LOG4CPLUS_DECLSPEC_IMPORT
#  endif // defined (INSIDE_LOG4CPLUS)
#endif // !_WIN32

#include "dcmtk/oflog/helpers/threadcf.h"


#endif // LOG4CPLUS_CONFIG_HXX

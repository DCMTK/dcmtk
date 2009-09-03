//#include <sstream>
#include "dcmtk/oflog/helpers/syncprims.h"
#include "dcmtk/ofstd/ofconsol.h"


namespace log4cplus { namespace thread { namespace detail {


LOG4CPLUS_EXPORT
void
syncprims_throw_exception (char const * const msg, char const * const file,
    int line)
{
    //STD_NAMESPACE ostringstream oss;
    CERR << file << ":" << line << msg;
    //throw STD_NAMESPACE runtime_error (oss.str ());
    abort();
}


} } } // namespace log4cplus { namespace thread { namespace detail {

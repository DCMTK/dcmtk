//#include <sstream>
#include "dcmtk/oflog/helpers/syncprims.h"
#include "dcmtk/ofstd/ofconsol.h"


namespace log4cplus { namespace thread { namespace detail {


LOG4CPLUS_EXPORT
void
syncprims_throw_exception (char const * const msg, char const * const file,
    int line)
{
    //std::ostringstream oss;
    CERR << file << ":" << line << msg;
    //throw std::runtime_error (oss.str ());
    abort();
}


} } } // namespace log4cplus { namespace thread { namespace detail {

//  Copyright (C) 2010, Vaclav Haisman. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modifica-
//  tion, are permitted provided that the following conditions are met:
//
//  1. Redistributions of  source code must  retain the above copyright  notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
//  APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
//  DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
//  OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
//  ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
//  (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "dcmtk/oflog/helpers/strhelp.h"

#if defined (DCMTK_LOG4CPLUS_WITH_ICONV)

#ifdef DCMTK_LOG4CPLUS_HAVE_ICONV_H
#include <iconv.h>
#endif

#include "dcmtk/ofstd/ofstream.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cerrno>
#include <cstring>


// This is here because some compilers (Sun CC) think that there is a
// difference if the typedefs are not in an extern "C" block.
extern "C"
{

    //! SUSv3 iconv() type.
    typedef size_t (& iconv_func_type_1) (iconv_t cd, char * * inbuf,
    size_t * inbytesleft, char * * outbuf, size_t * outbytesleft); 


    //! GNU iconv() type.
    typedef size_t (& iconv_func_type_2) (iconv_t cd, const char * * inbuf,
    size_t * inbytesleft, char * * outbuf, size_t * outbytesleft);

} // extern "C"


namespace dcmtk
{
namespace log4cplus
{

namespace helpers
{

namespace
{


static iconv_t const iconv_error_handle = OFreinterpret_cast(iconv_t, -1);


struct iconv_handle
{
    iconv_handle (char const * to, char const * from)
        : handle (iconv_open (to, from))
    {
    }

    ~iconv_handle ()
    {
        if (handle != iconv_error_handle)
        {
            int ret = iconv_close (handle);
            if (ret == -1)
            {
                STD_NAMESPACE ostringstream oss;
                oss << "iconv_close failed: " << errno;
                STD_NAMESPACE cerr << oss.str () << OFendl;
            }
        }
    }

    size_t
    call_iconv (iconv_func_type_1 iconv_func, char * * inbuf,
        size_t * inbytesleft, char * * outbuf, size_t * outbytesleft)
    {
        return iconv_func (handle, inbuf, inbytesleft, outbuf, outbytesleft);
    }

    size_t
    call_iconv (iconv_func_type_2 iconv_func, char * * inbuf,
        size_t * inbytesleft, char * * outbuf, size_t * outbytesleft)
    {
        return iconv_func (handle, OFconst_cast(const char * *, inbuf),
            inbytesleft, outbuf, outbytesleft);
    }

    size_t
    do_iconv (char * * inbuf, size_t * inbytesleft, char * * outbuf,
        size_t * outbytesleft)
    {
        return call_iconv (iconv, inbuf, inbytesleft, outbuf, outbytesleft);
    }

    iconv_t handle;
    
// to silence warnings
private:
    iconv_handle(const iconv_handle&);
    iconv_handle& operator=(const iconv_handle&);
};


template <typename T>
struct question_mark;


template <>
struct question_mark<char>
{
    static char const value = '?';
};


template <>
struct question_mark<wchar_t>
{
    static wchar_t const value = L'?';
};


char const question_mark<char>::value;


wchar_t const question_mark<wchar_t>::value;


template <typename DestType, typename SrcType>
static
void
iconv_conv (STD_NAMESPACE basic_string<DestType> & result, char const * destenc,
    SrcType const * src, size_t size, char const * srcenc)
{
    iconv_handle cvt (destenc, srcenc);
    if (cvt.handle == iconv_error_handle)
    {
        // TODO: Better error handling.
        result.resize (0);
        return;
    }

    typedef DestType outbuf_type;
    typedef SrcType inbuf_type;

    size_t result_size = size + size / 3 + 1;
    result.resize (result_size);

    char * inbuf = OFconst_cast(char *, OFreinterpret_cast(char const *, src));
    size_t inbytesleft = size * sizeof (inbuf_type);

    char * outbuf = OFreinterpret_cast(char *, &result[0]);
    size_t outbytesleft = result_size * sizeof (outbuf_type);

    size_t res;
    size_t const error_retval = OFstatic_cast(size_t, -1);

    while (inbytesleft != 0)
    {
        res = cvt.do_iconv (&inbuf, &inbytesleft, &outbuf, &outbytesleft);
        if (res == error_retval)
        {
            switch (errno)
            {
            case EILSEQ:
            case EINVAL:
                if (outbytesleft >= sizeof (outbuf_type))
                {
                    if (inbytesleft > 0)
                    {
                        ++inbuf;
                        inbytesleft -= sizeof (inbuf_type);
                    }

                    *outbuf = question_mark<outbuf_type>::value;
                    ++outbuf;
                    outbytesleft -= sizeof (outbuf_type);
                    
                    continue;
                }

                // Fall through.
                
            case E2BIG:;
                // Fall through.
            }

            size_t const outbuf_index = result_size;
            result_size *= 2;
            result.resize (result_size);
            outbuf = OFreinterpret_cast(char *, &result[0] + outbuf_index);
            outbytesleft = (result_size - outbuf_index) * sizeof (outbuf_type);
        }
        else
            result.resize (result_size - outbytesleft / sizeof (outbuf_type));
    }
}


} // namespace


OFString
tostring (const STD_NAMESPACE wstring & src)
{
    STD_NAMESPACE string ret;
    iconv_conv (ret, "UTF-8", src.c_str (), src.size (), "WCHAR_T");
    return OFString(ret.c_str(), ret.size());
}


OFString
tostring (wchar_t const * src)
{
    assert (src);
    STD_NAMESPACE string ret;
    iconv_conv (ret, "UTF-8", src, STD_NAMESPACE wcslen (src), "WCHAR_T");
    return OFString(ret.c_str(), ret.size());
}


STD_NAMESPACE wstring
towstring (const OFString& osrc)
{
    STD_NAMESPACE wstring ret;
    STD_NAMESPACE string src(osrc.c_str(), osrc.size());
    iconv_conv (ret, "WCHAR_T", src.c_str (), src.size (), "UTF-8");
    return ret;
}


STD_NAMESPACE wstring
towstring (char const * src)
{
    assert (src);
    STD_NAMESPACE wstring ret;
    iconv_conv (ret, "WCHAR_T", src, STD_NAMESPACE strlen (src), "UTF-8");
    return ret;
}


} // namespace helpers

} // namespace log4cplus


} // end namespace dcmtk

#else

int striconv_cc_dummy_to_keep_linker_from_moaning = 0;

#endif // DCMTK_LOG4CPLUS_WITH_ICONV

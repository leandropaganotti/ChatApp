#include "utils.h"

namespace utils {

std::string &trim(std::string &s)
{

    const std::string delimeters = " \f\n\r\t\v";

    size_t p = s.find_first_not_of(delimeters);
    s.erase(0, p);

    p = s.find_last_not_of(delimeters);
    if (std::string::npos != p)
        s.erase(p+1);

    return s;
}

std::string timestamp2string(std::time_t timestamp)
{
    struct tm t;
    if ( gmtime_r( &timestamp, &t ) == nullptr )
    {
        timestamp = 0;
        gmtime_r( &timestamp, &t );
    }
    char buffer[64] = { };
    strftime( buffer, 64, "%FT%T", &t );
    return buffer;
}


}

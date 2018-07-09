#ifndef LOGGER_H
#define LOGGER_H

#include <sstream>
#include <iostream>
#include "utils.h"

class Logger
{
public:
    std::ostringstream os;
    ~Logger()
    {
        std::cout << utils::timestamp2string( time(nullptr) ) << " " << os.str() << std::flush;
    }

};

#define logger Logger().os

#endif // LOGGER_H


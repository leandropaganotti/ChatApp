#ifndef UTILS
#define UTILS

#include <string>
#include <time.h>
#include <ctime>

namespace utils {

std::string& trim(std::string& s);
std::string timestamp2string( std::time_t timestamp );

}
#endif // UTILS


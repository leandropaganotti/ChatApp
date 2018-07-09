#ifndef EVENTDATA
#define EVENTDATA
#include <netinet/in.h>

struct EventData
{
    const char *message;
    size_t clientid;
};

#endif // EVENTDATA


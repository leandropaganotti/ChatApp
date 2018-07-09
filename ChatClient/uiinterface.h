#ifndef UIINTERFACE
#define UIINTERFACE

namespace Interface {
class UIInterface
{
public:
    virtual ~UIInterface(){}

    virtual void populateUsers(const void *) = 0;
};
}


#endif // UIINTERFACE


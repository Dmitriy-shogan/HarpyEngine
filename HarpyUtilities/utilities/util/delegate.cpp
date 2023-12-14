#include "delegate.h"

bool harpy::utilities::delegate::invoke()
{
    for(auto& i : functions)
    {
        i.operator()();
    }
    functions.clear();
    return true;
}

void harpy::utilities::delegate::operator()()
{
    invoke();
}

void harpy::utilities::delegate::clear()
{
    functions.clear();
}

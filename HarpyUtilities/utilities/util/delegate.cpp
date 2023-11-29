#include "delegate.h"

void harpy::utilities::delegate::invoke()
{
    for(auto& i : functions)
    {
        i.operator()();
    }
    functions.clear();
}

void harpy::utilities::delegate::operator()()
{
    invoke();
}

void harpy::utilities::delegate::clear()
{
    functions.clear();
}

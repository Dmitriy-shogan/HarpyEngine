#include <util/delegate.h>

bool harpy::utilities::delegate::invoke()
{
    for(auto& i : functions)
    {
        std::invoke(i);
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

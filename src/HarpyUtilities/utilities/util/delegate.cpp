#include <util/delegate.h>

#include <logger/harpy_little_error.h>

 bool harpy::utilities::delegate::invoke(bool do_clean)
{
    for(auto& i : functions)
    {
        std::invoke(i);
    }
    if (do_clean) functions.clear();
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

void harpy::utilities::delegate::invoke_one(size_t index) {
    if(index >= functions.size())
        throw harpy_little_error("Out of bounds exception while invoking one function in delegate");
    functions[index].operator()();
}

void harpy::utilities::delegate::pop_back() {
    functions.pop_back();

}

size_t harpy::utilities::delegate::size() {
    return functions.size();
}


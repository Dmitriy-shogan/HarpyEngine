#pragma once
#ifndef HARPY_UTILITIES_DELEGATE
#define HARPY_UTILITIES_DELEGATE
#include <concepts>
#include <functional>
#include <future>


namespace harpy::utilities
{
    //TODO: better delegate
    class delegate
    {
        std::vector<std::function<void()>> functions{};
    public:

        template <typename function, typename... args>
            requires std::invocable<function, args...>
        void push_back(function f, args... argues);

        void invoke();
        void operator()();

        void clear();
    };

    template <typename function, typename ... args> requires std::invocable<function, args...>
    void delegate::push_back(function f, args... argues)
    {
        auto task = [func = std::move(f), ... largs = std::move(argues)]() {
            func(largs...);
        };
        functions.push_back(task);
    }
}


#endif //HARPY_UTILITIES_DELEGATE
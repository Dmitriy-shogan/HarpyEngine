#pragma once
#ifndef HARPY_UTILITIES_DELEGATE
#define HARPY_UTILITIES_DELEGATE
#include <concepts>
#include <functional>
#include <future>
#include <../dll_macro.h>


namespace harpy::utilities
{
    //TODO: better delegate
    class HARPY_UTILITIES_API delegate
    {
        std::vector<std::function<void()>> functions{};
    public:

        template <typename function, typename... args>
        void push_back(function f, args... argues);

        //Два главных метода для вызова, исполняют одно и то же
        //Пока что кроме добавления (выше), вызова и очистки ничё ненадо, что ясно
        bool invoke();
        void operator()();
        void clear();
    };

    template <typename function, typename ... args>
    void delegate::push_back(function f, args... argues)
    {
        auto task = [func = std::move(f), ... largs = std::move(argues)]() {
            func(largs...);
        };
        functions.push_back(task);
    }
}


#endif //HARPY_UTILITIES_DELEGATE
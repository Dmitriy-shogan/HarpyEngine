#pragma once
#ifndef HARPY_UTILITIES_DELEGATE
#define HARPY_UTILITIES_DELEGATE
#include <concepts>
#include <functional>
#include <future>
#include <../dll_macro.h>


namespace harpy::utilities
{
    class HARPY_UTILITIES_API delegate
    {
        std::vector<std::function<void()>> functions{};
    public:

        template <typename function, typename... args>
        void push_back(function f, args... argues);

        //Два главных метода для вызова, исполняют одно и то же
        //Пока что кроме добавления (выше), вызова и очистки ничё ненадо, что ясно
        bool invoke(bool do_clean = false);
        void operator()();
        void invoke_one(size_t index);

        void clear();
        void pop_back();
        size_t size();
    };

    template <typename function, typename ... args>
    void delegate::push_back(function f, args... argues)
    {
        auto task = [func = std::move(f), ... largs = std::move(argues)]() mutable {
            func(largs...);
        };
        functions.push_back(task);
    }

    template<typename... types>
    class HARPY_UTILITIES_API parameters_delegate{
        std::vector<std::function<void(types...)>> functions{};
    public:

        void push_back(std::function<void(types...)> function);

        bool invoke(types&&... args, bool do_clean = false);
        void operator()(types&&... args);
        void invoke_one(size_t index, types&&... args);

        void clear();
        void pop_back();
        size_t size();
        };

    template<typename... types>
    size_t parameters_delegate<types...>::size() {
        return functions.size();
    }

    template<typename... types>
    void parameters_delegate<types...>::pop_back() {
        functions.pop_back();
    }

    template<typename... types>
    void parameters_delegate<types...>::clear() {
        functions.clear();
    }

    template<typename... types>
    void parameters_delegate<types...>::invoke_one(size_t index, types &&... args) {
        functions[index](std::forward<types>(args)...);
    }

    template<typename... types>
    void parameters_delegate<types...>::operator()(types&&... args) {
        invoke(std::forward<types>(args)...);
    }

    template<typename... types>
    bool parameters_delegate<types...>::invoke(types&&... args, bool do_clean) {
        for(auto& i : functions)
            i(std::forward<types>(args)...);

        if(do_clean)
            functions.clear();
        return true;
    }

    template<typename... types>
    void parameters_delegate<types...>::push_back(std::function<void(types...)> function) {
        auto task = [func = std::move(function)](types&&... args){
            func(std::forward<types>(args)...);
        };
        functions.push_back(task);
    }

}


#endif //HARPY_UTILITIES_DELEGATE
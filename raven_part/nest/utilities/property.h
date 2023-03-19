#ifndef HARPY_PROPERTY
#define HARPY_PROPERTY

#include <utilities/harpy_little_error.h>

#define st ::std::

namespace harpy_nest
{
    template <typename return_type, class T, typename ... Args>
    using method = st function<return_type(T&, Args ...)>;

    template <typename return_type, typename ... Args>
    using func = st function<return_type(Args ...)>;


    enum class access
    {
        read = 1,
        write,
        read_write
    };

    template <class container, typename arg_type, access access_prop = access::read_write>
    class property
    {
        container* container_ptr{nullptr};
        method<arg_type, container> get_func{nullptr};
        method<void, container, arg_type> set_func{nullptr};

    public:
        property() = default;

        void apply_object(container* object);
        void apply_setter_property(method<void, container, arg_type> setter);
        void apply_getter_property(method<arg_type, container> getter);

        operator arg_type()
        {
            if (access_prop == access::write) throw harpy_little_error("Can't get with write only property");
            if (!get_func) throw harpy_little_error("Can't return argument because there's no 'get function'");

            return get_func(*container_ptr);
        }

        arg_type& operator =(const arg_type arg)
        {
            if (access_prop == access::read) throw st exception("Can't set argument with read-only properties");
            if (!set_func) throw st exception("Can't set argument without 'set function'");
            set_func(*container_ptr, arg);
            return arg;
        }
    };

    template <class container, typename arg_type, access access_prop>
    void property<container, arg_type, access_prop>::apply_object(container* object)
    {
        container_ptr = object;
    }

    template <class container, typename arg_type, access access_prop>
    void property<container, arg_type, access_prop>::apply_setter_property(method<void, container, arg_type> setter)
    {
        set_func = setter;
    }

    template <class container, typename arg_type, access access_prop>
    void property<container, arg_type, access_prop>::apply_getter_property(method<arg_type, container> getter)
    {
        get_func = getter;
    }
}


#undef st

#endif

#include <chrono>

/* Only needed for the sake of this example. */
#include <iostream>
#include <thread>
    
void long_operation()
{
    /* Simulating a long, heavy operation. */

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(150ms);
}

template <typename T>
class cool_class
{
    T something;
public:
    T getter(){return something;}
    void setter(T i){something = i;}

    template <typename E>
    cool_class<T> operator=(cool_class<E> obj)
    {
        this->something = obj.getter();
        return *this;
    }    
};

int main()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    cool_class<int> integer;
    integer.setter(5);

    cool_class<char> character;
    character.setter('f');

    integer = character;
    std::cout<<integer.getter() <<std::endl;

    void* data = &integer;

    auto t1 = high_resolution_clock::now();
    long_operation();
    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms\n";
    return 0;
}
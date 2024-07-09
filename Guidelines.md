# FINALLY, GUIDELINES
## Variables    
Any variable must starts with not capital letter and be using snake_case. Also you SHOULD use initialization lists instead of *operator=*; It's same, but we can make initialization similar for variable of any kind.   
Names for them must show their respective use. If you use abbreviation for name, you SHOULD use comment to explain name. In some cases, if abbreviation is used really common, you can skip commenting
```c++
//Examples
int i {0};
std::vector<custom_class> cool_classes{};

//ci = create info
VkShit shit_ci{};

//No need for commenting on this
float* precision_ptr;
```

## Functions
Functions just like variables must be written in snake_case. Function name MUST represent what it does and mustn't contain anything that is not represented by her. Size of functions shouldn't be big, but it is not strict. 

```c++
//Examples
float sqrt(float x); //Get's back square root of num
//Getter
VkShit& get_vk_shit(); 
```

If we work with Vulkan and creating some wrappers in the nest, we *should* skip vk part
```c++
VkShit& nest::wrappers::shit& get_smth();
```

## Structs
Structs must be POD (https://stackoverflow.com/questions/146452/what-are-pod-types-in-c), names in snake_case. Variables in here should be initialized with something standard if this is custom type and MUST be initialized if it is basic type like int
```c++
//Examples
struct vk_helper{
    int_ptr ptr_to_instance{};
    bool not_so_true{true};
}
//WRONG struct
struct class_lover{
    class_lover(){rng(random_num);}
    int random_num;
}
```
POD structure provide us with ability to move in directly into gpu without wrappers or arrays. Structs must be used where we just want to combine some variables and we don't need ant complications.

## Enums
Here goes something interesting. We divide enums into two main categories: bitmasks or not. If we will create bitmasks out of enums (Vulkan flags for example), we must use just *enum*. In other situations *enum class* must be used.   
Also we must use three more variables in every enum: h(arpy)e(num)_first -- to show the value of FIRST actual element, while in bitmask enum we will just go with zero. he_is_bm shows is enum is bitmask (just for some checks not on compile time and for later scripting). he_count says how many positions are actually used, so we will count only first, second and third.
```c++
enum not_bitmask{
he_first = first,
first,
second,
third,

he_is_bm = 1,
he_count = 3
}

enum class bitmask{
he_first = 0,
first = 1 << 0,
second = 1 << 1, 
third = 1 << 2,

he_is_bm = 1,
he_count = 3
```
Simple tip: use if constexpr for checking   

## Macros
Every macros, that will be not only once or in one file MUST be initialized in nest/initializations.h. At the start of each file must be pragma once AND macro for this file. Naming must be in SNAKE_CAPITAL_CASE and should starts with HARPY_. Other naming parts of macros should contain namespace of the file and class name.
```c++
//Examples
//Good in nest::wrappers header with int_wrapper class
#define HARPY_NEST_WRAPPERS_INT_WRAPPER
//Bad in nest::wrappers header with int_wrapper class
#define WRAPPERS_INT_WRAPPER
#define HARPY_NEST_INT_WRAPPER
//Worst in nest::wrappers header with int_wrapper class
#define WRAPPERS_INT
```

## Namespaces
Due to using at least C++17, we use nested namespaces like this:
```c++
namespace harpy::nest::resources{};
```
Naming must show the kind of objects that will be included in namespace. For example nest::wrappers must contain wrappers for smth (For Vulkan in that case)   
If in cpp file we use nested namespace for getting object at the start of file we can use "using" for size reduction
```c++
//Example
// We have nest::resources::vulkan_common_resource.
using resource = nest::resources::vulkan_common_resource;
```
You mustn't use "using" in headers except you want to name a specification of template.

## Classes

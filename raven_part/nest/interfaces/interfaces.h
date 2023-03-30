#pragma once
#ifndef HARPY_INTERFACES
#define HARPY_INTERFACES

namespace harpy_interfaces{

    //I just really love how the second letter looks big with I before it

    class IDrawable
    {
    public:
        virtual void draw() = delete;
        virtual ~IDrawable();
    };

    class ITransformable
    {
    public:
        virtual ~ITransformable();
    };
    

}


#endif
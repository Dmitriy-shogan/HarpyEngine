#pragma once
#ifndef HARPY_INTERFACES
#define HARPY_INTERFACES

namespace harpy::interfaces{

    //I just really love how the second letter looks big with I before it
    //TODO: Currentlu IDrawable just exists, needed to be discarded or changed
    class IDrawable
    {
    public:
        virtual void draw() = 0;
        virtual ~IDrawable() = default;
    };

    class IRotatable
    {
    public:
        void virtual rotate(float angle, int x, int y, int z) = 0;
        void virtual rotate(float angle, glm::vec3 vec) = 0;

        virtual ~IRotatable() = default;
    };

    class IScalable
    {
    public:
        void virtual scale(int x, int y, int z) = 0;
        void virtual scale(glm::vec3 vec) = 0;

        virtual ~IScalable() = default;
    };

    class IMoveable
    {
    public:
        void virtual move(int x, int y, int z) = 0;
        void virtual move(glm::vec3 vec) = 0;

        virtual ~IMoveable() = default;
    };

    
    /**
     * \brief Interface for objects, that can be transformable;
     */
    class ITransformable : public IMoveable, public IScalable, public IRotatable
    {
    public:
        ~ITransformable() override = default;
    };

    //Just as a joke, don't use it
    class ICamerable : public IMoveable, public IRotatable
    {
    public:
        ~ICamerable() override = default;
    };


    /*
     * \brief Interface for saying, that this component is strong and can have getters for vk parts 
     */
    class IStrong_component
    {
    public:
        virtual VkDevice& get_vk_device() = 0;
        virtual ~IStrong_component() = default;
    };
    

}


#endif
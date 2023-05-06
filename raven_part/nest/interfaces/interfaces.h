#pragma once
#ifndef HARPY_INTERFACES
#define HARPY_INTERFACES

namespace harpy::interfaces{

    //I just really love how the second letter looks big with I before it

    class IDrawable
    {
    public:
        virtual void draw() = 0;
        virtual ~IDrawable() = default;
    };


    
    /**
     * \brief Interface for objects, that will be transformable;
     */
    class ITransformable
    {
    public:

        void virtual rotate(float angle, int x, int y, int z) = 0;
        void virtual rotate(float angle, glm::vec3 vec) = 0;

        void virtual move(float angle, int x, int y, int z) = 0;
        void virtual move(float angle, glm::vec3 vec) = 0;

        void virtual scale(float angle, int x, int y, int z) = 0;
        void virtual scale(float angle, glm::vec3 vec) = 0;
        
        virtual ~ITransformable() = default;
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
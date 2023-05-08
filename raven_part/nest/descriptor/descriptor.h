#pragma once
#ifndef HARPY_DESCRIPTOR
#define HARPY_DESCRIPTOR
#include <pools/descriptor_pool.h>

namespace harpy::nest
{
    class descriptor
    {
        VkDescriptorSet set{nullptr};
        VkDescriptorSetLayout layout{nullptr};
        pools::descriptor_pool& pool;

        VkDevice& device;
        
        
        
    public:
        descriptor(VkDevice& device, pools::descriptor_pool& pool) : pool(pool), device(device){}

        void init()
        {
            //Initialising layout 
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = 1;
            layoutInfo.pBindings = &uboLayoutBinding;

            if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }

            
        }

        VkDescriptorSet& get_vk_descriptor_set(){return set;}
        VkDescriptorSetLayout& get_vk_descriptor_set_layout(){return layout;}

        operator VkDescriptorSet&(){return set;}

        ~descriptor()
        {
            vkDestroyDescriptorSetLayout(device, layout, nullptr);
        }
    
    };
}

#endif //HARPY_DESCRIPTOR
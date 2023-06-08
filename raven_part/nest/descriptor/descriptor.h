#pragma once
#ifndef HARPY_DESCRIPTOR
#define HARPY_DESCRIPTOR
#include <pools/descriptor_pool.h>
#include <buffers/uniform_buffer.h>

namespace harpy::nest
{
    class descriptor
    {
        std::vector<VkDescriptorSet> sets;
        VkDescriptorSetLayout layout{nullptr};
        pools::descriptor_pool& pool;

        VkDevice& device;
        
        
        
    public:
        descriptor(VkDevice& device, pools::descriptor_pool& pool) : pool(pool), device(device){}

        void init_layout()
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
                throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
                    "failed to create descriptor set layout!");
            }
        }
        void init()
        {
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = pool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            sets.resize(MAX_FRAMES_IN_FLIGHT);
            if (vkAllocateDescriptorSets(device, &allocInfo, sets.data()) != VK_SUCCESS) {
                throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
                    "failed to allocate descriptor sets!");
            }
        }

        void populate(std::vector<buffers::uniform_buffer>& buffers)
        {
            for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(ubo);
                bufferInfo.buffer = buffers[i].get_vk_buffer();
            
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = sets[i];
                descriptorWrite.dstBinding = 0;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfo;

                vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
            }
        }
        

        std::vector<VkDescriptorSet>& get_vk_descriptor_set(){return sets;}
        VkDescriptorSetLayout& get_vk_descriptor_set_layout(){return layout;}

        operator std::vector<VkDescriptorSet>&(){return sets;}

        ~descriptor()
        {
            vkDestroyDescriptorSetLayout(device, layout, nullptr);
        }
    
    };
}

#endif //HARPY_DESCRIPTOR
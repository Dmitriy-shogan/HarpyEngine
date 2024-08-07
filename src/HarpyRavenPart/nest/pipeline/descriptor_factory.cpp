//
// Created by Xyeveloper on 14.07.2024.
//

#include <nest/pipeline/descriptor_factory.h>
#include <3D/uniform_buffer_objects.h>

harpy::nest::pipeline::descriptor_factory::descriptor_factory(uint32_t descriptor_amount,
                                                              harpy::nest::pools::descriptor_types type,
                                                              VkDevice *device) : pool(descriptor_amount, type, device), device(device) {

}

harpy::nest::pipeline::descriptor_factory::descriptor_factory(std::initializer_list<pools::pool_size_desc> descriptions,
                                                              uint32_t max_descriptors_amount, VkDevice *device) : pool(descriptions, max_descriptors_amount, device), device(device) {

}

VkDescriptorSetLayoutBinding harpy::nest::pipeline::descriptor_factory::get_sight_ub_binding(uint32_t descriptors_amount) {
    return VkDescriptorSetLayoutBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = descriptors_amount,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT};
}

harpy::nest::pipeline::descriptor_factory::~descriptor_factory() {

    for(auto& i : layouts)
        if(i)
            vkDestroyDescriptorSetLayout(*device, i, nullptr);
}

VkDescriptorSetLayout
harpy::nest::pipeline::descriptor_factory::allocate_descriptor_set_layout(VkDescriptorSetLayoutBinding binding) {
    //Just for the thread safety, we don't work with just layouts vector
    VkDescriptorSetLayout layout{};
    VkDescriptorSetLayoutCreateInfo layout_ci{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                              .bindingCount = 1,
                                              .pBindings = &binding,};

    HARPY_VK_CHECK(vkCreateDescriptorSetLayout(*device, &layout_ci, nullptr, &layout));
    layouts.push_back(layout);

    return layout;
}

VkDescriptorSetLayout harpy::nest::pipeline::descriptor_factory::allocate_descriptor_set_layout(
        std::vector<VkDescriptorSetLayoutBinding> bindings) {

    VkDescriptorSetLayout layout{};

    VkDescriptorSetLayoutCreateInfo layout_ci{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data()};

    HARPY_VK_CHECK(vkCreateDescriptorSetLayout(*device, &layout_ci, nullptr, &layout));

    this->layouts.push_back(layout);

    return layout;
}

std::vector<VkDescriptorSet>
harpy::nest::pipeline::descriptor_factory::allocate_descriptor_sets(std::vector<VkDescriptorSetLayout> layouts) {

    std::vector<VkDescriptorSet> sets{layouts.size()};
    VkDescriptorSetAllocateInfo alloc_ci{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                         .descriptorPool = pool,
                                         .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
                                         .pSetLayouts = layouts.data()};

    HARPY_VK_CHECK(vkAllocateDescriptorSets(*device, &alloc_ci, sets.data()));
    return sets;
}

VkDescriptorSet harpy::nest::pipeline::descriptor_factory::allocate_descriptor_sets(VkDescriptorSetLayout layout) {
    VkDescriptorSet set{};
    VkDescriptorSetAllocateInfo alloc_ci{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &layout};
    HARPY_VK_CHECK(vkAllocateDescriptorSets(*device, &alloc_ci, &set));
    return set;
}

VkDescriptorSetLayoutBinding harpy::nest::pipeline::descriptor_factory::get_image_sampler_binding(uint32_t descriptors_amount) {
    return VkDescriptorSetLayoutBinding{.binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = descriptors_amount,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
    };
}

harpy::nest::pipeline::descriptor_factory *harpy::nest::pipeline::descriptor_factory::start_updating_sets() {
    for(auto& i : writes){
            delete i.pBufferInfo;
            delete i.pImageInfo;
            delete i.pTexelBufferView;
    }
    writes.clear();
    return this;
}

harpy::nest::pipeline::descriptor_factory *
harpy::nest::pipeline::descriptor_factory::update_sight_ub(VkDescriptorSet &set,
                                                           harpy::nest::wrappers::data_buffer &uniform_buffer) {
    if (uniform_buffer.get_type() != wrappers::buffer_type::uniform)
        throw harpy::utilities::harpy_little_error(
                "Can't update sight ub descriptor set with non uniform buffer");


    writes.push_back({
                             .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                             .dstSet = set,
                             .dstBinding = 0,
                             .dstArrayElement = 0, /*For now*/
                             .descriptorCount = 1,
                             .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                             .pBufferInfo = new VkDescriptorBufferInfo{.buffer = uniform_buffer,
                                     .offset = 0,
                                     .range = sizeof(D3::uniform_buffers::sight_ub)}
                     });
    return this;
}

harpy::nest::pipeline::descriptor_factory *
harpy::nest::pipeline::descriptor_factory::update_sampler(VkDescriptorSet &set,
                                                          texturing::texture& texture,
                                                          harpy::nest::texturing::texture_sampler &sampler) {

    writes.push_back({
                             .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                             .dstSet = set,
                             .dstBinding = 1,
                             .dstArrayElement = 0, /*For now*/
                             .descriptorCount = 1,
                             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                             .pImageInfo = new VkDescriptorImageInfo{
                                     .sampler = sampler,
                                     .imageView = texture,
                                     .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                             }
                     });
    return this;
}

void harpy::nest::pipeline::descriptor_factory::end_updating_descriptor_sets() {

    vkUpdateDescriptorSets(*device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

#include <nest/pipeline/descriptors/descriptor_factory.h>

harpy::nest::pipeline::descriptors::descriptor_factory::descriptor_factory(descriptor_allocator&& allocator,
    VkDevice* device) : device(device), allocator(std::move(allocator))
{
}

void harpy::nest::pipeline::descriptors::descriptor_factory::bind_resourсe(resources::descriptor_resource&& res)
{
    local_res = std::move(res);
}

harpy::nest::resources::descriptor_resource harpy::nest::pipeline::descriptors::descriptor_factory::unbind_resourсe()
{
    auto temp = std::move(local_res);
    local_res = {};
    return temp;
}

harpy::nest::resources::descriptor_resource harpy::nest::pipeline::descriptors::descriptor_factory::get_resource()
{
    return local_res;
}

harpy::nest::pipeline::descriptors::descriptor_factory* harpy::nest::pipeline::descriptors::descriptor_factory::create_descriptor_layout(
    std::vector<VkDescriptorSetLayoutBinding>&& ci, VkDescriptorSetLayoutCreateFlags flags)
{
    delegate.push_back([this, flags, ci]()
    {
        VkDescriptorSetLayout layout{};
        layout_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_ci.bindingCount = ci.size();
        layout_ci.pBindings = ci.data();
        layout_ci.flags = flags;
    
        HARPY_VK_CHECK(vkCreateDescriptorSetLayout(*device, &layout_ci, nullptr, &layout));
        layouts.push_back(layout);
        local_res.bindings = ci;
        local_res.layouts.push_back(layout);
    });
    return this;
}

harpy::nest::pipeline::descriptors::descriptor_factory* harpy::nest::pipeline::descriptors::descriptor_factory::create_descriptor_layout(
    VkDescriptorSetLayoutBinding ci, VkDescriptorSetLayoutCreateFlags flags)
{
    delegate.push_back([this, flags, ci](){
        VkDescriptorSetLayout layout{};
    layout_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_ci.bindingCount = 1;
    layout_ci.pBindings = &ci;
    layout_ci.flags = flags;

    HARPY_VK_CHECK(vkCreateDescriptorSetLayout(*device, &layout_ci, nullptr, &layout));
    layouts.push_back(layout);
    local_res.bindings.push_back(ci);
    local_res.layouts.push_back(layout);}
    );
    
    
    return this;
}

VkDescriptorSetLayoutBinding harpy::nest::pipeline::descriptors::descriptor_factory::sight_ub_binding(descriptor_shader_stage when_changes)
{
    static VkDescriptorSetLayoutBinding binding{
    .binding = 0,
    .descriptorType = static_cast<VkDescriptorType>(pools::descriptor_types::uniform_buffer),
        //Just for now
    .descriptorCount = MAX_FRAMES_IN_FLIGHT,
    };
    binding.stageFlags = static_cast<VkPipelineStageFlags>(when_changes);
    
    return binding;
}

harpy::nest::pipeline::descriptors::descriptor_factory::~descriptor_factory()
{
    for(auto& i : layouts)
    {
        if(i)
            vkDestroyDescriptorSetLayout(*device, i, nullptr);
    }
}

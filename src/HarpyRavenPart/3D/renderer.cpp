#include <3D/renderer.h>
#include <threading/thread_pool.h>
#include <nest/shader_works/glsl_shader_factory.h>
#include <3D/model_loader.h>


using resource = harpy::nest::resources::common_vulkan_resource;
void harpy::D3::renderer::init_queue_managers() {
    for(auto& i : resource::get_resource().get_all_family_queues()) {
        queue_managers.emplace_back(i);
    }
}

void harpy::D3::renderer::init_thread_resources() {
    uint32_t sum{0};
    for(uint32_t f = 0; auto& i : queue_managers) {
        sum += i.get_all_available_queues().size();
        while(f++ < sum) {
            //TODO: unhardcode, this is just bullshit
            thread_resources.emplace_back(std::make_unique<nest::resources::command_thread_resource>(i.get_queue(), std::make_unique<nest::pools::command_pool>(i.get_family_index())));
        }
    }

    for(auto& i : thread_resources)
        if(i->queue.get_type() == nest::wrappers::queue_type::transfer || i->queue.get_type() == nest::wrappers::queue_type::universal) {
            transfer_thread_resource = &i;
            break;
        }
}

void harpy::D3::renderer::init_default_pipeline() {
    sz::string pipeline_id = "standard";

    auto& factory = nest::shaders::glsl_shader_factory::get_singleton();

    nest::shaders::shader_set set{};
    set.vertex = std::make_unique<nest::shaders::shader_module>(factory.create_shader_module("../external_resources/shaders/base/vertex/base.vert"));
    set.fragment = std::make_unique<nest::shaders::shader_module>(factory.create_shader_module("../external_resources/shaders/base/fragment/base.frag"));

    auto ci = std::make_unique<nest::pipeline::graphics_pipeline_ci>();
    ci->swapchain = &swapchain;
    ci->shaders = std::move(set);
    ci->descriptor_layouts = {global_layout};

    auto pipeline = nest::pipeline::graphics_pipeline{std::move(ci)};
    pipelines.emplace(pipeline_id, std::move(pipeline));
}

void harpy::D3::renderer::init_default_texture() {
    utilities::image image{"../external_resources/images/default/beware.png"};
    textures["default"] = image;
    commander.fast_load_texture(textures["default"], image);
}

void harpy::D3::renderer::init_descriptors() {
    descriptor_factory.allocate_descriptors(2000, nest::pools::descriptor_types::uniform_buffer);
    global_layout = descriptor_factory.get_layout(
        {
            descriptor_factory.get_standard_sight_binding(),
        descriptor_factory.get_standard_texture_binding()
        });

    VkDescriptorSetAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_factory;
    //Just for now
    allocInfo.descriptorSetCount = 2;
    allocInfo.pSetLayouts = &global_layout;

    VkDescriptorBufferInfo binfo{};
    binfo.offset = 0;
    binfo.range = sizeof(uniform_buffers::sight_ub);

    VkWriteDescriptorSet setWrite = {};
    setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrite.pNext = nullptr;
    setWrite.dstBinding = 0;
    setWrite.descriptorCount = 1;
    setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setWrite.pBufferInfo = &binfo;

    VkDescriptorImageInfo image_buffer_info{};
    image_buffer_info.sampler = texture_sampler;
    image_buffer_info.imageView = textures["default"];
    image_buffer_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet sampler_write{};
    sampler_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    sampler_write.pNext = nullptr;

    sampler_write.dstBinding = 1;
    sampler_write.descriptorCount = 1;
    sampler_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    sampler_write.pImageInfo = &image_buffer_info;

    VkWriteDescriptorSet sets[] = {setWrite, sampler_write};

    for(auto& i : frames_data) {
        vkAllocateDescriptorSets(nest::resources::common_vulkan_resource::get_resource(), &allocInfo, &i.global_descriptor_set);
        //information about the buffer we want to point at in the descriptor
        binfo.buffer = i.sight_uniform_buffer;
        setWrite.dstSet = i.global_descriptor_set;

        vkUpdateDescriptorSets(nest::resources::common_vulkan_resource::get_resource(), 2, sets, 0, nullptr);
    }
}

void harpy::D3::renderer::update_uniform_buffers() {
    for(auto& i : frames_data) {
        if(i.sight_uniform.proj == glm::mat4{1.0f}) {
            i.sight_uniform.proj = cam.construct_perspective_matrix(16.0f / 9.0f);
        }
        i.sight_uniform.view = cam.construct_view_matrix();
        i.sight_uniform.view_proj =  i.sight_uniform.proj * i.sight_uniform.view;
        memcpy(i.sight_uniform_buffer.get_mapped_ptr(), &i.sight_uniform, sizeof(uniform_buffers::sight_ub));
        i.sight_uniform_buffer.unmap_ptr();
    }

}

harpy::D3::renderer::render_target & harpy::D3::renderer::search_render_target(sz::string_view id) {
    for(auto& i : render_targets) {

    }
}


harpy::D3::renderer::renderer() {
    init_queue_managers();
    init_thread_resources();
    init_default_texture();
    init_descriptors();
    init_default_pipeline();
}

//TODO: add texture support
void harpy::D3::renderer::add_render_target(model& model) {
    static uint32_t copy_times = 0;
    std::unique_ptr<D3::model> model_copy = std::make_unique<D3::model>();

    model_copy->set_id(model.get_id().append(std::to_string(++copy_times)));
    model_copy->set_pipeline_id(model.get_pipeline_id());
    model_copy->mesh_loading_path = model.mesh_loading_path;
    model_copy->model_matrix = model.model_matrix;
    model_copy->texture_loading_path = model.texture_loading_path;

    model_copy->get_mesh().get_indices().init(model.msh.get_indices().get_size());
    model_copy->get_mesh().get_vertices().init(model.msh.get_vertices().get_size());

    std::unique_ptr temp_thing = std::move(commander.unbind_thread_res());
    commander.bind_thread_res(std::move(*transfer_thread_resource));
    auto promise {
        harpy::utilities::default_thread_pool::get_singleton().enqueue(commander.reset_pool()
    ->start_recording()
    ->copy_buffer(model.get_mesh().get_vertices(), model_copy->get_mesh().get_vertices(), model.get_mesh().get_vertices().get_size())
    ->copy_buffer(model.get_mesh().get_indices(), model_copy->get_mesh().get_indices(), model.get_mesh().get_indices().get_size())
    ->end_recording())
    };
    promise.wait();
    commander.submit();
    (*transfer_thread_resource) = std::move(commander.unbind_thread_res());
    commander.bind_thread_res(std::move(temp_thing));
}

void harpy::D3::renderer::add_render_target(sz::string_view path_to_model, sz::string_view id,  sz::string_view pipeline_id) {
    if(thread_resources.size() == 1) {
        auto resource {std::make_unique<nest::resources::command_thread_resource>
            (queue_managers.front().get_queue(),std::make_unique<nest::pools::command_pool>(queue_managers.front().get_family_index()))};
        model_loader loader{std::move(resource)};
        render_targets.emplace_back(loader.load_model(path_to_model, id, pipeline_id));
    } else {
        for(int i = thread_resources.size() -1; i >= 0; i--) {
            if(thread_resources[i]->queue.get_type() == nest::wrappers::queue_type::transfer) {
                model_loader loader{std::move(thread_resources[i])};
                render_targets.push_back({loader.load_model(path_to_model, id, pipeline_id)});
            }
        }
    };
}

harpy::D3::renderer * harpy::D3::renderer::change_viewport_sizes(float x, float y, float width, float height) {
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    return this;
}

void harpy::D3::renderer::fast_load_texture_into_model(model* md) {
    if(!md)
        return;

    auto& texture = md->texture;
    utilities::image image{md->texture_loading_path};
    std::unique_ptr temp_thing = std::move(commander.unbind_thread_res());
    commander.bind_thread_res(std::move(*transfer_thread_resource));
    auto promise {
        harpy::utilities::default_thread_pool::get_singleton().enqueue(commander.reset_pool()
    ->start_recording()
    ->load_into_texture(texture, image)
    ->end_recording())
    };
    promise.wait();
    commander.submit();
    (*transfer_thread_resource) = std::move(commander.unbind_thread_res());
    commander.bind_thread_res(std::move(temp_thing));
}

std::unique_ptr<harpy::D3::model> & harpy::D3::renderer::get_render_target(sz::string_view model_id) {
    for(auto& i : render_targets) {
        if(i->get_id() == model_id)
            return i;
    }
    throw utilities::harpy_little_error("Wrong model id!");
}

harpy::D3::renderer * harpy::D3::renderer::set_cam(camera cam) {
    this->cam = std::move(cam);
    return this;
}

harpy::D3::renderer * harpy::D3::renderer::render() {

    update_uniform_buffers();

    auto image_index = swapchain.acquire_vk_image_index(&semaphore, nullptr);
    auto& frame_data = frames_data[image_index];
    frame_data.render_fence.wait();

    if(!commander.is_binded())
        commander.bind_thread_res(std::move(thread_resources.front()));

    commander.reset_pool()
    ->start_recording()
            ->start_render_pass(swapchain, image_index)
    ->set_scissors(scissors)
    ->set_viewport(viewport);
    
    sz::string last_pipeline{};
    //TODO: make benchmark for is_really last mesh is faster, then bind over and over the same mesh
    mesh* last_mesh {nullptr};
    
    for(auto& i : render_targets) {
        if(!pipelines.contains(i->get_pipeline_id()))
        throw utilities::harpy_little_error(utilities::error_severity::not_init, "Wrong pipeline id!");

        if(last_pipeline != i->get_pipeline_id()) {
            last_pipeline = i->get_pipeline_id();
            commander.bind_pipeline(pipelines.at(last_pipeline));
        }
        
        commander.bind_push_constants(pipelines.at(last_pipeline), i->model_matrix)
        ->bind_descriptor_sets(frame_data.global_descriptor_set, pipelines.at(last_pipeline));
        auto& mesh = i->get_mesh();
        commander.bind_vertex_buffers(mesh.get_vertices())->
        bind_index_buffer(mesh.get_indices())->
        draw_indexed(mesh.get_indices().get_size() / sizeof(uint32_t));
    }

    auto delegate = commander.end_render_pass()
                    ->end_recording();

    utilities::default_thread_pool::get_singleton().enqueue(delegate).wait();
    commander.submit(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, &frame_data.render_fence, &frame_data.render_finish_sem, &frame_data.present_sem);
    return this;
}

void harpy::D3::renderer::show_on_screen()
{
    auto image_index = swapchain.acquire_vk_image_index(&semaphore, nullptr);
    auto& frame_data = frames_data[image_index];
    //We can always assume, that the first queue is present because it just how it works
    static VkQueue present_queue = queue_managers.front().get_queue();
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    //Just for now: render finish semaphore
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &frame_data.render_finish_sem.get_vk_semaphore();

    //Just for now: showing one image on one "main" swapchain
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain.get_vk_swapchain();

    presentInfo.pImageIndices = &image_index;

    HARPY_VK_CHECK(vkQueuePresentKHR(present_queue, &presentInfo));
}

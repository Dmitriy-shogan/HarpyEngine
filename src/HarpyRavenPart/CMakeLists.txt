add_executable(HarpyRavenExe ${HARPY_RAVEN_MODULE}/nest/inititalizations.h
        nest/inititalizations.cpp
        nest/command_commander/command_commander.cpp
        nest/managers/queue_manager.cpp
        nest/managers/swapchain_manager.cpp
        nest/pipeline/graphics_pipeline.cpp
        nest/pipeline/pipeline_cache.cpp
        nest/pipeline/descriptors/descriptor_allocator.cpp
        nest/pipeline/descriptors/descriptor_factory.cpp
        nest/pools/command_pool.cpp
        nest/pools/descriptor_pool.cpp
        nest/resources/common_vulkan_resource.cpp
        nest/shader_works/shader_module.cpp
        nest/shader_works/glsl_shader_factory.cpp
        nest/shader_works/hlsl_shader_factory.cpp
        nest/texturing/texture.cpp
        nest/texturing/texture_sampler.cpp
        nest/vulkan_threading/event.cpp
        nest/vulkan_threading/fence.cpp
        nest/vulkan_threading/pipeline_barrier.cpp
        nest/vulkan_threading/semaphor.cpp
        nest/windowing/window.cpp
        nest/wrappers/buffers/data_buffer.cpp
        nest/wrappers/queues/queue_family.cpp
        nest/wrappers/swapchain/swapchain.cpp
        3D/mesh.cpp
        3D/renderer.cpp
        main.cpp)

add_library(HarpyRavenLib
        ${HARPY_RAVEN_MODULE}/nest/inititalizations.h
        nest/inititalizations.cpp
        nest/command_commander/command_commander.cpp
        nest/managers/queue_manager.cpp
        nest/managers/swapchain_manager.cpp
        nest/pipeline/graphics_pipeline.cpp
        nest/pipeline/pipeline_cache.cpp
        nest/pipeline/descriptors/descriptor_allocator.cpp
        nest/pipeline/descriptors/descriptor_factory.cpp
        nest/pools/command_pool.cpp
        nest/pools/descriptor_pool.cpp
        nest/resources/common_vulkan_resource.cpp
        nest/shader_works/shader_module.cpp
        nest/shader_works/glsl_shader_factory.cpp
        nest/shader_works/hlsl_shader_factory.cpp
        nest/texturing/texture.cpp
        nest/texturing/texture_sampler.cpp
        nest/vulkan_threading/event.cpp
        nest/vulkan_threading/fence.cpp
        nest/vulkan_threading/pipeline_barrier.cpp
        nest/vulkan_threading/semaphor.cpp
        nest/windowing/window.cpp
        nest/wrappers/buffers/data_buffer.cpp
        nest/wrappers/queues/queue_family.cpp
        nest/wrappers/swapchain/swapchain.cpp
        3D/mesh.cpp
        3D/renderer.cpp)

file(COPY ${CMAKE_SOURCE_DIR}/external_resources DESTINATION ${CMAKE_BINARY_DIR})
target_compile_definitions(HarpyRavenLib PRIVATE HARPY_RAVEN_DLL_EXPORTS)
target_compile_definitions(HarpyRavenExe PRIVATE HARPY_RAVEN_DLL_EXPORTS)

target_include_directories(HarpyRavenLib
        PRIVATE ${glfw_SOURCE_DIR}/include
                ${volk_SOURCE_DIR}
                ${vma_SOURCE_DIR}/include
                ${Vulkan_INCLUDE_DIRS}
        PUBLIC  ${HARPY_UTILITIES_MODULE}
                ${glm_SOURCE_DIR}
                ${HARPY_INCLUDES}/HarpyRavenPart)
target_link_libraries(HarpyRavenLib
        PRIVATE glfw
                GPUOpen::VulkanMemoryAllocator
                volk_headers
                "${Vulkan_INCLUDE_DIRS}/../Lib/shaderc_shared.lib"
        PUBLIC HarpyUtilities
                glm)
target_include_directories(HarpyRavenExe
        PRIVATE ${glfw_SOURCE_DIR}/include
        ${volk_SOURCE_DIR}
        ${vma_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
        PUBLIC  ${HARPY_UTILITIES_MODULE}
        ${glm_SOURCE_DIR}
        ${HARPY_INCLUDES}/HarpyRavenPart)
target_link_libraries(HarpyRavenExe
        PRIVATE glfw
        GPUOpen::VulkanMemoryAllocator
        volk_headers
        "${Vulkan_INCLUDE_DIRS}/../Lib/shaderc_shared.lib"
        PUBLIC HarpyUtilities
        glm)
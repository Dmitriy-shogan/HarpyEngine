add_executable(HarpyRavenExe ${HARPY_RAVEN_MODULE}/nest/inititalizations.h
        nest/inititalizations.cpp
        nest/command_commander/command_commander.cpp
        nest/managers/queue_manager.cpp
        nest/pipeline/graphics_pipeline.cpp
        nest/pipeline/pipeline_cache.cpp
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
        nest/wrappers/data_buffer.cpp
        nest/wrappers/queue_family.cpp
        nest/wrappers/swapchain.cpp
        3D/mesh.cpp
        3D/renderer.cpp
        main.cpp
        ../../include/HarpyRavenPart/3D/uniform_buffer_objects.h
        nest/resources/depth_image.cpp
        ../../include/HarpyRavenPart/nest/resources/depth_image.h
        nest/windowing/input_controller.cpp
        ../../include/HarpyRavenPart/nest/windowing/input_controller.h
        ../../include/HarpyUtilities/utilities/allocators/allocator.h
        nest/wrappers/render_pass.cpp
        ../../include/HarpyRavenPart/nest/wrappers/render_pass.h
        3D/model_loader.cpp
        ../../include/HarpyRavenPart/3D/model_loader.h
        3D/model.cpp
        ../../include/HarpyRavenPart/3D/model.h
        3D/camera.cpp
        ../../include/HarpyRavenPart/3D/camera.h
        3D/primitive_models/circle.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/circle.h
        3D/primitive_models/sphere.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/sphere.h
        3D/primitive_models/point.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/point.h
        3D/primitive_models/rectangle.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/rectangle.h
        3D/primitive_models/cube.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/cube.h
        3D/primitive_models/line.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/line.h)

add_library(HarpyRavenLib
        ${HARPY_RAVEN_MODULE}/nest/inititalizations.h
        nest/inititalizations.cpp
        nest/command_commander/command_commander.cpp
        nest/managers/queue_manager.cpp
        nest/pipeline/graphics_pipeline.cpp
        nest/pipeline/pipeline_cache.cpp
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
        nest/wrappers/data_buffer.cpp
        nest/wrappers/queue_family.cpp
        nest/wrappers/swapchain.cpp
        3D/mesh.cpp
        3D/renderer.cpp
        nest/resources/depth_image.cpp
        ../../include/HarpyRavenPart/nest/resources/depth_image.h
        nest/windowing/input_controller.cpp
        ../../include/HarpyRavenPart/nest/windowing/input_controller.h
        ../../include/HarpyUtilities/utilities/allocators/allocator.h
        nest/wrappers/render_pass.cpp
        ../../include/HarpyRavenPart/nest/wrappers/render_pass.h
        3D/model_loader.cpp
        ../../include/HarpyRavenPart/3D/model_loader.h
        3D/model.cpp
        ../../include/HarpyRavenPart/3D/model.h
        3D/camera.cpp
        ../../include/HarpyRavenPart/3D/camera.h
        3D/primitive_models/Circle.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/circle.h
        3D/primitive_models/rectangle.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/rectangle.h
        3D/primitive_models/cube.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/cube.h
        3D/primitive_models/line.cpp
        ../../include/HarpyRavenPart/3D/primitive_models/line.h)

file(COPY ${CMAKE_SOURCE_DIR}/external_resources DESTINATION ${CMAKE_BINARY_DIR})
target_compile_definitions(HarpyRavenLib PRIVATE HARPY_RAVEN_DLL_EXPORTS)
target_compile_definitions(HarpyRavenExe PRIVATE HARPY_RAVEN_DLL_EXPORTS)

target_include_directories(HarpyRavenLib
        PRIVATE ${glfw_SOURCE_DIR}/include
                ${volk_SOURCE_DIR}
                ${vma_SOURCE_DIR}/include
                ${Vulkan_INCLUDE_DIRS}
                ${assimp_SOURCE_DIR}/include
                ${HARPY_CODEGEN_MODULE}
        PUBLIC  ${HARPY_UTILITIES_MODULE}
                ${glm_SOURCE_DIR}
                ${HARPY_INCLUDES}/HarpyRavenPart
)

target_link_libraries(HarpyRavenLib
        PRIVATE glfw
                GPUOpen::VulkanMemoryAllocator
                volk_headers
                "${Vulkan_INCLUDE_DIRS}/../Lib/shaderc_shared.lib"
                assimp
                HarpyCodeGen
        PUBLIC HarpyUtilities
                glm)

target_include_directories(HarpyRavenExe
        PRIVATE ${glfw_SOURCE_DIR}/include
        ${volk_SOURCE_DIR}
        ${vma_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
        ${assimp_SOURCE_DIR}/include
        ${HARPY_CODEGEN_MODULE}
        PUBLIC  ${HARPY_UTILITIES_MODULE}
        ${glm_SOURCE_DIR}
        ${HARPY_INCLUDES}/HarpyRavenPart)

target_link_libraries(HarpyRavenExe
        PRIVATE glfw
        GPUOpen::VulkanMemoryAllocator
        volk_headers
        "${Vulkan_INCLUDE_DIRS}/../Lib/shaderc_shared.lib"
        assimp
        HarpyCodeGen
        PUBLIC HarpyUtilities
        glm)
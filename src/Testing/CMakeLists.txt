add_executable(Test Testing.cpp)
target_include_directories(Test
        PRIVATE ${glfw_SOURCE_DIR}/include
        ${volk_SOURCE_DIR}
        ${vma_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
        ${glm_SOURCE_DIR}
        PUBLIC HarpyRavenLib
        ${HARPY_RAVEN_MODULE}

)
target_link_libraries(Test
        PRIVATE glfw
        GPUOpen::VulkanMemoryAllocator
        volk_headers
        "${Vulkan_INCLUDE_DIRS}/../Lib/shaderc_shared.lib"
        PUBLIC HarpyRavenLib
        HarpyUtilities
        glm
)
cmake_minimum_required(VERSION 3.28)
project(HarpyEngine VERSION 0.01)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

include(FetchContent)

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

#[[To not check is dependency up-to-date]]
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Do not update everytime I run cmake, please" FORCE)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
#[[Not working right now]]
option(HARPY_BUILD_TESTS "Build the harpy test programs" OFF)
option(HARPY_INSTALL "Generate installation target" OFF)
option(HARPY_BENCHMARK "For testing and measurements" OFF)

set(WITH_UNIFONT OFF CACHE BOOL "" FORCE)
set(WITH_ADE OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_apps OFF CACHE BOOL "")
set(BUILD_opencv_calib3d OFF CACHE BOOL "")
set(BUILD_opencv_core ON CACHE BOOL "")
set(BUILD_opencv_dnn OFF CACHE BOOL "")
set(BUILD_opencv_features2d OFF CACHE BOOL "")
set(BUILD_opencv_flann OFF CACHE BOOL "")
set(BUILD_opencv_gapi OFF CACHE BOOL "")
set(BUILD_opencv_highgui OFF CACHE BOOL "")
set(BUILD_opencv_imgcodecs ON CACHE BOOL "")
set(BUILD_opencv_imgproc ON CACHE BOOL "")
set(BUILD_opencv_java OFF CACHE BOOL "")
set(BUILD_opencv_java_bindings_generator OFF CACHE BOOL "")
set(BUILD_opencv_js OFF CACHE BOOL "")
set(BUILD_opencv_ml OFF CACHE BOOL "")
set(BUILD_opencv_objdetect OFF CACHE BOOL "")
set(BUILD_opencv_photo OFF CACHE BOOL "")
set(BUILD_opencv_python2 OFF CACHE BOOL "")
set(BUILD_opencv_python3 OFF CACHE BOOL "")
set(BUILD_opencv_python_bindings_generator OFF CACHE BOOL "")
set(BUILD_opencv_stitching OFF CACHE BOOL "")
set(BUILD_opencv_ts OFF CACHE BOOL "")
set(BUILD_opencv_video ON CACHE BOOL "")
set(BUILD_opencv_videoio ON CACHE BOOL "")
set(BUILD_opencv_world OFF CACHE BOOL "")
set(WITH_VULKAN ON CACHE BOOL "")

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_INSTALL OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "")
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "")
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "")
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "")


FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw)
FetchContent_Declare(ffmpeg
        GIT_REPOSITORY https://git.ffmpeg.org/ffmpeg.git
)
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/icaven/glm)
FetchContent_Declare(vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
FetchContent_Declare(volk
        GIT_REPOSITORY https://github.com/zeux/volk)
FetchContent_Declare(opencv
        GIT_REPOSITORY https://github.com/opencv/opencv
        GIT_TAG 5.x)
FetchContent_Declare(assimp
        GIT_REPOSITORY https://github.com/assimp/assimp)



FetchContent_MakeAvailable(glfw ffmpeg glm vma volk opencv assimp)

set(HARPY_INCLUDES "${PROJECT_SOURCE_DIR}/include")
set(HARPY_SOURCES "${PROJECT_BINARY_DIR}/src")
set(HARPY_RAVEN_MODULE "${HARPY_INCLUDES}/HarpyRavenPart")
set(HARPY_HUMAN_MODULE "${HARPY_INCLUDES}/HarpyHumanPart")
set(HARPY_UTILITIES_MODULE "${HARPY_INCLUDES}/HarpyUtilities")
set(HARPY_CODEGEN_MODULE "${HARPY_INCLUDES}/HarpyCodeGen")
set(WORKING_DIR "${CMAKE_BINARY_DIR}/build")


if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(LIBRARY_OUTPUT_PATH "${WORKING_DIR}/dlib" CACHE PATH "Build directory" FORCE)
    set(CMAKE_SHARED_LIBRARY_PREFIX "d")
    set(CMAKE_STATIC_LIBRARY_PREFIX "d")
    add_compile_definitions(_DEBUG HARPY_DEBUG)
else()
    set(LIBRARY_OUTPUT_PATH "${WORKING_DIR}/lib" CACHE PATH "Build directory" FORCE)
endif()

if(HARPY_BENCHMARK)
    add_compile_definitions(HARPY_BENCHMARK)
endif()


add_subdirectory(src)

add_executable(Engine main.cpp)

target_include_directories(Engine PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(Engine PRIVATE HarpyRavenLib HarpyUtilities)


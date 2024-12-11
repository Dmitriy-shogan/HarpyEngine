set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WORKING_DIR}/runtime_output")
set(CMAKE_BUILD_FILES_DIRECTORY "${WORKING_DIR}/runtime_output")
set(CMAKE_BUILD_DIRECTORY "${WORKING_DIR}/runtime_output")

add_library(HarpyUtilities
        utilities/images/image.cpp
        utilities/util/util.cpp
        utilities/logger/harpy_little_error.cpp
        utilities/logger/logger.cpp
        utilities/util/delegate.cpp
        ${HARPY_INCLUDES}/HarpyUtilities/utilities_dll_macro.h
)
target_compile_definitions(HarpyUtilities PRIVATE HARPY_UTILITIES_EXPORTS)
target_include_directories(HarpyUtilities PUBLIC  ${CMAKE_BINARY_DIR}
                                                  ${HARPY_INCLUDES}/HarpyUtilities/utilities
                                                  ${opencv_SOURCE_DIR}/include
                                                  ${opencv_SOURCE_DIR}/modules/core/include
                                                  ${opencv_SOURCE_DIR}/modules/imgcodecs/include
                                                  ${opencv_SOURCE_DIR}/modules/imgproc/include
                                                  ${opencv_SOURCE_DIR}/modules/video/include
                                                  ${opencv_SOURCE_DIR}/modules/videoio/include
                                                  ${stringzilla_SOURCE_DIR}/include
                                                  )
target_link_libraries(HarpyUtilities PUBLIC opencv_core opencv_imgcodecs opencv_imgproc opencv_video opencv_videoio ${STRINGZILLA_TARGET_NAME})

add_executable(utilities_debug
        utilities/debug.cpp
        utilities/images/image.cpp
        utilities/util/util.cpp
        utilities/logger/harpy_little_error.cpp
        utilities/logger/logger.cpp
        utilities/util/delegate.cpp
        ${HARPY_INCLUDES}/HarpyUtilities/utilities_dll_macro.h)
target_compile_definitions(utilities_debug PRIVATE HARPY_UTILITIES_EXPORTS)
target_include_directories(utilities_debug PRIVATE  ${CMAKE_BINARY_DIR}
        ${HARPY_INCLUDES}/HarpyUtilities/utilities
        ${opencv_SOURCE_DIR}/include
        ${opencv_SOURCE_DIR}/modules/core/include
        ${opencv_SOURCE_DIR}/modules/imgcodecs/include
        ${opencv_SOURCE_DIR}/modules/imgproc/include
        ${opencv_SOURCE_DIR}/modules/video/include
        ${opencv_SOURCE_DIR}/modules/videoio/include
        ${stringzilla_SOURCE_DIR}/include
)
target_link_libraries(utilities_debug PRIVATE opencv_core opencv_imgcodecs opencv_imgproc opencv_video opencv_videoio ${STRINGZILLA_TARGET_NAME})
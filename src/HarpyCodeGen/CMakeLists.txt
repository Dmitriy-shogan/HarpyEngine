add_library(HarpyCodeGen
        cmake/cmake_generator.cpp
        cmake/cmake_generator.hpp
        shader/glsl_generator.cpp
        ../../include/HarpyCodeGen/shader/glsl_generator.hpp
        cpp_code/cpp_generator.cpp
        cpp_code/cpp_generator.hpp
        ../../include/HarpyCodeGen/codegen_dll_macro.h
)

target_include_directories(HarpyCodeGen PUBLIC
        ${HARPY_INCLUDES}/HarpyUtilities/utilities
        ${HARPY_INCLUDES}/HarpyCodeGen
)

target_compile_definitions(HarpyCodeGen PRIVATE HARPY_CODEGEN_EXPORTS)

target_link_libraries(HarpyCodeGen PUBLIC HarpyUtilities)
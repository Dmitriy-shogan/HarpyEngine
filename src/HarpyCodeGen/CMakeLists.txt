add_library(HarpyCodeGen
        cmake/cmake_generator.cpp
        cmake/cmake_generator.hpp
        shader/glsl_generator.cpp
        ${HARPY_CODEGEN_MODULE}/shader/glsl_generator.hpp
        cpp_code/cpp_generator.cpp
        cpp_code/cpp_generator.hpp
)

target_include_directories(HarpyCodeGen PUBLIC
        ${HARPY_INCLUDES}/HarpyUtilities/utilities
        ${HARPY_CODEGEN_MODULE}
)

target_compile_definitions(HarpyCodeGen PRIVATE HARPY_CODEGEN_EXPORTS)

target_link_libraries(HarpyCodeGen PUBLIC HarpyUtilities)
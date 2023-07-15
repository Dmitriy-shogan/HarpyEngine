cmake_minimum_required(VERSION 3.5)
project (harpy_engine)
set (CMAKE_CXX_STANDARD 20)

#INLUDE FOLDERS RECURSIVE SEARCH MACRO
MACRO(HEADER_DIRECTORIES PATH1 return_list)
    FILE(GLOB_RECURSE new_list ${PATH1}/*.h)
    SET(dir_list "${PATH1}")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)

#HEADER_DIRECTORIES(${CMAKE_SOURCE_DIR}/human_part header_dir_list)

#list(LENGTH header_dir_list header_dir_list_count)
#message(STATUS "[INFO] Found ${header_dir_list_count} header directories.")

#include_directories(
#    ${header_dir_list} # Recursive
#)


#HEADER_DIRECTORIES(${CMAKE_SOURCE_DIR}/raven_part header_dir_list2)
#
#list(LENGTH header_dir_list2 header_dir_list_count2)
#message(STATUS "[INFO] Found ${header_dir_list_count2} header directories.")
#
#include_directories(
#    ${header_dir_list2} # Recursive
#)

set(header_dir_list 
    "third_party_services/include/"
    "raven_part/nest/"
    "raven_part/"
    "human_part/"
    )
    
set(third_party_lib_list 
    "third_party_services/libs/"
    "third_party_services/debug_libs/"
    "third_party_services/dlls/"
    )
    
set(link_flags 
    "GL"
    "glfw"
    "vulkan"
    )
set(local_link_flags 
    "${PROJECT_NAME}_library"
    )

############################################################
# HEADERS
############################################################


list(LENGTH header_dir_list header_dir_list_count)
message(STATUS "[INFO] Found ${header_dir_list_count} header directories.")
include_directories(${header_dir_list})

############################################################
# Create a library
############################################################
set(lib_src_list "")

FILE(GLOB_RECURSE tmp_lib_src_list ${CMAKE_SOURCE_DIR}/raven_part/*.cpp)
list (APPEND lib_src_list ${tmp_lib_src_list})
FILE(GLOB_RECURSE tmp_lib_src_list ${CMAKE_SOURCE_DIR}/human_part/*.cpp)
list (APPEND lib_src_list ${tmp_lib_src_list})
list (APPEND lib_src_list "harpy.cpp")
message(STATUS "[INFO] ${lib_src_list}")

add_library(${PROJECT_NAME}_library STATIC ${lib_src_list})




############################################################
# Create an executable
############################################################

add_executable(${PROJECT_NAME}_binary Main.cpp) 

target_link_directories(${PROJECT_NAME}_binary
    PRIVATE 
       "libs/" ${third_party_lib_list}
)

target_link_libraries(${PROJECT_NAME}_binary
    PRIVATE 
        ${local_link_flags} ${link_flags}
)





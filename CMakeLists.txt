cmake_minimum_required(VERSION 3.12)

project(nosync)

file(GLOB_RECURSE nosync_SRC FOLLOW_SYMLINKS CONFIGURE_DEPENDS "src/*.cpp")
add_library(nosync STATIC ${nosync_SRC})

file(GLOB_RECURSE nosync_ux_SRC FOLLOW_SYMLINKS CONFIGURE_DEPENDS "src-ux/*.cpp")
add_library(nosync_ux STATIC ${nosync_ux_SRC})

target_compile_features(nosync PUBLIC cxx_std_17)

target_compile_features(nosync_ux PUBLIC cxx_std_17)

target_include_directories(
    nosync
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_include_directories(
    nosync_ux
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src-ux)

find_package(Threads REQUIRED)

target_link_libraries(nosync ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(nosync_ux nosync ${CMAKE_THREAD_LIBS_INIT})

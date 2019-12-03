cmake_minimum_required(VERSION 3.13)

project(meta)

add_library(meta INTERFACE)

add_subdirectory(src)
add_subdirectory(include)

target_compile_features(meta INTERFACE cxx_std_17)

#Set(CMAKE_CXX_FLAGS "-Wall" "-Wextra")

install(TARGETS meta EXPORT meta DESTINATION lib/Meta)
install(EXPORT meta DESTINATION lib/Meta)
install(FILES meta-config.cmake DESTINATION lib/Meta)
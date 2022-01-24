cmake_minimum_required(VERSION 3.8)

project("sa2-gravity-camera")

add_library("${PROJECT_NAME}" SHARED "src/main.cpp")

target_compile_features("${PROJECT_NAME}" PUBLIC cxx_std_20)
set_target_properties("${PROJECT_NAME}" PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories("${PROJECT_NAME}" PRIVATE
    "extern/sa2-mod-loader/SA2ModLoader/include"
    "extern/SA2FlipScreen"
)

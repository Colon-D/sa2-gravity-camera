cmake_minimum_required(VERSION 3.8)

project("sa2-gravity-camera")

# compile ModLoaderCommon (for IniFile)
add_library("mod-loader-common" STATIC
    "extern/mod-loader-common/ModLoaderCommon/IniFile.cpp"
    "extern/mod-loader-common/ModLoaderCommon/TextConv.cpp"
    "extern/mod-loader-common/ModLoaderCommon/FileSystem.cpp"
    "extern/mod-loader-common/ModLoaderCommon/CodeParser.cpp"
)
target_include_directories("mod-loader-common" PRIVATE
    "extern/mod-loader-common/ModLoaderCommon"
)
set_target_properties("mod-loader-common" PROPERTIES CXX_EXTENSIONS OFF)

# compile my code (and FlipScreen's flipscreen.cpp)
add_library("${PROJECT_NAME}" SHARED
    "src/main.cpp"
)

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)
set_target_properties("${PROJECT_NAME}" PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories("${PROJECT_NAME}" PRIVATE
    "extern/sa2-mod-loader/SA2ModLoader/include"
    "extern/SA2FlipScreen"
    "extern/mod-loader-common/ModLoaderCommon"
)

target_link_libraries("${PROJECT_NAME}" PRIVATE
    "mod-loader-common"
)

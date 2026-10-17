cmake_minimum_required(VERSION 3.28)
project(JumpIchibanWatcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated")
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Autogen")

# Dependemcies
include(Dependencies.cmake)

# Compilation options
if(MSVC)
    add_compile_options(
        /W4
        /utf-8
    )
endif()

# Projects
add_subdirectory(JumpIchibanWatcher)
add_subdirectory(Core)
add_subdirectory(Ui)

# StartUp Project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT JumpIchibanWatcher)

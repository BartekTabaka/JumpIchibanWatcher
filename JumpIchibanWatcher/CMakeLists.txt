#
# App
#

# Files
set(ASSET_FILES

)

set(SOURCES
    Source/Main.cpp

    ${ASSET_FILES}
)
qt_add_executable(JumpIchibanWatcher)
target_sources(JumpIchibanWatcher PRIVATE ${SOURCES})

target_include_directories(JumpIchibanWatcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Libraries
target_link_libraries(JumpIchibanWatcher PRIVATE
    Qt6::Widgets
    Qt6::Network
    Core
)

# Windows-only stuff
if (WIN32)
    set_target_properties(JumpIchibanWatcher PROPERTIES 
        WIN32_EXECUTABLE $<CONFIG:RELEASE>
    )
endif()

# Compilation options
target_compile_options(JumpIchibanWatcher PRIVATE
    /W4
    /utf-8
)

# Working dir
set_target_properties(JumpIchibanWatcher PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/JumpIchibanWatcher"
)

source_group("Assets" FILES ${ASSET_FILES})

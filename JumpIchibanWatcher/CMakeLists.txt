#
# App
#

# Files
set(APP_SOURCES
    Source/Main.cpp

    Source/App.h
    Source/App.cpp
    Source/AppIdentity.h
    Source/NetworkManager.h
    Source/NetworkManager.cpp
    Source/Settings.h
    Source/Settings.cpp
)

# Add executable (.exe)
qt_add_executable(JumpIchibanWatcher)

# Set sources
target_sources(JumpIchibanWatcher PRIVATE ${APP_SOURCES})
target_include_directories(JumpIchibanWatcher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Link libraries
target_link_libraries(JumpIchibanWatcher PRIVATE
    Qt6::Network
    Ui
)

# Windows-only stuff
if (WIN32)
    set_target_properties(JumpIchibanWatcher PROPERTIES 
        WIN32_EXECUTABLE $<CONFIG:RELEASE>
    )
endif()

# Working directory
set_target_properties(JumpIchibanWatcher PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/JumpIchibanWatcher"
)

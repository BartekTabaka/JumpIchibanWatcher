#
# Core
#

# Files
set(CORE_SOURCES
    Source/Core/Product.h
    Source/Core/Product.cpp
    Source/Core/Mapper.h
    Source/Core/Mapper.cpp
    Source/Core/UrlValidation.h
)

# Library definition    
add_library(Core STATIC)

# Set sources
target_sources(Core PRIVATE ${CORE_SOURCES})
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Link libraries
target_link_libraries(Core PUBLIC
    Qt6::Core
)

# File tree in VS
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CORE_SOURCES})
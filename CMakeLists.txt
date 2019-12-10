file(GLOB SOURCES src/*.cpp)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
add_subdirectory(external/recastnavigation)

if (BUILD_PATHFINDER_STATIC)
    add_library(pathfinder STATIC ${SOURCES})
else ()
    add_library(pathfinder SHARED ${SOURCES})
endif ()

add_library(SuperCLine::pathfinder ALIAS pathfinder)

add_definitions(-DPATHFINDER_API=__DLL_EXPORT)
add_definitions(-DCORE_API=__DLL_IMPORT)
if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 /W4 /Od /DDEBUG /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 /W4 /O2 /DNDEBUG /MD")
else ()
	set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -O0 -W -Wall -g -ggdb")
	set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -rdynamic -O2 -W -Wall -DNDEBUG")
	target_link_libraries(pathfinder pthread)
endif ()

# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_REQUIRED_FLAGS -std=c++17)
# set_property(TARGET log PROPERTY CXX_STANDARD 17)

# include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
# ExternalProject_Add(recastnavigation
#     GIT_REPOSITORY "https://github.com/recastnavigation/recastnavigation.git"
#     UPDATE_COMMAND git pull "https://github.com/recastnavigation/recastnavigation.git"
#     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/recastnavigation
#     INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(pathfinder core)
target_link_libraries(pathfinder core DebugUtils Detour DetourCrowd DetourTileCache Recast)

target_include_directories(pathfinder
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/inc/private" "${CMAKE_SOURCE_DIR}/include" 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc/public"
)

set_target_properties(pathfinder PROPERTIES
    SOVERSION ${SOVERSION}
    VERSION ${VERSION}
    COTIRE_CXX_PREFIX_HEADER_INIT "${CMAKE_CURRENT_SOURCE_DIR}/inc/private/stdafx.h"
)
cotire(pathfinder)

install(TARGETS pathfinder
    ARCHIVE DESTINATION lib/pathfinder
    LIBRARY DESTINATION lib/pathfinder
    RUNTIME DESTINATION bin
    COMPONENT library
)

file(GLOB INCLUDES inc/public/*.h)
install(FILES ${INCLUDES} DESTINATION include/pathfinder)
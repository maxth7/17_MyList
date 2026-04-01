cmake_minimum_required(VERSION 3.20)

project(MyListProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Windows-specific setting
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # Disable the console window for GUI applications (if necessary)
    # add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})
endif()

# the path to the source code
set(SOURCES
    src/ListDataConverter.cpp
    src/MyList.cpp
)

set(HEADERS
    src/ListDataConverter.h
    src/ListNode.h
)

# Creating an executable file
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Linux-specific настройки (ПЕРЕМЕЩЕНО СЮДА - после add_executable)
if(UNIX AND NOT APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:DEBUG>:-g -O0>
        $<$<CONFIG:RELEASE>:-O3 -DNDEBUG>
    )
endif()

# Cross-platform definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:DEBUG>:_DEBUG>
    $<$<BOOL:${WIN32}>:_WIN32>
    $<$<BOOL:${UNIX}>:_LINUX>
)

# Setting up the output directory (cross-platform)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# If additional libraries are needed

# target_link_libraries(${PROJECT_NAME} PRIVATE 
#     $<$<BOOL:${WIN32}>:ws2_32>  # Пример для Windows
#     $<$<BOOL:${UNIX}>:pthread>  # Пример для Linux
# )

# Successful configuration message
message(STATUS "Project ${PROJECT_NAME} configured successfully for ${CMAKE_SYSTEM_NAME}")
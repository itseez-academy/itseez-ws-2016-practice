CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    set(CMAKE_USE_RELATIVE_PATHS ON)
endif()

# Set configuration
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configurations" FORCE)
if(DEFINED CMAKE_BUILD_TYPE)
    set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
endif()

if(CMAKE_GENERATOR MATCHES "Makefiles|Ninja" AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Installation Directory")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${cfg}" cfg)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_PDB_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib)
endforeach()


set(PROJECT_NAME skeleton)
project(${PROJECT_NAME})

set(LIBRARY    "${PROJECT_NAME}_filter")
set(TESTS      "test_${PROJECT_NAME}")
set(PERF_TESTS "perf_${PROJECT_NAME}")
set(CMAKE_CXX_FLAGS "-std=c++11")

# Find OpenCV
find_package(OpenCV REQUIRED)
set(LIBRARY_DEPS ${OpenCV_LIBS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS})

# Add modules
add_subdirectory(3rdparty/gtest)
add_subdirectory(3rdparty/opencv_ptest)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(perf)
add_subdirectory(sample)

# Report generation summary
message( STATUS "")
message( STATUS "General configuration for ${PROJECT_NAME}")
message( STATUS "======================================")
message( STATUS "")
if(DEFINED CMAKE_BUILD_TYPE)
message( STATUS "   Configuration:     ${CMAKE_BUILD_TYPE}")
else()
message( STATUS "   Configuration:     <defined by IDE>")
endif()
message( STATUS "   OpenCV build path: ${OpenCV_DIR}")
message( STATUS "")

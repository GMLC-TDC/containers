# ~~~
# Copyright (c) 2017-2022, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~

cmake_minimum_required(VERSION 3.10...3.25)

project(GMLC_CONTAINERS VERSION 0.5.0)

# -----------------------------------------------------------------------------
# GMLC CONTAINERS library Version number
# -----------------------------------------------------------------------------
set(GMLC_CONTAINERS_VERSION_BUILD)
set(GMLC_CONTAINERS_DATE "2022-10-25")

set(GMLC_CONTAINERS_VERSION_STRING
    "${GMLC_CONTAINERS_VERSION} (${GMLC_CONTAINERS_DATE})"
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # User settable
    set(GMLC_CONTAINERS_CXX_STD
        "17"
        CACHE STRING "The CMake standard to require"
    )

    set(CMAKE_CXX_STANDARD ${GMLC_CONTAINERS_CXX_STD})
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
# -----------------------------------------------------------------------------
# set the module path and include some common macros
# -----------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/config/cmake/"
                      "${PROJECT_SOURCE_DIR}/ThirdParty/cmake/"
)
include(extraMacros)
include(CMakeDependentOption)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Prohibit in-source build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(
        FATAL_ERROR
            "In-source build is not supported. Please, use an empty directory for building the project."
    )
endif()

add_library(containers_base INTERFACE)

include(compiler_flags)
target_link_libraries(containers_base INTERFACE compile_flags_target)

# -------------------------------------------------------------
# add threading support
# -------------------------------------------------------------
if(MSYS OR CYGWIN)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()
find_package(Threads QUIET)

if(TARGET Threads::Threads)
    target_link_libraries(containers_base INTERFACE Threads::Threads)
    # else() SET(GMLC_CONTAINERS_BENCHMARK OFF)
endif()

option(GMLC_CONTAINERS_INCLUDE_BOOST "Enable some boost library headers to be used" ON)
# Add boost to test boost::optional if available
find_package(Boost 1.65)
if(Boost_FOUND)
    target_link_libraries(containers_base INTERFACE Boost::boost)

    target_compile_definitions(containers_base INTERFACE USE_BOOST_OPTIONAL)
    target_compile_definitions(containers_base INTERFACE ENABLE_BOOST_TYPES)

endif()

target_include_directories(containers_base INTERFACE ThirdParty)

add_subdirectory(gmlc)

option(GMLC_CONTAINERS_TEST "Enable tests for the containers library" ON)
option(GMLC_CONTAINERS_BENCHMARK "Enable benchmarks for the containers library" ON)

if(GMLC_CONTAINERS_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

if(GMLC_CONTAINERS_BENCHMARK)
    add_subdirectory(benchmarks)
endif()

# -------------------------------------------------------------
# Get some configuration for C++17 as that becomes available
# -------------------------------------------------------------
# message(STATUS ${CMAKE_CXX_FLAGS})

option(GMLC_CONTAINERS_GENERATE_DOXYGEN_DOC "Generate Doxygen doc target" OFF)

if(GMLC_CONTAINERS_GENERATE_DOXYGEN_DOC)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)

        show_variable(
            DOXYGEN_OUTPUT_DIR PATH "location to put Doxygen docs"
            "${PROJECT_BINARY_DIR}/docs"
        )
        configure_file(
            ${PROJECT_SOURCE_DIR}/config/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile
            @ONLY
        )
        add_custom_target(
            gmlc_containers_doc
            ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${DOXYGET_OUTPUT_DIR}
            COMMENT "Generating GMLC CONTAINERS API documentation with Doxygen"
            VERBATIM
        )
    endif(DOXYGEN_FOUND)
endif(GMLC_CONTAINERS_GENERATE_DOXYGEN_DOC)

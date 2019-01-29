#
# Copyright © 2017-2019, Battelle Memorial Institute; Lawrence Livermore National
# Security, LLC; Alliance for Sustainable Energy, LLC All rights reserved. See LICENSE
# file and DISCLAIMER for more details.
#

cmake_minimum_required(VERSION 3.4)
cmake_policy(VERSION 3.4)

project (GMLC_CONTAINERS VERSION 0.1)

# -----------------------------------------------------------------------------
# GMLC CONTAINERS library Version number
#-----------------------------------------------------------------------------
set (GMLC_CONTAINERS_VERSION_BUILD )
set (GMLC_CONTAINERS_DATE "01-30-19")

set(GMLC_CONTAINERS_VERSION_STRING "${GMLC_CONTAINERS_VERSION} (${GMLC_CONTAINERS_DATE})")

# -----------------------------------------------------------------------------
# set the module path and include some common macros
# -----------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/config/cmake/")
include(extraMacros)
include(CMakeDependentOption)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Prohibit in-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(
    FATAL_ERROR
      "In-source build is not supported. Please, use an empty directory for building the project."
  )
endif()

add_library(containers_base INTERFACE)

include(compiler_flags)
message(STATUS "setting version build options to \"${VERSION_OPTION}\"")
if(VERSION_OPTION)
  if(MSVC)
		target_compile_options(containers_base INTERFACE ${VERSION_OPTION})
	else()
    target_compile_options(
      containers_base
      INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${VERSION_OPTION}>
    )
  endif(MSVC)
endif(VERSION_OPTION)


# -------------------------------------------------------------
# add threading support
# -------------------------------------------------------------
if(NOT WIN32)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
elseif(MSYS)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
else()
	set(CMAKE_USE_WIN32_THREADS_INIT ON)
endif()
find_package(Threads REQUIRED)

target_link_libraries(containers_base INTERFACE Threads::Threads)

option(GMLC_CONTAINERS_INCLUDE_BOOST "Enable some boost library headers to be used" ON)
# Add boost to test boost::optional if available
find_package(Boost 1.58)
if(Boost_FOUND)
    target_link_libraries(containers_base INTERFACE Boost::boost)

    target_compile_definitions(containers_base INTERFACE USE_BOOST_OPTIONAL)
	target_compile_definitions(containers_base INTERFACE ENABLE_BOOST_TYPES)
    #target_compile_definitions(OptionalTest PUBLIC CLI11_BOOST_OPTIONAL)
endif()

target_include_directories(containers_base INTERFACE ThirdParty)

add_subdirectory(src)

option(GMLC_CONTAINERS_TEST "Enable tests for the containers library" ON)
option(GMLC_CONTAINERS_BENCHMARK "Enable benchmarks for the containers library" ON)

if (GMLC_CONTAINERS_TEST)
	enable_testing()
	add_subdirectory(tests)
endif()

if (GMLC_CONTAINERS_BENCHMARK)
    add_subdirectory(benchmarks)
endif()
# -------------------------------------------------------------
# add coverage target
# -------------------------------------------------------------
  cmake_dependent_option(
    TEST_CODE_COVERAGE
    "Build a target for testing code coverage"
    OFF
    "GMLC_CONTAINERS_TESTS"
    OFF
  )
  if(TEST_CODE_COVERAGE)
    if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
      include(CodeCoverage)

      set(
        COVERAGE_EXCLUDES
        'usr/*'
        'dependencies/*'
        'ThirdParty/*'
        'tests/*'
        'interfaces/*'
        'examples/*'
		'benchmarks/*'
      )
      setup_target_for_coverage(
        NAME
        containers_coverage # New target name
        EXECUTABLE
        CTest # Executable in PROJECT_BINARY_DIR
      )
        else()
      message(
        FATAL_ERROR "CMAKE_BUILD_TYPE must be set to Coverage for testing code coverage"
      )
        endif()
	endif(TEST_CODE_COVERAGE)

mark_as_advanced(TEST_CODE_COVERAGE)

# -------------------------------------------------------------
# Get some configuration for C++17 as that becomes available
# -------------------------------------------------------------
# message(STATUS ${CMAKE_CXX_FLAGS})

option(GENERATE_DOXYGEN_DOC "Generate Doxygen doc target" OFF)

if(GENERATE_DOXYGEN_DOC)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)

    show_variable(
      DOXYGEN_OUTPUT_DIR
      PATH
      "location to put Doxygen docs"
      "${PROJECT_BINARY_DIR}/docs"
    )
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/config/Doxyfile.in
      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY
    )
    add_custom_target(
      doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${DOXYGET_OUTPUT_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM
		)
	endif(DOXYGEN_FOUND)
endif(GENERATE_DOXYGEN_DOC)


# -------------------------------------------------------------
# BOOST  find the boost libraries
# -------------------------------------------------------------
#set(
#  BOOST_REQUIRED_LIBRARIES
#  program_options
#  filesystem
#  system
#)
#if (BUILD_TESTING)
#	list(APPEND BOOST_REQUIRED_LIBRARIES unit_test_framework)
#endif()
#include(addBoost)

# -------------------------------------------------------------
# add threading support
# -------------------------------------------------------------
if(NOT WIN32)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
elseif(MSYS)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
else()
	set(CMAKE_USE_WIN32_THREADS_INIT ON)
endif()
find_package(Threads REQUIRED)


if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/benchmark/CMakeLists.txt")
    message(FATAL_ERROR "You have requested tests be built, but googletest is not downloaded. Please run:
    git submodule update --init")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/config/cmake")


set(GOOGLE_TEST_INDIVIDUAL OFF)
include(AddGooglebenchmark)

set(CONTAINERS_TESTS
    containerTest1
    )

# Only affects current directory, so safe
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

foreach(T ${CLI11_TESTS})

    add_executable(${T} ${T}.cpp )
    #target_link_libraries(${T} PUBLIC CLI11)
    add_gtest(${T})

   
endforeach()


# Link test (build error if inlines missing)
add_library(link_test_1 link_test_1.cpp)
target_link_libraries(link_test_1 PUBLIC CLI11)
set_target_properties(link_test_1 PROPERTIES FOLDER "Tests")
add_executable(link_test_2 link_test_2.cpp)
target_link_libraries(link_test_2 PUBLIC CLI11 link_test_1)
add_gtest(link_test_2)

# Add boost to test boost::optional if available
find_package(Boost 1.58)
if(Boost_FOUND)
    target_link_libraries(informational PUBLIC Boost::boost)
    target_link_libraries(OptionalTest PUBLIC Boost::boost)

    # Enforce Boost::Optional even if __has_include is missing on your compiler
    #target_compile_definitions(informational PUBLIC CLI11_BOOST_OPTIONAL)
    #target_compile_definitions(OptionalTest PUBLIC CLI11_BOOST_OPTIONAL)
endif()

# Adding this printout to CTest
#file(WRITE "${PROJECT_BINARY_DIR}/CTestCustom.cmake"
#    "set(CTEST_CUSTOM_PRE_TEST \"${CMAKE_BINARY_DIR}/informational\")"
#    )

#if(CMAKE_BUILD_TYPE STREQUAL Coverage)
#    include(CodeCoverage)
#    setup_target_for_coverage(
#        NAME CLI11_coverage
#        EXECUTABLE ctest
#        DEPENDENCIES
#          ${CLI11_TESTS}
#          ${CLI11_MULTIONLY_TESTS})
#endif()


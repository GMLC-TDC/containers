if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/googletest/CMakeLists.txt")
    message(FATAL_ERROR "You have requested tests be built, but googletest is not downloaded. Please run:
    git submodule update --init")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/config/cmake")


set(GOOGLE_TEST_INDIVIDUAL OFF)
include(AddGoogletest)

set(CONTAINERS_TESTS
    StackBufferTests
	CircularBufferTests
	AirLockTests
	MappedVectorTests
	DualMappedVectorTests
	BlockingQueueTests
	SimpleQueueTests
	PriorityBlockingQueueTests
	StableBlockDequeTests
	StableBlockVectorTests
    )

# Only affects current directory, so safe
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

foreach(T ${CONTAINERS_TESTS})

    add_executable(${T} ${T}.cpp )
    target_link_libraries(${T} PUBLIC containers)
    add_gtest(${T})
	set_target_properties(${T} PROPERTIES FOLDER tests)
   
endforeach()

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


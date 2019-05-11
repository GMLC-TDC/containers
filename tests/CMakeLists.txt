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

if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    include(CodeCoverage)
    setup_target_for_coverage(
        NAME CONTAINERS_coverage
        EXECUTABLE ctest
        DEPENDENCIES
          ${CONTAINERS_TESTS}
          )
endif()


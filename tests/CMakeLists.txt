# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2017-2024, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set(GOOGLE_TEST_INDIVIDUAL OFF)
include(AddGoogletest)

set(CONTAINERS_TESTS
    StackBufferTests
    CircularBufferTests
    AirLockTests
    MappedVectorTests
    StringMappedVectorTests
    MappedVectorTestsStable
    DualMappedVectorTests
    DualStringMappedVectorTests
    DualMappedVectorTestsStable
    MappedPointerVectorTests
    DualMappedPointerVectorTests
    BlockingQueueTests
    SimpleQueueTests
    PriorityBlockingQueueTests
    StableBlockDequeTests
    StableBlockVectorTests
    WorkQueueTests
)

# Only affects current directory, so safe
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

foreach(T ${CONTAINERS_TESTS})

    add_executable(${T} ${T}.cpp)
    target_link_libraries(${T} PUBLIC containers)
    add_gtest(${T})
    set_target_properties(${T} PROPERTIES FOLDER tests)

endforeach()

if(CMAKE_BUILD_TYPE STREQUAL Coverage)
    include(CodeCoverage)
    setup_target_for_coverage(
        NAME CONTAINERS_coverage EXECUTABLE ctest DEPENDENCIES ${CONTAINERS_TESTS}
    )
endif()

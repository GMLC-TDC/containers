#
# Copyright © 2017-2018, Battelle Memorial Institute; Lawrence Livermore National
# Security, LLC; Alliance for Sustainable Energy, LLC All rights reserved. See LICENSE
# file and DISCLAIMER for more details.
#

set(
  container_headers
  AirLock.hpp
  SimpleQueue.hpp
  BlockingQueue.hpp
  BlockingPriorityQueue.hpp
  MapTraits.hpp
  MappedVector.hpp
  DualMappedVector.hpp
  MappedPointerVector.hpp
  DualMappedPointerVector.hpp
  CircularBuffer.hpp
  StackBuffer.hpp
  StableBlockVector.hpp
  StableBlockDeque.hpp
  BlockIterator.hpp
  optionalDefinition.hpp
)

set(
  container_sources
  empty.cpp
)


add_library(containers STATIC ${container_sources} ${container_headers})

target_link_libraries(containers containers_base)
target_include_directories(containers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


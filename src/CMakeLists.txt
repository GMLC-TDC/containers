#
# Copyright © 2017-2018, Battelle Memorial Institute; Lawrence Livermore National
# Security, LLC; Alliance for Sustainable Energy, LLC All rights reserved. See LICENSE
# file and DISCLAIMER for more details.
#

set(
  container_headers
  include/AirLock.hpp
  include/simpleQueue.hpp
  include/BlockingQueue.hpp
  include/BlockingPriorityQueue.hpp
  include/MapTraits.hpp
  include/MappedVector.hpp
  include/DualMappedVector.hpp
  include/MappedPointerVector.hpp
  include/DualMappedPointerVector.hpp
  include/CircularBuffer.hpp
  include/StackQueue.hpp
  include/StableBlockVector.hpp
  include/StableBlockDeque.hpp
  include/BlockIterator.hpp
)

set(
  container_sources
  include/CircularBuffer.cpp
  include/StackQueue.cpp
)


add_library(containers STATIC ${container_sources} ${container_headers})

target_link_libraries(containers containers_base)
target_include_directories(containers INTERFACE include)

# =========================================================================
# Program:   otb-bv
# Language:  C++
#
# Copyright (c) CESBIO. All rights reserved.
#
# See otb-bv-copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the above copyright notices for more information.
#
# =========================================================================
project(otb-bv)

cmake_minimum_required(VERSION 2.8)

find_package(OTB)
if(OTB_FOUND)
  include(${OTB_USE_FILE})
else(OTB_FOUND)
  message(FATAL_ERROR
          "OTB not found. Please set OTB_DIR")
endif(OTB_FOUND)

if(NOT OTBBV_INSTALL_BIN_DIR)
  set(OTBBV_INSTALL_BIN_DIR "bin")
endif()

option(BUILD_SHARED_LIBS "Build ITK with shared libraries." ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/library)

include(${CMAKE_ROOT}/Modules/Dart.cmake)

add_subdirectory(library)
add_subdirectory(applications)
add_subdirectory(testing)
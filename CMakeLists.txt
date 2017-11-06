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
cmake_minimum_required(VERSION 3.0)
project(OTBBioVars)

set(CMAKE_MODULE_PATH ${OTBBioVars_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

message("-- Using OTBBioVars. Your OTB will be under the Affero GNU GPL License or better.")

if(NOT OTB_SOURCE_DIR)
  find_package(GSL)
  if(GSL_FOUND)
    message("-- Using GSL. Your OTB will be under the GNU GPL License or better.")
  else(GSL_FOUND)
    message(FATAL_ERROR "GSL not found but needed for spline interpolation.")
  endif(GSL_FOUND)
  include_directories(${GSL_INCLUDE_DIRS})

  find_package(OTB REQUIRED)
  list(APPEND CMAKE_MODULE_PATH ${OTB_CMAKE_DIR})
  include(OTBModuleExternal)
else()
  otb_module_impl()
endif()







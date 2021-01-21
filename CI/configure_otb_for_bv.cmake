#
# Copyright (C) 2005-2020 Centre National d'Etudes Spatiales (CNES)
#
# This file is part of Orfeo Toolbox
#
#     https://www.orfeo-toolbox.org/
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This script is a prototype for the future CI, it may evolve rapidly in a near future
#This file set the following variable :
# * otb_build_project_option
# * otb_use_option
# * otb_wrap_option
# * CONFIGURE_OPTIONS

set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Setting debug mode")
set (CMAKE_GENERATOR "Ninja" CACHE STRING "Generator NINJA")
set (CMAKE_INSTALL_PREFIX "/opt/otb" CACHE STRING "Install in /opt/otb")

# use, i.e. don't skip the full RPATH for the build tree
set (CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set (BUILD_COOKBOOK OFF CACHE BOOL "")
set (BUILD_EXAMPLES OFF CACHE BOOL "")
set (BUILD_SHARED_LIBS ON CACHE BOOL "")
set (BUILD_TESTING ON CACHE BOOL "")

set (OTB_USE_6S ON CACHE BOOL "")
set (OTB_USE_CURL ON CACHE BOOL "")
set (OTB_USE_GLEW OFF CACHE BOOL "")
set (OTB_USE_GLFW OFF CACHE BOOL "")
set (OTB_USE_GLUT OFF CACHE BOOL "")
set (OTB_USE_GSL ON CACHE BOOL "")
set (OTB_USE_LIBKML OFF CACHE BOOL "")
set (OTB_USE_LIBSVM ON CACHE BOOL "")
set (OTB_USE_MPI OFF CACHE BOOL "")
set (OTB_USE_MUPARSER ON CACHE BOOL "")
set (OTB_USE_MUPARSERX OFF CACHE BOOL "")
set (OTB_USE_OPENCV ON CACHE BOOL "")
set (OTB_USE_OPENGL ON CACHE BOOL "")
set (OTB_USE_OPENMP OFF CACHE BOOL "")
set (OTB_USE_QT OFF CACHE BOOL "")
set (OTB_USE_QWT OFF CACHE BOOL "")
set (OTB_USE_SHARK OFF CACHE BOOL "")
set (OTB_USE_SIFTFAST ON CACHE BOOL "")
set (OTB_USE_SPTW OFF CACHE BOOL "")
set (OTB_USE_SSE_FLAGS ON CACHE BOOL "")
set (Module_OTBTemporalGapFilling ON CACHE BOOL "")
set (Module_OTBPhenology ON CACHE BOOL "")
set (OTB_WRAP_PYTHON ON CACHE BOOL "")
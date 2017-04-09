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
project(OTBBioVars)

set(CMAKE_MODULE_PATH ${OTBBioVars_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
# find_package(GSL)
# if(GSL_FOUND)
#   message("-- Using GSL. Your OTB will be under the GNU GPL License or better.")
# else(GSL_FOUND)
#   message(FATAL_ERROR "GSL not found but needed for spline interpolation.")
# endif(GSL_FOUND)

# include_directories(${GSL_INCLUDE_DIRS})

message("-- Using OTBBioVars. Your OTB will be under the Affero GNU GPL License or better.")

otb_module_impl()






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

include_directories(../library/)

OTB_CREATE_APPLICATION(NAME           BVInputVariableGeneration
                       SOURCES        otbBVInputVariableGeneration.cxx
                       LINK_LIBRARIES OTBIO;OTBCommon)

OTB_CREATE_APPLICATION(NAME           ProSailSimulator
                       SOURCES        otbProSailSimulator.cxx
                       LINK_LIBRARIES OTBIO;OTBCommon;OTBSimulation)                     

OTB_CREATE_APPLICATION(NAME           InverseModelLearning
                       SOURCES        otbInverseModelLearning.cxx
                       LINK_LIBRARIES OTBIO;OTBCommon;OTBLearning)

OTB_CREATE_APPLICATION(NAME           BVInversion
                       SOURCES        otbBVInversion.cxx
                       LINK_LIBRARIES OTBIO;OTBCommon;OTBLearning)                     
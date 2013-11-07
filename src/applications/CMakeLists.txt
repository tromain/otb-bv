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

PROJECT(otb-bv)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FIND_PACKAGE(OTB)
IF(OTB_FOUND)
  INCLUDE(${OTB_USE_FILE})
ELSE(OTB_FOUND)
  MESSAGE(FATAL_ERROR
          "OTB not found. Please set OTB_DIR")
ENDIF(OTB_FOUND)

INCLUDE_DIRECTORIES(../library/)

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
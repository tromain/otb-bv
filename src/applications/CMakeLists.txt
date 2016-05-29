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

OTB_CREATE_APPLICATION(NAME           BVInputVariableGeneration
  SOURCES        otbBVInputVariableGeneration.cxx
  LINK_LIBRARIES ${OTB_LIBRARIES};OTBBVUtil)

OTB_CREATE_APPLICATION(NAME           ProSailSimulator
  SOURCES        otbProSailSimulator.cxx
  LINK_LIBRARIES ${OTB_LIBRARIES};OTBBVUtil)                     

OTB_CREATE_APPLICATION(NAME           InverseModelLearning
  SOURCES        otbInverseModelLearning.cxx
  LINK_LIBRARIES ${OTB_LIBRARIES};OTBBVUtil;${GSL_LIBRARY};${GSL_CBLAS_LIBRARY})

OTB_CREATE_APPLICATION(NAME           BVInversion
  SOURCES        otbBVInversion.cxx
  LINK_LIBRARIES ${OTB_LIBRARIES};OTBBVUtil;${GSL_LIBRARY};${GSL_CBLAS_LIBRARY})

OTB_CREATE_APPLICATION(NAME           BVImageInversion
  SOURCES        otbBVImageInversion.cxx
  LINK_LIBRARIES ${OTB_LIBRARIES};OTBBVUtil;${GSL_LIBRARY};${GSL_CBLAS_LIBRARY})

OTB_CREATE_APPLICATION(NAME           ProfileReprocessing
  SOURCES        otbProfileReprocessing.cxx
  LINK_LIBRARIES ${OTB_LIBRARIES};OTBTemporalGapFilling;OTBBVUtil;${PHENOTB_LIBRARY};${GSL_LIBRARY};${GSL_CBLAS_LIBRARY})

add_executable(testSimus testSimus.cxx)
target_link_libraries(testSimus ${OTB_LIBRARIES} OTBBVUtil ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})

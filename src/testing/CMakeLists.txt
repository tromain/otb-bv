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

set(OTBBV_TESTS ${otb-bv_BINARY_DIR}/testing/otbBVTests)

add_test(bvProSailSimulatorFunctor ${OTBBV_TESTS}
  bvProSailSimulatorFunctor ${otb-bv_SOURCE_DIR}/../data/formosat2_4b.rsr)

add_test(bvMultiLinearFitting ${OTBBV_TESTS}
  bvMultiLinearFitting)       

add_test(bvMultiLinearFittingConversions ${OTBBV_TESTS}
  bvMultiLinearFittingConversions)

add_test(bvMultiTemporalInversion ${OTBBV_TESTS}
  bvMultiTemporalInversion ${otb-bv_SOURCE_DIR}/../data/formosat2_4b.rsr)

set(BVTests_SRCS
  otbBVTests.cxx
  bvProSailSimulatorFunctor.cxx
  bvMultiLinearFitting.cxx
  bvMultiTemporalInversion.cxx)

add_executable(otbBVTests ${BVTests_SRCS})
target_link_libraries(otbBVTests OTBCommon OTBIO OTBTesting OTBSimulation gsl gslcblas)



### Test the applications
add_test(appBvGenInputVars ${OTB_DIR}/bin/otbApplicationLauncherCommandLine BVInputVariableGeneration ${otb-bv_BINARY_DIR}/applications/ 
  -samples 200 
  -out ${TEMP}/appBvGenInputVarssamples.txt)

add_test(appBvProSailSim ${OTB_DIR}/bin/otbApplicationLauncherCommandLine ProSailSimulator ${otb-bv_BINARY_DIR}/applications/ 
  -bvfile ${otb-bv_SOURCE_DIR}/../data/vars.txt
  -soilfile dummy
  -rsrfile ${otb-bv_SOURCE_DIR}/../data/formosat2_4b.rsr
  -out ${TEMP}/appProSailSimus.txt
  -solarzenith 33.469
  -sensorzenith 20.071
  -azimuth 169.0)

add_test(appBvInvModLear ${OTB_DIR}/bin/otbApplicationLauncherCommandLine
  InverseModelLearning ${otb-bv_BINARY_DIR}/applications/
  -training ${otb-bv_SOURCE_DIR}/../data/train.txt
  -out ${TEMP}/appInvMod.txt)

add_test(appBvInversion ${OTB_DIR}/bin/otbApplicationLauncherCommandLine
  BVInversion ${otb-bv_BINARY_DIR}/applications/
  -reflectances ${otb-bv_SOURCE_DIR}/../data/refls.txt
  -model ${otb-bv_SOURCE_DIR}/../data/model.txt
  -out ${TEMP}/appInvMod.txt)
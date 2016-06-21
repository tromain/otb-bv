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
otb_module_test()

set(${otb-module}Tests
  otbBVTests.cxx
  bvProSailSimulatorFunctor.cxx
  bvMultiLinearFitting.cxx
  bvMultiTemporalInversion.cxx
  bvVariableGenerationTests.cxx)

set(OTBBioVars_TEST_LINK_LIBS ${otb-module}
  ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY} ${OTBPhenology_LIBRARIES} ${OTBSimulation_LIBRARIES}
  ${OTBSupervised_LIBRARIES}
  )

add_executable(otbBioVarsTests ${${otb-module}Tests})
target_link_libraries(otbBioVarsTests ${${otb-module}-Test_LIBRARIES} ${OTBBioVars_TEST_LINK_LIBS})
otb_module_target_label(otbBioVarsTests)


### Test the applications
otb_test_application(NAME appBvGenInputVars
  APP BVInputVariableGeneration
  OPTIONS
  -samples 2000 
  -out ${TEMP}/appBvGenInputVarssamples.txt)

otb_test_application(NAME appBvProSailSim
  APP ProSailSimulator
  OPTIONS
  -bvfile ${OTBBioVars_SOURCE_DIR}/data/appBvGenInputVarssamples.txt
  -rsrfile ${OTBBioVars_SOURCE_DIR}/data/formosat2_4b.rsr
  -out ${TEMP}/appProSailSimus.txt
  -solarzenith 33.469
  -sensorzenith 20.071
  -azimuth 169.0)

otb_test_application(NAME appBvInvModLear
  APP InverseModelLearning
  OPTIONS
  -training ${OTBBioVars_SOURCE_DIR}/data/train-refls-lai.txt
  -regression nn
  -out ${TEMP}/appInvMod.txt
  -errest ${TEMP}/appInvModErrEst.txt)

otb_test_application(NAME appBvInversion
  APP BVInversion
  OPTIONS
  -reflectances ${OTBBioVars_SOURCE_DIR}/data/relfs_sim.txt
  -model ${OTBBioVars_SOURCE_DIR}/data/appInvMod.txt
  -out ${TEMP}/appInvLai.txt)

### Test functions and classes
otb_add_test(NAME bvProSailSimulatorFunctor 
  COMMAND otbBioVarsTests bvProSailSimulatorFunctor ${OTBBioVars_SOURCE_DIR}/data/formosat2_4b.rsr)

otb_add_test(NAME bvCorrelateWithLAI 
  COMMAND otbBioVarsTests bvCorrelateWithLAI)

otb_add_test(NAME bvMultiLinearFitting 
  COMMAND otbBioVarsTests bvMultiLinearFitting)       

otb_add_test(NAME bvMultiLinearFittingConversions 
  COMMAND otbBioVarsTests bvMultiLinearFittingConversions)

otb_add_test(NAME bvMultiTemporalInversion 
  COMMAND otbBioVarsTests bvMultiTemporalInversion ${OTBBioVars_SOURCE_DIR}/data/formosat2_4b.rsr
  33.469
  20.071
  169.0
  ${OTBBioVars_SOURCE_DIR}/data/appInvMod.txt
  ${OTBBioVars_SOURCE_DIR}/data/appInvModErrEst.txt
  ${TEMP}/bvMulTeInvRes.txt)

otb_add_test(NAME bvMultiTemporalInversionRadius 
  COMMAND otbBioVarsTests bvMultiTemporalInversion ${OTBBioVars_SOURCE_DIR}/data/formosat2_4b.rsr
  33.469
  20.071
  169.0
  ${OTBBioVars_SOURCE_DIR}/data/appInvMod.txt
  ${OTBBioVars_SOURCE_DIR}/data/appInvModErrEst.txt
  ${TEMP}/bvMulTeInvResRadius.txt
  5
  0)

otb_add_test(NAME bvMultiTemporalInversionFile 
  COMMAND otbBioVarsTests bvMultiTemporalInversionFromFile ${OTBBioVars_SOURCE_DIR}/data/bvMulTeInvRes.txt
  1
  1
  10e-5)

otb_add_test(NAME bvLaiLogNdvi 
  COMMAND otbBioVarsTests bvLaiLogNdvi)

add_executable(testSimus testSimus.cxx)
target_link_libraries(testSimus ${OTB_LIBRARIES} ${OTBBioVars_LIBRARIES} ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})

add_executable(an-par-sweep an-par-sweep.cxx)
#target_link_libraries(an-par-sweep )

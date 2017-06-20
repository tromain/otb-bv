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
  bvVariableGenerationTests.cxx
  bvValidityDomainTests.cxx  )

set(OTBBioVars_TEST_LINK_LIBS ${otb-module}
  ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY} ${OTBPhenology_LIBRARIES} ${OTBSimulation_LIBRARIES}
  ${OTBSupervised_LIBRARIES} ${OTBTemporalGapFilling_LIBRARIES} ${OTBBoost_LIBRARIES} ${GSL_LIBRARIES}
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

otb_test_application(NAME appBvProSailSimCovar
  APP ProSailSimulator
  OPTIONS
  -bvfile ${OTBBioVars_SOURCE_DIR}/data/appBvGenInputVarssamples.txt
  -rsrfile ${OTBBioVars_SOURCE_DIR}/data/formosat2_4b.rsr
  -out ${TEMP}/appProSailSimusCovar.txt
  -solarzenith 33.469
  -sensorzenith 20.071
  -azimuth 169.0
  -covariance ${TEMP}/appProSailCovarMatrix.txt
  VALID   --compare-ascii ${EPSILON_3}
  ${OTBBioVars_SOURCE_DIR}/data/appProSailCovarMatrix.txt
  ${TEMP}/appProSailCovarMatrix.txt  )

otb_test_application(NAME appBvProSailSimSoil
  APP ProSailSimulator
  OPTIONS
  -bvfile ${OTBBioVars_SOURCE_DIR}/data/appBvGenInputVarssamples.txt
  -rsrfile ${OTBBioVars_SOURCE_DIR}/data/formosat2_4b.rsr
  -out ${TEMP}/appProSailSimusSoil.txt
  -solarzenith 33.469
  -sensorzenith 20.071
  -azimuth 169.0
  -soilfile ${INPUTDATA}/Radiometry/Soils/sols_avignon_1990.txt
  -wlfactor 1000
  )

otb_test_application(NAME appBvProSailSimSoilSent
  APP ProSailSimulator
  OPTIONS
  -bvfile ${OTBBioVars_SOURCE_DIR}/data/appBvGenInputVarssamples.txt
  -rsrfile ${OTBBioVars_SOURCE_DIR}/data/sentinel2_10m.rsr
  -out ${TEMP}/appProSailSimusSoilSent.txt
  -solarzenith 33.469
  -sensorzenith 20.071
  -azimuth 169.0
  -soilfile ${OTBBioVars_SOURCE_DIR}/data/Sentinel2_Soils.csv
  -wlfactor 1
  )

otb_test_application(NAME appBvInvModLear
  APP BVInverseModelLearning
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

otb_test_application(NAME appBvInversionCovar
  APP BVInversion
  OPTIONS
  -reflectances ${OTBBioVars_SOURCE_DIR}/data/relfs_sim.txt
  -model ${OTBBioVars_SOURCE_DIR}/data/appInvMod.txt
  -out ${TEMP}/appInvLaiCovar.txt
  -covariance ${OTBBioVars_SOURCE_DIR}/data/appProSailCovarMatrix.txt)

### Test functions and classes
otb_add_test(NAME bvProSailSimulatorFunctorPixel 
  COMMAND otbBioVarsTests bvProSailSimulatorFunctorPixel ${OTBBioVars_SOURCE_DIR}/data/formosat2_4b.rsr)

otb_add_test(NAME bvProSailSimulatorFunctor
  COMMAND otbBioVarsTests bvProSailSimulatorFunctor 
  ${OTBBioVars_SOURCE_DIR}/data/sentinel2_10m_20m_noblue_nob8.rsr   #RSRFile 
  ${OTBBioVars_SOURCE_DIR}/data/Sentinel2_Soils.csv  #soilFile 
  1  #wlfactor 
  ${OTBBioVars_SOURCE_DIR}/data/Sentinel2_Laws.txt #bvVariableFile 
  ${OTBBioVars_SOURCE_DIR}/data/NNETInOutTrue.csv #simulationFile 
  8 #nbBands
  100 #nbsamples
  )

otb_add_test(NAME bvComputeCovarAndMean
  COMMAND otbBioVarsTests bvComputeCovarAndMean
  ${TEMP}/bvComputeCovarAndMeanCovarFile.txt  )

otb_add_test(NAME bvReadCovarianceFile
  COMMAND otbBioVarsTests bvReadCovarianceFile
  ${OTBBioVars_SOURCE_DIR}/data/bvCovarFile.txt)

otb_add_test(NAME bvIsSampleValid1D
  COMMAND otbBioVarsTests bvIsSampleValid1D)

otb_add_test(NAME bvIsSampleValid2D
  COMMAND otbBioVarsTests bvIsSampleValid2D)

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

add_executable(testSimus testSimus.cxx)
target_link_libraries(testSimus ${OTB_LIBRARIES} ${OTBBioVars_LIBRARIES} ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})

add_executable(an-par-sweep an-par-sweep.cxx)
#target_link_libraries(an-par-sweep )

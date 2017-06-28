set(DOCUMENTATION "Biophysical variable estimation from optical imagery.")

# define the dependencies of the include module and the tests
otb_module(OTBBioVars
  DEPENDS
  OTBApplicationEngine
  OTBBoost
  OTBCommon
  OTBITK
  OTBPhenology
  OTBTemporalGapFilling 
  OTBSimulation
  OTBSupervised
  TEST_DEPENDS
  OTBTestKernel
  OTBCommandLine
  DESCRIPTION
  "${DOCUMENTATION}"
  )

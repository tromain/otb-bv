set(DOCUMENTATION "Biophysical variable estimation from optical imagery.")

otb_module_requires_cxx11()

# define the dependencies of the include module and the tests
otb_module(OTBBioVars
  DEPENDS
  OTBApplicationEngine
  OTBBoost
  OTBCommon
  OTBITK
  OTBPhenology
  OTBSimulation
  OTBSupervised
  TEST_DEPENDS
  OTBTestKernel
  OTBCommandLine
  DESCRIPTION
  "${DOCUMENTATION}"
  )

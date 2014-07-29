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
         bvProSailSimulatorFunctor)

add_test(bvMultiLinearFitting ${OTBBV_TESTS}
         bvMultiLinearFitting)       

set(BVTests_SRCS
  otbBVTests.cxx
  bvProSailSimulatorFunctor.cxx
  bvMultiLinearFitting.cxx)

add_executable(otbBVTests ${BVTests_SRCS})
target_link_libraries(otbBVTests OTBCommon OTBIO OTBTesting OTBSimulation gsl gslcblas)

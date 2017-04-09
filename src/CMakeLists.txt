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
set(${otb-module}_SRC
  otbBVUtil.cxx
  )

add_library(${otb-module} ${${otb-module}_SRC})
target_link_libraries(${otb-module} ${OTBCommon_LIBRARIES} ${OTBITK_LIBRARIES} ${OTBTemporalGapFilling_LIBRARIES} ${OTBBoost_LIBRARIES} ${GSL_LIBRARIES})
otb_module_target(${otb-module})


#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================
#   Program:   otb-bv
#   Language:  python
#
#   Copyright (c) CESBIO. All rights reserved.
#
#   See otb-bv-copyright.txt for details.
#
#   This software is distributed WITHOUT ANY WARRANTY; without even
#   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#   PURPOSE.  See the above copyright notices for more information.
#
# =========================================================================

rsr_dir = "/home/inglada/Dev/otb-bv/data/"
fsat_rsr = rsr_dir+"formosat2.rsr"

fsat_data = ["formosat2", fsat_rsr]
fsat_126 = {'doy': 126, 'to': 20.071, 'po': 307.601, 'ts':33.469, 'ps': 138.026}
fsat_data.append(fsat_126)

fsat_177 = {'doy': 177, 'to': 19.205, 'po': 290.135, 'ts': 27.993, 'ps': 127.855}
fsat_data.append(fsat_177)

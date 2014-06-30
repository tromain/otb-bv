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

import otbApplication as otb
import bv_net as bv

"""
- 1 seul fichier de variables pour toutes les simulations
- 1 fichier de simulation par configuration de prise de vue
- inversion de chaque jeu de données simulées
- séparer les mesures sol par configuration et par culture
- plots
  - scatter de chaque jeu de données simulées
  - pour chaque date
    - scatter avec chaque culture en une couleur différente
  - regrouper par sat
  - regrouper tout
"""

working_dir = "/tmp/"
rsr_dir = "/home/inglada/Dev/otb-bv/data/"
input_var_file = working_dir+"input-vars"
nbSamples = 2000
fsat_rsr = rsr_dir+"formosat2.rsr"


bv.generateInputBVDistribution(input_var_file, nbSamples)


fsat_data = [fsat_rsr]
fsat_126 = {'doy': 126, 'to': 20.071, 'po': 307.601, 'ts':33.469, 'ps': 138.026}


simuPars = {}
simuPars['rsrFile'] = cfg.simulation.rsrFile
simuPars['outputFile'] = cfg.simulation.outputFile
simuPars['solarZenithAngle'] = cfg.simulation.solarZenithAngle
simuPars['sensorZenithAngle'] = cfg.simulation.sensorZenithAngle
simuPars['solarSensorAzimuth'] = cfg.simulation.solarSensorAzimuth

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
input_var_file_test = working_dir+"input-vars-test"
nbSamples_train = 200
nbSamples_test = 200
fsat_rsr = rsr_dir+"formosat2.rsr"


bv.generateInputBVDistribution(input_var_file, nbSamples_train)
bv.generateInputBVDistribution(input_var_file_test, nbSamples_test)

simus_list = []

fsat_data = ["formosat2", fsat_rsr]
fsat_126 = {'doy': 126, 'to': 20.071, 'po': 307.601, 'ts':33.469, 'ps': 138.026}
fsat_data.append(fsat_126)


simus_list.append(fsat_data)


for sat in simus_list:
    sat_name = sat[0]
    rsr_file = sat[1]
    for acqu in sat[2:]:
        reflectance_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_reflectances"
        training_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_training"
        reflectance_file_test = working_dir+sat_name+"_"+str(acqu['doy'])+"_reflectances_test"
        training_file_test = working_dir+sat_name+"_"+str(acqu['doy'])+"_training_test"
        normalization_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_normalization"
        inversion_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_inversion"
        model_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_model"
        simuPars = {}
        simuPars['rsrFile'] = rsr_file
        simuPars['outputFile'] = reflectance_file
        simuPars['solarZenithAngle'] = acqu['ts']
        simuPars['sensorZenithAngle'] = acqu['to']
        simuPars['solarSensorAzimuth'] = acqu['ps']-acqu['po']
        simuPars['soilFile'] = "whatever"
        bv.generateTrainingData(input_var_file, simuPars, training_file, bv.bvindex["MLAI"])
        simuPars['outputFile'] = reflectance_file_test
        bv.generateTrainingData(input_var_file_test, simuPars, training_file_test, bv.bvindex["MLAI"])
        bv.learnBVModel(training_file, model_file, normalization_file)
        bv.invertBV(reflectance_file_test, model_file, normalization_file, inversion_file)
        

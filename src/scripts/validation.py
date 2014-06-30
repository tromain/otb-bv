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

import string
import otbApplication as otb
import bv_net as bv
from formosat_data import *

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

bv.generateInputBVDistribution(input_var_file, nbSamples_train)
bv.generateInputBVDistribution(input_var_file_test, nbSamples_test)

simus_list = []
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
        validation_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_validation"
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
        with open(inversion_file, 'r') as ivf:
            with open(training_file_test, 'r') as tft:
                with open(validation_file, 'w') as vaf:
                    for(ivline, tftline) in zip(ivf.readlines(), tft.readlines()):
                        outline = string.split(ivline)[0]+" "+string.split(tftline)[0]+"\n"
                        vaf.write(outline)
                        

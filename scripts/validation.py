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
import os
import sys
from config import Config
import otbApplication as otb
import bv_net as bv
from formosat_data_noblue import *
from formosat_data import *
from spot4_data import *
from spot5_data import *
from landsat_data import *
from landsat2015_data import *
from s2_10m_ukr_data import *
from s2_10m_noblue_ukr_data import *


config_file = file(sys.argv[1])
cfg = Config(config_file)

wdpath = cfg.paths.wdpath
varName = cfg.paths.varName
working_dir = wdpath+"/"+varName+"/"
rsr_dir = cfg.paths.rsr_dir
soil_file = cfg.paths.soil_file
useSoilDB = bool(cfg.simulation.useSoilDB=="yes")
soil_index_max = cfg.simulation.soil_index_max
soil_wl_factor = float(cfg.simulation.soil_wl_factor)
input_var_file = working_dir+cfg.paths.input_var_file
input_var_file_test = working_dir+cfg.paths.input_var_file_test
nbSamples_train = int(cfg.simulation.nbSamples_train)
nbSamples_test = int(cfg.simulation.nbSamples_test)
minlai = float(cfg.simulation.minlai)
maxlai = float(cfg.simulation.maxlai)
modlai = float(cfg.simulation.modlai)
stdlai = float(cfg.simulation.stdlai)
distlai = cfg.simulation.distlai
minbs = float(cfg.simulation.minbs)
maxbs = float(cfg.simulation.maxbs)
modbs = float(cfg.simulation.modbs)
stdbs = float(cfg.simulation.stdbs)
noise_std = float(cfg.simulation.noise_std)
simulate = bool(str(cfg.simulation.simulate)=="yes")
useVI = bool(str(cfg.simulation.useVI)=="yes")
nthreads = int(cfg.simulation.nthreads)
bestof = int(cfg.inversion.bestof)
regressor = cfg.inversion.regressor
formosat = bool(str(cfg.sensors.formosat)=="yes")
formosat_noblue = bool(str(cfg.sensors.formosat_noblue)=="yes")
spot4 = bool(str(cfg.sensors.spot4)=="yes")
landsat2013 = bool(str(cfg.sensors.landsat2013)=="yes")
spot5 = bool(str(cfg.sensors.spot5)=="yes")
landsat2015 = bool(str(cfg.sensors.landsat2015)=="yes")
s2_10m_ukr = bool(str(cfg.sensors.s2_10m_ukr)=="yes")
s2_10m_noblue_ukr = bool(str(cfg.sensors.s2_10m_noblue_ukr)=="yes")

print "Working dir = ", working_dir

if(useSoilDB) :
    print "Using soil DB "+soil_file+" from 1 to "+str(soil_index_max)+" with wlfactor "+str(soil_wl_factor)

d = os.path.dirname(working_dir)
if not os.path.exists(d):
    os.makedirs(d)

if simulate :
    varPars = {}    
    varPars['minlai'] = minlai
    varPars['maxlai'] = maxlai
    varPars['modlai'] = modlai
    varPars['stdlai'] = stdlai
    varPars['distlai'] = distlai
    varPars['minbs'] = minbs
    varPars['maxbs'] = maxbs
    varPars['modbs'] = modbs
    varPars['stdbs'] = stdbs
    bv.generateInputBVDistribution(input_var_file, nbSamples_train, varPars)
    bv.generateInputBVDistribution(input_var_file_test, nbSamples_test, varPars)

simus_list = []
if formosat :
    simus_list.append(fsat_data)
if formosat_noblue :
    simus_list.append(fsatnb_data)
if spot4 :
    simus_list.append(spot4_data)
if landsat2013 :
    simus_list.append(lsat_data)
if spot5 :
    simus_list.append(spot5_data)
if landsat2015 :
    simus_list.append(lsat2015_data)
if s2_10m_ukr:
    simus_list.append(s2_10m_ukr_data)
if s2_10m_noblue_ukr:
    simus_list.append(s2_10m_noblue_ukr_data)

for sat in simus_list:
    sat_name = sat[0]
    rsr_file = sat[1]
    red_index = 0
    nir_index = 0
    if sat_name == "formosat2" and useVI:
        red_index = 3
        nir_index = 4
    if sat_name == "formosat2noblue" and useVI:
        red_index = 2
        nir_index = 3
    if (sat_name == "spot4" or sat_name == "spot5") and useVI:
        red_index = 2
        nir_index = 3
    if (sat_name == "landsat8" or sat_name == "landsat82015") and useVI:
        red_index = 2
        nir_index = 3
    if(sat_name == "s2_10m_ukr") and useVI:
        red_index = 3
        nir_index = 4
    if(sat_name == "s2_10m_noblue_ukr") and useVI:
        red_index = 2
        nir_index = 3

    print useVI, red_index, nir_index
    for acqu in sat[2:]:
        print "-------"+sat_name+"_"+str(acqu['doy'])+"_"+regressor
        reflectance_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_reflectances"
        training_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_training"
        reflectance_file_test = working_dir+sat_name+"_"+str(acqu['doy'])+"_reflectances_test"
        training_file_test = working_dir+sat_name+"_"+str(acqu['doy'])+"_training_test"
        normalization_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_normalization"
        inversion_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_inversion_"+regressor
        model_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_model_"+regressor
        validation_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_validation_"+regressor
        reflectances_gt_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_reflectances_gt"
        inversion_gt_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_inversion_gt_"+regressor
        validation_gt_file = working_dir+sat_name+"_"+str(acqu['doy'])+"_validation_gt_"+regressor
        simuPars = {}
        simuPars['rsrFile'] = rsr_dir+"/"+rsr_file
        simuPars['outputFile'] = reflectance_file
        simuPars['solarZenithAngle'] = acqu['ts']
        simuPars['sensorZenithAngle'] = acqu['to']
        simuPars['solarSensorAzimuth'] = acqu['ps']-acqu['po']
        simuPars['soilFile'] = "whatever"
        simuPars['noisestd'] = noise_std
        simuPars['soilfile'] = soil_file
        simuPars['soilindexmax'] = soil_index_max
        simuPars['soilwlfactor'] = soil_wl_factor
        simuPars['useSoilDB'] = useSoilDB
        print "\tSimulation training"
        bv.generateTrainingData(input_var_file, simuPars, training_file, bv.bvindex[varName], simulate, False, red_index, nir_index, nthreads)
        simuPars['outputFile'] = reflectance_file_test
        print "\tSimulation testing data"
        bv.generateTrainingData(input_var_file_test, simuPars, training_file_test, bv.bvindex[varName], simulate, False, red_index, nir_index, nthreads)
        print "\tLearning model"
        bv.learnBVModel(training_file, model_file, regressor, normalization_file, bestof)
        print "\tInversion for test data"
        bv.invertBV(reflectance_file_test, model_file, normalization_file, inversion_file, True, red_index, nir_index)
        with open(inversion_file, 'r') as ivf:
            with open(training_file_test, 'r') as tft:
                with open(validation_file, 'w') as vaf:
                    for(ivline, tftline) in zip(ivf.readlines(), tft.readlines()):
                        outline = string.split(ivline)[0]+" "+string.split(tftline)[0]+"\n"
                        vaf.write(outline)
        var_values_gt = []
        var_values_bvnet = []
        with open(reflectances_gt_file, 'w') as rfgtf:
            for gt_case in acqu['gt']:
                for refl in gt_case['refls']:
                    rfgtf.write(str(refl/1000.0)+" ")
                rfgtf.write("\n")
                var_values_gt.append(gt_case[bv.bv_val_names[varName][0]])
                var_values_bvnet.append(gt_case[bv.bv_val_names[varName][1]])
        if useVI:
            bv.addVI(reflectances_gt_file, red_index, nir_index)
        print "\tInversion for validation data"
        bv.invertBV(reflectances_gt_file, model_file, normalization_file, inversion_gt_file)
        with open(inversion_gt_file, 'r') as ivgtf:
            with open(validation_gt_file, 'w') as vgtf:
                for (ival, gtval, bvnetval) in zip(ivgtf.readlines(),var_values_gt,var_values_bvnet):
                    vgtf.write(str(gtval)+" "+str(bvnetval)+" "+str(ival))
                
                        

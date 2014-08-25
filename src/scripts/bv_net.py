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

import sys
import os
import string
from config import Config
import otbApplication as otb

# The indices here have to be coherent with the order used in the bv file (and the definition of the vars in otbBVTypes.h
bvindex = {"MLAI": 0, "ALA": 1, "CrownCover": 2, "HsD": 3, "N": 4, "Cab": 5, "Car": 6, "Cdm": 7, "CwRel": 4, "Cbp": 9, "Bs": 10, "FAPAR": 11, "FCOVER": 12}

def parseConfigFile(cfg):
    distFileName = cfg.bvDistribution.fileName
    nSamples = cfg.bvDistribution.samples
    simuPars = {}
    simuPars['soilFile'] = cfg.simulation.soilFile
    simuPars['rsrFile'] = cfg.simulation.rsrFile
    simuPars['outputFile'] = cfg.simulation.outputFile
    simuPars['solarZenithAngle'] = cfg.simulation.solarZenithAngle
    simuPars['sensorZenithAngle'] = cfg.simulation.sensorZenithAngle
    simuPars['solarSensorAzimuth'] = cfg.simulation.solarSensorAzimuth
    trainingFile = cfg.training.fileName
    modelFile = cfg.learning.outputFileName
    return (distFileName, nSamples, simuPars, trainingFile, modelFile)

def generateInputBVDistribution(bvFile, nSamples):
    app = otb.Registry.CreateApplication("BVInputVariableGeneration")
    app.SetParameterInt("samples", nSamples)
    app.SetParameterString("out", bvFile)
    app.ExecuteAndWriteOutput()

def generateTrainingData(bvFile, simuPars, trainingFile, bvidx, add_angles=False):
    app = otb.Registry.CreateApplication("ProSailSimulator")
    app.SetParameterString("bvfile", bvFile)
    app.SetParameterString("soilfile", simuPars['soilFile'])
    app.SetParameterString("rsrfile", simuPars['rsrFile'])
    app.SetParameterString("out", simuPars['outputFile'])
    app.SetParameterFloat("solarzenith", simuPars['solarZenithAngle'])
    app.SetParameterFloat("sensorzenith", simuPars['sensorZenithAngle'])
    app.SetParameterFloat("azimuth", simuPars['solarSensorAzimuth'])
    app.ExecuteAndWriteOutput()
    #combine the bv samples, the angles and the simulated reflectances for variable inversion and produce the training file
    with open(trainingFile, 'w') as tf:
        with open(bvFile, 'r') as bvf:
            bvf.readline() #header line
            with open(simuPars['outputFile'], 'r') as rf:
                #the output line follows the format: outputvar inputvar1 inputvar2 ... inputvarN
                for (refline, bvline) in zip(rf.readlines(), bvf.readlines()):
                    outline = ""
                    if bvidx == bvindex["FCOVER"] :
                        string.split(refline[-1]), ' '
                    else if bvidx == bvindex["FAPAR"] : 
                        string.split(refline[-2]), ' '
                    else:
                        outline = string.split(bvline)[bvidx]
                    outline = outline+" "+string.join(string.split(refline[:-2]), ' ')
                    outline.rstrip()
                    if add_angles:
                        angles = `simuPars['solarZenithAngle']`+" "+`simuPars['sensorZenithAngle']`+" "+`simuPars['solarSensorAzimuth']`
                        outline += " "+angles+"\n"
                    else:
                        outline += "\n"
                    tf.write(outline)
                

def learnBVModel(trainingFile, outputFile, normalizationFile):
    app = otb.Registry.CreateApplication("InverseModelLearning")
    app.SetParameterString("training", trainingFile)
    app.SetParameterString("out", outputFile)
    app.SetParameterString("normalization", normalizationFile)
    app.ExecuteAndWriteOutput()

def invertBV(reflectanceFile, modelFile, normalizationFile, outputFile):
    app = otb.Registry.CreateApplication("BVInversion")
    app.SetParameterString("reflectances", reflectanceFile)
    app.SetParameterString("model", modelFile)
    app.SetParameterString("normalization", normalizationFile)
    app.SetParameterString("out", outputFile)
    app.ExecuteAndWriteOutput()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        sys.exit('Usage: %s <configurationFilePath>.cfg' % sys.argv[0])
    if not os.path.exists(sys.argv[1]):
        sys.exit('ERROR: Configuration File path %s was not found!'% sys.argv[1] )
    f = file ( sys.argv[1] )
    cfg=Config(f)

    (bvDistributionFileName, numberOfSamples, simulationParameters, trainingDataFileName, outputModelFileName) = parseConfigFile(cfg)
    generateInputBVDistribution(bvDistributionFileName, numberOfSamples)
    generateTrainingData(bvDistributionFileName, simulationParameters, trainingDataFileName, bvindex[cfg.training.invertBV])
    learnBVModel(trainingDataFileName, outputModelFileName)



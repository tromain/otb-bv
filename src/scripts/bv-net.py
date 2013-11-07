#!/usr/bin/env python
# =========================================================================
#   Program:   otb-bv
#   Language:  C++
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
from config import Config
import otbApplication as otb

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

def generateTrainingData(bvFile, simuPars, trainingFile):
    app = otb.Registry.CreateApplication("ProSailSimulator")
    app.SetParameterString("bvfile", bvFile)
    app.SetParameterString("soilfile", simuPars['soilFile'])
    app.SetParameterString("rsrfile", simuPars['rsrFile'])
    app.SetParameterString("out", simuPars['outputFile'])
    app.SetParameterFloat("solarzenith", simuPars['solarZenithAngle'])
    app.SetParameterFloat("sensorzenith", simuPars['sensorZenithAngle'])
    app.SetParameterFloat("azimuth", simuPars['solarSensorAzimuth'])
    app.ExecuteAndWriteOutput()

def learnBVModel(trainingFile, outputFile):
    pass

if __name__ == '__main__':
    if len(sys.argv) < 2:
        sys.exit('Usage: %s <configurationFilePath>.cfg' % sys.argv[0])
    if not os.path.exists(sys.argv[1]):
        sys.exit('ERROR: Configuration File path %s was not found!'% sys.argv[1] )
    f = file ( sys.argv[1] )
    cfg=Config(f)

    (bvDistributionFileName, numberOfSamples, simulationParameters, trainingDataFileName, outputModelFileName) = parseConfigFile(cfg)
    generateInputBVDistribution(bvDistributionFileName, numberOfSamples)
    generateTrainingData(bvDistributionFileName, simulationParameters, trainingDataFileName)
    learnBVModel(trainingDataFileName, outputModelFileName)



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
import string
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
    #TODO: convert the variable to invert to an index
    bvindex = 1
    #combine the bv samples, the angles and the simulated reflectances for variable inversion and produce the training file
    with open(trainingFile, 'w') as tf:
        with open(bvFile, 'r') as bvf:
            bvf.readline() #header line
            with open(simuPars['outputFile'], 'r') as rf:
                #the output line follows the format: outputvar inputvar1 inputvar2 ... inputvarN
                for (refline, bvline) in zip(rf.readlines(), bvf.readlines()):
                    outline = string.split(bvline)[bvindex]
                    angles = `simuPars['solarZenithAngle']`+" "+`simuPars['sensorZenithAngle']`+" "+`simuPars['solarSensorAzimuth']`
                    outline = outline+" "+string.join(string.split(refline[:-1]), " ")+" "+angles+"\n"
                    tf.write(outline)
                

def learnBVModel(trainingFile, outputFile):
    app = otb.Registry.CreateApplication("InverseModelLearning")
    app.SetParameterString("training", trainingFile)
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
    generateTrainingData(bvDistributionFileName, simulationParameters, trainingDataFileName)
    learnBVModel(trainingDataFileName, outputModelFileName)



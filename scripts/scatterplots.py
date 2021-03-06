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
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import NullFormatter

def parseDataFile(sampleFileName):
    variables = []
    samples = {}
    bounds = {}
    with open(sampleFileName, 'r') as sampleFile:
        for varname in string.split(sampleFile.readline()):
            variables.append(varname)
            samples[varname] = []
        for line in sampleFile.readlines():
            for (variable,value) in zip(variables,string.split(line)):
                samples[variable].append(float(value))
    for var in variables:
        bounds[var] = {'min' : 1000, 'max' : 0}
        for s in samples[var]:
            if s < bounds[var]['min']:
                bounds[var]['min'] = s
            if s > bounds[var]['max']:
                bounds[var]['max'] = s
    return (variables, samples, bounds)


def plotOrSaveFile(variables, samples, bounds, fileName=None, nXTicks=1, nYTicks=3):
    nbVariables = len(variables)
    for row in range(nbVariables):
        for col in range(nbVariables):
            ax = plt.subplot(nbVariables, nbVariables, row*nbVariables+col+1)
            x = samples[variables[col]]
            ax.set_xlim(bounds[variables[col]]['min'], bounds[variables[col]]['max'])
            if col == 0:
                ax.set_ylabel(variables[row], fontsize=5)
            if row == 0:
                ax.set_title(variables[col], fontsize=5)
            ax.set_xticklabels([])
            ax.set_yticklabels([])
            ax.yaxis.set_ticks_position('right')
            ax.xaxis.set_ticks_position('bottom')
            if row == (nbVariables-1) :
                ticks = [t/float(nXTicks)*(bounds[variables[col]]['max']-bounds[variables[col]]['min'])+bounds[variables[col]]['min'] for t in range(nXTicks+1)]
                ax.set_xticks(ticks)
                ax.set_xticklabels([("%.2f"%t) for t in ticks], fontsize=5)
            if row != col :
                y = samples[variables[row]]
                ax.set_ylim(bounds[variables[row]]['min'], bounds[variables[row]]['max'])
                ax.scatter(x, y, marker=".", s=1)
                if col == (nbVariables-1):
                    ticks = [t/float(nYTicks)*(bounds[variables[row]]['max']-bounds[variables[row]]['min'])+bounds[variables[row]]['min'] for t in range(nYTicks+1)]
                    ax.set_yticks(ticks)
                    ax.set_yticklabels([("%.2f"%t) for t in ticks], fontsize=5)
            else :
                ax.hist(x, bins=20)
    plt.subplots_adjust(wspace=0.5)
    if fileName != None:
        plt.savefig(fileName)
    else :
        plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        sys.exit("Usage: "+sys.argv[0]+" <sample file name>\n")
    if not os.path.exists(sys.argv[1]):
        sys.exit("ERROR: file "+sys.argv[1]+" was not found!")    
    (variables, samples, bounds) = parseDataFile(sys.argv[1])
    plotOrSaveFile(variables, samples, bounds, fileName=(None if len(sys.argv)==2 else sys.argv[2]))

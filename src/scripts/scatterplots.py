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
import string
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import NullFormatter


if len(sys.argv) != 2:
    print "Usage: "+sys.argv[0]+" <sample file name>\n"
    exit(0)

sampleFileName = sys.argv[1]

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

nbVariables = len(variables)

for row in range(nbVariables):
    for col in range(nbVariables):
        ax = plt.subplot(nbVariables, nbVariables, row*nbVariables+col+1)
        x = samples[variables[col]]
        ax.set_xlim(bounds[variables[col]]['min'], bounds[variables[col]]['max'])
        if col == 0:
            ax.set_ylabel(variables[row])
            ax.set_yticklabels([bounds[variables[row]]['min'], bounds[variables[row]]['max']])
        if row == nbVariables-1:
            ax.set_xlabel(variables[col])
            ax.set_xticklabels([bounds[variables[col]]['min'], bounds[variables[col]]['max']])
        if row != col :
            y = samples[variables[row]]
            ax.set_ylim(bounds[variables[row]]['min'], bounds[variables[row]]['max'])
            ax.scatter(x, y)
        else :
            ax.hist(x, bins=20)
        
plt.show()

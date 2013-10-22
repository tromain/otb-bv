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
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import NullFormatter


if len(sys.argv) < 2 or len(sys.argv) > 3:
    sys.exit("Usage: "+sys.argv[0]+" <sample file name>\n")

if not os.path.exists(sys.argv[1]):
    sys.exit("ERROR: Configuration File path "+sys.argv[1]+" was not found!")    

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
nXTicks = 2
nYTicks = 3

for row in range(nbVariables):
    for col in range(nbVariables):
        ax = plt.subplot(nbVariables, nbVariables, row*nbVariables+col+1)
        x = samples[variables[col]]
        ax.set_xlim(bounds[variables[col]]['min'], bounds[variables[col]]['max'])
        if col == 0:
             ax.set_ylabel(variables[row])
        if row == 0:
             ax.set_title(variables[col])

        ax.set_xticklabels([])
        ax.set_yticklabels([])
        
        ax.yaxis.set_ticks_position('right')
        ax.xaxis.set_ticks_position('bottom')
        if row == (nbVariables-1) :
            ticks = [t/float(nXTicks)*(bounds[variables[col]]['max']-bounds[variables[col]]['min'])+bounds[variables[col]]['min'] for t in range(nXTicks+1)]
            ax.set_xticks(ticks)
            ax.set_xticklabels([("%.2f"%t) for t in ticks])
                    
        if row != col :
            y = samples[variables[row]]
            ax.set_ylim(bounds[variables[row]]['min'], bounds[variables[row]]['max'])
            ax.scatter(x, y, marker=".")
            if col == (nbVariables-1):
                ticks = [t/float(nYTicks)*(bounds[variables[row]]['max']-bounds[variables[row]]['min'])+bounds[variables[row]]['min'] for t in range(nYTicks+1)]
                ax.set_yticks(ticks)
                ax.set_yticklabels([("%.2f"%t) for t in ticks])
        else :
            ax.hist(x, bins=20)

plt.subplots_adjust(wspace=0.5)
if len(sys.argv) == 3:
    plt.savefig(sys.argv[2])
plt.show()

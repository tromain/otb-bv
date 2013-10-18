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
        ax.set_yticklabels([])
        ax.set_xticklabels([])
        x = samples[variables[col]]
        ax.set_xlim(bounds[variables[col]]['min'], bounds[variables[col]]['max'])
        if col == 0:
            ax.set_ylabel(variables[row])
        if row == nbVariables-1:
            ax.set_xlabel(variables[col])            
        if row != col :
            y = samples[variables[row]]
            ax.set_ylim(bounds[variables[row]]['min'], bounds[variables[row]]['max'])
            ax.scatter(x, y)
        else :
            ax.hist(x, bins=20)
        
plt.show()
# nullfmt   = NullFormatter()         # no labels

# # # definitions for the axes
# left, width = 0.1, 0.65
# bottom, height = 0.1, 0.65
# bottom_h = left_h = left+width+0.02

# rect_scatter = [left, bottom, width, height]
# rect_histx = [left, bottom_h, width, 0.2]
# rect_histy = [left_h, bottom, 0.2, height]

# # start with a rectangular Figure
# plt.figure(1, figsize=(8,8))

# axScatter = plt.axes(rect_scatter)
# axHistx = plt.axes(rect_histx)
# axHisty = plt.axes(rect_histy)

# # no labels
# axHistx.xaxis.set_major_formatter(nullfmt)
# axHisty.yaxis.set_major_formatter(nullfmt)

# # the scatter plot:
# axScatter.scatter(x, y)

# # now determine nice limits by hand:
# binwidth = 0.25
# xymax = np.max( [np.max(np.fabs(x)), np.max(np.fabs(y))] )
# lim = ( int(xymax/binwidth) + 1) * binwidth

# axScatter.set_xlim( (-lim, lim) )
# axScatter.set_ylim( (-lim, lim) )

# bins = np.arange(-lim, lim + binwidth, binwidth)
# axHistx.hist(x, bins=bins)
# axHisty.hist(y, bins=bins, orientation='horizontal')

# axHistx.set_xlim( axScatter.get_xlim() )
# axHisty.set_ylim( axScatter.get_ylim() )

# plt.show()

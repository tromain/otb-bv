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
import sys

def linterpol(wl, solar_table):
    
    for i in range(len(solar_table)-1):
        prev = solar_table[i][0]
        folw = solar_table[i+1][0]
        prevv = solar_table[i][1]
        folwv = solar_table[i+1][1]
        if prev <= wl and folw >= wl:
            result = (folwv-prevv)/(folw-prev)*(wl-prev)+prevv
            return result
    return -1
            
            
def generate_rsr_file(input_rsr_file, reference_rsr, output_rsr_file):
    ref_solar = []
    with open(reference_rsr,'r') as rrsr:
        for l in rrsr.readlines():
            (wl, solar) = string.split(l)[0:2]
            ref_solar.append((float(wl),float(solar)))

    with open(input_rsr_file, 'r') as irsr:
        with open(output_rsr_file, 'w') as orsr:
            for l in irsr.readlines():
                fields = string.split(l)
                wl = fields[0]
                solar = str(linterpol(float(wl), ref_solar))
                bands = string.join(fields[1:], ' ')
                orsr.write(wl+" "+solar+" "+bands+"\n")
    
if __name__ == '__main__':
    if len(sys.argv) != 4:
        print """
        Script to generate a RSR file from a csv file containing the spectral responses.
        The csv file is assumed to be space separated and contain a first colum with wavelenghts and the following columns being the spetral responses, one colume per band. The lines are assumed to be sorted in increasing order of wavelength.
        A reference RSR file is used to obtain the solar irradiances for each wavelength. This reference file is assumed to cover at least the same spectral domain as the input file. However, the wavelength sampling can be different since a linear interpolation is used to retrieve the solar irradiance for the input wavelengths.
        The output file will use the wavelength sampling of the input file and the second column will be the interpolated solar irradiances.
        """
        print "Usage: "+sys.argv[0]+" input_file reference_file output_file"
    else:
        generate_rsr_file(sys.argv[1],sys.argv[2],sys.argv[3])

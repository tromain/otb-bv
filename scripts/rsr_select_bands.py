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

def rsr_select_bands(input_rsr_file, output_rsr_file, band_list):
    bl = [int(b) for b in band_list]
    band_list = [str(b) for b in sorted(bl)]
    print "Selecting bands "+string.join(band_list,', ')
    with open(input_rsr_file, 'r') as irsr:
        with open(output_rsr_file, 'w') as orsr:
            for l in irsr.readlines():
                fields = string.split(l)
                ol = fields[0]+" "+fields[1]
                for b in band_list:
                    ol += " "+fields[int(b)+1]
                orsr.write(ol+"\n")
    
if __name__ == '__main__':
    if len(sys.argv) < 4:
        print """
        Script to select a subset of the spectral band of an RSR file and produce a new SRS file.
        """
        print "Usage: "+sys.argv[0]+" input_file output_file band_list [starting with 1, white space separated]"
    else:
        rsr_select_bands(sys.argv[1],sys.argv[2],sys.argv[3:])

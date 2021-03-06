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

import os

rsr_dir = os.environ['bv_src_dir']+"/data/"
nbSamples_train = 50000
nbSamples_test = 500
noise_std = 0.001
nthreads = 8
config_path = "./"

for useVI in ["yes","no"]:
    for distlai in ["normal", "lognormal"]:
        for stdlai in range(100,1000,100):
            for modlai in range(0,1000,50):
                for maxlai in range(500,1500,200):
                    cfg_base = useVI+"_"+distlai+"_"+str(maxlai)+"_"+str(modlai)+"_"+str(stdlai)
                    cfgname = cfg_base+".cfg"
                    with open(config_path+cfgname, 'w') as cfgf:
                        cfgf.write("""paths:
                        {
                        wdpath : \"/tmp/"""+cfg_base+"""\"
                        varName : \"MLAI\"
                        rsr_dir : \""""+rsr_dir+"""\"
                        input_var_file : \"input-vars\"
                        input_var_file_test : \"input-vars-test\"
                        }

                        simulation:
                        {
                        simulate : \"yes\"
                        nbSamples_train : """+str(nbSamples_train)+"""
                        nbSamples_test : """+str(nbSamples_test)+"""
                        noise_std : """+str(noise_std)+"""
                        minlai : 0.0
                        maxlai : """+str(maxlai/100.0)+"""
                        modlai : """+str(modlai/100.0)+"""
                        stdlai : """+str(stdlai/100.0)+"""
                        distlai : \""""+distlai+"""\"
                        useVI : \""""+useVI+"""\"
                        nthreads : """+str(nthreads)+"""
                        }

                        inversion:
                        {
                        bestof : 1
                        regressor : \"nn\" # nn svr rfr mlr
                        }
                        """ )

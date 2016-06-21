#!/bin/bash
#PBS -N parSweepOTBBV
#PBS -J 1-3600:1
#PBS -l select=1:ncpus=2:mem=500mb
#PBS -l walltime=08:00:00

module load cmake
module load curl
module load boost
module load gsl
module load python/2.7.5
module remove xerces/2.7
module load xerces/2.8
module load gdal/1.11.0-py2.7

otbbuild="otb_superbuild"
version="5.3.0"
build_type="Release"
name=$otbbuild-$version-$build_type
src=$DATACI/modules/repository/$otbbuild/OTB
dl=$DATACI/modules/repository/$otbbuild/SuperBuild-archives/
otb_install_dir=$DATACI/modules/repository/$otbbuild/$name-install/
bv_install_dir=$DATACI/modules/repository/otb-bv
bv_src_dir=$HOME/CESBIO/otb-bv
. $otb_install_dir/config_otb.sh


export ITK_AUTOLOAD_PATH=$bv_install_dir/applications
export PATH=${OTB_HOME}/bin:$PATH
export LD_LIBRARY_PATH=${OTB_HOME}/lib:${OTB_HOME}/lib/otb/python:${LD_LIBRARY_PATH}
export PYTHONPATH=${OTB_HOME}/lib/otb/python:${PYTHONPATH}
export GDAL_DATA=${OTB_HOME}/share/gdal
export GEOTIFF_CSV=${OTB_HOME}/share/epsg_csv

cd $PBS_O_WORKDIR

#file containing one config file name per line 
input="par-sweep-cfgs.txt"
#get the config filename for a job
cfg=$(sed -n ${PBS_ARRAY_INDEX}p $input)

#
python $bv_src_dir/src/scripts/validation.py $cfg
cp $cfg /tmp/$(basename $cfg .cfg)
tar cvzf $(basename $cfg .cfg).tgz /tmp/$(basename $cfg .cfg)
rm -rf /tmp/$(basename $cfg .cfg)
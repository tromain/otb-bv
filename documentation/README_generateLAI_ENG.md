**SCRIPT 'generate_LAI.py'**
============================

this script permits to generate an LAI image from real sattelite images from Muscate and Sen2Cor using as input :

	- an xml file associated to an image from Sentinel-2A or Landsat-8 available from:

		* Theia --> https://theia.cnes.fr/atdistrib/rocket/#/home --> Sentinel-2 and Landsat8 L2A images (images generated with CNES Muscate platform)
		* PEPS --> https://peps.cnes.fr/rocket/#/search?page=1 --> select Collection: "Sentinel-2 Tiles" and Level LEVEL2A (image generated with Sen2Cor)
		* Copernicus --> https://scihub.copernicus.eu/dhus/#/home --> Sentinel-2A L2A (image generated with Sen2Cor)

	- a .json configuration file containing the distribution parameters for the simulation of biophysical variables and the regression model.

	- name of the output LAI image

	- (optional) if you want to specify a rsr file rather than extracting it from the xml, you can give it as an argument (column 1: wavelength, column 2: solar irradiance, following columns : reflectance value for each band)

--------------------------------------------------------------------
**Script execution preparation**
--------------------------------------------------------------------
Before launching the script, you have to verify that you environment variables are correctly set, you can edit the file activate_env.sh in the script folder for this:

	- OTB_RESULT_DIR --> path to the results directory
	- PYTHONPATH --> path to OTB python libraries (.../OTB/lib/otb/python)
	- OTB_APPLICATION_PATH --> path to OTB applications (.../OTB/lib/otb/applications)

--------------------------------------------------------------------
**Script execution**
--------------------------------------------------------------------
Go to the script folder to launch it:

**<code>python3 generate_LAI.py -outLAI** output_test.tif **-xml** ~/Documents/ImagesTest/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_MTD_ALL.xml **-json** ~/Documents/DEV/OTB/SRC/Modules/Remote/OTBBioVars/scripts/config.json **-optBandList** B2 B3 B4 B8A **-extractRSR**</code>

Arguments : <ul><li><code>'-optBandList'</code> permits to specify which band we will use for the creation of the VRT file.</li>
<li><code>-extractRSR</code> tells the script to extract RSR informations from the xml file, a new RSR file will be written on the disk containing only the informations for the selected band. If you have an rsr file, you can use <code>'-rsrPath'</code> instead giving the path to you rsr file.</li>
<li><code>-json</code> file containing the distribution parameters of pbiophysical variables</li></ul>

For Theia images, you have an additional option available: '-optFRE_SRE'. it permits, for the VRT creation, to choose between SRE images ('Surface REflectance' corrected from atmospheric effects) and FRE images, ('Flat REflectance' with an additional slope effect correction)
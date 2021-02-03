**SCRIPT 'bv_net.py'**
======================

NB : The details about the content of each Input/output files of the following functions are stored in the file script_io_files_content.txt in the documentation folder

4 functions are executed by the script :

1. **<code>parseConfigFile()</code>** gather informations contained in the input file *config.cfg* :
		<ul><li><code>bvDistributionFileName</code> - Name of the file containing the biophysical variables (section 'bvDistribution' in *config.cfg*)</li>
		<li><code>numberOfSamples</code> - Number of samples to use for the training (section 'bvDistribution' in *config.cfg*)</li>
		<li><code>trainingDataFileName</code> - Name of the file containing the training data (section "training" in *config.cfg*)</li>
		<li><code>outputModelFileName</code> - Name of the output model file (section 'learning' in *config.cfg*)</li>
		<li><code>simulationParameters</code> - all parameters used in the variables simulation (section 'Simulation' in *config.cfg*</li></ul>

	INPUT: <ul><li>File *config.cfg* example can be found in the script folder config-example.cfg</li></ul>

------------------------------------------------------------------------------------------------------------------------------------
2. **<code>generateInputBVDistribution()</code>** executes the application **"BVInputVariableGeneration"** using the input parameters <code>simuPars</code>. This application generates values for all biophysical variables (MLAI, ALA, CrownCover, HsD, N, ...) using a distribution defined in *config.cfg*.

	INPUT: <ul><li><code>bvDistributionFileName</code> - Name of the file containing the biophysical variables</li>
		<li><code>numberOfSamples</code> - Number of samples for each biovar to simulate</li>
		<li><code>simulationParameters</code> - simulation parameters (min, max for each variable etc)</li></ul>

------------------------------------------------------------------------------------------------------------------------------------
3. **<code>generateTrainingData()</code>** executes the application **"ProSailSimulator"** which uses as input the values contained in <code>simuPars</code> and <code>bvDistributionFileName</code>. If simulation is activated, this application creates 2 files : one containing the reflectance values for each band using ProSail, one containing the output training data <code>trainingDataFileName</code> containing the variable to learn in the first column, and the associated reflectances on the other columns. In the file *config.cfg* there is a parameter which permits to add/remove the two last columns which corresponds to FAPAR/FCOVER values which can be ignored. if the option <code>add_angles</code> is activated, the values of <code>solarZenithAngle</code>, <code>sensorZenithAngle</code> and <code>solarSensorAzimuth</code> are added in <code>trainingDataFileName</code>.

	INPUT: <ul><li><code>bvDistributionFileName</code> - Name of the file containing the biophysical variables</li>
		<li><code>simulationParameters</code> - simulation parameters</li>
		<li><code>trainingDataFileName</code> - Name of the file containing the training data</li>
		<li><code>bvindex</code> - index of the variable to learn (MLAI, ALA... defined in the top of the script)</li></ul>

------------------------------------------------------------------------------------------------------------------------------------		 
4. **<code>learnBVModel()</code>** executes the application **"InverseModelLearning"** using the training data as input and generates the associated regression model. This also generates a normalization file, which permits to improve the estimation of the biophysical variable 

	INPUT: <ul><li><code>trainingDataFileName</code> - Name of the file containing the training data</li>
		<li><code>outputModelFileName</code> - Name of the output model</li>
		<li><code>regressionType</code> - regression method to use for model generation</li>
		<li><code>normalizationFile</code> - output normalization file which permits to normalize the biophysical value</li></ul>

------------------------------------------------------------------------------------------------------------------------------------
Applications used :
**"BVInputVariableGeneration"** ==> **"ProSailSimulator"** ==> **"InverseModelLearning"**
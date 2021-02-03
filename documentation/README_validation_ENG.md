**SCRIPT 'validation.py'**
======================

This scripts permits to create 2 datasets : (MLAI, ALA, CrownCover, HsD, N, ...):
* the first is used as training dataset: the simulated reflectances from this dataset will be used to create a regression model.
* the other is used as a test dataset to validate the model inversion on a new simulated reflectances dataset.

Note : the file script_io_files_content.txt in the documentation folder

------------------------------------------------------------------------------------------------------------------------------------
This script is calling the following functions:
1. Gather informations in *validation.cfg*
2. Generation of two biophysical variable files by calling **<code>generateInputBVDistribution()</code>** which launches **"BVInputVariableGeneration"**: generate training and test datasets
3. For each satellite mentioned in *validation.cfg*:
	- Extraction of informations about acquisition config and spectral band position
	- Training data generation using **<code>generateTrainingData()</code>**. Two files are created for each dataset: *satName_reflectances* containing the reflectance value for each band obtained from **"ProSailSimulator"** using acquisition and spectral band config, and *satName_training* in which the first column corresponds to the selected biophysical variable and the next columns are the associated reflectance values.
	- Regression model generation using *satName_training* as an input of **"InverseModelLearning"**: the output files *satName_model_nn*, *satName_model_nn_errest* and *satName_normalisation* contain the model carateristics.
	- Inversion of reflectance values from test dataset and validation using <code>invertBV()</code> which calls the application **"BVInversion"**. Creation of the file *satName_inversion_nn* et *satName_validation_nn* for the test dataset, and *satName_inversion_gt_nn* et *satName_validation_gt_nn* from ground truth reflectance contained in the script *satelliteName_data.py*. The Validation file contains in his first column the data from *satName_inversion_nn* and in its second column the values of the biophysical variable from *satName_training*.

------------------------------------------------------------------------------------------------------------------------------------
Applications used:
With training dataset:
**"BVInputVariableGeneration"** ==> **"ProSailSimulator"** ==> **"InverseModelLearning"**==> Creation of a regression model associated to the selected biophysical variable

With the test dataset:
**"BVInputVariableGeneration"** ==> **"ProSailSimulator"** ==> **"BVInversion"** using the regression model generated from the training data ==> results included in validation file

With the ground truth from *satelliteName_data.py*:
**"BVInversion"** using the regression model from training data ==> results included in the validation file
**SCRIPT 'validation.py'**
======================

Ce script permet de créer deux jeux de données contenant des variables biophysiques (MLAI, ALA, CrownCover, HsD, N, ...):
* Le premier sera utilisé comme base d'apprentissage: les valeurs de réflectance simulées à partir de ces données seront utilisées afin de créer un modèle de régression.
* L'autre sera utilisé comme base test pour tester et valider l'inversion du modèle sur un nouveau jeu de valeurs de réflectances simulées.

Note : Pour avoir le détail de ce que contient chaque fichier généré par les fonctions de BVNet, veuillez vous reporter au document BVnet_files_content_description présent dans le dossier documentation

------------------------------------------------------------------------------------------------------------------------------------
Pour plus de détails, ce script réalise les étapes suivantes:
1. Récupération des informations de configuration contenus dans le fichier *validation.cfg*
2. Génération de deux fichiers de variables biophysiques grâce à **<code>generateInputBVDistribution()</code>** exécutant l'application **"BVInputVariableGeneration"**: données d'apprentissage et données test
3. Pour chaque satellite mentionné dans *validation.cfg*:
	- Extraction des infos concernant les config d'acquisition et la position des bandes spectrales
	- Génération des données d'apprentissage pour les deux jeux de données grâce à **<code>generateTrainingData()</code>**. Deux fichiers sont créés pour chaque jeu de données: *satName_reflectances* regroupant les valeurs de réflectance (une colonne par bande spectrale) simulées grâce à l'application **"ProSailSimulator"** à partir des variables ProSail, de la configuration de l'acquisition et des bandes spectrales, et *satName_training* dont la première colonne correspond aux valeurs MLAI des variables d'entrée et les suivantes aux valeurs de réflectances associées.
	- Création d'un modèle de régression à partir du fichier *satName_training* et de l'application **"InverseModelLearning"**: les fichiers *satName_model_nn*, *satName_model_nn_errest* et *satName_normalisation* créés contiennent les caractéristiques du modèle.
	- Inversion des valeurs de réflectances des données test et de validation grâce à <code>invertBV()</code> exécutant l'application **"BVInversion"**. Création des fichiers *satName_inversion_nn* et *satName_validation_nn* pour la base test, et *satName_inversion_gt_nn* et *satName_validation_gt_nn* pour des valeurs de réflectances provenant du script *formosat_data.py*. Le fichier de validation regroupe dans sa première colonne les données du fichier *satName_inversion_nn* obtenu à partir du modèle établi à l'étape précédente et dans sa deuxième colonne les valeurs MLAI du fichier *satName_training* initial.

------------------------------------------------------------------------------------------------------------------------------------
Applications utilisées:
Avec les données de la base d'apprentissage:
**"BVInputVariableGeneration"** ==> **"ProSailSimulator"** ==> **"InverseModelLearning"**==> Création du modèle de régression associé aux variables biophysiques

Avec les données de la base test:
**"BVInputVariableGeneration"** ==> **"ProSailSimulator"** ==> **"BVInversion"** utilisant le modèle de régession obtenu par les données d'apprentissage ==> Résultats inclus dans fichier de validation

Avec les données provenant de *formosat_data.py*:
**"BVInversion"** utilisant le modèle de régession obtenu par les données d'apprentissage ==> Résultats inclus dans fichier de validation

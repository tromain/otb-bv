**SCRIPT 'bv_net.py'**
======================

Note : Pour avoir le détail de ce que contient chaque fichier généré par les fonctions qui suivent, veuillez vous reporter au document script_io_files_content.txt présent dans le dossier documentation

Quatre fonctions sont exécutées succcessivement:
1. **<code>parseConfigFile()</code>** récupère les infos contenus dans le fichier *config.cfg* fournit en entrée et les stocke:
		<ul><li><code>bvDistributionFileName</code> - Nom du fichier contenant les variables biophysiques (info partie 'bvDistribution' du *config.cfg*)</li>
		<li><code>numberOfSamples</code> - Nombre d'échantillons (info partie 'bvDistribution' du *config.cfg*)</li>
		<li><code>trainingDataFileName</code> - Nom du fichier contenant les données d'apprentissage (partie "training" du *config.cfg*)</li>
		<li><code>outputModelFileName</code> - Nom du fichier de sortie (partie 'learning' du *config.cfg*)</li>
		<li><code>simulationParameters</code> - Chargement de toutes les variables renseignées dans la partie 'Simulation' du *config.cfg*</li></ul>

	INPUT: <ul><li>Fichier *config.cfg* structuré selon le fichier suivant: ../DEV/OTB/SRC/Modules/Remote/OTBBioVars/data/validation_example.cfg</li></ul>

------------------------------------------------------------------------------------------------------------------------------------
2. **<code>generateInputBVDistribution()</code>** exécute l'application **"BVInputVariableGeneration"** en utilisant en argument les valeurs des paramètres provenant de <code>simuPars</code>. Cette application crée un fichier contenant des variables biophysiques d'entrées (MLAI, ALA, CrownCover, HsD, N, ...) générées selon la distribution renseignée dans le fichier *config.cfg*.

	INPUT: <ul><li><code>bvDistributionFileName</code> - Nom du fichier contenant les variables biophysiques</li>
		<li><code>numberOfSamples</code> - Nombre d'échantillons</li>
		<li><code>simulationParameters</code> - Paramètres de simulation</li></ul>

------------------------------------------------------------------------------------------------------------------------------------
3. **<code>generateTrainingData()</code>** exécute l'application **"ProSailSimulator"** qui utilise en argument des valeurs de paramètres provenant de <code>simuPars</code> et <code>bvDistributionFileName</code>. Si la simulation est activée, cette application crée deux fichiers: l'un contenant les valeurs de réflectance (une colonne par bande spectrale) simulées à partir des variables ProSail, de la configuration de l'acquisition et des bandes spectrales, et l'autre résumant le résultat de l'application dans le fichier <code>trainingDataFileName</code> (la première colonne correspond aux valeurs MLAI des variables biophysiques d'entrée et les suivantes aux valeurs de réflectances associées). En fonction du fichier *config.cfg*, les deux dernières valeurs de réflectance correspondant à FAPAR/FCOVER peuvent être ignorées. Si l'option <code>add_angles</code> est activée, les valeurs de <code>solarZenithAngle</code>, <code>sensorZenithAngle</code> et <code>solarSensorAzimuth</code>sont également ajoutées dans le fichier <code>trainingDataFileName</code>.

	INPUT: <ul><li><code>bvDistributionFileName</code> - Nom du fichier de distribution</li>
		<li><code>simulationParameters</code> - Paramètres de simulation</li>
		<li><code>trainingDataFileName</code> - Nom du fichier contenant les données d'apprentissage</li>
		<li><code>bvindex</code> - Position index MLAI dans fichier</li></ul>
    
------------------------------------------------------------------------------------------------------------------------------------		 
4. **<code>learnBVModel()</code>** exécute l'application **"InverseModelLearning"** utilisant comme argument les données d'apprentissage précédemment générées et permettant de créer le modèle de régression associé. 

	INPUT: <ul><li><code>trainingDataFileName</code> - Nom du fichier contenant les données d'apprentissage</li>
		<li><code>outputModelFileName</code> - Nom du fichier de sortie</li>
		<li><code>regressionType</code> - Méthode de régression à utiliser pour créer le modèle</li>
		<li><code>normalizationFile</code> - Fichier de normalisation</li></ul>

------------------------------------------------------------------------------------------------------------------------------------
Applications utilisées
**"BVInputVariableGeneration"** ==> **"ProSailSimulator"** ==> **"InverseModelLearning"**

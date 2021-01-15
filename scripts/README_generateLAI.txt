		**NOTES SCRIPT generate_LAI.PY**
--------------------------------------------------------------------
--------------------------------------------------------------------
Le script generate_LAI permet de générer une image LAI à partir de:
	- un fichier xml associé à une image Sentinel-2A pouvant être récupérée à partir des sites suivants:
		* Theia --> https://theia.cnes.fr/atdistrib/rocket/#/home --> Sentinel-2 L2A
		* PEPS --> https://peps.cnes.fr/rocket/#/search?page=1 --> sélectionner Collection: Sentinel-2 Tuilés et niveau de traitement LEVEL2A

	- un fichier de configuration .json contenant des paramètres de distribution à appliquer pour la simulation des varaibles biophysiques et l'application du modèle de régression.

	- le nom de l'image LAI générée en sortie

	- (optionnel) le chemin vers un fichier RSR contenant une série de valeurs (colonne 1: longueur d'onde, colonne 2: irradiance solaire, colonnes suivantes : valeurs de réflectance pour chaque bande)
--------------------------------------------------------------------
Exécution du script:
Avant de lancer le script, vérifier que les variables d'environnement suivantes ont été correctement renseignées:
	- OTB_RESULT_DIR --> chemin vers le répertoire où doivent être générés les résultats
	- PYTHONPATH --> chemin vers la bibliothèque python de l'OTB (.../OTB/build/lib/otb/python)
	- OTB_APPLICATION_PATH --> chemin vers l'application OTB (.../OTB/build/lib/otb/applications)
(cf .../scripts/activate_env.sh)

Se placer dans le répertoire contenant le script:
./generate_LAI.py -outLAI output_test.tif -xml ~/Documents/ImagesTest/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_MTD_ALL.xml -json ~/Documents/DEV/OTB/SRC/Modules/Remote/OTBBioVars/scripts/config.json -optBandList B2 B3 B4 B8A -extractRSR 

L'argument '-optBandList' permet de spécifier quelles bandes doivent être prises en compte lors de la création du VRT. Un nouveau fichier RSR sera créé à partir du RSR initial. Toutes les bandes non sélectionnées y seront supprimées.

Avec l'option '-extractRSR', le fichier RSR est directement créé à partir des valeurs de réflectance inclues dans la partie 'Informations radiométriques' du fichier xml fourni en entrée. Si l'on souhaite utiliser un fichier RSR externe, il faudra remplacer cette option par '-rsrPath' suivi du chemin vers le fichier RSR externe.

Pour les images Theia, une option supplémentaire peut être ajoutée: '-optFRE_SRE'. Elle permet, pour la création du VRT, le choix entre l'utilisation des images SRE, cad 'Surface REflectance' corrigées des effets atmosphériques et des images FRE, cad 'Flat REflectance' subissant une correction supplémentaire qui prend en compte les effets des pentes.









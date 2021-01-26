import filecmp
import generate_LAI as genLAI
import json
from osgeo import gdal
import unittest
import sys
import os
import inspect

class GenerateLaiTest(unittest.TestCase):

    def __init__(self, testname, orig_folder, dest_folder):
        super(GenerateLaiTest, self).__init__(testname)
        self.original_images_folder = orig_folder
        self.output_images_folder = dest_folder

    def setUp(self):
        """Initialisation des tests."""
        self.xml_muscate = self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_MTD_ALL.xml'
        self.xml_sen2cor = self.original_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE/MTD_MSIL2A.xml'
        filename = inspect.getframeinfo(inspect.currentframe()).filename
        path     = os.path.dirname(os.path.abspath(filename))
        self.cfgFile = path + '/config.json'
        self.bandList_muscate = ['B2', 'B3', 'B4', 'B5', 'B6', 'B7', 'B8', 'B8A', 'B11', 'B12']
        self.bandList_sen2cor = ['B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'B7', 'B8A', 'B9', 'B11', 'B12']
        self.resolution = [10, 20, 60]

        # Reference for files generated by Muscate
        self.ref_vrt_muscate = [self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_vrt_10.vrt',
                                self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_vrt_20.vrt',
                                self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_vrt_60.vrt']
        self.ref_img_lai_muscate = [self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_test_result_10.tif',
                                    self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_test_result_20.tif',
                                    self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_test_result_60.tif']
        self.ref_rsrFile_muscate = self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_rsrFile.rsr'
        self.ref_rsrFileFiltered_muscate = self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_rsrFileFiltered.rsr'

        # Reference for files generated by Sen2Cor
        self.ref_vrt_sen2cor = [self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_vrt_10.vrt',
                                self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_vrt_20.vrt',
                                self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_vrt_60.vrt']
        self.ref_img_lai_sen2cor = [self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_test_result_10.tif',
                                    self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_test_result_20.tif',
                                    self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_test_result_60.tif']
        self.ref_rsrFile_sen2cor = self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_rsrFile.rsr'
        self.ref_rsrFileFiltered_sen2cor = self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_rsrFileFiltered.rsr'

    def test0_load_cfg(self):
        print('-----  Testing config file import  -----')

        cfg = genLAI.ConfigFile()
        cfg.import_data_from_file(**json.load(open(self.cfgFile)))
        self.assertIsInstance(cfg, genLAI.ConfigFile)
        self.assertEqual(cfg.vrt_name, '_vrt.vrt')
        self.assertEqual(cfg.nb_samples, 1000)
        self.assertEqual(cfg.bv_filename, 'bv-input-vars.txt')
        self.assertEqual(cfg.minlai, 0.0)
        self.assertEqual(cfg.maxlai, 8.0)
        self.assertEqual(cfg.modlai, 2.0)
        self.assertEqual(cfg.stdlai, 2.0)
        self.assertEqual(cfg.distlai, 'normal')
        self.assertEqual(cfg.training_file, 'training.txt')
        self.assertEqual(cfg.invert_bv, 'MLAI')
        self.assertEqual(cfg.rsr_file, None)
        self.assertEqual(cfg.output_file, 'simus.txt')
        self.assertEqual(cfg.noisestd, 0.01)
        self.assertEqual(cfg.bestof, 1)
        self.assertEqual(cfg.regressor, 'nn')
        self.assertEqual(cfg.model_file, 'model.txt')
        self.assertEqual(cfg.useVI, 'no')
        self.assertEqual(cfg.simulate, True)
        self.assertEqual(cfg.add_angles, 0)
        self.assertEqual(cfg.red_index, 0)
        self.assertEqual(cfg.nir_index, 0)
        self.assertEqual(cfg.nthreads, 4)

        print('----------------------------------------')

    def test1_parser_muscate(self):
        cfg = genLAI.ConfigFile()
        cfg.import_data_from_file(**json.load(open(self.cfgFile)))

        print('----     Testing Muscate Parser   -----')

        prod = genLAI.parse_xml(self.xml_muscate, 'Muscate_Metadata', None, True)

        img_SRE = [['B11', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B11.tif'],
                   ['B12', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B12.tif'],
                   ['B2', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B2.tif'],
                   ['B3', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B3.tif'],
                   ['B4', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B4.tif'],
                   ['B5', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B5.tif'],
                   ['B6', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B6.tif'],
                   ['B7', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B7.tif'],
                   ['B8', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B8.tif'],
                   ['B8A', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_SRE_B8A.tif']]

        img_FRE = [['B11', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B11.tif'],
                   ['B12', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B12.tif'],
                   ['B2', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B2.tif'],
                   ['B3', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B3.tif'],
                   ['B4', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B4.tif'],
                   ['B5', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B5.tif'],
                   ['B6', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B6.tif'],
                   ['B7', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B7.tif'],
                   ['B8', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B8.tif'],
                   ['B8A', self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_FRE_B8A.tif']]

        self.assertIsInstance(prod, genLAI.Muscate)
        self.assertEqual(prod.xml_path, self.original_images_folder + 'SENTINEL2A_Toulouse/SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_V1-8_MTD_ALL.xml')
        self.assertEqual(prod.name, 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D')
        self.assertEqual(prod.platform, 'SENTINEL2A')
        self.assertEqual(prod.authority, 'THEIA')
        self.assertEqual(prod.solar_zenith_angle, 33.0)
        self.assertEqual(prod.sensor_zenith_angle, 2.0)
        self.assertEqual(prod.solar_sensor_azimuth_angle, -49.0)
        self.assertEqual(prod.band_list, None)
        self.assertEqual(prod.band_number, None)
        self.assertEqual(prod.image_band_SRE, img_SRE)
        self.assertEqual(prod.image_band_FRE, img_FRE)
        self.assertEqual(prod.ref_rsr_file, self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_rsrFile.rsr')
        self.assertEqual(prod.rsr_file, self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_rsrFileFiltered.rsr')
        self.assertEqual(prod.resolutions, {'B10': 60, 'B11': 20, 'B4': 10, 'B5': 20, 'B6': 20, 'B7': 20, 'B12': 20, 'B1': 60, 'B2': 10, 'B3': 10, 'B8A': 20, 'B8': 10, 'B9': 60})
        self.assertEqual(prod.selected_band_list, None)

        self.assertTrue(filecmp.cmp(prod.ref_rsr_file, self.ref_rsrFile_muscate), 'RSR files are different')

        print('----------------------------------------')

    def test2_parser_sen2cor(self):
        cfg = genLAI.ConfigFile()
        cfg.import_data_from_file(**json.load(open(self.cfgFile)))

        print('----     Testing Sen2Cor Parser   -----')

        prod = genLAI.parse_xml(self.xml_sen2cor, 'Sen2Cor_Metadata', None, True)

        path = self.original_images_folder + \
               'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE/' \
               'GRANULE/L2A_T48MVV_A018519_20190108T032310/IMG_DATA/'

        img_list = [('B2', 10, path + 'R10m/T48MVV_20190108T031111_B02_10m.jp2'),
                    ('B3', 10, path + 'R10m/T48MVV_20190108T031111_B03_10m.jp2'),
                    ('B4', 10, path + 'R10m/T48MVV_20190108T031111_B04_10m.jp2'),
                    ('B8', 10, path + 'R10m/T48MVV_20190108T031111_B08_10m.jp2'),
                    ('B2', 20, path + 'R20m/T48MVV_20190108T031111_B02_20m.jp2'),
                    ('B3', 20, path + 'R20m/T48MVV_20190108T031111_B03_20m.jp2'),
                    ('B4', 20, path + 'R20m/T48MVV_20190108T031111_B04_20m.jp2'),
                    ('B5', 20, path + 'R20m/T48MVV_20190108T031111_B05_20m.jp2'),
                    ('B6', 20, path + 'R20m/T48MVV_20190108T031111_B06_20m.jp2'),
                    ('B7', 20, path + 'R20m/T48MVV_20190108T031111_B07_20m.jp2'),
                    ('B8A', 20, path + 'R20m/T48MVV_20190108T031111_B8A_20m.jp2'),
                    ('B11', 20, path + 'R20m/T48MVV_20190108T031111_B11_20m.jp2'),
                    ('B12', 20, path + 'R20m/T48MVV_20190108T031111_B12_20m.jp2'),
                    ('B1', 60, path + 'R60m/T48MVV_20190108T031111_B01_60m.jp2'),
                    ('B2', 60, path + 'R60m/T48MVV_20190108T031111_B02_60m.jp2'),
                    ('B3', 60, path + 'R60m/T48MVV_20190108T031111_B03_60m.jp2'),
                    ('B4', 60, path + 'R60m/T48MVV_20190108T031111_B04_60m.jp2'),
                    ('B5', 60, path + 'R60m/T48MVV_20190108T031111_B05_60m.jp2'),
                    ('B6', 60, path + 'R60m/T48MVV_20190108T031111_B06_60m.jp2'),
                    ('B7', 60, path + 'R60m/T48MVV_20190108T031111_B07_60m.jp2'),
                    ('B8A', 60, path + 'R60m/T48MVV_20190108T031111_B8A_60m.jp2'),
                    ('B9', 60, path + 'R60m/T48MVV_20190108T031111_B09_60m.jp2'),
                    ('B11', 60, path + 'R60m/T48MVV_20190108T031111_B11_60m.jp2'),
                    ('B12', 60, path + 'R60m/T48MVV_20190108T031111_B12_60m.jp2')]

        self.assertIsInstance(prod, genLAI.Sen2Cor)
        self.assertEqual(prod.xml_path, self.original_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE/MTD_MSIL2A.xml')
        self.assertEqual(prod.name, 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE')
        self.assertEqual(prod.platform, 'Sentinel-2A')
        self.assertEqual(prod.authority, None)
        self.assertEqual(prod.solar_zenith_angle, 29.0)
        self.assertEqual(prod.sensor_zenith_angle, 3.0)
        self.assertEqual(prod.solar_sensor_azimuth_angle, -83.0)
        self.assertEqual(prod.band_list, None)
        self.assertEqual(prod.band_number, None)
        self.assertEqual(prod.image_band_list, img_list)
        self.assertEqual(prod.ref_rsr_file, self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_rsrFile.rsr')
        self.assertEqual(prod.rsr_file, self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_rsrFileFiltered.rsr')
        self.assertEqual(prod.resolutions, {'B10': 60, 'B11': 20, 'B4': 10, 'B5': 20, 'B6': 20, 'B7': 20, 'B12': 20, 'B1': 60, 'B2': 10, 'B3': 10, 'B8A': 20, 'B8': 10, 'B9': 60})
        self.assertEqual(prod.selected_band_list, None)

        self.assertTrue(filecmp.cmp(prod.ref_rsr_file, self.ref_rsrFile_sen2cor), 'RSR files are different')

        print('----------------------------------------')


    def test3_create_vrt_muscate(self):
        cfg = genLAI.ConfigFile()
        cfg.import_data_from_file(**json.load(open(self.cfgFile)))
        prod = genLAI.parse_xml(self.xml_muscate, 'Muscate_Metadata', None, True)

        print('---- Testing VRT creation Muscate -----')

        for res in self.resolution:
            prod.selected_band_list = self.bandList_muscate
            prod.filter_rsr_file()
            new_vrt_name = '_vrt_{}.vrt'.format(res)
            image_vrt = genLAI.create_vrt(prod, new_vrt_name, res)
            output_vrt = self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D' + new_vrt_name
            vrt = gdal.Open(output_vrt)
            gt = vrt.GetGeoTransform()
            self.assertEqual(abs(gt[1]), res)
            self.assertEqual(abs(gt[5]), res)

            self.assertTrue(filecmp.cmp(output_vrt, self.ref_vrt_muscate[self.resolution.index(res)]), 'VRT images are different')

        print('----------------------------------------')

    def test4_create_vrt_sen2cor(self):
        cfg = genLAI.ConfigFile()
        cfg.import_data_from_file(**json.load(open(self.cfgFile)))
        prod = genLAI.parse_xml(self.xml_sen2cor, 'Sen2Cor_Metadata', None, True)

        print('---- Testing VRT creation Sen2Cor -----')

        for res in self.resolution:
            prod.selected_band_list = self.bandList_sen2cor
            prod.filter_rsr_file()
            new_vrt_name = '_vrt_{}.vrt'.format(res)
            image_vrt = genLAI.create_vrt(prod, new_vrt_name, res)
            output_vrt = self.output_images_folder +'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE' + new_vrt_name
            vrt = gdal.Open(output_vrt)
            gt = vrt.GetGeoTransform()
            self.assertEqual(abs(gt[1]), res)
            self.assertEqual(abs(gt[5]), res)

            self.assertTrue(filecmp.cmp(output_vrt, self.ref_vrt_sen2cor[self.resolution.index(res)]), 'VRT images are different')

        print('----------------------------------------')

    def test5_img_inversion_muscate(self):
        cfg = genLAI.ConfigFile()
        cfg.import_data_from_file(**json.load(open(self.cfgFile)))
        prod = genLAI.parse_xml(self.xml_muscate, 'Muscate_Metadata', None, True)

        model = genLAI.generate_model(prod, cfg)

        # Files generated by genLAI.generate_model() are random --> no possible test

        prod.selected_band_list = self.bandList_muscate
        prod.filter_rsr_file()
        rsr_filtered_file = model.split('model')[0] + 'rsrFileFiltered.rsr'
        self.assertTrue(filecmp.cmp(rsr_filtered_file, self.ref_rsrFileFiltered_muscate),
                        'RSR filtered files are different')

        for res in self.resolution:
            new_vrt_name = '_vrt_{}.vrt'.format(res)
            image_vrt = genLAI.create_vrt(prod, new_vrt_name, res)

            print('---- Testing invImage - res {} musc  -----'.format(res))

            image_LAI = self.output_images_folder + 'SENTINEL2A_20180819-105124-182_L2A_T31TCJ_D_test_result_{}.tif'.format(res)

            genLAI.inv_image(image_vrt, model, image_LAI)

            img_lai = gdal.Open(image_LAI)
            gt = img_lai.GetGeoTransform()
            self.assertEqual(abs(gt[1]), res)
            self.assertEqual(abs(gt[5]), res)

    def test6_img_inversion_sen2cor(self):
        cfg = genLAI.ConfigFile()
        cfg.import_data_from_file(**json.load(open(self.cfgFile)))
        prod = genLAI.parse_xml(self.xml_sen2cor, 'Sen2Cor_Metadata', None, True)

        model = genLAI.generate_model(prod, cfg)

        # Files generated by genLAI.generate_model() are random --> no possible test

        prod.selected_band_list = self.bandList_sen2cor
        prod.filter_rsr_file()
        rsr_filtered_file = model.split('model')[0] + 'rsrFileFiltered.rsr'
        self.assertTrue(filecmp.cmp(rsr_filtered_file, self.ref_rsrFileFiltered_sen2cor),
                        'RSR filtered files are different')

        for res in self.resolution:
            new_vrt_name = '_vrt_{}.vrt'.format(res)
            image_vrt = genLAI.create_vrt(prod, new_vrt_name, res)

            print('----Testing invImage - res {} sen2cor-----'.format(res))

            image_LAI = self.output_images_folder + 'S2A_MSIL2A_20190108T031111_N0211_R075_T48MVV_20190108T071022.SAFE_test_result_{}.tif'.format(res)

            genLAI.inv_image(image_vrt, model, image_LAI)

            img_lai = gdal.Open(image_LAI)
            gt = img_lai.GetGeoTransform()
            self.assertEqual(abs(gt[1]), res)
            self.assertEqual(abs(gt[5]), res)

if __name__ == '__main__':

    if(len(sys.argv)==3):
        orig = sys.argv[1]
        dest = sys.argv[2]
    else:
        orig = '/home/travis/build/tromain/otb-bv/data/ImagesTest/'
        dest = '/home/travis/build/tromain/otb-bv/data/OutputTest/'
    
    test_loader = unittest.TestLoader()
    test_names = test_loader.getTestCaseNames(GenerateLaiTest)

    suite = unittest.TestSuite()
    for test_name in test_names:
        if "muscate" in test_name:
            suite.addTest(GenerateLaiTest(test_name, orig, dest))

    result = unittest.TextTestRunner().run(suite)
    sys.exit(not result.wasSuccessful())




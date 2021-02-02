#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================
#   Program:   generate LAI
#   Language:  python
#
#   Copyright (c) CS Group France. All rights reserved.
#
#   See otb-bv-copyright.txt for details.
#
#   This software is distributed WITHOUT ANY WARRANTY; without even
#   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#   PURPOSE.  See the above copyright notices for more information.
#
# =========================================================================

import abc
import argparse
import collections
import csv
import json
import logging
import os
from osgeo import gdal
import re
import sys
import xml.etree.ElementTree as ET

import muscateXMLParser as musc
import sen2corXMLParser as sen2cor
import otbApplication as otb
import bv_net as bv


LEVELS = ['CRITICAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG']


def _log_level_name_to_nb(log_name):
    if log_name not in LEVELS:
        raise argparse.ArgumentTypeError('{0} is invalid. Available choice: {1}'.format(log_name, LEVELS))

    log_nb = getattr(logging, log_name, logging.INFO)
    # check if logging choice has not changed from expected values
    assert isinstance(log_nb, int)

    return log_nb


class BandInfo(object):
    def __init__(self, name, resolution, min_value, max_value):
        self.name = name
        self.res = resolution
        self.min = min_value
        self.max = max_value


def inv_image(input_image, model_file, output_image, norm_file=''):
    if not os.path.isfile(input_image):
        raise FileNotFoundError("Input Image is invalid: {}".format(input_image))

    if not os.path.isfile(model_file):
        raise FileNotFoundError("Model file is invalid: {}".format(model_file))

    if norm_file.isspace():
        raise FileNotFoundError("No normalization step - file not provided")

    app = otb.Registry.CreateApplication("BVImageInversion")
    app.SetParameterString("in", input_image)
    app.SetParameterString("model", model_file)
    app.SetParameterString("normalization", norm_file)
    app.SetParameterString("out", output_image)
    app.ExecuteAndWriteOutput()

    logging.info("--------------------------------")
    logging.info("---  Image inversion done    ---")
    logging.info("--------------------------------")


def check_path(file, product):
    # Change path if no dirname in original path variable
    if os.environ.get('OTB_RESULT_DIR') in file:
        new_file = file
    else:
        new_file = os.path.join(os.environ.get('OTB_RESULT_DIR'), product.name + '_' + file)
    return new_file


def create_bv_input_vars(bv_file, nb_samples, simulation_param):
    bv.generateInputBVDistribution(bv_file, nb_samples, simulation_param)
    logging.info("Generating Input BV distribution file ({} samples) with parameters:".format(nb_samples))
    logging.info("minlai = " + str(simulation_param['minlai']))
    logging.info("maxlai = " + str(simulation_param['maxlai']))
    logging.info("modlai = " + str(simulation_param['modlai']))
    logging.info("stdlai = " + str(simulation_param['stdlai']))

    logging.info("--------------------------------")
    logging.info("--- BV Input File generated  ---")
    logging.info("--------------------------------")


def generate_train_data(bv_file, simulation_param, training_file, ind, simulate=True, add_angles=False, red_index=0,
                        nir_index=0, nthreads=2):
    bv.generateTrainingData(bv_file, simulation_param, training_file, ind, add_angles, red_index,
                            nir_index, nthreads)
    logging.info("Generating TrainingData with parameters:")
    logging.info("solarZenithAngle = " + str(simulation_param['solarZenithAngle']))
    logging.info("sensorZenithAngle = " + str(simulation_param['sensorZenithAngle']))
    logging.info("solarSensorAzimuth = " + str(simulation_param['solarSensorAzimuth']))

    logging.info("--------------------------------")
    logging.info("--- Training Data generated  ---")
    logging.info("--------------------------------")


def learn_bv_model(training_file, model_file, regression, norm_file, best_of):
    bv.learnBVModel(training_file, model_file, regression, norm_file, best_of)

    logging.info("Learning BV Model using:")
    logging.info("training file = " + str(training_file))
    logging.info("model file = " + str(model_file))
    logging.info("regression type = " + str(regression))
    logging.info("norm file = " + str(norm_file))

    logging.info("--------------------------------")
    logging.info("---     BV Model learned     ---")
    logging.info("--------------------------------")


def generate_model(product, config_info):

    sim_pars = \
        {'minlai': config_info.minlai, 'maxlai': config_info.maxlai, 'modlai': config_info.modlai,
         'stdlai': config_info.stdlai, 'distlai': config_info.distlai, 'rsrFile': product.rsr_file,
         'outputFile': check_path(config_info.output_file, product), 'solarZenithAngle': product.solar_zenith_angle,
         'sensorZenithAngle': product.sensor_zenith_angle, 'solarSensorAzimuth': product.solar_sensor_azimuth_angle,
         'noisestd': config_info.noisestd}

    if config_info.nb_samples is None or config_info.bv_filename is None:
        raise ValueError("Missing parameters in config file: nbSamples = {} and "
                         "bv_file = {}".format(config_info.nb_samples, config_info.bv_filename))

    # Check if all parameters have been defined for next application execution
    for element in sim_pars:
        if sim_pars[element] is None:
            raise KeyError("Missing parameter for model generation: {}".format(element))

    # Change path if no dirname in original variable
    bv_file = check_path(config_info.bv_filename, product)
    training_file = check_path(config_info.training_file, product)
    model_file = check_path(config_info.model_file, product)

    # Execute APPLICATION --- BVInputVariableGeneration ---
    create_bv_input_vars(bv_file,
                         config_info.nb_samples,
                         sim_pars)

    # Check if all parameters have been defined for next application execution
    if config_info.training_file is None or config_info.invert_bv is None:
        raise ValueError("Missing parameters in config file: trainingFilename = {} and "
                         "invertBV = {}".format(config_info.training_file, config_info.invert_bv))

    # Execute APPLICATION --- ProSailSimulator ---
    generate_train_data(bv_file,
                        sim_pars,
                        training_file,
                        bv.bvindex[config_info.invert_bv],
                        config_info.simulate,
                        config_info.add_angles,
                        config_info.red_index,
                        config_info.nir_index,
                        config_info.nthreads)

    best_of = int(config_info.bestof)
    regression = config_info.regressor
    norm_file = os.path.join(os.environ.get('OTB_RESULT_DIR'), product.name + '_normalization.txt')

    # Check if all parameters have been defined for next application execution
    if best_of is None or regression is None:
        raise ValueError("Missing parameters in config file: best_of = {} and "
                         "regression = {}".format(best_of, regression))

    # Execute APPLICATION --- InverseModelLearning ---
    learn_bv_model(training_file,
                   model_file,
                   regression,
                   norm_file,
                   best_of)
    
    return model_file


class ConfigFile:
    def __init__(self):
        self.vrt_name = '_output_VRT.tif'
        self.nb_samples = None
        self.bv_filename = 'bv-input-vars.txt'
        self.minlai = None
        self.maxlai = None
        self.modlai = None
        self.stdlai = None
        self.distlai = None
        self.useVI = None
        self.training_file = 'training.txt'
        self.invert_bv = None
        self.rsr_file = None
        self.output_file = 'simus.txt'
        self.noisestd = None
        self.bestof = None
        self.regressor = None
        self.model_file = 'model.txt'

        # default values - retirer du config?
        self.simulate = True
        self.add_angles = 0
        self.red_index = 0
        self.nir_index = 0
        self.nthreads = 2

    def import_data_from_file(self, **cfg_attrib):
        for attr_name, attr_value in cfg_attrib.items():
            if attr_name in self.__dict__:
                if isinstance(attr_value, str):
                    setattr(self, attr_name, str(attr_value))
                else:
                    setattr(self, attr_name, attr_value)
            else:
                logging.info("Attribute {} is not used in ConfigFile".format(attr_name))


def choose_parser(xml):
    xml_content = ET.parse(xml)
    metadata_format = xml_content.getroot().tag
    if 'Muscate_Metadata_Document' in metadata_format:
        return 'Muscate_Metadata'
    elif 'Level-2A_User_Product' in metadata_format:
        return 'Sen2Cor_Metadata'


def parse_xml(xml, type_of_xml, path_to_rsr, extract_rsr=True):
    product = None

    if type_of_xml == 'Muscate_Metadata':
        product = Muscate(xml)
    elif type_of_xml == 'Sen2Cor_Metadata':
        product = Sen2Cor(xml)

    product.fill_dataset_identification()
    product.fill_product_characteristics()
    product.fill_geometric_information()
    product.fill_product_organisation()
    product.get_resolutions()

    if extract_rsr:
        product.get_rsr()
        product.write_rsr_file()
    else:
        product.read_rsr_file(path_to_rsr)

    return product


def create_vrt(product, vrt_name, res, option_fre_sre='FRE'):

    if not res:
        output_vrt = os.path.join(os.environ.get('OTB_RESULT_DIR'), product.name + vrt_name)
    else:
        output_vrt = os.path.join(os.environ.get('OTB_RESULT_DIR'), product.name + "_vrt_" + "{}".format(res) +".vrt") 

    img_list = None
    if product.xml_type == 'Muscate_Metadata':
        if option_fre_sre:
            img_list = product.image_band_FRE
        else:
            img_list = product.image_band_SRE
    elif product.xml_type == 'Sen2Cor_Metadata':
        img_list = product.image_band_list
    verify_band_list(img_list, product.selected_band_list)
    if not res:
        res = check_resolution(product.resolutions, product.selected_band_list)
        logging.info("----- Use resolution of {}m -----".format(res))
    else:
        logging.info("----- Use resolution of {}m -----".format(res))

    img_selection = select_band(img_list, product.selected_band_list, res, product.xml_type)

    vrt_options = gdal.BuildVRTOptions(resolution='user', xRes=res, yRes=res, separate=True)
    gdal.BuildVRT(output_vrt, img_selection, options=vrt_options)

    logging.info("Output VRT: {}".format(output_vrt))
    logging.info("--------------------------------")
    logging.info("--- VRT image has been built ---")
    logging.info("--------------------------------")

    return output_vrt


def check_resolution(resolution_list, band_selection):
    res_max = resolution_list[band_selection[-1]]
    res_list = [res_max]
    for band_name in band_selection[:-1]:
        res = resolution_list[band_name]
        res_list.append(res)
        if res < res_max:
            res_max = res

    if len(list(set(res_list))) > 1:
        logging.info("All bands have not same resolution: {}. The highest resolution will be used: {}".
                     format(list(set(res_list)), res_max))
    else:
        logging.info("Single resolution found in selected bands: {}".format(list(set(res_list))))
    return res_max


def select_band(band_path_list, band_selection, res, type_of_xml):
    band_list = []
    bands = []
    for band_name in band_selection:
        if type_of_xml == 'Muscate_Metadata':
            # Only one image per band with specific resolution in image folder,
            # so selection of image according to band_name
            for s in band_path_list:
                if band_name == s[0]:
                    band_list.append(s[1])
        elif type_of_xml == 'Sen2Cor_Metadata':
            # Band with higher resolution have already been resampled in image folder,
            # so use of these resampled images according to user choice
            # if they don't exist in chosen resolution, they will be searched in closest resolution
            for s in band_path_list:
                if band_name == s[0] and str(res) == str(s[1]):
                    band_list.append(s[2])
                    bands.append(s[0])

            if band_name not in bands:
                # B1 and B9 exist only in 60m resolution and B8 only in 10m resolution
                if band_name == 'B1' or band_name == 'B9' or band_name == 'B8':
                    for s in band_path_list:
                        if band_name == s[0]:
                            band_list.append(s[2])
                            bands.append(s[0])
                else: # for B5, B6, B7, B8A, B11, B12
                    if str(res) == '10':
                        for s in band_path_list:
                            if band_name == s[0] and str(s[1]) == '20':
                                band_list.append(s[2])
                                bands.append(s[0])
                    else: # if res = 20
                        for s in band_path_list:
                            if band_name == s[0] and str(s[1]) == '60':
                                band_list.append(s[2])
                                bands.append(s[0])

    if not len(band_list) == len(band_selection):
        raise ValueError("Selected band number ({}) differs from image path selection ({})."
                         .format(len(band_selection), len(band_list)))
    return band_list


def verify_band_list(band_path_list, band_selection):
    for band_name in band_selection:
        band_exist = False
        for s in band_path_list:
            if band_name == s[0]:
                band_exist = True
        if not band_exist:
            raise ValueError("Band {} couldn't be found.".format(band_name))


class Product:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def __init__(self):
        self.xml_path = None
        self.name = None
        self.platform = None
        self.authority = None
        self.solar_zenith_angle = None
        self.sensor_zenith_angle = None
        self.solar_sensor_azimuth_angle = None
        self.band_list = None
        self.band_number = None
        self.image_band_SRE = None
        self.image_band_FRE = None
        self.rsr = None
        self.ref_rsr_file = None
        self.rsr_file = None
        self.resolutions = None
        self.selected_band_list = None

    @abc.abstractmethod
    def get_rsr(self):
        pass

    def filter_rsr_file(self):
        # RSR file will be filtered according to band Selection.
        # Only columns with reflectance values of selected bands will be copied from initial to new RSR file
        # ('filteredRSRFilePath')
        if not os.path.isfile(self.ref_rsr_file):
            raise ValueError("RSR File path is invalid: {}".format(self.ref_rsr_file))

        with open(self.ref_rsr_file, 'r') as ref_rsr:
            reader = csv.reader(ref_rsr, delimiter=' ')
            header = next(reader, None)
            ref_keys = [header[0], header[1]] + self.selected_band_list
            with open(self.rsr_file, 'w+') as new_rsr:
                writer = csv.DictWriter(new_rsr, fieldnames=ref_keys, delimiter=' ')
                for row in reader:
                    res = dict(zip(header, row))
                    values = dict(zip(ref_keys, [res[x] for x in ref_keys]))
                    writer.writerow(values)

    def write_rsr_file(self):
        # Generates RSR File from XML file including:
        #   - wavelengths and solar irradiance in first two columns
        #   - reflectance values (one column per band)
        if self.rsr is None:
            raise ValueError("No RSR values available for storage in CSV")

        val_list = self.rsr['values']
        # to split ordered dict containing list of reflectance values into list of ordered dict (one value, one key)
        res = [dict(zip(val_list, i)) for i in zip(*val_list.values())]

        with open(self.ref_rsr_file, 'w+') as f:
            writer = csv.DictWriter(f, fieldnames=self.rsr['bandLocation'], delimiter=' ')
            writer.writeheader()
            writer.writerows(res)

        if os.path.getsize(self.ref_rsr_file) == 0:
            raise ValueError("RSR file couldn't be generated.")

    def read_rsr_file(self, path_to_rsr):
        # Read a RSR file and extract wavelength, solar irradiance or reflectance values
        if not os.path.isfile(path_to_rsr):
            raise FileNotFoundError("External RSR file is invalid.")

        self.ref_rsr_file = path_to_rsr

        path = os.path.join(os.environ.get('OTB_RESULT_DIR'), str(self.name))
        self.rsr_file = path + '_rsrFileFiltered.rsr'

        self.rsr = dict()

        rsr_values = collections.OrderedDict()

        if self.platform == 'Sentinel-2A' or self.platform == 'SENTINEL2A' or self.platform == 'LANDSAT8':
            with open(path_to_rsr) as rsr:
                reader = csv.reader(rsr, delimiter=' ')
                header = next(reader, None)

                if header[0] != 'Wavelength':
                    header = input("Enter the list of attributes in RSR file : ")

                self.rsr['bandLocation'] = header

            with open(path_to_rsr, 'rU') as rsr:
                reader = csv.DictReader(rsr, fieldnames=self.rsr['bandLocation'], delimiter=' ')

                for r in reader:
                    for k, val in r.items():
                        if k in rsr_values:
                            # append the new number to the existing array at this slot
                            rsr_values[k].append(val)
                        else:
                            # create a new array in this slot
                            rsr_values[k] = []

        self.rsr['values'] = rsr_values


class Muscate(Product):
    def __init__(self, path):
        Product.__init__(self)
        self.xml_path = path
        self.content = ET.parse(self.xml_path)
        self.xml_type = 'Muscate_Metadata'

        self.name = None
        self.authority = None
        self.platform = None
        self.solar_zenith_angle = None
        self.sensor_zenith_angle = None
        self.solar_sensor_azimuth_angle = None
        self.band_list = None
        self.band_number = None
        self.image_band_SRE = None
        self.image_band_FRE = None
        self.rsr = None
        self.ref_rsr_file = None
        self.rsr_file = None
        self.resolutions = None
        self.selected_band_list = None

    def fill_dataset_identification(self):
        info_data = musc.DatasetIdentification(self.content)
        info_data.fill_info()

        self.name = info_data.IDENTIFIER
        self.authority = info_data.AUTHORITY

    def fill_product_characteristics(self):
        product_characterics = musc.ProductCharacteristics(self.content)
        product_characterics.fill_product_info()

        self.platform = product_characterics.PLATFORM
        self.band_number = product_characterics.band_number
        self.band_list = product_characterics.band_global_list

    def fill_geometric_information(self):
        geometric_information = musc.GeometricInformation(self.content)
        geometric_information.fill_info()

        self.solar_zenith_angle = geometric_information.solar_zenith_angle
        self.sensor_zenith_angle = geometric_information.sensor_zenith_angle
        self.solar_sensor_azimuth_angle = geometric_information.solar_sensor_azimuth

    def fill_product_organisation(self):
        product_organisation = musc.ProductOrganisation(self.content, self.xml_path)
        product_organisation.read_image_file_info()

        self.image_band_SRE = product_organisation.imageSRE
        self.image_band_FRE = product_organisation.imageFRE

    def get_rsr(self):
        radiometric_information = musc.RadiometricInformation(self.content, self.name)
        radiometric_information.retrieve_rsr_values()

        self.rsr = radiometric_information.rsr
        self.ref_rsr_file = radiometric_information.rsr_filepath
        self.rsr_file = radiometric_information.filtered_rsr_filepath

    def get_resolutions(self):
        radiometric_information = musc.RadiometricInformation(self.content, self.name)
        self.resolutions = radiometric_information.get_res()


class Sen2Cor(Product):
    def __init__(self, path):
        Product.__init__(self)
        self.xml_path = path
        self.content = ET.parse(self.xml_path)
        self.xml_type = 'Sen2Cor_Metadata'

        self.name = None
        self.authority = None
        self.platform = None
        self.solar_zenith_angle = None
        self.sensor_zenith_angle = None
        self.solar_sensor_azimuth_angle = None
        self.band_list = None
        self.band_number = None
        self.image_band_list = None             # will contain tuple with band_number / resolution / image path
        self.rsr = None
        self.ref_rsr_file = None
        self.rsr_file = None
        self.resolutions = None
        self.selected_band_list = None

    def fill_dataset_identification(self):
        root = self.content.getroot()
        self.name = root[0].find('Product_Info/PRODUCT_URI').text
        self.authority = None                                                               # Equivalent???

    def fill_product_characteristics(self):
        root = self.content.getroot()
        self.platform = root[0].find('Product_Info/Datatake/SPACECRAFT_NAME').text

        # get list and number of bands in xml file
        try:
            band_list = []
            for band in root[0].findall('Product_Image_Characteristics/Spectral_Information_List/Spectral_Information'):
                band_list.append(band.attrib.get('physicalBand'))
            setattr(self, "bandList", band_list)
            setattr(self, "bandNumber", len(band_list))
        except TypeError:
            logging.info("No band id list found - Product characteristics")

    def fill_geometric_information(self):
        geometric_information = sen2cor.GeometricInformation(self.xml_path)
        geometric_information.fill_info()

        self.solar_zenith_angle = geometric_information.solar_zenith_angle
        self.sensor_zenith_angle = geometric_information.sensor_zenith_angle
        self.solar_sensor_azimuth_angle = geometric_information.solar_sensor_azimuth

    def fill_product_organisation(self):
        root = self.content.getroot()
        img_file_info = root[0].find('Product_Info/Product_Organisation/Granule_List/Granule')

        image_band_list = []
        image_extend_list = []
        for file in img_file_info.findall('IMAGE_FILE'):
            img_format = img_file_info.attrib.get('imageFormat')
            if img_format == 'JPEG2000':
                if '_B' in file.text:
                    try:
                        found = re.search('_B(.+?)_', file.text).group(1)
                        found_res = file.text.split('_')[-1][:-1]
                        if 'A' in found:
                            band = 'B' + str(int(found[:-1])) + 'A'
                        else:
                            band = 'B' + str(int(found))
                    except AttributeError:
                        raise AttributeError("No band number found in image path list.")

                    image_band_list.append((band, int(found_res),
                                            os.path.join(os.path.dirname(self.xml_path), file.text + '.jp2')))
                else:
                    image_extend_list.append(os.path.join(os.path.dirname(self.xml_path), file.text + '.jp2'))
            else:
                raise ValueError("Image format is different than jp2")
        self.image_band_list = image_band_list

    def get_rsr(self):
        radiometric_information = sen2cor.RadiometricInformation(self.content, self.name)
        radiometric_information.retrieve_rsr_values()

        self.rsr = radiometric_information.rsr
        self.ref_rsr_file = radiometric_information.rsr_filepath
        self.rsr_file = radiometric_information.filtered_rsr_filepath

    def get_resolutions(self):
        radiometric_information = sen2cor.RadiometricInformation(self.content, self.name)
        self.resolutions = radiometric_information.get_res()


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('-xml', dest='xmlFile', required=True, help="Path to XML File")
    parser.add_argument('-outLAI', dest='outputLAI', required=True,
                        help="Name of LAI image output file - will be stored in RESULT folder")
    parser.add_argument('-json', dest='jsonFile', required=True, help="JSON File with user config info")
    parser.add_argument('-extractRSR', dest='extractRSRfromXML', required=False, action='store_true',
                        help="Extraction of spectral responses in XML file and creation of a RSR file")
    parser.add_argument('-rsrPath', dest='rsrPath', required='-extractRSR' not in sys.argv,
                        help="Path to external RSR file if no RSR file generation using XML file")
    parser.add_argument('-optFRE_SRE', dest='optionFRE_SRE', choices=['FRE', 'SRE'], default='FRE', required=False,
                        help="Choice for image band creation -reflectance image with or without slope correction "
                             "(FRE/SRE) - only for Theia - Default is FRE")
    parser.add_argument('-optBandList', dest='optionBandList', nargs='*', type=str, required=True,
                        help="Band list for VRT image creation - bands to integrate")
    parser.add_argument('--log-level', default='INFO', dest='log_level', type=_log_level_name_to_nb, nargs='?',
                        help='Set the logging output level. {0}'.format(LEVELS))
    parser.add_argument('-res', dest='resolution', required=False, choices=['10', '20', '60'],
                        help="Default configuration will take the highest resolution. "
                             "Choosing this argument will fix resolution at specified value.")
    args = parser.parse_args()

    logging.basicConfig(format='[%(levelname)s]:  %(message)s', level=logging.DEBUG)

    if not os.path.isfile(args.xmlFile):
        raise ValueError('ERROR: XML File is invalid: {}'.format(args.xmlFile))
    if not os.path.exists(args.jsonFile):
        raise ValueError('ERROR: Configuration JSON File path is invalid: {}.'.format(args.jsonFile))

    # ----------     IMPORT JSON FILE     ---------- #

    cfg = ConfigFile()
    cfg.import_data_from_file(**json.load(open(args.jsonFile)))

    # ----------      IMPORT XML DATA     ---------- #
    xml_type = choose_parser(args.xmlFile)
    logging.info("Choice for Parser --- {}".format(xml_type))

    prod = parse_xml(args.xmlFile, xml_type, args.rsrPath, args.extractRSRfromXML)

    # ----------  BAND SELECTION CHOICE  ---------- #
    if args.optionBandList is None:
        raise ValueError("No bands have been selected by user.")
    prod.selected_band_list = args.optionBandList

    # ----------  ADAPTATION OF RSR FILE  ---------- #
    prod.filter_rsr_file()

    # ----------    VRT IMAGE CREATION    ---------- #
    image_vrt = create_vrt(prod, cfg.vrt_name, args.resolution)

    # ----------     MODEL GENERATION     ---------- #
    model = generate_model(prod, cfg)

    # ----------   LAI IMAGE GENERATION   ---------- #
    image_LAI = prod.name + args.outputLAI
    inv_image(image_vrt, model, os.path.join(os.environ.get('OTB_RESULT_DIR'), image_LAI))

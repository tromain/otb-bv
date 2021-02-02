#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================
#   Program:   muscateXMLParser
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

import collections
import logging
import math
import numpy as np
import os


class Angle(object):
    def __init__(self, band_id, z_value, a_value, unit='deg'):
        self.band_id = band_id
        self.zenith_value = z_value
        self.azimuth_value = a_value
        self.unit = unit


class ProductCharacteristics:
    def __init__(self, xml_content):
        self.xml_content = xml_content
        self.PRODUCT_LEVEL = None
        self.PLATFORM = None
        self.band_global_list = None
        self.band_number = None

    def fill_product_info(self):
        product_info = self.xml_content.find('Product_Characteristics')

        # get info about PRODUCT_LEVEL and PLATFORM
        try:
            for child in product_info:
                if child.tag in self.__dict__:
                    setattr(self, child.tag, child.text)
        except TypeError:
            raise ValueError("Name of element in XML file incorrect")

        # get list and number of bands in xml file
        try:
            band_list = []
            for band in product_info.findall('Band_Global_List/BAND_ID'):
                band_list.append(band.text)
            setattr(self, "bandGlobalList", band_list)
            setattr(self, "bandNumber", len(band_list))
        except TypeError:
            logging.info("No band id list found - Product characteristics")


class DatasetIdentification:
    def __init__(self, xml_content):
        self.IDENTIFIER = None
        self.AUTHORITY = None
        self.xml_content = xml_content

    def fill_info(self):
        data_info = self.xml_content.find('Dataset_Identification')
        try:
            for child in data_info:
                if child.tag in self.__dict__:
                    setattr(self, child.tag, child.text)
        except TypeError:
            raise ValueError("Name of element in XML file incorrect")


class GeometricInformation:
    def __init__(self, xml_content):
        self.xml_content = xml_content
        self.solar_zenith_angle = None
        self.solar_azimuth_angle = None
        self.sensor_zenith_angle = None
        self.sensor_azimuth_angle = None
        self.solar_sensor_azimuth = None

    def fill_info(self):
        geom_info = self.xml_content.find('Geometric_Informations')

        # Acquisition angle retrieval (solar_zenith_angle, sensor_zenith_angle and solar_sensor_azimuth)
        sun_angles = geom_info.find('Mean_Value_List/Sun_Angles')
        if list(sun_angles)[0].tag == 'ZENITH_ANGLE' and list(sun_angles)[1].tag == 'AZIMUTH_ANGLE' \
                and len(list(sun_angles)) == 2:
            self.solar_zenith_angle = math.floor(float(sun_angles.find('ZENITH_ANGLE').text))
            # THETA_S in Sen2Agri <=> getSolarMeanAngles().zenith
            self.solar_azimuth_angle = math.floor(float(sun_angles.find('AZIMUTH_ANGLE').text))
        else:
            logging.info("Xml file has a different structure: solarAngles initialized to 0")
            self.solar_zenith_angle = 0
            self.solar_azimuth_angle = 0

        prodname = self.xml_content.find('Product_Characteristics/PLATFORM').text
        if "SENTINEL2A" in prodname :
            sensor_angles = geom_info.find('Mean_Value_List/Mean_Viewing_Incidence_Angle_List')
        elif "LANDSAT8" in prodname :
            sensor_angles = geom_info.find('Mean_Value_List/Incidence_Angles')
        elif "VENUS" in prodname :
            sensor_angles = geom_info.find('Mean_Value_List/Mean_Viewing_Incidence_Angle_List')
        else :
            logging.ingo("XML file does not contain sensor angles information")

        mean_angle_list = []
        nb_zenith = 0.0
        nb_azimuth = 0.0
        mean_zenith = 0.0
        mean_azimuth = 0.0
        for a in range(0, len(list(sensor_angles))):
            mean_angle = list(sensor_angles)[a]
            if "SENTINEL2A" in prodname:
                mean_angle_list.append(
                    Angle(mean_angle.attrib.get('band_id'), float(mean_angle.find('ZENITH_ANGLE').text),
                        float(mean_angle.find('AZIMUTH_ANGLE').text), mean_angle.attrib.get('unit')))
            elif "LANDSAT8" in prodname:
                mean_angle_list.append(Angle("B1",0.0,0.0,"deg"));
            elif "VENUS" in prodname:
                mean_angle_list.append(
                    Angle(mean_angle.attrib.get('detector_id'), float(mean_angle.find('ZENITH_ANGLE').text),
                        float(mean_angle.find('AZIMUTH_ANGLE').text), mean_angle.attrib.get('unit')))

            if not np.isnan(mean_angle_list[a].zenith_value):
                nb_zenith += 1.0
                mean_zenith += mean_angle_list[a].zenith_value
            else:
                nb_zenith = 1.0

            if not np.isnan(mean_angle_list[a].azimuth_value):
                nb_azimuth += 1.0
                mean_azimuth += mean_angle_list[a].azimuth_value
            else:
                nb_azimuth = 1.0

        self.sensor_zenith_angle = math.floor(mean_zenith / nb_zenith)
        # THETA_V in S2A <=> getSensorMeanAngles().zenith
        self.sensor_azimuth_angle = math.floor(mean_azimuth / nb_azimuth)

        # Extraction solar_sensor_azimuth <=> getRelativeAzimuthAngle() (diff between solar.azimuth et sensor.azimuth)
        self.solar_sensor_azimuth = self.solar_azimuth_angle - self.sensor_azimuth_angle  # REL_PHI in S2A


class RadiometricInformation:
    def __init__(self, xml_content, img_name):
        self.xml_content = xml_content
        self.img_name = img_name
        self.rsr = None
        self.rsr_filepath = None
        self.filtered_rsr_filepath = None
        self.refl_quantif_value = None
        self.wvp_quantif_value = None
        self.aot_quantif_value = None

    def get_res(self):
        radio_info = self.xml_content.find('Radiometric_Informations')
        reflc_info = radio_info.findall('Spectral_Band_Informations_List/Spectral_Band_Informations')
        resolution_list = {}

        for n in range(0, len(list(reflc_info))):
            band = list(reflc_info)[n]
            resolution_list[band.attrib.get('band_id')] = int(band.find('SPATIAL_RESOLUTION').text)

        return resolution_list

    def retrieve_rsr_values(self):
        radio_info = self.xml_content.find('Radiometric_Informations')
        reflc_info = radio_info.findall('Spectral_Band_Informations_List/Spectral_Band_Informations')

        self.refl_quantif_value = float(radio_info.find('REFLECTANCE_QUANTIFICATION_VALUE').text)
        self.wvp_quantif_value = float(radio_info.find('WATER_VAPOR_CONTENT_QUANTIFICATION_VALUE').text)
        self.aot_quantif_value = float(radio_info.find('AEROSOL_OPTICAL_THICKNESS_QUANTIFICATION_VALUE').text)

        wavelength_list = []
        content_list = ['Wavelength', 'SolarIrradiance']
        for n in range(0, len(list(reflc_info))):
            band = list(reflc_info)[n]
            content_list.append(band.attrib.get('band_id'))
            step = int(band.find('Spectral_Response/STEP').text)
            wavelength_range = np.arange(int(band.find('Wavelength/MIN').text),
                                         int(band.find('Wavelength/MAX').text) + step,
                                         step)
            for w in wavelength_range:
                if w not in wavelength_list:
                    wavelength_list.append(w)

        # Unit conversion --> 1000
        wavelength_list = [(a / 1000.0) for a in wavelength_list]

        rsr_values = collections.OrderedDict()
        for content in content_list:
            if content == 'Wavelength':
                rsr_values[content] = wavelength_list
            else:
                rsr_values[content] = [0, ] * len(wavelength_list)

        # Attention: solarIrradiance est une moyenne si plusieurs bandes se chevauchent
        for r in range(0, len(list(reflc_info))):
            band = list(reflc_info)[r]
            step = int(band.find('Spectral_Response/STEP').text)
            wavelength_part_list = np.arange(int(band.find('Wavelength/MIN').text),
                                             int(band.find('Wavelength/MAX').text) + step,
                                             step)
            # Unit conversion --> 1000
            wavelength_part_list = [(a / 1000.0) for a in wavelength_part_list]
            reflc_values = [float(x) for x in band.find('Spectral_Response/VALUES').text.split()]

            assert len(wavelength_part_list) == len(reflc_values)

            for n in range(0, len(wavelength_part_list)):
                ind = rsr_values['Wavelength'].index(wavelength_part_list[n])
                band_name = band.attrib.get('band_id')
                # Les images SRE et FRE sont codees sur 16 bits et
                # il faut diviser par 10000 pour obtenir des reflectances de surfaces
                #  http://www.cesbio.ups-tlse.fr/multitemp/?page_id=8352
                if str(band_name) == 'B1':
                    # La bande 1 correspond a la vapeur d eau il faut diviser les valeurs par 20 pour obtenir des g/cm2
                    rsr_values[str(band_name)][ind] = reflc_values[n] / (
                            self.wvp_quantif_value * self.refl_quantif_value)
                elif str(band_name) == 'B2':
                    # La bande 2 correspond a l epaisseur optique des aerosols
                    # il faut diviser les valeurs par 200 pour obtenir l epaisseur optique
                    rsr_values[str(band_name)][ind] = reflc_values[n] / (
                                self.aot_quantif_value * self.refl_quantif_value)
                else:
                    rsr_values[str(band_name)][ind] = reflc_values[n] / self.refl_quantif_value

                if rsr_values['SolarIrradiance'][ind] == 0:
                    rsr_values['SolarIrradiance'][ind] = float(band.find('SOLAR_IRRADIANCE').text)
                else:
                    tmp = rsr_values['SolarIrradiance'][ind]
                    rsr_values['SolarIrradiance'][ind] = (tmp + float(band.find('SOLAR_IRRADIANCE').text)) / 2.0

        # retrieve values in xml
        path = os.path.join(os.environ.get('OTB_RESULT_DIR'), str(self.img_name))
        self.rsr = dict()
        self.rsr['values'] = rsr_values
        self.rsr['bandLocation'] = content_list
        self.rsr_filepath = path + '_rsrFile.rsr'
        self.filtered_rsr_filepath = path + '_rsrFileFiltered.rsr'


class ProductOrganisation:
    def __init__(self, xml_content, xml_path):
        self.imageSRE = None
        self.imageFRE = None
        self.xml_content = xml_content
        self.xml_path = xml_path
        self.option_nature = None

    def read_image_file_info(self):
        product_orga = self.xml_content.find('Product_Organisation/Muscate_Product/Image_List')
        image_dir = os.path.dirname(self.xml_path)

        self.option_nature = []
        for n in product_orga.findall('Image'):
            img_type = n.find('Image_Properties/NATURE').text
            if img_type is not None:
                self.option_nature.append(img_type)
                if img_type == 'Flat_Reflectance':
                    self.imageFRE = [[img.attrib.get('band_id'), os.path.join(image_dir, img.text)]
                                     for img in n.findall('Image_File_List/IMAGE_FILE')]
                elif img_type == 'Surface_Reflectance':
                    self.imageSRE = [[img.attrib.get('band_id'), os.path.join(image_dir, img.text)]
                                     for img in n.findall('Image_File_List/IMAGE_FILE')]
#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =========================================================================
#   Program:   otb-bv
#   Language:  python
#
#   Copyright (c) CESBIO. All rights reserved.
#
#   See otb-bv-copyright.txt for details.
#
#   This software is distributed WITHOUT ANY WARRANTY; without even
#   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#   PURPOSE.  See the above copyright notices for more information.
#
# =========================================================================

import os


gt = [{'refls': [764, 766, 766, 766, 768, 325, 341], 'doy': '20160704', 'gai': 5.165, 'y': 5545201.199464054, 'x': 729295.1927849087, 'id': 1, 'lai-bvnet': 0},
{'refls': [763, 764, 765, 765, 766, 256, 269], 'doy': '20160704', 'gai': 4.88, 'y': 5541823.40189604, 'x': 725590.8413002626, 'id': 2, 'lai-bvnet': 0},
{'refls': [758, 759, 759, 759, 760, 275, 283], 'doy': '20160704', 'gai': 5.045, 'y': 5541532.333360561, 'x': 726090.2393360165, 'id': 3, 'lai-bvnet': 0},
{'refls': [759, 760, 760, 760, 761, 206, 226], 'doy': '20160704', 'gai': 5.58, 'y': 5541345.99856374, 'x': 726663.6935339964, 'id': 4, 'lai-bvnet': 0},
{'refls': [764, 765, 766, 766, 768, 277, 277], 'doy': '20160704', 'gai': 7.31, 'y': 5541126.259752877, 'x': 727992.5352762659, 'id': 5, 'lai-bvnet': 0},
{'refls': [761, 763, 763, 763, 765, 365, 369], 'doy': '20160704', 'gai': 0.001, 'y': 5544480.229855748, 'x': 729561.8576944787, 'id': 6, 'lai-bvnet': 0},
{'refls': [764, 765, 766, 766, 768, 245, 260], 'doy': '20160704', 'gai': 4.73, 'y': 5544161.371452251, 'x': 732315.7834693231, 'id': 7, 'lai-bvnet': 0},
{'refls': [762, 763, 763, 763, 765, 217, 232], 'doy': '20160704', 'gai': 0.001, 'y': 5544415.770980604, 'x': 732401.6105254542, 'id': 8, 'lai-bvnet': 0},
{'refls': [763, 764, 763, 763, 763, 312, 295], 'doy': '20160704', 'gai': 4.705, 'y': 5551534.915029728, 'x': 730902.8712169443, 'id': 9, 'lai-bvnet': 0},
{'refls': [770, 770, 769, 767, 766, 195, 224], 'doy': '20160704', 'gai': 4.44, 'y': 5551935.27030161, 'x': 727708.8201671492, 'id': 10, 'lai-bvnet': 0},
{'refls': [770, 770, 768, 767, 766, 198, 227], 'doy': '20160704', 'gai': 5.235, 'y': 5551792.215320655, 'x': 727693.8538021705, 'id': 11, 'lai-bvnet': 0},
{'refls': [766, 768, 769, 770, 772, 343, 335], 'doy': '20160704', 'gai': 4.825, 'y': 5549548.0746176895, 'x': 728731.3206053195, 'id': 12, 'lai-bvnet': 0},
{'refls': [762, 763, 764, 764, 765, 245, 212], 'doy': '20160704', 'gai': 4.825, 'y': 5545777.452207957, 'x': 728207.1147012842, 'id': 13, 'lai-bvnet': 0},
{'refls': [763, 765, 765, 765, 767, 364, 381], 'doy': '20160704', 'gai': 4.765, 'y': 5544955.30473116, 'x': 729247.6036986785, 'id': 14, 'lai-bvnet': 0},
{'refls': [761, 763, 763, 763, 765, 235, 234], 'doy': '20160704', 'gai': 6.04, 'y': 5541076.756621709, 'x': 727490.5550852425, 'id': 15, 'lai-bvnet': 0},
{'refls': [762, 764, 764, 764, 766, 252, 253], 'doy': '20160704', 'gai': 5.75, 'y': 5541487.865950403, 'x': 727543.9323232363, 'id': 16, 'lai-bvnet': 0},
{'refls': [758, 759, 759, 759, 760, 309, 326], 'doy': '20160704', 'gai': 5.76, 'y': 5541790.834567792, 'x': 725966.5340922098, 'id': 17, 'lai-bvnet': 0},
{'refls': [762, 764, 764, 764, 766, 226, 238], 'doy': '20160704', 'gai': 5.135, 'y': 5541491.4285816075, 'x': 726730.6610364506, 'id': 18, 'lai-bvnet': 0},
{'refls': [758, 760, 759, 759, 760, 327, 285], 'doy': '20160704', 'gai': 3.355, 'y': 5545503.3233427545, 'x': 728346.3661142786, 'id': 19, 'lai-bvnet': 0},
{'refls': [764, 766, 766, 766, 768, 343, 354], 'doy': '20160704', 'gai': 5.6, 'y': 5541824.638946047, 'x': 725815.2658355884, 'id': 20, 'lai-bvnet': 0},
]
l8_nopan_ukr_rsr = "landsat8_nopan.rsr"
l8_nopan_ukr_data = ["l8_nopan_ukr", l8_nopan_ukr_rsr]
l8_nopan_ukr_20160704 = {'doy' : 20160704, 'to' : 0.0, 'po' : 0.0,   'ts' :30.886192590, 'ps' :  146.117090130, 'gt':[s for s in gt if s['doy']=='20160704']}
l8_nopan_ukr_data.append(l8_nopan_ukr_20160704)

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


gt = [{'refls': [ 83, 104, 383, 253], 'doy': '20160906', 'gai': 0.25135, 'y': 4812808.0, 'x': 390624.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 62, 57, 417, 190], 'doy': '20160906', 'gai': 0.309841, 'y': 4812728.0, 'x': 390753.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 96, 122, 384, 273], 'doy': '20160906', 'gai': 0.059306, 'y': 4812088.0, 'x': 390424.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 106, 145, 348, 294], 'doy': '20160906', 'gai': 0.028788, 'y': 4812168.0, 'x': 390294.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 106, 169, 324, 325], 'doy': '20160906', 'gai': 0.01, 'y': 4812238.0, 'x': 390424.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 381, 396, 638, 520], 'doy': '20160406', 'gai': 1.711211, 'y': 4817488.0, 'x': 357473.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 344, 352, 602, 486], 'doy': '20160406', 'gai': 1.848123, 'y': 4817468.0, 'x': 357513.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 359, 380, 625, 500], 'doy': '20160406', 'gai': 1.683691, 'y': 4817438.0, 'x': 357553.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 322, 327, 573, 461], 'doy': '20160406', 'gai': 1.487609, 'y': 4817407.0, 'x': 357583.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 44, 21, 364, 144], 'doy': '20160117', 'gai': 0.962268, 'y': 4817488.0, 'x': 357473.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 42, 19, 387, 142], 'doy': '20160117', 'gai': 1.015339, 'y': 4817468.0, 'x': 357513.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 51, 34, 345, 154], 'doy': '20160117', 'gai': 0.973965, 'y': 4817438.0, 'x': 357553.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 43, 20, 382, 147], 'doy': '20160117', 'gai': 0.969573, 'y': 4817407.0, 'x': 357583.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 333, 322, 464, 279], 'doy': '20160524', 'gai': 0.052367, 'y': 4823428.0, 'x': 345573.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 334, 321, 464, 284], 'doy': '20160524', 'gai': 0.036315, 'y': 4823438.0, 'x': 345633.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 310, 302, 449, 279], 'doy': '20160524', 'gai': 0.07368, 'y': 4823558.0, 'x': 345523.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 244, 245, 403, 248], 'doy': '20160524', 'gai': 0.050289, 'y': 4823638.0, 'x': 345473.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 276, 267, 433, 260], 'doy': '20160524', 'gai': 0.050289, 'y': 4823738.0, 'x': 345423.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 269, 262, 429, 253], 'doy': '20160524', 'gai': 0.039145, 'y': 4823788.0, 'x': 345453.0, 'id': 6, 'lai-bvnet': 0},
{'refls': [ 262, 257, 425, 250], 'doy': '20160524', 'gai': 0.017993, 'y': 4823838.0, 'x': 345483.0, 'id': 7, 'lai-bvnet': 0},
{'refls': [ 257, 251, 422, 251], 'doy': '20160524', 'gai': 0.011688, 'y': 4823888.0, 'x': 345502.0, 'id': 8, 'lai-bvnet': 0},
{'refls': [ 257, 245, 429, 253], 'doy': '20160524', 'gai': 0.025957, 'y': 4823928.0, 'x': 345523.0, 'id': 9, 'lai-bvnet': 0},
{'refls': [ 134, 142, 298, 263], 'doy': '20160609', 'gai': 0.067153, 'y': 4812628.0, 'x': 329229.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 164, 181, 330, 303], 'doy': '20160609', 'gai': 0.145417, 'y': 4812808.0, 'x': 329279.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 63, 31, 583, 218], 'doy': '20160704', 'gai': 1.098692, 'y': 4823428.0, 'x': 345573.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 64, 33, 574, 215], 'doy': '20160704', 'gai': 1.151923, 'y': 4823438.0, 'x': 345633.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 69, 43, 512, 212], 'doy': '20160704', 'gai': 1.078525, 'y': 4823558.0, 'x': 345523.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 79, 70, 526, 235], 'doy': '20160704', 'gai': 1.496644, 'y': 4823638.0, 'x': 345473.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 69, 42, 551, 224], 'doy': '20160704', 'gai': 1.732035, 'y': 4823738.0, 'x': 345423.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 63, 33, 564, 221], 'doy': '20160704', 'gai': 1.783037, 'y': 4823788.0, 'x': 345453.0, 'id': 6, 'lai-bvnet': 0},
{'refls': [ 78, 52, 509, 230], 'doy': '20160704', 'gai': 0.567612, 'y': 4823838.0, 'x': 345483.0, 'id': 7, 'lai-bvnet': 0},
{'refls': [ 73, 53, 486, 213], 'doy': '20160704', 'gai': 1.001221, 'y': 4823888.0, 'x': 345502.0, 'id': 8, 'lai-bvnet': 0},
{'refls': [ 80, 44, 545, 233], 'doy': '20160704', 'gai': 0.812528, 'y': 4804531.0, 'x': 389751.0, 'id': 9, 'lai-bvnet': 0},
{'refls': [ 82, 45, 518, 230], 'doy': '20160704', 'gai': 0.942293, 'y': 4804500.0, 'x': 389791.0, 'id': 10, 'lai-bvnet': 0},
{'refls': [ 58, 51, 355, 225], 'doy': '20160704', 'gai': 0.672227, 'y': 4812808.0, 'x': 390624.0, 'id': 11, 'lai-bvnet': 0},
{'refls': [ 59, 53, 305, 208], 'doy': '20160704', 'gai': 0.731974, 'y': 4812728.0, 'x': 390753.0, 'id': 12, 'lai-bvnet': 0},
{'refls': [ 84, 86, 310, 264], 'doy': '20160704', 'gai': 0.467679, 'y': 4812088.0, 'x': 390424.0, 'id': 13, 'lai-bvnet': 0},
{'refls': [ 85, 88, 339, 257], 'doy': '20160704', 'gai': 0.668571, 'y': 4812168.0, 'x': 390294.0, 'id': 14, 'lai-bvnet': 0},
{'refls': [ 50, 43, 369, 191], 'doy': '20160704', 'gai': 1.566348, 'y': 4812238.0, 'x': 390424.0, 'id': 15, 'lai-bvnet': 0},
{'refls': [ 63, 48, 373, 211], 'doy': '20160704', 'gai': 1.047823, 'y': 4821706.0, 'x': 328127.0, 'id': 16, 'lai-bvnet': 0},
{'refls': [ 51, 36, 348, 164], 'doy': '20160704', 'gai': 1.12646, 'y': 4821616.0, 'x': 328797.0, 'id': 17, 'lai-bvnet': 0},
{'refls': [ 66, 65, 306, 255], 'doy': '20160704', 'gai': 0.798351, 'y': 4812628.0, 'x': 329229.0, 'id': 18, 'lai-bvnet': 0},
{'refls': [ 773, 769, 764, 696], 'doy': '20160711', 'gai': 1.098692, 'y': 4823428.0, 'x': 345573.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 778, 774, 769, 713], 'doy': '20160711', 'gai': 1.151923, 'y': 4823438.0, 'x': 345633.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 772, 768, 763, 703], 'doy': '20160711', 'gai': 1.078525, 'y': 4823558.0, 'x': 345523.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 776, 772, 767, 675], 'doy': '20160711', 'gai': 1.496644, 'y': 4823638.0, 'x': 345473.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 720, 727, 769, 581], 'doy': '20160711', 'gai': 1.732035, 'y': 4823738.0, 'x': 345423.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 719, 728, 768, 564], 'doy': '20160711', 'gai': 1.783037, 'y': 4823788.0, 'x': 345453.0, 'id': 6, 'lai-bvnet': 0},
{'refls': [ 719, 731, 761, 564], 'doy': '20160711', 'gai': 0.567612, 'y': 4823838.0, 'x': 345483.0, 'id': 7, 'lai-bvnet': 0},
{'refls': [ 739, 759, 765, 580], 'doy': '20160711', 'gai': 1.001221, 'y': 4823888.0, 'x': 345502.0, 'id': 8, 'lai-bvnet': 0},
{'refls': [ 631, 635, 668, 338], 'doy': '20160711', 'gai': 1.047823, 'y': 4821706.0, 'x': 328127.0, 'id': 9, 'lai-bvnet': 0},
{'refls': [ 664, 668, 690, 496], 'doy': '20160711', 'gai': 1.12646, 'y': 4821616.0, 'x': 328797.0, 'id': 10, 'lai-bvnet': 0},
{'refls': [ 751, 754, 768, 560], 'doy': '20160711', 'gai': 0.798351, 'y': 4812628.0, 'x': 329229.0, 'id': 11, 'lai-bvnet': 0},
{'refls': [ 213, 225, 298, 215], 'doy': '20160720', 'gai': 0.932315, 'y': 4823428.0, 'x': 345573.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 278, 287, 352, 256], 'doy': '20160720', 'gai': 0.996231, 'y': 4823438.0, 'x': 345633.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 218, 225, 299, 220], 'doy': '20160720', 'gai': 1.053154, 'y': 4823558.0, 'x': 345523.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 224, 231, 304, 222], 'doy': '20160720', 'gai': 1.147738, 'y': 4823638.0, 'x': 345473.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 222, 229, 301, 217], 'doy': '20160720', 'gai': 1.28267, 'y': 4823738.0, 'x': 345423.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 217, 226, 300, 216], 'doy': '20160720', 'gai': 1.181202, 'y': 4823788.0, 'x': 345453.0, 'id': 6, 'lai-bvnet': 0},
{'refls': [ 228, 236, 309, 223], 'doy': '20160720', 'gai': 0.903467, 'y': 4823838.0, 'x': 345483.0, 'id': 7, 'lai-bvnet': 0},
{'refls': [ 223, 233, 307, 221], 'doy': '20160720', 'gai': 1.195096, 'y': 4823888.0, 'x': 345502.0, 'id': 8, 'lai-bvnet': 0},
{'refls': [ 100, 83, 492, 216], 'doy': '20160720', 'gai': 1.195096, 'y': 4804531.0, 'x': 389751.0, 'id': 9, 'lai-bvnet': 0},
{'refls': [ 83, 63, 485, 181], 'doy': '20160720', 'gai': 1.404994, 'y': 4804201.0, 'x': 390451.0, 'id': 10, 'lai-bvnet': 0},
{'refls': [ 87, 68, 495, 192], 'doy': '20160720', 'gai': 1.544371, 'y': 4804201.0, 'x': 390501.0, 'id': 11, 'lai-bvnet': 0},
{'refls': [ 95, 78, 475, 207], 'doy': '20160720', 'gai': 1.349206, 'y': 4804500.0, 'x': 389791.0, 'id': 12, 'lai-bvnet': 0},
{'refls': [ 140, 139, 326, 239], 'doy': '20160821', 'gai': 0.563069, 'y': 4804531.0, 'x': 389751.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 146, 103, 536, 216], 'doy': '20160821', 'gai': 1.313977, 'y': 4804201.0, 'x': 390451.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 139, 92, 552, 205], 'doy': '20160821', 'gai': 1.300527, 'y': 4804201.0, 'x': 390501.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 149, 155, 319, 253], 'doy': '20160821', 'gai': 0.499839, 'y': 4804500.0, 'x': 389791.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 116, 101, 287, 164], 'doy': '20160812', 'gai': 0.695356, 'y': 4823428.0, 'x': 345573.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 115, 106, 280, 173], 'doy': '20160812', 'gai': 0.374307, 'y': 4823438.0, 'x': 345633.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 104, 80, 344, 160], 'doy': '20160812', 'gai': 0.622713, 'y': 4823558.0, 'x': 345523.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 118, 112, 292, 186], 'doy': '20160812', 'gai': 0.450487, 'y': 4823738.0, 'x': 345423.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 127, 126, 289, 204], 'doy': '20160812', 'gai': 0.542722, 'y': 4823788.0, 'x': 345453.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 162, 173, 312, 260], 'doy': '20160812', 'gai': 0.245101, 'y': 4823838.0, 'x': 345483.0, 'id': 6, 'lai-bvnet': 0},
{'refls': [ 120, 111, 297, 182], 'doy': '20160812', 'gai': 0.484723, 'y': 4823888.0, 'x': 345502.0, 'id': 7, 'lai-bvnet': 0},
{'refls': [ 50, 38, 387, 161], 'doy': '20160812', 'gai': 0.623414, 'y': 4821706.0, 'x': 328127.0, 'id': 8, 'lai-bvnet': 0},
{'refls': [ 58, 50, 396, 181], 'doy': '20160812', 'gai': 0.596712, 'y': 4821616.0, 'x': 328797.0, 'id': 9, 'lai-bvnet': 0},
{'refls': [ 52, 37, 423, 176], 'doy': '20160812', 'gai': 0.561391, 'y': 4812808.0, 'x': 329279.0, 'id': 10, 'lai-bvnet': 0},
{'refls': [ 340, 337, 468, 151], 'doy': '20160618', 'gai': 1.403994, 'y': 4823428.0, 'x': 345573.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 336, 331, 455, 152], 'doy': '20160618', 'gai': 0.878269, 'y': 4823438.0, 'x': 345633.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 343, 340, 453, 151], 'doy': '20160618', 'gai': 0.957286, 'y': 4823558.0, 'x': 345523.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 331, 329, 453, 151], 'doy': '20160618', 'gai': 1.099975, 'y': 4823638.0, 'x': 345473.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 306, 300, 436, 144], 'doy': '20160618', 'gai': 0.897058, 'y': 4823738.0, 'x': 345423.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 282, 286, 415, 128], 'doy': '20160618', 'gai': 0.724873, 'y': 4823788.0, 'x': 345453.0, 'id': 6, 'lai-bvnet': 0},
{'refls': [ 254, 250, 376, 123], 'doy': '20160618', 'gai': 0.621471, 'y': 4823838.0, 'x': 345483.0, 'id': 7, 'lai-bvnet': 0},
{'refls': [ 267, 263, 386, 124], 'doy': '20160618', 'gai': 0.597494, 'y': 4823888.0, 'x': 345502.0, 'id': 8, 'lai-bvnet': 0},
{'refls': [ 274, 271, 396, 127], 'doy': '20160618', 'gai': 0.758543, 'y': 4823928.0, 'x': 345523.0, 'id': 9, 'lai-bvnet': 0},
{'refls': [ 40, 13, 399, 125], 'doy': '20160805', 'gai': 1.087803, 'y': 4804531.0, 'x': 389751.0, 'id': 1, 'lai-bvnet': 0},
{'refls': [ 51, 11, 487, 146], 'doy': '20160805', 'gai': 1.247657, 'y': 4804201.0, 'x': 390451.0, 'id': 2, 'lai-bvnet': 0},
{'refls': [ 36, 3, 386, 111], 'doy': '20160805', 'gai': 1.445, 'y': 4804201.0, 'x': 390501.0, 'id': 3, 'lai-bvnet': 0},
{'refls': [ 39, 14, 383, 119], 'doy': '20160805', 'gai': 1.262547, 'y': 4804500.0, 'x': 389791.0, 'id': 4, 'lai-bvnet': 0},
{'refls': [ 81, 83, 222, 160], 'doy': '20160805', 'gai': 0.89426, 'y': 4812808.0, 'x': 390624.0, 'id': 5, 'lai-bvnet': 0},
{'refls': [ 0, 0, 129, 29], 'doy': '20160805', 'gai': 0.927358, 'y': 4812728.0, 'x': 390753.0, 'id': 6, 'lai-bvnet': 0},
{'refls': [ 0, 0, 144, 44], 'doy': '20160805', 'gai': 0.942755, 'y': 4812088.0, 'x': 390424.0, 'id': 7, 'lai-bvnet': 0},
{'refls': [ 0, 0, 104, 35], 'doy': '20160805', 'gai': 0.730142, 'y': 4812168.0, 'x': 390294.0, 'id': 8, 'lai-bvnet': 0},
{'refls': [ 0, 0, 102, 35], 'doy': '20160805', 'gai': 0.858784, 'y': 4812238.0, 'x': 390424.0, 'id': 9, 'lai-bvnet': 0},
]
l8_4b_fr_rsr = "landsat8_4b.rsr"
l8_4b_fr_data = ["l8_4b_fr", l8_4b_fr_rsr]
l8_4b_fr_20160906 = {'doy' : 20160906, 'to' : 0.0, 'po' : 0.0,   'ts' :40.480858810, 'ps' :  150.669083830, 'gt':[s for s in gt if s['doy']=='20160906']}
l8_4b_fr_data.append(l8_4b_fr_20160906)
l8_4b_fr_20160406 = {'doy' : 20160406, 'to' : 0.0, 'po' : 0.0,   'ts' :40.447358790, 'ps' :  148.688153990, 'gt':[s for s in gt if s['doy']=='20160406']}
l8_4b_fr_data.append(l8_4b_fr_20160406)
l8_4b_fr_20160117 = {'doy' : 20160117, 'to' : 0.0, 'po' : 0.0,   'ts' :67.010610420, 'ps' :  158.025337570, 'gt':[s for s in gt if s['doy']=='20160117']}
l8_4b_fr_data.append(l8_4b_fr_20160117)
l8_4b_fr_20160524 = {'doy' : 20160524, 'to' : 0.0, 'po' : 0.0,   'ts' :27.111922680, 'ps' :  139.497912360, 'gt':[s for s in gt if s['doy']=='20160524']}
l8_4b_fr_data.append(l8_4b_fr_20160524)
l8_4b_fr_20160609 = {'doy' : 20160609, 'to' : 0.0, 'po' : 0.0,   'ts' :25.620463480, 'ps' :  135.962984360, 'gt':[s for s in gt if s['doy']=='20160609']}
l8_4b_fr_data.append(l8_4b_fr_20160609)
l8_4b_fr_20160704 = {'doy' : 20160704, 'to' : 0.0, 'po' : 0.0,   'ts' :26.403065730, 'ps' :  134.010578480, 'gt':[s for s in gt if s['doy']=='20160704']}
l8_4b_fr_data.append(l8_4b_fr_20160704)
l8_4b_fr_20160711 = {'doy' : 20160711, 'to' : 0.0, 'po' : 0.0,   'ts' :27.200585520, 'ps' :  134.574293550, 'gt':[s for s in gt if s['doy']=='20160711']}
l8_4b_fr_data.append(l8_4b_fr_20160711)
l8_4b_fr_20160720 = {'doy' : 20160720, 'to' : 0.0, 'po' : 0.0,   'ts' :28.532671650, 'ps' :  136.000543090, 'gt':[s for s in gt if s['doy']=='20160720']}
l8_4b_fr_data.append(l8_4b_fr_20160720)
l8_4b_fr_20160821 = {'doy' : 20160821, 'to' : 0.0, 'po' : 0.0,   'ts' :35.705937000, 'ps' :  145.254628110, 'gt':[s for s in gt if s['doy']=='20160821']}
l8_4b_fr_data.append(l8_4b_fr_20160821)
l8_4b_fr_20160812 = {'doy' : 20160812, 'to' : 0.0, 'po' : 0.0,   'ts' :33.342586490, 'ps' :  142.246250730, 'gt':[s for s in gt if s['doy']=='20160812']}
l8_4b_fr_data.append(l8_4b_fr_20160812)
l8_4b_fr_20160618 = {'doy' : 20160618, 'to' : 0.0, 'po' : 0.0,   'ts' :25.495243460, 'ps' :  134.598868660, 'gt':[s for s in gt if s['doy']=='20160618']}
l8_4b_fr_data.append(l8_4b_fr_20160618)
l8_4b_fr_20160805 = {'doy' : 20160805, 'to' : 0.0, 'po' : 0.0,   'ts' :31.680487180, 'ps' :  140.055531060, 'gt':[s for s in gt if s['doy']=='20160805']}
l8_4b_fr_data.append(l8_4b_fr_20160805)

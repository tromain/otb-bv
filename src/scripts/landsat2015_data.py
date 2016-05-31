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


gt = [
#{'doy':190,'crop':"tournesol",'esu':1.0,'y':2015.0,'gai':1.1052314018460001,'fapar':0.67219421650940003,'fcover':0.63084372578959991,'lai-bvnet':,'refls':[84.0,56.0,512.0,103.0]},
#{'doy':190,'crop':"tournesol",'esu':10.0,'y':2015.0,'gai':1.0860242519824002,'fapar':0.64822546009259996,'fcover':0.57780857961419996,'lai-bvnet':,'refls':[107.0,95.0,446.0,137.0]},
{'doy':190,'crop':"tournesol",'esu':11.0,'y':2015.0,'gai':1.2082071374580001,'fapar':0.69024581001599994,'fcover':0.71969707054359999,'lai-bvnet':2.0,'refls':[79.0,65.0,423.0,100.0]},
{'doy':190,'crop':"tournesol",'esu':12.0,'y':2015.0,'gai':0.85392347808239999,'fapar':0.52517625354460007,'fcover':0.39147653827779999,'lai-bvnet':0.96666700000000005,'refls':[116.0,112.0,400.0,169.0]},
#{'doy':190,'crop':"tournesol",'esu':13.0,'y':2015.0,'gai':1.3787812073579999,'fapar':0.76279304780899992,'fcover':0.77348311678140003,'lai-bvnet':,'refls':[31.0,18.0,220.0,24.0]},
#{'doy':190,'crop':"tournesol",'esu':8.0,'y':2015.0,'gai':0.8433596803608,'fapar':0.54350246561820004,'fcover':0.55207568342919999,'lai-bvnet':,'refls':[185.0,194.0,440.0,222.0]},
#{'doy':190,'crop':"tournesol",'esu':9.0,'y':2015.0,'gai':0.64731013255879999,'fapar':0.45489489003520001,'fcover':0.40674610805479999,'lai-bvnet':,'refls':[243.0,257.0,467.0,289.0]},
{'doy':215,'crop':"tournesol",'esu':1.0,'y':2015.0,'gai':0.723005519672,'fapar':0.53909169835299997,'fcover':0.54782624535000002,'lai-bvnet':1.8666700000000001,'refls':[107.0,71.0,442.0,96.0]},
{'doy':215,'crop':"tournesol",'esu':10.0,'y':2015.0,'gai':0.78228709185699996,'fapar':0.49144036386700002,'fcover':0.482165979528,'lai-bvnet':0.93333299999999997,'refls':[124.0,114.0,373.0,128.0]},
{'doy':215,'crop':"tournesol",'esu':11.0,'y':2015.0,'gai':0.79305291254900001,'fapar':0.52654075426199998,'fcover':0.47161086179599998,'lai-bvnet':1.23333,'refls':[103.0,89.0,358.0,103.0]},
{'doy':215,'crop':"tournesol",'esu':12.0,'y':2015.0,'gai':0.86616884810799999,'fapar':0.55469360874999996,'fcover':0.49859689950699998,'lai-bvnet':2.1000000000000001,'refls':[83.0,59.0,425.0,92.0]},
{'doy':215,'crop':"tournesol",'esu':13.0,'y':2015.0,'gai':0.95488947870100005,'fapar':0.54893806301600001,'fcover':0.34262422188300001,'lai-bvnet':1.3,'refls':[105.0,88.0,358.0,93.0]},
{'doy':215,'crop':"tournesol",'esu':2.0,'y':2015.0,'gai':1.17565484622,'fapar':0.68364318059200002,'fcover':0.69191330957700004,'lai-bvnet':1.43333,'refls':[101.0,81.0,394.0,104.0]},
{'doy':215,'crop':"tournesol",'esu':3.0,'y':2015.0,'gai':1.2265727744399999,'fapar':0.68546661309699997,'fcover':0.67007983352199996,'lai-bvnet':1.73333,'refls':[108.0,82.0,430.0,86.0]},
{'doy':215,'crop':"tournesol",'esu':4.0,'y':2015.0,'gai':0.97815835231600001,'fapar':0.57845622501100002,'fcover':0.45432433131099997,'lai-bvnet':0.63333300000000003,'refls':[132.0,132.0,337.0,155.0]},
{'doy':215,'crop':"tournesol",'esu':5.0,'y':2015.0,'gai':1.4494186278100001,'fapar':0.75907324785300001,'fcover':0.72550082486400003,'lai-bvnet':1.73333,'refls':[102.0,75.0,403.0,79.0]},
{'doy':215,'crop':"tournesol",'esu':6.0,'y':2015.0,'gai':1.3071205302,'fapar':0.73910894321300002,'fcover':0.737831796072,'lai-bvnet':2.8666700000000001,'refls':[82.0,51.0,472.0,69.0]},
{'doy':215,'crop':"tournesol",'esu':7.0,'y':2015.0,'gai':1.0802662522199999,'fapar':0.67362584990800001,'fcover':0.72158011811699996,'lai-bvnet':2.1333299999999999,'refls':[97.0,71.0,464.0,93.0]},
{'doy':215,'crop':"tournesol",'esu':8.0,'y':2015.0,'gai':0.95702108686700005,'fapar':0.62861716398099998,'fcover':0.57485392643400002,'lai-bvnet':2.26667,'refls':[82.0,60.0,453.0,93.0]},
{'doy':215,'crop':"tournesol",'esu':9.0,'y':2015.0,'gai':0.95764065512700003,'fapar':0.59735371965799999,'fcover':0.37593060640499998,'lai-bvnet':1.1000000000000001,'refls':[101.0,86.0,362.0,121.0]}]

lsat_rsr = "landsat8_4b.rsr"

lsat2015_data = ["landsat82015", lsat_rsr]


landsat82015_190 = {'doy':190, 'to':0.0,'po':0.0,'ts':27.322494450000001,'ps':27.322494450000001, 'gt':[s for s in gt if s['doy']==190]}
lsat2015_data.append(landsat82015_190)
landsat82015_215 = {'doy':215, 'to':0.0,'po':0.0,'ts':31.572822925000001,'ps':31.572822925000001, 'gt':[s for s in gt if s['doy']==215]}
lsat2015_data.append(landsat82015_215)
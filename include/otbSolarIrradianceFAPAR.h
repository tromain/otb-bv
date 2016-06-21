/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OTBSOLARIRRADFAPAR_H
#define __OTBSOLARIRRADFAPAR_H

#include<array>
#include "otbBVTypes.h"

namespace otb{
using PrecisionType = otb::BV::PrecisionType;

static 
std::vector<std::pair<PrecisionType, PrecisionType>> solar_irradiance_fapar = {
  {0.4000, 1614.0900},
  {0.4025, 1631.0900},
  {0.4050, 1648.0900},
  {0.4075, 1677.1899},
  {0.4100, 1706.2900},
  {0.4125, 1744.8900},
  {0.4150, 1783.4900},
  {0.4175, 1750.1899},
  {0.4200, 1716.8900},
  {0.4225, 1705.1899},
  {0.4250, 1693.4900},
  {0.4275, 1597.6899},
  {0.4300, 1501.8900},
  {0.4325, 1630.9900},
  {0.4350, 1760.0900},
  {0.4375, 1775.2400},
  {0.4400, 1790.3900},
  {0.4425, 1859.9399},
  {0.4450, 1929.4900},
  {0.4475, 1993.4399},
  {0.4500, 2057.3899},
  {0.4525, 2039.2300},
  {0.4550, 2021.0800},
  {0.4575, 2030.7300},
  {0.4600, 2040.3800},
  {0.4625, 2026.5300},
  {0.4650, 2012.6801},
  {0.4675, 1999.5300},
  {0.4700, 1986.3800},
  {0.4725, 2002.8800},
  {0.4750, 2019.3800},
  {0.4775, 2038.0900},
  {0.4800, 2056.7900},
  {0.4825, 1967.7400},
  {0.4850, 1878.6801},
  {0.4875, 1905.8300},
  {0.4900, 1932.9800},
  {0.4925, 1953.5800},
  {0.4950, 1974.1801},
  {0.4975, 1935.6801},
  {0.5000, 1897.1899},
  {0.5025, 1916.7800},
  {0.5050, 1936.3800},
  {0.5075, 1937.2300},
  {0.5100, 1938.0900},
  {0.5125, 1881.4399},
  {0.5150, 1824.7900},
  {0.5175, 1814.0900},
  {0.5200, 1803.3900},
  {0.5225, 1832.2400},
  {0.5250, 1861.0900},
  {0.5275, 1885.9301},
  {0.5300, 1910.7800},
  {0.5325, 1904.6801},
  {0.5350, 1898.5800},
  {0.5375, 1875.7300},
  {0.5400, 1852.8800},
  {0.5425, 1865.6400},
  {0.5450, 1878.3900},
  {0.5475, 1874.7400},
  {0.5500, 1871.0900},
  {0.5525, 1872.4399},
  {0.5550, 1873.7900},
  {0.5575, 1850.3900},
  {0.5600, 1826.9900},
  {0.5625, 1837.0400},
  {0.5650, 1847.0900},
  {0.5675, 1841.1801},
  {0.5700, 1835.2800},
  {0.5725, 1849.4800},
  {0.5750, 1863.6899},
  {0.5775, 1851.0300},
  {0.5800, 1838.3800},
  {0.5825, 1840.7300},
  {0.5850, 1843.0800},
  {0.5875, 1802.8300},
  {0.5900, 1762.5800},
  {0.5925, 1778.7800},
  {0.5950, 1794.9900},
  {0.5975, 1777.4800},
  {0.6000, 1759.9800},
  {0.6025, 1764.7300},
  {0.6050, 1769.4900},
  {0.6075, 1753.4800},
  {0.6100, 1737.4800},
  {0.6125, 1713.1400},
  {0.6150, 1688.8000},
  {0.6175, 1702.8800},
  {0.6200, 1716.9700},
  {0.6225, 1696.0699},
  {0.6250, 1675.1700},
  {0.6275, 1672.0300},
  {0.6300, 1668.8900},
  {0.6325, 1663.5601},
  {0.6350, 1658.2300},
  {0.6375, 1647.7500},
  {0.6400, 1637.2700},
  {0.6425, 1630.0200},
  {0.6450, 1622.7700},
  {0.6475, 1606.0601},
  {0.6500, 1589.3600},
  {0.6525, 1552.2900},
  {0.6550, 1515.2200},
  {0.6575, 1528.9100},
  {0.6600, 1542.6000},
  {0.6625, 1548.9000},
  {0.6650, 1555.2100},
  {0.6675, 1544.4100},
  {0.6700, 1533.6200},
  {0.6725, 1525.2400},
  {0.6750, 1516.8600},
  {0.6775, 1507.9200},
  {0.6800, 1498.9800},
  {0.6825, 1484.0699},
  {0.6850, 1469.1700},
  {0.6875, 1464.2800},
  {0.6900, 1459.3900},
  {0.6925, 1448.7300},
  {0.6950, 1438.0800},
  {0.6975, 1423.1600},
  {0.7000, 1408.2400},
};
}
#endif
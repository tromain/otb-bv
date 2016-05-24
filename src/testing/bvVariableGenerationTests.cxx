/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkMacro.h"
#include "otbBVUtil.h"
#include <cmath>

int bvCorrelateWithLAI(int itkNotUsed(argc), char * itkNotUsed(argv)[])
{

  using namespace otb::BV;

  VarParams avar{30.0, 80.0, 60.0, 20.0, 55, 65, true, 3, DistType::GAUSSIAN};

  double lai{3};
  double v{40};
  auto corr = CorrelateValue(v, lai, avar);

  double vminlai{avar.min+lai*(avar.Min_LAI_Max-avar.min)};
  double vmaxlai{avar.max+lai*(avar.Max_LAI_Max-avar.max)};
  double expected{(v-avar.min)/(avar.max-avar.min)*(vmaxlai-vminlai)+vminlai};

  std::cout << "Vmin0 = " << avar.min << "\n";
  std::cout << "Vmax0 = " << avar.max << "\n";
  std::cout << "VminLAImax = " << avar.Min_LAI_Max << "\n";
  std::cout << "VmaxLAImax = " << avar.Max_LAI_Max << "\n";
  std::cout << "Vminlai = " << vminlai <<"\n";
  std::cout << "Vmaxlai = " << vmaxlai <<"\n";
  std::cout << "Expected = " << expected <<"\n";

  if(fabs(corr-expected)>10e-5)
    {
    std::cout << corr << "\n";
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}

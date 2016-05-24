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
  VarParams MLAI = {0.0, 15.0, 2.0, 2.0, 0, 0, false, 6, otb::BV::DistType::LOGNORMAL};

  double lai{3};
  double v{40};
  auto corr = CorrelateValue(v, lai, avar, MLAI);

  double expected{43.4};

  if(fabs(corr-expected)>10e-5)
    {
    std::cout << "Expected = " << expected <<"\n";
    std::cout << corr << "\n";
    return EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}

/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbMultiLinearRegressionModel.h"

int bvMultiLinearFitting(int argc, char * argv[])
{
  using MRM=otb::MultiLinearRegressionModel<double>;
  MRM::MatrixType x_vec = {
    {0.0, 0},   
    {0.1, 0.01},
    {0.2, 0.04},
    {0.3, 0.09},
    {0.4, 0.16},
    {0.5, 0.25},
    {0.6, 0.36},
    {0.7, 0.49},
    {0.8, 0.64},
    {0.9, 0.81},
    {1.0, 1},   
    {1.1, 1.21},
    {1.2, 1.44},
    {1.3, 1.69},
    {1.4,  1.96 }};
  MRM::VectorType w_vec = {0.1,    0.110517,0.12214,0.134986,0.149182,0.164872,0.182212,0.201375,0.222554,0.24596,0.271828,0.300417,0.332012,0.36693,0.40552};
  MRM::VectorType y_vec = {1.01339,1.09543, 1.42592,1.44889, 1.64064, 1.4381,  1.38541, 1.87696, 2.21684, 2.67938,2.71348, 2.61466, 3.09834, 3.73597,4.39221};
  MRM::VectorType best_fit = {1.09573, 0.424647, 1.15956};

  auto model = MRM();
  model.SetPredictorMatrix(x_vec);
  model.SetTargetVector(y_vec);
  model.SetWeightVector(w_vec);
  auto result = model.Train();
  if(fabs(result[0]-best_fit[0])>0.01 ||
     fabs(result[1]-best_fit[1])>0.01 ||
     fabs(result[2]-best_fit[2])>0.01)
    return EXIT_FAILURE;

  MRM::VectorType test_vec = {x_vec[2][0], x_vec[2][1]};
  if(fabs(model.Predict(test_vec)-y_vec[2])>0.2)
    {
    std::cout << model.Predict(test_vec) << " " << y_vec[2] << " " << fabs(model.Predict(test_vec)-y_vec[2]) << "\n";
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}

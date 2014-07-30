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
#include "gsl/gsl_multifit.h"
#include <vector>


template <typename PrecisionType=double>
struct  MultiLinearRegressionModel
{
  using VectorType = std::vector<PrecisionType>;
  using MatrixType = std::vector<VectorType>;

  void SetPredictorMatrix(MatrixType x)
  {
    m_x = x;
  }
  void SetTargetVector(VectorType y)
  {
    m_y = y;
  }
  void SetWeightVector(VectorType w)
  {
    m_w = w;
  }
  VectorType Train()
  {
    this->multi_linear_fit();
    return m_model;
  }

  PrecisionType Predict(VectorType x)
  {
    if(m_model.size()==0)
      {
      itkExceptionMacro(<< "Model is not initialized.")
      }    
    if(x.size()!=(m_model.size()-1))
      {
      itkExceptionMacro(<< "Predictor vector and model have different sizes.")
      }

    PrecisionType result = m_model[0];
    for(auto i=0; i<x.size(); i++)
      result += m_model[i+1]*x[i];
    
    return result;
  }
protected:
  void multi_linear_fit()
  {
    auto n = m_x[0].size();
    auto m = m_x.size()+1;
    auto X = gsl_matrix_alloc (n, m);
    auto y = gsl_vector_alloc (n);
    auto w = gsl_vector_alloc (n);
    auto c = gsl_vector_alloc (m);
    auto cov = gsl_matrix_alloc (m, m);
    for (auto i = 0; i < n; i++)
      {
      gsl_matrix_set (X, i, 0, 1.0);
      for(auto j=0; j<m-1; j++)
        gsl_matrix_set(X, i, j+1, m_x[j][i]);
      gsl_vector_set (y, i, m_y[i]);
      gsl_vector_set (w, i, 1.0/(m_w[i]*m_w[i]));
      }
    gsl_multifit_linear_workspace * work 
      = gsl_multifit_linear_alloc (n, m);
    double chisq{0};
    gsl_multifit_wlinear(X, w, y, c, cov, &chisq, work);
    gsl_multifit_linear_free(work);

    m_model = VectorType{};
    for(auto j=0; j<m; j++)
      m_model.push_back(gsl_vector_get(c,j));
  }

  std::string GetNameOfClass()
  {
    return std::string{"MultiLinearRegressionModel"};
  }
  MatrixType m_x;
  VectorType m_y;
  VectorType m_w;
  VectorType m_model;
  
};



int bvMultiLinearFitting(int argc, char * argv[])
{
  using MRM=MultiLinearRegressionModel<double>;
  MRM::MatrixType x_vec = {
    {0.0,    0.1,     0.2,    0.3,     0.4,     0.5,     0.6,     0.7,     0.8,     0.9,    1.0,     1.1,     1.2,     1.3,    1.4},
    { 0, 0.01, 0.04, 0.09, 0.16, 0.25, 0.36, 0.49, 0.64, 0.81, 1, 1.21, 1.44, 1.69, 1.96}};
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

  MRM::VectorType test_vec = {x_vec[0][2], x_vec[1][2]};
  if(fabs(model.Predict(test_vec)-y_vec[2])>0.2)
    {
    std::cout << model.Predict(test_vec) << " " << y_vec[2] << " " << fabs(model.Predict(test_vec)-y_vec[2]) << "\n";
    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}

/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OTBMLRM_H
#define __OTBMLRM_H

#include "itkMacro.h"
#include "gsl/gsl_multifit.h"
#include <vector>

namespace otb{
template <typename PrecisionType=double>
struct  MultiLinearRegressionModel
{
  using VectorType = std::vector<PrecisionType>;
  using MatrixType = std::vector<VectorType>;

  MultiLinearRegressionModel() : m_weights(false) {};
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
    m_weights = true;
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

  /** Save the model to file */
  void Save(const std::string & filename);

  /** Load the model from file */
  void Load(const std::string & filename);


  VectorType GetModel()
  {
    return m_model;
  }
protected:
  void multi_linear_fit();

  std::string GetNameOfClass()
  {
    return std::string{"MultiLinearRegressionModel"};
  }
  MatrixType m_x;
  VectorType m_y;
  VectorType m_w;
  bool m_weights;
  VectorType m_model;
  
};
}//namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbMultiLinearRegressionModel.txx"
#endif

#endif

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
#include "otbMachineLearningModel.h"

namespace otb{
template <typename PrecisionType=double>
struct  MultiLinearRegressionModel : 
    public MachineLearningModel<PrecisionType, PrecisionType>
{
  using VectorType = std::vector<PrecisionType>;
  using MatrixType = std::vector<VectorType>;
  using TargetSampleType = typename 
    MachineLearningModel<PrecisionType, 
                         PrecisionType>::TargetSampleType;
  using InputSampleType = typename 
    MachineLearningModel<PrecisionType, 
                         PrecisionType>::InputSampleType;

  MultiLinearRegressionModel() : m_weights(false) {};

  /** Standard class typedefs. */
  typedef MultiLinearRegressionModel           Self;
  typedef MachineLearningModel<PrecisionType, PrecisionType> Superclass;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;

  itkNewMacro(Self);
  itkTypeMacro(MultiLinearRegressionModel, itk::MachineLearningModel);

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
  void Train()
  {
    this->multi_linear_fit();
  }

  PrecisionType Predict(const VectorType x) const
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

  TargetSampleType Predict(const InputSampleType & input) const
  {
    VectorType tmp_vec(input.GetSize());
    TargetSampleType target;
    target[0] = this->Predict(tmp_vec);
    return target;
      }


  /** Save the model to file */
  void Save(const std::string & filename, const std::string & name="");

  /** Load the model from file */
  void Load(const std::string & filename, const std::string & name="");

  bool CanReadFile(const std::string &);

  bool CanWriteFile(const std::string &);

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

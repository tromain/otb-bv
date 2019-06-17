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
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wheader-guard"
#pragma clang diagnostic ignored "-Wexpansion-to-defined"
#else
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#endif
namespace otb{
template <typename PrecisionType=double>
class  MultiLinearRegressionModel : 
    public MachineLearningModel<PrecisionType, PrecisionType>
{
public:
  using VectorType = std::vector<PrecisionType>;
  using MatrixType = std::vector<VectorType>;
  using TargetSampleType = typename 
    MachineLearningModel<PrecisionType, 
                         PrecisionType>::TargetSampleType;
  using InputSampleType = typename 
    MachineLearningModel<PrecisionType, 
                         PrecisionType>::InputSampleType;

  typedef itk::Statistics::ListSample<TargetSampleType> TargetListSampleType;
  typedef itk::Statistics::ListSample<InputSampleType> InputListSampleType;

  MultiLinearRegressionModel() : m_weights(false) {};

  /** Standard class typedefs. */
  typedef MultiLinearRegressionModel           Self;
  typedef MachineLearningModel<PrecisionType, PrecisionType> Superclass;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;
  typedef typename Superclass::ConfidenceValueType     ConfidenceValueType;
  typedef typename Superclass::ProbaSampleType             ProbaSampleType;

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
  void Train() ITK_OVERRIDE
  {
    this->multi_linear_fit();
  }

  void SetInputListSample(InputListSampleType * ils)
  {
    MatrixType tmp_m;
    auto slIt = ils->Begin();
    auto slEnd = ils->End();
    while(slIt != slEnd)
      {
      tmp_m.push_back(this->SampleToVector(slIt.GetMeasurementVector()));
      ++slIt;
      }
    this->SetPredictorMatrix(tmp_m);
  }
  void SetTargetListSample(TargetListSampleType * tls)
  {
    VectorType tmp_v;
    auto slIt = tls->Begin();
    auto slEnd = tls->End();
    while(slIt != slEnd)
      {
      tmp_v.push_back(slIt.GetMeasurementVector()[0]);
      ++slIt;
      }
    this->SetTargetVector(tmp_v);

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

  MatrixType GetPredictorMatrix() const
  {
    return m_x;
  }

  VectorType GetTargetVector() const
  {
    return m_y;
  }

  PrecisionType PredictVector(const VectorType x) const
  {
    return this->DoPredict(x);
  }

protected:

  PrecisionType DoPredict(const VectorType x) const
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
    for(size_t i=0; i<x.size(); i++)
      result += m_model[i+1]*x[i];
    
    return result;
  }

  TargetSampleType DoPredict(const InputSampleType& input, 
                             ConfidenceValueType *quality=nullptr, 
                             ProbaSampleType *proba=nullptr) const override
  {
    VectorType tmp_vec(this->SampleToVector(input));
    TargetSampleType target;
    target[0] = this->DoPredict(tmp_vec);
    return target;
  }

  void multi_linear_fit();

  std::string GetNameOfClass()
  {
    return std::string{"MultiLinearRegressionModel"};
  }

  template<typename MVType>
  VectorType SampleToVector(MVType mv) const
  {
    VectorType tmp_vec(mv.Size());
    for(size_t i=0; i<mv.Size(); ++i)
      tmp_vec[i] = mv[i];
    return tmp_vec;
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

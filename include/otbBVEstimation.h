/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OTBBVESTIMATION_H
#define __OTBBVESTIMATION_H

#include "otbBVTypes.h"
namespace otb
{
namespace Functor
{

template <typename InputPixelType, typename OutputPixelType>
class BVEstimation
{
public:

  using FloatType = typename InputPixelType::ValueType;
  BVEstimation() = default;
  BVEstimation(BV::ModelType* model, 
               const BV::NormalizationVectorType& normalization) : 
    m_Model{model}, m_Normalization{normalization}, m_checkValidity{false} {}

  BVEstimation(BV::ModelType* model, 
               const BV::NormalizationVectorType& normalization,
               const vnl_matrix<FloatType>& covariance,
               const vnl_vector<FloatType>& meanvec,
               FloatType confidence ) : 
    m_Model{model}, m_Normalization{normalization},
    m_MeanVector{meanvec}, m_checkValidity{true}, m_Confidence{confidence}  {
      BV::InverseCovarianceAndDeterminant(covariance, m_InvCovariance);
    }

  ~BVEstimation() {};
  
  inline
  OutputPixelType operator ()(const InputPixelType& in_pix) const
  {
    bool normalization{m_Normalization!=BV::NormalizationVectorType{}};
    OutputPixelType pix{};
    if(!m_checkValidity)
      {
      pix.SetSize(1);
      }
    else
      {
      pix.SetSize(3); //inversion, valid, proba
      auto res = BV::IsValidSample(in_pix, m_InvCovariance, 
                                   m_MeanVector,
                                   m_Confidence);
      pix[1] = res.first;
      pix[2] = res.second;
      }
    auto nbInputVariables = in_pix.GetSize();
    BV::InputSampleType inputValue;
    inputValue.Reserve(nbInputVariables);
    for(size_t var = 0; var < nbInputVariables; ++var)
         {
         inputValue[var] = in_pix[var];
         if( normalization )
           inputValue[var] = BV::normalize(inputValue[var], m_Normalization[var]);
         }
    BV::OutputSampleType outputValue = m_Model->Predict(inputValue);
    pix[0] = outputValue[0];
    if( normalization )
      pix[0] = BV::denormalize(outputValue[0],
                               m_Normalization[nbInputVariables]);
    return pix;
  }

  bool operator !=(const BVEstimation& other) const
    {
      return (this->m_Model!=other.m_Model ||
              this->m_Normalization!=other.m_Normalization);
    }

    bool operator ==(const BVEstimation& other) const
    {
      return !(*this!=other);
    }

protected:
  typename BV::ModelType::Pointer m_Model{nullptr};
  BV::NormalizationVectorType m_Normalization{};
  bool m_checkValidity{false};
  vnl_matrix<FloatType> m_InvCovariance;
  vnl_vector<FloatType> m_MeanVector;
  FloatType m_Confidence{0.0};
};

}
}

#endif

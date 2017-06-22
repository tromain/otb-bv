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
  BVEstimation() = default;
  BVEstimation(BV::ModelType* model, 
               const BV::NormalizationVectorType& normalization) : 
    m_Model{model}, m_Normalization{normalization} {}

  ~BVEstimation() {};
  
  inline
  OutputPixelType operator ()(const InputPixelType& in_pix)
  {
    bool normalization{m_Normalization!=BV::NormalizationVectorType{}};
    OutputPixelType pix{};
    pix.SetSize(1);
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
  typename BV::ModelType::Pointer m_Model;
  BV::NormalizationVectorType m_Normalization;

};

}
}

#endif

/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"
#include "otbWrapperChoiceParameter.h"
#include "otbBVTypes.h"
#include <random>

namespace otb
{
namespace Wrapper
{

class BVInputVariableGeneration : public Application
{
public:
/** Standard class typedefs. */
  typedef BVInputVariableGeneration     Self;
  typedef Application                   Superclass;


  /** Standard macro */
  itkNewMacro(Self);
  itkTypeMacro(BVInputVariableGeneration, otb::Application);


private:
  void DoInit() override; 
  virtual ~BVInputVariableGeneration() override {}
  void DoUpdateParameters() override {}
  ///Builds the map with the values of the sample
  otb::BV::SampleType DrawSample();
  void WriteSample(otb::BV::SampleType s);
  void DoExecute() override;

  otb::BV::VarParams m_MLAI = {0.0, 15.0, 2.0, 2.0, 0, 0, true, 6, otb::BV::DistType::LOGNORMAL};
  otb::BV::VarParams m_ALA ={30.0, 80.0, 60.0, 20.0, 55, 65, true, 3, otb::BV::DistType::GAUSSIAN};
  otb::BV::VarParams m_CrownCover ={0.95, 1.0, 0.8, 0.4, 0.95, 1.0, true, 1, otb::BV::DistType::UNIFORM};
  otb::BV::VarParams m_HsD ={0.1, 0.5, 0.2, 0.5, 0.1, 0.5, true, 1, otb::BV::DistType::GAUSSIAN};
  otb::BV::VarParams m_N ={1.20, 2.20, 1.50, 0.30, 1.30, 1.80, true, 3, otb::BV::DistType::GAUSSIAN};
  otb::BV::VarParams m_Cab ={20.0, 90.0, 45.0, 30.0, 45, 90, true, 4, otb::BV::DistType::GAUSSIAN};
  /* m_Car distribution will not be used and Car=Cab/4, but we keep it here for future evolutions*/
  otb::BV::VarParams m_Car ={0.0, 25.0, 8.58, 3.95, 0, 0, true, 1, otb::BV::DistType::GAUSSIAN};
  otb::BV::VarParams m_Cdm ={0.0030, 0.0110, 0.0050, 0.0050, 0.0050, 0.0110, true, 4, otb::BV::DistType::GAUSSIAN};
  otb::BV::VarParams m_CwRel ={0.60, 0.85, 0.75, 0.08, 0.70, 0.80, true, 4, otb::BV::DistType::UNIFORM};
  otb::BV::VarParams m_Cbp ={0.00, 2.00, 0.00, 0.30, 0.00, 0.20, true, 3, otb::BV::DistType::GAUSSIAN  };
  otb::BV::VarParams m_Bs ={ 0.0, 1.00, 0.5, 2.00, 0.50, 1.20, true, 4, otb::BV::DistType::GAUSSIAN};
  otb::BV::VarParams m_SoilIndex ={ 1.0, 1.0, 5.0, 5.0, 0.0, 0.0, false, 1, otb::BV::DistType::UNIFORM};

  // the random number generator
  std::mt19937 m_RNG;
  // the output file
  std::ofstream m_SampleFile;
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::BVInputVariableGeneration)

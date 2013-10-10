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

#include "vnl/vnl_random.h"

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
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

/** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(BVInputVariableGeneration, otb::Application);


private:
  void DoInit()
  {
    SetName("BVInputVariableGeneration");
    SetDescription("Generate random input variable distribution for ... .");

    AddParameter(ParameterType_OutputFilename, "out", "Output file");
    SetParameterDescription( "out", "Filename where the variable sets are saved." );
    MandatoryOn("out");
  }

  virtual ~BVInputVariableGeneration()
  {
  }


  void DoUpdateParameters()
  {
   // Nothing to do here : all parameters are independent
  }



  void DoExecute()
  {
    //TODO: could use a particular seed if useful
    vnl_random rng = vnl_random();

    //uniform
    double drand32(double a, double b);

    //unit, centered normal
    double normal64();
  }



};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::BVInputVariableGeneration)

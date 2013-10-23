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

#include <fstream>

namespace otb
{

namespace Wrapper
{

class ProSailSimulator : public Application
{
public:
/** Standard class typedefs. */
  typedef ProSailSimulator     Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  
/** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(ProSailSimulator, otb::Application);

  // Parameters for the acquisition geometry
  enum AcquisitionParameters {HSPOT, TTS, TTO, PSI};
  typedef  std::map< AcquisitionParameters, double > AcquisitionParsType;
  
private:
  void DoInit()
  {
    SetName("ProSailSimulator");
    SetDescription("Simulate reflectances using Prospect+Sail.");

    AddParameter(ParameterType_InputFilename, "bv-file", "Input file containing the bv samples.");
    SetParameterDescription( "in", "Input file containing the biophysical variable samples. It can be generated using the BVInputVariableGeneration application." );
    MandatoryOn("bv-file");

    AddParameter(ParameterType_InputFilename, "soil-file", "Input file containing the soil spectra.");
    SetParameterDescription( "in", "Input file containing ." );
    MandatoryOn("soil-file");

    AddParameter(ParameterType_InputFilename, "srs-file", "Input file containing the spectral relative sensitivities..");
    SetParameterDescription( "in", "Input file containing ." );
    MandatoryOn("srs-file");
    
    AddParameter(ParameterType_OutputFilename, "out", "Output file");
    SetParameterDescription( "out", "Filename where the simulations are saved." );
    MandatoryOn("out");

    AddParameter(ParameterType_Float, "solar-zenith", "");
    SetParameterDescription( "solar-zenith", "." );
    MandatoryOn("solar-zenith");

    AddParameter(ParameterType_Float, "sensor-zenith", "");
    SetParameterDescription( "sensor-zenith", "." );
    MandatoryOn("sensor-zenith");

    AddParameter(ParameterType_Float, "azimuth", "");
    SetParameterDescription( "azimuth", "." );
    MandatoryOn("azimuth");

  }

  virtual ~ProSailSimulator()
  {
  }


  void DoUpdateParameters()
  {
    // Nothing to do here : all parameters are independent
  }


  void DoExecute()
  {

    

  }

  // the input file
  std::ifstream m_SampleFile;

  // the output file
  std::ofstream m_SimulationsFile;
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::ProSailSimulator)

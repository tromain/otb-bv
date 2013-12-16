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
#include <string>

#include "otbBVUtil.h"

#include "otbMachineLearningModelFactory.h"
#include "otbNeuralNetworkRegressionMachineLearningModel.h"
#include "itkListSample.h"

namespace otb
{

namespace Wrapper
{

class BVInversion : public Application
{
public:
/** Standard class typedefs. */
  typedef BVInversion     Self;
  typedef Application                   Superclass;
  
/** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(BVInversion, otb::Application);

  typedef double PrecisionType;
  typedef itk::FixedArray<PrecisionType, 1> OutputSampleType;
  typedef itk::VariableLengthVector<PrecisionType> InputSampleType;
  typedef itk::Statistics::ListSample<OutputSampleType> ListOutputSampleType;
  typedef itk::Statistics::ListSample<InputSampleType> ListInputSampleType;
  typedef otb::NeuralNetworkRegressionMachineLearningModel<PrecisionType, PrecisionType> NeuralNetworkType;
  
private:
  void DoInit()
  {
    SetName("BVInversion");
    SetDescription("Estimate biophysical variables using a neural network inversion of Prospect+Sail.");

    AddParameter(ParameterType_InputFilename, "reflectances", "Input file containing the reflectances to invert.");
    SetParameterDescription( "reflectances", "Input file containing the reflectances to invert. This is an ASCII file where each line is a sample. A line is a set of fields containing numerical values. The order of the fields must respect the one used for the neural network training." );
    MandatoryOn("reflectances");

    AddParameter(ParameterType_InputFilename, "model", "File containing the NN model.");
    SetParameterDescription( "model", "File containing the NN model.");
    MandatoryOn("model");
    
    AddParameter(ParameterType_OutputFilename, "out", "Output estimated variable.");
    SetParameterDescription( "out", "Filename where the estimated variables will be saved." );
    MandatoryOn("out");
  }

  virtual ~BVInversion()
  {
  }


  void DoUpdateParameters()
  {
    // Nothing to do here : all parameters are independent
  }

  
  void DoExecute()
  {
   
    auto reflectancesFileName = GetParameterString("reflectances");
    std::ifstream reflectancesFile;
    try
      {
      reflectancesFile.open(reflectancesFileName.c_str());
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " << reflectancesFileName);
      }


    auto outFileName = GetParameterString("out");
    std::ofstream outFile;
    try
      {
      outFile.open(outFileName.c_str());
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " << outFileName);
      }

    auto nbInputVariables = countColumns(reflectancesFileName);
    otbAppLogINFO("Found " << nbInputVariables << " input variables in "
                  << reflectancesFileName << std::endl);

    auto classifier = NeuralNetworkType::New();
    classifier->Load(GetParameterString("model"));    
    

    otbAppLogINFO("Applying NN regression ..." << std::endl);
    auto sampleCount = 0;
    for(std::string line; std::getline(reflectancesFile, line); )
      {
        if(line.size() > 1)
          {
          std::istringstream ss(line);
          InputSampleType inputValue;
          inputValue.Reserve(nbInputVariables);
          for(auto var = 0; var < nbInputVariables; ++var)
            ss >> inputValue[var];
          OutputSampleType outputValue = classifier->Predict(inputValue);
          outFile << outputValue[0] << std::endl;
          ++sampleCount;
          }
      }
    reflectancesFile.close();
    outFile.close();
    otbAppLogINFO("" << sampleCount << " samples processed. Results saved in "
                  << outFileName << std::endl);
  }



};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::BVInversion)

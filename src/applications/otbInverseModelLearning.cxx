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

class InverseModelLearning : public Application
{
public:
/** Standard class typedefs. */
  typedef InverseModelLearning     Self;
  typedef Application                   Superclass;

  
/** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(InverseModelLearning, otb::Application);

  typedef double PrecisionType;
  typedef itk::FixedArray<PrecisionType, 1> OutputSampleType;
  typedef itk::VariableLengthVector<PrecisionType> InputSampleType;
  typedef itk::Statistics::ListSample<OutputSampleType> ListOutputSampleType;
  typedef itk::Statistics::ListSample<InputSampleType> ListInputSampleType;
  typedef otb::NeuralNetworkRegressionMachineLearningModel<PrecisionType, PrecisionType> NeuralNetworkType;
  
private:
  void DoInit()
  {
    SetName("InverseModelLearning");
    SetDescription("Simulate reflectances using Prospect+Sail.");

    AddParameter(ParameterType_InputFilename, "training", "Input file containing the training samples.");
    SetParameterDescription( "training", "Input file containing the training samples. This is an ASCII file where each line is a training sample. A line is a set of fields containing numerical values. The first field is the value of the output variable and the other contain the values of the input variables." );
    MandatoryOn("training");

    AddParameter(ParameterType_OutputFilename, "out", "Output regression model.");
    SetParameterDescription( "out", "Filename where the regression model will be saved." );
    MandatoryOn("out");
  }

  virtual ~InverseModelLearning()
  {
  }


  void DoUpdateParameters()
  {
    // Nothing to do here : all parameters are independent
  }

  
  void DoExecute()
  {
   
    auto trainingFileName = GetParameterString("training");
    std::ifstream trainingFile;
    try
      {
      trainingFile.open(trainingFileName.c_str());
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " << trainingFileName);
      }

    auto nbInputVariables = countColumns(trainingFileName) - 1;

    otbAppLogINFO("Found " << nbInputVariables << " input variables in "
                  << trainingFileName << std::endl);

    auto inputListSample = ListInputSampleType::New();
    auto outputListSample = ListOutputSampleType::New();

    auto nbSamples = 0;
    for(std::string line; std::getline(trainingFile, line); )
      {
        if(line.size() > 1)
          {
          std::istringstream ss(line);
          OutputSampleType outputValue;
          ss >> outputValue[0];
          InputSampleType inputValue;
          inputValue.Reserve(nbInputVariables);
          for(unsigned int var = 0; var < nbInputVariables; ++var)
            ss >> inputValue[var];
          inputListSample->PushBack(inputValue);
          outputListSample->PushBack(outputValue);
          ++nbSamples;
          }
      }
    trainingFile.close();

    otbAppLogINFO("Found " << nbSamples << " samples in "
                  << trainingFileName << std::endl);
    auto classifier = NeuralNetworkType::New();
    classifier->SetInputListSample(inputListSample);
    classifier->SetTargetListSample(outputListSample);
    classifier->SetTrainMethod(CvANN_MLP_TrainParams::BACKPROP);
    // One hidden layer with 5 neurons and one output variable
    classifier->SetLayerSizes(std::vector<unsigned int>({nbInputVariables, 5, 1}));
    classifier->SetActivateFunction(CvANN_MLP::SIGMOID_SYM);
    classifier->SetAlpha(1.0);
    classifier->SetBeta(1.0);
    classifier->SetBackPropDWScale(0.1);
    classifier->SetBackPropMomentScale(0.1);
    classifier->SetRegPropDW0(0.1);
    classifier->SetRegPropDWMin(1e-7);
    classifier->SetTermCriteriaType(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS);
    classifier->SetEpsilon(1e-10);
    classifier->SetMaxIter(10000000);

    otbAppLogINFO("Training the neural network ..." << std::endl);
    classifier->Train();
    classifier->Save(GetParameterString("out"));

    otbAppLogINFO("Estimation of prediction error from training samples ..."
                  << std::endl);

    double rmse = 0.0;
    typename ListInputSampleType::ConstIterator sampleIt = inputListSample->Begin();
    typename ListOutputSampleType::ConstIterator resultIt = outputListSample->Begin();
    while(sampleIt != inputListSample->End() && resultIt != outputListSample->End())
      {
      rmse += pow(classifier->Predict(sampleIt.GetMeasurementVector())[0] -
                  resultIt.GetMeasurementVector()[0], 2.0);
      ++sampleIt;
      ++resultIt;
      }

    rmse = sqrt(rmse)/nbSamples;

    otbAppLogINFO("RMSE = " << rmse << std::endl);
  }



};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::InverseModelLearning)

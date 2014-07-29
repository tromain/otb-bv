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
#include <limits>

#include "otbBVUtil.h"
#include "otbBVTypes.h"

#include "otbMachineLearningModelFactory.h"
#include "otbSVMMachineLearningModel.h"
#include "itkListSample.h"

namespace otb
{


namespace Wrapper
{

class SVRInverseModelLearning : public Application
{
public:
/** Standard class typedefs. */
  typedef SVRInverseModelLearning     Self;
  typedef Application                   Superclass;

  
/** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(SVRInverseModelLearning, otb::Application);

  typedef itk::FixedArray<PrecisionType, 1> OutputSampleType;
  typedef itk::VariableLengthVector<PrecisionType> InputSampleType;
  typedef itk::Statistics::ListSample<OutputSampleType> ListOutputSampleType;
  typedef itk::Statistics::ListSample<InputSampleType> ListInputSampleType;
  typedef otb::SVMMachineLearningModel<PrecisionType, PrecisionType> SVRType;
  
private:
  void DoInit()
  {
    SetName("SVRInverseModelLearning");
    SetDescription("Simulate reflectances using Prospect+Sail.");

    AddParameter(ParameterType_InputFilename, "training", "Input file containing the training samples.");
    SetParameterDescription( "training", "Input file containing the training samples. This is an ASCII file where each line is a training sample. A line is a set of fields containing numerical values. The first field is the value of the output variable and the other contain the values of the input variables." );
    MandatoryOn("training");

    AddParameter(ParameterType_OutputFilename, "out", "Output regression model.");
    SetParameterDescription( "out", "Filename where the regression model will be saved." );
    MandatoryOn("out");

    AddParameter(ParameterType_OutputFilename, "normalization", "Output file containing min and max values per sample component.");
    SetParameterDescription( "normalization", "Output file containing min and max values per sample component. This file can be used by the inversion application. If no file is given as parameter, the variables are not normalized." );
    MandatoryOff("normalization");
  }

  virtual ~SVRInverseModelLearning()
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

    std::size_t nbInputVariables = countColumns(trainingFileName) - 1;

    otbAppLogINFO("Found " << nbInputVariables << " input variables in "
                  << trainingFileName << std::endl);

    auto inputListSample = ListInputSampleType::New();
    auto outputListSample = ListOutputSampleType::New();

    inputListSample->SetMeasurementVectorSize(nbInputVariables);
    outputListSample->SetMeasurementVectorSize(1);
    
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
          for(auto var = 0; var < nbInputVariables; ++var)
            ss >> inputValue[var];
          inputListSample->PushBack(inputValue);
          outputListSample->PushBack(outputValue);
          ++nbSamples;
          }
      }
    trainingFile.close();

    if( HasValue( "normalization" )==true )
      {
      otbAppLogINFO("Variable normalization."<< std::endl);

      typename ListInputSampleType::Iterator ilFirst = inputListSample->Begin();
      typename ListOutputSampleType::Iterator olFirst = outputListSample->Begin();
      typename ListInputSampleType::Iterator ilLast = inputListSample->End();
      typename ListOutputSampleType::Iterator olLast = outputListSample->End();
      
      auto var_minmax = estimate_var_minmax(ilFirst, ilLast, olFirst, olLast);
      write_normalization_file(var_minmax, GetParameterString("normalization"));
      normalize_variables(inputListSample, outputListSample, var_minmax);
      

      for(auto var = 0; var < nbInputVariables; ++var)
        otbAppLogINFO("Variable "<< var+1 << " min=" << var_minmax[var].first <<
                      " max=" << var_minmax[var].second <<std::endl);
      otbAppLogINFO("Output min=" << var_minmax[nbInputVariables].first <<
                      " max=" << var_minmax[nbInputVariables].second <<std::endl)
      }

    
    otbAppLogINFO("Found " << nbSamples << " samples in "
                  << trainingFileName << std::endl);
    auto classifier = SVRType::New();
    classifier->SetInputListSample(inputListSample);
    classifier->SetTargetListSample(outputListSample);
    classifier->SetSVMType(CvSVM::NU_SVR);
    classifier->SetNu(0.5);
    classifier->SetKernelType(CvSVM::RBF);
    classifier->SetTermCriteriaType(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS);
    classifier->SetMaxIter(100000);
    classifier->SetEpsilon(FLT_EPSILON);
    classifier->SetParameterOptimization(true);

    otbAppLogINFO("Training the SVR ..." << std::endl);
    classifier->Train();
    classifier->Save(GetParameterString("out"));

    otbAppLogINFO("Estimation of prediction error from training samples ..."
                  << std::endl);

    auto rmse = 0.0;
    auto sampleIt = inputListSample->Begin();
    auto resultIt = outputListSample->Begin();
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

OTB_APPLICATION_EXPORT(otb::Wrapper::SVRInverseModelLearning)

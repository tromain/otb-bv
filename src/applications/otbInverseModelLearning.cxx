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
#include "otbNeuralNetworkRegressionMachineLearningModel.h"
#include "otbSVMMachineLearningModel.h"
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

  typedef itk::FixedArray<PrecisionType, 1> OutputSampleType;
  typedef itk::VariableLengthVector<PrecisionType> InputSampleType;
  typedef itk::Statistics::ListSample<OutputSampleType> ListOutputSampleType;
  typedef itk::Statistics::ListSample<InputSampleType> ListInputSampleType;
  typedef otb::NeuralNetworkRegressionMachineLearningModel<PrecisionType, PrecisionType> NeuralNetworkType;
  typedef otb::SVMMachineLearningModel<PrecisionType, PrecisionType> SVRType;
  
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

    AddParameter(ParameterType_OutputFilename, "normalization", "Output file containing min and max values per sample component.");
    SetParameterDescription( "normalization", "Output file containing min and max values per sample component. This file can be used by the inversion application. If no file is given as parameter, the variables are not normalized." );
    MandatoryOff("normalization");

    AddParameter(ParameterType_String, "regression", "Regression to use for the training");
    SetParameterDescription("regression", "Choice of the regression to use for the training: svr, nn.");
    MandatoryOff("regression");

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
    double rmse{0.0};
    std::string regressor_type{"nn"};
    if (IsParameterEnabled("regression"))
      regressor_type = GetParameterString("regression");    
    if (regressor_type == "svr")
      rmse = EstimateSVRRegresionModel(inputListSample, outputListSample, nbInputVariables);
    else if (regressor_type == "nn")
      rmse = EstimateNNRegresionModel(inputListSample, outputListSample, nbInputVariables);
    otbAppLogINFO("RMSE = " << rmse << std::endl);
  }

  template <typename RegressionType>
  double EstimateRegressionModel(RegressionType rgrsn, ListInputSampleType::Pointer ils, ListOutputSampleType::Pointer ols)
  {
    rgrsn->SetInputListSample(ils);
    rgrsn->SetTargetListSample(ols);
    otbAppLogINFO("Model estimation ..." << std::endl);
    rgrsn->Train();
    rgrsn->Save(GetParameterString("out"));
    otbAppLogINFO("Estimation of prediction error from training samples ..."
                  << std::endl);
    auto nbSamples = 0;
    auto rmse = 0.0;
    auto sampleIt = ils->Begin();
    auto resultIt = ols->Begin();
    while(sampleIt != ils->End() && resultIt != ols->End())
      {
      rmse += pow(rgrsn->Predict(sampleIt.GetMeasurementVector())[0] -
                  resultIt.GetMeasurementVector()[0], 2.0);
      ++sampleIt;
      ++resultIt;
      ++nbSamples;
      }
    return sqrt(rmse)/nbSamples;
  }

  double EstimateNNRegresionModel(ListInputSampleType::Pointer ils, ListOutputSampleType::Pointer ols, std::size_t nbVars)
  {
    otbAppLogINFO("Neural networks");
    auto regression = NeuralNetworkType::New();
    regression->SetTrainMethod(CvANN_MLP_TrainParams::BACKPROP);
    // Two hidden layer with 5 neurons and one output variable
    regression->SetLayerSizes(std::vector<unsigned int>({static_cast<unsigned int>(nbVars), 5, 5, 1}));
    regression->SetActivateFunction(CvANN_MLP::SIGMOID_SYM);
    regression->SetAlpha(1.0);
    regression->SetBeta(1.0);
    regression->SetBackPropDWScale(0.1);
    regression->SetBackPropMomentScale(0.1);
    regression->SetRegPropDW0(0.1);
    regression->SetRegPropDWMin(1e-7);
    regression->SetTermCriteriaType(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS);
    regression->SetEpsilon(1e-10);
    regression->SetMaxIter(10000000);
    return EstimateRegressionModel(regression, ils, ols);
  }

  double EstimateSVRRegresionModel(ListInputSampleType::Pointer ils, ListOutputSampleType::Pointer ols, std::size_t nbVars)
  {
    otbAppLogINFO("Support vectors");
    auto regression = SVRType::New();
    regression->SetSVMType(CvSVM::NU_SVR);
    regression->SetNu(0.5);
    regression->SetKernelType(CvSVM::RBF);
    regression->SetTermCriteriaType(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS);
    regression->SetMaxIter(100000);
    regression->SetEpsilon(FLT_EPSILON);
    regression->SetParameterOptimization(true);
    return EstimateRegressionModel(regression, ils, ols);
  }
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::InverseModelLearning)

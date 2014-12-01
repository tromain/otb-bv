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
#include "otbMultiLinearRegressionModel.h"
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
  typedef otb::NeuralNetworkRegressionMachineLearningModel<PrecisionType, 
                                                           PrecisionType> 
  NeuralNetworkType;
  typedef otb::SVMMachineLearningModel<PrecisionType, PrecisionType> SVRType;
  typedef otb::MultiLinearRegressionModel<PrecisionType> MLRType;
  
private:
  void DoInit()
  {
    SetName("InverseModelLearning");
    SetDescription("Simulate reflectances using Prospect+Sail.");

    AddParameter(ParameterType_InputFilename, "training", 
                 "Input file containing the training samples.");
    SetParameterDescription( "training", 
                             "Input file containing the training samples. This is an ASCII file where each line is a training sample. A line is a set of fields containing numerical values. The first field is the value of the output variable and the other contain the values of the input variables." );
    MandatoryOn("training");

    AddParameter(ParameterType_OutputFilename, "out", 
                 "Output regression model.");
    SetParameterDescription( "out", 
                             "Filename where the regression model will be saved." );
    MandatoryOn("out");

    AddParameter(ParameterType_OutputFilename, "normalization", 
                 "Output file containing min and max values per sample component.");
    SetParameterDescription( "normalization", 
                             "Output file containing min and max values per sample component. This file can be used by the inversion application. If no file is given as parameter, the variables are not normalized." );
    MandatoryOff("normalization");

    AddParameter(ParameterType_String, "regression", 
                 "Regression to use for the training (nn, svr, mlr)");
    SetParameterDescription("regression", 
                            "Choice of the regression to use for the training: svr, nn, mlr.");
    MandatoryOff("regression");

    AddParameter(ParameterType_Int, "bestof", "Select the best of N models.");
    SetParameterDescription("bestof", "");
    MandatoryOff("bestof");

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
    unsigned int nbModels{1};
    if (IsParameterEnabled("bestof"))
      nbModels = static_cast<unsigned int>(GetParameterInt("bestof"));    
    if (IsParameterEnabled("regression"))
      regressor_type = GetParameterString("regression");    
    if (regressor_type == "svr")
      rmse = EstimateSVRRegresionModel(inputListSample, outputListSample, 
                                       nbInputVariables, nbModels);
    else if (regressor_type == "nn")
      rmse = EstimateNNRegresionModel(inputListSample, outputListSample, 
                                      nbInputVariables, nbModels);
    else if (regressor_type == "mlr")
      rmse = EstimateMLRRegresionModel(inputListSample, outputListSample, 
                                       nbInputVariables, nbModels);
    otbAppLogINFO("RMSE = " << rmse << std::endl);
  }

  template <typename RegressionType>
  double EstimateRegressionModel(RegressionType rgrsn, 
                                 ListInputSampleType::Pointer ils, 
                                 ListOutputSampleType::Pointer ols, 
                                 unsigned int nbModels=1)
  {
    double min_rmse{std::numeric_limits<double>::max()};
    auto sIt = ils->Begin();
    auto rIt = ols->Begin();
    auto total_n_samples = ils->Size();
    otbAppLogINFO("Selecting best of " << nbModels << " models." 
                  << " Total nb samples is " << total_n_samples << std::endl);
    for(auto iteration=0; iteration<nbModels; ++iteration)
      {
      auto ils_slice = ListInputSampleType::New();
      auto ols_slice = ListOutputSampleType::New();

      ils_slice->SetMeasurementVectorSize(ils->GetMeasurementVectorSize());
      ols_slice->SetMeasurementVectorSize(1);

      for(auto nsamples=0; nsamples<total_n_samples/nbModels; ++nsamples)
        {
        ils_slice->PushBack(sIt.GetMeasurementVector());
        ols_slice->PushBack(rIt.GetMeasurementVector());
        ++sIt;
        ++rIt;
        }
      rgrsn->SetInputListSample(ils_slice);
      rgrsn->SetTargetListSample(ols_slice);
      otbAppLogINFO("Model estimation ..." << std::endl);
      rgrsn->Train();
      otbAppLogINFO("Estimation of prediction error from training samples ..."
                    << std::endl);
      auto nbSamples = 0;
      auto rmse = 0.0;
      auto sampleIt = ils_slice->Begin();
      auto resultIt = ols_slice->Begin();
      while(sampleIt != ils_slice->End() && resultIt != ols_slice->End())
        {
        rmse += pow(rgrsn->Predict(sampleIt.GetMeasurementVector())[0] -
                    resultIt.GetMeasurementVector()[0], 2.0);
        ++sampleIt;
        ++resultIt;
        ++nbSamples;
        }
      rmse = sqrt(rmse)/nbSamples;
      otbAppLogINFO("RMSE for model number "<< iteration+1 
                    << " = " << rmse << " using " << nbSamples 
                    << " samples. " << std::endl);
      if(rmse<min_rmse) 
        {
        min_rmse=rmse;
        rgrsn->Save(GetParameterString("out"));
        otbAppLogINFO("Selecting model number " << iteration+1 << std::endl);
        }
      }
    return min_rmse;
  }

  double EstimateNNRegresionModel(ListInputSampleType::Pointer ils, 
                                  ListOutputSampleType::Pointer ols, 
                                  std::size_t nbVars, unsigned int nbModels)
  {
    otbAppLogINFO("Neural networks");
    auto regression = NeuralNetworkType::New();
    regression->SetTrainMethod(CvANN_MLP_TrainParams::BACKPROP);
    // Two hidden layer with 5 neurons and one output variable
    regression->SetLayerSizes(std::vector<unsigned int>(
      {static_cast<unsigned int>(nbVars), 5, 5, 1}));
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
    return EstimateRegressionModel(regression, ils, ols, nbModels);
  }

  double EstimateSVRRegresionModel(ListInputSampleType::Pointer ils, 
                                   ListOutputSampleType::Pointer ols, 
                                   std::size_t nbVars, unsigned int nbModels)
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
    return EstimateRegressionModel(regression, ils, ols, nbModels);
  }

  double EstimateMLRRegresionModel(ListInputSampleType::Pointer ils, 
                                   ListOutputSampleType::Pointer ols, 
                                   std::size_t nbVars, unsigned int nbModels)
  {
    otbAppLogINFO("Multilinear regression");
    auto regression = MLRType::New();
    return EstimateRegressionModel(regression, ils, ols, nbModels);
  }
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::InverseModelLearning)

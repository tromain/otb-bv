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
#include <cmath>

#include "otbBVUtil.h"
#include "otbBVTypes.h"

#include "otbMachineLearningModelFactory.h"
#include "otbNeuralNetworkMachineLearningModel.h"
#include "otbSVMMachineLearningModel.h"
#include "otbRandomForestsMachineLearningModel.h"
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

  using PrecisionType = otb::BV::PrecisionType;
  typedef itk::FixedArray<PrecisionType, 1> OutputSampleType;
  typedef itk::VariableLengthVector<PrecisionType> InputSampleType;
  typedef itk::Statistics::ListSample<OutputSampleType> ListOutputSampleType;
  typedef itk::Statistics::ListSample<InputSampleType> ListInputSampleType;
  typedef otb::NeuralNetworkMachineLearningModel<PrecisionType, 
                                                          PrecisionType> 
  NeuralNetworkType;
  typedef otb::SVMMachineLearningModel<PrecisionType, PrecisionType> SVRType;
  typedef otb::RandomForestsMachineLearningModel<PrecisionType, 
                                                 PrecisionType> RFRType;
  typedef otb::MultiLinearRegressionModel<PrecisionType> MLRType;
  
private:
  void DoInit() override
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

    AddParameter(ParameterType_OutputFilename, "errest", 
                 "Regression model for the error.");
    SetParameterDescription( "errest", 
                             "Filename where the regression model for the estimation of the regression error will be saved." );
    MandatoryOff("errest");

    AddParameter(ParameterType_OutputFilename, "normalization", 
                 "Output file containing min and max values per sample component.");
    SetParameterDescription( "normalization", 
                             "Output file containing min and max values per sample component. This file can be used by the inversion application. If no file is given as parameter, the variables are not normalized." );
    MandatoryOff("normalization");

    AddParameter(ParameterType_String, "regression", 
                 "Regression to use for the training (nn, svr, rfr, mlr)");
    SetParameterDescription("regression", 
                            "Choice of the regression to use for the training: svr, rfr, nn, mlr.");
    MandatoryOff("regression");

    AddParameter(ParameterType_Int, "bestof", "Select the best of N models.");
    SetParameterDescription("bestof", "");
    MandatoryOff("bestof");

  }

  virtual ~InverseModelLearning() override
  {
  }


  void DoUpdateParameters() override
  {
    // Nothing to do here : all parameters are independent
  }

  
  void DoExecute() override
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

    // Samples for the error estimation
    auto inputListSample_err = ListInputSampleType::New();
    auto outputListSample_err = ListOutputSampleType::New();

    inputListSample_err->SetMeasurementVectorSize(nbInputVariables);
    outputListSample_err->SetMeasurementVectorSize(1);

    auto nbSamples = read_input_samples(trainingFile, nbInputVariables, 
                                        inputListSample, outputListSample, 
                                        inputListSample_err, 
                                        outputListSample_err);
    otbAppLogINFO("Found " << nbSamples << " samples in "
                  << trainingFileName << std::endl);
    trainingFile.close();

    double rmse{0.0};
    std::string regressor_type{"nn"};
    unsigned int nbModels{1};
    if (IsParameterEnabled("bestof"))
      nbModels = static_cast<unsigned int>(GetParameterInt("bestof"));    
    if (IsParameterEnabled("regression"))
      regressor_type = GetParameterString("regression");    
    if (regressor_type == "svr")
      rmse = EstimateSVRRegresionModel(inputListSample, outputListSample, 
                                       nbModels);
    if (regressor_type == "rfr")
      rmse = EstimateRFRRegresionModel(inputListSample, outputListSample, 
                                       nbModels);
    else if (regressor_type == "nn")
      rmse = EstimateNNRegresionModel(inputListSample, outputListSample, 
                                      nbModels, nbInputVariables);
    else if (regressor_type == "mlr")
      rmse = EstimateMLRRegresionModel(inputListSample, outputListSample, 
                                       nbModels);
    otbAppLogINFO("RMSE = " << rmse << std::endl);
    if (IsParameterEnabled("errest"))
      {
      otbAppLogINFO("Learning regression model for the error " << std::endl);

      if (regressor_type == "svr")       
        EstimateErrorModel<SVRType>(inputListSample_err, 
                                    outputListSample_err,
                                    nbInputVariables);
      if (regressor_type == "rfr")
        EstimateErrorModel<RFRType>(inputListSample_err, 
                                    outputListSample_err,
                                    nbInputVariables);
      else if (regressor_type == "nn")
        EstimateErrorModel<NeuralNetworkType>(inputListSample_err, 
                                              outputListSample_err,
                                              nbInputVariables);
      else if (regressor_type == "mlr")
        EstimateErrorModel<MLRType>(inputListSample_err, 
                                    outputListSample_err,
                                    nbInputVariables);
      }
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
    for(size_t iteration=0; iteration<nbModels; ++iteration)
      {
      auto ils_slice = ListInputSampleType::New();
      auto ols_slice = ListOutputSampleType::New();

      ils_slice->SetMeasurementVectorSize(ils->GetMeasurementVectorSize());
      ols_slice->SetMeasurementVectorSize(1);

      for(size_t nsamples=0; nsamples<total_n_samples/nbModels; ++nsamples)
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
                                  unsigned int nbModels, std::size_t nbVars)
  {
    otbAppLogINFO("Neural networks");
    auto regression = NeuralNetworkType::New();
    regression->SetRegressionMode(true);
    regression->SetTrainMethod(CvANN_MLP_TrainParams::BACKPROP);
    // One hidden layer with 5 neurons and one output variable
    otbAppLogINFO("Input layer : " << nbVars);
    regression->SetLayerSizes(std::vector<unsigned int>(
      {static_cast<unsigned int>(nbVars), 5, 1}));
    regression->SetActivateFunction(CvANN_MLP::SIGMOID_SYM);
    regression->SetAlpha(0.5);
    regression->SetBeta(1.0);
    regression->SetBackPropDWScale(0.1);
    regression->SetBackPropMomentScale(0.1);
    regression->SetTermCriteriaType(CV_TERMCRIT_EPS);
    regression->SetEpsilon(1e-10);
    return EstimateRegressionModel(regression, ils, ols, nbModels);
  }

  double EstimateSVRRegresionModel(ListInputSampleType::Pointer ils, 
                                   ListOutputSampleType::Pointer ols, 
                                   unsigned int nbModels)
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

  double EstimateRFRRegresionModel(ListInputSampleType::Pointer ils, 
                                   ListOutputSampleType::Pointer ols, 
                                   unsigned int nbModels)
  {
    otbAppLogINFO("Support vectors");
    auto regression = RFRType::New();
    regression->SetMaxDepth(10);
    regression->SetMinSampleCount(1000);
    regression->SetRegressionAccuracy(0.01);
    regression->SetMaxNumberOfVariables(4);
    regression->SetMaxNumberOfTrees(100);
    regression->SetForestAccuracy(0.01);
    regression->SetRegressionMode(true);
    return EstimateRegressionModel(regression, ils, ols, nbModels);
  }

  double EstimateMLRRegresionModel(ListInputSampleType::Pointer ils, 
                                   ListOutputSampleType::Pointer ols, 
                                   unsigned int nbModels)
  {
    otbAppLogINFO("Multilinear regression");
    auto regression = MLRType::New();
    return EstimateRegressionModel(regression, ils, ols, nbModels);
  }

  template <typename RegressionType>
  void EstimateErrorModel(ListInputSampleType::Pointer ils, 
                          ListOutputSampleType::Pointer ols,
                          std::size_t nbVars)
  {
    // Generate the values of the error
    auto bv_regression = RegressionType::New();
    bv_regression->Load(GetParameterString("out"));    
    bv_regression->SetRegressionMode(true);

    auto err_ls = ListOutputSampleType::New();
    auto sIt = ils->Begin();
    auto rIt = ols->Begin();
    while(sIt != ils->End() && rIt != ols->End())
      {
      auto est_err = (bv_regression->Predict(sIt.GetMeasurementVector())[0] -
                      rIt.GetMeasurementVector()[0]);
      OutputSampleType outputValue;
      if( HasValue( "normalization" )==true )
        // we use the same normalization as for the BV
        outputValue[0] = otb::BV::normalize(est_err, var_minmax[nbVars]);
      else
        outputValue[0] = est_err;
      err_ls->PushBack(outputValue);
      ++sIt;
      ++rIt;
      }

    auto err_regression = NeuralNetworkType::New();
    err_regression->SetRegressionMode(1);
    err_regression->SetTrainMethod(CvANN_MLP_TrainParams::RPROP);
    // One hidden layer with 5 neurons and one output variable
    err_regression->SetLayerSizes(std::vector<unsigned int>(
                                    {static_cast<unsigned int>(nbVars), 5, 1}));
    err_regression->SetActivateFunction(CvANN_MLP::SIGMOID_SYM);
    err_regression->SetAlpha(1.0);
    err_regression->SetBeta(1.0);
    err_regression->SetBackPropDWScale(0.1);
    err_regression->SetBackPropMomentScale(0.1);
    err_regression->SetTermCriteriaType(CV_TERMCRIT_EPS);
    err_regression->SetEpsilon(1e-7);
    err_regression->SetInputListSample(ils);
    err_regression->SetTargetListSample(err_ls);
    otbAppLogINFO("Error model estimation ..." << std::endl);
    err_regression->Train();
    err_regression->Save(GetParameterString("errest"));
  }

  std::size_t read_input_samples(std::ifstream& trainingFile, 
                                 std::size_t nbInputVariables,
                          ListInputSampleType::Pointer inputListSample,
                          ListOutputSampleType::Pointer outputListSample,
                          ListInputSampleType::Pointer inputListSample_err,
                          ListOutputSampleType::Pointer outputListSample_err)
  {
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
        bool has_nan = false;
        for(size_t var = 0; var < nbInputVariables; ++var)
          {
          bool nan_var = false;
          std::tie(nan_var, inputValue[var]) = read_value_or_nan(ss);
          has_nan = has_nan || nan_var;
          }
        if(!has_nan)
          {
          if (IsParameterEnabled("errest") && (nbSamples%2 == 0))
            {
            inputListSample_err->PushBack(inputValue);
            outputListSample_err->PushBack(outputValue);
            }
          else
            {
            inputListSample->PushBack(inputValue);
            outputListSample->PushBack(outputValue);
            }
          ++nbSamples;
          }
        }
      }

    if( HasValue( "normalization" )==true )
      {
      otbAppLogINFO("Variable normalization."<< std::endl);
      typename ListInputSampleType::Iterator ilFirst = inputListSample->Begin();
      typename ListOutputSampleType::Iterator olFirst = outputListSample->Begin();
      typename ListInputSampleType::Iterator ilLast = inputListSample->End();
      typename ListOutputSampleType::Iterator olLast = outputListSample->End();
      var_minmax = otb::BV::estimate_var_minmax(ilFirst, ilLast, olFirst, olLast);
      otb::BV::write_normalization_file(var_minmax, GetParameterString("normalization"));
      otb::BV::normalize_variables(inputListSample, outputListSample, var_minmax);
      for(size_t var = 0; var < nbInputVariables; ++var)
        otbAppLogINFO("Variable "<< var+1 << " min=" << var_minmax[var].first <<
                      " max=" << var_minmax[var].second <<std::endl);
      otbAppLogINFO("Output min=" << var_minmax[nbInputVariables].first <<
                    " max=" << var_minmax[nbInputVariables].second <<std::endl)
        }
    return nbSamples;

  }
  std::tuple<bool, PrecisionType> read_value_or_nan(std::istringstream& ss)
  {
    bool nan_var = false;
    std::string in_value;
    ss >> in_value;
    if(in_value=="nan")
      {
      nan_var = true;
      }
    return std::make_tuple(nan_var,
                           static_cast<PrecisionType>(std::stod(in_value)));
  }
protected:
  otb::BV::NormalizationVectorType var_minmax;
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::InverseModelLearning)

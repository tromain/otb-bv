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
#include "itkListSample.h"

namespace otb
{

using NormalizationVectorType = std::vector<std::pair<PrecisionType, PrecisionType>>;
template<typename II, typename OI>
NormalizationVectorType estimate_var_minmax(II& ivIt, II& ivLast, OI& ovIt, OI& ovLast)
{

  std::size_t nbInputVariables{ivIt.GetMeasurementVector().Size()};
  NormalizationVectorType var_minmax{nbInputVariables+1, {std::numeric_limits<PrecisionType>::max(), std::numeric_limits<PrecisionType>::min()}};
      while(ovIt != ovLast &&
            ivIt != ivLast)
        {
        auto ov = ovIt.GetMeasurementVector()[0];
        if(ov<var_minmax[nbInputVariables].first)
            var_minmax[nbInputVariables].first = ov;
        if(ov>var_minmax[nbInputVariables].second)
          var_minmax[nbInputVariables].second = ov;
        for(auto var = 0; var < nbInputVariables; ++var)
          {
          auto iv = ivIt.GetMeasurementVector()[var];
          if(iv<var_minmax[var].first)
            var_minmax[var].first = iv;
          if(iv>var_minmax[var].second)
            var_minmax[var].second = iv;
          }
        ++ovIt;
        ++ivIt;
        }
      return var_minmax;
}

template<typename NVT>
void write_normalization_file(const NVT& var_minmax, std::string out_filename)
{
  std::ofstream norm_file{out_filename};
  for(auto val : var_minmax)
    {
    norm_file << std::setprecision(8);
    norm_file << std::setw(20) << std::left << val.first;
    norm_file << std::setw(20) << std::left << val.second;
    norm_file << std::endl;
    }
}

template<typename T, typename U>
inline
T normalize(T x, U p)
{
  return (x-p.first)/(p.second-p.first+std::numeric_limits<T>::epsilon());
}

template<typename IS, typename OS, typename NVT>
void normalize_variables(IS& isl, OS& osl, const NVT& var_minmax)
{
  auto ivIt = isl->Begin();
  auto ovIt = osl->Begin();
  auto ivLast = isl->End();
  auto ovLast = osl->End();

  std::size_t nbInputVariables{ivIt.GetMeasurementVector().Size()};
  while(ovIt != ovLast &&
        ivIt != ivLast)
    {
    auto ovInstId = ovIt.GetInstanceIdentifier();
    osl->SetMeasurement(ovInstId, 0, normalize(ovIt.GetMeasurementVector()[0],var_minmax[nbInputVariables]));
    auto ivInstId = ivIt.GetInstanceIdentifier();
    for(auto var = 0; var < nbInputVariables; ++var)
      {
      isl->SetMeasurement(ivInstId, var, normalize(ovIt.GetMeasurementVector()[var],var_minmax[var]));
      }
    ++ovIt;
    ++ivIt;
    }
}


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
    auto classifier = NeuralNetworkType::New();
    classifier->SetInputListSample(inputListSample);
    classifier->SetTargetListSample(outputListSample);
    classifier->SetTrainMethod(CvANN_MLP_TrainParams::BACKPROP);
    // Two hidden layer with 5 neurons and one output variable
    classifier->SetLayerSizes(std::vector<unsigned int>({static_cast<unsigned int>(nbInputVariables), 5, 5, 1}));
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

OTB_APPLICATION_EXPORT(otb::Wrapper::InverseModelLearning)

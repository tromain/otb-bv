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
#include <memory>

#include "otbBVUtil.h"

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
  typedef MachineLearningModel<PrecisionType, PrecisionType> ModelType;
  typedef otb::NeuralNetworkMachineLearningModel<PrecisionType, 
                                                          PrecisionType> 
  NeuralNetworkType;
  typedef otb::RandomForestsMachineLearningModel<PrecisionType, 
                                                 PrecisionType> RFRType;
  typedef otb::SVMMachineLearningModel<PrecisionType, PrecisionType> SVRType;
  typedef otb::MultiLinearRegressionModel<PrecisionType> MLRType;
  
private:
  void DoInit() override
  {
    SetName("BVInversion");
    SetDescription("Estimate biophysical variables using aninversion of Prospect and Sail");

    AddDocTag(Tags::FeatureExtraction);
    AddDocTag("Description");

    AddParameter(ParameterType_InputFilename, "reflectances", "Input file containing the reflectances to invert");
    SetParameterDescription( "reflectances", "Input file containing the reflectances to invert. This is an ASCII file where each line is a sample. A line is a set of fields containing numerical values. The order of the fields must respect the one used for the training." );
    MandatoryOn("reflectances");

    AddParameter(ParameterType_InputFilename, "model", "File containing the regression model");
    SetParameterDescription( "model", "File containing the regression model.");
    MandatoryOn("model");
    
    AddParameter(ParameterType_OutputFilename, "out", "Output estimated variable");
    SetParameterDescription( "out", "Filename where the estimated variables will be saved." );
    MandatoryOn("out");

    AddParameter(ParameterType_InputFilename, "normalization", "Input file containing min and max values per sample component");
    SetParameterDescription( "normalization", "Input file containing min and max values per sample component. This file can be produced by the invers model learning application. If no file is given as parameter, the variables are not normalized." );
    MandatoryOff("normalization");

    SetExampleComment("Example of use", 0);
    SetDocExampleParameterValue("reflectances", "reflectances.txt");
    SetDocExampleParameterValue("model", "reg_model.txt");
    SetDocExampleParameterValue("out", "estimated_BioVar.txt");
    SetDocExampleParameterValue("normalization", "norm.txt");

    SetOfficialDocLink();
  }

  virtual ~BVInversion() override
  {
  }


  void DoUpdateParameters() override
  {
    // Nothing to do here : all parameters are independent
  }

  
  void DoExecute() override
  {
   
    auto reflectancesFileName = GetParameterString("reflectances");
    std::ifstream reflectancesFile;
    try
      {
      reflectancesFile.open(reflectancesFileName.c_str());
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " 
                               << reflectancesFileName);
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

    size_t nbInputVariables = countColumns(reflectancesFileName);
    otbAppLogINFO("Found " << nbInputVariables << " input variables in "
                  << reflectancesFileName << std::endl);

    otb::BV::NormalizationVectorType var_minmax;
    if( HasValue( "normalization" )==true )
      {
      otbAppLogINFO("Variable normalization."<< std::endl);            
      var_minmax = otb::BV::read_normalization_file(GetParameterString("normalization"));
      if(var_minmax.size()!=nbInputVariables+1)
        itkGenericExceptionMacro(<< "Normalization file ("<< var_minmax.size() 
                                 << " - 1) is not coherent with the number of "
                                 << "input variables ("<< nbInputVariables 
                                 <<").");
      for(size_t var = 0; var < nbInputVariables; ++var)
        otbAppLogINFO("Variable "<< var+1 << " min=" << var_minmax[var].first <<
                      " max=" << var_minmax[var].second <<std::endl);
      otbAppLogINFO("Output min=" << var_minmax[nbInputVariables].first <<
                    " max=" << var_minmax[nbInputVariables].second 
                    << std::endl)
        }
    auto model_file = GetParameterString("model");
    ModelType* regressor;
    auto nn_regressor = NeuralNetworkType::New();
    auto svr_regressor = SVRType::New();
    auto rfr_regressor = RFRType::New();
    auto mlr_regressor = MLRType::New();
    if(nn_regressor->CanReadFile(model_file))
      {
      otbAppLogINFO("Loading model ..." << std::endl);
      regressor = dynamic_cast<ModelType*>(nn_regressor.GetPointer());
      otbAppLogINFO("Applying NN regression ..." << std::endl);
      }
    else if(svr_regressor->CanReadFile(model_file))
      {
      regressor = dynamic_cast<ModelType*>(svr_regressor.GetPointer());
      otbAppLogINFO("Applying SVR regression ..." << std::endl);
      }
    else if(rfr_regressor->CanReadFile(model_file))
      {
      regressor = dynamic_cast<ModelType*>(rfr_regressor.GetPointer());
      otbAppLogINFO("Applying RF regression ..." << std::endl);
      }
    else if(mlr_regressor->CanReadFile(model_file))
      {
      regressor = dynamic_cast<ModelType*>(mlr_regressor.GetPointer());
      otbAppLogINFO("Applying MLR regression ..." << std::endl);
      }
    else
      {
      itkGenericExceptionMacro(<< "Model in file " << model_file 
                               << " is not valid.\n");
      }
    regressor->Load(model_file);    
    regressor->SetRegressionMode(true);


    auto sampleCount = 0;
    for(std::string line; std::getline(reflectancesFile, line); )
      {
      if(line.size() > 1)
        {
        std::istringstream ss(line);
        InputSampleType inputValue;
        inputValue.Reserve(nbInputVariables);
        for(size_t var = 0; var < nbInputVariables; ++var)
          {
          ss >> inputValue[var];
          if( HasValue( "normalization" )==true )
            inputValue[var] = otb::BV::normalize(inputValue[var], var_minmax[var]);
          }
        OutputSampleType outputValue = regressor->Predict(inputValue);
        if( HasValue( "normalization" )==true )
          outputValue[0] = otb::BV::denormalize(outputValue[0],
                                                var_minmax[nbInputVariables]);
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

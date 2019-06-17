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
#include "otbBVEstimation.h"

namespace otb
{

/** Unary functor image filter which produces a vector image with a
* number of bands different from the input images */
template <class TInputImage, class TOutputImage, 
          class TFunctor>
class ITK_EXPORT UnaryFunctorImageFilterWithNBands : 
    public itk::UnaryFunctorImageFilter< TInputImage, TOutputImage, TFunctor >
{
public:
  typedef UnaryFunctorImageFilterWithNBands Self;
  typedef itk::UnaryFunctorImageFilter< TInputImage, TOutputImage, 
                                        TFunctor > Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Macro defining the type*/
  itkTypeMacro(UnaryFunctorImageFilterWithNBands, SuperClass);

  /** Accessors for the number of bands*/
  itkSetMacro(NumberOfOutputBands, unsigned int);
  itkGetConstMacro(NumberOfOutputBands, unsigned int);
  
protected:
  UnaryFunctorImageFilterWithNBands() {}
  virtual ~UnaryFunctorImageFilterWithNBands() {}

  virtual void GenerateOutputInformation()
  {
    Superclass::GenerateOutputInformation();
    this->GetOutput()->SetNumberOfComponentsPerPixel( m_NumberOfOutputBands );
  }
private:
  UnaryFunctorImageFilterWithNBands(const Self &); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  unsigned int m_NumberOfOutputBands;


};

namespace Wrapper
{

class BVImageInversion : public Application
{
public:
/** Standard class typedefs. */
  typedef BVImageInversion     Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  
/** Standard macro */
  itkNewMacro(Self);
  itkTypeMacro(BVImageInversion, otb::Application);

  using FunctorType = Functor::BVEstimation<FloatVectorImageType::PixelType, 
                                            FloatVectorImageType::PixelType>;
  using FilterType = UnaryFunctorImageFilterWithNBands<FloatVectorImageType,
                                                       FloatVectorImageType,
                                                       FunctorType>;
  
private:
  void DoInit() override
  {
    SetName("BVImageInversion");
    SetDescription("Estimate biophysical variables for every pixel of an image using an inversion of Prospect+Sail.");

    AddParameter(ParameterType_InputImage, "in", "Input Image");
    SetParameterDescription("in","Input image.");

    AddParameter(ParameterType_InputFilename, "model", "File containing the regression model.");
    SetParameterDescription( "model", "File containing the regression model.");
    
    AddParameter(ParameterType_OutputImage, "out", "Output Image");
    SetParameterDescription("out","Output image.");

    AddRAMParameter();

    AddParameter(ParameterType_InputFilename, "normalization", "Input file containing min and max values per sample component.");
    SetParameterDescription( "normalization", "Input file containing min and max values per sample component. This file can be produced by the invers model learning application. If no file is given as parameter, the variables are not normalized." );
    MandatoryOff("normalization");

  }

  virtual ~BVImageInversion() override
  {
  }


  void DoUpdateParameters() override
  {
    // Nothing to do here : all parameters are independent
  }

  void DoExecute() override
  {
    // read output info of the input image
    FloatVectorImageType::Pointer input_image = this->GetParameterImage("in");
    auto nb_bands = input_image->GetNumberOfComponentsPerPixel();
    otbAppLogINFO("Input image has " << nb_bands << " bands."<< std::endl);            
    auto nbInputVariables = nb_bands;

    BV::NormalizationVectorType var_minmax{};
    if( HasValue( "normalization" )==true )
      {
      otbAppLogINFO("Variable normalization."<< std::endl);            
      var_minmax = BV::read_normalization_file(GetParameterString("normalization"));
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
    BV::ModelType* regressor;
    auto nn_regressor = BV::NeuralNetworkType::New();
    auto svr_regressor = BV::SVRType::New();
    auto rfr_regressor = BV::RFRType::New();
    auto mlr_regressor = BV::MLRType::New();
    if(nn_regressor->CanReadFile(model_file))
      {
      regressor = dynamic_cast<BV::ModelType*>(nn_regressor.GetPointer());
      otbAppLogINFO("Applying NN regression ..." << std::endl);
      }
    else if(svr_regressor->CanReadFile(model_file))
      {
      regressor = dynamic_cast<BV::ModelType*>(svr_regressor.GetPointer());
      otbAppLogINFO("Applying SVR regression ..." << std::endl);
      }
    else if(rfr_regressor->CanReadFile(model_file))
      {
      regressor = dynamic_cast<BV::ModelType*>(rfr_regressor.GetPointer());
      otbAppLogINFO("Applying RF regression ..." << std::endl);
      }
    else if(mlr_regressor->CanReadFile(model_file))
      {
      regressor = dynamic_cast<BV::ModelType*>(mlr_regressor.GetPointer());
      otbAppLogINFO("Applying MLR regression ..." << std::endl);
      }
    else
      {
      itkGenericExceptionMacro(<< "Model in file " << model_file 
                               << " is not valid.\n");
      }
    regressor->Load(model_file);    
    regressor->SetRegressionMode(true);

    //instantiate a functor with the regressor and pass it to the
    //unary functor image filter pass also the normalization values
    bv_filter = FilterType::New();
    bv_filter->SetFunctor(FunctorType(regressor,var_minmax));
    bv_filter->SetInput(input_image);
    bv_filter->SetNumberOfOutputBands(1);
    SetParameterOutputImage("out", bv_filter->GetOutput());
  }
  FilterType::Pointer bv_filter;
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::BVImageInversion)

/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbNeuralNetworkRegressionMachineLearningModel_h
#define __otbNeuralNetworkRegressionMachineLearningModel_h

#include "otbNeuralNetworkMachineLearningModel.h"

namespace otb
{
template <class TInputValue, class TTargetValue>
class ITK_EXPORT NeuralNetworkRegressionMachineLearningModel
  : public NeuralNetworkMachineLearningModel <TInputValue, TTargetValue>
{
public:
  /** Standard class typedefs. */
  typedef NeuralNetworkRegressionMachineLearningModel           Self;
  typedef NeuralNetworkMachineLearningModel<TInputValue, TTargetValue> Superclass;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;

  // Input related typedefs
  typedef TInputValue                                     InputValueType;
  typedef itk::VariableLengthVector<InputValueType>       InputSampleType;
  typedef itk::Statistics::ListSample<InputSampleType>    InputListSampleType;

  // Target related typedefs
  typedef TTargetValue                                    TargetValueType;
  typedef itk::FixedArray<TargetValueType,1>              TargetSampleType;
  typedef itk::Statistics::ListSample<TargetSampleType>   TargetListSampleType;

  /** Run-time type information (and related methods). */
  itkNewMacro(Self);
  itkTypeMacro(NeuralNetworkRegressionMachineLearningModel, itk::MachineLearningModel);

    /**
   * Set the number of neurons in each layer (including input and output layers).
   * The number of neuron in the first layer (input layer) must be equal
   * to the number of samples in the \c InputListSample
   */
  void SetLayerSizes (const std::vector<unsigned int> layers)
  {
    const unsigned int nbLayers = layers.size();
    if (nbLayers < 3)
      itkExceptionMacro(<< "Number of layers in the Neural Network must be >= 3")

        m_LayerSizes = layers;
  }

  /** Train the machine learning model */
  virtual void Train();

  /** Predict values using the model */
  virtual TargetSampleType Predict(const InputSampleType & input) const;

  /** Save the model to file */
  virtual void Save(const std::string & filename, const std::string & name="");

  /** Load the model from file */
  virtual void Load(const std::string & filename, const std::string & name="");

protected:
  /** Constructor */
  NeuralNetworkRegressionMachineLearningModel();

  /** Destructor */
  virtual ~NeuralNetworkRegressionMachineLearningModel();

  void LabelsToMat(const TargetListSampleType * listSample, cv::Mat & output);

  /** PrintSelf method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  NeuralNetworkRegressionMachineLearningModel(const Self &); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  CvANN_MLP * m_ANNModel;
  int m_TrainMethod;
  int m_ActivateFunction;
  std::vector<unsigned int> m_LayerSizes;
  double m_Alpha;
  double m_Beta;
  double m_BackPropDWScale;
  double m_BackPropMomentScale;
  double m_RegPropDW0;
  double m_RegPropDWMin;
  int m_TermCriteriaType;
  int m_MaxIter;
  double m_Epsilon;


};
} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbNeuralNetworkRegressionMachineLearningModel.txx"
#endif

#endif

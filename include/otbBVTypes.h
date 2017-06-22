/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OTBBVTYPES_H
#define __OTBBVTYPES_H

#include <map>
#include <vector>

#include "otbMachineLearningModelFactory.h"
#include "otbNeuralNetworkMachineLearningModel.h"
#include "otbSVMMachineLearningModel.h"
#include "otbRandomForestsMachineLearningModel.h"
#include "otbMultiLinearRegressionModel.h"
#include "itkListSample.h"

namespace otb
{

namespace BV
{
enum class IVNames {MLAI, ALA, CrownCover, HsD, N, Cab, Car, Cdm, CwRel, Cbp, 
                    Bs, SoilIndex, IVNamesEnd};
enum class AcquisitionParameters {TTS, TTO, PSI, TTS_FAPAR, AcquisitionParametersEnd};

using AcquisitionParsType = std::map< AcquisitionParameters, double >;
using PrecisionType = double;
using BVType = std::map< IVNames, PrecisionType >;

using NormalizationVectorType = 
  std::vector<std::pair<PrecisionType, PrecisionType>>;

enum class DistType {GAUSSIAN, UNIFORM, LOGNORMAL};

struct VarParams {
  double min;
  double max;
  double mod;
  double std;
  double Min_LAI_Max;
  double Max_LAI_Max;
  bool CoDistrib;
  unsigned short  nbcl;
  DistType dist;
};
  
typedef std::map< IVNames, double > SampleType;


typedef itk::FixedArray<PrecisionType, 1> OutputSampleType;
typedef itk::VariableLengthVector<PrecisionType> InputSampleType;
typedef itk::Statistics::ListSample<OutputSampleType> ListOutputSampleType;
typedef itk::Statistics::ListSample<InputSampleType> ListInputSampleType;
typedef otb::MachineLearningModel<PrecisionType, PrecisionType> ModelType;
typedef ModelType::Pointer ModelPointerType;
typedef otb::NeuralNetworkMachineLearningModel<PrecisionType, 
                                               PrecisionType> 
NeuralNetworkType;
typedef otb::RandomForestsMachineLearningModel<PrecisionType, 
                                               PrecisionType> RFRType;
typedef otb::SVMMachineLearningModel<PrecisionType, PrecisionType> SVRType;
typedef otb::MultiLinearRegressionModel<PrecisionType> MLRType;

}//namespace BV
}//namespace otb
#endif

/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbBVInputVariableGeneration.h"
#include <random>
#include <fstream>


namespace otb
{

namespace BV
{
template<typename RNGType>
double Rng(VarParams vpars, RNGType rngen)
{
  //TODO : why us stdev defined for uniform?
  double min = vpars.min;
  double max = vpars.max;
  double mod = vpars.mod;
  double stdev = vpars.std;
  DistType dist = vpars.dist;

  double rn;
  if(dist == DistType::GAUSSIAN)
    {
    auto sampleInsideBounds = false;
    while(!sampleInsideBounds)
      {
      std::normal_distribution<> d(mod,stdev);
      rn = d(rngen);
      if( rn >= min && rn <= max)
        sampleInsideBounds = true;
      }
    }
  if(dist == DistType::LOGNORMAL)
    {
    auto sampleInsideBounds = false;
    while(!sampleInsideBounds)
      {
      std::lognormal_distribution<> d(mod, stdev);
      rn = d(rngen);
      if( rn >= min && rn <= max)
        sampleInsideBounds = true;
      }
    }
  else
    {
    std::uniform_real_distribution<> d(min, max);
    rn = d(rngen);
    }




  return rn;
}

/**

        V* = (V-Vmin(0))*(Vmax(LAI)-Vmin(LAI))/(Vmax(0)-Vmin(0))+Vmin(LAI)
        p 32 of ATBD_BioVar_Venµs_V1.0.pdf

  */
double CorrelateValue(double v, double lai, VarParams vpars)
{
  double Vmin0 = vpars.min;
  double Vmax0 = vpars.max;
  double VminLAImax = vpars.Min_LAI_Max;
  double VmaxLAImax = vpars.Max_LAI_Max;
  bool codist = vpars.CoDistrib;
  if(codist)
    {
    double VminLAI = Vmin0+lai*(VminLAImax-Vmin0);
    double VmaxLAI = Vmax0+lai*(VmaxLAImax-Vmax0);
    double res = (v-Vmin0)/(Vmax0-Vmin0)*(VmaxLAI-VminLAI)+VminLAI;
    return res<0?0:res;
    }
  else
    return v;
}

}//namespace BV 
namespace Wrapper
{

  void BVInputVariableGeneration::DoInit()
  {
    SetName("BVInputVariableGeneration");
    SetDescription("Generate random input variable distribution for ... .");

    AddDocTag(Tags::BV);
    AddParameter(ParameterType_Int, "samples", "Sample size");
    SetDefaultParameterInt("samples", 1000);
    SetParameterDescription("samples", "Number of samples to be generated");
    
    AddParameter(ParameterType_OutputFilename, "out", "Output file");
    SetParameterDescription( "out", "Filename where the variable sets are saved." );
    MandatoryOn("out");

    AddParameter(ParameterType_Float, "minlai", "Minimum value for LAI");
    SetDefaultParameterFloat("minlai", 0.0);
    SetParameterDescription("minlai", "Minimum value for LAI");

    AddParameter(ParameterType_Float, "maxlai", "Maximum value for LAI");
    SetDefaultParameterFloat("maxlai", 15.0);
    SetParameterDescription("maxlai", "Maximum value for LAI");

    AddParameter(ParameterType_Float, "modlai", "Mode value for LAI");
    SetDefaultParameterFloat("modlai", 2.0);
    SetParameterDescription("modlai", "Mode value for LAI");

    AddParameter(ParameterType_Float, "stdlai", "Standard deviation value for LAI");
    SetDefaultParameterFloat("stdlai", 2.0);
    SetParameterDescription("stdlai", "Standard deviation value for LAI");

    AddParameter(ParameterType_String, 
                 "distlai", "Probability distribution for LAI [normal|lognormal(default)]");
    SetParameterDescription("distlai", "Probability distribution for LAI (normal,lognormal)");
    MandatoryOff("distlai");

    AddParameter(ParameterType_Float, "minala", "Minimum value for ALA");
    SetDefaultParameterFloat("minala", 30.0);
    SetParameterDescription("minala", "Minimum value for ALA");

    AddParameter(ParameterType_Float, "maxala", "Maximum value for ALA");
    SetDefaultParameterFloat("maxala", 80.0);
    SetParameterDescription("maxala", "Maximum value for ALA");

    AddParameter(ParameterType_Float, "modala", "Mode value for ALA");
    SetDefaultParameterFloat("modala", 60.0);
    SetParameterDescription("modala", "Mode value for ALA");

    AddParameter(ParameterType_Float, "stdala", "Standard deviation value for ALA");
    SetDefaultParameterFloat("stdala", 20.0);
    SetParameterDescription("stdala", "Standard deviation value for ALA");

  }

  
///Builds the map with the values of the sample
otb::BV::SampleType BVInputVariableGeneration::DrawSample()
{
  using namespace otb::BV;
  SampleType s;
  s[IVNames::MLAI] = Rng(m_MLAI, m_RNG);
  s[IVNames::ALA] = CorrelateValue(Rng(m_ALA, m_RNG), 
                                   s[IVNames::MLAI], m_ALA);
  s[IVNames::CrownCover] = CorrelateValue(Rng(m_CrownCover, m_RNG), 
                                          s[IVNames::MLAI], m_CrownCover);
  s[IVNames::HsD] = CorrelateValue(Rng(m_HsD, m_RNG), 
                                   s[IVNames::MLAI], m_HsD);
  s[IVNames::N] = CorrelateValue(Rng(m_N, m_RNG), 
                                 s[IVNames::MLAI], m_N);
  s[IVNames::Cab] = CorrelateValue(Rng(m_Cab, m_RNG), 
                                   s[IVNames::MLAI], m_Cab);
  s[IVNames::Car] = s[IVNames::Cab]*0.25;
  s[IVNames::Cdm] = CorrelateValue(Rng(m_Cdm, m_RNG), 
                                   s[IVNames::MLAI], m_Cdm);
  s[IVNames::CwRel] = CorrelateValue(Rng(m_CwRel, m_RNG), 
                                     s[IVNames::MLAI], m_CwRel);
  s[IVNames::Cbp] = CorrelateValue(Rng(m_Cbp, m_RNG), 
                                   s[IVNames::MLAI], m_Cbp);
  s[IVNames::Bs] = CorrelateValue(Rng(m_Bs,m_RNG), 
                                        s[IVNames::MLAI], m_Bs);

  return s;
}

void BVInputVariableGeneration::WriteSample(otb::BV::SampleType s)
{
  auto si = s.begin();
  while( si != s.end())
    {
    m_SampleFile << std::setw(12) << std::left << (*si).second ;
    ++si;
    }
  m_SampleFile << std::endl;
}

void BVInputVariableGeneration::DoExecute()
{
  /*
     Car --> not used by bvnet; Féret's dissertation uses Cxc to denote Car
     and gives a mean of 8.58 and a stdev of 3.95 and fig 2.2, p.47 gives the
     min at 0 and the max at 25
     
  */

    m_MLAI.min = GetParameterFloat("minlai");
    m_MLAI.max = GetParameterFloat("maxlai");
    m_MLAI.mod = GetParameterFloat("modlai");
    m_MLAI.std = GetParameterFloat("stdlai");

    if(IsParameterEnabled("distlai"))
      {
      if( GetParameterString("distlai") == "normal" )
        {
        m_MLAI.dist = otb::BV::DistType::GAUSSIAN;
        }
      }
    if( m_MLAI.dist == otb::BV::DistType::GAUSSIAN)
      {
      otbAppLogINFO("LAI distribution is normal\n");
      }
    else
      {
      otbAppLogINFO("LAI distribution is lognormal\n");
      }

    m_ALA.min = GetParameterFloat("minala");
    m_ALA.max = GetParameterFloat("maxala");
    m_ALA.mod = GetParameterFloat("modala");
    m_ALA.std = GetParameterFloat("stdala");

    try
      {
      m_SampleFile.open(GetParameterString("out").c_str(), std::ofstream::out);
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " << GetParameterString("out"));
      }

    m_SampleFile << std::setprecision(4);
    m_SampleFile << std::setw(12) << std::left << "MLAI";
    m_SampleFile << std::setw(12) << std::left<< "ALA";
    m_SampleFile << std::setw(12) << std::left    << "CrownCover";
    m_SampleFile << std::setw(12) << std::left    << "HsD";
    m_SampleFile << std::setw(12) << std::left    << "N";
    m_SampleFile << std::setw(12) << std::left    << "Cab";
    m_SampleFile << std::setw(12) << std::left    << "Car";
    m_SampleFile << std::setw(12) << std::left    << "Cdm";
    m_SampleFile << std::setw(12) << std::left    << "CwRel";
    m_SampleFile << std::setw(12) << std::left    << "Cbp";
    m_SampleFile << std::setw(12) << std::left    << "Bs" << std::endl;
    
    auto maxSamples = GetParameterInt("samples");
    auto sampleCount = 0;

    m_RNG = std::mt19937(std::random_device{}());

    otbAppLogINFO("Generating BV samples" << std::endl);
    while(sampleCount < maxSamples)
      {
      this->WriteSample( this->DrawSample() );
      ++sampleCount;
      }
    m_SampleFile.close();

    otbAppLogINFO("" << sampleCount << " samples generated and saved in "
                  << GetParameterString("out") << std::endl);
  }

}//namespace Wrapper
}//namespace otb

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
#include <thread>
#include <boost/lexical_cast.hpp>
#include <random>

#include "otbBVUtil.h"
#include "otbProSailSimulatorFunctor.h"

namespace otb
{



namespace Wrapper
{

class ProSailSimulator : public Application
{
public:
/** Standard class typedefs. */
  typedef ProSailSimulator     Self;
  typedef Application                   Superclass;
  
/** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(ProSailSimulator, otb::Application);

  typedef double PrecisionType;
  typedef otb::SatelliteRSR<PrecisionType, PrecisionType>  SatRSRType;
  typedef Functor::ProSailSimulator<SatRSRType> ProSailType;
  typedef typename ProSailType::OutputType SimulationType;
  
private:
  void DoInit()
  {
    SetName("ProSailSimulator");
    SetDescription("Simulate reflectances, fcover and fapar using Prospect+Sail.");

    AddParameter(ParameterType_InputFilename, "bvfile", "Input file containing the bv samples.");
    SetParameterDescription( "bvfile", "Input file containing the biophysical variable samples. It can be generated using the BVInputVariableGeneration application." );
    MandatoryOn("bvfile");

    AddParameter(ParameterType_InputFilename, "soilfile", "Input file containing the soil spectra.");
    SetParameterDescription( "soilfile", "Input file containing ." );
    MandatoryOn("soilfile");

    AddParameter(ParameterType_InputFilename, "rsrfile", "Input file containing the relative spectral responses.");
    SetParameterDescription( "rsrfile", "Input file containing ." );
    MandatoryOn("rsrfile");
    
    AddParameter(ParameterType_OutputFilename, "out", "Output file");
    SetParameterDescription( "out", "Filename where the simulations are saved. The last 2 bands are fcover and fapar." );
    MandatoryOn("out");

    AddParameter(ParameterType_Float, "solarzenith", "");
    SetParameterDescription( "solarzenith", "." );
    MandatoryOn("solarzenith");

    AddParameter(ParameterType_Float, "solarzenithf", "");
    SetParameterDescription( "solarzenithf", "Solar zenith for the fAPAR simulation" );
    MandatoryOff("solarzenithf");
    
    AddParameter(ParameterType_Float, "sensorzenith", "");
    SetParameterDescription( "sensorzenith", "." );
    MandatoryOn("sensorzenith");

    AddParameter(ParameterType_Float, "azimuth", "");
    SetParameterDescription( "azimuth", "." );
    MandatoryOn("azimuth");

    AddParameter(ParameterType_StringList, "noisevar", "Variance of the noise to be added per band");
    SetParameterDescription("noisevar",
                            "Variance of the noise to be added per band.");
    MandatoryOff("noisevar");


  }

  virtual ~ProSailSimulator()
  {
  }


  void DoUpdateParameters()
  {
    // Nothing to do here : all parameters are independent
  }

  void WriteSimulation(SimulationType simu)
  {
    for(auto i=0; i<simu.size(); ++i)
      m_SimulationsFile << simu[i] << " " ;
    m_SimulationsFile << std::endl;
  }
  
  void DoExecute()
  {
    m_Azimuth = GetParameterFloat("azimuth");
    m_SolarZenith = GetParameterFloat("solarzenith");
    m_SolarZenith_Fapar = m_SolarZenith;
    if(IsParameterEnabled("solarzenithf"))
      m_SolarZenith_Fapar = GetParameterFloat("solarzenithf");
    m_SensorZenith = GetParameterFloat("sensorzenith");
    std::string rsrFileName = GetParameterString("rsrfile");
    //The first 2 columns of the rsr file correspond to the wavelenght and the solar radiation
    short int nbBands = countColumns(rsrFileName)-2;
    otbAppLogINFO("Simulating " << nbBands << " spectral bands."<<std::endl);
    auto satRSR = SatRSRType::New();
    satRSR->SetNbBands(nbBands);
    satRSR->SetSortBands(false);
    satRSR->Load(rsrFileName);

    
    std::stringstream ss;
    ss << "Bands for sensor" << std::endl;
    for(auto i = 0; i< nbBands; ++i)
      ss << i << " " << (satRSR->GetRSR())[i]->GetInterval().first
         << " " << (satRSR->GetRSR())[i]->GetInterval().second
         << std::endl;

    otbAppLogINFO(""<<ss);

    bool add_noise =IsParameterEnabled("noisevar");
    std::vector<std::normal_distribution<>> noise_generators;
    std::mt19937 RNG;
    if(add_noise)
      {
      RNG = std::mt19937(std::random_device{}());
      std::vector<std::string> var_str = GetParameterStringList("noisevar");
      if(var_str.size()==1)
        {
        var_str = std::vector<std::string>(nbBands, var_str[0]);
        otbAppLogINFO("All noise variances initialized to " << var_str[0] << "\n");
        }
      else if(var_str.size()!=nbBands)
        {
        itkGenericExceptionMacro(<< "Number of noise variances (" << var_str.size()
                                 << ") does not match number of spectral bands in "
                                 << rsrFileName << ": " << nbBands);
        }
      for(auto i=0; i<var_str.size(); i++)
        {
        noise_generators.push_back(
          std::normal_distribution<>(0,
                                     boost::lexical_cast<double>(var_str[i])));
        otbAppLogINFO("Noise variance for band " << i << " equal to " << var_str[0] << "\n");
        }
      }    

    std::string bvFileName = GetParameterString("bvfile");
    std::string outFileName = GetParameterString("out");

    try
      {
      m_SampleFile.open(bvFileName.c_str());
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " << bvFileName);
      }

    try
      {
      m_SimulationsFile.open(outFileName.c_str(), std::ofstream::out);
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " << outFileName);
      }    

    AcquisitionParsType prosailPars;
    prosailPars[TTS] = m_SolarZenith;
    prosailPars[TTS_FAPAR] = m_SolarZenith_Fapar;
    prosailPars[TTO] = m_SensorZenith;
    prosailPars[PSI] = m_Azimuth;
    
    //TODO : the soil file is not used --> implement a version of Sail using an external soil model instead of DataSpecP5B, then multipmy by Bs
    
    otbAppLogINFO("Processing simulations ..." << std::endl)
    //read variable names (first line)
    std::string line;
    std::getline(m_SampleFile, line);

    std::vector<BVType> bv_vec{};
    while(m_SampleFile.good())
      {
      BVType prosailBV;
      // Read the variable values
      std::getline(m_SampleFile, line);
      std::stringstream ss(line);
      for(auto varName = 0; varName != static_cast<unsigned int>(IVNames::IVNamesEnd);
          ++ varName)
        {
        double bvValue;
        ss >> bvValue;
        prosailBV[static_cast<IVNames>(varName)] = bvValue;
        }
      bv_vec.push_back(prosailBV);
      }
    m_SampleFile.close();
    auto sampleCount = bv_vec.size();
    otbAppLogINFO("" << sampleCount << " samples read."<< std::endl);

    std::vector<SimulationType> simus{sampleCount};
    
    auto simulator = [&](std::vector<BVType>::const_iterator sample_first,
                          std::vector<BVType>::const_iterator sample_last,
                          std::vector<SimulationType>::iterator simu_first){
      ProSailType prosail;
      prosail.SetRSR(satRSR);
      prosail.SetParameters(prosailPars);
      while(sample_first != sample_last)
        {
        prosail.SetBVs(*sample_first);
        *simu_first = prosail();
        if(add_noise)
          {
          for(auto i=0; i<nbBands; i++)
            {
            (*simu_first)[i] += noise_generators[i](RNG);
            }
          }
        ++sample_first;
        ++simu_first;
        }
    };    

    auto num_threads = std::thread::hardware_concurrency();
    otbAppLogINFO("" << num_threads << " CPUs available."<< std::endl);

    auto block_size = sampleCount/num_threads;
    if(num_threads>=sampleCount) block_size = sampleCount;
    std::vector<std::thread> threads(num_threads);
    auto input_start = std::begin(bv_vec);
    auto output_start = std::begin(simus);

    for(auto t=0; t<num_threads; ++t)
      {
      auto input_end = input_start;
      std::advance(input_end, block_size);
      threads[t] = std::thread(simulator,
                               input_start,
                               input_end,
                               output_start);
      input_start = input_end;
      std::advance(output_start, block_size);
      }
    std::for_each(threads.begin(),threads.end(),
                  std::mem_fn(&std::thread::join));
    
    otbAppLogINFO("" << sampleCount << " samples processed."<< std::endl);

    for(const auto& s : simus)
      this->WriteSimulation(s);
    
    m_SimulationsFile.close();
    otbAppLogINFO("Results saved in " << outFileName << std::endl);
  }

  double m_Azimuth;
  double m_SolarZenith;
  double m_SolarZenith_Fapar;
  double m_SensorZenith;
  // the input file
  std::ifstream m_SampleFile;
  // the output file
  std::ofstream m_SimulationsFile;
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::ProSailSimulator)

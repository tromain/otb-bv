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

#include "otbLeafParameters.h"
#include "otbSailModel.h"
#include "otbProspectModel.h"
#include "otbSatelliteRSR.h"
#include "otbReduceSpectralResponse.h"

#include <fstream>
#include <string>

#include "otbBVTypes.h"
#include "otbBVUtil.h"

namespace otb
{

namespace Functor
{

template <class TSatRSR, unsigned int SimNbBands = 2000>
class ProSailSimulator
{
public:
  /** Standard class typedefs */
  typedef TSatRSR SatRSRType;
  typedef typename SatRSRType::Pointer SatRSRPointerType;
  typedef typename otb::ProspectModel ProspectType;
  typedef typename otb::LeafParameters LeafParametersType;
  typedef typename LeafParametersType::Pointer LeafParametersPointerType;
  typedef typename otb::SailModel SailType;

  typedef typename SatRSRType::PrecisionType PrecisionType;
  typedef std::pair<PrecisionType,PrecisionType> PairType;
  typedef typename std::vector<PairType> VectorPairType;
  typedef otb::SpectralResponse< PrecisionType, PrecisionType>  ResponseType;
  typedef typename ResponseType::Pointer  ResponsePointerType;
  typedef otb::ReduceSpectralResponse < ResponseType,SatRSRType>  ReduceResponseType;
  typedef typename ReduceResponseType::Pointer  ReduceResponseTypePointerType;
  typedef typename std::vector<PrecisionType> OutputType;
  
  /** Constructor */
  ProSailSimulator() {
    m_SatRSR = SatRSRType::New();
  }

  /** Destructor */
  ~ProSailSimulator() {};
  
  /** Implementation of the () operator*/
  inline OutputType operator ()()
  {
    OutputType pix;
    for(unsigned int i=0;i<m_SatRSR->GetNbBands();i++)
      pix.push_back(0.0);
    VectorPairType hxSpectrum;
    for(unsigned int i=0;i<SimNbBands;i++)
      {
      PairType resp;
      resp.first = static_cast<PrecisionType>((400.0+i)/1000);
      hxSpectrum.push_back(resp);
      }

    ProspectType::Pointer prospect = ProspectType::New();
    prospect->SetInput(m_LP);

    SailType::Pointer sail = SailType::New();
    sail->SetLAI(m_LAI);
    sail->SetAngl(m_Angl);
    sail->SetPSoil(m_PSoil);
    sail->SetSkyl(m_Skyl);
    sail->SetHSpot(m_HSpot);
    sail->SetTTS(m_TTS);
    sail->SetTTO(m_TTO);
    sail->SetPSI(m_PSI);
    sail->SetReflectance(prospect->GetReflectance());
    sail->SetTransmittance(prospect->GetTransmittance());
    sail->Update();
    for(unsigned int i=0;i<SimNbBands;i++)
      {
      hxSpectrum[i].second = static_cast<PrecisionType>(sail->GetHemisphericalReflectance()->GetResponse()[i].second);
      }
    ResponsePointerType aResponse = ResponseType::New();
    aResponse->SetResponse( hxSpectrum );
    ReduceResponseTypePointerType  reduceResponse = ReduceResponseType::New();
    reduceResponse->SetInputSatRSR(m_SatRSR);
    reduceResponse->SetInputSpectralResponse( aResponse );
    reduceResponse->CalculateResponse();
    VectorPairType reducedResponse =  reduceResponse->GetReduceResponse()->GetResponse();
    for(unsigned int i=0;i<m_SatRSR->GetNbBands();i++)
      pix[i] = reducedResponse[i].second;
    return pix;
  }

  bool operator !=(const ProSailSimulator& other) const
  {
    return true;
  }

  bool operator ==(const ProSailSimulator& other) const
  {
    return false;
  }

  inline void SetRSR(const  SatRSRPointerType rsr)
  {
    m_SatRSR = rsr;
  }

  inline SatRSRPointerType GetRSR() const
  {
    return m_SatRSR;
  }

  inline void SetBVs(BVType bvmap)
  {
    m_LP->SetCab(bvmap[Cab]);
    m_LP->SetCar(bvmap[Car]);
    m_LP->SetCBrown(bvmap[Cbp]);
    m_LP->SetCw(bvmap[Cdm]*bvmap[CwRel]/(1.-bvmap[CwRel]));
    m_LP->SetCm(bvmap[Cdm]);
    m_LP->SetN(bvmap[N]);
    m_LAI = bvmap[MLAI];
    m_Angl = bvmap[ALA];
    m_PSoil = bvmap[Bs];
    m_Skyl = 0;
    m_HSpot = bvmap[HsD];
  }

  inline void SetParameters(AcquisitionParsType apmap)
  {
    m_TTS = apmap[TTS]; //solar zenith angle
    m_TTO = apmap[TTO]; //observer zenith angle
    m_PSI = apmap[PSI]; //azimuth

  }
  
protected:

  /** Satellite Relative spectral response*/
  SatRSRPointerType m_SatRSR;
  LeafParametersPointerType m_LP;
  double m_LAI; //leaf area index
  double m_Angl; //average leaf angle
  double m_PSoil; //soil coefficient
  double m_Skyl; //diffuse/direct radiation
  double m_HSpot; //hot spot
  double m_TTS; //solar zenith angle
  double m_TTO; //observer zenith angle
  double m_PSI; //azimuth
};


}

namespace Wrapper
{

class ProSailSimulator : public Application
{
public:
/** Standard class typedefs. */
  typedef ProSailSimulator     Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  
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
    SetDescription("Simulate reflectances using Prospect+Sail.");

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
    SetParameterDescription( "out", "Filename where the simulations are saved." );
    MandatoryOn("out");

    AddParameter(ParameterType_Float, "solarzenith", "");
    SetParameterDescription( "solarzenith", "." );
    MandatoryOn("solarzenith");

    AddParameter(ParameterType_Float, "sensorzenith", "");
    SetParameterDescription( "sensorzenith", "." );
    MandatoryOn("sensorzenith");

    AddParameter(ParameterType_Float, "azimuth", "");
    SetParameterDescription( "azimuth", "." );
    MandatoryOn("azimuth");

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
    //TODO: implement the method
  }
  
  void DoExecute()
  {
    m_Azimuth = GetParameterFloat("azimuth");
    m_SolarZenith = GetParameterFloat("solarzenith");
    m_SensorZenith = GetParameterFloat("sensorzenith");
    std::string rsrFileName = GetParameterString("rsrfile");
    //The first 2 columns of the rsr file correspond to the wavelenght and the solar radiation
    short int nbBands = countColumns(rsrFileName)-2;
    SatRSRType::Pointer  satRSR = SatRSRType::New();
    satRSR->SetNbBands(nbBands);
    satRSR->SetSortBands(false);
    satRSR->Load(rsrFileName);

    
    std::stringstream ss;
    ss << "Bands for sensor" << std::endl;
    for(unsigned int i = 0; i< nbBands; ++i)
      ss << i << " " << (satRSR->GetRSR())[i]->GetInterval().first
         << " " << (satRSR->GetRSR())[i]->GetInterval().second
         << std::endl;

    otbAppLogINFO(""<<ss);

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
    prosailPars[TTO] = m_SensorZenith;
    prosailPars[PSI] = m_Azimuth;
    
    ProSailType prosail;
    prosail.SetRSR(satRSR);

    //TODO : the soil file is not used
    
    unsigned long int sampleCount = 0;
    otbAppLogINFO("Processing simulations ..." << std::endl)
    //read variable names (first line)
    std::string line;
    std::getline(m_SampleFile, line);
    while(m_SampleFile.good())
      {
      BVType prosailBV;
      // Read the variable values
      std::getline(m_SampleFile, line);
      std::stringstream ss(line);
      for(unsigned int varName = 0; varName != static_cast<unsigned int>(IVNamesEnd);
          ++ varName)
        {
        double bvValue;
        ss >> bvValue;
        prosailBV[static_cast<IVNames>(varName)] = bvValue;
        }
      prosail.SetBVs(prosailBV);
      prosail.SetParameters(prosailPars);
      this->WriteSimulation(prosail());
      ++sampleCount;
      }
    m_SampleFile.close();
    m_SimulationsFile.close();
    otbAppLogINFO("" << sampleCount << " samples processed. Results saved in "
                  << outFileName << std::endl);
  }

  double m_Azimuth;
  double m_SolarZenith;
  double m_SensorZenith;
  // the input file
  std::ifstream m_SampleFile;
  // the output file
  std::ofstream m_SimulationsFile;
};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::ProSailSimulator)

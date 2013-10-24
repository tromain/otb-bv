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

#include "otbBVTypes.h"
#include "otbBVUtil.h"

namespace otb
{

namespace Functor
{

template <class TSatRSR, unsigned int SimNbBands = 2000>
class ProsailSimulator
{
public:
  /** Standard class typedefs */
  typedef TSatRSR SatRSRType;
  typedef typename SatRSRType::Pointer SatRSRPointerType;
  typedef typename otb::ProspectModel ProspectType;
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
  ProsailSimulator() {
    m_SatRSR = SatRSRType::New();
  }

  /** Destructor */
  ~ProsailSimulator() {};
  
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

    //TODO: set the parameters using the maps
    ProspectType::Pointer prospect = ProspectType::New();
    //  prospect->SetInput(m_LabelParameters[ label ]);
    SailType::Pointer sail = SailType::New();
/*    sail->SetLAI(lai);
    sail->SetAngl(m_AcquisitionParameters[std::string("Angl")]);
    sail->SetPSoil(m_AcquisitionParameters[std::string("PSoil")]);
    sail->SetSkyl(m_AcquisitionParameters[std::string("Skyl")]);
    sail->SetHSpot(m_AcquisitionParameters[std::string("HSpot")]);
    sail->SetTTS(m_AcquisitionParameters[std::string("TTS")]);
    sail->SetTTO(m_AcquisitionParameters[std::string("TTO")]);
    sail->SetPSI(m_AcquisitionParameters[std::string("PSI")]);
    sail->SetReflectance(prospect->GetReflectance());
    sail->SetTransmittance(prospect->GetTransmittance());*/
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

  bool operator !=(const ProsailSimulator& other) const
  {
    return true;
  }

  bool operator ==(const ProsailSimulator& other) const
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
    //TODO: implement the method
  }

  inline void SetParameters(AcquisitionParsType apmap)
  {
    //TODO: implement the method
  }
  
protected:

  /** Satellite Relative spectral response*/
  SatRSRPointerType m_SatRSR;
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
  typedef Functor::ProsailSimulator<SatRSRType> ProSailType;
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
    short int nbBands = countColumns(rsrFileName);
    SatRSRType::Pointer  satRSR = SatRSRType::New();
    satRSR->SetNbBands(nbBands);
    satRSR->SetSortBands(false);
    satRSR->Load(rsrFileName);

    std::cout << "Bands for sensor" << std::endl;
    for(unsigned int i = 0; i< nbBands; ++i)
      std::cout << i << " " << (satRSR->GetRSR())[i]->GetInterval().first << " " << (satRSR->GetRSR())[i]->GetInterval().second << std::endl;

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
    prosailPars[HSPOT] = 0; //TODO find the appropriate value
    prosailPars[TTS] = m_SolarZenith;
    prosailPars[TTO] = m_SensorZenith;
    prosailPars[PSI] = m_Azimuth;
    
    ProSailType prosail;
    prosail.SetRSR(satRSR);

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
      }

    m_SampleFile.close();
    m_SimulationsFile.close();
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

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

namespace otb
{

namespace Functor
{

template <class TSatRSR, unsigned int SimNbBands = 2000>
class ProsailSimulatorFunctor
{
public:
  /** Standard class typedefs */
  typedef TSatRSR SatRSRType;
  typedef typename SatRSRType::Pointer SatRSRPointerType;
  typedef typename otb::ProspectModel ProspectType;
  typedef typename otb::SailModel SailType;

  typedef double PrecisionType;
  typedef std::pair<PrecisionType,PrecisionType> PairType;
  typedef typename std::vector<PairType> VectorPairType;
  typedef otb::SpectralResponse< PrecisionType, PrecisionType>  ResponseType;
  typedef ResponseType::Pointer  ResponsePointerType;
  typedef otb::ReduceSpectralResponse < ResponseType,SatRSRType>  ReduceResponseType;
  typedef typename ReduceResponseType::Pointer  ReduceResponseTypePointerType;
  typedef typename std::vector<PrecisionType> OutputType;
  
  /** Constructor */
  ProsailSimulatorFunctor() {
    m_SatRSR = SatRSRType::New();
  }

  /** Destructor */
  ~ProsailSimulatorFunctor() {};
  
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
    ResponseType::Pointer aResponse = ResponseType::New();
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

  bool operator !=(const ProsailSimulatorFunctor& other) const
  {
    return true;
  }

  bool operator ==(const ProsailSimulatorFunctor& other) const
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

  // Parameters for the acquisition geometry
  enum AcquisitionParameters {HSPOT, TTS, TTO, PSI};
  typedef  std::map< AcquisitionParameters, double > AcquisitionParsType;
  
private:
  void DoInit()
  {
    SetName("ProSailSimulator");
    SetDescription("Simulate reflectances using Prospect+Sail.");

    AddParameter(ParameterType_InputFilename, "bv-file", "Input file containing the bv samples.");
    SetParameterDescription( "in", "Input file containing the biophysical variable samples. It can be generated using the BVInputVariableGeneration application." );
    MandatoryOn("bv-file");

    AddParameter(ParameterType_InputFilename, "soil-file", "Input file containing the soil spectra.");
    SetParameterDescription( "in", "Input file containing ." );
    MandatoryOn("soil-file");

    AddParameter(ParameterType_InputFilename, "rsr-file", "Input file containing the relative spectral responses.");
    SetParameterDescription( "in", "Input file containing ." );
    MandatoryOn("rsr-file");
    
    AddParameter(ParameterType_OutputFilename, "out", "Output file");
    SetParameterDescription( "out", "Filename where the simulations are saved." );
    MandatoryOn("out");

    AddParameter(ParameterType_Float, "solar-zenith", "");
    SetParameterDescription( "solar-zenith", "." );
    MandatoryOn("solar-zenith");

    AddParameter(ParameterType_Float, "sensor-zenith", "");
    SetParameterDescription( "sensor-zenith", "." );
    MandatoryOn("sensor-zenith");

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


  void DoExecute()
  {
    m_Azimuth = GetParameterFloat("azimuth");
    m_SolarZenith = GetParameterFloat("solar-zenith");
    m_SensorZenith = GetParameterFloat("sensor-zenith");
    

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

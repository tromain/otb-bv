/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbLeafParameters.h"
#include "otbSailModel.h"
#include "otbProspectModel.h"
#include "otbSatelliteRSR.h"
#include "otbReduceSpectralResponse.h"

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
  typedef otb::ReduceSpectralResponse < ResponseType,SatRSRType>  ReduceResponseType;
  typedef typename std::vector<PrecisionType> OutputType;
  
  /** Constructor */
  ProSailSimulator() {
    m_SatRSR = SatRSRType::New();
  }

  /** Destructor */
  ~ProSailSimulator() {};
  
  /** Implementation of the () operator*/
  inline
  OutputType operator ()()
  {
    OutputType pix;
    for(auto i=0;i<m_SatRSR->GetNbBands();i++)
      pix.push_back(0.0);
    VectorPairType hxSpectrum;
    for(auto i=0;i<SimNbBands;i++)
      {
      PairType resp;
      resp.first = static_cast<PrecisionType>((400.0+i)/1000);
      hxSpectrum.push_back(resp);
      }

    auto m_LP = LeafParametersType::New();
    m_LP->SetCab(m_BV[Cab]);
    m_LP->SetCar(m_BV[Car]);
    m_LP->SetCBrown(m_BV[Cbp]);
    double Cw = m_BV[Cdm]*m_BV[CwRel]/(1.-m_BV[CwRel]);
    //TODO : this check should not be needed if the simulations were OK
    if(Cw<0) Cw = 0.0;
    m_LP->SetCw(Cw);
    m_LP->SetCm(m_BV[Cdm]);
    m_LP->SetN(m_BV[N]);
    m_LAI = m_BV[MLAI];
    m_Angl = m_BV[ALA];
    m_PSoil = m_BV[Bs];
    m_Skyl = 0;
    m_HSpot = m_BV[HsD];
    
    auto prospect = ProspectType::New();
    prospect->SetInput(m_LP);
    
    auto sail = SailType::New();
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
    for(auto i=0;i<SimNbBands;i++)
      {
      hxSpectrum[i].second = static_cast<PrecisionType>(sail->GetHemisphericalReflectance()->GetResponse()[i].second);
      }
    auto aResponse = ResponseType::New();
    aResponse->SetResponse( hxSpectrum );
    auto  reduceResponse = ReduceResponseType::New();
    reduceResponse->SetInputSatRSR(m_SatRSR);
    reduceResponse->SetInputSpectralResponse( aResponse );
    reduceResponse->CalculateResponse();
    auto reducedResponse =  reduceResponse->GetReduceResponse()->GetResponse();
    for(auto i=0;i<m_SatRSR->GetNbBands();i++)
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

  inline
  void SetRSR(const  SatRSRPointerType rsr)
  {
    m_SatRSR = rsr;
  }

  inline
  SatRSRPointerType GetRSR() const
  {
    return m_SatRSR;
  }

  inline
  void SetBVs(BVType bvmap)
  {
    m_BV = bvmap;
  }

  inline
  void SetParameters(AcquisitionParsType apmap)
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
  BVType m_BV;
};


}
}

/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkMacro.h"
#include "otbProSailSimulatorFunctor.h"

int bvProSailSimulatorFunctor(int argc, char * argv[])
{
  typedef double PrecisionType;
  typedef otb::SatelliteRSR<PrecisionType, PrecisionType>  SatRSRType;
  typedef otb::Functor::ProSailSimulator<SatRSRType> ProSailType;
  typedef typename ProSailType::OutputType SimulationType;

  auto satRSR = SatRSRType::New();
  satRSR->SetNbBands(5);
  satRSR->SetSortBands(false);
  satRSR->Load("/home/inglada/Dev/otb-bv/data/formosat2.rsr");

  typename otb::AcquisitionParsType prosailPars;
  prosailPars[otb::TTS] = 0.6476*(180.0/3.141592);
  prosailPars[otb::TTO] = 0.30456*(180.0/3.141592);
  prosailPars[otb::PSI] = -2.5952*(180.0/3.141592);

  ProSailType prosail;
  prosail.SetRSR(satRSR);
  typename otb::BVType prosailBV;

  prosailBV[otb::IVNames::MLAI] = 3.7277;
  prosailBV[otb::IVNames::ALA] = 59.755;
  prosailBV[otb::IVNames::CrownCover] = 0.95768;
  prosailBV[otb::IVNames::HsD] = 0.18564;
  prosailBV[otb::IVNames::N] = 1.4942;
  prosailBV[otb::IVNames::Cab] = 64.632;
  prosailBV[otb::IVNames::Car] = 0;
  prosailBV[otb::IVNames::Cdm] = 0.0079628;
  prosailBV[otb::IVNames::CwRel] = 0.73298;
  prosailBV[otb::IVNames::Cbp] = 0.075167;
  prosailBV[otb::IVNames::Bs] = 0.72866;

  prosail.SetBVs(prosailBV);
  prosail.SetParameters(prosailPars);
  auto pix = prosail();

  std::cout << "--------------------" << std::endl;
  for(auto p : pix)
    std::cout << p << " ";

  std::cout << std::endl;
  std::cout << "--------------------" << std::endl;

  return EXIT_SUCCESS;
}


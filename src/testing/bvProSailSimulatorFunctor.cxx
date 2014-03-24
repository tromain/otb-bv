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
  prosailPars[otb::TTS] = 30.0;
  prosailPars[otb::TTO] = 0.0;
  prosailPars[otb::PSI] = 0.0;

  ProSailType prosail;
  prosail.SetRSR(satRSR);
  typename otb::BVType prosailBV;

  prosailBV[otb::MLAI] = 2.0;
  prosailBV[otb::ALA] = 60.0;
  prosailBV[otb::CrownCover] = 0.8;
  prosailBV[otb::HsD] = 0.2;
  prosailBV[otb::N] = 1.5;
  prosailBV[otb::Cab] = 45.0;
  prosailBV[otb::Car] = 8.5;
  prosailBV[otb::Cdm] = 0.005;
  prosailBV[otb::CwRel] = 0.75;
  prosailBV[otb::Cbp] = 0.5;
  prosailBV[otb::Bs] = 1.2;

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


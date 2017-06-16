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
  if(argc<2)
    {
    std::cout << " At least one parameter is needed" << std::endl;
    return EXIT_FAILURE;
    }

  typedef double PrecisionType;
  typedef otb::SatelliteRSR<PrecisionType, PrecisionType>  SatRSRType;
  typedef otb::Functor::ProSailSimulator<SatRSRType> ProSailType;
  auto satRSR = SatRSRType::New();
  satRSR->SetNbBands(4);
  satRSR->SetSortBands(false);
  satRSR->Load(argv[1]);

  using namespace otb::BV;
  AcquisitionParsType prosailPars;
  prosailPars[AcquisitionParameters::TTS] = 0.6476*(180.0/3.141592);
  prosailPars[AcquisitionParameters::TTO] = 0.30456*(180.0/3.141592);
  prosailPars[AcquisitionParameters::PSI] = -2.5952*(180.0/3.141592);

  ProSailType prosail;
  prosail.SetRSR(satRSR);
  typename otb::BV::BVType prosailBV;

  prosailBV[IVNames::MLAI] = 3.7277;
  prosailBV[IVNames::ALA] = 59.755;
  prosailBV[IVNames::CrownCover] = 0.95768;
  prosailBV[IVNames::HsD] = 0.18564;
  prosailBV[IVNames::N] = 1.4942;
  prosailBV[IVNames::Cab] = 64.632;
  prosailBV[IVNames::Car] = 0;
  prosailBV[IVNames::Cdm] = 0.0079628;
  prosailBV[IVNames::CwRel] = 0.73298;
  prosailBV[IVNames::Cbp] = 0.075167;
  prosailBV[IVNames::Bs] = 0.72866;

  prosail.SetBVs(prosailBV);
  prosail.SetParameters(prosailPars);
  auto pix = prosail();


  std::cout << "--------------------" << std::endl;
  for(auto& p : pix)
    std::cout << p << " ";

  std::cout << std::endl;
  std::cout << "--------------------" << std::endl;

  auto tolerance = double{1e-5};
  decltype(pix) ref_pix{0.019252, 0.0257225, 0.0162109, 0.388866, 0.854399, 0.850187};
  auto err_sim = double{0};

  for(size_t i=0; i<ref_pix.size(); i++)
    err_sim += fabs(ref_pix[i]-pix[i]);

  if(err_sim>tolerance)
    {
    std::cout << "Regression error" << std::endl;
    for(auto& p : ref_pix)
      std::cout << p << " ";

    std::cout << std::endl;
    std::cout << "--------------------" << std::endl;

    return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}


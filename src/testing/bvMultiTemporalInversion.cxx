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

#include "phenoFunctions.h"
#include "otbBVUtil.h"
#include "otbProSailSimulatorFunctor.h"
#include "otbMachineLearningModelFactory.h"
#include "otbNeuralNetworkRegressionMachineLearningModel.h"

#include <vector>
#include <random>

using PrecisionType=double;
using VectorType=std::vector<PrecisionType>;
using SatRSRType = otb::SatelliteRSR<PrecisionType, PrecisionType>;
using ProSailType = otb::Functor::ProSailSimulator<SatRSRType>;
using SimulationType = typename ProSailType::OutputType;
using PixelType = ProSailType::OutputType;
typedef itk::VariableLengthVector<PrecisionType> InputSampleType;
typedef otb::NeuralNetworkRegressionMachineLearningModel<PrecisionType, 
                                                         PrecisionType> 
NeuralNetworkType;


std::pair<VectorType, VectorType> generate_lai(const VectorType doys)
{
  VectorType sigmo_pars{100, 3, 200, 3, 6, 0.1};
  auto simu_lai = pheno::sigmoid::F(doys, sigmo_pars);
  auto noisy_lai = simu_lai;

  auto rng = std::mt19937(std::random_device{}());
  std::normal_distribution<> d(0.0,0.1);
  for(auto& l : noisy_lai)
    {
    l+=d(rng);
    l=(l<0)?0:l;
    }

  return std::make_pair(simu_lai, noisy_lai);

}

std::vector<PixelType> generate_reflectances(VectorType lai_vec, 
                                             std::string rsr_file,
                                             double solarzenith,
                                             double sensorzenith,
                                             double azimuth)
{
  auto satRSR = SatRSRType::New();
  short int nbBands = otb::countColumns(rsr_file.c_str())-2;
  satRSR->SetNbBands(nbBands);
  satRSR->SetSortBands(false);
  satRSR->Load(rsr_file.c_str());

  typename otb::AcquisitionParsType prosailPars;
  prosailPars[otb::TTS] = solarzenith;
  prosailPars[otb::TTO] = sensorzenith;
  prosailPars[otb::PSI] = azimuth;

  ProSailType prosail;
  prosail.SetRSR(satRSR);
  prosail.SetParameters(prosailPars);

  typename otb::BVType prosailBV;
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



  std::vector<PixelType> simus;
  for(auto l : lai_vec)
    {
    prosailBV[otb::IVNames::MLAI] = l;
    prosail.SetBVs(prosailBV);
    auto pix = prosail();

    std::cout << pix.size() << " ---------" << std::endl;

    //add noise to simulations
    simus.push_back(pix);
    }

  return simus;
}

int bvMultiTemporalInversion(int argc, char * argv[])
{

  double solarzenith = std::atof(argv[2]);
  double sensorzenith = std::atof(argv[3]);
  double azimuth = std::atof(argv[4]);
  std::string rsr_file{argv[1]};
  VectorType doys;
  for(auto d=0; d<365; d+=10)
    doys.push_back(d);

  VectorType simu_lai, noisy_lai;

  std::tie(simu_lai, noisy_lai) = generate_lai(doys);
  auto simu_refls = generate_reflectances(noisy_lai, rsr_file, solarzenith,
                                          sensorzenith, azimuth);


  for(auto i=0; i<simu_lai.size(); ++i)
    std::cout << doys[i] << " " << simu_lai[i] 
              << "  " << noisy_lai[i] 
              << "  " << simu_refls[i][3] << std::endl;


  auto nn_regressor = NeuralNetworkType::New();
  nn_regressor->Load(argv[5]);


  std::cout << simu_refls[0].size() << " -----------" << std::endl;
  for(auto i=0; i<simu_lai.size(); ++i)
    {
    InputSampleType pix(simu_refls[i].data(), simu_refls[i].size());
    nn_regressor->Predict(pix);
    }
  return EXIT_SUCCESS;
}


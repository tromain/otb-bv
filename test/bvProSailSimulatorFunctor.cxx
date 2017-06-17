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
#include "otbBVUtil.h"

int bvProSailSimulatorFunctorPixel(int argc, char * argv[])
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
  decltype(pix) ref_pix{0.0181956, 0.0254008, 0.0152147, 0.392725, 0.854399, 0.850187};
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

std::vector<std::vector<double>> parse_simu_file(std::ifstream& simu_file, size_t nb_bands)
{
  std::vector<std::vector<double>> result;
  std::string line;
  std::getline(simu_file, line);
  while(simu_file.good())
    {
    std::vector<double> line_values;
    // Read the variable values
    std::getline(simu_file, line);
    if(!line.empty())
      {
      std::stringstream ss(line);
      const auto nb_columns = nb_bands+5; //3 angles, lai_bv,et, lai_true
      for(size_t i=0; i<nb_columns; ++i)
        {
        double value;
        ss >> value;
        line_values.push_back(value);
        }
      result.push_back(line_values);
      }
    }
  return result;
}
int bvProSailSimulatorFunctor(int argc, char * argv[])
{
  if(argc!=7)
    {
    std::cout << "Usage: " << argv[0] << 
      " RSRFile soilFile wlfactor bvVariableFile simulationFile nbBands" << std::endl;
    return EXIT_FAILURE;
    }

  auto nb_bands = std::stoi(argv[6]);
  typedef double PrecisionType;
  typedef otb::SatelliteRSR<PrecisionType, PrecisionType>  SatRSRType;
  typedef otb::Functor::ProSailSimulator<SatRSRType> ProSailType;
  auto satRSR = SatRSRType::New();
  satRSR->SetNbBands(nb_bands);
  satRSR->SetSortBands(false);
  satRSR->Load(argv[1]);

  auto soilDB = std::make_shared<otb::SoilDataBase>(argv[2], std::stod(argv[3]));

  ProSailType prosail;
  prosail.SetRSR(satRSR);
  prosail.UseExternalSoilDB(soilDB);

  std::ifstream bv_file(argv[4]);
  std::ifstream sim_file(argv[5]);
  using namespace otb::BV;

  auto bvvector = otb::BV::parse_bv_sample_file(bv_file);
  auto simuvector = parse_simu_file(sim_file, nb_bands);

  if(bvvector.size()!=simuvector.size())
    {
    std::cout << "Sample file and simulation file have to have the same number of lines\n";
    return EXIT_FAILURE;
    }

  size_t simu_counter = 0;
  for(const auto& bv_sample : bvvector)
    {
    const auto simu_data = simuvector[simu_counter++];

    AcquisitionParsType prosailPars;
    prosailPars[AcquisitionParameters::TTS] = std::acos(simu_data[nb_bands+1])*(180.0/3.141592);
    prosailPars[AcquisitionParameters::TTO] = std::acos(simu_data[nb_bands])*(180.0/3.141592);
    prosailPars[AcquisitionParameters::PSI] = std::acos(simu_data[nb_bands+2])*(180.0/3.141592);

    typename otb::BV::BVType prosailBV;

    prosail.SetBVs(bv_sample);
    prosail.SetParameters(prosailPars);
    auto pix = prosail();
    // remove fapar and fcover
    pix.resize(nb_bands);

    std::cout << "--------------------" << std::endl;
    for(auto& p : pix)
          std::cout << p << " ";

        std::cout << std::endl;
        std::cout << "--------------------" << std::endl;

        auto tolerance = double{1e-5};
        decltype(pix) ref_pix(simu_data.cbegin(),simu_data.cbegin()+nb_bands);
        auto err_sim = double{0};

        for(size_t i=0; i<ref_pix.size(); i++)
          err_sim += fabs(ref_pix[i]-pix[i]);

        if(err_sim>tolerance)
          {
          std::cout << "Regression error" << std::endl;
          for(auto& p : ref_pix)
            std::cout << p << " ";

          std::cout << std::endl;

          std::cout << bv_sample.at(IVNames::MLAI) << " " << 
            bv_sample.at(IVNames::ALA) << " " << bv_sample.at(IVNames::CrownCover) << 
            " " << bv_sample.at(IVNames::HsD) << " " << bv_sample.at(IVNames::N) << 
            " " << bv_sample.at(IVNames::Cab) << " " << bv_sample.at(IVNames::Car) << 
            " " << bv_sample.at(IVNames::Cdm) << " " << bv_sample.at(IVNames::CwRel) << 
            " " << bv_sample.at(IVNames::Cbp) << " " << bv_sample.at(IVNames::Bs) << '\n';

          std::cout << "--------------------" << std::endl;

          return EXIT_FAILURE;
          }
    }  
  return EXIT_SUCCESS;
}

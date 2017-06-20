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
#include "otbBVUtil.h"
#include "otbProSailSimulatorFunctor.h"
#include <cmath>

int bvComputeCovarAndMean(int argc, char * argv[])
{

  using namespace otb::BV;

  typedef double PrecisionType;
  typedef otb::SatelliteRSR<PrecisionType, PrecisionType>  SatRSRType;
  typedef otb::Functor::ProSailSimulator<SatRSRType> ProSailType;
  typedef typename ProSailType::OutputType SimulationType;

  std::vector<SimulationType> simus{{1.0, 2.0, 0, 0},{2.0, 1.0, 0, 0}};
  vnl_matrix<double> covariance;
  vnl_vector<double> mean_vector;

  EstimateReflectanceDensity(simus, covariance, mean_vector);

  if(covariance(0,0) != 0.25) return EXIT_FAILURE;
  if(mean_vector(0) != 1.5 ) return EXIT_FAILURE;

  std::cout << "-- Covariance -- \n";
  std::cout << covariance << '\n';

  std::cout << "-- Mean -- \n";
  std::cout << mean_vector << '\n';

  WriteReflectanceDensity(covariance, mean_vector, argv[1]);

  return EXIT_SUCCESS;
}

int bvReadCovarianceFile(int argc, char * argv[])
{
  using namespace otb::BV;
  vnl_matrix<double> covariance;
  vnl_vector<double> mean_vector;
  ReadReflectanceDensity(argv[1], covariance, mean_vector);

  return EXIT_SUCCESS;

}

double gaussian(double x, double m, double s)
{
  return exp(-(x-m)*(x-m)/(2*s*s))/std::sqrt(2*M_PI*s*s);
}

int bvIsSampleValid1D(int argc, char * argv[])
{
  using namespace otb::BV;
  vnl_matrix<double> covariance(1,1);
  vnl_vector<double> mean_vector(1);

  covariance(0,0) = 1.0;
  mean_vector(0) = 0.0;

  itk::VariableLengthVector<double> sample(1);

  for(auto x=0u; x<100; ++x)
    for(auto c=0u; c<100; ++c)
      {
      sample[0] = x/100.0;
      auto confidence = c/100.0;

      auto proba = gaussian(sample[0], mean_vector(0), covariance(0,0));
      auto valid = proba > 1-confidence;


      auto result = IsValidSample(sample, covariance, mean_vector, 
                                  covariance(0,0), confidence);

      auto error = fabs(result.second - proba);
      if(error > 10e-3)
        {
        std::cout << "Error in probability estimation\n";
        std::cout << result.first << " " << result.second<< " " << proba << '\n';
        return EXIT_FAILURE;
        }

      if( valid != result.first )
        {
        std::cout << "Error in validity estimation\n";
        return EXIT_FAILURE;
        }
      }

  return EXIT_SUCCESS;

}



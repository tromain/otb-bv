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

double gaussian2D(double x0, 
                  double x1, 
                  vnl_matrix<double>& inv_covariance, double cov_det)
{
  const auto norm_factor = 2*M_PI*std::sqrt(cov_det);
  const auto v1 = inv_covariance(0,0)*x0+inv_covariance(0,1)*x1;
  const auto v2 = inv_covariance(1,0)*x0+inv_covariance(1,1)*x1;
  const auto v3 = x0*v1+x1*v2;
  const auto v4 = -0.5*v3;
  return std::exp(v4)/norm_factor;
}

int bvIsSampleValid2D(int argc, char * argv[])
{
  using namespace otb::BV;
  vnl_matrix<double> inv_covar(2,2);
  vnl_matrix<double> covariance(2,2);
  vnl_vector<double> mean_vector(2);

  covariance(0,0) = 4.0;
  covariance(0,1) = 1.0;
  covariance(1,0) = 1.0;
  covariance(1,1) = 2.0;

  /* covar     inv
    4 1        2 -1
    1 2        -1 4*/
  inv_covar(0,0) = 2.0;
  inv_covar(0,1) = -1.0;
  inv_covar(1,0) = -1.0;
  inv_covar(1,1) = 4.0;
  auto determinant = 7;
  inv_covar/=determinant;
  mean_vector(0) = 0.0;
  mean_vector(1) = 1.0;

  vnl_matrix<double> inv_covariance(2,2);
  auto cov_det = InverseCovarianceAndDeterminant(covariance, inv_covariance);

  if(fabs(cov_det - determinant)>10e-5)
    {
    std::cout << "wrong determinant estimation\n";
    std::cout << cov_det << " instead of " << determinant << "\n";
    return EXIT_FAILURE;
    }
  if(fabs(inv_covar(0,0) - inv_covariance(0,0))>10e-5 ||
     fabs(inv_covar(1,0) - inv_covariance(1,0))>10e-5 ||
     fabs(inv_covar(0,1) - inv_covariance(0,1))>10e-5 ||
     fabs(inv_covar(1,1) - inv_covariance(1,1))>10e-5 )
    {
    std::cout << "wrong inverse convariance estimation\n";
    std::cout << inv_covariance << " instead of\n" << inv_covar << "\n";
    return EXIT_FAILURE;
    }

  itk::VariableLengthVector<double> sample(2);

  for(auto x=0u; x<100; ++x)
    for(auto c=0u; c<100; ++c)
      {
      sample[0] = x/100.0;
      sample[1] = c/100.0;
      auto confidence = c/100.0;

      auto proba = gaussian2D(sample[0]-mean_vector(0), 
                              sample[1]-mean_vector(1), 
                              inv_covariance, cov_det);
      auto valid = proba > 1-confidence;


      auto result = IsValidSample(sample, inv_covariance, mean_vector, 
                                  cov_det, confidence);

      std::cout << proba << "  " << result.second << '\n';

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



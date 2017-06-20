/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <random>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>

#include "otbBVTypes.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

namespace otb
{
namespace BV
{
template<typename RNGType>
double Rng(VarParams vpars, RNGType& rngen)
{
  //TODO : why us stdev defined for uniform?
  double min = vpars.min;
  double max = vpars.max;
  double mod = vpars.mod;
  double stdev = vpars.std;
  DistType dist = vpars.dist;

  double rn;
  if(dist == DistType::GAUSSIAN)
    {
    auto sampleInsideBounds = false;
    while(!sampleInsideBounds)
      {
      std::normal_distribution<> d(mod,stdev);
      rn = d(rngen);
      if( rn >= min && rn <= max)
        sampleInsideBounds = true;
      }
    }
  if(dist == DistType::LOGNORMAL)
    {
    auto sampleInsideBounds = false;
    while(!sampleInsideBounds)
      {
      std::lognormal_distribution<> d(mod, stdev);
      rn = d(rngen);
      if( rn >= min && rn <= max)
        sampleInsideBounds = true;
      }
    }
  else
    {
    std::uniform_real_distribution<> d(min, max);
    rn = d(rngen);
    }
  return rn;
}

template<typename II, typename OI>
inline
NormalizationVectorType estimate_var_minmax(II& ivIt, II& ivLast, OI& ovIt, OI& ovLast)
{

  std::size_t nbInputVariables{ivIt.GetMeasurementVector().Size()};
  NormalizationVectorType var_minmax{nbInputVariables+1, {std::numeric_limits<PrecisionType>::max(), std::numeric_limits<PrecisionType>::min()}};
  while(ovIt != ovLast &&
        ivIt != ivLast)
    {
    auto ov = ovIt.GetMeasurementVector()[0];
    if(ov<var_minmax[nbInputVariables].first)
      var_minmax[nbInputVariables].first = ov;
    if(ov>var_minmax[nbInputVariables].second)
      var_minmax[nbInputVariables].second = ov;
    for(size_t var = 0; var < nbInputVariables; ++var)
      {
      auto iv = ivIt.GetMeasurementVector()[var];
      if(iv<var_minmax[var].first)
        var_minmax[var].first = iv;
      if(iv>var_minmax[var].second)
        var_minmax[var].second = iv;
      }
    ++ovIt;
    ++ivIt;
    }
  return var_minmax;
}

template<typename NVT>
inline
void write_normalization_file(const NVT& var_minmax, const std::string out_filename)
{
  std::ofstream norm_file{out_filename};
  for(auto val : var_minmax)
    {
    norm_file << std::setprecision(8);
    norm_file << std::setw(20) << std::left << val.first;
    norm_file << std::setw(20) << std::left << val.second;
    norm_file << std::endl;
    }
}

template<typename T, typename U>
inline
T normalize(T x, U p)
{
  return 2*(
    (x-p.first)/
    (p.second-p.first+std::numeric_limits<T>::epsilon())
    -0.5);
}

template<typename T, typename U>
inline
T denormalize(T x, U p)
{
  return (x*0.5+0.5)*(p.second-p.first+std::numeric_limits<T>::epsilon())
    +p.first;
}

template<typename IS, typename OS, typename NVT>
inline
void normalize_variables(IS& isl, OS& osl, const NVT& var_minmax)
{
  auto ivIt = isl->Begin();
  auto ovIt = osl->Begin();
  auto ivLast = isl->End();
  auto ovLast = osl->End();

  std::size_t nbInputVariables{ivIt.GetMeasurementVector().Size()};
  while(ovIt != ovLast &&
        ivIt != ivLast)
    {
    auto ovInstId = ovIt.GetInstanceIdentifier();
    osl->SetMeasurement(ovInstId, 0, normalize(ovIt.GetMeasurementVector()[0],var_minmax[nbInputVariables]));
    auto ivInstId = ivIt.GetInstanceIdentifier();
    for(size_t var = 0; var < nbInputVariables; ++var)
      {
      isl->SetMeasurement(ivInstId, var, normalize(ivIt.GetMeasurementVector()[var],var_minmax[var]));
      }
    ++ovIt;
    ++ivIt;
    }
}

template<typename SimulationType>
void EstimateReflectanceDensity(const std::vector<SimulationType>& simus,
                                vnl_matrix<double>& covariance,
                                vnl_vector<double>& mean_vector)
{

  auto nbBands = simus[0].size()-2; //the last 2 values are fcover and fapar
  auto nbSamples = simus.size();
  covariance.set_size(nbBands, nbBands);
  covariance.fill(0);
  mean_vector.set_size(nbBands);
  mean_vector.fill(0);

  for(const auto& sample : simus)
    {
    for(size_t i=0; i<nbBands; ++i)
      mean_vector[i] += sample[i];
    }
  mean_vector /= nbSamples;

  for(const auto& sample : simus)
    {
    vnl_vector<double> v(sample.data(), nbBands);
    v -= mean_vector;
    vnl_matrix<double> x(v.data_block(), nbBands, 1);
    vnl_matrix<double> xt(v.data_block(), 1, nbBands);
    const auto tmpcov = x*xt;
    covariance += tmpcov;
    }
  covariance /= nbSamples;
}

template<typename SampleType>
std::pair<bool, double> IsValidSample(SampleType sample, vnl_matrix<double>& inv_cov,
                                      vnl_vector<double>& mean_vector, double cov_det, 
                   double confidence)
{
  // evaluate if the sample probability is > 1-confidence
  assert(sample.Size() == mean_vector.size());
  assert(inv_cov.rows() == inv_cov.columns());

  auto k = sample.Size();

  vnl_vector<double> sample_v(sample.GetDataPointer(), k);
  auto norm_factor = std::sqrt(std::pow(2*M_PI,k)*cov_det);
  auto centered_sample = sample_v-mean_vector;
  vnl_matrix<double> centered_sample_t(centered_sample.data_block(), 1, k);
  auto exponent = 0.5*(centered_sample_t*inv_cov*centered_sample)(0);
  auto probability = std::exp(-exponent)/norm_factor;
  

  return std::make_pair((probability > 1-confidence), probability);
}

}//namespace BV 
}


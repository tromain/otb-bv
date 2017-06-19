/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OTBBVUTIL_H
#define __OTBBVUTIL_H

#include "otbBVTypes.h"
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

namespace otb
{
size_t countColumns(std::string fileName);

namespace BV
{

//Generates a random number of the appropriate distribution and respecting the bounds
template<typename RNGType>
double Rng(otb::BV::VarParams vpars, RNGType& rngen);
double CorrelateValue(double v, double lai, VarParams vpars, VarParams laipars);
std::vector<BVType> parse_bv_sample_file(std::ifstream& sample_file);

template<typename II, typename OI>
inline
NormalizationVectorType estimate_var_minmax(II& ivIt, II& ivLast, OI& ovIt, OI& ovLast);

template<typename NVT>
inline
void write_normalization_file(const NVT& var_minmax, const std::string out_filename);

NormalizationVectorType read_normalization_file(const std::string in_filename);

template<typename T, typename U>
inline
T normalize(T x, U p);

template<typename T, typename U>
inline
T denormalize(T x, U p);

template<typename IS, typename OS, typename NVT>
inline
void normalize_variables(IS& isl, OS& osl, const NVT& var_minmax);

template<typename SimulationType>
void EstimateReflectanceDensity(const std::vector<SimulationType>& simus,
                                vnl_matrix<double>& covariance,
                                vnl_vector<double>& mean_vector);

void WriteReflectanceDensity(vnl_matrix<double>& covariance,
                             vnl_vector<double>& mean_vector, 
                             std::string file_name);

void ReadReflectanceDensity(std::string file_name, vnl_matrix<double>& covariance,
                            vnl_vector<double>& mean_vector);

double InverseCovarianceAndDeterminant(vnl_matrix<double>& cov, 
                                       vnl_matrix<double>& inv_conv);
template<typename SampleType>
bool IsValidSample(SampleType sample, vnl_matrix<double>& inv_cov,
                   vnl_vector<double>& mean_vector, double cov_det, 
                   double confidence);

}//namespace BV
}//namespace otb
// include the definition of the template functions
#include "otbBVUtil.txx"
#endif

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
size_t countSpaces(std::string& line);

size_t countColumns(const std::string& fileName);

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
                                       vnl_matrix<double>& inv_cov);


/** Evaluate how likely the sample is wrt the multivariate normal. We
can't use the multivariate normal pdf as an estimation of the
probability, since this function can locally have values much
larger than 1 (if the covariance has very low values and
therefore its determinant is very small. What we do here is to
compute the ratio pdf(sample)/pdf(mean), that is, the likelihood
of the sample relative to the mean vector (the max probability of
the distribution). This is equivalent to drop the normalization
factor of the pdf sqrt((2\pi)^{k}*det_covar). Therefore, only the
exponential of the multivariate gaussian needs to be evaluated.
If we take the option of a log-likelihood ratio, only
(x-mean)'cov^{-1}(x-mean) has to be evaluated. The confidence
value passed should be equal to -log(lr) where lr is the
likelihood ratio threshold value. It is a positive value (since
log(lr) is negative) value and the higher its value, the higher
the likelihood of a sample to be considered as belonging to the
distribution. For instance, if lr=0.01, confidence is 4.6. and
lr=0.1 gives confidence=2.3.*/
template<typename SampleType>
std::pair<bool, double> IsValidSample(SampleType sample, vnl_matrix<double>& inv_cov,
                                      vnl_vector<double>& mean_vector, 
                                      double confidence);

}//namespace BV
}//namespace otb
// include the definition of the template functions
#include "otbBVUtil.txx"
#endif

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

#include <iomanip>
#include <fstream>
#include <string>
#include <limits>
#include "otbBVTypes.h"

namespace otb
{
size_t countColumns(std::string fileName);

namespace BV
{

//Generates a random number of the appropriate distribution and respecting the bounds
template<typename RNGType>
double Rng(otb::BV::VarParams vpars, RNGType& rngen);
double CorrelateValue(double v, double lai, VarParams vpars, VarParams laipars);

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

NormalizationVectorType read_normalization_file(const std::string in_filename);

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

}//namespace BV
}//namespace otb
// include the definition of the template functions
#include "otbBVUtil.txx"
#endif

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

#include <string>

namespace otb
{
unsigned short int countColumns(std::string fileName);



template<typename II, typename OI>
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
        for(auto var = 0; var < nbInputVariables; ++var)
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
void write_normalization_file(const NVT& var_minmax, std::string out_filename)
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
  return (x-p.first)/(p.second-p.first+std::numeric_limits<T>::epsilon());
}

template<typename T, typename U>
inline
T denormalize(T x, U p)
{
  return x*(p.second-p.first)+p.first;
}

template<typename IS, typename OS, typename NVT>
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
    for(auto var = 0; var < nbInputVariables; ++var)
      {
      isl->SetMeasurement(ivInstId, var, normalize(ovIt.GetMeasurementVector()[var],var_minmax[var]));
      }
    ++ovIt;
    ++ivIt;
    }
}

}

#endif

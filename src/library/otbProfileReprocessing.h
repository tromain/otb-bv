/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OTBBVPROFREPR_H
#define __OTBBVPROFREPR_H

#include <vector>

using PrecisionType = double;
using VectorType = std::vector<PrecisionType>;
constexpr PrecisionType not_processed_value{0};
constexpr PrecisionType processed_value{1};


namespace otb
{
template <typename T>
inline 
T compute_weight(T delta, T err)
{
  T one{1};
  return (one/(one+delta)+one/(one+err));
}

std::pair<VectorType, VectorType> 
fit_csdm(VectorType dts, VectorType ts, VectorType ets)
{
  assert(ts.size()==ets.size() && ts.size()==dts.size());
  auto result = ts;
  auto result_flag = ts;
  // std::vector to vnl_vector
  pheno::VectorType profile_vec(ts.size());
  pheno::VectorType date_vec(dts.size());

  for(auto i=0; i<ts.size(); i++)
    {
    profile_vec[i] = ts[i];
    date_vec[i] = dts[i];
    }

  // fit
  auto approximation_result = 
    pheno::normalized_sigmoid::TwoCycleApproximation(profile_vec, date_vec);
  auto princ_cycle = std::get<1>(approximation_result);
  auto x_hat = std::get<0>(princ_cycle);
  auto min_max = std::get<1>(princ_cycle);
  auto A_hat = min_max.second - min_max.first;
  auto B_hat = min_max.first;
  auto p = pheno::normalized_sigmoid::F(date_vec, x_hat);
  //fill the result vectors
  for(auto i=0; i<ts.size(); i++)
    {
    result[i] = p[i]*A_hat+B_hat;
    result_flag[i] = processed_value;
    }

  return std::make_pair(result,result_flag);
}
std::pair<VectorType, VectorType> 
smooth_time_series_n_minus_1_with_error(VectorType dts,
                                        VectorType ts, 
                                        VectorType ets)
{
  assert(ts.size()==ets.size() && ts.size()==dts.size());
  auto result = ts;
  auto result_flag = ts;
  auto ot = result.begin();
  auto otf = result_flag.begin();
  auto eit = ets.begin();
  auto last = ts.end();
  auto prev = ts.begin();
  auto next = ts.begin();
  auto e_prev = ets.begin();
  auto e_next = ets.begin();
  auto dti = dts.begin();
  auto d_prev = dts.begin();
  auto d_next = dts.begin();
  *otf = not_processed_value;
  //advance iterators
  ++ot;
  ++otf;
  ++eit;
  ++dti;
  std::advance(next, 2);
  std::advance(e_next, 2);
  std::advance(d_next, 2);
  while(next!=last)
    {
    auto w_prev = compute_weight((*dti-*d_prev),fabs(*e_prev));
    auto w_curr = compute_weight(PrecisionType{0},fabs(*eit));
    auto w_next = compute_weight((*d_next-*dti),fabs(*e_next));
    *ot = ((*prev)*w_prev+(*ot)*w_curr+(*next)*w_next)/(w_prev+w_curr+w_next);
    *otf = processed_value;
    ++prev;
    ++next;
    ++e_prev;
    ++e_next;
    ++d_prev;
    ++d_next;
    ++ot;
    ++otf;
    ++eit;
    ++dti;
    }
  *otf = not_processed_value;
  return std::make_pair(result,result_flag);
}

VectorType smooth_time_series(VectorType ts, PrecisionType alpha, 
                              bool online=true)
{
  auto result = ts;
  auto it = ts.begin();
  auto ot = result.begin();
  auto last = ts.end();
  auto prev = *it;
  while(it!=last)
    {
    *ot = (*it)*(1-alpha)+alpha*prev;
    if(online)
      prev = *ot;
    else
      prev = *it;
    ++it;
    ++ot;
    }
  return result;
}

//assumes regular time sampling
VectorType smooth_time_series_n_minus_1(VectorType ts, PrecisionType alpha)
{
  auto result = ts;
  auto ot = result.begin();
  auto last = ts.end();
  auto prev = ts.begin();
  auto next = ts.begin();
  //advance iterators
  ++ot;
  ++next;
  while(next!=last)
    {
    auto lin_interp = ((*prev)+(*next))/2.0;
    *ot = (lin_interp)*(1-alpha)+alpha*(*ot);
    ++prev;
    ++next;
    ++ot;
    }
  return result;
}

}

#endif

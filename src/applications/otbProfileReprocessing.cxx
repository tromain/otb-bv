/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include <vector>

#include "dateUtils.h"
#include "phenoFunctions.h"

using PrecisionType = double;
using VectorType = std::vector<PrecisionType>;
constexpr PrecisionType not_processed_value{0};
constexpr PrecisionType processed_value{1};

namespace otb
{
int date_to_doy(std::string& date_str)
{
  return pheno::doy(pheno::make_date(date_str));
}

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

namespace Wrapper
{

class ProfileReprocessing : public Application
{
public:
  /** Standard class typedefs. */
  typedef ProfileReprocessing               Self;
  typedef Application                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(ProfileReprocessing, otb::Application);

private:
  void DoInit()
  {

    SetName("ProfileReprocessing");
    SetDescription("Reprocess a BV time profile.");
   
    AddParameter(ParameterType_InputFilename, "ipf", "Input profile file.");
    SetParameterDescription( "ipf", "Input file containing the profile to process. This is an ASCII file where each line contains the date (YYYMMDD) the BV estimation and the error." );
    MandatoryOn("ipf");

    AddParameter(ParameterType_OutputFilename, "opf", "Output profile file.");
    SetParameterDescription( "opf", "Filename where the reprocessed profile saved. This is an ASCII file where each line contains the date (YYYMMDD) the new BV estimation and a boolean information which is 0 if the value has not been reprocessed." );
    MandatoryOn("opf");

    AddParameter(ParameterType_String, "algo", 
                 "Reprocessing algorithm: n1, fit.");
    SetParameterDescription("algo", 
                            "Reprocessing algorithm: n1 is reprocessing the n-1 date, fit is a double logisting fitting of the complete profile.");
    MandatoryOff("algo");
  }

  void DoUpdateParameters()
  {
    //std::cout << "ProfileReprocessing::DoUpdateParameters" << std::endl;
  }

  void DoExecute()
  {
    auto ipfn = GetParameterString("ipf");
    std::ifstream in_profile_file;
    try
      {
      in_profile_file.open(ipfn.c_str());
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " 
                               << ipfn);
      }

    auto nb_dates = 0;
    std::vector<std::string> date_str_vec{};
    VectorType date_vec{};
    VectorType bv_vec{};
    VectorType err_vec{};
    for(std::string line; std::getline(in_profile_file, line); )
      {
      if(line.size() > 1)
        {
        std::istringstream ss(line);
        std::string date_str;
        PrecisionType in_bv;
        PrecisionType bv_err;

        ss >> date_str;
        ss >> in_bv;
        ss >> bv_err;

        date_str_vec.push_back(date_str);
        date_vec.push_back(date_to_doy(date_str));
        bv_vec.push_back(in_bv);
        err_vec.push_back(bv_err);

        nb_dates++;
        }
      }
    otbAppLogINFO("Input profile contains " << nb_dates << " dates.\n");

    in_profile_file.close();

    VectorType out_bv_vec{};
    VectorType out_flag_vec{};

    std::string algo{"n1"};
    if (IsParameterEnabled("algo"))
      algo = GetParameterString("algo");    
    if (algo == "n1")
      std::tie(out_bv_vec, out_flag_vec) = 
        smooth_time_series_n_minus_1_with_error(date_vec, bv_vec, err_vec);
    else if (algo == "fit")
      std::tie(out_bv_vec, out_flag_vec) = 
        fit_csdm(date_vec, bv_vec, err_vec);
    else
      itkGenericExceptionMacro(<< "Unknown algorithm " << algo 
                               << ". Available algorithms are: n1, fit.\n");

    auto opfn = GetParameterString("opf");
    std::ofstream out_profile_file;
    try
      {
      out_profile_file.open(opfn.c_str());
      }
    catch(...)
      {
      itkGenericExceptionMacro(<< "Could not open file " << opfn);
      }
    for(auto i=0; i<date_vec.size(); ++i)
      {
      std::stringstream ss;
      ss << date_str_vec[i] << "\t" << out_bv_vec[i] << "\t" 
         << out_flag_vec[i] << std::endl;
      out_profile_file << ss.str();
      }

    out_profile_file.close();


  }
};
}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::ProfileReprocessing)
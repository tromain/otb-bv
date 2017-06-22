/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

enum class Sorting {rmse, rrmse, median, max, r2};

Sorting to_sorting(const std::string s)
{
  if(s=="rmse") return Sorting::rmse;
  if(s=="rrmse") return Sorting::rrmse;
  if(s=="max") return Sorting::max;
  if(s=="median") return Sorting::median;
  if(s=="r2") return Sorting::r2;
  throw std::invalid_argument("Wrong sorting criterion");
}
struct error_metrics {
  std::string name{""};
  double rmse{0};
  double rrmse{0};
  double median_abs_err{0};
  double max_abs_err{0};
  double r2{0};
};

std::ostream& operator<<(std::ostream& os, const error_metrics em)
{

  os << "---- " << em.name << " ----\n" 
     << "rmse:" << em.rmse << "\nrrmse:" << em.rrmse 
     << "\nmed:" << em.median_abs_err << "\nmae:" << em.max_abs_err 
     << "\nR2:" << em.r2 << '\n';
  return os;
}

std::ostream& operator<<(std::ostream& os, 
                         const std::pair<error_metrics, error_metrics> em)
{

  os << "---- " << em.first.name << " | " << em.second.name << " ----" 
     << "\nrmse:" << em.first.rmse            << "\trmse:" << em.second.rmse          
     << "\nrrmse:" << em.first.rrmse          << "\trrmse:" << em.second.rrmse        
     << "\nmed:" << em.first.median_abs_err   << "\tmed:" << em.second.median_abs_err 
     << "\nmae:" << em.first.max_abs_err      << "\tmae:" << em.second.max_abs_err    
     << "\nR2:" << em.first.r2                << "\tR2:" << em.second.r2              
     << '\n';
  return os;
}
struct bv_validation_point
{
  double fieldv;
  double inrav;
  double otbv;
};

enum class MeasureType {Field, Inra, Otb};

using bv_validation_vec = std::vector<bv_validation_point>;

inline
std::vector<std::string> string_split(const std::string& s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
  elems.push_back(item);
  }
  return elems;
}

std::string get_file_basename(std::string name)
{
  auto name_with_extension = *(--string_split(name, '/').end());
  auto name_without_extension = string_split(name_with_extension,'.')[0];
  return name_without_extension;
}

inline
std::string get_file_contents(std::string filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
    {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
    }
  throw(errno);
}

bv_validation_vec validation_string_to_vector(const std::string& vs)
{
  bv_validation_vec result;
  auto lines = string_split(vs,'\n');
  for(const auto& v : lines)
    {
    auto li = string_split(v,' ');
    assert(li.size()==3);
    auto fieldv = std::atof(li[0].c_str());
    auto inrav = std::atof(li[1].c_str());
    auto otbv = std::atof(li[2].c_str());
    bv_validation_point bvvp{fieldv,inrav,otbv};
    result.push_back(bvvp);
    }
  return result;
}

inline
double get_measure(bv_validation_point vp, MeasureType measure)
{
  switch(measure)
    {
    case MeasureType::Field:
      return vp.fieldv;
      break;
    case MeasureType::Inra:
      return vp.inrav;
      break;
    case MeasureType::Otb:
      return vp.otbv;
      break;
    }
  return 0;
}

error_metrics compute_errors(const bv_validation_vec& bvv, 
                             MeasureType ref_measure, MeasureType est_measure,
                             std::string name)
{
  error_metrics result{};
  result.name = name;
  std::vector<double> abserrv{};
  abserrv.reserve(bvv.size());
  double y_average{0};
  double y_square{0};
  for(const auto& v : bvv)
    {
    auto expected = get_measure(v, ref_measure);
    auto estimated = get_measure(v, est_measure);
    auto abserr = std::fabs(estimated-expected);
    abserrv.push_back(abserr);
    result.rmse += abserr*abserr;
    result.rrmse += result.rmse/(std::max(expected,
                                          std::numeric_limits<double>::epsilon()));
    result.max_abs_err = (abserr>result.max_abs_err?abserr:result.max_abs_err);

    y_average += estimated;
    y_square += estimated*estimated;
    }

  auto ss_tot = y_square+ y_average*y_average - 2*std::sqrt(y_square)*y_average;
  //https://en.wikipedia.org/wiki/Coefficient_of_determination
  result.r2 = 1.0 - result.rmse/ss_tot;
  result.rmse /= bvv.size();
  result.rrmse /= bvv.size();
  std::sort(abserrv.begin(), abserrv.end());
  result.median_abs_err = abserrv[abserrv.size()/2];
  return result;
}

error_metrics retrieve_errors(const std::string filename, 
                              MeasureType mes_type=MeasureType::Otb)
{
  std::string suf_mes_type = (mes_type==MeasureType::Otb)?" OTB":" INRA";
  auto err_name = get_file_basename(filename)+suf_mes_type;
  auto file_contents = get_file_contents(filename);
  auto validation = validation_string_to_vector(file_contents);
  return compute_errors(validation, MeasureType::Field, mes_type, 
                        err_name);
}

auto by_rmse = [](error_metrics x, error_metrics y)
{
  return x.rmse < y.rmse;
};

auto by_rrmse = [](error_metrics x, error_metrics y)
{
  return x.rrmse < y.rrmse;
};

auto by_median_abs_err = [](error_metrics x, error_metrics y)
{
  return x.median_abs_err < y.median_abs_err;
};

auto by_max_abs_err = [](error_metrics x, error_metrics y)
{
  return x.max_abs_err < y.max_abs_err;
};

auto by_r2 = [](error_metrics x, error_metrics y)
{
  return x.r2 > y.r2;
};

int main(int argc, char* argv[])
{
  if(argc < 4)
    {
    std::cerr << "At least 2 files needed\n";
    std::cerr << argv[0] << " sorting [rmse|rrmse|median|max|r2] file1 file2 [file...]\n";
    return 1;
    }
  
  std::string sorting{argv[1]};
  if(sorting!="rmse" && sorting!="rrmse" && sorting!="median" && sorting!="max" && sorting!="r2")
    {
    std::cerr << "Wrong sorting criterion\n";
    std::cerr << argv[0] << " sorting [rmse|rrmse|median|max|r2] file1 file2 [file...]\n";
    return 1;
    }
  std::vector<error_metrics> em{};
  std::vector<error_metrics> eminra{};
  for(size_t i = 2; i<size_t(argc); ++i)
    {
    std::cout << argv[i] << '\n';
    em.push_back(retrieve_errors(argv[i]));
    eminra.push_back(retrieve_errors(argv[i],MeasureType::Inra));
    }

  switch(to_sorting(sorting))
    {
    case Sorting::rmse:
      std::sort(em.begin(), em.end(), by_rmse);
      break;
    case Sorting::rrmse:
      std::sort(em.begin(), em.end(), by_rrmse);
      break;
    case Sorting::max:
      std::sort(em.begin(), em.end(), by_max_abs_err);
      break;
    case Sorting::median:
      std::sort(em.begin(), em.end(), by_median_abs_err);
      break;
    case Sorting::r2:
      std::sort(em.begin(), em.end(), by_r2);
      break;
    }

  auto emIt = em.begin();
  auto eminraIt = eminra.begin();
  while(emIt != em.end() && eminraIt != eminra.end())
    {
    std::cout << std::make_pair(*emIt,*eminraIt);
    ++emIt;
    ++eminraIt;
    }
  return 0;
}

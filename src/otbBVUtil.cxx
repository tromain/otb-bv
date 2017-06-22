/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "itkMacro.h"
#include "otbBVUtil.h"
#include <vnl/algo/vnl_cholesky.h>

namespace otb
{
size_t countSpaces(std::string& line)
{
  size_t nbSpaces = 0;
  boost::trim(line);
  auto found = line.find(' ');
  while(found!=std::string::npos)
    {
    ++nbSpaces;
    found = line.find(' ', found+1);
    }
  return nbSpaces+1;
}

size_t countColumns(const std::string& fileName)
{
  std::ifstream ifile(fileName.c_str());
  std::string line;
  if (ifile.is_open())
    {
    getline(ifile,line);
    ifile.close();
    return countSpaces(line);
    }
  else
    {
    itkGenericExceptionMacro(<< "Could not open file " << fileName);
    }

}

namespace BV
{

otb::BV::NormalizationVectorType read_normalization_file(const std::string in_filename)
{
  using namespace otb::BV;
  NormalizationVectorType var_minmax;

  std::ifstream norm_file;
  try
    {
    norm_file.open(in_filename);
    }
  catch(...)
    {
    itkGenericExceptionMacro(<< "Could not open file " << in_filename);
    }

  for(std::string line; std::getline(norm_file, line); )
    {
    if(line.size() < 2)
      {
      itkGenericExceptionMacro(<< "Wrong line format in " << in_filename << ": " << line << std::endl);
      }
    std::istringstream ss(line);
    PrecisionType minval, maxval;
    ss >> minval;
    ss >> maxval;
    var_minmax.push_back(std::make_pair(minval, maxval));
    }
  norm_file.close();
  return var_minmax;
}


/**

        V* = (V-Vmin(0))*(Vmax(LAI)-Vmin(LAI))/(Vmax(0)-Vmin(0))+Vmin(LAI)
        p 32 of ATBD_BioVar_Venµs_V1.0.pdf

  */
double CorrelateValue(double v, double lai, VarParams vpars, VarParams laipars)
{
  double Vmin0 = vpars.min;
  double Vmax0 = vpars.max;
  double VminLAImax = vpars.Min_LAI_Max;
  double VmaxLAImax = vpars.Max_LAI_Max;
  double LAImax = laipars.max;
  double LAImin = laipars.min;
  bool codist = vpars.CoDistrib;
  if(codist)
    {
    double VminLAI = Vmin0+lai*(VminLAImax-Vmin0)/(LAImax-LAImin);
    double VmaxLAI = Vmax0+lai*(VmaxLAImax-Vmax0)/(LAImax-LAImin);
    double res = (v-Vmin0)/(Vmax0-Vmin0)*(VmaxLAI-VminLAI)+VminLAI;
    return res<0?0:res;
    }
  else
    return v;
}

std::vector<BVType> parse_bv_sample_file(std::ifstream& sample_file)
{    
  //read variable names (first line)
  std::string line;
  std::getline(sample_file, line);

  std::vector<BVType> bv_vec{};
  while(sample_file.good())
    {
    BVType prosailBV;
    // Read the variable values
    std::getline(sample_file, line);
    if(!line.empty())
      {
      std::stringstream ss(line);
      for(auto varName = 0; 
          varName != static_cast<unsigned int>(IVNames::IVNamesEnd);
          ++ varName)
        {
        double bvValue;
        ss >> bvValue;
        prosailBV[static_cast<IVNames>(varName)] = bvValue;
        }
      bv_vec.push_back(prosailBV);
      }
    }
  sample_file.close();
  return bv_vec;
}

void WriteReflectanceDensity(vnl_matrix<double>& covariance,
                             vnl_vector<double>& mean_vector, 
                             std::string file_name)
{
  std::ofstream covariancefile(file_name);
  covariancefile << "# Mean vector \n";
  for(size_t i=0; i<mean_vector.size(); ++i)
    {
    covariancefile << mean_vector[i] << " ";
    }
  covariancefile << "\n# Covariance matrix \n";
  for(size_t i=0; i<covariance.rows(); ++i)
    {
    for(size_t j=0; j<covariance.columns(); ++j)
      {
      covariancefile << covariance.get(i,j) << " ";
      }
    covariancefile << '\n';
    }
}

void ReadReflectanceDensity(std::string file_name, vnl_matrix<double>& covariance,
                            vnl_vector<double>& mean_vector)
{
  std::ifstream covariancefile(file_name);
  std::string line{};
  std::getline(covariancefile, line);
  if(line != "# Mean vector ")
    {
    throw std::runtime_error("Error in reflectance density file format : mean header");
    }
  //Read the mean vector
  std::getline(covariancefile, line);
  std::stringstream ss(line);
  const auto nb_bands = countSpaces(line);
  covariance.set_size(nb_bands, nb_bands);
  mean_vector.set_size(nb_bands);

  for(size_t b=0; b<nb_bands; ++b)
    {
    double val;
    ss >> val;
    mean_vector(b) = val;
    }

  std::cout << "Mean vector\n" << mean_vector << '\n';

  //Read the covariance matrix
  std::getline(covariancefile, line);
  if(line != "# Covariance matrix ")
    {
    throw std::runtime_error("Error in reflectance density file format : matrix header");
    }
  for(size_t l=0; l<nb_bands; ++l)
    {
    std::getline(covariancefile, line);
    std::stringstream ssm(line);
    for(size_t c=0; c<nb_bands; ++c)
      {
      double val;
      ssm >> val;
      covariance(l,c) = val;
      }
    }
  std::cout << "Covariance matrix\n" << covariance << '\n';
}

double InverseCovarianceAndDeterminant(const vnl_matrix<double>& cov, 
                                       vnl_matrix<double>& inv_cov,
                                       double ridge_epsilon)
{
  // We use Cholesky for the inverse, since only positive definite
  // matrices have a Cholesky decomposition. This is an additional
  // check for the conditioning of the covariance matrix. We also
  // apply a ridge regression to the matrix before inversion.
  vnl_matrix<double> ridge(cov.rows(), cov.columns(), 0);
  ridge.fill_diagonal(ridge_epsilon);
  vnl_cholesky inverse_calc(cov+ridge, vnl_cholesky::estimate_condition);
  inv_cov = inverse_calc.inverse();
  const auto determinant = inverse_calc.determinant();
  return determinant;
}

}//namespace BV 
}


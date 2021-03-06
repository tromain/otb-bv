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

namespace otb
{
size_t countColumns(std::string fileName)
{
  std::ifstream ifile(fileName.c_str());
  std::string line;
  size_t nbSpaces = 0;
  if (ifile.is_open())
    {
    getline(ifile,line);
    ifile.close();
    boost::trim(line);
    auto found = line.find(' ');
    while(found!=std::string::npos)
      {
      ++nbSpaces;
      found = line.find(' ', found+1);
      }
    return nbSpaces+1;
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

}//namespace BV 
}


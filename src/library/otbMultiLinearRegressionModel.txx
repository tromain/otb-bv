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
#include <iomanip>

namespace otb{
template <typename PrecisionType>
void  MultiLinearRegressionModel<PrecisionType>::Save(const std::string & filename){
  std::ofstream model_file;
  try
    {
    model_file.open(filename.c_str(), std::ofstream::out);
    }
  catch(...)
    {
    itkGenericExceptionMacro(<< "Could not open file " << filename);
    }
  model_file << "# Multilinear regression model\n";
  model_file << std::setprecision(10);
  for(auto& coef: m_model)
    model_file << coef << "\n";
  model_file.close();
}
template <typename PrecisionType>
void  MultiLinearRegressionModel<PrecisionType>::Load(const std::string & filename){
  std::ifstream model_file;
  try
    {
    model_file.open(filename.c_str());
    }
  catch(...)
    {
    itkGenericExceptionMacro(<< "Could not open file " << model_file);
    }
  m_model.clear();
  std::string line;
  std::getline(model_file, line); //skip header line
  while(std::getline(model_file, line))
    {
    if(line.size() > 1)
      {
      std::istringstream ss(line);
      PrecisionType value;
      ss >> value;
      m_model.push_back(value);
      }
    else
      itkGenericExceptionMacro(<< "Bad format in model file " << filename << "\n" << line << "\n");
    }
  model_file.close();
}
}//namespace otb

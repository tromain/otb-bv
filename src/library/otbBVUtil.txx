/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <random>

namespace otb
{
namespace BV
{
template<typename RNGType>
double Rng(VarParams vpars, RNGType rngen)
{
  //TODO : why us stdev defined for uniform?
  double min = vpars.min;
  double max = vpars.max;
  double mod = vpars.mod;
  double stdev = vpars.std;
  DistType dist = vpars.dist;

  double rn;
  if(dist == DistType::GAUSSIAN)
    {
    auto sampleInsideBounds = false;
    while(!sampleInsideBounds)
      {
      std::normal_distribution<> d(mod,stdev);
      rn = d(rngen);
      if( rn >= min && rn <= max)
        sampleInsideBounds = true;
      }
    }
  if(dist == DistType::LOGNORMAL)
    {
    auto sampleInsideBounds = false;
    while(!sampleInsideBounds)
      {
      std::lognormal_distribution<> d(mod, stdev);
      rn = d(rngen);
      if( rn >= min && rn <= max)
        sampleInsideBounds = true;
      }
    }
  else
    {
    std::uniform_real_distribution<> d(min, max);
    rn = d(rngen);
    }




  return rn;
}

}//namespace BV 
}


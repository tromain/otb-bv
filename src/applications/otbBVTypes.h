/*=========================================================================
  Program:   otb-bv
  Language:  C++

  Copyright (c) CESBIO. All rights reserved.

  See otb-bv-copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbBVTypes_h
#define __otbBVTypes_h
namespace otb
{
enum IVNames {MLAI, ALA, CrownCover, HsD, N, Cab, Cdm, CwRel, Cbp, Bs, IVNamesEnd};
enum AcquisitionParameters {HSPOT, TTS, TTO, PSI, AcquisitionParametersEnd};

typedef std::map< AcquisitionParameters, double > AcquisitionParsType;
typedef std::map< IVNames, double > BVType;
}
#endif

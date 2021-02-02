% =========================================================================
%   Program:   plot validation data
%   Language:  Octave/MATLAB
%
%   Copyright (c) CS Group France. All rights reserved.
%
%   See otb-bv-copyright.txt for details.
%
%   This software is distributed WITHOUT ANY WARRANTY; without even
%   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
%   PURPOSE.  See the above copyright notices for more information.
%
% =========================================================================

satnames = {"formosat2","s2","landsat8","spot4","spot5"};
cd "/tmp/MLAI/"
allDirs = dir("/tmp/MLAI");
hf = figure();
for i = 3:size(allDirs)
  satfound = 0;
  currDir = allDirs(i).name;
  currPath = strcat("/tmp/MLAI/",currDir);
  for currSat = satnames
    if(findstr(currDir,currSat{1}) == 1)
      satfound = 1;
    endif
  endfor
  if(satfound == 1)
    cd (currPath)
    load validation_gt_nn
    clf
    plot (validation_gt_nn)
    legend ("Ground Truth","BVNET","NN")
    title (currDir)
    print (hf, "results_validation_gt.png", "-dpng")
  endif
endfor
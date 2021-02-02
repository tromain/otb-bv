% =========================================================================
%   Program:   plot estimated vs ground truth LAI
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
    load validation_nn
    load validation_gt_nn
    clf
    xlabel "Field LAI"
    ylabel "Estimated LAI"
    plot (validation_nn(:,1),validation_nn(:,2),"linestyle","none","marker",'x');
    hold on;
    plot (validation_gt_nn(:,1),validation_gt_nn(:,3),"linestyle","none","marker",'+');
    hold on;
    plot (validation_gt_nn(:,1),validation_gt_nn(:,2),"linestyle","none","marker",'o');
    legend ("simus","BVNET-NN","BVNET-Classic")
    title (currDir)
    print (hf, "results_LAI_estVSfield.png", "-dpng")
  endif
endfor
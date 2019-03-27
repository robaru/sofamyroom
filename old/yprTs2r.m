function T = yprTs2r(ypr,xyz)
% YPRTS2R  Compute sensor-to-room coordinate transformation matrix
%    T = YPRTR2S(YPR) computes the 3D sensor-to-room coordinate
%    transformation matrix using the sensor's yaw-pitch-roll orientation
%    YPR = [YAW,PITCH,ROLL].

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Author: Steven Schimmel, stevenmschimmel@gmail.com
% Copyright 2009, University of Zurich
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file is part of ROOMSIM.
%
% ROOMSIM is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version. 
%
% ROOMSIM is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with ROOMSIM. If not, see <http://www.gnu.org/licenses/>.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

C = cos(pi*ypr/180); Cy = C(1); Cp = C(2); Cr = C(3);
S = sin(pi*ypr/180); Sy = S(1); Sp = S(2); Sr = S(3);

T = [ Cy*Cp                , -Cr*Sy-Cy*Sp*Sr      , -Cy*Cr*Sp+Sy*Sr      ;
      Cp*Sy                , Cy*Cr-Sy*Sp*Sr       , -Cr*Sy*Sp-Cy*Sr      ;
      Sp                   , Cp*Sr                , Cp*Cr                ];

if nargin>1,
    T = [T xyz(:); 0 0 0 1];
end;


function h = plotcoordsystem(xyz,ypr,u, c)
% internal function that plots a 3d coordinate system to indicate a 
% sensor position and orientation in a room

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

if nargin==0, xyz = 1; end
if nargin<=1
    u = xyz;
    xyz = [0 0 0]';
    ypr = [0 0 0]';
end

v = [0 u 0 0 0 0;
     0 0 0 u 0 0;
     0 0 0 0 0 u;
     1 1 1 1 1 1];
v = yprTs2r(ypr,xyz) * v;
hold on;
h = plot3(reshape(v(1,:),2,3),reshape(v(2,:),2,3),reshape(v(3,:),2,3), 'LineWidth', 3);
set(h(1),'color',[c, 0.9]);
set(h(2),'color',[0.6, 0.6, 0.6, 0.7]);
set(h(3),'color',[0.6, 0.6, 0.6, 0.7]);


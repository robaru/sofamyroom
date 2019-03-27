function plotroom(param)
% PLOTROOM  Plot 3D room geometry
%    PLOTROOM(PARAM) plots the room specified by the room setup structure
%    PARAM. The plot illustrates the room dimensions, and the source and
%    receiver positions and orientations.

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

% extract room dimensions
Rx = param.room.dimension(1);
Ry = param.room.dimension(2);
Rz = param.room.dimension(3);

% start new figure and axes
% figure(1), clf, subplot(4,1,1:3), hold on
newplot, cla, hold on

%--------------------------------------------------------------------
% draw room in 3d

% define room vertices (z0, ccw; z1, ccw)
V = [0 0 0; Rx 0 0; Rx Ry 0; 0 Ry 0; 0 0 Rz; Rx 0 Rz; Rx Ry Rz; 0 Ry Rz];
% define room faces (x0,x1,y0,y1,z0,z1)
F = [1 4 8 5; 2 3 7 6; 1 2 6 5; 4 3 7 8; 1 2 3 4; 5 6 7 8];
% define face colours
C = [1   0   0  ;  % x0 : red
     0   0   1  ;  % x1 : blue
     1   0   1  ;  % y0 : cyan
     0   1   0  ;  % y1 : green
     0.5 0.5 0.5;  % z0 : grey
     1   1   1  ]; % z1 : white
% define face transparency (0=clear, 1=opaque)
A = [0.25; 0.25; 0.25; 0.25; 0.75; 0];

% draw room
patch('Faces',F,'Vertices',V,...
      'AlphaDataMapping','none',...
      'FaceVertexAlphaData',A,...
      'FaceAlpha','flat',...
      'FaceColor','flat',...
      'FaceVertexCdata',C,...
      'EdgeColor','none');

% outline the floor (save handle for legend)
plot3([0  0],  [0  Ry], [0 0],'r',...
      [Rx Rx], [0  Ry], [0 0],'b',...
      [0  Rx], [0  0],  [0 0],'m',...
      [0  Rx], [Ry Ry], [0 0],'g'); 
              
% outline the vertical edges
plot3([Rx Rx], [0  0],  [0 Rz],'m',...
      [0  0],  [0  0],  [0 Rz],'m',...
      [0  0],  [Ry Ry], [0 Rz],'g',...
      [Rx Rx], [Ry Ry], [0 Rz],'g'); 

% outline the ceiling 
plot3([0  Rx], [0  0],  [Rz Rz],'m',...
      [0  0],  [0  Ry], [Rz Rz],'r',...
      [0  Rx], [Ry Ry], [Rz Rz],'g',...
      [Rx Rx], [0  Ry], [Rz Rz],'b');

%---------------------------------------------------
% draw source
S = param.source;
Nsources = length(S);

% draw source location and orientation
Hsource = zeros(1,Nsources);
for i = 1:Nsources,
    l = S(i).location;
    o = S(i).orientation;
    plotcoordsystem(l,o,1/4);
    Hsource(i) = plot3(l(1),l(2),l(3),'o','MarkerEdgeColor','k','MarkerFaceColor','g');
    l = yprTs2r(o,l) * [-1 -1 -1 16]'/16;
    text(l(1),l(2),l(3),['S_' num2str(i)]);
end;

% create legend label for source(s)
sourcelabel = 'Source'; if Nsources>1, sourcelabel = 'Sources'; end;

%---------------------------------------------------------
% draw receiver
P = param.receiver;
Nreceivers = length(P);

% draw receiver location and orientation
Hreceiver = zeros(1,Nreceivers);
for i = 1:Nreceivers,
    l = P(i).location;
    o = P(i).orientation;
    plotcoordsystem(l,o,1/4);
    Hreceiver(i) = plot3(l(1),l(2),l(3),'o','MarkerEdgeColor','k','MarkerFaceColor','m');
    l = yprTs2r(o,l) * [-1 -1 -1 16]'/16;
    text(l(1),l(2),l(3),['R_' num2str(i)]);
end;

%----------------------------------------------------------
% format axis
prevwarnstate = warning('off');
axis equal;
axis([0 Rx 0 Ry 0 Rz]);
xlabel('Depth (x, meters)'); 
ylabel('Width (y, meters)'); 
zlabel('Height (z, meters)');
title(sprintf('Room, %s and Receiver Geometry', sourcelabel));
view(3);
set(gca,'xtick',0:Rx,'ytick',0:Ry,'ztick',0:Rz);
grid on;
warning(prevwarnstate);

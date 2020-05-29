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
C = [0   0   1  ;  % x0 : red
     0   0   1  ;  % x1 : blue
     0   0   1  ;  % y0 : cyan
     0   0   1  ;  % y1 : green
     0.5 0.5 0.5;  % z0 : grey
     1   1   1  ]; % z1 : white
% define face transparency (0=clear, 1=opaque)
A = [0.1; 0.1; 0.1; 0.1; 0.8; 0];

% draw room
patch('Faces',F,'Vertices',V,...
      'AlphaDataMapping','none',...
      'FaceVertexAlphaData',A,...
      'FaceAlpha','flat',...
      'FaceColor','flat',...
      'FaceVertexCdata',C,...
      'EdgeColor','none');

% outline the floor (save handle for legend)
plot3([0  0],  [0  Ry], [0 0],'k',...
      [Rx Rx], [0  Ry], [0 0],'k',...
      [0  Rx], [0  0],  [0 0],'k',...
      [0  Rx], [Ry Ry], [0 0],'k'); 
              
% outline the vertical edges
plot3([Rx Rx], [0  0],  [0 Rz],'k',...
      [0  0],  [0  0],  [0 Rz],'k',...
      [0  0],  [Ry Ry], [0 Rz],'k',...
      [Rx Rx], [Ry Ry], [0 Rz],'k'); 

% outline the ceiling 
plot3([0  Rx], [0  0],  [Rz Rz],'k',...
      [0  0],  [0  Ry], [Rz Rz],'k',...
      [0  Rx], [Ry Ry], [Rz Rz],'k',...
      [Rx Rx], [0  Ry], [Rz Rz],'k');

%---------------------------------------------------
% draw source
S = param.source;
Nsources = length(S);

orientation_length = max(param.room.dimension)*0.07;

% draw source location and orientation
source_color = [0.6350, 0.0780, 0.1840];
Hsource = zeros(1,Nsources);
for i = 1:Nsources
    l = S(i).location;
    o = S(i).orientation;
    plotcoordsystem(l,o,orientation_length, source_color);
    Hsource(i) = plot3(l(1),l(2),l(3),'o','MarkerSize', 16, 'MarkerEdgeColor',source_color,'MarkerFaceColor',source_color);
    l = yprTs2r(o,l) * [-1 -1 -1 16]'/16;
    text(l(1)+0.2,l(2)+0.2,l(3)+0.2,['S_' num2str(i)]);
end

% create legend label for source(s)
sourcelabel = 'Source'; if Nsources>1, sourcelabel = 'Sources'; end

%---------------------------------------------------------
% draw receiver
P = param.receiver;
Nreceivers = length(P);

% draw receiver location and orientation
Hreceiver = zeros(1,Nreceivers);
receiver_color = [0, 0.4470, 0.7410];
for i = 1:Nreceivers
    l = P(i).location;
    o = P(i).orientation;
    plotcoordsystem(l,o,orientation_length, receiver_color);
    Hreceiver(i) = plot3(l(1),l(2),l(3),'o','MarkerSize', 16, 'MarkerEdgeColor', receiver_color,'MarkerFaceColor', receiver_color);
    l = yprTs2r(o,l) * [-1 -1 -1 16]'/16;
    text(l(1)+0.2,l(2)+0.2,l(3)+0.2,['R_' num2str(i)]);
end

%----------------------------------------------------------
% format axis
prevwarnstate = warning('off');
axis equal;
axis([0 Rx 0 Ry 0 Rz]);
xlabel('Depth (x, [m])'); 
ylabel('Width (y, [m])'); 
zlabel('Height (z, [m])');
title(sprintf('Room, %s and Receiver Geometry', sourcelabel));
view(3);
set(gca,'xtick',0:Rx,'ytick',0:Ry,'ztick',0:Rz);
grid on;
warning(prevwarnstate);

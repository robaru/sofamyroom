function h = plotabsorption(absfreq,abscoef)
% PLOTABSORPTION  Plot surface absorption
%    PLOTABSORPTION(PARAM) plots the surface absorption coefficients
%    defined in the room setup structure PARAM.

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

if nargin==1
    name = {'X0 (back wall)'
            'X1 (front wall)'
            'Y0 (right wall)'
            'Y1 (left wall)'
            'Z0 (floor)'
            'Z1 (ceiling)'};
    param = absfreq;
    for i = 1:6
        subplot(3,2,i);
        plotabsorption(param.room.surface.frequency,...
            param.room.surface.absorption(i,:));
        try
            d = param.room.surface.description{i};
            if length(d)>30, d=[d(1:30) '...']; end
            title({name{i},d});
        catch
            title(name{i});
        end
        if i<5
            xlabel([]); 
            set(gca,'xticklabel',[]); 
        end
        if rem(i,2)==0
            ylabel([]);
            set(gca,'yticklabel',[]); 
        end
    end
else
    newplot
    h = semilogx(absfreq,abscoef,'b-x','linewidth',2,'markersize',10);
    axis([absfreq(1)/1.4 absfreq(end)*1.4 0 1]);
    grid on
    xlabel('Frequency (Hz)');
    ylabel('Absorption'); 
end

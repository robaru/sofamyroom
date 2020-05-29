function paramout = editabsorption(param)
% EDITABSORPTION  Edit surface absorption coefficients
%    PARAM = EDITABSORPTION(PARAM) provides a user-interface to manually 
%    adjust the surface absorption coefficients in the room setup structure 
%    PARAM.

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
% if no output specified, editing is pointless
if nargout==0
    warning('no output specified'); 
end

name = {'X0 (back wall)' , ...
         'X1 (front wall)', ...
         'Y0 (right wall)', ...
         'Y1 (left wall)' , ...
         'Z0 (floor)', ...
         'Z1 (ceiling)'};
     
hfig = figure('menubar','none',...
              'closerequestfcn',@callback_cancel,...
              'name','Edit absorption (double click graph to select material)',...
              'numbertitle','off');
hdata = zeros(1,6);
for i = 1:6
    subplot(3,2,i);
    hdata(i) = plotabsorption(...
          param.room.surface.frequency,...
          param.room.surface.absorption(i,:));
    set(gca,'buttondownfcn',@callback_buttondown);
    try
        title({name{i},param.room.surface.description{i}});
    catch
        title({name{i},''});
    end
end

% add OK/Cancel button
uicontrol('style','pushbutton',...
          'string','OK',...
          'position',[20 20 80 20],...
          'callback',@callback_ok);
uicontrol('style','pushbutton',...
          'string','Cancel',...
          'position',[120 20 80 20],...
          'callback',@callback_cancel);

% wait for ok/cancel
waitfor(hfig,'userdata');

% extract new data on OK
switch get(hfig,'userdata')
    case 'ok'
        for i = 1:6
            a = get(hdata(i),'ydata');
            t = get(get(get(hdata(i),'parent'),'title'),'string');
            d = t{2};
            param.room.surface.absorption(i,:) = a;
            param.room.surface.description{i} = d;
        end
end

% close figure
delete(hfig);
drawnow;
if nargout>0, paramout = param; end

%------------------------------------------------------------
function callback_buttondown(gcbo,arg2)
% respond to double-click on axes
if ~strcmp(get(gcf,'selectiontype'),'open'), return; end
[a,d] = selectabsorption(gcbo);
if ~isempty(a)
    htitle = get(gcbo,'title');
    s = get(htitle,'string');
    s{2} = d;
    set(htitle,'string',s);
end

%------------------------------------------------------------
function callback_ok(gcbo,arg2)
set(gcf,'userdata','ok');

%------------------------------------------------------------
function callback_cancel(gcbo,arg2)
set(gcf,'userdata','cancel');

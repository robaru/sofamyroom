function [absorption,description] = selectabsorption(h)
% SELECTABSORPTION  Internal function to select absorption coefficients
%                   from a materials database

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

% determine figure, axes, plot handles from h
htype = get(h,'type');
switch htype,
    case 'figure'
        haxes = findobj(h,'type','axes');
        haxes = haxes(1);
        hplot = findobj(haxes,'type','line');
        hplot = hplot(1);
        
    case 'axes'
        haxes = h;
        hplot = findobj(haxes,'type','line');
        hplot = hplot(1);
        
    case 'line'
        hplot = h;
        haxes = get(h,'parent');
end;

% bring figure to foreground
hpfig = get(haxes,'parent');
figure(hpfig);

% read original plot data
orgdata = get(hplot,'ydata');

% load material database
material = [];
load('data/materials');

% convert materials to single cell array
m = cell(length(material),3);
[m{:,1}] = material.type;
[m{:,2}] = material.description;
[m{:,3}] = material.absorption;

% determine wall types
type = unique(m(:,1));
% determine wall type indices
typeidx = cell(length(type),1);
for i = 1:length(type),
    typeidx{i} = strmatch(type{i},m(:,1));
end;

% create new figure
FIGW = 400; FIGH = 300;
screen = get(0,'screensize');
figpos = (screen(3:4) - [FIGW FIGH])/2;

hfig = figure('menubar','none',...
         'units','pixels',...
         'position',[figpos FIGW FIGH],...
         'numbertitle','off',...
         'name','Select material...',...
         'resize','off',...
         'windowstyle','modal');
col = get(hfig,'fixedcolors');

% set up userdata for figure
ud.m = m;
ud.type = type;
ud.typeidx = typeidx;
ud.hplot = hplot;
ud.orgdata = orgdata;

% add uicontrols
uicontrol('style','text',...
          'units','pixels',...
          'position',[10 FIGH-30 FIGW-20 20],...
          'backgroundcolor',col(3,:),...
          'string','Type:',...
          'horizontalalign','left');
uicontrol('style','text',...
          'units','pixels',...
          'position',[10 FIGH-75 FIGW-20 20],...
          'backgroundcolor',col(3,:),...
          'string','Material:',...
          'horizontalalign','left');
ud.hpopup = uicontrol('style','popupmenu',...
          'units','pixels',...
          'position',[10 FIGH-45 FIGW-20 20],...
          'backgroundcolor',col(2,:),...
          'callback',@callback_popup,...
          'string',type);
ud.hlistbox = uicontrol('style','listbox',...
          'units','pixels',...
          'position',[10 40 FIGW-20 FIGH-110],...
          'backgroundcolor',col(2,:),...
          'callback',@callback_listbox,...
          'string',m(typeidx{1},2));
uicontrol('style','pushbutton',...
          'units','pixels',...
          'position',[10 10 80 20],...
          'backgroundcolor',col(3,:),...
          'callback',@callback_ok,...
          'string','OK');
uicontrol('style','pushbutton',...
          'units','pixels',...
          'position',[100 10 80 20],...
          'backgroundcolor',col(3,:),...
          'callback',@callback_cancel,...
          'string','Cancel');

% find best material fit to orgdata
dmin = 1000; didx = [];
for i = 1:size(m,1),
    d = sum((m{i,3}-orgdata).^2);
    if d<dmin, dmin=d; didx=i; end;
end;

% link userdata to figure
guidata(hfig,ud);

% found a reasonable fit?
if ~isempty(didx),
    tidx = strmatch(m{didx,1},type);
    midx = strmatch(m{didx,2},m(typeidx{tidx},2));
    set(ud.hpopup,'value',tidx);
    callback_popup(ud.hpopup,[]);
    set(ud.hlistbox,'value',midx);
    callback_listbox(ud.hlistbox,[]);
end;

% wait for user response
waitfor(hfig,'userdata');

% get answer
ud = get(hfig,'userdata');

% prepare output
if isempty(ud),
    absorption = [];
    description = [];
else
    idx = typeidx{ud(1)}(ud(2));
    absorption = m{idx,3};
    description = m{idx,2};
end;

% close figure
close(hfig);
return;

function callback_listbox(gcbo,arg2)
sel = get(gcbo,'value');
if isempty(sel), return; end;
sel = sel(1);
ud = guidata(gcbo);
t = get(ud.hpopup,'value');
mnr = ud.typeidx{t}(sel);
set(ud.hplot,'ydata',ud.m{mnr,3});

function callback_popup(gcbo,arg2)
sel = get(gcbo,'value');
ud = guidata(gcbo);
set(ud.hlistbox,'string',ud.m(ud.typeidx{sel},2),'value',1);
callback_listbox(ud.hlistbox,[]);

function callback_ok(gcbo,arg2)
ud = guidata(gcbo);
tsel = get(ud.hpopup,'value');
msel = get(ud.hlistbox,'value');
set(gcf,'userdata',[tsel msel]);

function callback_cancel(gcbo,arg2)
ud = guidata(gcbo);
set(ud.hplot,'ydata',ud.orgdata);
set(gcf,'userdata',[]);

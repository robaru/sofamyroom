function hh = plotbrir(h,fs,varargin)
% PLOTBRIR Plot binaural room impulse response
%    PLOTBRIR(H,Fs) plots the binaural room impulse response H that is
%    sampled at Fs Hz.

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

if size(h,1)==2, h=h'; end;
if chkflag('db',varargin), 
    v = 20*log10(eps+abs(h));
    yl = max(v(:)) + [-80 0];
    if yl(2)<0, yl(2)=0; end;
    yunit = ' (dB)';
else
    v = h;
    m = max(abs(v(:))); if m==0, m=1; end;
    yl = [-1 1] * 10^ceil(log10(m));
    yunit = [];
end;
t = (0:size(h,1)-1)/fs*1000;
if chkflag('maxat0',varargin),
    [mv,mvi] = max(v);
    [mm,mmi] = max(mv);
    t = t - (mvi(mmi)-1)/fs*1000;
end;
hlax = subplot(2,1,1);
co = get(hlax,'colororder');
idx = length(findall(gca,'type','line'));
c = co(mod(idx*2,size(co,1))+1,:);
plot(t,v(:,1),'color',c);
xlabel([]); ylabel(['Left' yunit]);
axis tight, ylim(yl), grid on
hrax = subplot(2,1,2);
co = get(hlax,'colororder');
idx = length(findall(gca,'type','line'));
c = co(mod(idx*2+1,size(co,1))+1,:);
plot(t,v(:,2),'color',c);
axis tight, ylim(yl), grid on

% xtick = get(hrax,'xtick');
% set([hlax hrax],'xtick',xtick);
set(hlax,'xticklabel',[]);
% set(hrax,'xticklabel',xtick*1000);
xlabel('Time (ms)'); ylabel(['Right' yunit]);

if nargout>0, hh = [hlax hrax]; end;

function result = chkflag(flag,args)
result = ~isempty(strmatch(flag,args));

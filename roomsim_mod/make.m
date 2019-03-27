function make(varargin)
% MAKE Build roomsim project
%   MAKE without arguments builds the roomsim project and updates the
%   ROOMSIM function in MATLAB.
%
%   MAKE TEST builds the unit tests of the roomsim project and updates the
%   ROOMSIMTEST function in MATLAB.

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

% version = '1.00a';
% buildnr = '2731';
% date = '20090423';

% Set default switches
switches = { '-DMEX'
             '-DLOGLEVEL=0'
             '-DMEXP=19937'
             '-Isource\libroomsim\include'
             '-Isource\libfftw\win32\include'
             '-Isource\libsfmt'
             '-Isource'
           };
options = { '-Lsource\libfftw\win32\lib\'
            '-lfftw3-3'
          };
output = 'roomsim';       

mexfiles = { 'source\libroomsim\source\3D.c'
             'source\libroomsim\source\dsp.c'
             'source\libroomsim\source\interface.c'
             'source\libroomsim\source\interp.c'
             'source\libroomsim\source\roomsim.c'
             'source\libroomsim\source\rng.c'
             'source\libroomsim\source\sensor.c'
             'source\libsfmt\SFMT.c'
             'source\mexmain.c'
             'source\build.c'
           };
       
% distribfiles = { 'roomsim.mexw32'
%                  'roomsim.exe'
%                  'libfftw3-3.dll'
%                  'roomsim.m'
%                  'sampleroomsetup.m'
%                  'example.m'
%                  'editabsorption.m'
%                  'estimateroombreakfreq.m'
%                  'estimateRT60.m'
%                  'loadsurface.m'
%                  'mit2hrtf.m'
%                  'near.m'
%                  'placesensor.m'
%                  'plotabsorption.m'
%                  'plotbinsignal.m'
%                  'plotbrir.m'
%                  'plotbrtf.m'
%                  'plotcoordsystem.m'
%                  'plotroom.m'
%                  'plotRT60.m'
%                  'plotsignal.m'
%                  'readbrir.m'
%                  'readsetup.m'
%                  'roomsim_sd.m'
%                  'rsound.m'
%                  'schroederplot.m'
%                  'selectabsorption.m'
%                  'setcomplexity.m'
%                  'spec.m'
%                  'yprTs2r.m'
%                  'yprTr2s.m'
%                  'data\testsignal.mat'
%                  'data\materials.mat'
%                  'data\MIT\KEMARsmall.hrtf'
%                };
           
% % handle single 'distrib' argument
% if nargin==1 && ischar(varargin{1}) && strcmpi(varargin{1},'distrib')==1,
%     addpath('source\Release');
%     zip(['roomsim_v' version '_build' buildnr '_' date '.zip'],distribfiles);
%     return;
% end;

% Process optional arguments
for i = 1:nargin,
    switch varargin{i}
        case 'test'
            switches{end+1} = '-DUNIT_TEST';
            output = 'roomsimtest';
        case 'debug'
            switches{end+1} = '-g';
        otherwise
            switches{end+1} = varargin{i};
    end;
end;

% remove roomsim from memory, if loaded
clear roomsim;

% Echo command to be executed
fprintf('mex '); 
fprintf('%s ', switches{:});
fprintf('-output %s ', output);
fprintf('%s ', mexfiles{:});
fprintf('%s ', options{:});
fprintf('\n');

% Run MEX to build the desired target
err = 0;
try mex(switches{:},'-output',output,mexfiles{:},options{:}); catch err = 1; end;

% Continue making the build 
% if ~err && makebuild,
% end;

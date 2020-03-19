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

if ismac
    fprintf("Platform not supported\n")
    return
end

% Set default switches
 switches = {'-DMEX'
             '-DLOGLEVEL=0'
             '-DMEXP=19937'
             ['-Isource' filesep 'libroomsim' filesep 'include']
             ['-Isource' filesep 'libsfmt']
             ['-Isource' filesep 'wavwriter' filesep 'include']
           };
       
if ispc
    switches = [switches
                ['-Isource' filesep 'libfftw' filesep 'Windows' filesep 'x64' filesep 'include']
                ['-Isource' filesep 'libmysofa' filesep 'Windows' filesep 'x64' filesep 'include']
                ];
end

options = {'-lmysofa'
          };

if ispc
    options = [options
               ['-Lsource' filesep 'libfftw' filesep 'Windows' filesep 'x64' filesep 'lib']
               ['-Lsource' filesep 'libmysofa' filesep 'Windows' filesep 'x64' filesep 'lib' filesep 'Release']
               '-lfftw3-3.lib'
            ];

elseif isunix
    options = [options
               '-lfftw3'
               ];
end

output = 'roomsim';       

mexfiles = { ['source' filesep 'libroomsim' filesep 'source' filesep '3D.c']
             ['source' filesep  'libroomsim'  filesep  'source'  filesep  'dsp.c']
             ['source' filesep  'libroomsim'  filesep  'source'  filesep  'interface.c']
             ['source' filesep  'libroomsim'  filesep  'source'  filesep  'interp.c']
             ['source' filesep  'libroomsim'  filesep  'source'  filesep  'roomsim.c']
             ['source' filesep  'libroomsim'  filesep  'source'  filesep  'rng.c']
             ['source' filesep  'libroomsim'  filesep  'source'  filesep  'sensor.c']
             ['source' filesep  'libsfmt'  filesep  'SFMT.c']
             ['source' filesep  'mexmain.c']
             ['source' filesep  'build.c']
           };

% Process optional arguments
for i = 1:nargin
    switch varargin{i}
        case 'test'
            switches{end1} = '-DUNIT_TEST';
            output = 'roomsimtest';
        case 'debug'
            switches{end1} = '-g';
        otherwise
            switches{end1} = varargin{i};
    end
end

% Remove roomsim from memory, if loaded
clear roomsim;

% Echo command to be executed
fprintf('mex '); 
fprintf('%s ', switches{:});
fprintf('-output %s ', output);
fprintf('%s ', mexfiles{:});
fprintf('%s ', options{:});
fprintf('\n');

% Run MEX to build the desired target
mex(switches{:}, options{:}, mexfiles{:}, '-output', output)

%Copy libfftw3-3.dll into working directory
if ispc
    [status, msg] = copyfile(...
        ['source' filesep 'libfftw' filesep 'Windows' filesep 'x64' filesep 'bin' filesep 'libfftw3-3.dll']...
    );
    if ~status
        fprintf("An error occured while copying libfftw3-3.dll file: %s\n", msg)
    else
        fprintf("libfftw3-3.dll successfully copied.\n")
    end
end

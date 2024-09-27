% SAMPLEROOMSETUP.M - Sample setup parameter structure for roomsim

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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    room settings
%
room.dimension              = [ 7.1 5.1 3 ];   % room dimension (x,y,z)
room.humidity               = 0.42;         % relative humidity (0,...,1)
room.temperature            = 20;           % room temperature (celsius)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    surface absorption/diffusion coefficients
%
room.surface.frequency      = [ 125 250 500 1000 2000 4000 8000];
room.surface.absorption     = [];
room.surface.diffusion      = [];


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    simulation options
%
options.fs                  = [];                % sampling frequency in Hz
options.responseduration    = 1.25;                 % duration of impulse response
options.bandsperoctave      = 1;                    % simulation frequency accuracy (1, 2, 3, or 4 bands/octave)
options.referencefrequency  = 125;                  % reference frequency for frequency octaves
options.airabsorption       = true;                 % apply air absorption?
options.distanceattenuation = true;                 % apply distance attenuation?
options.subsampleaccuracy   = false;                % apply subsample accuracy?
options.highpasscutoff      = 0;                    % 3dB frequency of high-pass filter (0=none)
options.verbose             = true;                 % print status messages?

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    specular reflections simulation options
%
options.simulatespecular    = true;                 % simulate specular reflections?
options.reflectionorder     = [ 10 10 10 ];         % maximum specular reflection order (x,y,z)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    diffuse reflections simulation options
%
options.simulatediffuse     = true;                 % simulate diffuse reflections?
options.numberofrays        = 2000;                 % number of rays in simulation (20*K^2)
options.diffusetimestep     = 0.010;                % time resolution in diffuse energy histogram (seconds)
options.rayenergyfloordB    = -80;                  % ray energy threshold (dB, with respect to initial energy)
options.uncorrelatednoise   = true;                 % use uncorrelated poisson arrivals for binaural impulse responses?

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    output options
%
options.outputname			= 'output';           	% name of the output file
options.saveaswav	        = false;                 % format of the ouput file

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    source definitions
%
source(1).location           = [ 3.55 2.55 1.5 ];       % location of source (x,y,z; meters)
source(1).orientation        = [ 180 0 0 ];         % orientation of source (yaw,pitch,roll; degrees)
source(1).description        = 'subcardioid';       % source type

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    receiver definitions
%
receiver(1).location         = [];         % location of receiver (x,y,z; meters)
receiver(1).orientation      = [];           % orientation of receiver (yaw,pitch,roll; degrees)
receiver(1).description      = [];  % receiver type

function paramout = setcomplexity(param)
% SETCOMPLEXITY Set complexity of room simulation
%    PARAM = SETCOMPLEXITY(PARAM) determines the complexity of the room
%    simulation specified by the room setup structure PARAM, and sets the
%    REFLECTIONORDER and RESPONSEDURATION simulation options in PARAM
%    accordingly.

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

% calculate speed of sound (m/s) 
c = round(331*sqrt(1+0.0036*param.room.temperature)); 

% calculate reverberation time RT60
RT60 = estimateRT60(param);

% set response duration (seconds)
param.options.responseduration = max(RT60) + norm(param.room.dimension)/c;

% compute maximum source-receiver distance
MaxDistance = c * param.options.responseduration;

% determine number of X/Y/Z reflections within MaxDistance
param.options.reflectionorder = ceil(MaxDistance ./ param.room.dimension);

fprintf('  Response duration: %.3f\n',param.options.responseduration);
fprintf('  Reflection order : %d %d %d\n',param.options.reflectionorder);

if nargout>0, paramout = param; end;

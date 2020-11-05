function F = estimateroombreakfreq(param,RT60)
% ESTIMATEROOMBREAKFREQ Estimate room break frequencies
%    F = ESTIMATEROOMBREAKFREQ(PARAM) estimates the room break frequencies
%    F for the room specified by the room setup structure PARAM. When
%    called without output variable, the function prints the room break 
%    frequencies and a legend explaining them to the command window.

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

if nargin==1 || isempty(RT60), RT60 = estimateRT60(param); end;
SpeedOfSound = round(331*sqrt(1+0.0036*param.room.temperature)); 
F(1) = SpeedOfSound / max(param.room.dimension);
F(2) = 11250 * sqrt(0.0283*mean(RT60)/prod(param.room.dimension));
F(3) = 4*F(2);

if nargout==0,
    fprintf('Estimate of room break frequencies:\n');
    fprintf('   F(1) = %6.0f Hz\n',F(1));
    fprintf('   F(2) = %6.0f Hz\n',F(2));
    fprintf('   F(3) = %6.0f Hz\n',F(3));
    fprintf('\n');
    fprintf('Legend:\n');
    fprintf('   F(1): Lowest room mode, i.e. below F(1) no resonant support for sound in room.\n');
    fprintf('       --- Between F(1) and F(2) room modes dominate ---\n');
    fprintf('   F(2): Approximate cutoff (crossover) frequency.\n');
    fprintf('       --- Between F(2) and F(3) diffraction and diffusion dominate ---\n');
    fprintf('   F(3): Above F(3) specular reflections and ray acoustics are valid.\n');
    clear F;
end;

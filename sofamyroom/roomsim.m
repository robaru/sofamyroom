% ROOMSIM  Simulate shoebox room acoustics
%
%    ROOMSIM(PAR) simulates the acoustics of the 'shoebox' room specified
%    by the settings in the parameter structure PAR.
%
%    See also READSETUP, SAMPLEROOMSETUP.
%
%    A note on how roomsim handles sources and receivers
%    ===================================================
%    It can take some time to load a source or receiver from disk into
%    memory, especially when a data-heavy binaural HRTF receiver is loaded
%    from a network drive. ROOMSIM therefore allows the user to pre-load
%    the sources and receivers that are used in a simulation. It also has a
%    function to list the loaded sources and receivers, and to remove
%    source and receivers from memory.
%
%       ROOMSIM LIST shows a list of available audio sources and receivers.
%
%       ROOMSIM LOAD 'SOURCE/RECEIVER' loads the specified source/receiver
%       into memory. 
%
%       ROOMSIM WHOS shows a list of loaded sources/receivers.
%
%       ROOMSIM CLEAR ['SOURCE/RECEIVER'] removes the specified source or 
%       receiver from memory, or all sources and receivers if none specified.
%
%    Sources and receivers are also loaded on demand by the ROOMSIM(PAR)
%    command, and are removed from memory when the MEX-file is cleared.
%

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


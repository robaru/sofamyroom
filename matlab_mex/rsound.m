function y = rsound(y,fs,h)
% RSOUND  Play sound after filtering with room impulse response
%    RSOUND(X,Fs,H) filters the signal X with the room impulse response H,
%    and plays the filtered signal at a sampling frequency of Fs Hz. The
%    signal X must be monaural, and the impulse response H must be monaural 
%    or binaural.
%
%    Y = RSOUND(X,Fs,H) returns the filtered signal Y without playing it.

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

if size(h,1)==1 || size(h,1)==2, h = h'; end

if size(h,2)~=1 && size(h,2)~=2
    error('room impulse response not monaural or binaural');
end

y = y(:); y(end+size(h,1)) = 0;
yl = fftfilt(h(:,1),y);

if size(h,2)==1
    y = yl;
else
    y = [yl fftfilt(h(:,2),y)];
end

if nargout==0
    wavplay(y,fs);
    clear y;
end

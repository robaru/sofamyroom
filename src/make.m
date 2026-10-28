function make(varargin)
% MAKE Build sofamyroom project
%   MAKE without arguments builds the sofamyroom project and updates the
%   sofamyroom function in MATLAB.
%
%   MAKE TEST builds the unit tests of the sofamyroom project and updates the
%   sofamyroomTEST function in MATLAB.

%Check MATLAB version
[~, maxArraySize] = computer; 
is64bitComputer = maxArraySize > 2^31;
if ~is64bitComputer
    fprintf("Building of Sofamyroom is not supported on 32-bit MATLAB\n");
    return
end

% Version
version = '1.0';

% MEX-file name
output = 'sofamyroom';

src_path = ['..' filesep 'src'];

% Set default switches
 switches = {'-DMEX'
             '-DLOGLEVEL=0'
             '-DSFMT_MEXP=19937'
             ['-I' src_path filesep 'libroomsim' filesep 'include']
             ['-I' src_path filesep 'libsfmt']
             ['-I' src_path filesep 'libpocketfft']
             ['-I' src_path filesep 'libmysofa']
             ['-I' src_path filesep 'libmysofa' filesep 'hrtf']
             ['-I' src_path filesep 'wavwriter' filesep 'include']
           };

% Process optional arguments
for i = 1:nargin
    switch varargin{i}
        case 'test'
            switches = [switches
                        '-DUNIT_TEST'
                        '-g'
                        ];
            output = 'sofamyroomtest';
        case 'debug'
            switches = [switches 
                        '-g'
                        ];
        otherwise
            switches = [switches 
                        varargin{i}
                        ];
    end
end

% Link options: libmysofa needs zlib. On macOS and Linux it is a system
% library; on Windows pass the include and library paths of a zlib build
% as extra arguments, e.g. make('-IC:\zlib\include', '-LC:\zlib\lib', '-lzlib').
options = {};
if ~ispc
    options = [options
               '-lz'
              ];
end

mexfiles = { [src_path filesep 'libroomsim' filesep 'source' filesep '3D.c']
             [src_path filesep  'libroomsim'  filesep  'source'  filesep  'dsp.c']
             [src_path filesep  'libroomsim'  filesep  'source'  filesep  'interface.c']
             [src_path filesep  'libroomsim'  filesep  'source'  filesep  'interp.c']
             [src_path filesep  'libroomsim'  filesep  'source'  filesep  'roomsim.c']
             [src_path filesep  'libroomsim'  filesep  'source'  filesep  'rng.c']
             [src_path filesep  'libroomsim'  filesep  'source'  filesep  'sensor.c']
             [src_path filesep  'libsfmt'  filesep  'SFMT.c']
             [src_path filesep  'libpocketfft'  filesep  'pocketfft.c']
             [src_path filesep  'libmysofa'  filesep  'hdf'  filesep  'btree.c']
             [src_path filesep  'libmysofa'  filesep  'hdf'  filesep  'dataobject.c']
             [src_path filesep  'libmysofa'  filesep  'hdf'  filesep  'fractalhead.c']
             [src_path filesep  'libmysofa'  filesep  'hdf'  filesep  'gcol.c']
             [src_path filesep  'libmysofa'  filesep  'hdf'  filesep  'gunzip.c']
             [src_path filesep  'libmysofa'  filesep  'hdf'  filesep  'superblock.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'cache.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'check.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'easy.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'interpolate.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'kdtree.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'lookup.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'loudness.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'minphase.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'neighbors.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'reader.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'resample.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'spherical.c']
             [src_path filesep  'libmysofa'  filesep  'hrtf'  filesep  'tools.c']
             [src_path filesep  'libmysofa'  filesep  'resampler'  filesep  'speex_resampler.c']
             [src_path filesep  'mexmain.c']
             [src_path filesep  'build.c']
             [src_path filesep  'wavwriter' filesep 'source' filesep 'wavwriter.c']
           };

% Remove sofamyroom from memory, if loaded
clear sofamyroom;

% Echo command to be executed
fprintf('mex '); 
fprintf('%s ', switches{:});
fprintf('-output %s ', output);
fprintf('%s ', mexfiles{:});
fprintf('%s ', options{:});
fprintf('\n');

% Run MEX to build the desired target
mex(switches{:}, '-output', output, mexfiles{:}, options{:})

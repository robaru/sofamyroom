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
             '-DMEXP=19937'
             ['-I' src_path filesep 'libroomsim' filesep 'include']
             ['-I' src_path filesep 'libsfmt']
             ['-I' src_path filesep 'wavwriter' filesep 'include']
           };
       
if ispc
    switches =  [switches
                ['-I' src_path filesep 'libfftw' filesep 'Windows' filesep 'x64' filesep 'include']
                ['-I' src_path filesep 'libmysofa' filesep 'Windows' filesep 'x64' filesep 'include']
                ];
            
elseif ismac
    switches = [switches
               ['-I' src_path filesep 'libfftw' filesep 'MacOS' filesep '64bit' filesep 'include']
               ['-I' src_path filesep 'libmysofa' filesep 'MacOS' filesep '64bit' filesep 'include']
               ];
            
elseif isunix
    switches = [switches
               ['-I' src_path filesep 'libmysofa' filesep 'Linux' filesep '64bit' filesep 'include']
               ];
end

% Process optional arguments
debug = false;
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
            debug = true;
        otherwise
            switches = [switches 
                        varargin{i}
                        ];
    end
end

% Set options
options = {};

if ispc
    options = [options
               ['-L' src_path filesep 'libfftw' filesep 'Windows' filesep 'x64' filesep 'lib']
               '-lfftw3-3.lib'
               ];
    if ~debug
        options = [options
                  ['-L' src_path filesep 'libmysofa' filesep 'Windows' filesep 'x64' filesep 'lib' filesep 'Release']
                  '-lmysofa'
            ];
    else
        options = [options
                  ['-L' src_path filesep 'libmysofa' filesep 'Windows' filesep 'x64' filesep 'lib' filesep 'Debug']
                  '-lmysofad'
                  ];
    end
    
elseif ismac
    if ~debug
        options = [options
                  ['-L' src_path filesep 'libmysofa' filesep 'MacOS' filesep '64bit' filesep 'lib' filesep 'Release']
                  '-lmysofa'
                  ['-L' src_path filesep 'libfftw' filesep 'MacOS' filesep '64bit' filesep 'lib' filesep 'Release']
                  '-lfftw3'
            ];
    else
        options = [options
                  ['-L' src_path filesep 'libmysofa' filesep 'MacOS' filesep '64bit' filesep 'lib' filesep 'Debug']
                  '-lmysofad'
                  ['-L' src_path filesep 'libfftw' filesep 'MacOS' filesep '64bit' filesep 'lib' filesep 'Debug']
                  '-lfftw3d'
                  ];
    end
    options = [options
              "-lz"
               ];
    
elseif isunix
    if ~debug
        options = [options
                  ['-L' src_path filesep 'libmysofa' filesep 'Linux' filesep '64bit' filesep 'lib' filesep 'Release']
                  '-lmysofa'  
                  ];
    else
        options = [options
                  ['-L' src_path filesep 'libmysofa' filesep 'Linux' filesep '64bit' filesep 'lib' filesep 'Debug']
                  '-lmysofad'
                  ];
    end
    options = [options
               '-lz'
               '-lfftw3'
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

% Copy libfftw3-3.dll into working directory, Windows only
if ispc
    [status, msg] = copyfile(...
        [src_path filesep 'libfftw' filesep 'Windows' filesep 'x64' filesep 'bin' filesep 'libfftw3-3.dll']...
    );
    if ~status
        fprintf("An error occured while copying libfftw3-3.dll file: %s\n", msg)
    else
        fprintf("libfftw3-3.dll successfully copied.\n")
    end
end

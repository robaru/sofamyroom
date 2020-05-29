% This demo initialize three VAST structure with common parameters (sample
% frequency, sizes, ...) and populate the structure to reproduce
% three of the conditions of the paper.
% At the end of the computation the VAST structure and the BRIR are saved 
% into <RoomName>.mat 
% The functions VASTGeneration is an example on how it is possibile to
% populate the VAST structure. 
% 
% In order to populate properly the VAST, it is necessary to install the
% SOFA toolbox available at: https://github.com/sofacoustics/API_MO
% 
% AUTHOR: Antoine Deleforge and Clément Gaultier,
%       PANAMA Research Group, Inria, France
%       http://thevastproject.inria.fr/dataset/
% adapted by Roberto Barumerli, barumerli@dei.unipd.it
close all 
clear all

if ~isfolder('SOFA/API_MO-master')
    checkSOFAtoolbox()
else
    addpath('SOFA/API_MO-master')
end

% load SOFA library
SOFAstart

% save BRIR into separated stereo WAVE files
SaveWAV = true;

%% Demo VAST
% common parameters
Fs = 16e3;
RoomSize = [7.1 5.1 3];
ReceiverPos = [0.211 0.294 0.5].*RoomSize;

% acoustic parameters
CeilingAbsorb = [0.02   0.06    0.14    0.37    0.60    0.65    0.65];
FloorAbsorb =   [0.55   0.86    0.83    0.87    0.90    0.87    0.87];
WallsAbsorb = CeilingAbsorb;
Diffuse = [0.1    0.1     0.1     0.1     0.1     0.1     0.1];

% download sofa file 
SofaPath = 'database/hrtf_ci1.sofa';
RoomName = 'room_sofa1';
VAST1 = SubVASTGeneration(Fs, RoomSize, RoomName, CeilingAbsorb, ...
                 FloorAbsorb, WallsAbsorb, ReceiverPos, Diffuse, SofaPath);

SofaPath = 'database/mit_kemar_normal_pinna.sofa';
RoomName = 'room_sofa2';
VAST2 = SubVASTGeneration(Fs, RoomSize, RoomName, CeilingAbsorb, ...
                 FloorAbsorb, WallsAbsorb, ReceiverPos, Diffuse, SofaPath);

SofaPath = 'database/subject_003.sofa';
RoomName = 'room_sofa3';
VAST3 = SubVASTGeneration(Fs, RoomSize, RoomName, CeilingAbsorb, ...
                 FloorAbsorb, WallsAbsorb, ReceiverPos, Diffuse, SofaPath);



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% INSTALL SOFA FUNCTION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function checkSOFAtoolbox()
    selection = questdlg(['It appears that the SOFA toolbox' ...
        'is not available. Can I take care to install it?'],'Yes', 'No');
                    
    if strcmp(selection, 'Yes')
        fprintf('Downloading SOFA toolbox...\n')
        file_zip = websave('sofa.zip', ...
              'https://github.com/sofacoustics/API_MO/archive/master.zip');
        fprintf('Extracting into SOFA folder...\n')
        files = unzip(file_zip,'SOFA');
        fprintf('Adding path...\n')
        addpath(files{2})
        fprintf('Removing zip...\n')
        delete(file_zip)
        
        sprintf('Done!\n')
    else
        error(['In order to run this demo you need to download' ...
        ' the SOFA toolbox from https://github.com/sofacoustics/API_MO']) 
    end
end

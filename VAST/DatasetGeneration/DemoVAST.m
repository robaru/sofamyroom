% This script initialize a VAST structure with common parameters (sample
% frequency, sizes, ...) and build the rest of the structure to reproduce
% one of the conditions of the paper.
% At the end of the computation the VAST structure and the BRIR are saved into
% VAST.mat 
% If you want to save the BRIR into a set of WAVE file change the SaveWAV
% variable from false to true. 
% The functions SubVASTGeneration is an example on how it is possibile to
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

if ~isfolder('SOFA/API_MO')
    error(['In order to run this demo you need to download' ...
        ' the SOFA toolbox from https://github.com/sofacoustics/API_MO'])
end
% save BRIR into separated stereo WAVE files
SaveWAV = true;

%% Demo VAST
% common parameters
RoomSize = [7.1 5.1 3];
ReceiverPos = [0.211 0.294 0.5].*RoomSize;
GridDistance = 1;
RoomName = 'room_demo';

% acoustic parameters
CeilingAbsorb = [0.02   0.06    0.14    0.37    0.60    0.65    0.65];
FloorAbsorb =   [0.55   0.86    0.83    0.87    0.90    0.87    0.87];
WallsAbsorb = CeilingAbsorb;
Diffuse = [0.1    0.1     0.1     0.1     0.1     0.1     0.1];

% download sofa file (in this example, the first subject from the CIPIC
% dataset is used)
SofaPath1 = 'database/hrtf_ci1.sofa'; %websave('subject_008.sofa','https://sofacoustics.org/data/database/cipic/subject_008.sofa');
VAST1 = SubVASTGeneration( RoomSize, RoomName, CeilingAbsorb, FloorAbsorb, WallsAbsorb, GridDistance, ReceiverPos, Diffuse, SofaPath1, SaveWAV);

SofaPath2 = 'database/mit_kemar_normal_pinna.sofa'; %websave('subject_008.sofa','https://sofacoustics.org/data/database/cipic/subject_008.sofa');
VAST2 = SubVASTGeneration( RoomSize, RoomName, CeilingAbsorb, FloorAbsorb, WallsAbsorb, GridDistance, ReceiverPos, Diffuse, SofaPath2, SaveWAV);

SofaPath3 = 'database/subject_003.sofa'; %websave('subject_008.sofa','https://sofacoustics.org/data/database/cipic/subject_008.sofa');
VAST3 = SubVASTGeneration( RoomSize, RoomName, CeilingAbsorb, FloorAbsorb, WallsAbsorb, GridDistance, ReceiverPos, Diffuse, SofaPath3, SaveWAV);

VAST = VASTmerge(VAST1,VAST2);
VAST = VASTmerge(VAST,VAST3);





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
% AUTHOR: Antoine Deleforge and Clément Gaultier,
%       PANAMA Research Group, Inria, France
%       http://thevastproject.inria.fr/dataset/
% adapted by Roberto Barumerli, barumerli@dei.unipd.it
close all 
clear all

% download sofa file (in this example, the first subject from the CIPIC
% dataset is used)
SofaPath = websave('subject_008.sofa','https://sofacoustics.org/data/database/cipic/subject_008.sofa');

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


VAST = SubVASTGeneration( RoomSize, RoomName, CeilingAbsorb, FloorAbsorb, WallsAbsorb, GridDistance, ReceiverPos, Diffuse, SofaPath, SaveWAV);


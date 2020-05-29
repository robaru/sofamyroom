% This initialize a VAST structure with common parameters (sample
% frequency, sizes, ...) and build the rest of the structure to reproduce
% one of the conditions of the paper.
% At the end of the computation the VAST structure and the BRIR are saved into
% VAST.mat 
% If you want to save the BRIR into a set of WAVE file change the SaveWAV
% variable from false to true. 
close all 
clear all
% sofa_path = 'https://sofacoustics.org/data/database/cipic/subject_003.sofa'
sofa_path = fullfile('data','subject_003.sofa');

%% Demo VAST
% common parameters
RoomSize = [7.1 5.1 3];
SaveWAV = false;

ReceiverPos = [0.211 0.294 0.5].*RoomSize;

CeilingAbsorb = [0.02   0.06    0.14    0.37    0.60    0.65    0.65];
FloorAbsorb =   [0.55   0.86    0.83    0.87    0.90    0.87    0.87];
WallsAbsorb = CeilingAbsorb;
Diffuse = [0.1    0.1     0.1     0.1     0.1     0.1     0.1];


RoomName = sprintf('room%i', r);

VAST = SubVASTGeneration( RoomSize, RoomNb, CeilingAbsorb, FloorAbsorb, WallsAbsorb, SourcePos, ReceiveLoc, Diffuse, SaveWAV);
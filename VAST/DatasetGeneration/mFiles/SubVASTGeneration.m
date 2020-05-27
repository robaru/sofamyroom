function [ SubsetVAST ] = SubVASTGeneration(RoomSize, RoomNb, CeilingAbsorb, FloorAbsorb, WallsAbsorb, GridDistance, ReceiveLoc, Diffuse, SaveWAV)
%SubVASTGeneration Provides a VAST structure with impulse responses
%corresponding to the sources inside the domain of 'RoomSize' at the
%distance 'GridDistance' from the receiver which is at the 'ReceiveLoc'
%position
%   RoomSize: 
%   GridDistance: 
%   ReceiveLoc: 
%   Absorb:
%   Diffuse:
%

JobID = num2str(randi(1000));
    
%% Paths and folders

ResDir = ['./Results/Room' RoomNb '/'];

if ~isfolder(ResDir)
    mkdir(ResDir)
end


%% Sampled points generation
EquatorStep = 9;
% CartCoord and SphereCoord are in the receiver referential
[CartCoord, SpherCoord] = SphereSampling(GridDistance, EquatorStep);

% Referential shifting from receiver to room
CartCoord(1,:) = CartCoord(1,:) + ReceiveLoc(1); % x
CartCoord(2,:) = CartCoord(2,:) + ReceiveLoc(2); % y
CartCoord(3,:) = CartCoord(3,:) + ReceiveLoc(3); % z

%% Inside domain validity check

% Room Dimensions
%RoomSize = [3.5,5,2.50];
RWidth = RoomSize(1);
RLength = RoomSize(2);
RHeight = RoomSize(3);
% Indexing valid points from the sampled sphere which fall inside the room
V = (CartCoord(3,:)>0.2)&(CartCoord(3,:)<(RHeight-0.2))&(CartCoord(2,:)>0.2)&(CartCoord(2,:)<(RLength-0.2))&(CartCoord(1,:)>0.2)&(CartCoord(1,:)<(RWidth-0.2));
% Indexing coordinates
ValidCartCoord = CartCoord(:,V);
ValidSpherCoord = SpherCoord(:,V);


%% SubVAST initialization

SubsetVAST.GlobalParams.SamplingFrequency = 16000;
SubsetVAST.GlobalParams.FreqBin = [125,250,500,1000,2000,4000,8000];
SubsetVAST.GlobalParams.RIRLengthTime = 1.25;
SubsetVAST.GlobalParams.RIRLengthSamples = SubsetVAST.GlobalParams.RIRLengthTime.*SubsetVAST.GlobalParams.SamplingFrequency;
RIRNumber = length(ValidCartCoord);            
SubsetVAST = VASTinit(SubsetVAST,RIRNumber);

%% SofaMyRoom setup

% The mex file supplied works with MATLAB installed on 64-bit linux distro only. If on windows or mac, contact me. I can explain how to handle that case.
sofamyroom load omnidirectional;	% loads the receiver model
sofamyroom load SOFA ../hrtf/mit_kemar_normal_pinna.sofa;	% loads the source model

%% Creating parameter variable 'SofaMyRoomParam'

% loading basic parameters of the room

%
%    room settings
SofaMyRoomParam.room.humidity               = 0.42;         % relative humidity (0,...,1)
SofaMyRoomParam.room.temperature            = 20;           % room temperature (celsius)

SofaMyRoomParam.room.surface.frequency      = SubsetVAST.GlobalParams.FreqBin;
%
%    simulation options

SofaMyRoomParam.options.bandsperoctave      = 1;                    % simulation frequency accuracy (1, 2, 3, or 4 bands/octave)
SofaMyRoomParam.options.referencefrequency  = 125;                  % reference frequency for frequency octaves
SofaMyRoomParam.options.airabsorption       = true;                 % apply air absorption?
SofaMyRoomParam.options.distanceattenuation = true;                 % apply distance attenuation?
SofaMyRoomParam.options.subsampleaccuracy   = false;                % apply subsample accuracy?
SofaMyRoomParam.options.highpasscutoff      = 0;                    % 3dB frequency of high-pass filter (0=none)
SofaMyRoomParam.options.verbose             = true;                 % print status messages?

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    specular reflections simulation options
%
SofaMyRoomParam.options.simulatespecular    = true;                 % simulate specular reflections?
SofaMyRoomParam.options.reflectionorder     = [ 10 10 10 ];         % maximum specular reflection order (x,y,z)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    diffuse reflections simulation options
%
SofaMyRoomParam.options.simulatediffuse     = true;                 % simulate diffuse reflections?
SofaMyRoomParam.options.numberofrays        = 2000;                 % number of rays in simulation (20*K^2)
SofaMyRoomParam.options.diffusetimestep     = 0.010;                % time resolution in diffuse energy histogram (seconds)
SofaMyRoomParam.options.rayenergyfloordB    = -80;                  % ray energy threshold (dB, with respect to initial energy)
SofaMyRoomParam.options.uncorrelatednoise   = true;                 % use uncorrelated poisson arrivals for binaural impulse responses?

SofaMyRoomParam.options.outputname			= 'output';           	% name of the output file
SofaMyRoomParam.options.saveaswav	        = SaveWAV;                 % format of the ouput file

% appending more parameters in variable 'SofaMyRoomParam'
SofaMyRoomParam.room.dimension	             = RoomSize;
SofaMyRoomParam.receiver(1).location       	 = ReceiveLoc;
SofaMyRoomParam.receiver(1).orientation		 = [ 0, 0, 0 ];
SofaMyRoomParam.receiver(1).description     	 = 'SOFA ../hrtf/mit_kemar_normal_pinna.sofa';
SofaMyRoomParam.source(1).orientation			 = [ 0, 0, 0 ];
SofaMyRoomParam.source(1).description		     = 'omnidirectional';
SofaMyRoomParam.room.surface.diffusion = repmat(Diffuse,6,1);	% diffusion profiles for all 6 surfaces % Diffuse
SofaMyRoomParam.options.simulatediffuse = false;	% change this to false if user does not want to simulate diffuse reflections
SofaMyRoomParam.options.fs = SubsetVAST.Receiver.HRTF.Fs; % Different from SubsetVAST.Global.SamplingFrequency because of the KEMARSmall.hrtf file
SofaMyRoomParam.options.responseduration = SubsetVAST.GlobalParams.RIRLengthTime;
SofaMyRoomParam.room.surface.absorption = [repmat(WallsAbsorb,4,1); FloorAbsorb; CeilingAbsorb]; % Absorb

assert(size(SofaMyRoomParam.room.surface.absorption,2) == length(SubsetVAST.GlobalParams.FreqBin))
%% Impulse response generation

for source = 1:1:RIRNumber
    % Compute impulse responses
    SofaMyRoomParam.source(1).location = ValidCartCoord(:,source);
    ImpRes=sofamyroom(SofaMyRoomParam);

    % Get Rt60 value
    RtParams.Fs = SofaMyRoomParam.options.fs;
    RtParams.FreqBin = SofaMyRoomParam.room.surface.frequency;
    SubsetVAST.Room.FreqRT60(:,source) = Room2RT60(SofaMyRoomParam);
    SubsetVAST.Room.GlobalRT60(1,source) = mean(Room2RT60(SofaMyRoomParam));

    % Resampling to SubsetVAST.Global.SamplingFrequency
    [p,q] = rat(SubsetVAST.GlobalParams.SamplingFrequency/SubsetVAST.Receiver.HRTF.Fs);
    ImpRes = resample(ImpRes,p,q);

    % Store them in the VAST struct
    SubsetVAST.RIR.Left(:,source) = single(ImpRes(:,1));
    SubsetVAST.RIR.Right(:,source) = single(ImpRes(:,2));

    % Updating relative position
    SubsetVAST.Source.Distance(1,source) = ValidSpherCoord(1,source);
    SubsetVAST.Source.Azimuth(1,source) = ValidSpherCoord(2,source);
    SubsetVAST.Source.Elevation(1,source) = ValidSpherCoord(3,source);

    % Updating absolute position
    SubsetVAST.Source.AbsolutePos(:,source) = ValidCartCoord(:,source);
end

%% Updating the VAST structure

% Room
SubsetVAST.Room.Size = repmat(RoomSize',1,RIRNumber);

% Receiver
SubsetVAST.Receiver.Position = repmat(ReceiveLoc',1,RIRNumber);

% Absorption values
SubsetVAST.Room.Absorption.NorthWall = repmat(WallsAbsorb',1,RIRNumber);
SubsetVAST.Room.Absorption.WestWall = repmat(WallsAbsorb',1,RIRNumber);
SubsetVAST.Room.Absorption.SouthWall = repmat(WallsAbsorb',1,RIRNumber);
SubsetVAST.Room.Absorption.EastWall = repmat(WallsAbsorb',1,RIRNumber);
SubsetVAST.Room.Absorption.Ceiling = repmat(CeilingAbsorb',1,RIRNumber);
SubsetVAST.Room.Absorption.Floor = repmat(FloorAbsorb',1,RIRNumber);

% Diffusion values
SubsetVAST.Room.Diffusion = repmat(Diffuse',1,RIRNumber);


%% Saving the mat file

FileName = [ResDir 'JobID_' JobID '_Grid_' num2str(GridDistance)...
    '_RecLoc_' num2str(ReceiveLoc) '.mat'];

FileName(isspace(FileName)) = [];

save(FileName, 'SubsetVAST');
end


function [ SubsetVAST ] = SubVASTGeneration(RoomSize, RoomName, CeilingAbsorb, FloorAbsorb, WallsAbsorb, GridDistance, ReceiveLoc, Diffuse, SofaPath, SaveWAV)
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% AUTHORS: Antoine Deleforge and Clément Gaultier,
%       PANAMA Research Group, Inria, France
%       http://thevastproject.inria.fr/dataset/
% adapted by Roberto Barumerli, barumerli@dei.unipd.it
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Paths and folders

ResDir = ['./Room' RoomName '/'];

if ~isfolder(ResDir)
    mkdir(ResDir)
end

%% Sampled points generation
EquatorStep = 60;
% CartCoord and SphereCoord are in the receiver referential
[CartCoord, SpherCoord] = SphereSampling(GridDistance, EquatorStep);

% Referential shifting from receiver to room
CartCoord(1,:) = CartCoord(1,:) + ReceiveLoc(1); % x
CartCoord(2,:) = CartCoord(2,:) + ReceiveLoc(2); % y
CartCoord(3,:) = CartCoord(3,:) + ReceiveLoc(3); % z

%% Inside domain validity check

% Room Dimensions
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

SofaMyRoomParam.options.outputname			= 'brir';           	% name of the output file
SofaMyRoomParam.options.saveaswav	        = SaveWAV;                 % format of the ouput file

% appending more parameters in variable 'SofaMyRoomParam'
SofaMyRoomParam.room.dimension	             = RoomSize;
SofaMyRoomParam.receiver(1).location       	 = ReceiveLoc;
SofaMyRoomParam.receiver(1).orientation		 = [ 0, 0, 0 ];
SofaMyRoomParam.receiver(1).description     	 = sprintf('SOFA %s interp=1 norm=1 resampling=1', SofaPath);
SofaMyRoomParam.source(1).orientation			 = [ 0, 0, 0 ];
SofaMyRoomParam.source(1).description		     = 'omnidirectional';
SofaMyRoomParam.room.surface.diffusion = repmat(Diffuse,6,1);	% diffusion profiles for all 6 surfaces % Diffuse
SofaMyRoomParam.options.simulatediffuse = true;	% change this to false if user does not want to simulate diffuse reflections
SofaMyRoomParam.options.fs = SubsetVAST.Receiver.HRTF.Fs; % Different from SubsetVAST.Global.SamplingFrequency because of the KEMARSmall.hrtf file
SofaMyRoomParam.options.responseduration = SubsetVAST.GlobalParams.RIRLengthTime;
SofaMyRoomParam.room.surface.absorption = [repmat(WallsAbsorb,4,1); FloorAbsorb; CeilingAbsorb]; % Absorb

assert(size(SofaMyRoomParam.room.surface.absorption,2) == length(SubsetVAST.GlobalParams.FreqBin))
%% Impulse response generation

for source = 1:RIRNumber
    fprintf("BRIR %i/%i\n", source, RIRNumber)
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

FileName = sprintf("%s%s.mat", ResDir, RoomName);

FileName(isspace(FileName)) = [];

save(FileName, 'SubsetVAST');


function [RT60,estimator] = Room2RT60(param,estimator)
% Calculate a room reverberation time (RT60) using a Sabine or Norris-Eyring estimate.
% estimator is of type character and is one of:'Sabine','SabineAir','SabineAirHiAbs','Norris_Eyring'
% Ref. http://www.teicontrols.com/notes/AcousticsEE363N/EngineeringAcoustics.pdf

if nargin==1, estimator = ''; end

% extract room dimensions from parameter structure
Rx = param.room.dimension(1);
Ry = param.room.dimension(2);
Rz = param.room.dimension(3);

% extract absorption data from parameter structure
AC = param.room.surface.absorption';
AF = param.room.surface.frequency';

% calculate frequency dependent air absorption
AirAbs = 5.5E-4*(0.50/param.room.humidity)*(AF/1000).^(1.7); % m_air
% calculate speed of sound (m/s) 
SpeedOfSound = round(331*sqrt(1+0.0036*param.room.temperature)); 

Volume = Rx * Ry * Rz; % V
AreaXY = Rx * Ry;  % Sxy
AreaXZ = Rx * Rz;
AreaYZ = Ry * Rz;

SurfaceArea = 2 * (AreaXY + AreaXZ + AreaYZ); % S
EffAbsArea = AreaYZ * (AC(:,1) + AC(:,2)) + ...
             AreaXZ * (AC(:,3) + AC(:,4)) + ...
             AreaXY * (AC(:,5) + AC(:,6)); % Se
MeanAbs = EffAbsArea / SurfaceArea; % a_bar

MeanAirAbs = mean(AirAbs); % m
% compute mean free path = average distance between successive reflections (Ref A4)
MeanFreePath = 4 * Volume / SurfaceArea; % MFP

% prepare output
RT60 = zeros(size(AF));

% detect anechoic case and return all zeros RT60
if abs(1 - MeanAbs) < eps, return; end

switch estimator
    case 'Sabine'
        % Sabine equation
        RT60 = (55.25/SpeedOfSound)*Volume./EffAbsArea; 

    case 'SabineAir'
        % Sabine equation (SI units) adjusted for air
        RT60 = (55.25/SpeedOfSound)*Volume./(4*AirAbs * Volume + EffAbsArea); 

    case 'SabineAirHiAbs'
        % Sabine equation (SI units) adjusted for air and high absorption
        RT60 = (55.25/SpeedOfSound)*Volume./(4*AirAbs * Volume + EffAbsArea .* (1+MeanAbs/2)); 

    otherwise
        % Norris-Eyring estimate adjusted for air absorption
        RT60 = (55.25/SpeedOfSound)*Volume./(4*AirAbs * Volume - SurfaceArea * log(1-MeanAbs+eps));
        estimator = 'Norris-Eyring';
end


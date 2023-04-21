function VAST = VASTGeneration(Fs, RoomSize, RoomName, ...
                               CeilingAbsorb, FloorAbsorb, WallsAbsorb, ...
                               ReceiveLoc, Diffuse, SofaPath)
% VASTGeneration: Provides a VAST structure with impulse responses
% corresponding to the sources inside the domain of 'RoomSize' for each 
% point of the spherical grid into the SOFA file, 
% from the receiver which is at the 'ReceiveLoc'.
%
%   Input
%
%     Fs: sampling rate of the dataset
%     RoomSize: room dimensions (x,y,z)
%     CeilingAbsorb: absorption coefficients for the ceiling 
%     FloorAbsorb: absorption coefficients for the floor
%     WallsAbsorb: absorption coefficients for the walls
%     ReceiveLoc: receiver position 
%     Diffuse: scattering coefficients for all the surfaces
%     SofaPath: path of the sofa file 
%
%   Output
%     VAST: structure containing the BRIRs and the relative metadata
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

ResDir = sprintf('./%s/', RoomName);

if ~isfolder(ResDir)
    mkdir(ResDir)
end

%% Load SOFA data
SofaObj = SOFAload(SofaPath);

%% Sampled points generation
% Referential shifting from receiver to room
SourceCoord.Cartesian = transpose(SOFAconvertCoordinates(...
                    SofaObj.SourcePosition, SofaObj.SourcePosition_Type,...
                    'cartesian', SofaObj.SourcePosition_Units, 'metre'));
SourceCoord.Spherical = transpose(SofaObj.SourcePosition);
CartCoord(1,:) = SourceCoord.Cartesian(1,:) + ReceiveLoc(1); % x
CartCoord(2,:) = SourceCoord.Cartesian(2,:) + ReceiveLoc(2); % y
CartCoord(3,:) = SourceCoord.Cartesian(3,:) + ReceiveLoc(3); % z

%% Inside domain validity check

% Room Dimensions
RWidth = RoomSize(1);
RLength = RoomSize(2);
RHeight = RoomSize(3);
% Indexing valid points from the sampled sphere which fall inside the room
V = (CartCoord(3,:)>0.2)&(CartCoord(3,:)<(RHeight-0.2))& ...
    (CartCoord(2,:)>0.2)&(CartCoord(2,:)<(RLength-0.2))& ...
    (CartCoord(1,:)>0.2)&(CartCoord(1,:)<(RWidth-0.2));
% Indexing coordinates
ValidCartCoord = CartCoord(:,V);
ValidSpherCoord = SourceCoord.Spherical(:,V);


%% SubVAST initialization

VAST.GlobalParams.SamplingFrequency = Fs;
VAST.GlobalParams.FreqBin = [125,250,500,1000,2000,4000,8000];
VAST.GlobalParams.RIRLengthTime = 1.25;
VAST.GlobalParams.RIRLengthSamples = ...
      VAST.GlobalParams.RIRLengthTime.*VAST.GlobalParams.SamplingFrequency;
RIRNumber = length(ValidCartCoord);  
VAST.Receiver = struct('Position',single(zeros(3,RIRNumber)), ...
        'Orientation',single(zeros(1,RIRNumber)), ...
        'HRTF', struct('Left',single([SofaObj.API.N,SofaObj.API.M]),...
        'Right',single([SofaObj.API.N,SofaObj.API.M]),...
        'Fs',SofaObj.Data.SamplingRate));

VAST = VASTinit(VAST,RIRNumber);

%% Creating parameter variable 'SofaMyRoomParam'
% loading basic parameters of the room, check sofamyroom parameters

%    room settings
SofaMyRoomParam.room.humidity               = 0.42;
SofaMyRoomParam.room.temperature            = 20;
SofaMyRoomParam.options.responseduration    = ...
                                        VAST.GlobalParams.RIRLengthTime;
SofaMyRoomParam.room.surface.frequency      = VAST.GlobalParams.FreqBin;
%
%    simulation options
SofaMyRoomParam.options.fs                  = VAST.Receiver.HRTF.Fs; 
SofaMyRoomParam.options.bandsperoctave      = 1;
SofaMyRoomParam.options.referencefrequency  = 125;
SofaMyRoomParam.options.airabsorption       = true;
SofaMyRoomParam.options.distanceattenuation = true;
SofaMyRoomParam.options.subsampleaccuracy   = false;
SofaMyRoomParam.options.highpasscutoff      = 0;    
SofaMyRoomParam.options.verbose             = true; 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    specular reflections simulation options
%
SofaMyRoomParam.options.simulatespecular    = true; 
SofaMyRoomParam.options.reflectionorder     = [ 10 10 10 ];
SofaMyRoomParam.room.surface.absorption = ...
                     [repmat(WallsAbsorb,4,1); FloorAbsorb; CeilingAbsorb];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    diffuse reflections simulation options
%
SofaMyRoomParam.options.simulatediffuse     = true;
SofaMyRoomParam.room.surface.diffusion      = repmat(Diffuse,6,1);
SofaMyRoomParam.options.numberofrays        = 2000;
SofaMyRoomParam.options.diffusetimestep     = 0.010;
SofaMyRoomParam.options.rayenergyfloordB    = -80;
SofaMyRoomParam.options.uncorrelatednoise   = true;

SofaMyRoomParam.options.outputname			= 'brir';
SofaMyRoomParam.options.mex_saveaswav       = false;
SofaMyRoomParam.options.saveaswav           = false;

% appending more parameters in variable 'SofaMyRoomParam'
SofaMyRoomParam.room.dimension	             = RoomSize;
SofaMyRoomParam.receiver(1).location       	 = ReceiveLoc;
SofaMyRoomParam.receiver(1).orientation		 = [ 0, 0, 0 ];
SofaMyRoomParam.receiver(1).description      = ...
                 sprintf('SOFA %s interp=1 norm=1 resampling=1', SofaPath);
SofaMyRoomParam.source(1).orientation		 = [ 0, 0, 0 ];
SofaMyRoomParam.source(1).description		 = 'omnidirectional';

assert(size(SofaMyRoomParam.room.surface.absorption,2) == ...
                                        length(VAST.GlobalParams.FreqBin))
%% Impulse response generation

for source = 1:2%RIRNumber
    fprintf("BRIR %i/%i\n", source, RIRNumber)
    % Compute impulse responses
    SofaMyRoomParam.source(1).location = ValidCartCoord(:,source);
    
    ImpRes=sofamyroom(SofaMyRoomParam);
    
    % Resampling to SubsetVAST.Global.SamplingFrequency
    [p,q] = rat(VAST.GlobalParams.SamplingFrequency/VAST.Receiver.HRTF.Fs);
    ImpRes = resample(ImpRes,p,q);

    % Store them in the VAST struct
    VAST.RIR.Left(:,source) = single(ImpRes(:,1));
    VAST.RIR.Right(:,source) = single(ImpRes(:,2));

    % Get Rt60 value
    RtParams.Fs = SofaMyRoomParam.options.fs;
    RtParams.FreqBin = SofaMyRoomParam.room.surface.frequency;
    VAST.Room.FreqRT60(:,source) = Room2RT60(SofaMyRoomParam);
    VAST.Room.GlobalRT60(1,source) = mean(Room2RT60(SofaMyRoomParam));

    % Updating relative position
    VAST.Source.Distance(1,source) = ValidSpherCoord(1,source);
    VAST.Source.Azimuth(1,source) = ValidSpherCoord(2,source);
    VAST.Source.Elevation(1,source) = ValidSpherCoord(3,source);

    % Updating absolute position
    VAST.Source.AbsolutePos(:,source) = ValidCartCoord(:,source);
end

%% Updating the VAST structure

% Room
VAST.Room.Size = repmat(RoomSize',1,RIRNumber);

% Receiver
VAST.Receiver.Position = repmat(ReceiveLoc',1,RIRNumber);

% Absorption values
VAST.Room.Absorption.NorthWall = repmat(WallsAbsorb',1,RIRNumber);
VAST.Room.Absorption.WestWall = repmat(WallsAbsorb',1,RIRNumber);
VAST.Room.Absorption.SouthWall = repmat(WallsAbsorb',1,RIRNumber);
VAST.Room.Absorption.EastWall = repmat(WallsAbsorb',1,RIRNumber);
VAST.Room.Absorption.Ceiling = repmat(CeilingAbsorb',1,RIRNumber);
VAST.Room.Absorption.Floor = repmat(FloorAbsorb',1,RIRNumber);

% Diffusion values
VAST.Room.Diffusion = repmat(Diffuse',1,RIRNumber);


%% Saving the mat file

FileName = sprintf("%s%s.mat", ResDir, RoomName);

FileName(isspace(FileName)) = [];

save(FileName, 'VAST');


function [RT60,estimator] = Room2RT60(param,estimator)
% Calculate a room reverberation time (RT60)
% estimator is of type character and is one of:
%      'Sabine','SabineAir','SabineAirHiAbs','Norris_Eyring'

if nargin==1, estimator = ''; end

% extract room dimensions from parameter structure
Rx = param.room.dimension(1);
Ry = param.room.dimension(2);
Rz = param.room.dimension(3);

% extract absorption data from parameter structure
AC = param.room.surface.absorption';
AF = param.room.surface.frequency';

% calculate frequency dependent air absorption
AirAbs = 5.5E-4*(0.50/param.room.humidity)*(AF/1000).^(1.7);
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
        RT60 = (55.25/SpeedOfSound)*Volume./ ...
                    (4*AirAbs * Volume + EffAbsArea); 

    case 'SabineAirHiAbs'
        % Sabine equation (SI units) adjusted for air and high absorption
        RT60 = (55.25/SpeedOfSound)*Volume./ ...
                    (4*AirAbs * Volume + EffAbsArea .* (1+MeanAbs/2)); 

    otherwise
        % Norris-Eyring estimate adjusted for air absorption
        RT60 = (55.25/SpeedOfSound)*Volume./ ...
                    (4*AirAbs * Volume - SurfaceArea * log(1-MeanAbs+eps));
        estimator = 'Norris-Eyring';
end


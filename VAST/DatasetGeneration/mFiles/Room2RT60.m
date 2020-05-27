function [RT60,estimator] = Room2RT60(param,estimator)
% Calculate a room reverberation time (RT60) using a Sabine or Norris-Eyring estimate.
% estimator is of type character and is one of:'Sabine','SabineAir','SabineAirHiAbs','Norris_Eyring'
% Ref. http://www.teicontrols.com/notes/AcousticsEE363N/EngineeringAcoustics.pdf

if nargin==1, estimator = ''; end;

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
if abs(1 - MeanAbs) < eps, return; end;

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
end;

% if nargout==0, RT60 = RT60(near(AF,500)); end;

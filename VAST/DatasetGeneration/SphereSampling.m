function [ CartCoord, SpherCoord ] = SphereSampling( Radius, EquatorStep)
%SphereSampling Sample points on a sphere of radius 'Radius' with a
%discretization corresponding to the azimuth step at the equador line (0 deg
%elevation) and elevation discretization corresponding to EquatorStep
%   Inputs
%
%   Radius: radius of the sphere in meters 1x1 double
%   EquatorStep: Azimuth discretization step at the equator line in degrees 1x1 double
%   
%
%   Outputs
%
%   CartCoord : cartezian coordinates 3xN double
%   SpherCoord : spherical coordinates 3xN double
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% AUTHORS: Antoine Deleforge and Clément Gaultier,
%       PANAMA Research Group, Inria, France
%       http://thevastproject.inria.fr/dataset/
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


rng('shuffle');

nbPointEquator = 360/EquatorStep;
nbPoint = 0;
RandElevationShift = rand*EquatorStep;
for el = (-90+RandElevationShift):EquatorStep:(90+RandElevationShift-EquatorStep)
    el_rad = pi*el/180;   
    nbAz = ceil(nbPointEquator*cos(el_rad)+0.01);
    step_az = 360/nbAz;
    RandAzimuthShift = rand*step_az;
    for az =(-180+RandAzimuthShift):step_az:(180-step_az+RandAzimuthShift)
        nbPoint = nbPoint + 1;
        az_rad = pi*(az)/180;
        CartCoord(1,nbPoint) = Radius*cos(el_rad)*cos(az_rad); % x
        CartCoord(2,nbPoint) = Radius*cos(el_rad)*sin(az_rad); % y
        CartCoord(3,nbPoint) = Radius*sin(el_rad); % z
        
        SpherCoord(1,nbPoint) = Radius; % R
        SpherCoord(2,nbPoint) = az; % Theta (azimuth)
        SpherCoord(3,nbPoint) = el; % Phi (elevation)
    end
end

end


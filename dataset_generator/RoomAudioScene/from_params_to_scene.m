function [vast] = from_params_to_scene(vast, params, verbose)

%   ATTENTION: before any usage of rnd function, use
%               rng('shuffle');
Fs = params.Fs;
n_rirs = height(vast);

TDOA_thr  = 0.05/Fs;
iTDOA_thr = 0.05/Fs;
TDOE_thr  = 0.01/Fs;

% Helpers and lambdas
uniform = @(N,a,b) (a + (b - a).*rand(N,1)); % return 1xN random vector in (a,b)
uniform_with_n_random_margin = ...          % return 1xN random vector in (a+margin)
                        @(a,rnd_b) ...      % with margin which is 1xN random as well
                         (a + (rnd_b-a).*rand(length(rnd_b),1));
row_norm = @(A) sqrt(sum(A.^2,2));


fprintf('Generating parameters of the auditory scene: \n')

%% generate possible rooms
rng('shuffle');
vast.RoomSize(:,1) = uniform(n_rirs, params.min_width, params.max_width); % width in meters
rng('shuffle');
vast.RoomSize(:,2) = uniform(n_rirs, params.min_length, params.max_length); % length in meters
rng('shuffle');
vast.RoomSize(:,3) = uniform(n_rirs, params.min_height, params.max_height); % height in meters
%%%%%
n_bands = size(params.abs_coef_freq_bin, 2);

% % Diffusion
assert(all(size(params.diffuse) == [1,n_bands]))
wall_diff = params.diffuse;
vast.diff_wall_north   = repmat(wall_diff, n_rirs, 1);
vast.diff_wall_east    = repmat(wall_diff, n_rirs, 1);
vast.diff_wall_west    = repmat(wall_diff, n_rirs, 1);
vast.diff_wall_south   = repmat(wall_diff, n_rirs, 1);
vast.diff_wall_floor   = repmat(wall_diff, n_rirs, 1);
vast.diff_wall_ceiling = repmat(wall_diff, n_rirs, 1);
%
% % Absorption
wall_abs = uniform(n_rirs, params.min_walls_abs, params.max_walls_abs);
picnic_abs = uniform(n_rirs, params.min_picnic_abs, params.max_picnic_abs);
vast.abs_wall_north   = repmat(wall_abs,  [1,n_bands]);
vast.abs_wall_east    = repmat(wall_abs,  [1,n_bands]);
vast.abs_wall_west    = repmat(wall_abs,  [1,n_bands]);
vast.abs_wall_south   = repmat(wall_abs,  [1,n_bands]);
vast.abs_wall_floor   = repmat(picnic_abs,[1,n_bands]);
vast.abs_wall_ceiling = repmat(wall_abs,  [1,n_bands]);

PicnicSize(:,1) = vast.RoomSize(:,1) - params.wall_mic_tol;
PicnicSize(:,2) = vast.RoomSize(:,2) - params.wall_mic_tol;
PicnicSize(:,3) = params.max_mic_height;

% Receiver (Position in the room)
% First of the 2 receivers is chosen randomly in the room and then the
% 2nd receiver is drawn on a uniformly sampled sphere of random diameter between 4
% to 16 cm which center is the first receiver.
% Choice of the distance between the 2 sensors
rng('shuffle');
SensorDist = uniform(n_rirs, params.min_mic_dist, params.max_mic_dist);
% Choice of the 1st sensor location
rng('shuffle');
Sensor1Loc(:,1) = uniform_with_n_random_margin(params.wall_mic_tol, PicnicSize(:,1));
rng('shuffle');
Sensor1Loc(:,2) = uniform_with_n_random_margin(params.wall_mic_tol, PicnicSize(:,2));
rng('shuffle');
if params.do_fix_mic2surf_height
    params.min_mic_height = params.max_mic_height;
end
Sensor1Loc(:,3) = uniform(n_rirs, params.min_mic_height, PicnicSize(:,3));
% check
if any(are_out_of_the_shoebox(Sensor1Loc,vast.RoomSize))
    error(['Ouch! Sensor1 is outside of the room.', ...
            'Are you sure? Maybe you want to check it.', ...
            'Maybe it s on porpose, if so, comment me!'])
end

% Choice of the 2nd sensor position in the local coordinate system of the
% 1st sensor
Sensor2Az(:,1) = uniform(n_rirs, 0, 2*pi);
mics_max_elevation_rad = deg2rad(params.mics_max_elevation);  % in rad
mics_min_elevation_rad = -deg2rad(params.mics_max_elevation);  % in rad
Sensor2El(:,1) = uniform(n_rirs, mics_min_elevation_rad, mics_max_elevation_rad);

[Sensor2Cart(:,1), Sensor2Cart(:,2), Sensor2Cart(:,3)] = ...
    sph2cart(SensorDist,Sensor2Az,Sensor2El);
% in the global reference
Sensor2Loc = Sensor1Loc + Sensor2Cart;

% Rejection sampling to remove outliers
% close to floor assumption: the entire microphone array must be within the
% the boundaries
fprintf('  Sampling and rejection for microphone 2...\n')
local_condition = @(pos, min_h_thr, room_size, picnic_size) ...
                        (are_out_of_the_shoebox(pos,room_size))  ...    % sensor outside of the room
                        | (are_out_of_the_shoebox(pos, picnic_size)) ...% sensor outside of the picnic
                        | (pos(:,3) < min_h_thr); % sensor lower than the thr for the picnic

outl = local_condition(Sensor2Loc, params.min_mic_height, vast.RoomSize, PicnicSize);
assert(all(size(outl) == [n_rirs,1]))
tic
while any(outl)
    n_outl = sum(outl);
    fprintf('Searching for %d new samples\n',n_outl);
    % drawn new samples
    rng('shuffle');
    Sensor2Az(outl,1) = uniform(n_outl, 0, 2*pi);
    rng('shuffle');
    Sensor2El(outl,1) = uniform(n_outl, mics_min_elevation_rad, mics_max_elevation_rad);
    [Sensor2Cart(outl,1), Sensor2Cart(outl,2), Sensor2Cart(outl,3)] ...
            = sph2cart(SensorDist(outl),Sensor2Az(outl),Sensor2El(outl));
    % Back to the room coordinates system
    Sensor2Loc(outl,:) = Sensor1Loc(outl,:) + Sensor2Cart(outl,:);
    % update count of outliers
    outl = local_condition(Sensor2Loc, params.min_mic_height, vast.RoomSize, PicnicSize);
end
fprintf(' done!\n  It seems you spent %1.3f seconds trying to put them all in the shoebox\n\n',toc)

% check
if any(are_out_of_the_shoebox(Sensor2Loc, vast.RoomSize))
    error('Ouch! Sensor2 is outside of the room')
end
if any(are_out_of_the_shoebox(Sensor2Loc, PicnicSize))
    error('Ouch! Sensor2 is outside of the picnicbox')
end
if any(are_out_of_the_shoebox(Sensor1Loc, PicnicSize))
    error('Ouch! Sensor1 is outside of the picnicbox')
end

% Receiver Orientation
Receiver1Orientation = zeros(n_rirs,1);%-180 + 360*rand(n_rirs,1);
Receiver2Orientation = zeros(n_rirs,1);%-180 + 360*rand(n_rirs,1);

% Barycenter coordinates
BarycenterLoc = (Sensor2Loc+Sensor1Loc)/2;

% Source (Position in the room)
rng('shuffle');
SourceLoc(:,1) = uniform_with_n_random_margin(params.wall_src_tol,  vast.RoomSize(:,1));
rng('shuffle');
SourceLoc(:,2) = uniform_with_n_random_margin(params.wall_src_tol,  vast.RoomSize(:,2));
rng('shuffle');
SourceLoc(:,3) = uniform_with_n_random_margin(params.wall_src_tol,  ...
                            params.max_src_height.*ones(n_rirs,1));
% source to microphones distance
local_condition = @(bar_mics, srcs, room_size) ...
                        row_norm(bar_mics-srcs) < 15*params.min_mic_dist ...
                        | are_out_of_the_shoebox(srcs,room_size);

% rejection sampling to remove outliers
% far field assumption: at least 10 times more than the inter microphone distance
outl = local_condition(BarycenterLoc,SourceLoc, vast.RoomSize);
tic
while any(outl)
    n_outl = sum(outl);
    fprintf('Searching for %d new samples\n',n_outl);
    % drawn new samples
    rng('shuffle');
    SourceLoc(outl,1) = uniform_with_n_random_margin(params.wall_src_tol,vast.RoomSize(outl,1));
    rng('shuffle');
    SourceLoc(outl,2) = uniform_with_n_random_margin(params.wall_src_tol,vast.RoomSize(outl,2));
    rng('shuffle');
    SourceLoc(outl,3 ) = uniform_with_n_random_margin(params.wall_src_tol, ...
                                params.max_src_height.*ones(n_outl,1));
    % compute outliers
    outl = local_condition(BarycenterLoc, SourceLoc, vast.RoomSize);
end
fprintf(' done!\n  It seems you spent %1.3f seconds trying to put them all in the shoebox\n\n',toc)
if any(are_out_of_the_shoebox(SourceLoc, vast.RoomSize))
    error('Ouch! Sources are out of the Room')
end
vast.SourceLoc = SourceLoc;

% Relative coordinates to receivers
CartCoord2Sensor1 = SourceLoc - Sensor1Loc;
CartCoord2Sensor2 = SourceLoc - Sensor2Loc;

% Relative coordinates to barycenter
CartCoord2Barycenter = SourceLoc - BarycenterLoc;

% Converting to Azimuth elevation distance
[ValidSpherCoord1(:,3),ValidSpherCoord1(:,2),ValidSpherCoord1(:,1)] = cart2sph(CartCoord2Sensor1(:,1),CartCoord2Sensor1(:,2),CartCoord2Sensor1(:,3));
[ValidSpherCoord2(:,3),ValidSpherCoord2(:,2),ValidSpherCoord2(:,1)] = cart2sph(CartCoord2Sensor2(:,1),CartCoord2Sensor2(:,2),CartCoord2Sensor2(:,3));
[ValidSpherCoordBary(:,3),ValidSpherCoordBary(:,2),ValidSpherCoordBary(:,1)] = cart2sph(CartCoord2Barycenter(:,1),CartCoord2Barycenter(:,2),CartCoord2Barycenter(:,3));

% Converting to degree
ValidSpherCoord1(:,2) = ValidSpherCoord1(:,2)*180/pi;
ValidSpherCoord1(:,3) = ValidSpherCoord1(:,3)*180/pi;
ValidSpherCoord2(:,2) = ValidSpherCoord2(:,2)*180/pi;
ValidSpherCoord2(:,3) = ValidSpherCoord2(:,3)*180/pi;
ValidSpherCoordBary(:,2) = ValidSpherCoordBary(:,2)*180/pi;
ValidSpherCoordBary(:,3) = ValidSpherCoordBary(:,3)*180/pi;

% Positions
vast.SourcePosition     = SourceLoc;
vast.BarycenterPosition = BarycenterLoc;
vast.Sensor1Position    = Sensor1Loc;
vast.Sensor2Position    = Sensor2Loc;
% orientations
vast.Receiver1Orientation = Receiver1Orientation;
vast.Receiver2Orientation = Receiver2Orientation;
% azimuths
vast.AzimuthBarycenter = mod(ValidSpherCoordBary(:,3) - Receiver1Orientation(:,1) + 180,360) - 180;
vast.AzimuthSensor1    = mod(ValidSpherCoord1(:,3)    - Receiver1Orientation(:,1) + 180,360) - 180;
vast.AzimuthSensor2    = mod(ValidSpherCoord2(:,3)    - Receiver2Orientation(:,1) + 180,360) - 180;
% elevations
vast.ElevationBarycenter = ValidSpherCoord2(:,2);
vast.ElevationSensor1    = ValidSpherCoord1(:,2);
vast.ElevationSensor2    = ValidSpherCoord2(:,2);
% distances
vast.DistanceBarycenter = ValidSpherCoordBary(:,1);
vast.DistanceSensor1    = ValidSpherCoord1(:,1);
vast.DistanceSensor2    = ValidSpherCoord2(:,1);
vast.SensorDist         = SensorDist;

% Compute TDOA
iSourceLoc = make_image_floor(SourceLoc);

vast.tau1s1  = row_norm(Sensor1Loc - SourceLoc)/params.speed_of_sound;
vast.tau1s2  = row_norm(Sensor2Loc - SourceLoc)/params.speed_of_sound;
vast.tau2s1  = row_norm(Sensor1Loc - iSourceLoc)/params.speed_of_sound;
vast.tau2s2  = row_norm(Sensor2Loc - iSourceLoc)/params.speed_of_sound;
vast.delta1 = vast.tau2s1 - vast.tau1s1;
vast.delta2 = vast.tau2s2 - vast.tau1s2;
vast.ddiff  = vast.delta2 - vast.delta1;
vast.tdoa   = vast.tau1s2 - vast.tau1s1;
vast.itdoa  = vast.tau2s2 - vast.tau2s1;
vast.tdoe1  = vast.tau2s1 - vast.tau1s1;
vast.tdoe2  = vast.tau2s2 - vast.tau1s2;

out = vast.delta1 < 0 | vast.delta2 < 0;
if any(out)
    disp([vast.tau11(out), vast.tau21(out), vast.tdoa(out), ...
          vast.tau12(out), vast.tau22(out), vast.ddiff(out)])
    error('Ouch! It seems that your deltas are negative')
end

outliers = abs(vast.tdoa) < TDOA_thr ...
            | abs(vast.itdoa) < iTDOA_thr ...
            | abs(vast.ddiff) < TDOE_thr;
warning([num2str(sum(abs(vast.tdoa)  < TDOA_thr))   ' TDOA have been pruned'])
warning([num2str(sum(abs(vast.itdoa) < iTDOA_thr)) ' iTDOA have been pruned'])
warning([num2str(sum(abs(vast.ddiff) < TDOE_thr))  ' DDIFF have been pruned'])
warning([num2str(sum(outliers)) ' have been pruned'])
vast(outliers,:) = [];

end

function [x,y,z] = sph2cart(distance, azimuth, elevation)
    x = distance.*cos(elevation).*cos(azimuth); % x
    y = distance.*cos(elevation).*sin(azimuth); % y
    z = distance.*sin(elevation); % z
end

function [out] =  are_out_of_the_shoebox(points, shoebox_sizes)
    %% Parameter
    [Np,Dp] = size(points);
    [Ns,Ds] = size(shoebox_sizes);
    assert(all([Np, Dp] == [Ns,3]))
    assert(Dp == Ds)

    out = zeros(Ns,1);

    x = points(:,1);
    y = points(:,2);
    z = points(:,3);

    Corner = [0,0,0]; % the shoebox always start in (0,0,0) to (W,L,H)

    %% Determine
    out(x < Corner(1)) = 1;
    out(y < Corner(2)) = 1;
    out(z < Corner(3)) = 1;
    out(x > Corner(1) + shoebox_sizes(:,1)) = 1;
    out(y > Corner(2) + shoebox_sizes(:,2)) = 1;
    out(z > Corner(3) + shoebox_sizes(:,3)) = 1;
end

function is = make_image_floor(s)
    pi = [5,5,0];
    ni = [0,0,1];
    is = s +2*(sum((pi - s).*ni,2).*ni);
    assert(all(is(:,3) == -s(:,3)))
end

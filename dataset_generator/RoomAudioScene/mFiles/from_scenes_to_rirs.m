function rirs = from_scenes_to_rirs(vast, rirs, params, roomsim_params, verbose)
do_plot = false;

n_rirs = height(vast);

energy = @(x) sum(abs(x).^2);

%% ROOMSIM setup
fprintf('Loading ROOMSIM params ...\n')
% The mex file supplied works with MATLAB installed on 64-bit linux distro only. If on windows or mac, contact me. I can explain how to handle that case.
%roomsim load omnidirectional;   % loads the receiver model
%roomsim load omnidirectional;   % loads the source model

fprintf('done!\n\n')

n_freq_abs = length(roomsim_params.room.surface.frequency);

for sample = 1:n_rirs
    
    fprintf('Processing %d/%d\n',sample,n_rirs);
    
    % Compute impulse responses
    roomsim_params.room.dimension = vast.RoomSize(sample,:);
    % Receiver params
    sensor1 = vast.Sensor1Position(sample,:);
    sensor2 = vast.Sensor2Position(sample,:);
    assert(all(size(sensor1) == [1,3]));
    assert(all(size(sensor2) == [1,3]));
    roomsim_params.receiver(1).location = sensor1;
    roomsim_params.receiver(2).location = sensor2;
    % Source params
    source = vast.SourceLoc(sample,:);
    assert(all(size(source) == [1,3]));
    roomsim_params.source(1).location = source;
    % Walls profile
    wall_abs = ...
        [ vast.abs_wall_north(sample,:);
        vast.abs_wall_east(sample,:);
        vast.abs_wall_west(sample,:);
        vast.abs_wall_south(sample,:);
        vast.abs_wall_floor(sample,:);
        vast.abs_wall_ceiling(sample,:)];
    assert(all(size(wall_abs) == [6,n_freq_abs]));
    roomsim_params.room.surface.absorption = wall_abs;
    % Diffusion profiles
    wall_diff = [vast.diff_wall_north(sample,:);
        vast.diff_wall_east(sample,:);
        vast.diff_wall_west(sample,:);
        vast.diff_wall_south(sample,:);
        vast.diff_wall_floor(sample,:);
        vast.diff_wall_ceiling(sample,:)];
    assert(all(size(wall_diff) == [6,n_freq_abs]))
    roomsim_params.room.surface.diffusion = wall_diff;
    
    if params.do_real_rirs
        % Real Impulse Response generation
        curr_rirs = check_rirs(roomsim(roomsim_params));
        % Get Rt60 values
        RtParams.Fs = roomsim_params.options.fs;
        RtParams.FreqBin = roomsim_params.room.surface.frequency;
        [vast.FreqRT60(sample,:), vast.GlobalRT60(sample)] ...
            = Rir2Rt60(mean(curr_rirs,2),RtParams);
        
        rirs.RIR1(sample,:) = curr_rirs(:,1);
        rirs.RIR2(sample,:) = curr_rirs(:,2);
    end
    
    % direct path
    dp_roomsim_params = roomsim_params;
    dp_roomsim_params.options.reflectionorder = [ 0 0 0 ]; % maximum specular reflection order (x,y,z)
    rirs_dp = check_rirs(roomsim(dp_roomsim_params));
    % early reflection
    er_roomsim_params = roomsim_params;
    er_roomsim_params.options.reflectionorder = [ 1 1 1 ]; % maximum specular reflection order (x,y,z)
    rirs_dp_er = check_rirs(roomsim(er_roomsim_params));
    % late reverberation
    rirs_lr = curr_rirs - rirs_dp_er;
    % early reflection
    rirs_er = rirs_dp_er - rirs_dp; 
    % compute Direct-To-Reverberant Ratio
    DRR = mean(20*log10(energy(rirs_dp)./(energy(rirs_lr)+energy(rirs_er))));
    % compute Direct-To-Early-Echo Ratio
    DER = mean(20*log10(energy(rirs_dp)./energy(rirs_er)));
    
    assert(all(all(abs(curr_rirs - (rirs_dp + rirs_er + rirs_lr))<1e-7)))
    
    if do_plot
        subplot(311)
        plot(curr_rirs(:,1));
        subplot(312)
        plot(rirs_dp(:,1));
        hold on
        plot(rirs_er(:,1));
        plot(rirs_lr(:,1));
        subplot(313)
        plot(curr_rirs(:,1) - rirs_dp(:,1) - rirs_er(:,1) - rirs_lr(:,1));
    end
    
    vast.DRR(sample,1) = DRR;
    vast.DER(sample,1) = DER;
    rirs.id(sample,1) = sample;
    
    % % Generating synthetic RIR (no diffusion, lower reflexion order)
    if params.do_syth_rirs
        % first echo
        ideal_roomsim_params = roomsim_params;
        ideal_roomsim_params.options.reflectionorder = [ 1 1 1 ]; % maximum specular reflection order (x,y,z)
        ideal_roomsim_params.options.simulatediffuse = false;    % change this to false if user does not want to simulate diffuse reflections
        ideal_roomsim_params.room.surface.absorption([1,2,3,4,6],:) = 1; % make only the direct path and first echoes
        curr_rirs = check_rirs(roomsim(ideal_roomsim_params));
        rirs.RIRSynth1(sample,:) = curr_rirs(:,1);
        rirs.RIRSynth2(sample,:) = curr_rirs(:,2);
    end
    
end
end

function rirs = check_rirs(rirs)
if iscell(rirs)
    rirs = cell2mat(rirs);
    rirs = single(rirs);
end
if any(isnan(rirs))
    error('Wrong Impulse Response')
end

end

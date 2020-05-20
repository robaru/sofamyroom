function [vast_params, roomsim_params] = load_parameters(n_experience)

	if ~ischar(n_experience)
		switch n_experience
			case 0
				vast_params.name = 'VastPicnic';
				% test case
			    %% MIRAGE HARDCODING
				%  [mic2mic_elevation fixed, mic2surf_height fixed]
    			% - task0 : [0, 0]
    			% - task1 : [0, 1]
    			% - task2 : [1, 1]
				vast_params.do_planar_array = 1;
            	vast_params.do_fix_mic2surf_height = 1;
				% toggle debug mode
			    vast_params.debug = 1;
			    % Signal parameters parameters
		        vast_params.Fs = 48000;
    			vast_params.abs_coef_freq_bin = [125,250,500,1000,2000,4000,8000]; %Rimettere 8000
    			vast_params.max_rir_time = 1.50;
			    vast_params.datatype = "single";
			    vast_params.max_mic_dist   = 0.20; % [meters]
			    vast_params.max_mic_height = 0.20; % [meters]
			    vast_params.min_mic_dist   = 0.03; % [meters]
			    vast_params.wall_mic_tol   = 0.10 + 2*vast_params.max_mic_height; % [meters]
			    vast_params.min_mic_height = 0.03; % [meters]
			    vast_params.wall_src_tol   = 0.50; % [meters]
			    vast_params.max_src_height = 2.00; % [meters]
			    vast_params.min_width   = 3;
			    vast_params.max_width   = 5;
			    vast_params.min_length  = 3;
			    vast_params.max_length  = 7;
			    vast_params.min_height  = 2;
			    vast_params.max_height  = 4;
			    vast_params.do_syth_rirs = true;
			    vast_params.do_real_rirs = true;
			    vast_params.FreqBin    = [125,250,500,1000,2000,4000,8000];
			    vast_params.RIRMaxTime = 1.25;
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                vast_params.mics_max_elevation = 0;
                vast_params.mics_min_elevation = 0; 
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			    %% Creating parameter variable 'roomsim_params'
			    %    absorption profile
			    vast_params.use_realistic_material_profiles = false;
			    vast_params.path_to_absorption_profiles = '';
			    vast_params.do_freq_dep_abs_profiles = false;
			    vast_params.min_picnic_abs = 0;
			    vast_params.max_picnic_abs = 0.5;
			    vast_params.min_walls_abs  = 0.5;
			    vast_params.max_walls_abs  = 1;
                vast_params.use_multiple_diffusion = 0;
			    vast_params.diffuse = [0.0109,0.0075,0.0511,0.0668,0.1472,0.2878,0.002];
			    % loading basic parameters of the room
			    %    room settings
			    roomsim_params.room.humidity               = 0.42;         % relative humidity (0,...,1)
			    roomsim_params.room.temperature            = 20;           % room temperature (celsius)
			    vast_params.speed_of_sound = (331.3 + 0.606*roomsim_params.room.temperature);
			    roomsim_params.room.surface.frequency      = vast_params.abs_coef_freq_bin;
        	    %    simulation options
			    roomsim_params.options.fs                  = vast_params.Fs;
			    roomsim_params.options.responseduration    = vast_params.max_rir_time;
			    roomsim_params.options.bandsperoctave      = 1;            % simulation frequency accuracy (1, 2, 3, or 4 bands/octave)
			    roomsim_params.options.referencefrequency  = 125;          % reference frequency for frequency octaves
			    roomsim_params.options.airabsorption       = true;         % apply air absorption?
			    roomsim_params.options.distanceattenuation = true;         % apply distance attenuation?
			    roomsim_params.options.subsampleaccuracy   = false;        % apply subsample accuracy?
			    roomsim_params.options.highpasscutoff      = 0;            % 3dB frequency of high-pass filter (0=none)
			    roomsim_params.options.verbose             = false;        % print status messages?
			    %    specular reflections simulation options
			    roomsim_params.options.simulatespecular    = true;         % simulate specular reflections?
			    roomsim_params.options.reflectionorder     = [ 10 10 10 ]; % maximum specular reflection order (x,y,z)
			    %    diffuse reflections simulation options
			    roomsim_params.options.numberofrays        = 2000;         % number of rays in simulation (20*K^2)
			    roomsim_params.options.diffusetimestep     = 0.010;        % time resolution in diffuse energy histogram (seconds)
			    roomsim_params.options.rayenergyfloordB    = -80;          % ray energy threshold (dB, with respect to initial energy)
			    roomsim_params.options.uncorrelatednoise   = true;         % use uncorrelated poisson arrivals for binaural impulse responses?
			    roomsim_params.options.simulatediffuse     = true;         % change this to false if user does not want to simulate diffuse reflections
                % output options
                roomsim_params.options.outputname           = 'output';
                roomsim_params.options.saveaswav            = true;
                % appending more parameters in variable 'roomsim_params'
			    roomsim_params.receiver(1).description     = 'SOFA ../sofamyroom/data/SOFA/MIT_KEMAR_large_pinna.sofa resampling=true';
			    roomsim_params.receiver(2).description     = 'SOFA ../sofamyroom/data/SOFA/MIT_KEMAR_large_pinna.sofa resampling=true';
			    roomsim_params.receiver(1).orientation     = [ 0, 0, 0 ];
			    roomsim_params.receiver(2).orientation     = [ 0, 0, 0 ];
			    roomsim_params.source(1).orientation       = [ 0, 0, 0 ];
			    roomsim_params.source(1).description       = 'omnidirectional';
			    roomsim_params.room.surface.diffusion      = repmat(vast_params.diffuse,6,1);  % diffusion profiles for all 6 surfaces % Diffuse

			case 1
				vast_params.name = 'VastPicnic_fixRoom';
				% test case
			    %% MIRAGE HARDCODING
				%  [mic2mic_elevation fixed, mic2surf_height fixed]
    			% - task0 : [0, 0]
    			% - task1 : [0, 1]
    			% - task2 : [1, 1]
				vast_params.do_planar_array = 1;
            	vast_params.do_fix_mic2surf_height = 1;
				% toggle debug mode
			    vast_params.debug = 1;
			    % Signal parameters parameters
		        vast_params.Fs = 48000;
    			vast_params.abs_coef_freq_bin = [125,250,500,1000,2000,4000,8000]; %Rimettere 8000
    			vast_params.max_rir_time = 1.50;
			    vast_params.datatype = "single";
			    vast_params.max_mic_dist   = 0.20; % [meters]
			    vast_params.max_mic_height = 0.20; % [meters]
			    vast_params.min_mic_dist   = 0.03; % [meters]
			    vast_params.wall_mic_tol   = 0.10 + 2*vast_params.max_mic_height; % [meters]
			    vast_params.min_mic_height = 0.03; % [meters]
			    vast_params.wall_src_tol   = 0.50; % [meters]
			    vast_params.max_src_height = 2.00; % [meters]
			    vast_params.min_width   = 3;
			    vast_params.max_width   = 3;
			    vast_params.min_length  = 5;
			    vast_params.max_length  = 5;
			    vast_params.min_height  = 2;
			    vast_params.max_height  = 2;
			    vast_params.do_syth_rirs = true;
			    vast_params.do_real_rirs = true;
			    vast_params.FreqBin    = [125,250,500,1000,2000,4000,8000];
			    vast_params.RIRMaxTime = 1.25;
			    %% Creating parameter variable 'roomsim_params'
			    %    absorption profile
			    vast_params.use_realistic_material_profiles = false;
			    vast_params.path_to_absorption_profiles = '';
			    vast_params.do_freq_dep_abs_profiles = false;
			    vast_params.min_picnic_abs = 0.2;
			    vast_params.max_picnic_abs = 0.2;
			    vast_params.min_walls_abs  = 0.8;
			    vast_params.max_walls_abs  = 0.8;
			    vast_params.diffuse = [0.0109,0.0075,0.0511,0.0668,0.1472,0.2878,0.002]; %aggiunto uno
			    % loading basic parameters of the room
			    %    room settings
			    roomsim_params.room.humidity               = 0.42;         % relative humidity (0,...,1)
			    roomsim_params.room.temperature            = 20;           % room temperature (celsius)
			    vast_params.speed_of_sound = (331.3 + 0.606*roomsim_params.room.temperature);
			    roomsim_params.room.surface.frequency      = vast_params.abs_coef_freq_bin;
        	    %    simulation options
			    roomsim_params.options.fs                  = vast_params.Fs;
			    roomsim_params.options.responseduration    = vast_params.max_rir_time;
			    roomsim_params.options.bandsperoctave      = 1;            % simulation frequency accuracy (1, 2, 3, or 4 bands/octave)
			    roomsim_params.options.referencefrequency  = 125;          % reference frequency for frequency octaves
			    roomsim_params.options.airabsorption       = true;         % apply air absorption?
			    roomsim_params.options.distanceattenuation = true;         % apply distance attenuation?
			    roomsim_params.options.subsampleaccuracy   = false;        % apply subsample accuracy?
			    roomsim_params.options.highpasscutoff      = 0;            % 3dB frequency of high-pass filter (0=none)
			    roomsim_params.options.verbose             = false;        % print status messages?
			    %    specular reflections simulation options
			    roomsim_params.options.simulatespecular    = true;         % simulate specular reflections?
			    roomsim_params.options.reflectionorder     = [ 10 10 10 ]; % maximum specular reflection order (x,y,z)
			    %    diffuse reflections simulation options
			    roomsim_params.options.numberofrays        = 2000;         % number of rays in simulation (20*K^2)
			    roomsim_params.options.diffusetimestep     = 0.010;        % time resolution in diffuse energy histogram (seconds)
			    roomsim_params.options.rayenergyfloordB    = -80;          % ray energy threshold (dB, with respect to initial energy)
			    roomsim_params.options.uncorrelatednoise   = true;         % use uncorrelated poisson arrivals for binaural impulse responses?
			    roomsim_params.options.simulatediffuse     = true;         % change this to false if user does not want to simulate diffuse reflections
			    % output options
                roomsim_params.options.outputname           = 'output';
                roomsim_params.options.saveaswav            = true;
                % appending more parameters in variable 'roomsim_params'
			    roomsim_params.receiver(1).description     = 'SOFA MIT_KEMAR_large_pinna.sofa resampling=true';
			    roomsim_params.receiver(2).description     = 'SOFA MIT_KEMAR_large_pinna.sofa resampling=true';
			    roomsim_params.receiver(1).orientation     = [ 0, 0, 0 ];
			    roomsim_params.receiver(2).orientation     = [ 0, 0, 0 ];
			    roomsim_params.source(1).orientation       = [ 0, 0, 0 ];
			    roomsim_params.source(1).description       = 'omnidirectional';
			    roomsim_params.room.surface.diffusion      = repmat(vast_params.diffuse,6,1);  % diffusion profiles for all 6 surfaces % Diffuse
			otherwise
				fprintf('\nTODO to set a defoult test case\n.')
		end
	else
		f = load(['./Data/' n_experience '.mat']);
		vast_params  = f.vast_params;
		roomsim_params = f.roomsim_params;
	end
end
function [vast_params, sofamyroom_params] = load_parameters(n_experience)
	if ~ischar(n_experience)
		switch n_experience
			case 0
				vast_params.name = 'VASTtest';
				
				vast_params.do_planar_array = 1;
            	vast_params.do_fix_mic2surf_height = 1;
				% toggle debug mode
			    vast_params.debug = 1;
			    % Signal parameters parameters
		        vast_params.Fs = 48000;
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
			    vast_params.freq_bin    = [125,250,500,1000,2000,4000,8000];
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                vast_params.mics_max_elevation = 0;
                vast_params.mics_min_elevation = 0; 
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			    %% Creating parameter variable 'sofamyroom_params'
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
			    sofamyroom_params.room.humidity               = 0.42;         % relative humidity (0,...,1)
			    sofamyroom_params.room.temperature            = 20;           % room temperature (celsius)
			    vast_params.speed_of_sound = (331.3 + 0.606*sofamyroom_params.room.temperature);
			    sofamyroom_params.room.surface.frequency      = vast_params.abs_coef_freq_bin;
        	    %    simulation options
			    sofamyroom_params.options.fs                  = vast_params.Fs;
			    sofamyroom_params.options.responseduration    = vast_params.max_rir_time;
			    sofamyroom_params.options.bandsperoctave      = 1;            % simulation frequency accuracy (1, 2, 3, or 4 bands/octave)
			    sofamyroom_params.options.referencefrequency  = 125;          % reference frequency for frequency octaves
			    sofamyroom_params.options.airabsorption       = true;         % apply air absorption?
			    sofamyroom_params.options.distanceattenuation = true;         % apply distance attenuation?
			    sofamyroom_params.options.subsampleaccuracy   = false;        % apply subsample accuracy?
			    sofamyroom_params.options.highpasscutoff      = 0;            % 3dB frequency of high-pass filter (0=none)
			    sofamyroom_params.options.verbose             = false;        % print status messages?
			    %    specular reflections simulation options
			    sofamyroom_params.options.simulatespecular    = true;         % simulate specular reflections?
			    sofamyroom_params.options.reflectionorder     = [ 10 10 10 ]; % maximum specular reflection order (x,y,z)
			    %    diffuse reflections simulation options
			    sofamyroom_params.options.numberofrays        = 2000;         % number of rays in simulation (20*K^2)
			    sofamyroom_params.options.diffusetimestep     = 0.010;        % time resolution in diffuse energy histogram (seconds)
			    sofamyroom_params.options.rayenergyfloordB    = -80;          % ray energy threshold (dB, with respect to initial energy)
			    sofamyroom_params.options.uncorrelatednoise   = true;         % use uncorrelated poisson arrivals for binaural impulse responses?
			    sofamyroom_params.options.simulatediffuse     = true;         % change this to false if user does not want to simulate diffuse reflections
                % output options
                sofamyroom_params.options.outputname           = 'output';
                sofamyroom_params.options.saveaswav            = true;
                % appending more parameters in variable 'sofamyroom_params'
			    sofamyroom_params.receiver(1).description     = 'SOFA ../sofamyroom/data/SOFA/MIT_KEMAR_large_pinna.sofa resampling=true';
			    sofamyroom_params.receiver(1).orientation     = [ 0, 0, 0 ];
			    sofamyroom_params.source(1).orientation       = [ 0, 0, 0 ];
			    sofamyroom_params.source(1).description       = 'omnidirectional';
			    sofamyroom_params.room.surface.diffusion      = repmat(vast_params.diffuse,6,1);  % diffusion profiles for all 6 surfaces % Diffuse
			otherwise
				fprintf('\nTODO to set a default test case\n.')
		end
	else
		f = load(['./Data/' n_experience '.mat']);
		vast_params  = f.vast_params;
		sofamyroom_params = f.sofamyroom_params;
	end
end
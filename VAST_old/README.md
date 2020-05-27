# The Virtual Acoustic Space Traveler (VAST) Project

### Machine Hearing by Simulated Learning

This tool has been adapted to be used with SofaMyRoom project.

Please refer to http://thevastproject.inria.fr for its full documentation.

#### References

[VAST : The Virtual Acoustic Space Traveler Dataset](https://hal.archives-ouvertes.fr/hal-01416508)

Clément Gaultier, Saurabh Kataria, Antoine Deleforge, *International Conference on Latent Variable Analysis and Signal Separation (LVA/ICA)*, **Feb 2017**, Grenoble, France

[Hearing in a shoe-box : binaural source position and wall absorption estimation using virtually supervised learning](https://hal.inria.fr/hal-01372435v2)

Saurabh Kataria, Clément Gaultier, Antoine Deleforge *2017 IEEE International Conference on Acoustics, Speech and Signal Processing (ICASSP)*, **Mar 2017**, New-Orleans, United States

## SAMPLE EXAMPLE

1. Add the SofaMyRoom MEX's path `addpath('../sofamyroom.mex')`
2. Run main.m 

A mat file with the 



## VAST PARAMETERS


	vast_params.name = 'VASTtest';
	vast_params.do_planar_array = 1;
	vast_params.do_fix_mic2surf_height = 1;
	% toggle debug mode
	vast_params.debug = 1;
	% Signal parameters parameters
	vast_params.Fs = 48000;
	vast_params.abs_coef_freq_bin = [125,250,500,1000,2000,4000,8000];
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
	vast_params.speed_of_sound = (331.3 + 0.606*sofamyroom_params.room.temperature);
function [annotations, rirs] = main(n_rirs, path_to_config_file)
    if nargin == 0
       n_rirs = 2;
       path_to_config_file = 0;
    end

    %% ARGUMENT FOR THE COMPILED VERSION
    if (isdeployed)
        % if it is compiled read the argument as...
        n_rirs = eval(n_rirs);
        JobID = [getenv('OAR_JOB_ID')];
    else
        % if it is running in a matlab session
        JobID = num2str(randi([0 1000],1));
        verbose = 1;
    end

    %% 0. IMPORT VARIABLES from CONFIG FILE
    % all the parameters should be written/stored in a .mat
    % configuration files.
    fprintf('Reading config file... ')
    [params, roomsim_params] = load_parameters(path_to_config_file);
    fprintf(' done!\n\n')

    % SUFFIX FOR FILENAME
    if ischar(path_to_config_file)
        save_as = path_to_config_file;
    else
        save_as = num2str(path_to_config_file);
    end
    suffix = ['_p-' save_as];
    % Paths and folders
    result_dir = ['./Results/' save_as '/'];
    if ~isdir(result_dir); mkdir(result_dir); end

    save_as = [result_dir params.name '_' JobID suffix];
    save_as(isspace(save_as)) = [];

    %% 1. initialize le dataset data-structure
    fprintf('Initialize the output files... ')
    [annotations, rirs] = vasttable_init(n_rirs, params);
    fprintf(' done!\n\n')

    %% 2. From input parameters to auditory scenes
    fprintf('Generating all the scene from the input parameters ...')
    annotations = from_params_to_scene(annotations, params);
    fprintf(' done.\n')
    save([save_as '_note.mat'],'annotations','-v7.3');

    %% 3. From auditory scenes to room impulse responses
    fprintf('Generating RIRs form all the scenes ... ')
    rirs = from_scenes_to_rirs(annotations, rirs, params, roomsim_params);
    fprintf(' done.\n')

    %% 4. save results in the result directory
    fprintf('Saving results as:\n\t%s ...',save_as);
    save([save_as '_rirs.mat'],'rirs','-v7.3');
    disp('Done and Finished!!!\n');

end

function [vast, rirs] = vasttable_init(n_rirs, params)
    vast = table();

    %% General parameters
    % indipendet from the number of rirs
    % Signal Processing properties
    n_smp = params.max_rir_time*params.Fs;
    n_freq_abs = length(params.abs_coef_freq_bin);
    %% RIRs
    % Pre-allocate memory for stroing room impulse responses.
    % The maximum lenght of the RIR should be define.
    % ATTENTION: Check the RT60 to choose the proper lenght
    % it is possible to choose between single and double precision
    rirs = table();
    if params.do_real_rirs
        rirs.RIR1 = zeros(n_rirs,n_smp);
        rirs.RIR2 = zeros(n_rirs,n_smp);
    end
    if params.do_syth_rirs
        rirs.RIRSynth1 = zeros(n_rirs,n_smp);
        rirs.RIRSynth2 = zeros(n_rirs,n_smp);
    end

    %% Source
    % geometry
    vast.SourceLoc      = zeros(n_rirs,3); % 3xN (x,y,z)

    %% Microphones / Receiver / Array
    % geometry
    vast.Sensor1Position   = zeros(n_rirs,3); % 3xN (x,y,z)
    vast.Sensor2Position   = zeros(n_rirs,3); % 3xN (x,y,z)
    vast.BarycenterPosition  = zeros(n_rirs,3); % 3xN (x,y,z)
    vast.Receiver1Orientation = zeros(n_rirs,1);%-180 + 360*rand(n_rirs,1);
    vast.Receiver2Orientation = zeros(n_rirs,1);%-180 + 360*rand(n_rirs,1);

    %% Rooms
    % geometry
    vast.RoomSize = zeros(n_rirs,3);
    % Rooms Sound properties
    vast.FreqRT60   = zeros(n_rirs,7); %originale 6
    vast.GlobalRT60 = zeros(n_rirs,1);
    % Walls Absorption coefficient
    vast.abs_wall_east = zeros(n_rirs, n_freq_abs);
    vast.abs_wall_west = zeros(n_rirs, n_freq_abs);
    vast.abs_wall_north = zeros(n_rirs, n_freq_abs);
    vast.abs_wall_south = zeros(n_rirs, n_freq_abs);
    vast.abs_wall_floor = zeros(n_rirs, n_freq_abs);
    vast.abs_wall_ceiling = zeros(n_rirs, n_freq_abs);
    % Diffusion
    vast.diff_wall_east = zeros(n_rirs, n_freq_abs);
    vast.diff_wall_west = zeros(n_rirs, n_freq_abs);
    vast.diff_wall_north = zeros(n_rirs, n_freq_abs);
    vast.diff_wall_south = zeros(n_rirs, n_freq_abs);
    vast.diff_wall_floor = zeros(n_rirs, n_freq_abs);
    vast.diff_wall_ceiling = zeros(n_rirs, n_freq_abs);

    %% TDOA analysis variables
    vast.tau1s1  = zeros(n_rirs,1); % TOA at mic1
    vast.tau2s1  = zeros(n_rirs,1); % TOA at mic2
    vast.tau1s2  = zeros(n_rirs,1); % TOA of the 1st reflect at mic1
    vast.tau2s2  = zeros(n_rirs,1); % TOA of the 1st reflect at mic2
    vast.delta1 = zeros(n_rirs,1); % TDOA of direct path and 1st echo at mic1
    vast.delta2 = zeros(n_rirs,1); % TDOA of direct path and 1st echo at mic2
    vast.tdoa   = zeros(n_rirs,1); % TDOA classic and easypeasy
    vast.itdoa  = zeros(n_rirs,1); % TDOA at the image microphones
    vast.ddiff  = zeros(n_rirs,1); % delta2-delta1
    vast.tdoe1  = zeros(n_rirs,1); %
    vast.tdoe2  = zeros(n_rirs,1); %
end

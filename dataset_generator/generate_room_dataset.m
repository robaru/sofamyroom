function dataset = generate_room_dataset(configuration)
    for receiver=1:length(configuration.receivers)
        config = prepare_config(configuration, receiver);
        [rt60, ~]=estimateRT60(config, 'Sabine');
        disp(['Avg. RT60 (Sabine): ',num2str(mean(rt60))]);
        RIRS = RIR_generation(config); 
        files_list = get_files(configuration.timit_path);
        temp_dataset = generate_dataset(RIRS, files_list, config, configuration.dataset_freq, configuration.voice_length);
        dataset(receiver).data = temp_dataset;
        clear temp_dataset;
    end
end

function dataset = generate_dataset(RIRS, files_list, config, fs, voice_length)
    combinations = height(RIRS)*config.max_voices;
    audio_rows = cell(config.reps_per_azimuth*combinations,1);
    audio_idx = 1;
    audio_row = cell(config.max_voices,1);
    audio_row_idx = [];
    for v=1:config.max_voices
        check = false;
        while not(check)
            [temp_voice, temp_voice_fs] = audioread(strcat(files_list(audio_idx).folder,'/',files_list(audio_idx).name));
            %pick samples form the half to avoid low volume at the
            %beginning, so minimum length is 2 times bigger
            if size(temp_voice,1) < 2*temp_voice_fs*voice_length
                audio_idx = audio_idx + 1;
                if audio_idx > size(files_list,1)
                    audio_idx = 1;
                end
            else
                check = true;
            end
         end
         audio_row{v,1} = strcat(files_list(audio_idx).folder,'/',files_list(audio_idx).name);
         audio_row_idx = [audio_row_idx; audio_idx];
         audio_idx = audio_idx+1;
    end
    for rep = 1:config.reps_per_azimuth
        for row=1:combinations
            voices = 1+mod(row-1, config.max_voices);
            audio_rows{(rep-1)*combinations+row,1} = audio_row(1:voices);
            audio_rows_idx{(rep-1)*combinations+row, 1} = audio_row_idx(1:voices);
        end
    end
    samples_length = floor(voice_length*config.options.fs);
    max_voices = config.max_voices;
    
    for rep=1:config.reps_per_azimuth
        parfor row=1+(rep-1)*combinations:rep*combinations
            disp(num2str(row));
            rirs_candidates = RIRS;
            audio_row = audio_rows{row,:};
            audio_row_idxs = audio_rows_idx{row,:};
            voices = length(audio_row);
            % select azimuths
            rirs_curr = cell(voices,1);
            audio_curr = cell(voices,1);
            curr_rir = 1+floor(((row-(rep-1)*combinations)-1)/max_voices);
            rirs_curr{1,1} = rirs_candidates(curr_rir,:).RIR;
            azimuth_curr = rirs_candidates(curr_rir,:).Azimuth;
            [temp_audio, temp_audio_fs] = audioread(audio_row{1});
            dataset(row).voices = cell(max_voices,1);
            dataset(row).voices{1} = audio_row{1};
            dataset(row).voices_idx = audio_row_idxs;
            if temp_audio_fs ~= config.options.fs
                [p,q] = rat(config.options.fs/temp_audio_fs);
                temp_audio = resample(temp_audio, p, q);
            end
            audio_curr{1,1} = temp_audio;
            to_rem = find(abs([rirs_candidates(:,:).Azimuth]-azimuth_curr(1)) < 2*config.delta_angle);
            rirs_candidates(to_rem,:) = [];
            %disp(mat2str(azimuth_curr));
            if voices > 1
                for voice = 2:voices
                    random_rir = randi(size(rirs_candidates,1));
                    rirs_curr{voice,1} = rirs_candidates(random_rir,:).RIR;
                    azimuth_curr = [azimuth_curr; rirs_candidates(random_rir,:).Azimuth];
                    [temp_audio, temp_audio_fs] = audioread(audio_row{voice});
                    dataset(row).voices{voice} = audio_row{voice};
                    %dataset(row).voices_idx = [dataset(row).voices_idx; audio_row_idx(voice)];
                    if temp_audio_fs ~= config.options.fs
                        [p,q] = rat(config.options.fs/temp_audio_fs);
                        temp_audio = resample(temp_audio, p, q);
                    end
                    audio_curr{voice,1} = temp_audio;
                    to_rem = find(abs([rirs_candidates(:,:).Azimuth]-azimuth_curr(voice)) < 2*config.delta_angle);
                    rirs_candidates(to_rem,:) = [];
                end
            end
            % generate audio signal
            audio_conv = rir_convolution(rirs_curr, audio_curr, samples_length);
            % resample
            [p,q] = rat(fs/config.options.fs);
            audio_conv = resample(audio_conv, p, q);

            % store everything
            dataset(row).audio = audio_conv;
            dataset(row).azimuth = round(azimuth_curr);
            dataset(row).rec_position = config.receiver(1).location; 
        end
    end
end

function files_list = get_files(folder)
    files_list = dir(strcat(folder, '/TEST/**/*.WAV'));
end

function out = rir_convolution(RIRS, audios, samples_length)
    audio_tracks = cell(length(audios),1);
    min_audio_length = samples_length;
    for voice=1:length(audios)
        RIR = squeeze(RIRS{voice,1});
        audio = audios{voice,1};
        audio = audio ./ max(audio);
        audio_tracks{voice,1} = [conv(audio, RIR(:,1), 'full'), conv(audio, RIR(:,2), 'full')];
    end
    out = zeros(min_audio_length,2);
    for a=1:length(audio_tracks)
        audio_track = audio_tracks{a,1};
        audio_half = floor(size(audio_track,1)/2);
        for s=1:min_audio_length
            out(s,1) = out(s,1)+audio_track(audio_half+s-1,1);
            out(s,2) = out(s,2)+audio_track(audio_half+s-1,2);
        end
    end
%     for s=1:min_audio_length
%         for a=1:length(audio_tracks)
%             out(s,1) = out(s,1) + audio_tracks{a,1}(s,1);
%             out(s,2) = out(s,2) + audio_tracks{a,1}(s,2);
%         end
%     end
end

function roomsim_config = prepare_config(configuration, receiver_number)
    %% create the data structure to pass to the ROOMSIM 
    roomsim_config.max_voices = configuration.max_voices;
    roomsim_config.delta_angle = configuration.delta_angle;
    roomsim_config.reps_per_azimuth = configuration.reps_per_azimuth;
    roomsim_config.room.dimension = configuration.room_size;
    roomsim_config.room.humidity               = configuration.humidity;         % relative humidity (0,...,1)
    roomsim_config.room.temperature            = configuration.temp;           % room temperature (celsius)
    roomsim_config.room.surface.frequency = configuration.freq_buckets;
    roomsim_config.room.surface.absorption = [configuration.abs_walls; configuration.abs_walls; configuration.abs_walls; configuration.abs_walls; configuration.abs_floor; configuration.abs_ceil]; 
    roomsim_config.room.surface.diffusion =  [configuration.scat_walls; configuration.scat_walls; configuration.scat_walls; configuration.scat_walls; configuration.scat_floor; configuration.scat_ceil]; 
    roomsim_config.options.responseduration = 2;
    roomsim_config.options.fs = 44100;
    roomsim_config.options.bandsperoctave      = 1;            % simulation frequency accuracy (1, 2, 3, or 4 bands/octave)
    roomsim_config.options.referencefrequency  = 125;          % reference frequency for frequency octaves
    roomsim_config.options.airabsorption       = true;         % apply air absorption?
    roomsim_config.options.distanceattenuation = true;         % apply distance attenuation?
    roomsim_config.options.subsampleaccuracy   = true;        % apply subsample accuracy?
    roomsim_config.options.highpasscutoff      = 0;            % 3dB frequency of high-pass filter (0=none)
    roomsim_config.options.verbose             = false;        % print status messages?
    roomsim_config.options.simulatespecular    = true;         % simulate specular reflections?
    roomsim_config.options.reflectionorder     = [ 3 3 3 ]; % maximum specular reflection order (x,y,z)
    roomsim_config.options.numberofrays        = 2000;         % number of rays in simulation (20*K^2)
    roomsim_config.options.diffusetimestep     = 0.010;        % time resolution in diffuse energy histogram (seconds)
    roomsim_config.options.rayenergyfloordB    = -80;          % ray energy threshold (dB, with respect to initial energy)
    roomsim_config.options.uncorrelatednoise   = true;         % use uncorrelated poisson arrivals for binaural impulse responses?
    roomsim_config.options.simulatediffuse     = true;         % change this to false if user does not want to simulate diffuse reflections
    receiver.X = configuration.receivers(receiver_number).position(1) * configuration.room_size(1);
    receiver.Y = configuration.receivers(receiver_number).position(2) * configuration.room_size(2);
    receiver.Z = configuration.receivers(receiver_number).position(3) * configuration.room_size(3);
    roomsim_config.sources = generate_sources_from_point(receiver, configuration.distance, configuration.min_angle, configuration.max_angle, configuration.delta_angle, 'cardioid');
    roomsim_config.receiver(1).location         = [ receiver.X, receiver.Y, receiver.Z ];         % location of receiver (x,y,z; meters)
    roomsim_config.receiver(1).orientation      = [ 90, 0, 0 ];           % orientation of receiver (yaw,pitch,roll; degrees)
    roomsim_config.receiver(1).description      = 'MIT ./datasets/HRTF/KEMARsmall.hrtf';  % receiver type
end
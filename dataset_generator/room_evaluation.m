function room_evaluation(IDX, ABS_WALLS, ABS_CEIL, ABS_FLOOR, SCAT_WALLS, SCAT_CEIL, SCAT_FLOOR)
%% SIMULATED ROOM EVALUATION

%% ----------  Description  ----------
%  ROOM SIZE: 7.1 x 5.1 x 3 m
%  MATERIALS: SEE https://www.researchgate.net/figure/Sound-absorption-coefficients-of-different-materials-used-in-the-Kocatepe-Mosque_tbl1_233083108
%  TEMPERATURE = 20°C
%  HUMIDITY = 30%

%% ----------  Implementation -----------

%% ----------  Room Properties ----------
% Setup parameters: room size
ROOM_SIZE(1).dimensions = [7.1 5.1 3];
% Setup parameters: frequency buckets for abs & scattering coeffs.
FREQ_BUCKETS = [125 250 500 1000 2000 4000];
% Setup parameters: temperature and humidity
TEMPERATURE = 20;
HUMIDITY = 0.3;
%% ----------- Sources Positions -----------
% some sample values, normalized from 0 to 1 for each dimension!!!
RECEIVERS(1).position = [0.211 0.294 0.583];
RECEIVERS(2).position = [0.5 0.294 0.583];
RECEIVERS(3).position = [0.570 0.598 0.583];

DISTANCE =  1.4;
MIN_ANGLE = -180;
MAX_ANGLE = 175;
DELTA_ANGLE = 5;
%% ---------- Dataset Settings ----------
REPETITIONS_PER_AZIMUTH = 5;
TIMIT_PATH = './datasets/TIMIT';
DATASET_FREQ = 16000;
MAX_VOICES = 3;
VOICE_LENGTH = 2.3; % in s
%% ----------  Models  Settings  ----------
USE_MAY2011 = true;
USE_DIETZ2011 = true;
USE_MAY2017 = true;
USE_HMM = true;
disp('[room_evaluation] --- Loading models...');
models = load_models(USE_MAY2011, USE_DIETZ2011, USE_MAY2017, USE_HMM);
%% ---------- Dataset Generation ----------
for room_size = 1:length(ROOM_SIZE)
    disp(['[room_evaluation] --- ROOM ', num2str(room_size),': ',mat2str(ROOM_SIZE(room_size).dimensions)]);
    % generate dataset for the room
    configuration.room_size = ROOM_SIZE(room_size).dimensions;
    configuration.freq_buckets = FREQ_BUCKETS;
    configuration.abs_walls = ABS_WALLS;
    configuration.abs_ceil = ABS_CEIL;
    configuration.abs_floor = ABS_FLOOR;
    configuration.scat_walls = SCAT_WALLS;
    configuration.scat_ceil = SCAT_CEIL;
    configuration.scat_floor = SCAT_FLOOR;
    configuration.temp = TEMPERATURE;
    configuration.humidity = HUMIDITY;
    configuration.receivers = RECEIVERS;
    configuration.distance = DISTANCE;
    configuration.min_angle = MIN_ANGLE;
    configuration.max_angle = MAX_ANGLE;
    configuration.delta_angle = DELTA_ANGLE;
    configuration.reps_per_azimuth = REPETITIONS_PER_AZIMUTH;
    configuration.timit_path = TIMIT_PATH;
    configuration.dataset_freq = DATASET_FREQ;
    configuration.max_voices = MAX_VOICES;
    configuration.voice_length = VOICE_LENGTH;
    disp('[room_evaluation] --- Generating dataset...');
    DATASET_ROOM = generate_room_dataset(configuration);
    disp('[room_evaluation] --- Evaluating...');
    results(room_size).receivers = evaluate(DATASET_ROOM, models, DATASET_FREQ, MAX_VOICES);
    results(room_size).table = make_table(DATASET_ROOM, configuration, results(room_size).receivers, models, USE_MAY2011, USE_DIETZ2011, USE_MAY2017, USE_HMM);
end
room_eval.models = models;
room_eval.results = results;
if not(exist('./results/new_exps', 'dir'))
        mkdir('./results/new_exps');
end
save(fullfile('./','results','new_exps',['room_',IDX,'.mat']),'room_eval');
end

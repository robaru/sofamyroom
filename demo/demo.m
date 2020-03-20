addpath('../sofamyroom')
sofa_path = './SOFA/API_MO';

if ~exist(sofa_path, 'dir')
    error('Please install SOFA toolbox! See instructions in ./SOFA/how_to_install.txt');             
end

addpath('./SOFA/API_MO')
SOFAstart
warning('off','SOFA:upgrade');

sofa1_path = fullfile('database','hrtf_ci1.sofa');
sofa2_path = fullfile('database','mit_kemar_normal_pinna.sofa');
sofa3_path = fullfile('database','subject_003.sofa');

sofa1_obj = SOFAload(sofa1_path);
sofa1_obj.SourcePosition = SOFAconvertCoordinates(sofa1_obj.SourcePosition, sofa1_obj.SourcePosition_Type, 'cartesian', sofa1_obj.SourcePosition_Units, 'metre');
sofa1_obj.SourcePosition_Type = 'cartesian';
sofa1_obj.SourcePosition_Units = 'metre';

sofa2_obj = SOFAload(sofa2_path);
sofa2_obj.SourcePosition = SOFAconvertCoordinates(sofa2_obj.SourcePosition, sofa2_obj.SourcePosition_Type, 'cartesian', sofa2_obj.SourcePosition_Units, 'metre');
sofa2_obj.SourcePosition_Type = 'cartesian';
sofa2_obj.SourcePosition_Units = 'metre';

sofa3_obj = SOFAload(sofa3_path);
sofa3_obj.SourcePosition = SOFAconvertCoordinates(sofa3_obj.SourcePosition, sofa3_obj.SourcePosition_Type, 'cartesian', sofa3_obj.SourcePosition_Units, 'metre');
sofa3_obj.SourcePosition_Type = 'cartesian';
sofa3_obj.SourcePosition_Units = 'metre';

roomsim load 'SOFA ./database/hrtf_ci1.sofa'
roomsim load 'SOFA ./database/mit_kemar_normal_pinna.sofa'
roomsim load 'SOFA ./database/subject_003.sofa'

common_params = readsetup('roomsetup.m');

room1_absorption = [[0.55   0.86    0.83    0.87    0.90    0.87    0.87]    % prayers 1/1
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]];   % carpet on concrete

room1_diffusion =  [[0.5    0.5     0.5     0.5     0.5     0.5     0.5]    % prayers 1/1
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]];  % carpet on concrete

receiver1_position = [0.211 * common_params.room.dimension(1) 0.294 * common_params.room.dimension(2) 0.583 * common_params.room.dimension(3)];
receiver1_orientation = [0 0 0];
receiver1_description = ['SOFA ' sofa1_path];

room1_setup = common_params;
room1_setup.options.fs = sofa1_obj.Data.SamplingRate;
room1_setup.room.surface.absorption = room1_absorption;
room1_setup.room.surface.diffusion = room1_diffusion;
room1_setup.receiver.orientation = receiver1_orientation;
room1_setup.receiver.description = receiver1_description;

iterations = size(sofa1_obj.SourcePosition, 1);
room1_output = zeros(ceil(room1_setup.options.fs * room1_setup.options.responseduration), 2 * iterations);
for i=1:iterations
   fprintf("Room 1 - %d/%d\n", i, iterations);
   room1_setup.receiver.location = receiver1_position + sofa1_obj.SourcePosition(i, :);
   room1_output(:, [2 * i - 1 2 * i]) = roomsim(room1_setup);
end

room2_absorption = [[0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]];   % carpet on concrete

room2_diffusion =  [[0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]];  % carpet on concrete
            
receiver2_position = [0.5 * common_params.room.dimension(1) 0.294 * common_params.room.dimension(2) 0.583 * common_params.room.dimension(3)];
receiver2_orientation = [0 0 0];
receiver2_description = ['SOFA ' sofa2_path];

room2_setup = common_params;
room2_setup.options.fs = sofa2_obj.Data.SamplingRate;
room2_setup.room.surface.absorption = room2_absorption;
room2_setup.room.surface.diffusion = room2_diffusion;
room2_setup.receiver.orientation = receiver2_orientation;
room2_setup.receiver.description = receiver2_description;

iterations = size(sofa2_obj.SourcePosition, 1);
room2_output = zeros(ceil(room2_setup.options.fs * room2_setup.options.responseduration), 2 * iterations);
for i=1:iterations
    fprintf("Room 2 - %d/%d\n", i, iterations);
    room2_setup.receiver.location = receiver2_position + sofa2_obj.SourcePosition(i, :);
    room2_output(:, [2 * i - 1 2 * i]) = roomsim(room2_setup);
end

room3_absorption = [[0.01   0.01    0.01    0.02    0.02    0.02    0.02]     % ceramic tiles
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]];   % solid wood panel

room3_diffusion = [[0.3    0.3     0.3     0.3     0.3     0.3     0.3]   % ceramic tiles
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]]; % solid wood panel]

receiver3_position = [0.570 * common_params.room.dimension(1) 0.598 * common_params.room.dimension(2) 0.583 * common_params.room.dimension(3)];
receiver3_orientation = [0 0 0];
receiver3_description = ['SOFA ' sofa3_path];

room3_setup = common_params;
room3_setup.options.fs = sofa3_obj.Data.SamplingRate;
room3_setup.room.surface.absorption = room3_absorption;
room3_setup.room.surface.diffusion = room3_diffusion;
room3_setup.receiver.orientation = receiver3_orientation;
room3_setup.receiver.description = receiver3_description;

iterations = size(sofa3_obj.SourcePosition, 1);
room3_output = zeros(ceil(room3_setup.options.fs * room3_setup.options.responseduration), 2 * iterations);
for i=1:iterations
    fprintf("Room 3 - %d/%d\n", i, iterations);
    room3_setup.receiver.location = receiver3_position + sofa3_obj.SourcePosition(i, :);
    room3_output(:, [2 * i - 1 2 * i]) = roomsim(room3_setup);
end
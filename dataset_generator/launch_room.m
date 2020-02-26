parpool('local');

%% prayers 1/1 floor + resto carpet 0.37
IDX = 'small_reverb';
ABS_FLOOR = [0.55 0.86 0.83 0.87 0.90 0.87]; % prayers 1/1
ABS_WALLS = [0.02 0.06 0.14 0.37 0.60 0.65]; % carpet on concrete
ABS_CEIL = [0.02 0.06 0.14 0.37 0.60 0.65];  % carpet on concrete
SCAT_FLOOR =  [0.5 0.5 0.5 0.5 0.5 0.5];     % prayers 1/1
SCAT_WALLS = [0.1 0.1 0.1 0.1 0.1 0.1];      % carpet on concrete
SCAT_CEIL = [0.1 0.1 0.1 0.1 0.1 0.1];       % carpet on concrete
room_evaluation(IDX, ABS_WALLS, ABS_CEIL, ABS_FLOOR, SCAT_WALLS, SCAT_CEIL, SCAT_FLOOR);

%% pav. legno + resto carpet 0.98
IDX = 'medium_reverb';
ABS_FLOOR = [0.15 0.10 0.06 0.08 0.10 0.05]; % solid wood panel
ABS_WALLS = [0.02 0.06 0.14 0.37 0.60 0.65]; % carpet on concrete
ABS_CEIL = [0.02 0.06 0.14 0.37 0.60 0.65];  % carpet on concrete
SCAT_FLOOR = [0.1 0.1 0.1 0.1 0.1 0.1];      % solid wood panel
SCAT_WALLS = [0.1 0.1 0.1 0.1 0.1 0.1];      % carpet on concrete
SCAT_CEIL = [0.1 0.1 0.1 0.1 0.1 0.1];       % carpet on concrete
room_evaluation(IDX, ABS_WALLS, ABS_CEIL, ABS_FLOOR, SCAT_WALLS, SCAT_CEIL, SCAT_FLOOR);

%% pav. ceramica e pareti legno 1.88
IDX = 'high_reverb';
ABS_FLOOR = [0.01 0.01 0.01 0.02 0.02 0.02]; % ceramic tiles
ABS_WALLS = [0.15 0.10 0.06 0.08 0.10 0.05]; % solid wood panel
ABS_CEIL = [0.15 0.10 0.06 0.08 0.10 0.05];  % solid wood panel
SCAT_FLOOR =  [0.3 0.3 0.3 0.3 0.3 0.3];     % ceramic tiles
SCAT_WALLS = [0.1 0.1 0.1 0.1 0.1 0.1];      % solid wood panel
SCAT_CEIL = [0.1 0.1 0.1 0.1 0.1 0.1];       % solid wood panel
room_evaluation(IDX, ABS_WALLS, ABS_CEIL, ABS_FLOOR, SCAT_WALLS, SCAT_CEIL, SCAT_FLOOR);
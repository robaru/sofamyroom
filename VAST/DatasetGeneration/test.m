addpath("/home/robaru/repos/amtoolbox-code")
addpath("/home/robaru/repos/roomsim_old/roomsim_mod/")
% amt_start

sofa1_path = fullfile('database','hrtf_ci1.sofa');
sofa2_path = fullfile('database','mit_kemar_normal_pinna.sofa');
sofa3_path = fullfile('database','subject_003.sofa');

sofa = struct('obj', struct());
sofa(1).obj = SOFAload(sofa1_path);
sofa(2).obj = SOFAload(sofa2_path);
sofa(3).obj = SOFAload(sofa3_path);

%% ROOMs
room(1).absorption = [[0.55   0.86    0.83    0.87    0.90    0.87    0.87]    % prayers 1/1
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]    % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]];   % carpet on concrete

room(1).diffusion =  [[0.5    0.5     0.5     0.5     0.5     0.5     0.5]    % prayers 1/1
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]];  % carpet on concrete
                
room(2).absorption = [[0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]     % carpet on concrete
                    [0.02   0.06    0.14    0.37    0.60    0.65    0.65]];   % carpet on concrete

room(2).diffusion =  [[0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]    % carpet on concrete
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]];  % carpet on concrete
                

room(3).absorption = [[0.01   0.01    0.01    0.02    0.02    0.02    0.02]     % ceramic tiles
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]     % solid wood panel
                    [0.15   0.10    0.06    0.08    0.10    0.05    0.05]];   % solid wood panel

room(3).diffusion = [[0.3    0.3     0.3     0.3     0.3     0.3     0.3]   % ceramic tiles
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]   % solid wood panel
                    [0.1    0.1     0.1     0.1     0.1     0.1     0.1]]; % solid wood panel]
                
%% RECEIVERS
receiver(1).location = [0.211 * common_params.room.dimension(1) 0.294 * common_params.room.dimension(2) 0.5 * common_params.room.dimension(3)];
receiver(1).orientation = [0 0 0];

receiver(2).location = [0.5 * common_params.room.dimension(1) 0.294 * common_params.room.dimension(2) 0.5 * common_params.room.dimension(3)];
receiver(2).orientation = [0 0 0];

receiver(3).location = [0.570 * common_params.room.dimension(1) 0.598 * common_params.room.dimension(2) 0.5 * common_params.room.dimension(3)];
receiver(3).orientation = [0 0 0];

%% RECEIVERS TYPE
receiver_type = cell(3,1);
receiver_type{1} = ['SOFA ' sofa1_path ' interp=1 norm=1'];
receiver_type{2} = ['SOFA ' sofa2_path ' interp=1 norm=1'];
receiver_type{3} = ['SOFA ' sofa3_path ' interp=1 norm=1'];

for l = 1:3
    num_pos = length(sofa(l).obj.SourcePosition);
    cartesian_locations = SOFAconvertCoordinates(sofa(l).obj.SourcePosition, sofa(l).obj.SourcePosition_Type, 'cartesian', sofa(l).obj.SourcePosition_Units, 'metre');
    for r = 1:3
        for p = 1:3
            setup = common_params;
            setup.room.surface.absorption = room(r).absorption;
            setup.room.surface.diffusion = room(r).diffusion;
            setup.receiver.location = receiver(p).location;
            setup.receiver.orientation = receiver(p).orientation;
            setup.receiver.description = receiver_type{l};
            setup.options.fs = sofa(l).obj.Data.SamplingRate;
            for s = 1:num_pos % source
                setup.source(s,1).location = receiver(p).location + cartesian_locations(s, :);
                setup.source(s,1).orientation = [0 0 0];
                setup.source(s,1).description = 'omnidirectional';
            end

            tic
            fprintf("\tgenerating BRIRs")
            brir = roomsim(setup);
            toc

            if isa(brir, 'double')
                if ~isempty(find(isnan(brir)))
                    warning(sprintf("NaN detected r%i p%i l%i", r,p,l))
                end
            elseif ~isempty(find(isnan(cell2mat(brir))))
                warning(sprintf("NaN detected r%i p%i l%i", r,p,l))
            end

            parsave(sprintf('brir/brir_r%i_p%i_l%i.mat', r, p, l), brir)
        end
    end
end

function parsave(fname, brir)
  save(fname, 'brir')
end
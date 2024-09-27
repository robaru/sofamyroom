%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 
%   This script shows an example on using sofa_my_room acoustic simulator 
%   with the dataset generator. It accounts for downloading different HRTF
%   datasets, load the room configurations, run an auditory model for sound
%   localization to estimate the Direction of Arrival of the source. 
%   
%   The auditory model is available within the Auditory Modelling Toolbox
%   available at http://amtoolbox.sourceforge.net/
%
%   For any further help or question please contact Roberto Barumerli,
%   barumerli@dei.unipd.it
%   
%   Licensed under the EUPL-1.2
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% download sofa files
% the sofacoustics.org project provides different HRTF databases
% for this demo the MIT Kemar and a CIPIC subject were choosen
sofa_names = {''};

% check if files are available otherwise download them
outfilename = websave(filename,url)
for i = 1:length(filenames)
    path = ['db://reijniers2014/',filenames{i}];
    
end

%% define rooms
% use the rooms provided into the ICA publication
% plot room geometry

%% generate dataset
% plot some impulse responses

%% run simulations
% auditory model
    % Preallocation
    res = struct('err',struct([]), 'doa_real',struct([]));
    res = repmat(res,length(filenames),1);

    amt_disp('Processing three subjects in parallel...','progress');

    parfor i = 1:length(filenames)
        amt_disp(['Processing subject #' num2str(i)],'progress');
        % Get directions from SOFA files
        [template, target] = reijniers2014_preproc(SOFA_obj(i));
        doa = reijniers2014(template, target,'num_exp',num_exp);

        res(i).err = reijniers2014_metrics(doa);
        res(i).doa_real = doa.real;
    end

end

%% plot estimation results
if flags.do_plot
    for i = 1:length(sofa_names)
        [~, cbar] = plot_reijniers2014(res(i).doa_real, ...
                                res(i).err, ...
                                flags.plot_type, flags.type);
        title(sprintf('Fig. 3(a), Subject %i', i));
        caxis([0,20]);
        cbar.Label.String = 'Mean spherical error [^\circ]';
        cbar.Label.FontSize = 12;    
        ct1=get(cbar,'TickLabels');
        for k=1:numel(ct1)
            ct1{k}=sprintf('%s',ct1{k});
        end
        set(cbar,'xticklabel',ct1);
    end
end
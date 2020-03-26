clear
clc

PATH_TO_RIRS_TABLES_FILES = '../Results/task10/';
RIRS_FILENAMES = '*';
RESULTS_FOLDER = '../Data/';

if strcmp(RIRS_FILENAMES,'*')
    fprintf('READING RIRs DATA FOLDER\n%s\n',PATH_TO_RIRS_TABLES_FILES)
    d = dir([PATH_TO_RIRS_TABLES_FILES '*.mat']);
    [~,index] = sortrows({d.name}.'); 
    d = d(index);
    name_folds = {d.name};
    name_folds(ismember(name_folds,{'.','..'})) = [];
else
    name_folds = {RIRS_FILENAMES};
end
n_tables = length(name_folds);
fprintf('\t%d MATLAB files founded\n\n',n_tables);
rir_max_samples = 2000;

for i = 1:n_tables
    
    path_to_rirs = [PATH_TO_RIRS_TABLES_FILES name_folds{i}];
    
    [filepath,name,ext] = fileparts(path_to_rirs);
    
    f = load(path_to_rirs);
    
    Fs = 16000;

    
    idx = abs(f.vast.ddiff)   < 0.01/Fs ...
           | abs(f.vast.tdoa)  < 1/Fs ...
           | abs(f.vast.itdoa) < 1/Fs ...
           | f.vast.delta1     < 2/Fs ...
           | f.vast.delta2     < 2/Fs;
    f.vast(idx,:) = [];
    warning(['removed ' num2str(sum(idx)) ' rirs'])
    
    if i == 1
        r = f.vast;
        clean_rirs = single(zeros(height(r), 2, rir_max_samples));
        noisy_rirs = single(zeros(height(r), 2, rir_max_samples));
        clean_rirs(:,1,:) = single(r.RIRSynth1(:,1:rir_max_samples));
        clean_rirs(:,2,:) = single(r.RIRSynth2(:,1:rir_max_samples));
        noisy_rirs(:,1,:) = single(r.RIR1(:,1:rir_max_samples));
        noisy_rirs(:,2,:) = single(r.RIR2(:,1:rir_max_samples));
        r.RIR1 = [];
        r.RIR2 = [];
        r.RIRSynth1 = [];
        r.RIRSynth2 = [];

    else
        cr = single(zeros(height(f.vast), 2, rir_max_samples));
        nr = single(zeros(height(f.vast), 2, rir_max_samples));
        cr(:,1,:) = single(f.vast.RIRSynth1(:,1:rir_max_samples));
        cr(:,2,:) = single(f.vast.RIRSynth2(:,1:rir_max_samples));
        nr(:,1,:) = single(f.vast.RIR1(:,1:rir_max_samples));
        nr(:,2,:) = single(f.vast.RIR2(:,1:rir_max_samples));
        f.vast.RIR1 = [];
        f.vast.RIR2 = [];
        f.vast.RIRSynth1 = [];
        f.vast.RIRSynth2 = [];
        r = [f.vast; r];
        clean_rirs = [cr; clean_rirs];
        noisy_rirs = [nr; noisy_rirs];
    end
    fprintf('Progress %d\n',floor(100*(i/n_tables)))
    
end

disp('... done.')
disp('Now saving to output file...')

annotations = r;

file_parts = strsplit(name,'_');
saveas = [RESULTS_FOLDER 'annotations.mat'];
save(saveas,'annotations','-v7.3');
saveas = [RESULTS_FOLDER 'clean_rirs.mat'];
save(saveas,'clean_rirs','-v7.3');
saveas = [RESULTS_FOLDER 'noisy_rirs.mat'];
save(saveas,'noisy_rirs','-v7.3');
disp('... done.')
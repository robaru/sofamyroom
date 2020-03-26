function table2vars(path_to_rir_dataset)

load(path_to_rir_dataset)

[filepath,name,ext] = fileparts(path_to_rir_dataset);

tab = annotations;

vars = tab.Properties.VariableNames;
n_fields = length(vars);
for i = 1:n_fields
    eval([vars{i} ' = tab.' vars{i} ';']) 
end
clear ILD_clean
clear rIPD_clean
clear iIPD_clean

clear ILD_clean_10th
clear rIPD_clean_10th
clear iIPD_clean_10th
clear annotations
clear tab
clear vars
clear i
clear n_fields
clear rir_dataset

save_as = [filepath '/' name '_vars' ext];

save(save_as);
disp('... done!')

end
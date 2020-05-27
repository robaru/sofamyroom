function VAST_out = VAST_load(VAST_PATH,varargin)
%%%%%%%%%%%%%%%%%%%%%%%%%%% VAST_load %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Load a subset of VAST dataset best on a list of .mat files and a set of
% constraints
%%%% Input %%%%
% - VAST_PATH (string) : file or folder path. If folder, then recursively
%                        process every .mat file in the folder and its
%                        subfolders
% - VAST_PATH (struct) : VAST dataset structure
%
% <optional>:
% - 'FIELD',[min,max] : only get data such that the parameters contained in
%   the field 'FIELD' of the VAST structure are within the interval
%   specified by [min,max].
%   EXAMPLES:
%       - VAST_get(VAST_PATH,'Source.Azimuth', [-90,90],...
%                            'Receiver.Position(1)', [0,1]);
%         Only return points such that the source azimuth is between -90
%         and +90 degrees, and the receiver's X position is between 0 and
%         1 meters.
%%%% Output %%%%
% - VAST_out (struct) : VAST dataset structure respecting the interval
%                       constraints and merging all the .mat VAST files in
%                       the input directory.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% Process optional input parameters
[src_az,src_el,src_dist,...
 rec_pos1,rec_pos2,rec_pos3,rec_spot] = process_options(varargin,...
                                 'Source.Azimuth',[-Inf,Inf],...
                                 'Source.Elevation',[-Inf,Inf],...
                                 'Source.Distance',[-Inf,Inf],...
                                 'Receiver.Position(1)',[-Inf,Inf],...
                                 'Receiver.Position(2)',[-Inf,Inf],...
                                 'Receiver.Position(3)',[-Inf,Inf],...
                                 'Receiver.Spot',...
                                     {'South','SouthWest','SouthEast',...
                                      'North','NorthWest','NorthEast',...
                                      'Middle','East','West',''});
% Call the recursive function:                             
VAST_out = VAST_load_rec(VAST_PATH,...
                        src_az,src_el,src_dist,...
                        rec_pos1,rec_pos2,rec_pos3,rec_spot);

end


%%%%%%%%%%%%%%%%%%%%%%%%%%% VAST_load_rec %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This is the recursive function used inside VAST_get. It does not use
% varargin, to avoid problems due to recursivity
function VAST_out = VAST_load_rec(VAST_PATH,...
                                         src_az,src_el,src_dist,...
                                         rec_pos1,rec_pos2,rec_pos3,...
                                         rec_spot)
                                     
% Print the current path being processed:
fprintf(1,['Loading VAST data: ',VAST_PATH,'\n']);

%%% If the input is a struct, then directly use the input
if(isstruct(VAST_PATH))
    VAST_full = VAST_PATH;
else
%%% If the input is a folder, recursively apply to each file and subfolder
if exist(VAST_PATH,'file')
    if(exist(VAST_PATH,'dir'))
        listing = dir(VAST_PATH);
        nfiles = numel(listing);
        VAST_out = struct;
        for f=1:nfiles
            % Recursive call on each .mat file and each subfolder:
            if(~isempty(strfind(listing(f).name,'.mat')) ||...
               (listing(f).isdir && listing(f).name(1)~='.'))
                VAST_out_f = VAST_load_rec([VAST_PATH,listing(f).name],...
                                          src_az,src_el,src_dist,...
                                          rec_pos1,rec_pos2,rec_pos3,...
                                          rec_spot);
                VAST_out = VAST_merge(VAST_out,VAST_out_f);
            end
        end
        return;
    else
        if(isempty(strfind(VAST_PATH,'.mat')))
        	error('VAST_PATH does not point to a folder or a .mat file');
        end
    end
else
    error([VAST_PATH,' is not a vaild VAST path.']);
end
%%% If it is a .mat file then open and filter
VAST_full = load(VAST_PATH);
end

% Indexes of valid data:
idx = VAST_full.Source.Azimuth>=src_az(1) & ...
      VAST_full.Source.Azimuth<=src_az(2) & ...
      VAST_full.Source.Elevation>=src_el(1) & ...
      VAST_full.Source.Elevation<=src_el(2) & ...
      VAST_full.Source.Distance>=src_dist(1) & ...
      VAST_full.Source.Distance<=src_dist(2) & ...
      VAST_full.Receiver.Position(1,:)>=rec_pos1(1) & ...
      VAST_full.Receiver.Position(1,:)<=rec_pos1(2) & ...
      VAST_full.Receiver.Position(2,:)>=rec_pos2(1) & ...
      VAST_full.Receiver.Position(2,:)<=rec_pos2(2) & ...
      VAST_full.Receiver.Position(3,:)>=rec_pos3(1) & ...
      VAST_full.Receiver.Position(3,:)<=rec_pos3(2);

% Spot
if(isfield(VAST_full.Receiver,'Spot'))
    idx_spot = false;
    for i=1:numel(rec_spot)
       idx_spot = idx_spot | strcmp(VAST_full.Receiver.Spot,rec_spot{i});
    end
    idx = idx & idx_spot;
end
VAST_out = VAST_full; % Initialize;

%%% Filter :
% RIR
VAST_out.RIR.Left = VAST_full.RIR.Left(:,idx);
VAST_out.RIR.Right = VAST_full.RIR.Right(:,idx);

% Source Properties
VAST_out.Source.Azimuth = VAST_full.Source.Azimuth(:,idx);
VAST_out.Source.Elevation = VAST_full.Source.Elevation(:,idx);
VAST_out.Source.Distance = VAST_full.Source.Distance(:,idx);
VAST_out.Source.AbsolutePos = VAST_full.Source.AbsolutePos(:,idx);

% Receiver Properties
VAST_out.Receiver.Position = VAST_full.Receiver.Position(:,idx);
VAST_out.Receiver.Orientation = VAST_full.Receiver.Orientation(:,idx);
if(isfield(VAST_full.Receiver,'Spot'))
    VAST_out.Receiver.Spot = VAST_full.Receiver.Spot(idx);
end
% Room Properties
VAST_out.Room.Absorption.Ceiling = VAST_full.Room.Absorption.Ceiling(:,idx);
VAST_out.Room.Absorption.Floor = VAST_full.Room.Absorption.Floor(:,idx);
VAST_out.Room.Absorption.EastWall = VAST_full.Room.Absorption.EastWall(:,idx);
VAST_out.Room.Absorption.NorthWall = VAST_full.Room.Absorption.NorthWall(:,idx);
VAST_out.Room.Absorption.SouthWall = VAST_full.Room.Absorption.SouthWall(:,idx);
VAST_out.Room.Absorption.WestWall = VAST_full.Room.Absorption.WestWall(:,idx);
                            
VAST_out.Room.Diffusion = VAST_full.Room.Diffusion(:,idx);
VAST_out.Room.Size = VAST_full.Room.Size(:,idx);
VAST_out.Room.FreqRT60 = VAST_full.Room.FreqRT60(:,idx);
VAST_out.Room.GlobalRT60 = VAST_full.Room.GlobalRT60(:,idx);

end
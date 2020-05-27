function VAST_out = VAST_merge(VAST1,VAST2)
%%%%%%%%%%%%%%%%%%%%%%%%%%%% VAST_merge %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Merges two VAST dataset structures
%%%% Input %%%%
% - VAST_1 (struct) : VAST dataset structure
% - VAST_2 (struct) : VAST dataset structure

%%%% Output %%%%
% - VAST_out (struct) : VAST dataset structure merging the two
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

VAST_out = VAST_init();

if(~isfield(VAST1,'RIR'))
    VAST_out = VAST2;
    return
end
if(~isfield(VAST2,'RIR'))
    VAST_out = VAST1;
    return
end

% Get sizes
[L1,N1] = size(VAST1.RIR.Left);
[L2,N2] = size(VAST2.RIR.Left);

% Global Parameters:
if (VAST1.GlobalParams.SamplingFrequency == VAST2.GlobalParams.SamplingFrequency)
    VAST_out.GlobalParams.SamplingFrequency = VAST1.GlobalParams.SamplingFrequency;
else
    error('Incompatible GlobalParams.SamplingFrequency')
end

if (VAST1.GlobalParams.FreqBin == VAST2.GlobalParams.FreqBin)
    VAST_out.GlobalParams.FreqBin = VAST1.GlobalParams.FreqBin;
else
    error('Incompatible GlobalParams.FreqBin')
end

% Receiver's Parameters: 
%  (use checksums)
if((VAST1.Receiver.HRTF.Fs == VAST2.Receiver.HRTF.Fs) &&...
   (sum(VAST1.Receiver.HRTF.Left(:)) == sum(VAST2.Receiver.HRTF.Left(:))) &&...
   (sum(VAST1.Receiver.HRTF.Right(:)) == sum(VAST2.Receiver.HRTF.Right(:))))
    VAST_out.Receiver.HRTF = VAST1.Receiver.HRTF;
else
	error('Incompatible Receivers')
end
if(~isfield(VAST1.Receiver,'Spot'))
    VAST1.Receiver.Spot = cell(1,N1);
    VAST1.Receiver.Spot(:) = {''};
end
if(~isfield(VAST2.Receiver,'Spot'))
    VAST2.Receiver.Spot = cell(1,N2);
    VAST2.Receiver.Spot(:) = {''};
end
VAST_out.Receiver.Spot = [VAST1.Receiver.Spot VAST2.Receiver.Spot];

VAST_out.Receiver.Position = [VAST1.Receiver.Position VAST2.Receiver.Position];
VAST_out.Receiver.Orientation = [VAST1.Receiver.Orientation VAST2.Receiver.Orientation];

% Room Parameters
VAST_out.Room.Size = [VAST1.Room.Size VAST2.Room.Size];
VAST_out.Room.FreqRT60 = [VAST1.Room.FreqRT60 VAST2.Room.FreqRT60];
VAST_out.Room.GlobalRT60 = [VAST1.Room.GlobalRT60 VAST2.Room.GlobalRT60];
VAST_out.Room.Absorption.NorthWall = [VAST1.Room.Absorption.NorthWall VAST2.Room.Absorption.NorthWall];
VAST_out.Room.Absorption.WestWall = [VAST1.Room.Absorption.WestWall VAST2.Room.Absorption.WestWall];
VAST_out.Room.Absorption.SouthWall = [VAST1.Room.Absorption.SouthWall VAST2.Room.Absorption.SouthWall];
VAST_out.Room.Absorption.EastWall = [VAST1.Room.Absorption.EastWall VAST2.Room.Absorption.EastWall];
VAST_out.Room.Absorption.Ceiling = [VAST1.Room.Absorption.Ceiling VAST2.Room.Absorption.Ceiling];
VAST_out.Room.Absorption.Floor = [VAST1.Room.Absorption.Floor VAST2.Room.Absorption.Floor];
VAST_out.Room.Diffusion = [VAST1.Room.Diffusion VAST2.Room.Diffusion];

% Source Parameters
VAST_out.Source.Azimuth = [VAST1.Source.Azimuth VAST2.Source.Azimuth];
VAST_out.Source.Elevation = [VAST1.Source.Elevation VAST2.Source.Elevation];
VAST_out.Source.Distance = [VAST1.Source.Distance VAST2.Source.Distance];
VAST_out.Source.AbsolutePos = [VAST1.Source.AbsolutePos VAST2.Source.AbsolutePos];

% Room Impulse Responses
% Padd with zeroes the shortest RIRs
if(L1>L2)
   VAST2.RIR.Left = [VAST2.RIR.Left; zeros(L1-L2,N2)]; 
   VAST2.RIR.Right = [VAST2.RIR.Right; zeros(L1-L2,N2)]; 
else
   VAST1.RIR.Left = [VAST1.RIR.Left; zeros(L2-L1,N1)]; 
   VAST1.RIR.Right = [VAST1.RIR.Right; zeros(L2-L1,N1)]; 
end 

VAST_out.RIR.Left = [VAST1.RIR.Left VAST2.RIR.Left];
VAST_out.RIR.Right = [VAST1.RIR.Right VAST2.RIR.Right];

end
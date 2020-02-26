function [ RIRS ] = RIR_generation(configuration)
    RIRS = table_init(configuration);
    % Receiver (Position in the room) 
    RIRS.ReceiverPosition(:,1) = configuration.receiver(1).location(1);
    RIRS.ReceiverPosition(:,2) = configuration.receiver(1).location(2);
    RIRS.ReceiverPosition(:,3) = configuration.receiver(1).location(3);
    % Receiver Orientation
    RIRS.Receiver1Orientation(:,1) = configuration.receiver(1).orientation(1);
    RIRS.Receiver1Orientation(:,2) = configuration.receiver(1).orientation(2);
    RIRS.Receiver1Orientation(:,3) = configuration.receiver(1).orientation(3);
    % Sources (Position in the room)
    for c=1:height(RIRS)
        RIRS.SourcePosition(c,1) = configuration.sources(c).location(1);
        RIRS.SourcePosition(c,2) = configuration.sources(c).location(2);
        RIRS.SourcePosition(c,3) = configuration.sources(c).location(3);
        %Azimuth estimation
        az = atan2d((RIRS.SourcePosition(c,2)-RIRS.ReceiverPosition(c,2)),(RIRS.SourcePosition(c,1)-RIRS.ReceiverPosition(c,1)));
        if az < 0
            az = 360 + az;
        else    
        end
        az = -(-90 + az);
        if abs(az) > 180
            az = (360 + az);
        end
        RIRS.Azimuth(c) = az;
    end  
    warning ('off','all');
    combinations = height(RIRS);
    %% Impulse response generation
    for s = 1:combinations
        config_temp = configuration;
        config_temp.source(1) = config_temp.sources(s);
        RawImpRes=roomsim(config_temp);
        if iscell(RawImpRes)
           ImpRes = cell2mat(RawImpRes);
        else
           ImpRes = RawImpRes;
        end
           
        % check
        if or(isnan(ImpRes(:,1)),isnan(ImpRes(:,2)))
           error('Wrong Impulse Response')
        else
        RIRS.RIR(s,:) = ImpRes(:);
        % Get Rt60 values
        RtParams.Fs = configuration.options.fs;
        RtParams.FreqBin = configuration.room.surface.frequency;
        [RIRS.FreqRT60(s,:), RIRS.GlobalRT60(s)] = Rir2Rt60(mean(ImpRes,2),RtParams);
        end
    end
end
function tab = table_init(configuration)
    tab = table();
    %% General parameters
    num_smp = configuration.options.responseduration*configuration.options.fs;
    num_rirs = length(configuration.sources);
    %% RIRs
    tab.RIR = zeros(num_rirs,num_smp,2);
    %% Source
    % geometry
    tab.SourcePosition = zeros(num_rirs,3); % 3xN (x,y,z)
    %% Microphones / Receiver / Array
    % geometry
    tab.ReceiverPosition = zeros(num_rirs,3); % 3xN (x,y,z)
    %% Azimuth estimate
    tab.Azimuth  = zeros(num_rirs,1); % Azimuth WRT receiver
    %% Rooms
    % Rooms Sound properties
    tab.FreqRT60   = zeros(num_rirs,length(configuration.room.surface.frequency));
    tab.GlobalRT60 = zeros(num_rirs,1);
end

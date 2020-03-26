function [ FreqRt60, GlobalRt60 ] = Rir2Rt60( varargin )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

if nargin < 2
    Params = [];
    RIR = varargin{1};
else    
    if isa(varargin{2}, 'struct')
        Params = varargin{2};
        RIR = varargin{1};
    else
        disp('Wrong call, please use: [Rt60] = Rir2Rt60(RIR,Params) with Params a struct')
    end
end

if ~isfield(Params, 'BandsPerOctave') || isempty(Params.BandsPerOctave)
    Params.BandsPerOctave = 1;
end

if ~isfield(Params, 'Fs') || isempty(Params.Fs)
    Params.Fs = 44100;
end

if ~isfield(Params, 'TimeWeighting') || isempty(Params.TimeWeighting)
    Params.TimeWeighting = 1/(Params.Fs);
end

if ~isfield(Params, 'FilterOrder') || isempty(Params.FilterOrder)
    Params.FilterOrder = 6;
end

if ~isfield(Params, 'FreqBin') || isempty(Params.FreqBin)
    Params.FreqBin = [125,250,500,1000,2000,4000,8000];
end

if ~isfield(Params, 'F0') || isempty(Params.F0)
    Params.F0 = 1000;
end

MoySamples = ceil(Params.TimeWeighting*Params.Fs);


%% Global Rt60 calculations

% Suppressing the transient state 
MaxInd = find(RIR==max(RIR));
RIR = RIR(MaxInd:end);
TimeVec = linspace(0, length(RIR)/(Params.Fs), length(RIR));

% Schroeder Inegration method for energy
L = zeros(length(RIR),1);
L(end:-1:1)=(cumsum(RIR(end:-1:1).^2)/sum(RIR(1:end).^2));
LdB = 10*log10(L);
% Normalizing decay curve to zero
LdB = LdB - max(LdB);

% Smoothing (time moving window)
LdB = filter(ones(1,MoySamples)/MoySamples,1,LdB);

% Curve fitting
SIndex=find(LdB <  -5, 1);
FIndex=find(LdB < -65, 1);

LdBfit = LdB(SIndex:FIndex);
Timefit = TimeVec(SIndex:FIndex);

p = polyfit(Timefit', LdBfit, 1);

GlobalRt60 = -60/p(1);


%% Frequency Band setup


f = fdesign.octave(Params.BandsPerOctave,'Class 1','N,F0',Params.FilterOrder,...
    Params.F0,Params.Fs);
Params.F0 = validfrequencies(f);
Params.F0(Params.F0>max(Params.FreqBin)) = [];
Params.F0(Params.F0<min(Params.FreqBin)) = [];
Nfc = length(Params.FreqBin);

%% Frequency band Rt60 calculations

for i=1:Nfc
    f.F0 = Params.F0(i);
    Hd(i) = design(f,'butter');
    RIRfilt(:,i) = filter(Hd(i),RIR);
    % Schroeder Inegration method for energy
    L = zeros(length(RIRfilt(:,1)),1);
    L(end:-1:1)=(cumsum(RIRfilt(end:-1:1,i).^2)/sum(RIRfilt(1:end,i).^2));
    LdB = 10*log10(L);
    % Normalizing decay curve to zero
    LdB = LdB - max(LdB);

    % Smoothing (time moving window)
    LdB = filter(ones(1,MoySamples)/MoySamples,1,LdB);

    % Curve fitting
    SIndex=find(LdB <  -5, 1);
    FIndex=find(LdB < -65, 1);

    LdBfit = LdB(SIndex:FIndex);
    Timefit = TimeVec(SIndex:FIndex);

    p = polyfit(Timefit', LdBfit, 1);

    FreqRt60(i,1) = -60/p(1);
end


end


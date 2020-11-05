function [ VAST ] = VASTinit(varargin)
%VASTinit Initialize a VAST structure with common parameters (sample
%frequency, sizes, ...) and build the rest of the structure.
%
%   Usage:
%   [VAST] = VASTinit();
%   [VAST] = VASTinit(PreviousVASTstruct)
%   [VAST] = VASTinit(PreviousVASTstruct,RIRnb)
%   [VAST] = VASTinit(RIRnb)
%
%   Input (optional)
%
%   varargin{1}: VASTstruct = struct already containing appropriate fields
%   varargin{2}: Number of RIR to store 'RIRnb'
%
%   Output
%
%   VAST: 1x1 struct containing the default fields for the dataset
%   initialization
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% AUTHORS: Antoine Deleforge and Clément Gaultier,
%       PANAMA Research Group, Inria, France
%       http://thevastproject.inria.fr/dataset/
% adapted by Roberto Barumerli, barumerli@dei.unipd.it
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin < 1
    VAST = [];
else    
    if isa(varargin{1}, 'struct')
        VAST = varargin{1};
        if isa(varargin{2}, 'double')
            RIRnb = varargin{2};
        end
    else
        if isa(varargin{1}, 'double')
            RIRnb = varargin{1};
            VAST = [];
        end
    end
end


if ~isfield(VAST, 'GlobalParams') || isempty(VAST.GlobalParams)
    VAST.GlobalParams.SamplingFrequency = 16000;
    VAST.GlobalParams.FreqBin = [125,250,500,1000,2000,4000];
    VAST.GlobalParams.RIRLengthTime = 1.25;
end

if ~isfield(VAST, 'RIR') || isempty(VAST.RIR)
    if exist('RIRnb','var')
        VAST.RIR.Left = ...
            single(zeros(VAST.GlobalParams.RIRLengthSamples, RIRnb));
        VAST.RIR.Right = ...
            single(zeros(VAST.GlobalParams.RIRLengthSamples, RIRnb));
    else
        VAST.RIR.Left = single([]);
        VAST.RIR.Right = single([]);
    end
end

if ~isfield(VAST, 'Receiver') || isempty(VAST.Receiver)
    if exist('RIRnb','var')
        VAST.Receiver = struct('Position',single(zeros(3,RIRnb)),...
            'Orientation',single(zeros(1,RIRnb)),...
            'HRTF',struct('Left',single([]),'Right',single([]),'Fs',NaN));
    else
        VAST.Receiver = struct('Position',single([]),...
            'Orientation',single([]),...
            'HRTF',struct('Left',single([]),'Right',single([]),'Fs',NaN));
    end
end

if ~isfield(VAST, 'Source') || isempty(VAST.Source)
    if exist('RIRnb','var')
        VAST.Source = struct('Azimuth',single(zeros(1,RIRnb)),...
            'Elevation',single(zeros(1,RIRnb)),...
            'Distance',single(zeros(1,RIRnb)),...
            'AbsolutePos',single(zeros(3,RIRnb)));
    else
        VAST.Source = struct('Azimuth',single([]),...
            'Elevation',single([]),'Distance',single([]),...
            'AbsolutePos',single([]));
    end
end

if ~isfield(VAST, 'Room') || isempty(VAST.Room)
    if exist('RIRnb','var')
        freq_bands = length(VAST.GlobalParams.FreqBin);
        VAST.Room = struct('Size',single(zeros(3,RIRnb)),...
            'FreqRT60',single(zeros(freq_bands,RIRnb)),...
            'GlobalRT60',single(zeros(1,RIRnb)),...
            'Absorption',struct('NorthWall',single(zeros(6,RIRnb)),...
            'WestWall',single(zeros(freq_bands,RIRnb)),...
            'SouthWall',single(zeros(freq_bands,RIRnb)),...
            'EastWall',single(zeros(freq_bands,RIRnb)),...
            'Ceiling',single(zeros(freq_bands,RIRnb)),...
            'Floor',single(zeros(freq_bands,RIRnb))),...
            'Diffusion',single(zeros(freq_bands,RIRnb)));
    else
        VAST.Room = struct('Size',single([]),...
            'FreqRT60',single([]),...
            'GlobalRT60',single([]),...
            'Absorption',struct('NorthWall',single([]),...
            'WestWall',single([]),...
            'SouthWall',single([]),...
            'EastWall',single([]),...
            'Ceiling',single([]),...
            'Floor',single([])),...
            'Diffusion',single([]));
    end
end

end


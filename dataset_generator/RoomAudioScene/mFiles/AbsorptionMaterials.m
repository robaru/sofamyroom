FloorAbsorb = ...
	[0.40,	0.30, 	0.20, 	0.17, 	0.15,	0.10; % Wood platform with large space beneath
	 0.08, 	0.07, 	0.06, 	0.07, 	0.08, 	0.08; % Raised computer floor, steel-faced 45mm chipboard 800mm above concrete floor, no carpet
	 0.27, 	0.26, 	0.52, 	0.43, 	0.51, 	0.58; % Raised computer floor, steel-faced 45mm chipboard 800mm above concrete floor, office-grade carpet tiles 
	 0.02,	0.04,	0.08,	0.2,	0.3,	0.4;  % VAST
	 0.3,	0.2,	0.1,	0.1,	0.1,	0.07];% VAST
CeilingAbsorb = ...
	[0.45,	0.70,	0.80,	0.80,	0.65,	0.45; % Gypsum plaster tiles, 17% perforated, 22mm 
 	0.45,	0.55,	0.6,	0.9,	0.86,	0.75];% VAST

WallAbsorb = ...
	[0.3,	0.69,	1,		0.81,	0.66,	0.62; % VAST
	 0.21,	0.66,	1,		1,		0.97,	0.98];% VAST

Diffuse = ...
	[0.0109,0.00750,0.0511,	0.0668,	0.1472,	0.2878;% VAST
	 0.003, 0.004,	0.045, 	0.077, 	0.210, 	0.431]; % Schimmel
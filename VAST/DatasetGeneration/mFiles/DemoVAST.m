%% Demo VAST
RoomSize = [9,6,3.5];
RoomNb = 'Anechoic';
CeilingAbsorb = [1,1,1,1,1,1,1];
FloorAbsorb = CeilingAbsorb;
WallsAbsorb = CeilingAbsorb;
GridDistance = 1;
ReceiveLoc = [3.5,3,1.7];
Diffuse = [0.0109,0.0075,0.0511,0.0668,0.1472,0.2878,1];
SaveWAV = true;

[ SubsetVAST ] = SubVASTGeneration( RoomSize, RoomNb, CeilingAbsorb, FloorAbsorb, WallsAbsorb, GridDistance, ReceiveLoc, Diffuse, SaveWAV );
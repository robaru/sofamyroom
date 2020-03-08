function R = shoebox2vertex(RoomSize)

	if numel(RoomSize) ~= 3
		error('Number of parameters should be 3.');
	end
	w = RoomSize(1);
	l = RoomSize(2);
	h = RoomSize(3);
	
% 	if w <= 0 | h <= 0 | l <= 0
% 		error('Dimesion cannot be negative');
% 	end
% 
% 	if h > l | h > w
% 		warning('Usually the rooms are deeper or lower, are you sure?');
% 	end
% 
% 	R = [
% 	0, 0, 0;
% 	0, 0, 1;
% 	0, 1, 0;
% 	0, 1, 1;
% 	1, 0, 0;
% 	1, 0, 1;
% 	1, 1, 0;
% 	1, 1, 1;
% 	];
% 	R = R.*[w,l,h];
%     
    A = [0 0 0];
    B = [RoomSize(1) 0 0];
    C = [0 RoomSize(2) 0];
    D = [0 0 RoomSize(3)];
    E = [0 RoomSize(2) RoomSize(3)];
    F = [RoomSize(1) 0 RoomSize(3)];
    G = [RoomSize(1) RoomSize(2) 0];
    H = [RoomSize(1) RoomSize(2) RoomSize(3)];
    R = [A;B;F;H;G;C;A;D;E;H;F;D;E;C;G;B];

end
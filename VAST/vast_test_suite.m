function tests = vast_test_suite
    tests = functiontests(localfunctions);
end


% SHOEBOX2VERTEX
function test_shoebox2vertex1(testCase)
	RoomSize = [1,1,1];
	actual_solution = shoebox2vertex(RoomSize);
	expect_solution = [0, 0, 0; 0, 0, 1; 0, 1, 0; 0, 1, 1; ...
					   1, 0, 0; 1, 0, 1; 1, 1, 0; 1, 1, 1; ];
	verifyEqual(testCase,actual_solution,expect_solution)				   
end

function test_shoebox2vertex2(testCase)
	RoomSize = [5,6,2];
	actual_solution = shoebox2vertex(RoomSize);
	expect_solution = [0, 0, 0; 0, 0, 2; 0, 6, 0; 0, 6, 2; ...
					   5, 0, 0; 5, 0, 2; 5, 6, 0; 5, 6, 2; ];
	verifyEqual(testCase,actual_solution,expect_solution)				   
end

% IMAGE_SOURCE

#include <stdio.h>
#include "mstruct.h"
#include "roomsim.h"


//float mul ( const MyStruct pSetup ){
//    float return_value = pSetup.first * pSetup.second;
//    printf("    In cmult : int: %d float %.1f returning  %.1f\n", pSetup.first,
//            pSetup.second, return_value);
//    return return_value;
//}

//Or that
const MyStruct make_struct(int first, float second){
    const MyStruct hey={first, second};
    return hey;
}

float mul ( const MyStruct *pSetup ){
    float return_value = pSetup->first * pSetup->second;
    printf("    In cmult : int: %d float %.1f returning  %.1f\n", pSetup->first,
            pSetup->second, return_value);
    return return_value;
}

float real_mul(int first, float second){
    float return_value = first * second;
    return return_value;
}

// The main is just to be able to compile it and see that it works.
int main (void){
	MyStruct my_struct;
	my_struct.first = 8;
	my_struct.second = 1.2;
//	printf("Hello, World! %d, %.1f \n", my_struct.first, my_struct.second);
	mul(&my_struct);
	return 0;
}


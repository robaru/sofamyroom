#include <stdio.h>
#include "mstruct.h"
#include "roomsim.h"


//float mul ( const MyStruct pSetup ){
//    float return_value = pSetup.first * pSetup.second;
//    printf("    In cmult : int: %d float %.1f returning  %.1f\n", pSetup.first,
//            pSetup.second, return_value);
//    return return_value;
//}

//const CSensor make_sources

//CSensor* create_sensor(int nSensor){
//	CSensor source[nSensor];
//	return source;
//};


//CSensor fill_sensor(CSensor sensors[], int idx, int first, float second){
//	sensors[idx].first=first;
//	sensors[idx].second=second;
//	return sensors[idx];
//}
//
////Or that
//const MyStruct make_struct(int first, float second){
//    const MyStruct hey={first, second};
//    return hey;
//}
//
//float mul ( const MyStruct *pSetup ){
//    float return_value = pSetup->first * pSetup->second;
//    printf("    In cmult : int: %d float %.1f returning  %.1f\n", pSetup->first,
//            pSetup->second, return_value);
//    return return_value;
//}
//
//float real_mul(int first, float second){
//    float return_value = first * second;
//    return return_value;
//}
//
//void print_room(const CRoomSetup *pSetup){
//	printf("%i\n", pSetup->source[0].first);
//	printf("%f\n", pSetup->source[0].second);
//	printf("%i\n", pSetup->source[1].first);
//	printf("%f\n", pSetup->source[1].second);
//}
//
//void print_sensor(const CSensor *sensor){
//	printf("First %i \n", sensor[0].first);
//	printf("Second %f \n", sensor[0].second);
//	printf("First %i \n", sensor[1].first);
//	printf("Second %f \n", sensor[1].second);
//}

void print_options(const COptions *options){
	printf("%f\n", options->fs);
	printf("%i\n", options->numberofrays);
	printf("%d \n", options->airabsorption);
	printf("%i \n", options->reflectionorder[0]);
}


void print_surface(const CSurface *surface){
	printf("%f \n", surface->frequency[0]);
	printf("%f \n", surface->absorption[0]);
	printf("%f \n", surface->diffusion[0]);
	printf("%i \n", surface->nRowsDiffusion);
	printf("%i \n", surface->nColsDiffusion);

}

void print_room(const CRoom *room){
	printf("%f \n", room->temperature );
	printf("%f \n", room->humidity );
	printf("%f \n", room->dimension[0] );
}


void print_setup(const CRoomSetup *setup){
	printf("setup.options.referencefrequency) = %f  \n", setup->options.referencefrequency);
	printf("setup.room.surface.frequency[0] = %f \n", setup->room.surface.frequency[0]);
	printf("setup.source.location[0])= %f \n", setup->source->location[0]);
	printf("setup.receiver.description) = %s \n", setup->receiver->description);
}

void print_sensor(const CSensor *sensor){
	printf(" %f \n", sensor[0].location[0]);
	printf(" %f \n", sensor[0].orientation[0]);
	printf(" %s \n", sensor[0].description);
	printf(" %f \n", sensor[1].location[0]);
	printf(" %f \n", sensor[1].orientation[0]);
}
// The main is just to be able to compile it and see that it works.
int main (void){
//	int n_src = 2;
//	sensor = create_sensor(n_src);
//	fill_sensor(&sensor, int 0, int 1, float 1.0);
//	CRoomSetup pSetup;
//	static const CSensor source[2] = {
//		{1, 2.0}, {2, 3.0}
//	};
//	static const CSensor sensor[3];
//	pSetup.source = source;
//	print_room(&pSetup);
//	MyStruct my_struct;
//	my_struct.first = 8;
//	my_struct.second = 1.2;
////	printf("Hello, World! %d, %.1f \n", my_struct.first, my_struct.second);
//	mul(&my_struct);
	return 0;
}


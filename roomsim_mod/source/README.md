

# Roomsim

Roomsim is a fast, accurate, and flexible "shoebox" room acoustics simulator that supports both specular and diffuse reflections. The simulator extends the work released by Schimmel et al. by adding the rendering of Binaural Room Impulse Response, BRIR. It supports the AES Spatially Oriented Format for Acoustics (SOFA) file format for storing HRTFs thanks to MySofa library (hereinafter referred as `libmysofa`).

## Authors
* Steven M. Schimmel, Martin F. Muller, and Norbert Dillier - [Roomsim](https://sourceforge.net/projects/roomsim/)
* Christian Hoene, Isabel C. Patiño Mejía, Alexandru Cacerovschi - [MySofa](https://github.com/hoene/libmysofa)
* Piotr Majdak et al. - [Spatially Oriented Format for Acoustics (SOFA)](https://www.sofaconventions.org/)

# Installing Roomsim

This repository includes the binary files and, if in case of need, the CMake scripts allow a customized compilation.

## OS support

Roomsim binaries are available for Windows 10, Ubuntu Linux 16.04 and 18.04 and MacOS High Sierra.

## Installation on Windows

No futher steps are required to run Roomsim on Windows.

## Installation on Ubuntu Linux

`libmysofa` requires `zlib` library in order to work properly. To install it, open a terminal and enter the following command:
```bash
$ sudo apt-get install zlib1g
```
## Installation on MacOS

No further steps are required to run Roomsim on MacOS.

# Usage

To use Roomsim, type this command:

```bash
roomsim setup output
```
`setup` is the name of the text file containing all the roomsim setup parameters structure. A sample of it can be found in `sampleroomsetup.m`. `output` is the name of the binary file written by roomsim, which contains the result of the simulation. To read this file, use MATLAB function `readbrir`, provided with this project. `setup` is mandatory, `output` is optional (the default name of the output file is `output.brir`).

## Usage with MATLAB

A MEX-file for 64-bit MATLAB is available. To run it, type these commands in the Command Window:
```matlab
>> setup = readsetup('sampleroomsetup.m');
>> roomsim(setup)
```
## Notes about the setup file

The format of the field `receiver().description` **must** be the following:
```
'RECEIVER_ID PATH_TO_HRTF_FILE INTERPOLATION'
```
`RECEIVER_ID` must be `SOFA`. `PATH_TO_HRTF_FILE` is the relative or absolute path to your `.sofa` file. `INTERPOLATION` can be `1` or `0`. If `0`, roomsim looks for the HRTF that is closest to the given coordinates, if `1` roomsim performs an interpolation of the neighboring HRTFs. `RECEIVER_ID` and `PATH_TO_HRTF_FILE` are mandatory. `INTERPOLATION` is optional, if not set the default value is `0`.

# Building Roomsim

These instructions will guide you through the steps to build Roomsim on your local machine.

## Building on Windows

You can find a ready-to-go Visual Studio 2017 solution called `roomsim.sln` in the folder named `vs2008ex`. If, for some reason, you want to create a new Visual Studio solution, you have to install CMake. Then, open the Command Prompt window and change directory to `source`.  Type this command to generate a Visual Studio 2015 solution:
```bash
cmake . -G"Visual Studio 14 2015"
```
To know how to generate solutions for other versions of Visual Studio, type `cmake --help`.

## Building on Ubuntu Linux

You can use CMake to generate a Makefile for Roomsim. You can install it by typing the following command in a terminal:
```bash
$ sudo apt-get install cmake
```
To generate the Makefile, change directory to `source` and type the following command:
```bash
$ cmake . -G"Unix Makefiles" -DCMAKE_BULD_TYPE=BUILD_TYPE
```
`BUILD_TYPE` can be `Debug` or `Release`. If `CMAKE_BULD_TYPE` is not set by the user, it defaults to `Release`.

## Building on MacOS

You can use CMake to generate an XCode project. To generate the project, change directory to `source` and type the following command:
```bash
$ cmake . -G"XCode"
```
## Building with MATLAB (Windows)

You can use MATLAB to generate a MEX-file. If you are using a 64-bit version of MATLAB, you have to type the following command in the Command Window:
```matlab
>> make
```
If you are using a 32-bit version of MATLAB, you have to type:
```matlab
>> make32
```
Please note that the build process with `make32` was not tested.

# Known issues

* The 32-bit version built using Debug configuration on Ubuntu Linux crashes due to a segmentation fault cause by `libmysofa`;

# License

* Roomsim - GNU GPL;
* `libmysofa` - See `LICENSE` on the project page;

# Changelog

Here is a list of changes made to the code.

## File "interface.h"

`#include "mysofa.h"` (row 27) was needed to define the macro `__bool_true_false_are_defined`, used by `mstruct.h` (see below).

## File "mstruct.h"

The original Roomsim defined its own bool type (row 46).
```c
#  else
	 typedef int bool;
#    define TRUE  1
#    define FALSE 0
#  endif
```
This was in conflict with the definition of bool in `stdbool.h` used by `libmysofa`. To solve this conflict, a check of `__bool_true_false_are_defined`, defined in `stdbool.h`, was needed.
```c
#  else
#	ifndef __bool_true_false_are_defined
	 typedef int bool;
#    define TRUE  1
#    define FALSE 0
#	else
#	 define TRUE true
#	 define FALSE false
#	endif
#  endif
```

## File "defs.h"

The original Roomsim compared strings with `stricmp` and `strnicmp` functions (row 100). 
```c
/* system-dependent functions */
#if 1
#define stricmp     _stricmp
#define strnicmp    _strnicmp
#endif
```
These functions are Windows-specific. New functions had to be used in order to build Roomsim on other operating systems.
```c
/* system-dependent functions */
#if 1
#	ifdef __GNUC__
#		define stricmp     strcasecmp
#		define strnicmp    strncasecmp
#	else
#		define stricmp     _stricmp
#		define strnicmp    _strnicmp
#	endif	
#endif
```

## File "types.h"

This file describes the `struct` `CSensorDefinition`, that holds all the information about a sensor. The `struct` was expanded in order to manage SOFA HRTF data (row 120).
```c
	/* for SOFA HRTFs */
#	ifdef MYSOFA_H_INCLUDED
	struct MYSOFA_EASY *sofaHandle;
	double *interpResponseDataDouble;
	float  *interpResponseDataFloat;
	float  *delays;
	bool   interpolation;
#	endif
```
`#include "mysofa.h"` (row 27) allowed the use of `MYSOFA_EASY`.

## File "sensor.c"

`#include "mysofa.h"` (row 34) allowed the use of `libmysofa` functions. 

The list of sensors (row 688) was updated to support SOFA HRTFs:
```c
CSensorListItem sensor[] = {
    ...
	{"SOFA",            sensor_SOFA_init            }
	...
};
```

Four new functions were added to the file to manage the SOFA format and compute the HRTFs:
* `sensor_SOFA_init`: it reads SOFA file, allocates memory, fills sensor data;
* `getInterpolation`: it reads `receiver().description` in order to retreive the interpolation flag;
* `sensor_SOFA_probe`: it looks for the HRTF that is closest to the given coordinates;
* `sensor_SOFA_probe_interp`: it computes the HRTF by interpolating the neighboring HRTFs.

Function `SensorGetResponse` was edited in order to obtain the interpolated or non-interpolated HRTFs  (row 67):
```c
#			ifdef MYSOFA_H_INCLUDED
			if (!sensor->interpolation)
			{
#			endif
				if ((idx = sensor->probe.xyz2idx(sensor, xyz)) < 0)
					return 0;
				response->type = SR_IMPULSERESPONSE;
				response->data.impulseresponse = &sensor->responsedata[idx * sensor->nSamples * sensor->nChannels];
				return 1;
#			ifdef MYSOFA_H_INCLUDED
			}
			else
			{
				sensor->probe.xyz2idx(sensor, xyz);
				response->type = SR_IMPULSERESPONSE;
				response->data.impulseresponse = sensor->interpResponseDataDouble;
				return 1;
			}
#			endif
```
Memory allocated by the functions mentioned above must be freed. Function `ClearSensor` was edited to perform these operations (row 854):
```c
#	ifdef MYSOFA_H_INCLUDED
	mysofa_close(item->definition.sofaHandle);
	if (item->definition.interpolation)
	{
		if (item->definition.interpResponseDataDouble)
			MemFree(item->definition.interpResponseDataDouble);
		if (item->definition.interpResponseDataFloat)
			MemFree(item->definition.interpResponseDataFloat);
		if (item->definition.delays)
			MemFree(item->definition.delays);
	}
#	endif
```

Some minor changes not related to SOFA HRTFs were carried out to suppress some compiler warnings:
* Rows 789, 843: `MsgPrintf(msg);` became `MsgPrintf("%s", msg);`
* Row 825: `char msg[256];` became `char msg[512];`

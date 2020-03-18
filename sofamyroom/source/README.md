
# Roomsim

Roomsim is a fast, accurate, and flexible "shoebox" room acoustics simulator that supports both specular and diffuse reflections. The simulator extends the work released by Schimmel et al. by adding the rendering of Binaural Room Impulse Response, BRIR. It supports the AES Spatially Oriented Format for Acoustics (SOFA) file format for storing HRTFs thanks to MySofa library (hereinafter referred as `libmysofa`) and it can export the results of the simulation to a Windows WAVE file thanks to a set of function provided by Andrew Ippoliti.

## Authors
* Steven M. Schimmel, Martin F. Muller, and Norbert Dillier - [Roomsim](https://sourceforge.net/projects/roomsim/)
* Christian Hoene, Isabel C. Patiño Mejía, Alexandru Cacerovschi - [MySofa](https://github.com/hoene/libmysofa)
* Piotr Majdak et al. - [Spatially Oriented Format for Acoustics (SOFA)](https://www.sofaconventions.org/)
*  Andrew Ippoliti - [WAV file writer](http://blog.acipo.com/generating-wave-files-in-c/)

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
roomsim setup
```
`setup` is the name of the text file containing all the roomsim setup parameters structure. A sample of it can be found in `sampleroomsetup.m`.
## Usage with MATLAB (Windows)

A MEX-file for 64-bit MATLAB is available. To run it, type these commands in the Command Window:
```matlab
>> setup = readsetup('sampleroomsetup.m');
>> output = roomsim(setup)
```
## Notes about the setup file

This file stores all the information needed by roomsim to perform the simulation. 

### Notes about the options
Besides those related to the simulation, there are two options related to the output file: 
```matlab
options.outputname			= 'output';           	% name of the output file
options.saveaswav	        = true;                 % format of the ouput file
```
`options.outputname` is the name given to the output file that stores the result of the simulation. The name must not contain the extension of the file, because roomsim takes care of that. `options.saveaswav` tells roomsim to export the results to a Windows WAVE (.wav) file (`options.saveaswav = true;`) or to a binary file (`options.saveaswav = false;`) that can later be opened in MATLAB by using the function `readbrir()`. 

Note that one binary file stores the results of the simulation for every source-receiver couple, while there is one WAVE file for every source-receiver couple.

### Notes about the receiver
The format of the field `receiver().description` is the following:
```
'RECEIVER_ID PATH_TO_HRTF_FILE interp=interp_value norm=norm_value resampling=resampling_value'
```
`RECEIVER_ID` must be `SOFA`. `PATH_TO_HRTF_FILE` is the relative or absolute path to your `.sofa` file. The following values are:
* `interp`: Roomsim can look up the HRTF that is closest to the given coordinates or it can interpolate the neighboring HRTFs to obtain the desired HRTF. `interp_value` can be T[RUE], t[rue], 1 (interpolation is active), or F[ALSE], f[alse], 0 (interpolation is not performed). Words in square brackets are optional, Roomsim just looks for the first character.
*  `norm`: Roomsim can normalize HRTF data. `norm_value` behaves just like  `interp_value`, described above.
*  `resampling`: Roomsim can resample the HRTF data according to  the sampling frequency defined in `options.fs`. `resampling_value` behaves just like `interp_value` and `norm_value`, described above.

All the options described above are optional. If not set, interpolation, normalization and resampling are not performed. Unrecognized options are skipped. If options are repeated, only the last one is used.

### Examples
All the examples proposed below are valid:
```
'SOFA ./data/SOFA/mySofaFile.sofa'
```
```
'SOFA ./data/SOFA/mySofaFile.sofa interp=TRUE norm=0 resampling=t'
```
```
'SOFA ./data/SOFA/mySofaFile.sofa interp=1 norm=f'
```
```
'SOFA ./data/SOFA/mySofaFile.sofa resampling=0 norm=F interp=0'
```

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
Please note that the build process with `make32` is not tested.

# Known issues

* The 32-bit version built using Debug configuration on Ubuntu Linux crashes due to a segmentation fault cause by `libmysofa` (file `src/hdf/dataobject.c`, function `log(" REFERENCE %lu %lX %s\n", reference, dataobject, buffer)`, line 551).

# License

SofaMyRoom is covered by EUPL. 

* Roomsim - GNU GPL;
* `libmysofa` - See `LICENSE` on `libmysofa` project page;

# Changelog

Here is a list of changes made to the code.

## File "global.h"

This file was added in order to declare the global variable `g_fs`, used to store the value of `options.fs` in `sampleroomsetup.m`. The global variable avoided the edit of many definitions and declarations of functions.

## File "interface.h"

`#include "mysofa.h"` was needed to define the macro `__bool_true_false_are_defined`, used by `mstruct.h` (see below).

## File "mstruct.h"

The original Roomsim defined its own bool type.
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
Two options were added to the `struct` `COptions` to handle the name and the format of the output file:
```c
STRUCTBEGIN ( COptions )
    ...
	FIELDSTRING	  ( outputname			)
	FIELDBOOL	  ( saveaswav			)
STRUCTEND ( COptions )
```

## File "defs.h"

The original Roomsim compared strings with `stricmp` and `strnicmp` functions. 
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

This file describes the `struct` `CSensorDefinition`, that holds all the information about a sensor. The `struct` was expanded in order to manage SOFA HRTF data.
```c
	/* for SOFA HRTFs */
#	ifdef MYSOFA_H_INCLUDED
	struct MYSOFA_EASY *sofaHandle;
	double *interpResponseDataDouble;
	float  *interpResponseDataFloat;
	float  *delays; //delays[0] -> left
	bool   interpolation, normalization, resampling;
#	endif
```
`#include "mysofa.h"` allowed the use of `MYSOFA_EASY`.

## File "sensor.c"

`#include "mysofa.h"` allowed the use of `libmysofa` functions. 

`#include "global.h"` allowed the use of the global variable `g_fs`.

The list of sensors was updated to support SOFA HRTFs:
```c
#	ifndef MYSOFA_H_INCLUDED
    {"MIT",             sensor_MIT_init             },
#	else
    {"SOFA",            sensor_SOFA_init             }
#	endif
```

Four new functions were added to the file to manage the SOFA format and compute the HRTFs:
* `sensor_SOFA_init`: it reads SOFA file, allocates memory, fills sensor data;
* `getOptions`: it reads `receiver().description` in order to retreive the options of the sensor;
* `sensor_SOFA_probe`: it looks for the HRTF that is closest to the given coordinates;
* `sensor_SOFA_probe_interp`: it computes the HRTF by interpolating the neighboring HRTFs.

Function `SensorGetResponse` was edited in order to obtain the interpolated or non-interpolated HRTFs:
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
Memory allocated by the functions mentioned above must be freed. Function `ClearSensor` was edited to perform these operations:
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

## File "roomsim.c"

In order to define and use the global variable `g_fs`, the following code was added to the file:
```c
#include "global.h"

#ifdef GLOBAL_FOR_MYSOFA
double g_fs = 0;
#endif
...
CRoomsimInternal *RoomsimInit(const CRoomSetup *pSetup)
{
...
#	ifdef GLOBAL_FOR_MYSOFA
	/* copy setup variable to global variable */
	g_fs = pSetup->options.fs;
#	endif
...
}
```

A check of the sampling rate (field `options.fs` of the setup file) is performed. A sample rate lower than 44100Hz may lead to artifacts in the sound:
```c
/* check simulation sample frequency */
if (pSetup->options.fs < 44100)
{
	sprintf(msg, "warning: the sample rate of the simulation is low, should be at least 44100Hz\n");
	MsgPrintf("%s", msg);
}
```
According to the algorithm on which Roomsim is based, the function `InitSimulationWeights` is called only when the type of the sensor is not `ST_IMPULSERESPONSE`:
```c
/* workaround, not used for impulse response */
if (pSimulation->receiver[r].definition->type == ST_IMPULSERESPONSE)
{
	pSimulation->receiver[r].definition->simulationfrequency = NULL;
	pSimulation->receiver[r].definition->simulationlogweights = NULL;
	pSimulation->receiver[r].definition->nSimulationBands = -1;
}
else
	InitSimulationWeights(pSimulation, pSimulation->receiver[r].definition);
```
## File "main.c"
This file is responsible for saving the output file. Edits were made to support the export of the results to Windows WAVE (.wav) files.
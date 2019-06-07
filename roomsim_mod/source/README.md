# Roomsim

Roomsim is a fast, accurate, and flexible "shoebox" room acoustics simulator that supports both specular and diffuse reflections. It supports the AES SOFA file format for storing HRTFs thanks to `libmysofa` library.


# Installing Roomsim

These instructions will get you a copy of the program up and running on your local machine.

## OS support

Roomsim binaries are available for Windows, Ubuntu Linux and hopefully MacOS, both 32 and 64 bit. 

## Installation on Windows

No futher steps are required to run Roomsim on Windows.

## Installation on Ubuntu Linux

`libmysofa` requires `zlib` library in order to work properly. To install it, open a terminal and enter the following command:
```bash
$ sudo apt-get install zlib1g
```
## Installation on MacOS

Hopefully, no further steps are required to run Roomsim on MacOS.

# Usage

To use Roomsim, type this command:

```bash
roomsim setup output
```
`setup` is a text file containing all the roomsim setup parameters structure. A sample of it can be found in `sampleroomsetup.m`. `output` is the name of the binary file written by roomsim, which contains the result of the simulation. To read this file, use MATLAB `readbrir` function, provided with this project. `setup` is mandatory, while `output` is optional (the default name of the output file is `output.brir`).

## Usage with MATLAB

A MEX-file for 64-bit MATLAB is available. To run it, type this commands in the Command Window:
```matlab
>> setup = readsetup('sampleroomsetup.m');
>> roomsim(setup)
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
`BUILD_TYPE` can be `Debug` or `Release`.

## Building on MacOS

...

## Building with MATLAB

You can use MATLAB to generate a MEX-file. If you are using a 64-bit version of MATLAB, you have to type the following command in the Command Window:
```matlab
>> make
```
If you are using a 32-bit version of MATLAB, you have to type:
```matlab
>> make32
```
Please note that the build using `make32` is not tested by us.

# Changelog

Here is a list of changes made to the code.

## File mstruct.h

The original Roomsim defined its own bool type (row 46).
```c
#  else
	 typedef int bool;
#    define TRUE  1
#    define FALSE 0
#  endif
```
This was in conflict with the definition of bool in `stdbool.h`, used by `libmysofa`. To solve this conflict, a check of `__bool_true_false_are_defined`, defined in `stdbool.h`, was needed.
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

## File defs.h

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
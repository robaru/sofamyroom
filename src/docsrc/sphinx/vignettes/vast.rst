======================================
SofaMyRoom - VAST 
======================================

.. toctree::
	:caption: Contents:
	
VAST structure
-----------------------
.. _here: http://thevastproject.inria.fr/dataset/
Here is reported the organization of the VAST structure. The VAST project is available `here`_.


The VAST dataset is stored with the multilevel MATLAB ``struct``. 
In order to initialize and populate a VAST dataset check the script ``VAST/DemoVAST.m``.

:GlobalParams: - SamplingFrequency
	       - FreqBin
	       - RIRLengthTime
* RIR
 * Left
 * Right
* Room
 * Size
 * FreqRT60
 * GlobalRT60
 * Diffusion
 * Absorption
  * NorthWall
  * WestWall
  * SouthWall
  * EastWall
  * Ceiling
  * Floor
* Receiver
 * Position
 * Orientation
 * HRTF
  * Fs
  * Left
  * Right
* Source
 * Azimuth
 * Elevation
 * Distance
 * AbsolutePos



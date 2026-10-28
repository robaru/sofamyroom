****************************************
SofaMyRoom - Parameters
****************************************

.. contents:: Table of contents

Parameters
======================

Complete list of the parameters accepted in the setup file, in the MATLAB structure and in the Python ``Setup`` (same names and defaults). Each parameter is one ``name = value;`` assignment and a line starting with ``%`` is a comment.

==============================  ==============================  ============================================================
Name                            Type                            Description
==============================  ==============================  ============================================================
room.dimension                  ``[1, 3] double``               Room dimension [m]
room.humidity                   ``double``                      Relative humidity 
room.temperature                ``double``                      Room temperature [°C] 

**Surface Absorption and Diffusion Coefficients**
----------------------------------------------------------------------------------------------------------------------------
room.surface.frequency          ``[1, F] double``               Frequency bands    [Hz]
room.surface.absorption         ``[6, F] double`` [#n_coef]_    Energy absorption coefficients, in [0, 1]
room.surface.diffusion          ``[6, F] double`` [#n_coef]_    Scattering coefficients, in [0, 1]

**Simulation Options**
----------------------------------------------------------------------------------------------------------------------------
options.fs                      ``integer``                     Sampling frequency [Hz] 
options.responseduration        ``double``                      Maximum duration [s]
options.bandsperoctave          ``integer``                     Simulation frequency accuracy 
options.referencefrequency      ``double``                      Reference frequency [Hz] 
options.airabsorption           ``boolean``                     Apply air absorption 
options.distanceattenuation     ``boolean``                     Apply distance attenuation 
options.verbose                 ``boolean``                     Print status messages 

**Specular Reflections**
----------------------------------------------------------------------------------------------------------------------------
options.simulatespecular        ``boolean``                     Simulate specular reflections 
options.reflectionorder         ``[1, 3] integer``              Maximum specular reflection order [x,y,z]

**Diffuse reflections**
----------------------------------------------------------------------------------------------------------------------------
options.simulatediffuse         ``boolean``                     Simulate diffuse reflections 
options.numberofrays            ``integer``                     Number of rays 20*K^2 
options.diffusetimestep         ``double``                      Time resolution in diffuse energy histogram [s]
options.rayenergyfloordB        ``double``                      Ray energy threshold with respect to initial energy [dB]
options.uncorrelatednoise       ``boolean``                     Uncorrelated poisson arrivals
options.seed                    ``integer``                     Seed of the random number generator (optional, 0 = built-in default)

**Output Options**
----------------------------------------------------------------------------------------------------------------------------
options.outputname              ``string``                      Name of the output file 
options.mex_saveaswav           ``boolean`` [#n_matlab]_        Write WAVE files instead of returning the responses

**Source Definitions**
----------------------------------------------------------------------------------------------------------------------------
source(<n> [#n_rec]_).location  ``[1, 3] double`` [#n_check]_   Location of source [m]
source(<n>).orientation         ``[1, 3] double`` [#n_orient]_  Orientation of source [deg]
source(<n>).description         see [#n_recpos]_                Source type

**Receiver Definitions**
----------------------------------------------------------------------------------------------------------------------------
receiver(<n>).location          ``[1, 3] double`` [#n_check]_   Location of receiver [m]
receiver(<n>).orientation       ``[1, 3] double`` [#n_orient]_  Orientation of receiver [deg]
receiver(<n>).description       see [#n_recpos]_, [#n_sofa]_    Receiver type
==============================  ==============================  ============================================================

.. rubric:: Notes
.. [#n_matlab] MATLAB MEX only; ignored by the command line tool, which always writes WAVE files, and by Python, which always returns arrays.
.. [#n_rec] SofaMyRoom can handle more than one source or receiver per run. Substitute <n> with a progressive ``integer`` starting from 1. One response is produced per source-receiver pair (one WAVE file each from the command line).
.. [#n_check] SofaMyRoom does not check if the source or receiver position is within the room. Handle with care.
.. [#n_orient] Defined as (yaw, pitch, roll) in degrees. Run the script ``matlab/plotroom.m`` if you need to visualize your configuration.
.. [#n_recpos] Possible values: omnidirectional, cardioid, subcardioid, hypercardioid, supercardioid, unidirectional, bidirectional, dipole, hemisphere.
.. [#n_sofa] A binaural receiver is ``'SOFA path.sofa [interp=.. norm=.. resampling=.. delay=..]'``, see `the setup file <../README.html#the-setup-file>`__ in the overview.
.. [#n_coef] One row per surface (two per axis, in the order x, y, z), one column per frequency in ``room.surface.frequency``; the values are interpolated linearly to the simulation bands. ``absorption`` is the energy absorption coefficient: each specular reflection multiplies the amplitude by ``sqrt(1 - absorption)``. ``diffusion`` is the scattering coefficient: the specular image source is weighted by ``(1 - diffusion)`` and the diffuse part by ``diffusion``, both in amplitude (the original Roomsim convention, kept for backward compatibility). Both must be in [0, 1]; values above 1 found in some material tables have to be clipped before use, otherwise the setup is rejected. Reflections are positive real and the analytical directivity patterns use the magnitude only.

The options ``options.subsampleaccuracy`` and ``options.highpasscutoff`` of earlier versions are still accepted but have no effect.


Example
===================

``data/sampleroomsetup.txt`` in the repository:

.. literalinclude:: ../../../../data/sampleroomsetup.txt
   :language: matlab

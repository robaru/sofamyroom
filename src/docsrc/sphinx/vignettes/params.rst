****************************************
SofaMyRoom - Parameters 
****************************************

.. contents:: Table of contents

Paramaters
======================

Here a complete list of the paramaters that SofaMyRoom can accept. Notice that SofaMyRoom can also handle the parameters through its standard input. 

A line starting with % is considered a comment and all the parameters must end with a semicolon. 

==============================  ==============================  ============================================================
Name                            Type                            Description
==============================  ==============================  ============================================================
room.dimension                  ``[1, 3] double``               Room dimension [m]
room.humidity                   ``double``                      Relative humidity 
room.temperature                ``double``                      Room temperature [°C] 

**Surface Absorption and Diffusion Coefficients**
----------------------------------------------------------------------------------------------------------------------------
room.surface.frequency          ``[1, F] double``               Frequency bands    [Hz]
room.surface.absorption         ``[6, F] double``               Absorption coefficients
room.surface.diffusion          ``[6, F] double``               Diffusion coefficients

**Simulation Options**
----------------------------------------------------------------------------------------------------------------------------
options.fs                      ``integer``                     Sampling frequency [Hz] 
options.responseduration        ``double``                      Maximum duration [s]
options.bandsperoctave          ``integer``                     Simulation frequency accuracy 
options.referencefrequency      ``double``                      Reference frequency [Hz] 
options.airabsorption           ``boolean``                     Apply air absorption 
options.distanceattenuation     ``boolean``                     Apply distance attenuation 
options.subsampleaccuracy       ``boolean``                     Apply subsample accuracy 
options.highpasscutoff          ``boolean``                     3dB high-pass filter 
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

**Output Options**
----------------------------------------------------------------------------------------------------------------------------
options.outputname              ``string``                      Name of the output file 
options.max_saveaswav           ``boolean`` [#n_matlab]_        Format of the ouput file 

**Source Definitions**
----------------------------------------------------------------------------------------------------------------------------
source(<n> [#n_rec]_).location  ``[1, 3] double`` [#n_check]_   Location of source [m]
source(<n>).orientation         ``[1, 3] double`` [#n_orient]_  Orientation of source [deg]
source(<n>).description         see [#n_recpos]_                Source type

**Source Definitions**
----------------------------------------------------------------------------------------------------------------------------
receiver(<n>).location          ``[1, 3] double``               Location of receiver [m]
receiver(<n>).orientation       ``[1, 3] double`` [#n_orient]_  Orientation of receiver
receiver(<n>).description       see [#n_recpos]_, [#n_sofa]_    Receiver type
==============================  ==============================  ============================================================

.. rubric:: Notes
.. [#n_matlab] Used only in MATLAB MEX.
.. [#n_rec] SofaMyRoom can handle more than one receiver or source per run. Substitute <n> with a progressive ``integer`` to use this feature. Each BRIR is going to be saved into a separate WAVE file.
.. [#n_check] SofaMyRoom does not check if the source or receiver position is within the room. Handle with care.
.. [#n_orient] Defined as (yaw, pitch, roll) in degrees. Run the script ``matlab_helpers/plotroom.m`` if you need to visualize your configuration.
.. [#n_recpos] Possible values: omnidirectional, cardioid, subcardioid, hypercardiod, supercardioid, unidirectional, bidirectional, dipole, hemisphere
.. [#n_sofa] Check the README.md to understand how to handle a SOFA receiver.


Example
===================

Here follows a sample for the parameter file. This example is intended for the native version of SofaMyRoom. If you need the MATLAB 

.. literalinclude:: params_example
   :language: matlab

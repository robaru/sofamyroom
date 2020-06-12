======================================
SofaMyRoom - Parameters 
======================================

.. toctree::
	:caption: Contents:
	
Paramaters
-----------------------

Here a complete list of the paramaters that SofaMyRoom can accept. Notice that SofaMyRoom can also handle the parameters through its standard input. 

A line starting with % is considered a comment and all the parameters must end with a semicolon. 

==============================  ==============================  ==============================
Name				Description			Type
==============================  ==============================  ==============================
room.dimension			Room dimension [m]		double array 
room.humidity			Relative humidity 		double 
room.temperature		Room temperature [°C]		double 

**Surface Absorption and Diffusion Coefficients**
----------------------------------------------------------------------------------------------
room.surface.frequency		Frequency bands	[Hz]		double array
room.surface.absorption		Absorption coefficients		double array
room.surface.diffusion		Diffusion coefficients		double array

**Simulation Options**
----------------------------------------------------------------------------------------------
options.fs			Sampling frequency [Hz]		integer 
options.responseduration	Maximum duration [s]		double
options.bandsperoctave		Simulation frequency accuracy 	integer 
options.referencefrequency	Reference frequency [Hz]	double 
options.airabsorption		Apply air absorption		boolean 
options.distanceattenuation	Apply distance attenuation	boolean 
options.subsampleaccuracy	Apply subsample accuracy	boolean 
options.highpasscutoff		3dB high-pass filter		boolean 
options.verbose			Print status messages		boolean 

**Specular Reflections**
----------------------------------------------------------------------------------------------
options.simulatespecular	Simulate specular reflections	boolean 
options.reflectionorder		Maximum specular

				reflection order [x,y,z]	integer arrray 

**Diffuse reflections**
----------------------------------------------------------------------------------------------
options.simulatediffuse		Simulate diffuse reflections	boolean 
options.numberofrays		Number of rays 20*K^2		integer 
options.diffusetimestep		Time resolution in diffuse
	
				energy histogram [s]		double
options.rayenergyfloordB	Ray energy threshold with 	

				respect to initial energy [dB]	double
options.uncorrelatednoise	Uncorrelated poisson arrivals	boolean 

**Output Options**
----------------------------------------------------------------------------------------------
options.outputname		Name of the output file		string 
options.saveaswav		Format of the ouput file	boolean [#n_matlab]_

**Source Definitions**
----------------------------------------------------------------------------------------------
source(<n> [#n_rec]_).location	Location of source [m]		double array [#n_check]_
source(<n>).orientation		Orientation of source [deg] 	double array [#n_orient]_
source(<n>).description		Source type			[#n_recpos]_

**Source Definitions**
----------------------------------------------------------------------------------------------
receiver(<n>).location		Location of receiver [m]	double array
receiver(<n>).orientation	Orientation of receiver 	double array  [#n_orient]_
receiver(<n>).description	Receiver type			SOFA [#n_sofa]_ and [#n_recpos]_
==============================  ==============================  ==============================

.. rubric:: Notes
.. [#n_matlab] Available only for MATLAB MEX.
.. [#n_rec] SofaMyRoom can handle more than one receiver or source per run. Substitute <n> with a progressive integer to use this feature. Each BRIR is going to be saved into a separate WAVE file.
.. [#n_check] SofaMyRoom does not check if the source or receiver position is within the room. Handle with care.
.. [#n_orient] Defined as (yaw, pitch, roll) in degrees. Run the script ``matlab_helpers/plotroom.m`` if you need to visualize your configuration.
.. [#n_recpos] Possible values: omnidirectional, cardioid, subcardioid, hypercardiod, supercardioid, unidirectional, bidirectional, dipole, hemisphere
.. [#n_sofa] Check the README.md to understand how to handle a SOFA receiver.


Example
-----------------------
Here follows a sample for the parameter file. 

.. literalinclude:: params_test
   :language: matlab

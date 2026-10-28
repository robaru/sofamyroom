**********************************
SofaMyRoom from Python
**********************************

.. contents:: Table of Contents

The ``pysofamyroom`` package drives the SofaMyRoom library in-process. It
takes the same parameters, with the same names and defaults, as the setup
file of the command line tool and the MATLAB structure of the MEX
interface, and it returns the same impulse responses: for a given setup
the three front ends produce identical samples.

==================================
Installation
==================================

.. code-block:: bash

   pip install pysofamyroom

Wheels are published for Linux, macOS and Windows. Installing from the
source distribution (or from a clone of the repository) compiles the C
library and needs a C compiler, CMake and the zlib development files; the
other third-party code is included.

Requires Python 3.10 or later, numpy, scipy and `sofar`_ (used to read the
SOFA source grid when generating VAST datasets).

.. _sofar: https://sofar.readthedocs.io/

==================================
Quick start
==================================

.. code-block:: python

   import pysofamyroom as smr

   setup = smr.Setup(
       room=smr.Room(dimension=(10, 7, 4), humidity=0.42, temperature=20),
       options=smr.Options(fs=44100, responseduration=1.25,
                           simulatediffuse=True, seed=1),
       sources=[smr.Source((8, 2.5, 1.6), (180, 0, 0), "subcardioid")],
       receivers=[smr.Receiver.sofa("MIT_KEMAR_normal_pinna.sofa",
                                    (3, 5, 1.2), interp=True)],
   )
   brirs = smr.simulate(setup)
   brir = brirs[0][0]          # source 0, receiver 0: (n_samples, 2)

``brirs[s][r]`` holds the response of source ``s`` at receiver ``r`` as a
float64 array of shape ``(n_samples, n_channels)``, sampled at
``setup.options.fs``, which is what the MEX file returns in its cell array.
Write it to disk with ``scipy.io.wavfile.write`` or ``soundfile``.

Absorption and diffusion are ``(6, n_bands)`` arrays, one row per surface
in the order ``x = 0``, ``x = Lx``, ``y = 0``, ``y = Ly``, floor, ceiling
(:data:`pysofamyroom.SURFACES`), one column per entry of
``Surface.frequency``. A 1-D array is applied to all six surfaces:

.. code-block:: python

   surface = smr.Surface(frequency=[125, 250, 500, 1000, 2000, 4000, 8000],
                         absorption=[0.02, 0.06, 0.14, 0.37, 0.6, 0.65, 0.65],
                         diffusion=0.1 * np.ones(7))
   room = smr.Room((7.1, 5.1, 3.0), surface=surface)

----------------------------------
Setup files
----------------------------------

Existing setup files are read and written unchanged, so a room described
for the command line tool can be reused and modified in Python:

.. code-block:: python

   setup = smr.read_setup("data/sampleroomsetup.txt")
   setup.sources[0].location = (2.0, 3.5, 1.6)
   setup.options.seed = 42
   brirs = setup.simulate()
   setup.save("my_room.txt")        # for the command line tool

``smr.simulate()`` also accepts the path of a setup file or its text.
Relative paths inside a setup (for example the SOFA file) are resolved
against the current working directory, as in the command line tool;
:meth:`pysofamyroom.Receiver.sofa` stores absolute, quoted paths so that
directories with spaces work.

----------------------------------
Messages, errors and parallelism
----------------------------------

The simulator's progress messages (``options.verbose``) are written to
``sys.stdout``; redirect them with :func:`pysofamyroom.set_log` or
silence them with ``set_log(lambda text: None)``. An invalid setup or a
missing SOFA file raises :class:`pysofamyroom.SofaMyRoomError` with the
simulator's message.

The C library is single-threaded and keeps the loaded SOFA files in a
cache between calls (drop it with :func:`pysofamyroom.clear_sensors`).
Calls from several threads are serialised; to simulate many rooms in
parallel use processes, for example ``concurrent.futures
.ProcessPoolExecutor``.

==================================
VAST datasets
==================================

:mod:`pysofamyroom.vast` is a port of ``matlab/VAST/VASTGeneration.m``.
For every :class:`~pysofamyroom.vast.RoomCondition` the sources are placed
on the measurement grid of the SOFA file, centred on the receiver and
clipped to the room, one response is simulated per grid point and the
responses of all conditions are stacked in one
:class:`~pysofamyroom.vast.VAST` object together with the room, receiver
and source annotations described in :doc:`vignettes/vast`:

.. code-block:: python

   from pysofamyroom import vast

   conditions = [
       vast.RoomCondition("living room", room,
                          receiver_location=(1.5, 1.5, 1.5)),
       vast.RoomCondition("living room, turned", room,
                          receiver_location=(1.5, 1.5, 1.5), receiver_yaw=90),
       vast.RoomCondition("hall", smr.Room((12, 9, 5), surface=surface),
                          receiver_location=(4, 3, 1.5)),
   ]
   dataset = vast.generate(conditions, "MIT_KEMAR_normal_pinna.sofa",
                           fs=16000, duration=1.25)
   dataset.rir_left.shape        # (20000, n_responses)
   dataset.save("vast_kemar.mat")   # loadable in MATLAB: load('vast_kemar.mat')

``workers=4`` simulates the chunks of source positions in four processes;
the dataset is the same whatever the number of workers (in a script, keep
the call under ``if __name__ == "__main__":`` as for any multiprocessing).
The simulation runs at the sampling frequency of the HRTFs and the stored
responses are resampled to ``fs``; ``vast.rt60()`` gives the
Norris-Eyring reverberation time per band used for the annotation. The
complete example is ``examples/vast_demo.py``.

==========================================
Correspondence with the other front ends
==========================================

.. list-table::
   :header-rows: 1
   :widths: 28 32 40

   * - Setup file / MATLAB
     - Python
     - Notes
   * - ``room.*``
     - ``Setup.room`` (``Room``)
     - ``room.surface`` is a ``Surface``
   * - ``options.*``
     - ``Setup.options`` (``Options``)
     - unknown fields are kept in ``Options.extra``
   * - ``source(i).*``
     - ``Setup.sources[i-1]`` (``Source``)
     -
   * - ``receiver(i).*``
     - ``Setup.receivers[i-1]`` (``Receiver``)
     - ``Receiver.sofa()`` builds the SOFA description
   * - ``sofamyroom setup.txt``
     - ``smr.simulate("setup.txt")``
     - arrays instead of WAVE files
   * - ``sofamyroom(param)``
     - ``smr.simulate(setup)``
     - same ``[source][receiver]`` layout as the cell array
   * - ``VASTGeneration(...)``
     - ``vast.generate(...)``
     - several rooms per dataset

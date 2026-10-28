**********************************
C API reference
**********************************

.. contents:: Table of Contents

SofaMyRoom is a C library, ``libroomsim``, with three front ends: the
``sofamyroom`` command line tool, the MATLAB MEX file and the Python
package. Two headers are meant for other programs.

``src/libroomsim/include/capi.h`` is the embedding interface: a handful of
plain C functions that take the setup as text and return the impulse
responses, without exposing the internal structures. It is what
``pysofamyroom`` calls through ``ctypes`` and it is exported from the shared
library built with ``-DSOFAMYROOM_PYTHON=ON``.

``src/libroomsim.h`` is the original Roomsim interface used by ``main.c`` and
``mexmain.c``; it works on the parsed setup structure (``CRoomSetup`` in
``src/libroomsim/include/types.h``) and is the one to use when linking the
static library directly.

All functions must be called from one thread at a time: the simulator keeps
global state (the cache of loaded sensors and the error slot).

==================================
Embedding interface (capi.h)
==================================

.. c:struct:: smr_brir

   One source/receiver impulse response. Samples are stored channel by
   channel: ``sample[j + n_samples*k]`` is sample ``j`` of channel ``k``.

   .. c:member:: double fs

      Sampling frequency in Hz (``options.fs`` of the setup).

   .. c:member:: int n_samples

      Number of samples per channel.

   .. c:member:: int n_channels

      Number of channels (two for a SOFA receiver).

   .. c:member:: const double *sample

      Sample buffer of ``n_samples * n_channels`` doubles, owned by the
      result.

.. c:struct:: smr_result

   Result of one simulation: ``n_sources * n_receivers`` responses, where
   ``brir[r*n_sources + s]`` belongs to source ``s`` and receiver ``r``,
   the same order as the MATLAB cell array and the command line output
   files.

   .. c:member:: int n_sources
   .. c:member:: int n_receivers
   .. c:member:: smr_brir *brir
   .. c:member:: void *handle

      Internal, do not touch.

.. c:type:: void (*smr_log_fn)(const char *text)

   Callback receiving the status messages printed by the simulator.

.. c:function:: const char *smr_version(void)

   Library version string.

.. c:function:: int smr_simulate_text(const char *text, size_t length, smr_result **out)

   Run a simulation from the text of a setup file (same grammar as the
   command line tool). Returns 0 and sets ``*out`` on success, a negative
   value on failure; :c:func:`smr_last_error` then holds the message.
   Relative paths inside the setup (for example the SOFA file) are resolved
   against the current working directory.

.. c:function:: int smr_simulate_file(const char *path, smr_result **out)

   Same as :c:func:`smr_simulate_text`, reading the setup from a file.

.. c:function:: void smr_result_free(smr_result *result)

   Release a result returned by :c:func:`smr_simulate_text` or
   :c:func:`smr_simulate_file`. ``NULL`` is allowed.

.. c:function:: const char *smr_last_error(void)

   Message of the last failed call, empty string if none.

.. c:function:: void smr_clear_sensors(void)

   Drop the cached sources and receivers (SOFA files) kept between calls.

.. c:function:: void smr_set_log(smr_log_fn fn)

   Route status messages to ``fn``; ``NULL`` restores ``stdout``.

Example
----------------------------------

.. code-block:: c

   #include <stdio.h>
   #include "capi.h"

   int main(void)
   {
       smr_result *result;
       if (smr_simulate_file("data/sampleroomsetup.txt", &result) < 0) {
           fprintf(stderr, "%s\n", smr_last_error());
           return 1;
       }
       printf("%d responses of %d samples at %.0f Hz\n",
              result->n_sources * result->n_receivers,
              result->brir[0].n_samples, result->brir[0].fs);
       smr_result_free(result);
       smr_clear_sensors();
       return 0;
   }

==================================
Roomsim interface (libroomsim.h)
==================================

The setup is read with ``ReadSetup()`` and ``LoadCRoomSetup()`` from
``setup.h`` and ``interface.h`` (see ``src/main.c`` for the complete
sequence).

.. c:struct:: BRIR

   One source/receiver impulse response, the layout of :c:struct:`smr_brir`
   with the Roomsim field names ``fs``, ``nChannels``, ``nSamples`` and
   ``sample``.

.. c:function:: void ValidateSetup(const CRoomSetup *pSetup)

   Check the ranges and sizes of the setup (surface coefficients in [0, 1],
   six surfaces, one column per frequency band, ...). An invalid setup
   stops the program with the message ``invalid setup: ...``; in the
   embedded library the error is reported by :c:func:`smr_last_error`
   instead.

.. c:function:: BRIR *Roomsim(const CRoomSetup *pSetup)

   Run the simulation and return an array of ``nSources * nReceivers``
   responses, indexed ``r * nSources + s``, followed by a terminating
   element whose ``sample`` pointer is ``NULL``.

.. c:function:: void ReleaseBRIR(BRIR *brir)

   Free an array returned by :c:func:`Roomsim`.

.. c:function:: void ClearAllSensors(void)

   Free the sources and receivers loaded so far (SOFA files are kept in
   memory between simulations).

**********************************
C API reference
**********************************

.. contents:: Table of Contents

SofaMyRoom is a C library, ``libroomsim``, with three front ends: the
``sofamyroom`` command line tool, the MATLAB MEX file and the Python
package. Two headers are meant for other programs; the reference below is
extracted from their comments.

``src/libroomsim/include/capi.h`` is the embedding interface: a handful of
plain C functions that take the setup as text and return the impulse
responses, without exposing the internal structures. It is what
``pysofamyroom`` calls through ``ctypes`` and it is exported from the shared
library built with ``-DSOFAMYROOM_PYTHON=ON``.

``src/libroomsim.h`` is the original Roomsim interface used by ``main.c`` and
``mexmain.c``; it works on the parsed setup structure (``CRoomSetup`` in
``src/libroomsim/include/types.h``, filled by ``ReadSetup()`` and
``LoadCRoomSetup()``) and is the one to use when linking the static library
directly.

All functions must be called from one thread at a time: the simulator keeps
global state (the cache of loaded sensors and the error slot).

==================================
Embedding interface (capi.h)
==================================

.. c:autodoc:: libroomsim/include/capi.h

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

.. c:autodoc:: libroomsim.h

from libc.stdlib cimport malloc, free
from libcpp cimport bool
from cpython cimport array

cdef extern from "mstruct.h":
    ctypedef struct CSurface:
        int nBands
        const double *frequency
        # Comes from here.
        #define FIELDDYNDOUBLEARRAY2D(na,ncR,ncC) int ncR; int ncC; const double *na;
        #FIELDDYNDOUBLEARRAY2D ( diffusion,  nRowsDiffusion, nColsDiffusion   )
        int nRowsAbsorption
        int nColsAbsorption
        const double *absorption

        int nRowsDiffusion
        int nColsDiffusion
        const double *diffusion

    ctypedef struct CRoom:
        double dimension[3]
        CSurface surface
        double humidity
        double temperature

    ctypedef struct COptions:
        double fs
        double responseduration
        double bandsperoctave
        double referencefrequency
        bool airabsorption
        bool distanceattenuation
        bool subsampleaccuracy
        double highpasscutoff
        bool verbose

        bool simulatespecular
        int reflectionorder[3]

        bool simulatediffuse
        int numberofrays
        double diffusetimestep
        double rayenergyfloordB
        bool uncorrelatednoise
        # const char *outputname

    ctypedef struct CSensor:
        double location[3]
        double orientation[3]
        const char *description

    ctypedef struct CRoomSetup:
        CRoom room
        COptions options
        int nSources
        const CSensor *source
        int nReceivers
        const CSensor *receiver
        # Comes from here
        #define FIELDDYNSTRUCTARRAY(t,na,nc)    int nc; const t *na;
        # FIELDDYNSTRUCTARRAY ( CSensor, source, nSources     )
        # FIELDDYNSTRUCTARRAY ( CSensor, receiver, nReceivers )

    ctypedef struct BRIR:
        double  fs
        int     nChannels
        int     nSamples
        double  *sample

cdef extern from "roomsim.h":
    void print_options(const COptions *options)
    void print_surface(const CSurface *surface)
    void print_room(const CRoom *room)
    void print_setup(const CRoomSetup *setup)
    void print_sensor(const CSensor *sensor)
    BRIR *Roomsim(const CRoomSetup *pSetup)

    #
    # /* allocate memory for BRIR matrix */
    # pSimulation->brir = (BRIR *)MemCalloc(pSimulation->nSources * pSimulation->nReceivers + 1, sizeof(BRIR));
    #
    # /* initialize structure and allocate memory for all source/receiver combinations */
    # for (s=0; s<pSimulation->nSources; s++)
    # {
    #     for (r=0; r<pSimulation->nReceivers; r++)
    #     {
    #         i = r * pSimulation->nSources + s; /* s*pSimulation->nReceivers + r; */
    #
    #         pSimulation->brir[i].fs = pSetup->options.fs;
    #
    #         pSimulation->brir[i].nChannels = pSimulation->receiver[r].definition->nChannels;
    #         pSimulation->brir[i].nSamples  = pSimulation->length;
    #
    #         /* allocate memory for impulse response and set to zero */
    #         pSimulation->brir[i].sample = (double *) MemCalloc(pSimulation->brir[i].nChannels * pSimulation->brir[i].nSamples, sizeof(double));
    #     }
    # }
# In python, BRIR would be a list of multichannel recordings.
# First items in the list: receivers for source 0, then receivers for source 1 etc..
# Other question how are the several channels packed?

def generate(opt_dict: dict):
    cdef BRIR *brir
    cdef CRoomSetup room_setup
    _make_roomsetup(opt_dict, &room_setup)
    # TODO: don't forget to free
    brir = Roomsim(&room_setup)
    # cdef array.array samples = array.array('d', brir.sample)
    # print(brir.sample.data.as_doubles)
    # return samples
    # return brir.nChannels, brir.fs, brir.nSamples


cdef convert_brir_samples(double *brir_prt, int n_samples, int )

cdef void _make_roomsetup(opt_dict: dict, CRoomSetup* setup):
    # setup room, options, sources, receivers.
    _make_room(opt_dict["room"], &setup.room)
    setup.options = opt_dict["options"]
    # setup.source is already an address

    cdef int n_src=len(opt_dict["sources"])
    cdef CSensor *sources = <CSensor *> malloc(n_src * sizeof(CSensor))
    _make_sensors(opt_dict["sources"], sources)
    setup.source = sources

    cdef int n_rcv=len(opt_dict["receivers"])
    cdef CSensor *receivers = <CSensor *> malloc(n_rcv * sizeof(CSensor))
    _make_sensors(opt_dict["receivers"], receivers)
    setup.receiver = receivers

cdef void _make_room(opt_dict: dict, CRoom* room):
    _make_surface(opt_dict["surface"], &room.surface)
    # print_surface(room.surface)
    room.dimension = opt_dict["dimension"]
    room.humidity = opt_dict["humidity"]
    room.temperature = opt_dict["temperature"]
    print_room(room)

cdef void _make_surface(opt_dict: dict, CSurface* surface):
    cdef int nBands = len(opt_dict["frequency"])
    cdef int nColsAbsorption = len(opt_dict["absorption"]) // nBands
    cdef int nColsDiffusion = len(opt_dict["diffusion"]) // nBands

    cdef double for_size
    cdef int freq_size = sizeof(for_size) * len(opt_dict["frequency"])
    cdef int abs_size = sizeof(for_size) * len(opt_dict["absorption"])
    cdef int diff_size = sizeof(for_size) * len(opt_dict["diffusion"])

    cdef double *frequency = <double *> malloc(freq_size)
    cdef double *absorption = <double *> malloc(abs_size)
    cdef double *diffusion = <double *> malloc(diff_size)

    for i in range(len(opt_dict["frequency"])):
        frequency[i] = opt_dict["frequency"][i]

    for i in range(len(opt_dict["absorption"])):
        absorption[i] = opt_dict["absorption"][i]
        diffusion[i] = opt_dict["diffusion"][i]

    surface.frequency = frequency
    surface.nBands = nBands
    surface.absorption = absorption
    surface.nRowsAbsorption = nBands
    surface.nColsAbsorption = nColsAbsorption
    surface.diffusion = diffusion
    surface.nRowsDiffusion = nBands
    surface.nColsDiffusion = nColsDiffusion
    print_surface(surface)

cdef void _make_sensors(opt_dict: list, CSensor sensors[]):
    cdef int n_src=len(opt_dict)
    for i in range(n_src):
        sensors[i].location = opt_dict[i]["location"]
        sensors[i].orientation = opt_dict[i]["orientation"]
        sensors[i].description = opt_dict[i]["description"]

#### Just for testing
def make_roomsetup(opt_dict: dict):
    cdef CRoomSetup room_setup
    _make_roomsetup(opt_dict, &room_setup)
    print_setup(&room_setup)

def make_room(opt_dict: dict):
    cdef CRoom room
    _make_room(opt_dict, &room)
    print_room(&room)

def make_surface(opt_dict: dict):
    cdef CSurface surface
    _make_surface(opt_dict, &surface)
    print_surface(&surface)

def make_options(opt_dict: dict):
    cdef COptions options=opt_dict
    print_options(&options)
    return options

def make_sensors(opt_dict: list):
    cdef int n_src=len(opt_dict)
    cdef CSensor *sensors = <CSensor *> malloc(n_src * sizeof(CSensor))
    _make_sensors(opt_dict, sensors)
    print_sensor(sensors)




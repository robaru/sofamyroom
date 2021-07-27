# Imported in libroomsim.h

cdef extern from "mstruct.h":
    # TODO: correct the path
    """
    TODO: clarify that. We don't need to access it?
    From here: https://cython.readthedocs.io/en/latest/src/userguide/external_C_code.html

    In some cases, you might not need any of the struct’s members,
    in which case you can just put pass in the body of the struct declaration, e.g.:

    cdef extern from "foo.h":
    struct spam:
        pass
    """


    ctypedef struct CSurface:
        int nBands
        const double *frequency
        
        int nRowsAbsorption
        int nColsAbsorption
        const double *absorption

        int nRowsDiffusionint 
        int nColsDiffusion
        const double *diffusion
        # Comes from here.
        #define FIELDDYNDOUBLEARRAY2D(na,ncR,ncC) int ncR; int ncC; const double *na;
        #FIELDDYNDOUBLEARRAY2D ( diffusion,  nRowsDiffusion, nColsDiffusion   )


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

        # Might remove. And probably not const char*
        const char *outputname

    ctypedef struct CSensor:
        double location[3]
        double orientation[3]
        # TODO: find out about strings
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
        pass

cdef extern from "types.h":
    ctypedef struct BRIR:
        pass


cdef extern from "setup.h":


cdef extern from "librooomsim.h":
    void  ValidateSetup   ( const CRoomSetup *pSetup )
    BRIR *Roomsim         ( const CRoomSetup *pSetup )
    void  ReleaseBRIR     ( BRIR *brir )
    void  ClearAllSensors ( void )


"""
One thing I don't understand: where does the LoadCRoomSetup come from? I don't see it defined anywhere in the code. 
This is just reading the txt file into the CRoomSetup, maybe we don't need it, this is not the time consuming part. 
Validate setup is still usable though (but the code is very small, maybe we want to handle our errors, but why not use it.)
--> The main will change. We'll get a CRoomSetup in the input, and return wav (probably)

Everything in one file for now, but eventually we'll move to several files to expose only what we want 
as here https://cython.readthedocs.io/en/latest/src/tutorial/clibraries.html


The only important string is the receiver's description, apprently passing strings is not so simple. 
See this https://cython.readthedocs.io/en/latest/src/tutorial/strings.html.
Get back to tutorials later on: https://cython.readthedocs.io/en/latest/src/tutorial/index.html

Maybe we don't need all the 

"""

## Perso
# TODO: Understand the code behind wav saving. I'm not sure I understand the loops. 
# What if we only retrieve the response in response = Roomsim(&setup);
# In this case, we'll also need to expose the post generation cleaning, and call it from python? 
# We'll have to see how that works.  


# TODO: write a very small example with a struct, a function that expects it and adds the fields
# two simple ints will be fine. And then, complexify.

# TODO: The receivers and the sources are arrays of Struct, which seems complicated to
# pass as arguments, how to do that? We'll see.

## General
# TODO: change filenames, no spaces in filenames.
# TODO: chmod +x of sofamyroom.
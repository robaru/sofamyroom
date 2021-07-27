from libc.stdint cimport uintptr_t
import random
from libc.stdlib cimport malloc, free

cdef extern from "mstruct.h":
    ctypedef struct MyStruct:
        int first
        float second

    ctypedef struct CSensor:
        int first
        float second

    ctypedef struct SensorList:
        int n_sensor
        CSensor source

# cdef class Trial:
#     cdef MyStruct ob

#     def __cinit__(self, first, second):
        

cdef extern from "roomsim.h":
    float real_mul(int first, float second)
    float mul ( const MyStruct *pSetup )
    const MyStruct make_struct(int first, float second)
    void print_sensor(const CSensor *sensor)
    CSensor create_sensor(int nSensor)
    CSensor fill_sensor(CSensor sensors[], int idx, int first, float second)
    void print_sensor(const CSensor *sensor)

def make_sources(*list_of_dicts):
    cdef int n_src=len(list_of_dicts)
    # allocate number * sizeof(double) bytes of memory
    cdef CSensor *source = <CSensor *> malloc(n_src * sizeof(CSensor))
    # TODO: don't forget to free
    # if not source:
    #     raise MemoryError()

    # cdef CSensor source[5]
    # print(len(list_of_dicts))
    for i, d in enumerate(list_of_dicts):
        source[i] = fill_sensor(source, i, d["first"], d["second"])
        print(source[i])
    print_sensor(source)
    # hey.n_sensor = 2
    # print(hey.n_sensor)
    # cdef MyStruct heyhey={"first": first, "second": second}
    # return source
    # sources = create_sensor(len(list_of_dicts))
    # sources[0] = list_of_dicts[0]
    # cdef CSensor source=list_of_dicts
    # static const
    # hey = []
    # for _ in range(len(list_of_dicts)):
    #     cdef CSensor source#[len(list_of_dicts)] # = {d for d in list_of_dicts};
    #     hey.append(source)
    # source[0] = list_of_dicts[0]
    # print_sensor(&source)


# def make_sources(*list_of_dicts):
    # static const CSensor source[] = {
    #     {{500,500,500}, {180,0,0}, "unidirectional"}
    # };
#     # Does that work?
#     cdef static const CSensor source[] = {
#         {d.values() for d in list_of_dicts}
#     };
#
#     src = []
#     for d in list_of_dicts:
#         src.append(make_one_source(**d))
#     make_sources(*src)

def py_mul_struct(int first, float second):
    # heyhey = make_struct(first, second)
    cdef MyStruct heyhey={"first": first, "second": second}
    # This didn't work
    # return mul(&make_struct(first, second))
    return mul(&heyhey)

def py_mul(int first, float second):
    return real_mul(first, second)
from libc.stdint cimport uintptr_t

cdef extern from "mstruct.h":
    ctypedef struct MyStruct:
        int first
        float second

# cdef class Trial:
#     cdef MyStruct ob

#     def __cinit__(self, first, second):
        

cdef extern from "roomsim.h":
    float real_mul(int first, float second)
    float mul ( const MyStruct *pSetup )
    const MyStruct make_struct(int first, float second)

def py_mul_struct(int first, float second):
    heyhey = make_struct(first, second)
    # This didn't work 
    # return mul(&make_struct(first, second))
    return mul(&heyhey)

def py_mul(int first, float second):
    return real_mul(first, second)
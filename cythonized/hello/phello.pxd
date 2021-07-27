cdef extern from "hello.h":
    ctypedef struct Foo:
        int x

    cdef int hello(Foo* what)
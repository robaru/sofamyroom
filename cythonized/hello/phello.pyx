cimport phello

cdef class Foo_c:
    cdef phello.Foo* s

    def hello_fn(self):
        return phello.hello(self.s)

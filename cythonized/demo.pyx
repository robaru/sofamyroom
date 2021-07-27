cdef extern from "string.h":
    cpdef char* strstr(const char *haystack, const char *needle)

cdef char* data = "hfvcakdfagbcffvschvxcdfgccbcfhvgcsnfxjh"

cdef char* pos = strstr(needle='akd', haystack=data)
print(pos is not NULL)
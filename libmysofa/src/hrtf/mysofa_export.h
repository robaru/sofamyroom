
#ifndef MYSOFA_EXPORT_H
#define MYSOFA_EXPORT_H

#ifdef MYSOFA_STATIC_DEFINE
#  define MYSOFA_EXPORT
#  define MYSOFA_NO_EXPORT
#else
#  ifndef MYSOFA_EXPORT
#    ifdef mysofa_static_EXPORTS
        /* We are building this library */
#      define MYSOFA_EXPORT 
#    else
        /* We are using this library */
#      define MYSOFA_EXPORT 
#    endif
#  endif

#  ifndef MYSOFA_NO_EXPORT
#    define MYSOFA_NO_EXPORT 
#  endif
#endif

#ifndef MYSOFA_DEPRECATED
#  define MYSOFA_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef MYSOFA_DEPRECATED_EXPORT
#  define MYSOFA_DEPRECATED_EXPORT MYSOFA_EXPORT MYSOFA_DEPRECATED
#endif

#ifndef MYSOFA_DEPRECATED_NO_EXPORT
#  define MYSOFA_DEPRECATED_NO_EXPORT MYSOFA_NO_EXPORT MYSOFA_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MYSOFA_NO_DEPRECATED
#    define MYSOFA_NO_DEPRECATED
#  endif
#endif

#endif /* MYSOFA_EXPORT_H */

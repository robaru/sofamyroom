/*********************************************************************//**
 * @file capi.h
 * @brief Plain C entry points for embedding SofaMyRoom (used by pysofamyroom).
 **********************************************************************//*
 * Copyright 2026, Roberto Barumerli
 *
 * This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
 * and is licensed under the EUPL-1.2.
 *************************************************************************
 * Only scalars, strings and double pointers cross this boundary, so the
 * library can be driven from ctypes or any other FFI without mirroring
 * the internal structures. All functions must be called from one thread
 * at a time: the simulator keeps global state (sensor cache, error slot).
 *************************************************************************/

#ifndef _CAPI_H_7120938471209384710298
#define _CAPI_H_7120938471209384710298

#include <stddef.h>

#if defined(_WIN32)
#  if defined(SMR_BUILD_DLL)
#    define SMR_API __declspec(dllexport)
#  else
#    define SMR_API
#  endif
#elif defined(__GNUC__)
#  define SMR_API __attribute__((visibility("default")))
#else
#  define SMR_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** One source/receiver impulse response. Samples are stored channel by
 *  channel: sample[j + n_samples*k] is sample j of channel k. */
typedef struct {
	double        fs;
	int           n_samples;
	int           n_channels;
	const double *sample;
} smr_brir;

/** Result of one simulation: n_sources*n_receivers responses, where
 *  brir[r*n_sources + s] belongs to source s and receiver r (same order as
 *  the MATLAB cell array and the CLI output files). */
typedef struct {
	int       n_sources;
	int       n_receivers;
	smr_brir *brir;
	void     *handle;   /**< internal, do not touch */
} smr_result;

/** Callback receiving the status messages printed by the simulator. */
typedef void (*smr_log_fn)(const char *text);

/** Library version string (SOFAMYROOM_VERSION). */
SMR_API const char *smr_version(void);

/** Run a simulation from the text of a setup file (same grammar as the
 *  CLI). Returns 0 and sets *out on success, a negative value on failure;
 *  smr_last_error() then holds the message. */
SMR_API int smr_simulate_text(const char *text, size_t length, smr_result **out);

/** Same as smr_simulate_text(), reading the setup from a file. */
SMR_API int smr_simulate_file(const char *path, smr_result **out);

/** Release a result returned by smr_simulate_*(). NULL is allowed. */
SMR_API void smr_result_free(smr_result *result);

/** Message of the last failed call, empty string if none. */
SMR_API const char *smr_last_error(void);

/** Drop the cached sources/receivers (SOFA files) kept between calls. */
SMR_API void smr_clear_sensors(void);

/** Route status messages to fn; NULL restores stdout. */
SMR_API void smr_set_log(smr_log_fn fn);

#ifdef __cplusplus
}
#endif

#endif /* _CAPI_H_7120938471209384710298 */

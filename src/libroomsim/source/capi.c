/*********************************************************************//**
 * @file capi.c
 * @brief Plain C entry points for embedding SofaMyRoom (used by pysofamyroom).
 **********************************************************************//*
 * Copyright 2026, Roberto Barumerli
 *
 * This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
 * and is licensed under the EUPL-1.2.
 *************************************************************************
 * Compiled with SMR_EMBED: msg.h routes MsgPrintf() to SmrPrintf() and
 * MsgErrorExit() to SmrErrorExit(), which stores the message and unwinds
 * with longjmp() to the smr_simulate_*() call in progress. Buffers that the
 * simulator allocated before the error are not reclaimed; setup errors are
 * raised before any large allocation, so this only matters for failures
 * inside Roomsim() itself.
 *************************************************************************/

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "capi.h"
#include "libroomsim.h"
#include "setup.h"
#include "mem.h"
#include "build.h"

#define SMR_ERROR_LEN 1024
#define SMR_LOG_LEN   4096

static jmp_buf    g_jmp;
static int        g_jmp_valid = 0;
static char       g_error[SMR_ERROR_LEN] = "";
static smr_log_fn g_log = NULL;

/*
 * message hooks referenced by msg.h under SMR_EMBED
 */

void SmrPrintf(const char *fmt, ...)
{
	char buf[SMR_LOG_LEN];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (g_log)
	{
		g_log(buf);
	}
	else
	{
		fputs(buf, stdout);
		fflush(stdout);
	}
}

void SmrErrorExit(const char *msg)
{
	size_t n;

	strncpy(g_error, msg ? msg : "unknown error", SMR_ERROR_LEN - 1);
	g_error[SMR_ERROR_LEN - 1] = '\0';

	/* drop trailing newlines, the Python side formats the message */
	n = strlen(g_error);
	while (n > 0 && (g_error[n-1] == '\n' || g_error[n-1] == '\r'))
		g_error[--n] = '\0';

	if (g_jmp_valid)
		longjmp(g_jmp, 1);

	/* error outside a simulation call: nothing to unwind to */
	fprintf(stderr, "error: %s\n", g_error);
	abort();
}

static void SetError(const char *msg)
{
	strncpy(g_error, msg, SMR_ERROR_LEN - 1);
	g_error[SMR_ERROR_LEN - 1] = '\0';
}

/*
 * memory owned by a loaded CRoomSetup (allocated by LoadCRoomSetup)
 */

static void FreeCRoomSetup(CRoomSetup *setup)
{
	MemFree((void *)setup->room.surface.frequency);
	MemFree((void *)setup->room.surface.absorption);
	MemFree((void *)setup->room.surface.diffusion);
	MemFree((void *)setup->source);
	MemFree((void *)setup->receiver);
	memset(setup, 0, sizeof(*setup));
}

/*
 * simulation from a parsed setup; takes ownership of *filesetup
 */

static int Simulate(CFileSetup *filesetup, smr_result **out)
{
	CRoomSetup  setup;
	BRIR       *brir = NULL;
	smr_result *result;
	int         i, n;

	memset(&setup, 0, sizeof(setup));

	if (setjmp(g_jmp) == 0)
	{
		g_jmp_valid = 1;
		LoadCRoomSetup(&filesetup->root, &setup);
		ValidateSetup(&setup);
		brir = Roomsim(&setup);
		g_jmp_valid = 0;
	}
	else
	{
		/* MsgErrorExit() was called somewhere below */
		g_jmp_valid = 0;
		FreeCRoomSetup(&setup);
		FreeSetup(filesetup);
		return -3;
	}

	n = setup.nSources * setup.nReceivers;

	result = (smr_result *)malloc(sizeof(smr_result));
	if (result)
		result->brir = (smr_brir *)malloc((n > 0 ? n : 1) * sizeof(smr_brir));
	if (!result || !result->brir)
	{
		free(result);
		ReleaseBRIR(brir);
		FreeCRoomSetup(&setup);
		FreeSetup(filesetup);
		SetError("out of memory building the result");
		return -4;
	}

	result->n_sources   = setup.nSources;
	result->n_receivers = setup.nReceivers;
	result->handle      = brir;
	for (i = 0; i < n; i++)
	{
		result->brir[i].fs         = brir[i].fs;
		result->brir[i].n_samples  = brir[i].nSamples;
		result->brir[i].n_channels = brir[i].nChannels;
		result->brir[i].sample     = brir[i].sample;
	}

	FreeCRoomSetup(&setup);
	FreeSetup(filesetup);

	*out = result;
	return 0;
}

/*
 * public API
 */

SMR_API const char *smr_version(void)
{
	return SOFAMYROOM_VERSION;
}

SMR_API int smr_simulate_text(const char *text, size_t length, smr_result **out)
{
	CFileSetup filesetup;
	int        r;

	g_error[0] = '\0';
	if (!text || !out)
	{
		SetError("smr_simulate_text: NULL argument");
		return -1;
	}
	*out = NULL;

	r = ParseSetup(text, length, &filesetup);
	if (r < 0)
	{
		char msg[128];
		snprintf(msg, sizeof(msg), "cannot parse the setup text (code %d)", r);
		SetError(msg);
		return -2;
	}

	return Simulate(&filesetup, out);
}

SMR_API int smr_simulate_file(const char *path, smr_result **out)
{
	CFileSetup filesetup;
	int        r;

	g_error[0] = '\0';
	if (!path || !out)
	{
		SetError("smr_simulate_file: NULL argument");
		return -1;
	}
	*out = NULL;

	r = ReadSetup((char *)path, &filesetup);
	if (r < 0)
	{
		char msg[SMR_ERROR_LEN];
		snprintf(msg, sizeof(msg), "cannot read the setup file '%s' (code %d)", path, r);
		SetError(msg);
		return -2;
	}

	return Simulate(&filesetup, out);
}

SMR_API void smr_result_free(smr_result *result)
{
	if (!result) return;
	if (result->handle)
		ReleaseBRIR((BRIR *)result->handle);
	free(result->brir);
	free(result);
}

SMR_API const char *smr_last_error(void)
{
	return g_error;
}

SMR_API void smr_clear_sensors(void)
{
	ClearAllSensors();
}

SMR_API void smr_set_log(smr_log_fn fn)
{
	g_log = fn;
}

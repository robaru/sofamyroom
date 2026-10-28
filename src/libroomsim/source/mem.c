/*********************************************************************//**
 * @file mem.c
 * @brief Checked memory allocation for the command line build.
 **********************************************************************//*
 * Copyright 2026, Roberto Barumerli
 *
 * This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
 * and is licensed under the EUPL-1.2. In the MEX build mxMalloc/mxCalloc
 * already abort on failure, so these functions are only used otherwise.
 *************************************************************************/

#ifndef MEX

#include <stdio.h>
#include <stdlib.h>

#include "mem.h"

static void MemOutOfMemory(size_t bytes, const char *file, int line)
{
	fprintf(stderr, "error: out of memory allocating %lu bytes (%s:%d)\n",
		(unsigned long)bytes, file, line);
	exit(1);
}

void *MemMallocChecked(size_t size, const char *file, int line)
{
	void *p = malloc(size);
	if (!p && size > 0)
		MemOutOfMemory(size, file, line);
	return p;
}

void *MemCallocChecked(size_t count, size_t size, const char *file, int line)
{
	void *p = calloc(count, size);
	if (!p && count * size > 0)
		MemOutOfMemory(count * size, file, line);
	return p;
}

#endif /* MEX */

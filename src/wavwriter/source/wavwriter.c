/*********************************************************************//**
 * @file wavwriter.c
 * @brief Minimal writer of 32-bit float WAVE files.
 **********************************************************************//*
 * Copyright 2026, Roberto Barumerli
 *
 * This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
 * and is licensed under the European Union Public Licence (EUPL) v1.2.
 * You may obtain a copy of the Licence at
 * https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12
 *
 * See wavwriter.h for the history of this file.
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wavwriter.h"

/* disable warnings about unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4996)
#endif

#define WAV_HEADER_SIZE 44

/* little-endian serialisation, independent of the host byte order */
static unsigned char *put_u16(unsigned char *p, unsigned int v)
{
	p[0] = (unsigned char)(v);
	p[1] = (unsigned char)(v >> 8);
	return p + 2;
}

static unsigned char *put_u32(unsigned char *p, unsigned long v)
{
	p[0] = (unsigned char)(v);
	p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16);
	p[3] = (unsigned char)(v >> 24);
	return p + 4;
}

static unsigned char *put_f32(unsigned char *p, float f)
{
	unsigned long v;
	memcpy(&v, &f, sizeof(float));	/* float is IEEE 754 on every supported platform */
	return put_u32(p, v);
}

Wave makeWave(int sampleRate, int numChannels, long long nFrames)
{
	Wave w;
	w.sampleRate  = sampleRate;
	w.numChannels = numChannels;
	w.nFrames     = nFrames;
	w.index       = 0;
	w.data        = (float *)malloc((size_t)(nFrames * numChannels) * sizeof(float));
	return w;
}

void waveAddSampleFloat(Wave *wave, const float *samples)
{
	if (!wave->data || wave->index >= wave->nFrames) return;
	memcpy(wave->data + wave->index * wave->numChannels, samples, wave->numChannels * sizeof(float));
	wave->index++;
}

int waveToFile(const Wave *wave, const char *filename)
{
	unsigned char header[WAV_HEADER_SIZE], *p = header;
	unsigned char *bytes;
	FILE *file;
	long long i, n = wave->nFrames * wave->numChannels;
	unsigned long dataBytes = (unsigned long)(n * sizeof(float));
	unsigned int blockAlign = wave->numChannels * sizeof(float);
	int ok;

	if (!wave->data) return -1;

	/* RIFF header, "fmt " chunk (IEEE float) and "data" chunk header */
	memcpy(p, "RIFF", 4); p += 4;
	p = put_u32(p, 36 + dataBytes);
	memcpy(p, "WAVE", 4); p += 4;
	memcpy(p, "fmt ", 4); p += 4;
	p = put_u32(p, 16);
	p = put_u16(p, 3);							/* WAVE_FORMAT_IEEE_FLOAT */
	p = put_u16(p, wave->numChannels);
	p = put_u32(p, wave->sampleRate);
	p = put_u32(p, (unsigned long)wave->sampleRate * blockAlign);
	p = put_u16(p, blockAlign);
	p = put_u16(p, 32);
	memcpy(p, "data", 4); p += 4;
	p = put_u32(p, dataBytes);

	bytes = (unsigned char *)malloc(dataBytes);
	if (!bytes) return -1;
	for (i = 0, p = bytes; i < n; i++)
		p = put_f32(p, wave->data[i]);

	file = fopen(filename, "wb");
	if (!file) { free(bytes); return -1; }
	ok = fwrite(header, 1, WAV_HEADER_SIZE, file) == WAV_HEADER_SIZE &&
	     fwrite(bytes, 1, dataBytes, file) == dataBytes;
	ok = (fclose(file) == 0) && ok;
	free(bytes);
	return ok ? 0 : -1;
}

void waveDestroy(Wave *wave)
{
	free(wave->data);
	wave->data = NULL;
}

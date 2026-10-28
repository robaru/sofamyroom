/*********************************************************************//**
 * @file wavwriter.h
 * @brief Minimal writer of 32-bit float WAVE files.
 **********************************************************************//*
 * Copyright 2026, Roberto Barumerli
 *
 * This file is part of SofaMyRoom (https://github.com/robaru/sofamyroom)
 * and is licensed under the European Union Public Licence (EUPL) v1.2.
 * You may obtain a copy of the Licence at
 * https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12
 *
 * Replaces the WAVE writer that earlier versions of SofaMyRoom adapted
 * from Andrew Ippoliti's blog post "Generating WAVE files in C"
 * (http://blog.acipo.com/generating-wave-files-in-c/, 2014), which did not
 * state a licence. The file layout written is the same: a 44-byte RIFF
 * header (format tag 3, IEEE float) followed by interleaved 32-bit samples.
 *************************************************************************/

#ifndef WAVWRITER_H
#define WAVWRITER_H

typedef struct Wave {
	int       sampleRate;	/**< Sampling rate in Hz. */
	int       numChannels;	/**< Number of interleaved channels. */
	long long nFrames;		/**< Number of frames (samples per channel). */
	long long index;		/**< Frames added so far. */
	float     *data;		/**< Interleaved samples, nFrames*numChannels. */
} Wave;

/** Create a wave of nFrames frames; data is NULL if the allocation failed. */
Wave makeWave(int sampleRate, int numChannels, long long nFrames);

/** Append one frame (numChannels samples); frames beyond nFrames are ignored. */
void waveAddSampleFloat(Wave *wave, const float *samples);

/** Write the wave to a file. Returns 0 on success, -1 on failure. */
int waveToFile(const Wave *wave, const char *filename);

/** Release the sample memory. */
void waveDestroy(Wave *wave);

#endif /* WAVWRITER_H */

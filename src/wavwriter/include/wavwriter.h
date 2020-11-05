// -------------------------------------------------- [ Section: Endianness ] -
int isBigEndian();
void reverseEndianness(const long long int size, void* value);
void toBigEndian(const long long int size, void* value);
void toLittleEndian(const long long int size, void* value);

// ------------------------------------------------- [ Section: Wave Header ] -
typedef struct WaveHeader {
	// Riff Wave Header
	char chunkId[4];
	int  chunkSize;
	char format[4];

	// Format Subchunk
	char subChunk1Id[4];
	int  subChunk1Size;
	short int audioFormat;
	short int numChannels;
	int sampleRate;
	int byteRate;
	short int blockAlign;
	short int bitsPerSample;
	//short int extraParamSize;

	// Data Subchunk
	char subChunk2Id[4];
	int  subChunk2Size;

} WaveHeader;

WaveHeader makeWaveHeader(short int const audioFormat, int const sampleRate, short int const numChannels, short int const bitsPerSample);

// -------------------------------------------------------- [ Section: Wave ] -
typedef struct Wave {
	WaveHeader header;
	char* data;
	long long int index;
	long long int size;
	long long int nSamples;
} Wave;

Wave makeWave(short int const audioFormat, int const sampleRate, short int const numChannels, short int const bitsPerSample);
void waveDestroy(Wave* wave);
void waveSetDuration(Wave* wave, const float seconds);
void waveAddSample(Wave* wave, const float* samples);
void waveAddSampleFloat(Wave* wave, const float* samples);
void waveToFile(Wave* wave, const char* filename);
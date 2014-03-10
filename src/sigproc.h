#ifndef SIGPROC_H
#define SIGPROC_H

// frames overlap 31/32
#define OVERLAP 32

// number of samples in a frame
#define FRAMESIZE 2048

#define OVERLAPSAMPLES (FRAMESIZE/OVERLAP)

// expected original frequency of input wave file
#define OFREQ1 44100
#define OFREQ1MULT 8
#define OFREQ2 11025
#define OFREQ2MULT 2

// downsampled frequency
#define DFREQ 5513

// amount of time in a frame
#define FRAME_TLEN ((float) FRAMESIZE / (float) DFREQ)

// minimum and maximum frequency to consider
#define MINFREQ 300
#define MAXFREQ 2000

#define MINCOEF (FRAME_TLEN * MINFREQ)
//#define MAXCOEF (FRAME_TLEN * MAXFREQ)

// normalize power (volume) of a wave file.
// minimum and maximum rms to normalize from.
#define MINRMS 0.1
#define MAXRMS 3



#include "filters.h"

void dump_sf_info(const SF_INFO& sf_info, const char* msg);
/** Read wave from file. */
float * wavread(const char * fn, unsigned int * nsamples, unsigned int * freq);


/** Given a set of filters to use and a wave sample, convert wave sample to
 * bit representation.
 */
unsigned int * wav2bits(vector<Filter> & filters, float * samples,
			unsigned int nsamples, unsigned int freq,
			unsigned int * nbits);


#endif

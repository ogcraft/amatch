#ifndef __utils_h__
#define __utils_h__

////////////////////////////
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <math.h>

#include <sndfile.h>
//#include <armadillo>
#include "amatch.h"

#define ARRAY_LEN(x)  ((int) (sizeof (x) / sizeof (x [0])))
#define MAX(x,y)    ((x) > (y) ? (x) : (y))
#define MIN(x,y)    ((x) < (y) ? (x) : (y))

#ifdef _WIN32
static inline double round(double val)
{    
    return floor(val + 0.5);
}
#endif

#ifdef _WIN32

#include <intrin.h>
inline unsigned int bit_count(unsigned int x)
{
    //return std::bitset<32>(x).count();
    return __popcnt(x);
}
#else
inline unsigned int bit_count(unsigned int x)
{
    //return std::bitset<32>(x).count();
	return __builtin_popcount(x);
}
#endif

/* Open stereo file and place channels in rows of a matrix */
//void spectrogram(const arma::mat& S, int window_sz, int hop_sz, arma::mat& result);
void writebits(unsigned int bits[], unsigned int nbits, const char * fn);
void writekeys(const key_vector keys, const char* fn);

double time_now();

#endif // __utils_h__

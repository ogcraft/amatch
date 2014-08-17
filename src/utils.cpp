#include "utils.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/time.h> 
#endif

double time_now()
{
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    unsigned long long tt = ft.dwHighDateTime;
    tt <<=32;
    tt |= ft.dwLowDateTime;
    tt /=10;
    tt -= 11644473600000000ULL;
    return ( double) tt / 1000000.0;
#else
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
#endif
}

//using namespace arma;
#if 0
/* Open stereo file and place channels in rows of a matrix */
sf_count_t load_stereo_file_into_matrix(SNDFILE *sndfile, sf_count_t nframes, mat& data )
{
  int count = 0;
  /* A frame contains samples for all channels at time t */
  double frame[2];
  /* de-interleave data and place into 2 row matrix */
  for(count = 0; count < nframes; count++)
  {
      /* read interleaved frames one-by-one and de-interleave */
      sf_readf_double( sndfile, frame, (sf_count_t)1);
      data(0,count) = frame[0];
      data(1,count) = frame[1];
  }
  return count;
}
#endif
void write_bits(unsigned int bits[], unsigned int nbits, const char* filename)
{
	FILE * f = fopen(filename, "wb");
	if (!f) {
		printf("Error: Can't open %s for writing.\n", filename);
		exit(1);
	}

	unsigned int t = 32000; // special case to represent packed bits

	fwrite(&t, 4, 1, f);
	fwrite(&nbits, 4, 1, f);

	for (unsigned int i = 0; i < nbits; i++) {
		fwrite(&(bits[i]), 4, 1, f);
	}

	fflush(f);
	fclose(f);
}

void write_fpkeys(const key_vector keys, const char* filename)
{
	FILE * f = fopen(filename, "wb");
	if (!f) {
		printf("Error: Can't open %s for writing.\n", filename);
		exit(1);
	}

	unsigned int t = 32000; // special case to represent packed bits
	unsigned nkeys = keys.size();
	fwrite(&t, 4, 1, f);
	fwrite(&nkeys, 4, 1, f);

	for (unsigned int i = 0; i < nkeys; i++) {
		fwrite(&(keys[i]), 4, 1, f);
	}

	fflush(f);
	fclose(f);
}


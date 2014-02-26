#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include "amatch.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3) {
        printf("Usage: amatch_sample <track filename> <sample filename> [num of secs to match]\n\n");
        return 1;
    }
    double nsecs_to_match = 5.0;
    if(argc >= 4) {
        nsecs_to_match = atof(argv[4]);
    }
    std::string track_fn(argv[1]);
	std::string sample_fn(argv[2]);

	key_vector track_keys;
	key_vector sample_keys;
   
    read_keys_from_file(track_fn, track_keys);
    read_keys_from_file(sample_fn, sample_keys);
    
    printf( "Read fpkeys: %d from: %s\n", track_keys.size(), track_fn.c_str());
    printf( "Read fpkeys: %d from: %s\n", sample_keys.size(), sample_fn.c_str());
	
    size_t nrecords = track_keys.size();
    size_t nsamples = sample_keys.size();
    
    double sample_size_secs = 20.0;
    size_t sample_size_keys = (int) sample_size_secs * keys_in_sec;
    
    printf("Sample size secs: %f keys: %d\n", sample_size_secs, sample_size_keys);

    match_single_sample(track_keys, sample_keys, 
            /* start sec of track */ 4000, /* end sec of track*/ 4600, //(nrecords - nsamples + 1) * sec_per_sample, 
            1.0, sample_size_secs, 
            0, nsecs_to_match);
 
	return 0;
}

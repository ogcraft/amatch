#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include "amatch.h"
#include "utils.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3) {
        printf("Usage: amatch_sample <track filename> <sample filename> [num of secs to match]\n\n");
        return 1;
    }
    double nsecs_to_match = 5.0;
    double start_sec_of_track = 0.1;
    double end_sec_of_track = 0.1;
    
    if(argc >= 4) {
        nsecs_to_match = atof(argv[3]);
    }
    
    double start_t = time_now();

    
    std::string track_fn(argv[1]);
	std::string sample_fn(argv[2]);

	key_vector track_keys;
	key_vector sample_keys;
   
    read_keys_from_file(track_fn, track_keys);
    read_keys_from_file(sample_fn, sample_keys);
    
    printf( "Read fpkeys: %lu from: %s\n", track_keys.size(), track_fn.c_str());
    printf( "Read fpkeys: %lu from: %s\n", sample_keys.size(), sample_fn.c_str());
	
    
    size_t nrecords = track_keys.size();
    size_t nsamples = sample_keys.size();
    
    double sample_size_secs = 20.0;
    size_t sample_size_keys = (int) sample_size_secs * keys_in_sec;
    
    printf("Sample size secs: %f keys: %lu\n", sample_size_secs, sample_size_keys);
    
    end_sec_of_track = (nrecords - nsamples +1) * sec_per_sample;
    printf("Track start sec: %f end sec: %f keys: %lu\n", start_sec_of_track, end_sec_of_track, nrecords);
    
    match_single_sample(track_keys, sample_keys, 
           start_sec_of_track, end_sec_of_track, 
            0.01, sample_size_secs, 
            0, nsecs_to_match);
 
    double end_t = time_now();
    printf("match_single_sample() Elapsed: %f\n", end_t - start_t);
    
    return 0;
}

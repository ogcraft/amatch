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
        printf("Usage: amatch <track filename> <sample filename> [num of tests]\n\n");
        return 1;
    }
    int nsec = 10;
    int max_tryes = 2000;
    int start_sample = 100; // 5 * keys_in_sec;
    if(argc >= 4) {
        max_tryes = atoi(argv[3]);
    }
    if(argc >= 5) {
        nsec = atoi(argv[4]);
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
    size_t sample_size = 20 * keys_in_sec;
    printf("Sample size: %d secs: %f\n", sample_size, sample_size * sec_per_sample); 
    size_t from_end_of_record = nrecords;
    int i = 0;
    int ntest = 0;
    while( from_end_of_record > sample_size ) { 
        printf("\n---------------- Test %d -----------------\n", i);
        printf("Looking for sec: %f (%d) from total: %f\n", start_sample * sec_per_sample, start_sample, nsamples * sec_per_sample);
        
        //key_vector new_sample_keys(sample_keys.begin() + start_sample, sample_keys.end());

        match_single_pass(track_keys, sample_keys, start_sample, nsec, start_sample * sec_per_sample);

        start_sample += sample_size;
        from_end_of_record = (nsamples - 10) - (start_sample + sample_size);
        i++;
        ntest++;
        if(ntest >= max_tryes) {
            break;
        }
    }

	return 0;
}

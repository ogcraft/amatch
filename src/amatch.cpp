#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <map>
#include <math.h>
#include "utils.h"
#include "amatch.h"

#define all(C) C.begin(),C.end()

std::pair<int,int> min_diffs(const diff_vector1& diffs)
{
	std::pair<int,int> m = std::make_pair(10000,0);
	for(diff_vector1::const_iterator it = diffs.begin();
			it != diffs.end(); ++it) {
		if(it->first < m.first) {
			m.first = it->first;
			m.second = it->second;
		}
	}
	return m;
}

int calc_dist(size_t start_pos, const  key_vector& record_keys, const  key_vector& sample_keys,  size_t sample_key_start, int nsec, int shift_sec)
{
    size_t w = nsec * keys_in_sec;
    size_t shift = shift_sec * keys_in_sec;
    int acc = 0;
	for(size_t i = 0; i < w; i++) {
		int d =  bit_count(record_keys[start_pos+i] ^ sample_keys[sample_key_start + i + shift]);
		acc += d;
	}
	return (int)round(w/acc);
}

void calc_dist_double(size_t start_pos, key_const_iterator& track_begin, key_const_iterator& key_end,
                      key_const_iterator& sample_begin, key_const_iterator& sample_end,
                      double nsec, double shift_sec1, double shift_sec2,
                      double& d1, double& d2)
{
	unsigned int w = (unsigned int) (nsec * keys_in_sec);
    unsigned int shift1 = (unsigned int)(shift_sec1 * keys_in_sec);
    unsigned int shift2 = (unsigned int)(shift_sec2 * keys_in_sec);
    unsigned int acc1 = 32;
    unsigned int acc2 = 32;
    //printf("w: %d nsec: %f\n", w, nsec);
    for(size_t i = 0; i < w; i++) {
		acc1 += bit_count(*(track_begin + start_pos+i) ^ *(sample_begin + i + shift1));
		acc2 += bit_count(*(track_begin + start_pos+i) ^ *(sample_begin + i + shift2));
	}
    d1 = w/acc1;
    d2 = w/acc2;
}

size_t read_keys_from_file(const std::string& filename, key_vector& keys)
{
    std::ifstream ifile( filename, std::ios::binary);
	
    uint32_t magic = 0; 
	ifile.read((char*)&magic, sizeof(magic));
	
    uint32_t len = 0;
	ifile.read((char*)&len, sizeof(len));
    
    keys.reserve(len);
    uint32_t n = 0;
    while(ifile.read((char *)&n, sizeof(n))) {	
        keys.push_back(n);
	}

	if(keys.size() != len) {
		printf("Read wrong number of keys: %d != %d\n",keys.size(), len); 
	}

    return len; 
}

size_t match_single_pass(const key_vector& record_keys, const key_vector& sample_keys, size_t sample_key_start, int nsec, double sec)
{
    size_t nrecords = record_keys.size();
    size_t nsamples = sample_keys.size();

    diff_vector1 diffs(nrecords, std::make_pair(33,0));
    size_t max_i =  (nrecords-(nsec+1) * keys_in_sec);
    printf("nrecords: %d nsamples:%d max_i: %d\n", nrecords, nsamples, max_i);
    size_t i = 0;
    for(; i < max_i; i++ ) {
        int d = calc_dist(i, record_keys, sample_keys, sample_key_start, nsec, 0);
        diffs[i] = std::make_pair(d,i);
    }
    
    printf("Collected %d diffs i: %d\n", diffs.size(), i);
    
    std::pair<int,int> dp = min_diffs(diffs);
    
    int m = dp.first;
    int index = dp.second;
    
    printf("Found sec: %f at index: %d  minimum: %d\n", index*sec_per_sample, index, m);
    double diff_in_secs = (sec - index * sec_per_sample);
    printf("Diff secs_diff: %f \n",  diff_in_secs);
    
    std::string isfound = abs(diff_in_secs) > 1 ? "TEST FAILED !!!":"TEST PASSED";
    printf("%s\n", isfound.c_str());
    return 0;
}

void match_double_pass(key_const_iterator& track_begin, key_const_iterator& track_end,
                       key_const_iterator& sample_begin, key_const_iterator& sample_end ,
                       double secs_to_match, double track_ssec, 
                       diff_vector& diffs1, diff_vector& diffs2)
{
    unsigned ntrack_keys = std::distance(track_begin, track_end);
    unsigned nsamples = std::distance(sample_begin, sample_end);
    unsigned keys_to_match = (unsigned) secs_to_match * keys_in_sec;
    size_t max_track_pos = ntrack_keys - keys_to_match + 1;
    printf("max_record_pos: %d\n", max_track_pos);
    diffs1.reserve(max_track_pos);
    diffs2.reserve(max_track_pos);
    std::fill( all(diffs1), std::make_pair(0.0,0));
    std::fill( all(diffs2), std::make_pair(0.0,0));
    printf("Doing: ");
    for(size_t i = 0; i < max_track_pos - 500; i++) {
        if( i % 5000 == 0 ) printf(" %d", i);
        double d1 = 0;
        double d2 = 0;
        calc_dist_double(i, track_begin, track_end, sample_begin, sample_end, 
                      secs_to_match, 0, 10, d1, d2);
        diffs1.push_back(std::make_pair(d1,i));
        diffs2.push_back(std::make_pair(d2,i));
    }
    printf("\n");
    printf("Calculating diffs1:%lu diffs2:%lu\n", diffs1.size(), diffs2.size());
}

bool match_single_sample(const key_vector& track, const key_vector& sample, 
                        double track_ssec, double track_esec, 
                        double sample_ssec, double sample_esec, 
                        double sample_sshift_secs, double secs_to_match)
{
    bool ret = true;
    printf("match_single_sample: track_ssec: %fsec (%d) track_esec: %fsec (%d)\n", track_ssec, track_ssec * keys_in_sec, track_esec, track_esec * keys_in_sec);  
    printf("match_single_sample: sample_ssec: %fsec (%d) sample_esec: %fsec (%d)\n", sample_ssec, sample_ssec * keys_in_sec, sample_esec, sample_esec * keys_in_sec);  
    printf("\n--------------------------------\n"); 
    unsigned sample_spos = (unsigned)((sample_ssec + sample_sshift_secs) * keys_in_sec) + 1;
    unsigned sample_epos = (unsigned)(sample_esec * keys_in_sec);
    printf("Looking for sec: %fsec (%d) from total: %fsec (%d)\n", sample_spos * sec_per_sample, sample_spos, sample_esec, sample_esec * keys_in_sec);
    unsigned track_spos = (unsigned)(track_ssec * keys_in_sec) + 1;
    unsigned track_epos = (unsigned)(track_esec - 10.0) * keys_in_sec;
    printf("track_spos: %d track_epos: %d\n", track_spos, track_epos);
    printf("sample_spos: %d sample_epos: %d\n", sample_spos, sample_epos);
    diff_vector diff1;
    diff_vector diff2;
    match_double_pass(track.begin() + track_spos, track.begin() + track_epos, 
                      sample.begin() + sample_spos, sample.begin() + sample_epos, 
                      secs_to_match, track_ssec, diff1, diff2);

    return ret;
}



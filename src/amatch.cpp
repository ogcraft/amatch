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

std::pair<int,int> min_diffs(const diff_vector& diffs)
{
	std::pair<int,int> m = std::make_pair(10000,0);
	for(diff_vector::const_iterator it = diffs.begin();
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
	return round(acc/w);
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

    diff_vector diffs(nrecords, std::make_pair(33,0));
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


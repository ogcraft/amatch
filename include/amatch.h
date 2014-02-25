#ifndef AMATCH_H
#define AMATCH_H

#include <stdint.h>
#include <string> 
#include <vector> 

typedef std::vector<uint32_t> key_vector;
typedef key_vector::iterator key_iterator;
typedef key_vector::const_iterator key_const_iterator;

typedef std::vector< std::pair<int, int> > diff_vector1;
typedef std::vector< std::pair<double, int> > diff_vector;

#define keys_in_sec 86
#define sec_per_sample ((double) 0.01164) 

unsigned int bit_count(unsigned int x);
int calc_dist(size_t start_pos, const  key_vector& record_keys, const  key_vector& sample_keys,  size_t sample_key_start, int nsec, int shift_sec);
size_t read_keys_from_file(const std::string& filename, key_vector& keys);
size_t match_single_pass(const key_vector& record_keys, const key_vector& sample_keys, size_t sample_key_start, int nsec, double sec);
void dump_snd_file_info(const char* fn);
int generate_fpkeys(const std::string& wavfn); 

bool match_single_sample(const key_vector& track, const key_vector& sample, 
                        double track_ssec, double track_esec, 
                        double sample_ssec, double sample_esec, 
                        double sample_sshift_secs, double secs_to_match);

#endif	/* AMATCH_H */


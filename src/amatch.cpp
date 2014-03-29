#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <map>
#include <math.h>
#include <functional>   
#include <numeric>      // std::accumulate

#include "utils.h"
#include "amatch.h"
#include "sigproc.h"

#define all(C) C.begin(),C.end()
#define PRINTDIFFVEC( v ) {diff_vector::iterator it;for(it=v.begin();it!=v.end();++it){std::cout << it->first <<","<<it->second<<" ";};std::cout<<std::endl;}

std::vector<Filter> prepare_filters();

unsigned g_count = 0;

bool less_by_diff_value(diff_type x, diff_type y)
{
    return x.first < y.first;
}

diff_type use_diff_value(diff_type x, diff_type y)
{
    //printf(" %f,%f", x.first, y.first);
    return std::make_pair(x.first + y.first, 0);
}

class normalize_diff
{
   private:
      double _m; 
   public:
      normalize_diff(double m):_m(m){}
      diff_type operator() ( diff_type& elem ) const 
      {
        elem.first -= _m;
         return elem;
      }
};

void process_diffs(diff_vector& diffs, double& m, unsigned& i)
{
    //avg = mean(in_diffs)
    
    diff_type sum = std::accumulate(all(diffs), std::make_pair(0.0,0), use_diff_value);
    m = sum.first / diffs.size();
    //diffs = map((x)-> x>avg ? x - avg : 0.0, in_diffs)
    std::transform (diffs.begin(), diffs.end(), diffs.begin(), normalize_diff(m) );
    //m,index=findmax(diffs)
    diff_type dm = *std::max_element(all(diffs),less_by_diff_value);
    m = dm.first;
    i = dm.second;
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

double hamming_distance(size_t strack, size_t ssample, int sz, const key_vector& track, const key_vector& sample)
{
    unsigned hd = 0;
    for(int i = 0; i < sz; i++) {
        g_count++;
		hd += bit_count(track[i+strack] ^ sample[i+ssample]); // xor
		//hd += __popcnt(track[i+strack] ^ sample[i+ssample]); // xor
	}
    return hd/sz;
}

void calc_dist_double(	size_t start_pos, size_t track_spos, size_t track_epos, const key_vector& track,
						size_t sample_spos, size_t sample_epos, const key_vector& sample,
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
        uint32_t t = track[track_spos + start_pos+i];
        uint32_t s1 = sample[sample_spos + i + shift1];
        uint32_t s2 = sample[sample_spos + i + shift2];
		acc1 += bit_count( t ^ s1 );
		acc2 += bit_count( t ^ s2 );
	}
    d1 = ((double)w)/acc1;
    d2 = ((double)w)/acc2;
}

size_t read_keys_from_file(const std::string& filename, key_vector& keys)
{
    std::ifstream ifile( filename.c_str(), std::ios::binary);
	
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
		printf("Read wrong number of keys: %d != %d\n",(int)keys.size(), len); 
	}

    return len; 
}

int fpkeys_from_samples(float* samples, unsigned int nsamples, unsigned int freq, key_vector& keys) 
{
	unsigned int nbits;
	std::vector<Filter> filters = prepare_filters();
	unsigned int* bits = wav2bits(filters, samples, nsamples, freq, &nbits);
	keys.insert(keys.end(), bits, bits+nbits);
	return nbits;
}

size_t match_single_pass(const key_vector& record_keys, const key_vector& sample_keys, size_t sample_key_start, int nsec, double sec)
{
    size_t nrecords = record_keys.size();
    size_t nsamples = sample_keys.size();

    diff_vector1 diffs(nrecords, std::make_pair(33,0));
    size_t max_i =  (nrecords-(nsec+1) * keys_in_sec);
    printf("nrecords: %d nsamples:%d max_i: %d\n", (int)nrecords, (int)nsamples, (int)max_i);
    size_t i = 0;
    for(; i < max_i; i++ ) {
        int d = calc_dist(i, record_keys, sample_keys, sample_key_start, nsec, 0);
        diffs[i] = std::make_pair(d,i);
    }
    
    printf("Collected %lu diffs i: %lu\n", diffs.size(), i);
    
    std::pair<int,int> dp = std::make_pair(0,0);//min_diffs(diffs);
    
    int m = dp.first;
    int index = dp.second;
    
    printf("Found sec: %f at index: %d  minimum: %d\n", index*sec_per_sample, index, m);
    double diff_in_secs = (sec - index * sec_per_sample);
    printf("Diff secs_diff: %f \n",  diff_in_secs);
    
    std::string isfound = abs((int)diff_in_secs) > 1 ? "TEST FAILED !!!":"TEST PASSED";
    printf("%s\n", isfound.c_str());
    return 0;
}

void match_double_pass(	size_t track_spos,  size_t track_epos,  const key_vector& track,
						size_t sample_spos,	size_t sample_epos,	const key_vector& sample, 
                        double secs_to_match, double track_ssec, 
                        diff_vector& diffs1, diff_vector& diffs2)
{
    unsigned ntrack_keys = track_epos - track_spos;
    unsigned nsamples = sample_epos - sample_spos;
    unsigned keys_to_match = (unsigned) secs_to_match * keys_in_sec;
    size_t max_track_pos = ntrack_keys - keys_to_match + 1;
    printf("max_record_pos: %lu\n", max_track_pos);
    diffs1.reserve(max_track_pos);
    diffs2.reserve(max_track_pos);
    std::fill( all(diffs1), std::make_pair(0.0,0));
    std::fill( all(diffs2), std::make_pair(0.0,0));
    printf("Doing: ");
    for(size_t i = 0; i < max_track_pos - 500; i++) {
        if( i % 5000 == 0 ) printf(" %lu", i);
        double d1 = 0;
        double d2 = 0;
        calc_dist_double(i, track_spos, track_epos, track, sample_spos, sample_epos, sample, 
                      secs_to_match, 0, 10, d1, d2);
        //printf(" d1:%f,d2:%f", d1, d2);
        diffs1.push_back(std::make_pair(d1,i));
        diffs2.push_back(std::make_pair(d2,i));
    }
    printf("\n");
    printf("Calculating diffs1:%lu diffs2:%lu\n", diffs1.size(), diffs2.size());
    double m1 = 0.0;
    unsigned i1 = 0;
    process_diffs(diffs1, m1, i1);
    double m2 = 0.0;
    unsigned i2 = 0;
    process_diffs(diffs2, m2, i2);
    double sec1 =  (track_ssec + i1 * sec_per_sample);
    double sec2 =  (track_ssec + i2 * sec_per_sample); 
    printf("Found sec1: %f max1: %f i1: %d | sec2: %f max2: %f i2: %d\n",sec1, m1, i1, sec2, m2, i2); 
    int di = i2-i1;
    double ds = (double)di * sec_per_sample;
    printf("Diff in secs: %f(%d) relative: %f\n", ds, di, (10.0-ds)/10.0);
}

//   ti - index in track 
//   si - start index in sample
//   msize - match window size
//   sz - sample size
//   track - track keys vector
//   sample - sample keys vector
double match_sample_simple(size_t ti, size_t si, int msize, const key_vector& track, const key_vector& sample)
{
    int sz = sample.size();
    int max_sample_index =  si + msize; 
    int i = 0;
    double d = 32.0;
    while( i < max_sample_index && i < sz ) {
        d += hamming_distance(ti+i, i, msize, track, sample);
        i += 1;
    } 
    return i/d;
}

//int match_simple( size_t is,  size_t ie, double match_sec,  const key_vector& track, const key_vector& sample) 
//{
//    unsigned msize = (unsigned)(match_sec * keys_in_sec);
//    int len = ie - is;
//    printf("len: %d msize: %d\n", len, msize);
//    double m = 0.0;
//    for (int i = 0; i < len; i++) {
//        double d = match_sample_simple(i+is, 1, msize, track, sample);
//        m += d;
//        double dd =  d/(m/i); 
//        if(dd > 1.7) {
//            printf("Found: index: %d dd:%f\n", i, dd);
//            return i;
//        }
//    }
//    return 0;
//}

int match_simple( size_t is,  size_t ie, double match_sec,  const key_vector& track, const key_vector& sample) 
{
    unsigned msize = (unsigned)(match_sec * keys_in_sec);
    int len = ie - is;
    printf("len: %d msize: %d\n", len, msize);
    double m = 0.0;
    for (int i = 0; i < len; i++) {
        //double d = match_sample_simple(i+is, 1, msize, track, sample);
        double d = msize / hamming_distance(i+is, 0, msize, track, sample);
        m += d;
        double dd =  d/(m/i); 
        if(dd > 1.7) {
            printf("Found: index: %d dd:%f\n", i, dd);
            return i;
        }
    }
    return 0;
}

int match_single_sample(const key_vector& track, const key_vector& sample, 
                        double track_ssec, double track_esec, 
                        double sample_ssec, double sample_esec, 
                        double sample_sshift_secs, double secs_to_match)
{
    //printf("keys_in_sec: %d\n", keys_in_sec);
    printf("match_single_sample: track_ssec: %fsec (%d) track_esec: %fsec (%d)\n", 
        track_ssec, (int)(track_ssec * keys_in_sec), track_esec, (int)(track_esec * keys_in_sec));  
    printf("match_single_sample: sample_ssec: %fsec (%d) sample_esec: %fsec (%d)\n", 
        sample_ssec, (int) (sample_ssec * keys_in_sec), sample_esec, (int) (sample_esec * keys_in_sec));  
    printf("--------------------------------\n"); 
    unsigned sample_spos = (unsigned)((sample_ssec + sample_sshift_secs) * keys_in_sec) + 1;
    unsigned sample_epos = (unsigned)(sample_esec * keys_in_sec);
    unsigned track_spos = (unsigned)(track_ssec * keys_in_sec) + 1;
    unsigned track_epos = (unsigned)(track_esec - 10.0) * keys_in_sec;
    printf("track_spos: %d track_epos: %d total: %d\n", track_spos, track_epos, track_epos - track_spos);
    printf("sample_spos: %d sample_epos: %d total: %d\n", sample_spos, sample_epos, sample_epos - sample_spos);
    diff_vector diff1;
    diff_vector diff2;
    int index = match_simple( track_spos, track_epos, secs_to_match, track, sample); 
    //printf("=== Found sec: %f index:%d g_count:%d\n", index*sec_per_sample, index, g_count);
    return index;
}

bool match_single_sample_double_pass(const key_vector& track, const key_vector& sample, 
                        double track_ssec, double track_esec, 
                        double sample_ssec, double sample_esec, 
                        double sample_sshift_secs, double secs_to_match)
{
    bool ret = true;
    //printf("keys_in_sec: %d\n", keys_in_sec);
    printf("match_single_sample: track_ssec: %fsec (%d) track_esec: %fsec (%d)\n", 
        track_ssec, (int)(track_ssec * keys_in_sec), track_esec, (int)(track_esec * keys_in_sec));  
    printf("match_single_sample: sample_ssec: %fsec (%d) sample_esec: %fsec (%d)\n", 
        sample_ssec, (int) (sample_ssec * keys_in_sec), sample_esec, (int) (sample_esec * keys_in_sec));  
    printf("--------------------------------\n"); 
    unsigned sample_spos = (unsigned)((sample_ssec + sample_sshift_secs) * keys_in_sec) + 1;
    unsigned sample_epos = (unsigned)(sample_esec * keys_in_sec);
    unsigned track_spos = (unsigned)(track_ssec * keys_in_sec) + 1;
    unsigned track_epos = (unsigned)(track_esec - 10.0) * keys_in_sec;
    printf("track_spos: %d track_epos: %d total: %d\n", track_spos, track_epos, track_epos - track_spos);
    printf("sample_spos: %d sample_epos: %d total: %d\n", sample_spos, sample_epos, sample_epos - sample_spos);
    diff_vector diff1;
    diff_vector diff2;
    match_double_pass(track_spos,	track_epos,		track, 
                      sample_spos,	sample_epos,	sample, 
                      secs_to_match, track_ssec, diff1, diff2);
    return ret;
}


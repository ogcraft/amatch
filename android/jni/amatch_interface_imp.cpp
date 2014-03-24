/*
amatch_interface.c: 
*/

#include <jni.h>
#include <stdio.h>
#include <sndfile.h>
#include <sys/time.h> 
#include <assert.h> 
#include <algorithm>

#include "amatch.h"
#include "utils.h"
#include "amatch_interface.h"
#include "opensl_io.h"
#include <android/log.h>
#include "opensl_io.h"

#include "logging.h"

const char* TAG = "Amatch";

const char* amatch_version()
{
	return AMATCH_VER;

}

struct amatch_context
{
	
	OPENSL_STREAM  *p;
	key_vector track_keys;
	key_vector rec_keys;
	int sec_to_record;
	int sec_to_match;
	float  record_buffer[2][NSAMPLES];
	int recording_buff_index;
	int search_buff_index;
	int samps_collected[2];
	long start_rec_time[2];
	amatch_context();
};

static amatch_context _ctx;

amatch_context::amatch_context()
	:p(NULL),
	 track_keys(),
	 rec_keys(),
	 sec_to_record(SEC_TO_RECORD),
	 sec_to_match(SEC_TO_MATCH),
	 recording_buff_index(0),
	 search_buff_index(1)
{
	samps_collected[0]=0;
	samps_collected[1]=0;
	start_rec_time[0] =0;
	start_rec_time[1] =0;
}

amatch_context* get_amatch_context()
{
	return &(_ctx);
}

//{
//	if(_ctx == NULL) {
//		_ctx = new amatch_context();
//		LOGD(TAG,"Creating new amactch_context: 0x%08x\n", _ctx);
//	}
//	return _ctx;
//}

//bool init_amatch_context()
//{
//	_ctx = new amatch_context();
//	return _ctx != NULL;
//}

//void destroy_amatch_context()
//{
//	delete _ctx;
//	_ctx = NULL;
//}

bool open_audio_device()
{
	_ctx.p = android_OpenAudioDevice(SR,1,1,BUFFERFRAMES);
	return _ctx.p != NULL;
}

size_t read_track_fpkeys(const char* fn)
{
	size_t n = read_keys_from_file(fn, _ctx.track_keys);
	assert(n == _ctx.track_keys.size());
	LOGD(TAG,"Read track fpkeys: %lu (%f secs) from: %s\n", _ctx.track_keys.size(), sec_per_sample*(_ctx.track_keys.size()) , fn);
	return n;
}

int recording_buff_index()
{
	return _ctx.recording_buff_index;
}

int searching_buff_index()
{
	return _ctx.search_buff_index;
}

void swap_rec_buff_indexes()
{
	LOGD(TAG,"Swaping %d -> %d\n", _ctx.recording_buff_index, _ctx.search_buff_index);
	std::swap(_ctx.recording_buff_index, _ctx.search_buff_index);
}

long get_start_rec_time()
{
	return _ctx.start_rec_time[recording_buff_index()];
}

void skip_samples(int nsamples)
{
	float  inbuffer[VECSAMPS_MONO]={0.0};
	for(int n = 0; n < 25; n++) {
		read_audio_in(inbuffer, VECSAMPS_MONO);
	}
}

void close_audo_device()
{
		if(_ctx.p) android_CloseAudioDevice(_ctx.p);
		_ctx.p = NULL;
}

int read_audio_in(float inbuffer[], size_t nsamples)
{
	int samps = android_AudioIn(_ctx.p, inbuffer, nsamples);
	return samps;
}

int collect_rec_samples(long start_rec_time)
{
	float  inbuffer[VECSAMPS_MONO]={0.0};

	int buff_index = recording_buff_index();
	LOGD(TAG, "collect_rec_samples(%d): buffer: %d\n", start_rec_time, buff_index);
	_ctx.samps_collected[buff_index] = 0;

	_ctx.start_rec_time[buff_index] = start_rec_time;

	long max_samps = _ctx.sec_to_record * SR;

	for(int n = 0; n < max_samps/VECSAMPS_MONO; n++) {
		int samps = read_audio_in(inbuffer,VECSAMPS_MONO);
		//for(int i = 0; i < samps; i++) { printf("%f ", inbuffer[i]); }
		//printf("\nn:%d collected:%d samps: %d\n", n, samps_collected, samps);
		std::copy(&inbuffer[0],&inbuffer[samps], &(_ctx.record_buffer[buff_index][_ctx.samps_collected[buff_index]]));
		_ctx.samps_collected[buff_index] += samps;
		if(_ctx.samps_collected[buff_index] >= max_samps) {
			break;
		}
	}
	LOGD(TAG,"Collected samps: %d\n", _ctx.samps_collected[buff_index]);
	swap_rec_buff_indexes();
	return _ctx.samps_collected[buff_index];
}

int generate_fp_keys_from_in()
{
	LOGD(TAG,"generate_fp_keys_from_in()");
	int buff_index = searching_buff_index();
	_ctx.rec_keys.clear();
	//for(int i = 1000; i < 1100/*SR*/; i++) { printf("samps: %d %f\n", i, samplebuffer[searching_buff_index][i]); }
	fpkeys_from_samples(_ctx.record_buffer[buff_index], _ctx.samps_collected[buff_index],
						SR, _ctx.rec_keys);
	LOGD(TAG,"******** Generated samps keys: %d\n", _ctx.rec_keys.size());
	return _ctx.rec_keys.size();
}
		
int match_sample()
{
	LOGD(TAG,"match_sample()");
	double nsecs_to_match = (double) _ctx.sec_to_match;
	double start_sec_of_track = 0.1;
	double end_sec_of_track = 0.1;
	size_t sample_size_keys = _ctx.rec_keys.size();
	double sample_size_secs = sample_size_keys * sec_per_sample;
	size_t nrecords = _ctx.track_keys.size();
	end_sec_of_track = (nrecords - sample_size_keys +1) * sec_per_sample;

	int found_index = match_single_sample(_ctx.track_keys, _ctx.rec_keys,
			start_sec_of_track, end_sec_of_track, 
			0, sample_size_secs, 
			0, nsecs_to_match);

	LOGD(TAG,"match_sample(): Found: %d\n", found_index);
	return found_index;
}

#if 0
static int on;
void start_process() {
  OPENSL_STREAM  *p;
  int samps, i, j;
  float  inbuffer[VECSAMPS_MONO], outbuffer[VECSAMPS_STEREO];
  p = android_OpenAudioDevice(SR,1,2,BUFFERFRAMES);
  if(p == NULL) return; 
  on = 1;
  while(on) {
   samps = android_AudioIn(p,inbuffer,VECSAMPS_MONO);
   for(i = 0, j=0; i < samps; i++, j+=2)
     outbuffer[j] = outbuffer[j+1] = inbuffer[i];
   android_AudioOut(p,outbuffer,samps*2); 
  }  
  android_CloseAudioDevice(p);
}

void stop_process(){
  on = 0;
}
#endif

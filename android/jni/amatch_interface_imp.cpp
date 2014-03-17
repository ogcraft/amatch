/*
amatch_interface.c: 
*/

#include <jni.h>
#include <stdio.h>
#include <sndfile.h>
#include <sys/time.h> 
#include <assert.h> 

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
	return "1.1";

}
struct amatch_context
{
	
	OPENSL_STREAM  *p;
	key_vector track_keys;
	key_vector rec_keys;
	float  record_buffer[NSAMPLES];

	amatch_context();
};

static amatch_context* _ctx;

amatch_context::amatch_context()
	:p(NULL)
{
}

amatch_context* get_amatch_context()
{
	if(_ctx == NULL) {
		_ctx = new amatch_context();
		LOGD(TAG,"Creating new amactch_context: 0x%08x\n", _ctx);
	}
	return _ctx;
}

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
	amatch_context* ctx = get_amatch_context();
	ctx->p = android_OpenAudioDevice(SR,1,1,BUFFERFRAMES);
	return ctx->p != NULL;
}

size_t read_track_fpkeys(const char* fn)
{
	amatch_context* ctx = get_amatch_context();
    size_t n = read_keys_from_file(fn, ctx->track_keys);
	assert(n == ctx->track_keys.size());
	LOGD(TAG,"Read track fpkeys: %lu (%f secs) from: %s\n", ctx->track_keys.size(), sec_per_sample*(ctx->track_keys.size()) , fn);
	return n;
}

void skip_samples(int nsamples)
{
	get_amatch_context();
	float  inbuffer[VECSAMPS_MONO]={0.0};
	for(int n = 0; n < 25; n++) {
		read_audio_in(inbuffer, VECSAMPS_MONO);
	}
}

void close_audo_device()
{
	amatch_context* ctx = get_amatch_context();
	if(ctx) {
		android_CloseAudioDevice(ctx->p);
		ctx->p = NULL;
	}
}

int read_audio_in(float inbuffer[], size_t nsamples)
{
	amatch_context* ctx = get_amatch_context();
	int samps = android_AudioIn(ctx->p, inbuffer, nsamples);
	return samps;
}

int generate_fp_keys_from_in()
{
	LOGD(TAG,"generate_fp_keys_from_in()");
	amatch_context* ctx = get_amatch_context();
	LOGD(TAG,"generate_fp_keys_from_in() ctx: 0x%08x", ctx);
	size_t samps_collected = 0;
	float  inbuffer[VECSAMPS_MONO]={0.0};
	float  samplebuffer[NSAMPLES]={0.0};
	ctx->rec_keys.clear();
	LOGD(TAG,"1");
	for(int n = 0; n < (30*SR)/VECSAMPS_MONO; n++) {
		int samps = read_audio_in(inbuffer,VECSAMPS_MONO);
		//for(int i = 0; i < samps; i++) { printf("%f ", inbuffer[i]); }
		//printf("\nn:%d collected:%d samps: %d\n", n, samps_collected, samps);
		std::copy(&inbuffer[0],&inbuffer[samps], &samplebuffer[samps_collected]);
		samps_collected += samps;
		if(samps_collected >= SR*20) {
			break;
		}
	}  
	LOGD(TAG,"Collected samps: %d\n", samps_collected);
	//for(int i = 1000; i < 1100/*SR*/; i++) { printf("samps: %d %f\n", i, samplebuffer[i]); }
	fpkeys_from_samples(samplebuffer, samps_collected, SR, ctx->rec_keys);
	LOGD(TAG,"******** Generated samps keys: %d\n", ctx->rec_keys.size());
	return ctx->rec_keys.size();
}
		
int match_sample()
{
	LOGD(TAG,"match_sample()");
	amatch_context* ctx = get_amatch_context();
	double nsecs_to_match = 5.0;
	double start_sec_of_track = 0.1;
	double end_sec_of_track = 0.1;
	size_t sample_size_keys = ctx->rec_keys.size();
	double sample_size_secs = sample_size_keys * sec_per_sample;
	size_t nrecords = ctx->track_keys.size();
	end_sec_of_track = (nrecords - sample_size_keys +1) * sec_per_sample;

	int found_index = match_single_sample(ctx->track_keys, ctx->rec_keys,
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

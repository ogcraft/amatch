/*
amatch_interface.c: 
*/

#include <jni.h>
#include <stdio.h>
#include <sndfile.h>
#include <sndfile.hh>
#include <sys/time.h> 
#include <assert.h> 
#include <algorithm>

#include <boost/circular_buffer.hpp>
#include "amatch.h"
#include "utils.h"
#include "amatch_interface.h"
#include <android/log.h>
#include "opensl_io4.h"

#include "logging.h"

const char* TAG = "Amatch";

#define CONV16BIT 32768
#define CONVMYFLT (1./32768.)

const char* amatch_version()
{
	return AMATCH_VER;

}

int nrecsamples()
{
	return NRECSAMPLES;
}

double num_sec_to_record()
{
	return (double) SEC_TO_RECORD;
}

int get_sample_rate()
{
	return SR;
}

double delay_per_sec()
{
	return (double)DELAY_PER_SEC;
}

struct amatch_context
{
	
	OPENSL_STREAM  *p;
	key_vector track_keys;
	key_vector rec_keys;
	//float  record_buffer[NSAMPLES];
	boost::circular_buffer<float> record_buffer;
	amatch_context();
};

static amatch_context _ctx;

amatch_context::amatch_context()
	:p(NULL),track_keys(),rec_keys(),record_buffer(NRECSAMPLES)
{

}

key_vector& recorded_keys()
{
	return _ctx.rec_keys;
}

boost::circular_buffer<float>& get_record_buffer()
{
	return _ctx.record_buffer;
}

void get_recorded_samples(float p[])
{
	std::copy(_ctx.record_buffer.begin(), _ctx.record_buffer.end(), p);
}

void put_recorded_samples(short p[], int size)
{
	for(int i = 0; i < size; i++) {
		float f = ((float) p[i]) * CONVMYFLT;
		_ctx.record_buffer.push_back(f);
	}
}

int get_recorded_samples_size()
{
	return _ctx.record_buffer.size();
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
	LOGD(TAG,"open_audio_device(): 0x%p\n", (void*)_ctx.p);
	return _ctx.p != NULL;
}

size_t read_track_fpkeys(const char* fn)
{
	size_t n = read_keys_from_file(fn, _ctx.track_keys);
	assert(n == _ctx.track_keys.size());
	LOGD(TAG,"Read track fpkeys: %u (%f secs) from: %s\n", _ctx.track_keys.size(), sec_per_sample*(_ctx.track_keys.size()) , fn);
	return n;
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
		LOGD(TAG,"close_audio_device()\n");
		if(_ctx.p) android_CloseAudioDevice(_ctx.p);

		_ctx.p = NULL;
}

int read_audio_in(float inbuffer[], size_t nsamples)
{
	int samps = android_AudioIn(_ctx.p, inbuffer, nsamples);
	return samps;
}

int recording()
{
	//LOGD(TAG,"recording()");
	collect_AudioIn(_ctx.p, _ctx.record_buffer);
	//LOGD(TAG,"recording(): Collected samps: %d\n", _ctx.record_buffer.size());
	return _ctx.record_buffer.size();

}

int recording1()
{
	LOGD(TAG,"recording()");

	size_t samps_collected = 0;

	float  inbuffer[VECSAMPS_MONO]={0.0};

	for(int n = 0; n < (SEC_TO_RECORD*SR)/VECSAMPS_MONO; n++) {
		int samps = read_audio_in(inbuffer,VECSAMPS_MONO);
		//for(int i = 0; i < 10; i++) { LOGD(TAG,"%f ", inbuffer[i]); }
		//LOGD(TAG,"\nn:%d collected:%d samps: %d\n", n, samps_collected, samps);
		std::copy(&inbuffer[0],&inbuffer[samps], _ctx.record_buffer.end() + samps_collected);
		samps_collected += samps;
		if(samps_collected >= SR*SEC_TO_RECORD) {
				break;
		}
	}
	LOGD(TAG,"recording(): Collected samps: %d\n", samps_collected);
	return samps_collected;

}

int generate_fp_keys_from_in()
{
	LOGD(TAG,"generate_fp_keys_from_in() from %d samples", _ctx.record_buffer.size());
	size_t samps_to_match = _ctx.record_buffer.size();
	_ctx.rec_keys.clear();
	//for(int i = 1000; i < 1100/*SR*/; i++) { printf("samps: %d %f\n", i, samplebuffer[i]); }
	fpkeys_from_samples(_ctx.record_buffer.linearize(), samps_to_match, SR, _ctx.rec_keys);
	LOGD(TAG,"******** Generated samps keys: %d\n", _ctx.rec_keys.size());
	return _ctx.rec_keys.size();
}
		
int match_sample()
{
	
	double nsecs_to_match = SEC_TO_MATCH;
	double start_sec_of_track = 0.1;
	//double end_sec_of_track = 0.1;
	double end_sec_of_track = SEC_TO_RECORD * 3;
	size_t sample_size_keys = _ctx.rec_keys.size();
	double sample_size_secs = sample_size_keys * sec_per_sample;
	size_t nrecords = _ctx.track_keys.size();
	LOGD(TAG,"match_sample() rec_keys: %d nrecords:%d\n", sample_size_keys, nrecords);

	end_sec_of_track = (nrecords - sample_size_keys +1) * sec_per_sample;

	int found_index = match_single_sample(_ctx.track_keys, _ctx.rec_keys,
			start_sec_of_track, end_sec_of_track, 
			0, sample_size_secs, 
			0, nsecs_to_match);

	LOGD(TAG,"match_sample(): Found: %d\n", found_index);
	return found_index;
}

void start_playing()
{
	setPlayerState(_ctx.p, SL_PLAYSTATE_PLAYING);
}
void stop_playing()
{
	setPlayerState(_ctx.p, SL_PLAYSTATE_STOPPED);
}
void pause_playing()
{
	setPlayerState(_ctx.p, SL_PLAYSTATE_PAUSED);
}

void start_recording()
{
	setRecorderState(_ctx.p, SL_RECORDSTATE_RECORDING);
}
void stop_recording()
{
	setRecorderState(_ctx.p, SL_RECORDSTATE_STOPPED);
}
void pause_recording()
{
	setRecorderState(_ctx.p, SL_RECORDSTATE_PAUSED);
}
int player_state()
{
	return (int) getPlayState(_ctx.p);
}
int recorder_state()
{
	return getRecorderState(_ctx.p);
}



void create_file (const char * fname, const float* data, int size)
{	
	SNDFILE	*file ;
	SF_INFO	sfinfo ;

	LOGD(TAG,"Creating file named '%s' data size: %d\n", fname, size) ;
	memset (&sfinfo, 0, sizeof (sfinfo)) ;


	sfinfo.samplerate	= SR ;
	sfinfo.frames		= size ;
	sfinfo.channels		= 1 ;
	sfinfo.format		= (SF_FORMAT_WAV | SF_FORMAT_PCM_16) ;
	LOGD(TAG,"Open file %s\n", fname);

	if (! (file = sf_open(fname, SFM_WRITE, &sfinfo))) {
		LOGD(TAG,"Error : Not able to open output file.\n") ;
			return;
	}

	if( sf_write_float(file, data, (sf_count_t) sfinfo.frames ) != sfinfo.frames ) {
		LOGD(TAG, "Write error: %s\n", sf_strerror (file)) ;
	}
	LOGD(TAG, "Succesfully writen %s\n", fname);
	sf_close (file) ;
	return;
}

void write_recorded_as_file (const char * fname)
{
	LOGD(TAG,"write_recorded_as_file(): %s\n", fname);
	create_file (fname, _ctx.record_buffer.linearize(), _ctx.record_buffer.size());
}

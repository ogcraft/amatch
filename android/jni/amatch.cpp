#include <jni.h>
#include <stdio.h>
#include <sndfile.h>
#include <sys/time.h> 

#include "amatch.h"
#include "utils.h"
#include "amatch_interface.h"
#include "opensl_io.h"
#if 1

void writekeys(const key_vector keys, const char* fn);

#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
#define SR 11025
#define NSAMPLES (SR * 30)

int main(int argc, char** argv)
{
	LOGD("amatch","Run : %s\n", argv[0]);
	char  sndflile_ver [128] ;
	sf_command (NULL, SFC_GET_LIB_VERSION, sndflile_ver, sizeof (sndflile_ver)) ;
	printf("Sndfile version: %s\n",sndflile_ver);
	printf("amatch android: version 1.0\n\n"); 
	
	const char* track_keys_fn = "/storage/sdcard0/asearch/monstr-eng1-11025.fpkeys";
	const char* sample_keys_fn = "/storage/sdcard0/asearch/monstr-sample.fpkeys";
	key_vector track_keys;
    read_keys_from_file(track_keys_fn, track_keys);
    printf( "Read track fpkeys: %lu (%f secs) from: %s\n", track_keys.size(), sec_per_sample*track_keys.size() , track_keys_fn);

	OPENSL_STREAM  *p = NULL;
	int samps = 0;
	
	float  inbuffer[VECSAMPS_MONO]={0.0};
	float  samplebuffer[NSAMPLES]={0.0};
	p = android_OpenAudioDevice(SR,1,2,BUFFERFRAMES);
	
    //struct timeval start_rec_tv;
    //gettimeofday(&start_rec_tv,0);
    //return tv.tv_sec + (tv.tv_usec / 1000000.0);
	if(p == NULL) {
		printf("p == NULL\n");
		return 0;
	}
	
	for(int n = 0; n < 25; n++) {
		samps = android_AudioIn(p,inbuffer,VECSAMPS_MONO);
	}  
    int tryes = 100;	
	while(tryes--) {	
		printf("START RECORDING...\n");	
		double start_rec = time_now();  
		size_t samps_collected = 0;
		for(int n = 0; n < (30*SR)/VECSAMPS_MONO; n++) {
			samps = android_AudioIn(p,inbuffer,VECSAMPS_MONO);
			//for(int i = 0; i < samps; i++) { printf("%f ", inbuffer[i]); }
			//printf("\nn:%d collected:%d samps: %d\n", n, samps_collected, samps);
			std::copy(&inbuffer[0],&inbuffer[samps], &samplebuffer[samps_collected]);
			samps_collected += samps;
			if(samps_collected >= SR*20) {
				break;
			}
		}  

		printf("Collected samps: %d\n", samps_collected);

		//for(int i = 0; i < SR; i++) { printf("samps: %d %f\n", i, samplebuffer[i]); }
		key_vector sample_keys;
		fpkeys_from_samples(inbuffer, samps_collected, SR, sample_keys); 

		printf("Generated samps keys: %d\n", sample_keys.size());
		//for(int i = 0; i < sample_keys.size(); i++) { printf("K: 0x%08x\n", sample_keys[i]); }

		//writekeys(sample_keys, sample_keys_fn);

		double nsecs_to_match = 5.0;
		double start_sec_of_track = 0.1;
		double end_sec_of_track = 0.1;
		size_t sample_size_keys = sample_keys.size();
		double sample_size_secs = sample_size_keys * sec_per_sample;
		size_t nrecords = track_keys.size();
		end_sec_of_track = (nrecords - sample_size_keys +1) * sec_per_sample;

		int found_index = match_single_sample(track_keys, sample_keys, 
				start_sec_of_track, end_sec_of_track, 
				0, sample_size_secs, 
				0, nsecs_to_match);
		float found_sec = found_index * sec_per_sample;

		//struct timeval now_tv;
		//gettimeofday(&now_tv,0);

		double now = time_now();  

		float corrected_sec = found_sec + now - start_rec;
		if(found_index > 10 ) {
			printf("\n!!!!!!!!   Playing:     %f     sec | Found %d %f\n", corrected_sec, found_index, found_sec);
		}
		sleep(3);
	}
	android_CloseAudioDevice(p);

	return 0;
}

#else

#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
//#define VECSAMPS_STEREO 128
#define SR 11025

int main() 
{
  OPENSL_STREAM  *p;
  int samps, i, j;
  float  inbuffer[VECSAMPS_MONO]; //, outbuffer[VECSAMPS_STEREO];
  p = android_OpenAudioDevice(SR,1,2,BUFFERFRAMES);
  if(p == NULL) return 0; 
  int on = 1;
  while(on) {
   samps = android_AudioIn(p,inbuffer,VECSAMPS_MONO);
   for(i = 0; i < samps; i++) {
		printf("%f ", inbuffer[i]);
	   //outbuffer[j] = outbuffer[j+1] = inbuffer[i];
   }
   //android_AudioOut(p,outbuffer,samps*2); 
  }  
  android_CloseAudioDevice(p);
  return 0;
}
#endif


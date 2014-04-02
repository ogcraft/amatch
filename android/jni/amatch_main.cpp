#include <jni.h>
#include <stdio.h>
#include <sndfile.h>
#include <sndfile.hh>
#include <sys/time.h> 
#include <assert.h> 

#include <boost/circular_buffer.hpp>
#include "logging.h"
#include "amatch.h"
#include "utils.h"
#include "amatch_interface.h"
#include "opensl_io.h"
#if 1

//void writekeys(const key_vector keys, const char* fn);

key_vector& recorded_keys();
boost::circular_buffer<float>& get_record_buffer();

#define		BUFFER_LEN		1024

void create_file (const char * fname, int format, const float* data, int size)
{	

	SndfileHandle file ;
	int channels = 1;
	int srate = 11025;

	printf ("Creating file named '%s'\n", fname) ;

	file = SndfileHandle (fname, SFM_WRITE, format, channels, srate) ;
	file.write (data, size) ;

	puts ("") ;
	/*
	**	The SndfileHandle object will automatically close the file and
	**	release all allocated memory when the object goes out of scope.
	**	This is the Resource Acquisition Is Initailization idom.
	**	See : http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization
	*/
} /* create_file */

int main(int argc, char** argv)
{
	LOGD("amatch","Run : %s\n", argv[0]);
	char  sndflile_ver [128] ;
	sf_command (NULL, SFC_GET_LIB_VERSION, sndflile_ver, sizeof (sndflile_ver)) ;
	printf("Sndfile version: %s\n",sndflile_ver);
	printf("amatch android: version 1.1\n\n"); 
	///////////////////////////////////////////////
	const char* track_keys_fn = "/storage/sdcard0/asearch/monstr-eng-11025.fpkeys";
	const char* outfname = "/storage/sdcard0/asearch/testrec.wav";
	read_track_fpkeys(track_keys_fn);

	int maxsamps = num_samples_to_record();
	
	if(!open_audio_device()) {
		printf("Failed to open audio_device\n");
		return 1;
	}
	
    //struct timeval start_rec_tv;
    //gettimeofday(&start_rec_tv,0);
    //return tv.tv_sec + (tv.tv_usec / 1000000.0);

	int tryes = 100;
//	while(tryes--) {	
		printf("\n------------------------------\nSTART RECORDING...\n");	
		double start_rec = time_now();  
		int samps = 0;
		printf("maxsamps: %d\n", maxsamps);
		while(samps < maxsamps) {
			samps = recording();
			printf("samps %d of %d\n", samps, maxsamps);
		}		
		printf("Recorded %d samples\n", samps);
		
		create_file (outfname, SF_FORMAT_WAV | SF_FORMAT_PCM_16, get_record_buffer().linearize(), get_record_buffer().size());
		
		return 0;
		generate_fp_keys_from_in();

		key_vector rec_keys = recorded_keys();

		printf("Generated: %d keys\n", rec_keys.size());
		
		int found_index = match_sample();

		float found_sec = found_index * sec_per_sample;
		
		//struct timeval now_tv;
		//gettimeofday(&now_tv,0);
		double now = time_now();  
		float corrected_sec = found_sec + now - start_rec;
		if(found_index > 10 ) {
			printf("\n!!!!!!!!   Playing:     %f     sec | Found %d %f\n", corrected_sec, found_index, found_sec);
		}
//		sleep(3);
//	}
	
	close_audo_device();
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


#include <jni.h>
#include <stdio.h>
#include <sndfile.h>
#include <sys/time.h> 
#include <assert.h> 

#include "logging.h"
#include "amatch.h"
#include "utils.h"
#include "amatch_interface.h"
#include "opensl_io.h"


//void writekeys(const key_vector keys, const char* fn);

int main(int argc, char** argv)
{
	LOGD("amatch","Run : %s\n", argv[0]);
	char  sndflile_ver [128] ;
	sf_command (NULL, SFC_GET_LIB_VERSION, sndflile_ver, sizeof (sndflile_ver)) ;
	printf("Sndfile version: %s\n",sndflile_ver);
	printf("amatch android: version 1.1\n\n"); 
	///////////////////////////////////////////////
//	if(!init_amatch_context()) {
//		printf("Failed to create amatch_context. Exiting.\n");
//		return 1;
//	}
	
	const char* track_keys_fn = "/storage/sdcard0/asearch/monstr-eng1-11025.fpkeys";
	read_track_fpkeys(track_keys_fn);
	
	const char* sample_keys_fn = "/storage/sdcard0/asearch/monstr-sample.fpkeys";
    const char* translation_fn = "/storage/sdcard0/asearch/monstr-rus-44k.mp3"

	int samps = 0;
	
	if(!open_audio_device()) {
		printf("Failed to open audio_device\n");
		return 1;
	}

	
    //struct timeval start_rec_tv;
    //gettimeofday(&start_rec_tv,0);
    //return tv.tv_sec + (tv.tv_usec / 1000000.0);

	skip_samples(25);

	int tryes = 100;
	while(tryes--) {	
		printf("\n------------------------------\nSTART RECORDING...\n");	
		double start_rec = time_now();  
		
		generate_fp_keys_from_in();
		
		//for(int i = 0; i < sample_keys.size(); i++) { printf("K: 0x%08x\n", sample_keys[i]); }
		//writekeys(sample_keys, sample_keys_fn);
		int found_index = match_sample();

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
	
	close_audo_device();
//	destroy_amatch_context();
	return 0;
}


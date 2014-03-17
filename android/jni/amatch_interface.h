#ifdef __cplusplus
extern "C" {
#endif

//void start_process();
//void stop_process();

#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
#define SR 11025
#define NSAMPLES (SR * 30)

struct amatch_context;

amatch_context* get_amatch_context();
bool init_amatch_context();
void destroy_amatch_context();

bool open_audio_device();

size_t read_track_fpkeys(const char* fn);

void close_audo_device();

int read_audio_in(float inbuffer[], size_t nsamples);

int generate_fp_keys_from_in();
		
int match_sample();

#ifdef __cplusplus
};
#endif

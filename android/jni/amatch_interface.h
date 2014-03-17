#ifdef __cplusplus
extern "C" {
#endif


#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
#define SR 11025
#define NSAMPLES (SR * 30)
#define SEC_PER_KEY 0.011609977324263039

struct amatch_context;

const char* amatch_version();
amatch_context* get_amatch_context();
//bool init_amatch_context();
//void destroy_amatch_context();

bool open_audio_device();

size_t read_track_fpkeys(const char* fn);

void close_audo_device();

void skip_samples(int nsamples);

int read_audio_in(float inbuffer[], size_t nsamples);

int generate_fp_keys_from_in();
		
int match_sample();

#ifdef __cplusplus
};
#endif

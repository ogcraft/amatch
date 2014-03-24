#ifdef __cplusplus
extern "C" {
#endif

#define AMATCH_VER "1.13"
#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
#define SR 11025
#define SEC_TO_RECORD 20
#define SEC_TO_MATCH 5
#define NSAMPLES (SR * SEC_TO_RECORD)

#define SEC_PER_KEY 0.011609977324263039

struct amatch_context;

const char* amatch_version();
amatch_context* get_amatch_context();
//bool init_amatch_context();
//void destroy_amatch_context();

long get_start_rec_time();

bool open_audio_device();

size_t read_track_fpkeys(const char* fn);

void close_audo_device();

void skip_samples(int nsamples);

int read_audio_in(float inbuffer[], size_t nsamples);

int collect_rec_samples(long start_rec_time);

int generate_fp_keys_from_in();
		
int match_sample();

#ifdef __cplusplus
};
#endif

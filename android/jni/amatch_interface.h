#ifdef __cplusplus
extern "C" {
#endif

#define AMATCH_VER "1.36"
#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
#define SR 11025
#define SEC_TO_RECORD 10.0
#define SEC_TO_MATCH 7.0
#define NRECSAMPLES (int)(SR * (SEC_TO_RECORD))

#define DELAY_PER_SEC 0.00255126

#define SEC_PER_KEY 0.011609977324263039

struct amatch_context;

const char* amatch_version();
int num_samples_to_record();
double num_sec_to_record();
int get_sample_rate();
double delay_per_sec();


size_t read_track_fpkeys(const char* fn);

int generate_fp_keys_from_in();
		
int match_sample();


void get_recorded_samples(float p[]);
void put_recorded_samples(short p[], int size);
int get_recorded_samples_size();
void clear_recorded_samples();

/*
void start_playing();
void stop_playing();
void pause_playing();
void start_recording();
void stop_recording();
void pause_recording();
int player_state();
int recorder_state();
bool init_amatch_context();
void destroy_amatch_context();
bool open_audio_device();
void close_audo_device();
void skip_samples(int nsamples);
int read_audio_in(float inbuffer[], size_t nsamples);
int recording();
*/

void write_recorded_as_file (const char * fname);

#ifdef __cplusplus
};
#endif

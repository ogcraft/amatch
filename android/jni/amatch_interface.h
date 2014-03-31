#ifdef __cplusplus
extern "C" {
#endif

#define AMATCH_VER "1.20b"
#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
#define SR 11025
#define SEC_TO_RECORD 20
#define SEC_TO_MATCH 5
#define NRECSAMPLES (SR * (SEC_TO_RECORD + 1))

#define SEC_PER_KEY 0.011609977324263039

struct amatch_context;

const char* amatch_version();
int nrecsamples();

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

int recording();

void start_playing();
void stop_playing();
void pause_playing();
void start_recording();
void stop_recording();
void pause_recording();
int player_state();
int recorder_state();

void get_recorded_samples(float p[]);
void put_recorded_samples(short p[], int size);
int get_recorded_samples_size();

void write_recorded_as_file (const char * fname);

#ifdef __cplusplus
};
#endif

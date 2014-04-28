////////////////////////////
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <string>         // std::string
#include <bitset>         // std::bitset

#include <sndfile.h>
#include "amatch.h"
#include "utils.h"
#include "sigproc.h"

using namespace std;

int main(int argc, char* argv[])
{
	int ret = 0;
	char  sndflile_ver [128] ;
	sf_command (NULL, SFC_GET_LIB_VERSION, sndflile_ver, sizeof (sndflile_ver)) ;
	std::cout << "Sndfile version: " << sndflile_ver << std::endl << std::endl;

	if(argc < 2) {
		std::cout << "Usage: " << argv[0] << " wav_file_name" << std::endl;
		exit(1);
	}

	const char* wavfile = argv[1]; 
	std::cout << "Running " << argv[0] << " with '" << argv[1] << "'" << std::endl << std::endl;

	dump_snd_file_info(wavfile);

    ret =  0; //generate_fpkeys(wavfile); 
	
	return ret;
}



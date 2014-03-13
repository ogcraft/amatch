#include <jni.h>
#include <stdio.h>
#include <sndfile.h>

#include "amatch_interface.h"

int main(int argc, char** argv)
{
	LOGD("amatch","Run : %s\n", argv[0]);
	char  sndflile_ver [128] ;
	sf_command (NULL, SFC_GET_LIB_VERSION, sndflile_ver, sizeof (sndflile_ver)) ;
	printf("Sndfile version: %s\n",sndflile_ver);



	return 0;
}

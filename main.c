#include "lwh2f.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	//Read how many times data should be polled
	int count = 1;
	if (argc > 1)
		count = strtol(argv[1], NULL, 10);

	//Initialize LWH2F bridge
	if ( lwh2f_init() == -1) {
		return -1;
	}

	int i = 0;
	while (i < count) {
		uint16_t temp = lwh2f_poll();
		if (temp < 4096) {	//Check if data is valid
			printf("%04x\n", temp);
			i++;
		}
	}

	//Close LWH2F bridge
	if ( lwh2f_stop() == -1) {
		return -1;
	}

	return 0;
}
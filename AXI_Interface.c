#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include "hps.h"
#include <unistd.h>
#include <stdint.h>
#include "hps_0.h"

int main(void) {
	//Open file descpriptor to device memory
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1) {
		perror("Could not open memory device driver");
		return -1;
	}

	//Map H2F bridge to virtual memory
	void *virtualBase = mmap(NULL, ALT_H2F_UB_ADDR - ALT_H2F_LB_ADDR + 1, PROT_READ,
							MAP_PRIVATE, fd, (off_t)ALT_H2F_ADDR);
	if (virtualBase == MAP_FAILED) {
		perror("Could not map H2F hardware");
		if ( close(fd) == -1)
			perror("Could not close memory device driver");
		return -1;
	}

	//Assign variable to FPGA data address
	volatile uint16_t *H2F_Data;
	H2F_Data = (volatile uint16_t *)(virtualBase + DATA_BUFFER_BASE);

	printf("Success\n");
	return 0;
}
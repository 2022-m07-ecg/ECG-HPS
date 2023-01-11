#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include "hps.h"
#include <unistd.h>
#include <stdint.h>
#include "hps_0.h"

#define LWH2F_BASE ALT_LWFPGASLVS_ADDR
#define LWH2F_SPAN ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1

int main(void) {
	int fd;	//File descriptor of /dev/mem
	void *lwh2f_virtual_base = NULL;	//Virtual address of LWH2F bridge
	volatile uint16_t *data_base = NULL;	//Virtual address of data from FPGA

	//Open file descpriptor to device memory
	fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (fd == -1) {
		perror("Could not open memory device driver");
		return -1;
	}
	printf("Opened memory device driver\n");

	//Map LWH2F bridge to virtual memory
	lwh2f_virtual_base = mmap(NULL, LWH2F_SPAN, PROT_READ, MAP_SHARED, fd, (off_t)LWH2F_BASE);
	if (lwh2f_virtual_base == MAP_FAILED) {
		perror("Could not map LWH2F hardware");
		if ( close(fd) == -1)
			perror("Could not close memory device driver");
		return -1;
	}
	printf("Mapped LWH2F hardware\n");

	//Get virtual address for FPGA data
	data_base = (volatile uint16_t *)(lwh2f_virtual_base + DATA_BUFFER_BASE);

	//Print FPGA data
	int i;
	for (i = 0; i < 10; i++) {
		uint16_t temp = *data_base;
		printf("%u\t", temp);
	}
	printf("\n");

	//Close memory
	if ( munmap(lwh2f_virtual_base, LWH2F_SPAN) == -1) {
		perror("Could not unmap LWH2F hardware");
		return -1;
	}
	if ( close(fd) == -1) {
		perror("Could not close memory device driver");
		return -1;
	}
	return 0;
}
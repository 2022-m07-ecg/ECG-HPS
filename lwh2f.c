#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include "hps.h"
#include <unistd.h>
#include <stdint.h>
#include "hps_0.h"
#include "lwh2f.h"

#define LWH2F_BASE ALT_LWFPGASLVS_ADDR
#define LWH2F_SPAN ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1

static int fd;	//File descriptor of /dev/mem
static void *lwh2f_virtual_base = NULL;	//Virtual address of LWH2F bridge
static volatile uint16_t *data_base = NULL;	//Virtual address of data from FPGA

int lwh2f_init(void) {
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
	
	return 0;
}

int lwh2f_stop(void) {
	//Close memory
	if ( munmap(lwh2f_virtual_base, LWH2F_SPAN) == -1) {
		perror("Could not unmap LWH2F hardware");
		return -1;
	}
	printf("Unmapped LWH2F hardware\n");

	lwh2f_virtual_base = NULL;
	data_base = NULL;
	
	if ( close(fd) == -1) {
		perror("Could not close memory device driver");
		return -1;
	}
	printf("Closed memory device driver\n");
	
	return 0;
}

uint16_t lwh2f_read(void) {
	return *data_base;
}
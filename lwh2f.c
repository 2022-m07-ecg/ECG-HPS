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

//Hardware constants
#define LWH2F_BASE ALT_LWFPGASLVS_ADDR
#define LWH2F_SPAN ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1
#define PIO_DATA_OFST 0x0
#define PIO_DIR_OFST 0x1
#define PIO_INT_MASK_OFST 0x2
#define PIO_EDGE_CAPT_OFST 0x3
#define PIO_DATA_MASK 0xFFF
#define PIO_BIT_12_MASK 1 << 12

//Private global variables
static int fd;									//File descriptor for /dev/mem
static void *lwh2f_virtual_base = NULL;			//Virtual address of LWH2F bridge
static void *pio_base = NULL;					//Virtual address of PIO port from FPGA
static volatile uint16_t *pio_data = NULL;		//PIO data register
static volatile uint16_t *pio_dir = NULL;		//PIO data direction register
static volatile uint16_t *pio_int_mask = NULL;	//PIO interrupt mask register
static volatile uint16_t *pio_edge_capt = NULL;	//PIO edge caputre register

int lwh2f_init(void) {
	//Open file descpriptor to device memory
	fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (fd == -1) {
		perror("Could not open memory device driver");
		return -1;
	}
	printf("Opened memory device driver\n");

	//Map LWH2F bridge to virtual memory
	lwh2f_virtual_base = mmap(NULL, LWH2F_SPAN, (PROT_READ | PROT_WRITE),
								MAP_SHARED, fd, (off_t)LWH2F_BASE);
	if (lwh2f_virtual_base == MAP_FAILED) {
		perror("Could not map LWH2F hardware");
		if ( close(fd) == -1)
			perror("Could not close memory device driver");
		return -1;
	}
	printf("Mapped LWH2F hardware\n");

	//Get virtual address for PIO
	pio_base = lwh2f_virtual_base + DATA_BUFFER_BASE;
	pio_data = (volatile uint16_t *)(pio_base + 2 * PIO_DATA_OFST);
	pio_dir = (volatile uint16_t *)(pio_base + 2 * PIO_DIR_OFST);
	pio_int_mask = (volatile uint16_t *)(pio_base + 2 * PIO_INT_MASK_OFST);
	pio_edge_capt = (volatile uint16_t *)(pio_base + 2 * PIO_EDGE_CAPT_OFST);
	// ptinyf("size of uint ptr = %d\n", sizeof(uInt16_t *));
	*pio_int_mask = 0x0;	//Make sure interrupts are disabled
	*pio_edge_capt = 1;		//Clear edge capture register
	
	return 0;
}

int lwh2f_stop(void) {
	//Close memory
	if (munmap(lwh2f_virtual_base, LWH2F_SPAN) == -1) {
		perror("Could not unmap LWH2F hardware");
		return -1;
	}
	printf("Unmapped LWH2F hardware\n");

	//Set pointers to NULL
	lwh2f_virtual_base = NULL;
	pio_base = NULL;
	pio_data = NULL;
	pio_int_mask = NULL;
	pio_edge_capt = NULL;
	
	if (close(fd) == -1) {
		perror("Could not close memory device driver");
		return -1;
	}
	printf("Closed memory device driver\n");
	
	return 0;
}

uint16_t lwh2f_poll(void) {
	uint16_t result;

	if (*pio_data == 6144) {
		printf("Data: %08x\t", *pio_data);
		printf("direction: %08x\t", *(pio_data+1));
		printf("interrupt: %08x\t", *(pio_data+2));
		int i;
		for (i = 0; i < 10; i++) {
			printf("%08x\t", *(pio_data+3+i));
		}
		printf("\n");
	}
	

	//Check for new data
	if (*pio_edge_capt) {
		*pio_edge_capt = 1;	//Clear edge capture register
		result =  *pio_data - 4096;	//Read data 11:0
		// printf("%X\t%X\n", *pio_data, *pio_edge_capt);
	} else {
		result = 4096;	//No new data available
	}

	return result;
}
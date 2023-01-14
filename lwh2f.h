#ifndef _LWH2F_H
#define _LWH2F_H

#include <stdint.h>

/**
 * @brief Initializes FPGA hardware and sets up HPS software for using the LWH2F bridge
 * 
 * @return -1 on failure, else 0
 */
int lwh2f_init(void);

/**
 * @brief Deallocated HPS memory, closes file descpriptor and sets pionters to NULL
 * 
 * @return -1 on failure, else 0
 */
int lwh2f_stop(void);

/**
 * @brief Checks for new FPGA data and returns data if available
 * 
 * @return Value of FPGA data between 0 and 4095 if new, else 4096
 */
uint16_t lwh2f_poll(void);

#endif //_LWH2F_H
// #include "lwh2f.h"
// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>

// int main(int argc, char **argv) {
// 	if ( lwh2f_init() == -1) {
// 		return -1;
// 	}

// 	int count = 1;
// 	if (argc > 1)
// 		count = strtol(argv[1], NULL, 10);

// 	int i;
// 	for (i = 0; i < count; i++) {
// 		uint16_t temp = lwh2f_read();
// 		printf("%u\n", temp);
// 	}

// 	if ( lwh2f_stop() == -1) {
// 		return -1;
// 	}

// 	return 0;
// }
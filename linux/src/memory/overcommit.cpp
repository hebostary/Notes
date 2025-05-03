#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "memory.hpp"

int malloc_64mb_loop() {
    int n = 0;
    char *p;

    while (1) {
        // 每次分配 64 MB
        p = (char *)malloc(1<<26);
        if ( p == NULL) {
                printf("malloc failure after %d MB\n", n);
                return 0;
        }

        memset (p, 0, (1<<26));

	    n +=64;
        printf ("got %d MB\n", n);
    }
}

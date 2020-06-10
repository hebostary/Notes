#include <stdio.h>
#include <stdlib.h>

int main (void) {
    int n = 0;

    while (1) {
        if (malloc(1<<24) == NULL) {
                printf("malloc failure after %d MB\n", n);
                return 0;
        }
	n +=16;
        printf ("got %d MB\n", n);
    }
}

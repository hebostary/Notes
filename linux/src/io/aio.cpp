#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <libaio.h>

#include "io.hpp"

int aio_write() {
	printf("\n[INFO] running aio write demo...\n");
	io_context_t ctx;
	struct iocb iocb;
	struct iocb * iocbs[1];
	struct io_event events[1];
	struct timespec timeout;
	int fd;

	fd = open("/tmp/test", O_WRONLY | O_CREAT);
	if (fd < 0) err(1, "open");

	memset(&ctx, 0, sizeof(ctx));
	if (io_setup(10, &ctx) != 0) err(1, "io_setup");

	const char *msg = "hello aio";
	io_prep_pwrite(&iocb, fd, (void *)msg, strlen(msg), 0);
	iocb.data = (void *)msg;

	iocbs[0] = &iocb;

    printf("submitting aio requst\n");
	if (io_submit(ctx, 1, iocbs) != 1) {
		io_destroy(ctx);
		err(1, "io_submit");
	}

	while (1) {
		timeout.tv_sec = 0;
		timeout.tv_nsec = 500000000;
		if (io_getevents(ctx, 0, 1, events, &timeout) == 1) {
			close(fd);
			break;
		}
		printf("not done yet\n");
		sleep(1);
	}
    printf("aio done\n");
	io_destroy(ctx);

	return 0;
}
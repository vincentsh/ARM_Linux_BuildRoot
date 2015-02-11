#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "memdev.h"

int main(int argc, char **argv)
{
	int on;
	int fd;

	printf("BBB LED control app!\n");

	if ((argc != 2) || (sscanf(argv[1], "%d", &on) != 1) || (on < 0) || (on > 1))
	{
		fprintf(stderr, "Usage:%s [0|1]\n", argv[0]);
		exit(1);
	}

	fd = open("/dev/bbb_leds", 0);
	if (fd < 0)
	{
		perror("open device leds failed\n");
		exit(1);
	}

	if(on)
	{
		printf("turn on led!\n");
		ioctl(fd, MEMDEV_IOCON, 0);
	}
	else
	{
		printf("turn off led!\n");
		ioctl(fd, MEMDEV_IOCOFF, 0);
	}

	close(fd);
	return 0;
}


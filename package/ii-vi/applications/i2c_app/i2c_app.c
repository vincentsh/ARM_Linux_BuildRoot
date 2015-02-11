#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/string.h>

static int str2hex(const char *str)
{
	char *e;
	return strtoul(str, &e, 16);
}

int main(int argc, char **argv)
{
	int fd;
	int argp = 1;
	int dev = 0;
	char devChar[] = "/dev/i2c-0";
	int addr = 0;
	unsigned char data_buf[64];
	int nw = 0; 		// number of byte of data to write
	int nr = 0;			// number of byte of data to read
	int tmp_data = -1;

	if ((argc < 3) || (sscanf(argv[argp++], "%d", &dev) != 1)
		|| (sscanf(argv[argp], "0x%x|%X", &addr) != 1)
		|| (addr < 0) || (addr > 255)
		|| (dev < 0) || (dev > 2))
	{
		fprintf(stderr, "Usage:%s [dev:0-2] [addr:0x0-0xFE] [[w dat0 dat1 dat2 ...] | [r rlen]]\n", argv[0]);
		exit(1);
	}

	argp++;
	if (strcasecmp(argv[argp], "W") == 0)	// I2C data write command
	{
		argp++;
		for(; (argp < argc) && (1 == sscanf(argv[argp], "%x|%X", &tmp_data)); argp++)
		{
			data_buf[nw++] = tmp_data;
		}

		if (nw == 0)
		{
			if (argp < argc)
				perror("! invalid argument!\n");
			else
				perror("! missing argument!\n");
			exit(1);
		}
	}

	if ((argp < argc) && (strcasecmp(argv[argp], "R") == 0))
	{
		argp++;
		if ((argp < argc) && (1 == sscanf(argv[argp], "%x|%X", &tmp_data)))
		{
			nr = tmp_data;
		}
		else
		{
			perror("! missing argument!\n");
			exit(1);
		}
	}

	printf("dev%d, addr=0x%x, rlen=%d, wlen=%d, wdata=%d, %d, %d, %d, ...\n", dev, addr, nr, nw, data_buf[0], data_buf[1], data_buf[2], data_buf[3]);

	devChar[9] = dev + 0x30;
	fd = open(devChar, O_RDWR);
	if (fd < 0)
	{
		printf("open device i2c (%s) failed\n", devChar);
		exit(1);
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)		// set I2C slave address
	{
		perror("i2c set slave address failed\n");
		close(fd);
		exit(1);
	}

	struct i2c_msg msg;			// I2C message structure
	struct i2c_rdwr_ioctl_data ioctl_data;

	msg.addr = addr;
	msg.len = nw;
	msg.buf = data_buf;
	msg.flags = 0;
	ioctl_data.nmsgs = 1;
	ioctl_data.msgs = &msg;

	if (nw != 0)
	{
		printf("ioctl write, return %d\n", ioctl(fd, I2C_RDWR, &ioctl_data));
	}

	sleep(1);

	if (nr != 0)
	{
		msg.len = nr;
		msg.flags |= I2C_M_RD;

		printf("ioctl read, return %d\n", ioctl(fd, I2C_RDWR, &ioctl_data));
	}

	int i;
	printf("i2c read data (raw) = ");
	for (i = 0; i < nr; i++)
	{
		printf("%X ", data_buf[i]);
	}

	printf("\n");
	printf("i2c read data (char) = ");
	for (i = 0; i < nr; i++)
	{
		printf("%c", data_buf[i]);
	}
	printf("\n");

	close(fd);
	return 0;
}


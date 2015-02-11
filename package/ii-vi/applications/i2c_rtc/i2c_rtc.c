#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/string.h>

#define RTC_I2C_DEV		"/dev/i2c-1"
#define RTC_I2C_ADDR	(0x6f)
static unsigned char HOMERTC[1] = { 0 };
static unsigned char EWRTC[2] = { 7, 0x10 };	// { offset, enable_cmd }
static unsigned char DWRTC[2] = { 7, 0x00 };	// { offset, disable_cmd }

#define ESC_KEY			'\x1B'

typedef struct
{
	unsigned short year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char xsecond; // second/100
} DateTime;

static int i2c_fd = 0;
static struct i2c_msg msg;			// I2C message structure
static struct i2c_rdwr_ioctl_data ioctl_data;
static unsigned char rtc_buf[8] = {0, };

void rtcEnableWrite()
{
	if (i2c_fd < 0)
	{
		return;
	}

	msg.addr = RTC_I2C_ADDR;
	msg.len = 2;
	msg.buf = EWRTC;
	msg.flags = 0;
	ioctl_data.nmsgs = 1;
	ioctl_data.msgs = &msg;

	ioctl(i2c_fd, I2C_RDWR, &ioctl_data);
}

void rtcDisableWrite()
{
	if (i2c_fd < 0)
	{
		return;
	}

	msg.addr = RTC_I2C_ADDR;
	msg.len = 2;
	msg.buf = DWRTC;
	msg.flags = 0;
	ioctl_data.nmsgs = 1;
	ioctl_data.msgs = &msg;

	ioctl(i2c_fd, I2C_RDWR, &ioctl_data);
}

static int ParseDateTime ( const char *str, const unsigned fields[3], unsigned parts[3] )
{
	int i = 0;
	for (i = 0; i < 3; i++)
	{
		char *e;
		parts[i] = strtoul(str, &e, 10);
		if ((e == str) || (*e != (char)(fields[i])) || (parts[i] < ((fields[i]>>8) & 0xFFF)) || (parts[i] > ((fields[i]>>20) & 0xFFF)))
			return 0;
		str = e + 1;
	}
	return 1;
}

static int BCDToInt ( int bcd )
{
	return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static int ToBCD ( int n )
{
	return ((n / 10) << 4) | (n % 10);
}

void rtcGetDateTime(DateTime *datetime)
{
	if (i2c_fd < 0)
	{
		return;
	}

	msg.addr = RTC_I2C_ADDR;
	msg.len = 1;
	msg.buf = HOMERTC;
	msg.flags = 0;
	ioctl_data.nmsgs = 1;
	ioctl_data.msgs = &msg;

	ioctl(i2c_fd, I2C_RDWR, &ioctl_data);

	msg.addr = RTC_I2C_ADDR;
	msg.len = 6;
	msg.buf = rtc_buf;
	msg.flags |= I2C_M_RD;
	ioctl_data.nmsgs = 1;
	ioctl_data.msgs = &msg;

	ioctl(i2c_fd, I2C_RDWR, &ioctl_data);

	datetime->year = 2000 + BCDToInt(rtc_buf[5]);
	datetime->month = BCDToInt(rtc_buf[4]);
	datetime->day = BCDToInt(rtc_buf[3]);
	datetime->hour = BCDToInt(rtc_buf[2] & 0x7f);
	datetime->minute = BCDToInt(rtc_buf[1]);
	datetime->second = BCDToInt(rtc_buf[0]);
}

int rtcSetDateTime(const DateTime *datetime)
{
	int ok = 1;
	int buf_ptr = 0;
	int wlen = 1;

	if (i2c_fd < 0)
	{
		return 0;
	}

	if (*(int *)&datetime[0] != -1) // set date
	{
		wlen += 3;
		buf_ptr = 7;
		if (datetime->year < 2000)
		{
			perror("! year must be larger than 2000\n");
			exit(1);
		}
		rtc_buf[--buf_ptr] = ToBCD(datetime->year - 2000);
		rtc_buf[--buf_ptr] = ToBCD(datetime->month);
		rtc_buf[--buf_ptr] = ToBCD(datetime->day);
	}

	if (*(int *)&datetime[1] != -1) // set time
	{
		wlen += 3;
		buf_ptr = 4;
		rtc_buf[--buf_ptr] = ToBCD(datetime->hour) | 0x80;
		rtc_buf[--buf_ptr] = ToBCD(datetime->minute);
		rtc_buf[--buf_ptr] = ToBCD(datetime->second);
	}

	if(buf_ptr != 0)
	{
		rtcEnableWrite();

		rtc_buf[0] = buf_ptr - 1;

		msg.addr = RTC_I2C_ADDR;
		msg.len = wlen;
		msg.buf = rtc_buf;
		msg.flags = 0;
		ioctl_data.nmsgs = 1;
		ioctl_data.msgs = &msg;

		ioctl(i2c_fd, I2C_RDWR, &ioctl_data);

		rtcDisableWrite();
	}
	return ok;
}

int main(int argc, char **argv)
{
	i2c_fd = open(RTC_I2C_DEV, O_RDWR);						// open I2C driver file
	if (i2c_fd < 0)
	{
		printf("failed to open I2C device!\n");
	}

	if (ioctl(i2c_fd, I2C_SLAVE_FORCE, RTC_I2C_ADDR) < 0)		// set I2C slave address
	{
		perror("i2c set slave address failed\n");
		close(i2c_fd);
		exit(1);
	}

	DateTime datetime = { -1, -1, -1, -1, -1, -1, -1 };
	int eol = '\r';
	int repeat = 0;
	const char *argp = 0;
	if(argc <= 3)
	{
		if( (argc == 2) && strcasecmp((const char *)argv[1], "R" )==0 )	// 1. RTC run!
		{
			repeat = 1;
		}
		else if(argc >= 2)											// 2. RTC set date time
		{
			argp = (const char *)argv[1];
			static const unsigned datefields[3] =
			{
				2099 << 20 | 2000 << 8 | '-',
				  12 << 20 |    1 << 8 | '-',
				  31 << 20 |    1 << 8
			};
			unsigned parts[3];
			if (ParseDateTime(argp, datefields, parts))
			{
				datetime.year = parts[0];
				datetime.month = parts[1];
				datetime.day = parts[2];
				argp = (const char *)argv[2];
				if(argc == 2)
				{
					rtcSetDateTime(&datetime);
					return;
				}
			}
			else
			{
				argp = (const char *)argv[1];
			}

			static const unsigned timefields[3] =
			{
				  23 << 20 |    0 << 8 | ':',
				  59 << 20 |    0 << 8 | ':',
				  59 << 20 |    0 << 8
			};

			if (ParseDateTime(argp, timefields, parts))
			{
				datetime.hour = parts[0];
				datetime.minute = parts[1];
				datetime.second = parts[2];
				rtcSetDateTime(&datetime);
				return;
			}
			else
			{
				perror( "! invalid argument\n" );
			}
		}

		do
		{
			rtcGetDateTime(&datetime);

			printf("RTC: %d-%02d-%02d %02d:%02d:%02d\r",
							datetime.year,
							datetime.month, datetime.day,
							datetime.hour, datetime.minute,
							datetime.second);

			fflush(stdout);

			sleep(1);
		}
		while(repeat);
		printf("\n");
	}
	else
	{
		perror( "! invalid argument\n" );
	}

	close(i2c_fd);
	return 0;
}


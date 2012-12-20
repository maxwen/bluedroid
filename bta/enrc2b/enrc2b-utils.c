#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "btm_api.h"

typedef unsigned char uchar;

int uart_fd = -1;

uchar buffer[1024];

uchar hci_reset[] = { 0x01, 0x03, 0x0c, 0x00 };

uchar hci_download_minidriver[] = { 0x01, 0x2e, 0xfc, 0x00 };

uchar hci_update_baud_rate[] = { 0x01, 0x18, 0xfc, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00 };

uchar hci_write_bd_addr[] = { 0x01, 0x01, 0xfc, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uchar hci_write_sleep_mode[] = { 0x01, 0x27, 0xfc, 0x0c,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00 };

uchar hci_write_sco_pcm_int[] =
	{ 0x01, 0x1C, 0xFC, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };

uchar hci_write_pcm_data_format[] =
	{ 0x01, 0x1e, 0xFC, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };

uchar hci_write_i2spcm_interface_param[] =
	{ 0x01, 0x6d, 0xFC, 0x04, 0x00, 0x00, 0x00, 0x00 };


int
parse_i2s(char *optarg)
{
	int param[4];
	int ret;
	int i;

	ret = sscanf(optarg, "%d,%d,%d,%d", &param[0], &param[1], &param[2],
		&param[3]);

	if (ret != 4) {
		return(1);
	}

	for (i = 0; i < 4; i++) {
		hci_write_i2spcm_interface_param[4 + i] = param[i];
	}

	return(0);
}

void
read_event(int fd, uchar *buffer)
{
	int i = 0;
	int len = 3;
	int count;

	while ((count = read(fd, &buffer[i], len)) < len) {
		i += count;
		len -= count;
	}

	i += count;
	len = buffer[2];

	while ((count = read(fd, &buffer[i], len)) < len) {
		i += count;
		len -= count;
	}
}

void
hci_send_cmd(uchar *buf, int len)
{
	write(uart_fd, buf, len);
}


void
proc_i2s()
{
	hci_send_cmd(hci_write_i2spcm_interface_param,
		sizeof(hci_write_i2spcm_interface_param));

	read_event(uart_fd, buffer);
}

BTM_API extern tBTM_STATUS BTM_ConfigI2SPCM (tBTM_SCO_CODEC_TYPE codec_type, UINT8 role, UINT8 sample_rate, UINT8 clock_rate)
{
	if ((uart_fd = open("/dev/ttyHS2", O_RDWR | O_NOCTTY)) == -1) {
        return 0;
    }

    parse_i2s("1,0,0,1");
    proc_i2s();

    return 0;
}

BTM_API extern tBTM_STATUS BTM_SetWBSCodec (tBTM_SCO_CODEC_TYPE codec_type)
{
    return 0;
}


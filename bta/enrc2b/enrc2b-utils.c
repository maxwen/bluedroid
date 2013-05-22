#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "btm_api.h"

typedef unsigned char uchar;
static int uart_fd = -1;
static uchar buffer[1024];
static uchar hci_write_i2spcm_interface_param[] =
	{ 0x01, 0x6d, 0xFC, 0x04, 0x00, 0x00, 0x00, 0x00 };

static void read_event(int fd, uchar *buffer)
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

static void hci_send_cmd(uchar *buf, int len)
{
	write(uart_fd, buf, len);
}


static void proc_i2s()
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

	hci_write_i2spcm_interface_param[4] = 1;
	hci_write_i2spcm_interface_param[5] = role;
	hci_write_i2spcm_interface_param[6] = sample_rate;
	hci_write_i2spcm_interface_param[7] = clock_rate;

    proc_i2s();

    return 0;
}

BTM_API extern tBTM_STATUS BTM_SetWBSCodec (tBTM_SCO_CODEC_TYPE codec_type)
{
    return 0;
}


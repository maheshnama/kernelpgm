
#ifndef FIRMWARE_H
#define FIRMWARE_H
#include <stddef.h>
#include "common.h"

int fw_fileopen(char *name);
size_t fw_read(unsigned char *buff, int length);
size_t fw_write(unsigned char *buff, int length);
size_t fw_filelength();
void fw_close();
int fw_resetposition();

#endif



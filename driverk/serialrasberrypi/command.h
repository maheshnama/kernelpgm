#ifndef COMMAND_H
#define COMMAND_H
#include <stddef.h>
#include <stdint.h>
#include "common.h"
#define MAX_FLASH_WRITE     128 

void ReaddatetimeRequest();
void WritedatetimeRequest();
uint8_t getChecksum(uint8_t *buff,int length);


#endif

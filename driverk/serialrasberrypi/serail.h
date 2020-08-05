#ifndef SERAIL_H
#define SERAIL_H
#include <stddef.h>
#include <stdint.h>
#include <sys/select.h>
#include "common.h"

struct serial_data
{
    unsigned char  len; /* data length  */
   unsigned  char data[140] ;
	
};
int fd;

int openPort(char *portname);
void closePort();
int writePort(struct serial_data *send_data);
void setportReadminsize(int min_size);
int readPort();
#endif


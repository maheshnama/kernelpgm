#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "common.h"
#include "firmware.h"
#include "command.h"
#include "serail.h"

extern struct serial_data writedata;

uint8_t getChecksum(uint8_t *buff,int length)
{
	uint8_t result=0;
	int i;

	for (i=0;i<length;i++)
	{
		result=result^buff[i];
	}
	
	return result;
}


void ReaddatetimeRequest()
{
    uint8_t  result;
    int n=readPort();
    printf("flashidresponse=%d\n",n);  
        
        
}

void WritedatetimeRequest(char *fileName)
{
    struct serial_data writedata;
    unsigned char fileBuff[MAX_FLASH_WRITE];
    fw_fileopen(fileName);
    // skip the first four bytes by reading it
    fw_read(fileBuff,sizeof(fileBuff));
    int i;
    // read the binary until the end of the file
	writedata.len=sizeof(fileBuff);
    memcpy(writedata.data,fileBuff,sizeof(fileBuff));
    writePort(&writedata);
    int n=readPort();
    printf("read from cpu length=%d\n",n);      
}
	



 





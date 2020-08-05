#include <stdio.h>
#include <string.h>
#include "firmware.h"

static FILE *file;


unsigned long fileLen;

int fw_fileopen(char *name)
{
	DEBUG_PRINT("opening file");
	file = fopen(name, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s\n", name);
		return;
	}
	
	DEBUG_PRINT("file opened successfully");

}

void fw_close()
{   
    fclose(file);
    DEBUG_PRINT("closing file");
}

size_t fw_read(unsigned char *buff, int length)
{
   
	return fread(buff, 1, length, file);

}
size_t fw_write(unsigned char *buff, int length)
{
   
	return fwrite(buff ,1,length,file);

}


size_t fw_filelength()
{
    size_t size;
    
    if(file==NULL)
    return -1;
    
    DEBUG_PRINT("Reading Filelength");
    fseek(file, 0L, SEEK_SET);
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    
    return size;

}


int fw_resetposition()
{
    DEBUG_PRINT("Resetting file position");
    fseek(file, 0L, SEEK_SET);
}

int fw_seekfourbyte()
{
    DEBUG_PRINT("fw_seekfourbyte");
    fseek(file, 4L, SEEK_SET);
}

#include <stdio.h> // standard input / output functions
#include <string.h> // string function definitions
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h> // File control definitions
#include <errno.h> // Error number definitions
#include <termios.h> // POSIX terminal control definitionss
#include <time.h>   // time calls
#include <stdint.h>
#include "common.h"
#include "serail.h"

int read_port_flag = 0;


//open serail port
int openPort(char *portname)
{
    DEBUG_PRINT("opening serial port\n");
    struct termios tty;
    fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        
    perror("open_port: Unable to open /dev/ttyUSB - ");
    }
    else
    fcntl(fd, F_SETFL, 0);
    DEBUG_PRINT("opened sucessfully serial port\n");
    if (tcgetattr (fd, &tty) != 0)
    {
        return -1;
    }
        memset (&tty, 0, sizeof tty);
    cfsetospeed (&tty, B38400);
    cfsetispeed (&tty, B38400);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 10;            //seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= 0;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
       // error_message ("error %d from tcsetattr", errno);
        return -1;
    }
    return (fd);
  
}

void closePort()
{  
    DEBUG_PRINT("closing serial port\n");
    close(fd);
    DEBUG_PRINT("closed  serial port\n");
}

int writePort(struct serial_data *send_data)
{
    if ((write(fd, send_data->data, send_data->len)) != send_data->len)
    {
         return (-1);
    }
     return 0;
}





void setportReadminsize(int min_size)
{
	struct termios settings;

	if (tcgetattr(fd, &settings))
	{
		   /* handle error */
		    return;
	}

	/* set the minimimum no. of chracters to read in each
	 * read call.
	 */
	settings.c_cc[VMIN]  = min_size;
	/* set “read timeout between characters” as 100 ms.
	 * read returns either when the specified number of chars
	 * are received or the timout occurs */
	settings.c_cc[VTIME] = 1; /* 1 * .1s */

	if (tcsetattr (fd, TCSANOW, &settings))
	{
		    /* handle error */
	}
}


int readPort()
{
	struct serial_data recv_data;
	 int recvbytes = 0;
	int maxfd = fd + 1;
	int index = 0;
	/* set the timeout as 1 sec for each read */
	struct timeval timeout = {1, 0};
//	 struct timeval timeout;      
  //  timeout.tv_sec = 10;
    //timeout.tv_usec = 0;

	fd_set readSet;

	/* set the “mininum characters to read” */
	setportReadminsize(132);
	read_port_flag = 1;
    DEBUG_PRINT(" Reading from serial port\n");  
	while(read_port_flag)
	{
	    FD_ZERO(&readSet);
	    FD_SET(fd, &readSet);
	    if(select(maxfd, &readSet, NULL, NULL, &timeout) > 0)
	    {
		    if (FD_ISSET(fd, &readSet))
		    {
		        recvbytes = read(fd, &recv_data.data, sizeof(recv_data.data));
		        if(recvbytes)
		        {
		            recv_data.len = recvbytes;
		            
		            for(index = 0; index < recvbytes; index++)
		            {
		                printf("%p\n",recv_data.data[index]);
		            }
		       
		        }
		        else
		        {
	      		 printf(" handle error");
		            
		        }

		    }
		
	    }

        /* select() – returns 0 on timeout and -1 on error condtion */
        if (!read_port_flag)
        {
	        return  ;
        }
        read_port_flag = 0;
	}
   
   return recvbytes;

    
}




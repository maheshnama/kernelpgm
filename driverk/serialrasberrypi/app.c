#include <stdio.h>
#include <string.h>
#include "firmware.h"

int main(int argc, char **argv)
{
   
  
    if(!openPort(argv[1]))
    {
    return 1;
    }
   
   
   ReaddatetimeRequest();
   WritedatetimeRequest();
    closePort();
  
    return 0;
}

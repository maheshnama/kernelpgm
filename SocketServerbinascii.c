#include <sys/types.h>
#include <netdb.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h> //write
#include<pthread.h> //for threading , link with lpthread
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PORT_NUMBER 1910
//the thread function
void *connection_handler(void *);
void Storetime();
void StoreData(char *receivedData,int rec_size,int unitid);
void StoreDataAscii(char *receivedData,int rec_size);
int CheckMessageTypeAndGetUnitId(unsigned char *msg);
int CheckMessageTypeGetPacketID(unsigned char *msg);
void GetDateTime(char *buffer);
pthread_mutex_t lock;
char buffer [255];

#define MSG_TYPE            3
#define UNIT_ID             0
#define PACKET_ID           357
#define MSG_TYPE_21         21
#define AQUAMETRO_PACKET_LEN        359
// the raw data hit at the server
#define START_CHAR '$'
#define END_CHAR '*'
#define NUM_DATA			60
#define PACKET_ID_MAX_LEN           15



void GetDateTime(char *buffer)
{
    struct timeval te;
    struct tm * timeinfo;
    gettimeofday(&te, NULL);
    timeinfo = localtime (&te.tv_sec);
    sprintf(buffer,"%02d%02d%02d_%02d%02d%02d_%06d",(timeinfo->tm_year - 100),timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,(te.tv_usec));


}

void StoreData(char *receivedData,int rec_size,int unitid)
{
    char filename[100];
    time_t t;
    FILE* fp;
    char buffer[30];
    GetDateTime(buffer);
    sprintf(filename,"/root/AquaMetro_1889/aquametrodatadump/%d_%s.bin",unitid,buffer);
    fp = fopen(filename,"wb");
    fwrite(receivedData,1,rec_size,fp);
    fclose(fp);
}


void StoreDataAscii(char *receivedData1,int rec_size1)
{
	char filename[100];
    FILE* fp;
	char buffer[300];
	GetDateTime(buffer);
    sprintf(filename,"/root/AquaMetro_1910/aquametrodatadump/%s.txt",buffer);
	fp = fopen(filename,"w");
	if (fp == NULL) {
 	 fprintf(stderr, "Can't open input file in.list!\n");
  	    exit(1);
	}
    fwrite(receivedData1,1,rec_size1,fp);
	fclose(fp);  		

        
}



int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c, *new_sock;
    struct sockaddr_in server , *servinfo,client;
    int ttl = 60; /* max = 255 */
    int error;
    int rv;
    int tr=1;

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

//    puts("Socket created");
    

    // kill "Address already in use" error message
    if (setsockopt(socket_desc,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) {
        perror("setsockopt");
        return 1;
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUMBER);
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }

  //      puts("bind done");
//        printf("thread messgae");

    listen(socket_desc ,100);

    //Accept and incoming connection
//    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
//       printf("Connection accepted\n");
              //sleep(1);

        new_sock = malloc(1);
        *new_sock = client_sock;
       if( pthread_create( &thread_id , NULL , connection_handler , (void*) new_sock) < 0)
        {
        perror("could not create thread");
        return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_detach(thread_id);
        //puts("Handler assigned");
    }

    if (client_sock ==0)
    {
        perror("accept failed");
        close(client_sock);
        exit(0);
        return 1;
    }


    return 0;
}


void Storetime()
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
  //printf ( "Current local time and date: %s", asctime (timeinfo) );
}

int CheckMessageTypeAndGetUnitId(unsigned char *msg)
{
    int unitId = 1;
    //printf("INg check messgae");

    if(msg[MSG_TYPE] == MSG_TYPE_21)
    {
    //    printf("UnitId - %02x%02x%02x\n",msg[UNIT_ID],msg[UNIT_ID+1],msg[UNIT_ID+2]);
        unitId = (((msg[UNIT_ID]) & 0x000000FF) << 16) | (((msg[UNIT_ID + 1]) & 0x000000FF) << 8) | ((msg[UNIT_ID+2]) & 0x00000FFF);
        //  printf("unitId=%d\n",unitId);
        return unitId;
    }
    else
    {
        //printf("bad data UintID not found\n");
        Storetime();
        return 99;
    }
}

int CheckMessageTypeGetPacketID(unsigned char *msg)
{
    int packetId = 1;
    //printf(" check messgae for packet Id");

    if(msg[MSG_TYPE] == MSG_TYPE_21)
    {
//        printf("packetId - %02x%02x\n",msg[PACKET_ID],msg[PACKET_ID+1]);
        packetId = ((((msg[PACKET_ID])&0x00FF)<< 8) | ((msg[PACKET_ID+1])&0x00FF));
  //      printf("packetId=%d\n",packetId);
        return packetId;
    }
    else
    {
      //printf("bad data packetId  not found\n");
      //Storetime();
       return 1299;
    }
}

int GetCommand(int unitId,char *cmd){
    char fileName[100];
    sprintf(fileName,"/var/www/aquametro.qdvts.com/public_html/command2Unit/%d.txt",unitId);
    char readGprsCommand[1000];
    FILE *file;
    file = fopen(fileName, "r");
    char s2[] = "\r\n"; 
    char s3[100];
    char* p; 
  
    if(file)
    {
        printf("file is present\n");
        fscanf(file, "%s",cmd);
	    p = strstr(cmd, s2);
	    if (p) 
        { 
         cmd[strlen(cmd)-2] = '\0';
        }   
        fclose(file);
        return 1;
    }
    else{
       return 0;
    }
}

int DeleteCommand(int unitId){
    char fileName[100];
    sprintf(fileName,"/var/www/aquametro.qdvts.com/public_html/command2Unit/%d.txt",unitId);
    remove(fileName);
}

/*
* This will handle connection for each client
* */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int readSize;
    char *message , client_message[2000];
    char *debugmessage;
    bool result=true;
    char *ack;
     int i;
	//char packetBuff[1024];
    int  packetBuffCount = 0; 
	int flagStartDetected = 0;
    int numPackets = 0;
    int unitId;
   // int i;
    int packetId;
    char packetBuff[512];
    char packetIdbuff[1024];
    uint16_t incompleteDataLen = 0;
    uint16_t offset;
    //Send some messages to the client
    message = "SOC_ESTB\n";
    debugmessage="DBG_ACK\n";
    write(sock , message , strlen(message));
//    char buffer[20];
        // puts("Connection handler");
    //Receive a message from client
    char receivedCmd[1000];
    while((readSize = recv(sock ,client_message,sizeof(client_message), 0)) > 0)
    {
        printf("data received\n");
        printf("readSize=%d\n",readSize);
        fflush(stdout);
        
        	
	    // check for start byte
	    for(i = 0; i < readSize; i++)
	    {
	        if(client_message[i] == START_CHAR)
	        {
	            flagStartDetected = 1;
	        }
	        
	        if(flagStartDetected)
	        {
	            packetBuff[packetBuffCount] = client_message[i];    
	            packetBuffCount++;
	            if(client_message[i] == END_CHAR)
	            {
			    packetBuff[packetBuffCount] = '\0';
	            StoreDataAscii(packetBuff,packetBuffCount);
	            write(sock , debugmessage , strlen(debugmessage)); 
	             write(sock,packetIdbuff,strlen(packetIdbuff));
			    flagStartDetected = 0;
	             packetBuffCount = 0;
	            }
	        }
	        
	    }
        if(readSize==AQUAMETRO_PACKET_LEN)
        {
            unitId = CheckMessageTypeAndGetUnitId(client_message);
            printf("\nunitId=%d\n",unitId);
              //changes on 1 june 2018 for sending packetId as reply
            packetId = CheckMessageTypeGetPacketID(client_message);
            printf("\npacketId=%d\n",packetId);

            if(GetCommand(unitId,receivedCmd))
            {
		    sprintf(packetIdbuff,"$%d&%s",packetId,receivedCmd);
              	DeleteCommand(unitId);
            }
            else{
                sprintf(packetIdbuff,"$%d&",packetId);
            }

                fflush(stdout);

            if(unitId != -1)
            {
                StoreData(client_message,AQUAMETRO_PACKET_LEN,unitId);
                write(sock,packetIdbuff,strlen(packetIdbuff));
            }
            else
            {
                //TODO log invalid packet data
            }
         }

    }

    if(readSize == 0)
    {
    //    puts("Client disconnected\n");
        fflush(stdout);
    }
    else if(readSize == -1)
    {
        perror("recv failed");
    //     puts("recv failed\n");
                fflush(stdout);
    }
    else
    {
    //     puts("Incomplete data received from client disconnected\n");
        fflush(stdout);
    }


    close(sock);

return 0;
}


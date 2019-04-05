
/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
 /* USING C++  some of the code is in C but can be compiled using g++ compiler */

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;
void error(char *msg)
{
    perror(msg);
    exit(1);
}

//Global declaration of walls code, binary integer value
int w1[4]= {-1,1,-1,1};
int w2[4]= {-1,-1,1,1};
int w3[4]= {-1,1,1,-1};

int b0[3] = {-1,-1,-1};
int b1[3] = {-1,-1,1};
int b2[3] = {-1,1,-1};
int b3[3] = {-1,1,1}; 
int b4[3] = {1,-1,-1};
int b5[3] = {1,-1,1};
int b6[3] = {1,1,-1};
int b7[3] = {1,1,1};

int EM[33];
int EM1[33], EM2[33], EM3[33];

int buffer1[33];
int buffer2[33];
int buffer3[33];

int acceptedsocid[5];

//Encoding function
// binaryarray is b1,b2 ---- w is wals code -- esave is em1,2,3 
void encoder (int buff[], int wcode[], int Esave[])
{
    int binaryarray[3];

    if(buff[32] == 0)
    {
        copy(b0, b0+3, binaryarray);
    }else if(buff[32] == 1)
    {
        copy(b1,b1+3 , binaryarray);
    }else if(buff[32] == 2)
    {
        copy(b2,b2+3 , binaryarray);
    }else if(buff[32] == 3)
    {
        copy(b3,b3+3 , binaryarray);
    }else if(buff[32] == 4)
    {
        copy(b4,b4+3 , binaryarray);
    }else if(buff[32] == 5)
    {
        copy(b5,b5+3 , binaryarray);
    }else if(buff[32] == 6)
    {
        copy(b6,b6+3 , binaryarray);
    }else if(buff[32] == 7)
    {
        copy(b7,b7+3 , binaryarray);
    }


    int count = 0;
    for(int i =0 ; i < 3; i++)
    {
        for(int j = 0; j<4; j++)
        {
            Esave[count] = binaryarray[i] * wcode[j];
            count++;
        }

    }
}

//copy array function
void copyarray(int arr[], int toarr[])
{
    for(int i = 0; i<33; i++)
    {
        toarr[i] = arr[i];
    }
}


int main(int argc, char *argv[])
{
    int EMFINAL1[33];
    int EMFINAL2[33];
    int EMFINAL3[33];
    struct sockaddr cli_addr;
    socklen_t clilen;
    
    int sockfd, newsockfd, portno;
    int buffer[33];

    struct sockaddr_in serv_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    for(int k = 0 ; k<3; k++) //3 loop so that we can listen to 3 input from the client
    {
        listen(sockfd,5); //keeps server awake
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, &cli_addr, &clilen);
        //newsockfd = accept (sockfd,(struct sockaddr *) &cli_addr , &cli_addr);
        if (newsockfd < 0)
            error("ERROR on accept");
        //bzero(buffer,256);
        n = read(newsockfd,&buffer,33* sizeof(int));
        if (n < 0) error("ERROR reading from socket");

        // cout << "--------" << newsockfd << endl;
        if(buffer[30] == 1)
        {
            //copy the buffer to the new array buffer1
            copyarray(buffer , buffer1);
            //encoding for EM1 using datats end by child 1
            encoder(buffer1, w1, EM1);
            //save newsock id to the acceptedsocid 25, so that we can connection to child 1 in client side
            acceptedsocid[1] = newsockfd;
        }else if(buffer[30] == 2){
            copyarray(buffer , buffer2);
            encoder(buffer2, w2, EM2); // encodes value send by child 2 and generate EM2
            //save the socket so that we can make connection to child 2 in client side , we need to used saved socket to send data specific to child 2
            acceptedsocid[2] = newsockfd;
        }else
        {
            copyarray(buffer, buffer3);
            encoder(buffer3, w3, EM3);// encodes value send by child 3 and generate EM2
            //save the socket so that we can make connection to child 3, we need to use saved socket to send data to specific child 3
            acceptedsocid[3] = newsockfd;
        }
        
        // n = write(newsockfd,"I got your message",18);
        // if (n < 0) error("ERROR writing to socket");
    }

    //create EM using all EM1, EM2, EM3

    for(int i = 0; i<12 ;i++)
    {
        EM[i] = EM1[i] + EM2[i] + EM3[i];
    }

    
    //EMFINAL is EM with wcode which is required to be send to specific child process, make sure you attatch write w-code to respective EM
    
    if(buffer1[31] == 1) 
    {
        //here I have created Final EM
        copy(EM,EM+12,EMFINAL1); //copied EM value to Final EM and bottom 4 steps is just adding right w-code to the final EM after 12 bits EM code
        EMFINAL1[12]=w1[0];
        EMFINAL1[13]=w1[1];
        EMFINAL1[14]=w1[2];
        EMFINAL1[15]=w1[3];

    }else if(buffer2[31] == 1)
    {
        //cout << "I am buffer 1.2 " <<endl;
        copy(EM,EM+12,EMFINAL1);
        EMFINAL1[12]=w2[0];
        EMFINAL1[13]=w2[1];
        EMFINAL1[14]=w2[2];
        EMFINAL1[15]=w2[3];
    }else if(buffer3[31] == 1)
    {
        // cout << "I am buffer 1.3" <<endl;
        copy(EM,EM+12,EMFINAL1);
        EMFINAL1[12]=w3[0];
        EMFINAL1[13]=w3[1];
        EMFINAL1[14]=w3[2];
        EMFINAL1[15]=w3[3];
    }
//FINAL EM2, this will be send to child process 2 with respective w-code
    if(buffer2[31] == 2) 
    {
        copy(EM,EM+12,EMFINAL2);
        EMFINAL2[12]=w1[0];
        EMFINAL2[13]=w1[1];
        EMFINAL2[14]=w1[2];
        EMFINAL2[15]=w1[3];
    }else if(buffer2[31] == 2)
    {
        copy(EM,EM+12,EMFINAL2);
        EMFINAL2[12]=w2[0];
        EMFINAL2[13]=w2[1];
        EMFINAL2[14]=w2[2];
        EMFINAL2[15]=w2[3];
    }else if(buffer3[31] == 2)
    {
        copy(EM,EM+12,EMFINAL2);
        EMFINAL2[12]=w3[0];
        EMFINAL2[13]=w3[1];
        EMFINAL2[14]=w3[2];
        EMFINAL2[15]=w3[3];
    }
//FINAL EM3, this will be send to child process 3 with respective w-code
    if(buffer1[31] == 3) 
    {            
        copy(EM,EM+12,EMFINAL3);
        EMFINAL3[12]=w1[0];
        EMFINAL3[13]=w1[1];
        EMFINAL3[14]=w1[2];
        EMFINAL3[15]=w1[3];
    }else if(buffer2[31] == 3)
    {
        copy(EM,EM+12,EMFINAL3);
        EMFINAL3[12]=w2[0];
        EMFINAL3[13]=w2[1];
        EMFINAL3[14]=w2[2];
        EMFINAL3[15]=w2[3];
    }else if(buffer3[31] == 3)
    {
        copy(EM,EM+12,EMFINAL3);
        EMFINAL3[12]=w3[0];
        EMFINAL3[13]=w3[1];
        EMFINAL3[14]=w3[2];
        EMFINAL3[15]=w3[3];
    }


//PRINTING AS REQUIRED =========================
    cout << "Here is the message from child 1: Value = " << buffer1[32] <<", Destination = " << buffer1[31] << endl; 
    cout << "Here is the message from child 2: Value = " << buffer2[32] <<", Destination = " << buffer2[31] << endl;
    cout << "Here is the message from child 3: Value = " << buffer3[32] <<", Destination = " << buffer3[31] << endl;
    
    
        
    n = write(acceptedsocid[1],&EMFINAL1,33* sizeof(int)); // sending to child process 1
    if (n < 0) error("ERROR writing to socket");
    sleep(1);//sleeping for 1 sec

    n = write(acceptedsocid[2],&EMFINAL2,16* sizeof(int)); // sending to child process 2
    if (n < 0) error("ERROR writing to socket");
    sleep(1);//sleeping for 1 sec
    
    n = write(acceptedsocid[3],&EMFINAL3,16* sizeof(int)); // sending to child process 3
    if (n < 0) error("ERROR writing to socket");
    sleep(1);//sleeping for 1 sec

    return 0;
}


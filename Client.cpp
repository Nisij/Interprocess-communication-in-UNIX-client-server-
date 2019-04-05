

/* Client side program USING C++  some of the code is in C but can be compiled using g++ compiler */
/* warnings can be ignored*/

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
using namespace std;
int EM1[33], EM2[33], EM3[33];
//DECODING =========================

int Decode(int arr[])
{
    int value ;
    int EM[12];
    int code[5];
    int DM[12];
    int count = 0;
    int DM1,DM2,DM3;

    copy (arr, arr+12, EM);
    copy (arr+12, arr+16, code);

    for(int i = 0; i <12;i++)
    {
        DM[i] = EM[i] * code[count];
        count++;
        if(count == 4)
        { 
            count = 0;
        }   
    }
    DM1 = ((DM[0]+DM[1]+DM[2]+DM[3])/4);
    DM2 = ((DM[4]+DM[5]+DM[6]+DM[7])/4);
    DM3 = ((DM[8]+DM[9]+DM[10]+DM[11])/4);

    if (DM1 == -1){DM1 = 0;} 
    if (DM2 == -1){DM2 = 0;} 
    if (DM3 == -1){DM3 = 0;}

    if (DM1 == 0)
    {
        if(DM2 == 0)
        {
            if(DM3 == 0)
            {
                return 0;
            }else
            {
                return 1;
            }
            
        }else if (DM3 == 0)
        {
            return 2;
        }else
        {
            return 3;
        }
    }else if(DM2 == 0 && DM3 == 0)
    {
        return 4;
    }

    if(DM1 == 1 && DM2 == 0 && DM3 == 1){return 5;}
    if(DM1 == 1 && DM2 == 1 && DM3 == 0){return 6;}
    if(DM1 == 1 && DM2 == 1 && DM3 == 1){return 7;}



return value;
}



void error(char *msg)
{
    perror(msg);
    exit(0);
}


//printing fucntion
void print(int arr[], int i)
{
    cout << endl;
    cout << "Child " << i << endl;
    cout << "Signal:";
    for(int z = 0; z<12; z++)
    {
        cout << arr[z] << " " ;
    }
    cout << endl;
    cout << "Code: ";
    for (int z = 12; z < 16; z++)
    {
        cout << arr[z] << " "; 
    }
    cout << endl;

    
}


int main(int argc, char *argv[])
{
    int value;
    int sockfd, portno, n, i;
    pid_t pid;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    int buffer1[33];
    int buffer2[33];
    int buffer3[33];
    int EM1[16],EM2[16],EM3[16];
    
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    //to read from
    buffer1[30] = 1, buffer2[30] = 2; buffer3[30] = 3;
    cin >> buffer1[31] >> buffer1[32];
    cin >> buffer2[31] >> buffer2[32];
    cin >> buffer3[31] >> buffer3[32];

    cout << "Child 1, sending value: " << buffer1[32] << " to child process " << buffer1[31] << endl;
    cout << "Child 2, sending value: " << buffer2[32] << " to child process " << buffer2[31] << endl;
    cout << "Child 3, sending value: " << buffer3[32] << " to child process " << buffer3[31] << endl;

    for (i =0; i <3; i++)
    {

        if((pid=fork())==0)      // child process
            break;
    }

    if (pid == 0)
    {
        //creating a socket
        
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        // cout << "the pid " << getpid() << " the ppid is  " << getppid() << endl;
        if (sockfd < 0)
            error("ERROR opening socket");
        server = gethostbyname(argv[1]);
        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
        serv_addr.sin_port = htons(portno);
        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
            error("ERROR connecting");
        
        
        //first child process
        if( i == 0)
        {
            n = write(sockfd,&buffer1,33* sizeof(int)); //sends to server
            n = read(sockfd,&EM1,33* sizeof(int)); // recives from server
            
            print (EM1, 1);// display

            value = Decode(EM1); // value after decode
            cout << "Recived value = " << value << endl; 

        }else if(i == 1 )
        {// second child
            
            n = write(sockfd,&buffer2,33* sizeof(int)); // sends to server
            n = read(sockfd,&EM2,33* sizeof(int)); //recives from server
            print (EM2, 2);// display
            value = Decode(EM2); // value after decode
            cout << "Recived value = " << value << endl;

        }else if(i == 2)
        {//third child
            
            n = write(sockfd,&buffer3,33* sizeof(int)); // sends to server
            n = read(sockfd,&EM3,33* sizeof(int)); // recives from server
            print (EM3, 3); // display
            value = Decode(EM3);// value after decode
            cout << "Received value = " << value << endl;
        }
    }
    else
    {    // parent process
        for (int i=0;i<3;i++)
        {
            wait(NULL); //waits until all child process are done
        }
    }
    return 0;
}

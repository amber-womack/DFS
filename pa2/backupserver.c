#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for the sockets
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> // for atoi
#include <unistd.h> //for closing
#include <string.h>
#include <errno.h> //for perror
#include <time.h>
#include <sys/stat.h>
#include <netinet/in.h> // for the structures
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h> //for forking
#include <sys/poll.h> //for the timer function
#include <openssl/md5.h>

#define ERROR -1 //the error
#define MAX_CLIENTS 10 // max clients to have in wait queue, listen call used by kernel
#define MAX_DATA 1024 // size of the buffer, passed to send and receive

struct config {
    char * userOne;
    char * userTwo;
    char * passOne;
    char * passTwo;
};

//struct to parse the dfs.conf file
struct config dfsConf(){  

    struct config configfile;
    char * line;
    char * piece;
    int parsecount = 0;
    char * source = malloc(MAX_DATA);
    //open the file, error if not
    FILE *fp = fopen("dfs.conf", "r");
    if (fp != NULL){
        size_t newLen = fread(source, sizeof(char), MAX_DATA, fp);
        if (newLen == 0){
            fputs("Error reading file", stderr);
        } 
        else{
            source[++newLen] = '\0'; //Just to be safe add null at end of char array 
        }
        fclose(fp);
    }
    //loop through the file, seperating on the spaces and new lines
    while((line = strsep(&source, "\n")) != NULL){  
        while((piece = strsep(&line, " ")) != NULL){ 
            if(parsecount == 0){
            configfile.userOne = piece;
            printf("userOne = %s\n", configfile.userOne);
            }
            else if(parsecount == 1){
            configfile.passOne = piece;
            printf("passOne = %s\n", configfile.passOne);
            }
            else if(parsecount == 2){
            configfile.userTwo = piece;
            printf("userTwo = %s\n", configfile.userTwo);
            }
            else if(parsecount == 3){
            configfile.passTwo = piece;
            printf("passTwo = %s\n", configfile.passTwo);
            }
            parsecount++;
        } 
    }
    return configfile;
}

int main(){

    struct config configcount;
    configcount = dfsConf();

    //struct type server, contains port to which the server is bound locally
    //and client information connecting to us
    struct sockaddr_in server1;
    struct sockaddr_in client1;
    struct sockaddr_in server2;
    struct sockaddr_in client2;
    struct sockaddr_in server3;
    struct sockaddr_in client3;
    struct sockaddr_in server4;
    struct sockaddr_in client4;
    int sock1; // server socket
    int new1;  // client socket
    int sock2; 
    int new2;  
    int sock3; 
    int new3;  
    int sock4; 
    int new4;  
    char * portnum1 = "10001";
    char * portnum2 = "10002";
    char * portnum3 = "10003";
    char * portnum4 = "10004";

    unsigned int sockaddr_len  = sizeof(struct sockaddr_in); //size of the socket calls //////////////? need multiple?? 
    // Length of the data
  //  int data_len;
    //character array that is allocated in memory, defined size at top
    //char data[MAX_DATA];
   // char dataCopy[MAX_DATA];
//_________________________________________________________________________________________

    // ESTABLISH SOCKET - allows for a physical connection between the client and server
    if((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("socket1: ");
        exit(-1);
    }

    // SERVER DATA_STRUCT
    //filling the server data structure
    server1.sin_family = AF_INET;
    //The port number from parsing config file and changing to network byte order
    server1.sin_port = htons(atoi(portnum1));
    //to find what address the server will bind too, and listen on all interfaces
    server1.sin_addr.s_addr = INADDR_ANY; 
    //for padding, its 8 bytes long
    bzero(&server1.sin_zero, 8);

    // BIND PORT TO SOCKET
    //give the server address, a pointer cast too sockaddr 
    if((bind(sock1, (struct sockaddr *) &server1, sockaddr_len)) == ERROR){
        perror("bind1: ");
        exit(-1);
    }

    // LISTEN ON SOCKET
    //to listen for a number of connections
    if((listen(sock1, MAX_CLIENTS)) == ERROR){
        perror("listen1: ");
        exit(-1);
    }

    if(!fork()){
        while(1){ // WAIT FOR CLIENT CONNECTION

            // BLOCK ON ACCEPT, WAIT FOR CLIENT
            //we block on accept, accept returns when the client comes in
            //recevie data from the client, sends back the data to the client
            //once done, keeps waiting for new connections
            //soket descriptor for which client is talking to 
            //Call access to block, waiting for client
            if((new1 = accept(sock1, (struct sockaddr *)&client1, &sockaddr_len)) == ERROR){
                perror("accept: ");
                exit(-1);
            }
            //print that the client is accepted and which port and IP
            printf("New Client connected from port no %d and IP %s\n", ntohs(client1.sin_port), inet_ntoa(client1.sin_addr));
            //close(new1);
        }
    }

    //printf("\nClient Disconnected\n");
    

    //______________________________________________________________
    

    // ESTABLISH SOCKET - allows for a physical connection between the client and server
    if((sock2 = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("socket2: ");
        exit(-1);
    }

    // SERVER DATA_STRUCT
    //filling the server data structure
    server2.sin_family = AF_INET;
    //The port number from parsing config file and changing to network byte order
    server2.sin_port = htons(atoi(portnum2));
    //to find what address the server will bind too, and listen on all interfaces
    server2.sin_addr.s_addr = INADDR_ANY; 
    //for padding, its 8 bytes long
    bzero(&server2.sin_zero, 8);

    // BIND PORT TO SOCKET
    //give the server address, a pointer cast too sockaddr 
    if((bind(sock2, (struct sockaddr *) &server2, sockaddr_len)) == ERROR){
        perror("bind2: ");
        exit(-1);
    }

    // LISTEN ON SOCKET
    //to listen for a number of connections
    if((listen(sock2, MAX_CLIENTS)) == ERROR){
        perror("listen2: ");
        exit(-1);
    }
    if(!fork()){
        while(1){ // WAIT FOR CLIENT CONNECTION

            // BLOCK ON ACCEPT, WAIT FOR CLIENT
            //we block on accept, accept returns when the client comes in
            //recevie data from the client, sends back the data to the client
            //once done, keeps waiting for new connections
            //soket descriptor for which client is talking to 
            //Call access to block, waiting for client
            if((new2 = accept(sock1, (struct sockaddr *)&client2, &sockaddr_len)) == ERROR){
                perror("accept: ");
                exit(-1);
            }
            //print that the client is accepted and which port and IP
            printf("New Client connected from port no %d and IP %s\n", ntohs(client2.sin_port), inet_ntoa(client2.sin_addr));
        }
    }
    //printf("\nClient Disconnected\n");
    //close(new2);

    //________________________________________________________________

    // ESTABLISH SOCKET - allows for a physical connection between the client and server
    if((sock3 = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("socket3: ");
        exit(-1);
    }

    // SERVER DATA_STRUCT
    //filling the server data structure
    server3.sin_family = AF_INET;
    //The port number from parsing config file and changing to network byte order
    server3.sin_port = htons(atoi(portnum3));
    //to find what address the server will bind too, and listen on all interfaces
    server3.sin_addr.s_addr = INADDR_ANY; 
    //for padding, its 8 bytes long
    bzero(&server3.sin_zero, 8);

    // BIND PORT TO SOCKET
    //give the server address, a pointer cast too sockaddr 
    if((bind(sock3, (struct sockaddr *) &server3, sockaddr_len)) == ERROR){
        perror("bind3: ");
        exit(-1);
    }

    // LISTEN ON SOCKET
    //to listen for a number of connections
    if((listen(sock3, MAX_CLIENTS)) == ERROR){
        perror("listen3: ");
        exit(-1);
    }
    if(!fork()){
        while(1){ // WAIT FOR CLIENT CONNECTION

            // BLOCK ON ACCEPT, WAIT FOR CLIENT
            //we block on accept, accept returns when the client comes in
            //recevie data from the client, sends back the data to the client
            //once done, keeps waiting for new connections
            //soket descriptor for which client is talking to 
            //Call access to block, waiting for client
            if((new3 = accept(sock1, (struct sockaddr *)&client3, &sockaddr_len)) == ERROR){
                perror("accept: ");
                exit(-1);
            }
            //print that the client is accepted and which port and IP
            printf("New Client connected from port no %d and IP %s\n", ntohs(client3.sin_port), inet_ntoa(client3.sin_addr));
        }
    }
    //printf("\nClient Disconnected\n");
    //close(new3);

    //_______________________________________________________________

    // ESTABLISH SOCKET - allows for a physical connection between the client and server
    if((sock4 = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("socket4: ");
        exit(-1);
    }

    // SERVER DATA_STRUCT
    //filling the server data structure
    server4.sin_family = AF_INET;
    //The port number from parsing config file and changing to network byte order
    server4.sin_port = htons(atoi(portnum4));
    //to find what address the server will bind too, and listen on all interfaces
    server4.sin_addr.s_addr = INADDR_ANY; 
    //for padding, its 8 bytes long
    bzero(&server4.sin_zero, 8);

    // BIND PORT TO SOCKET
    //give the server address, a pointer cast too sockaddr 
    if((bind(sock4, (struct sockaddr *) &server4, sockaddr_len)) == ERROR){
        perror("bind4: ");
        exit(-1);
    }

    // LISTEN ON SOCKET
    //to listen for a number of connections
    if((listen(sock4, MAX_CLIENTS)) == ERROR){
        perror("listen4: ");
        exit(-1);
    }
    if(!fork()){
        while(1){ // WAIT FOR CLIENT CONNECTION

            // BLOCK ON ACCEPT, WAIT FOR CLIENT
            //we block on accept, accept returns when the client comes in
            //recevie data from the client, sends back the data to the client
            //once done, keeps waiting for new connections
            //soket descriptor for which client is talking to 
            //Call access to block, waiting for client
            if((new4 = accept(sock4, (struct sockaddr *)&client4, &sockaddr_len)) == ERROR){
                perror("accept: ");
                exit(-1);
            }
            //print that the client is accepted and which port and IP
            printf("New Client connected from port no %d and IP %s\n", ntohs(client4.sin_port), inet_ntoa(client4.sin_addr));
        }
    }
    //printf("\nClient Disconnected\n");
    //close(new4);
    int pid;
    do
    {
        pid = wait(NULL);
    }while(pid != -1);
    return 0;
}
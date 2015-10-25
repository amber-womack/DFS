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
    free(source);
    return configfile;
}

int main(int argc, char *argv[]){

    struct config configcount;
    configcount = dfsConf();

    //struct type server, contains port to which the server is bound locally
    //and client information connecting to us
    struct sockaddr_in server1;
    struct sockaddr_in client1;
   
    int sock1; // server socket
    int new1;  // client socket
    char * portnum = argv[2];
    //printf("%s\n", portnum);

    unsigned int sockaddr_len  = sizeof(struct sockaddr_in); //size of the socket calls //////////////? need multiple?? 

    //mkdir of server files
    char *newstr = calloc(256,1);
    strcat(newstr,".");
    strcat(newstr, argv[1]);
    mkdir(newstr, 0777);

    
    // ESTABLISH SOCKET - allows for a physical connection between the client and server
    if((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("socket1: ");
        exit(-1);
    }

    // SERVER DATA_STRUCT
    //filling the server data structure
    server1.sin_family = AF_INET;
    //The port number from parsing config file and changing to network byte order
    server1.sin_port = htons(atoi(portnum));
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
       
        //AUTHORIZED?
        //read in username
       // if(username)
        //Forking and keeping while open
        if(!fork()){
            close(sock1);
            while(1){
                //READS HEADER FROM CLIENT
                //PASSWORD HANDLING, REQUEST TYPE, MESSAGE SIZE
                char filename1[256];
                char filename2[256];
                char username[256];
                char password[256];
                char reqType[256];
                char messageSize[256];

                
                read(new1, username, 256);
                read(new1, password, 256);
                read(new1, reqType, 256);
                read(new1, messageSize, 256);

                int msgSize = atoi(messageSize);

                printf("%s\n", filename1);
                printf("%s\n", filename2);
                printf("%s\n", username);
                printf("%s\n", password);
                printf("%s\n", reqType);
                printf("%s\n", messageSize);
                printf("%d\n", msgSize);
                
                char message1[msgSize];
                char message2[msgSize];
               
              // (read in filename1 here)
                read(new1, filename1, 256);
                read(new1, message1, msgSize); 
                printf("THIS IS MESSAGE 1   %s\n", message1);
               // (read in filename2 here)
                read(new1, filename2, 256);
                read(new1, message2, msgSize); 
                printf("THIS IS MESSAGE 2   %s\n", message2);

                //char buf[msgSize];
               //  ssize_t bytes_read;
               //  do {
               //      bytes_read = read(new1, buf, msgSize);
               //      printf("%zd", bytes_read);
               //  } while (bytes_read > 0); 
               //  printf("%s", buf);  
                // read(new1, message1, msgSize);
                strcat(newstr, "/");
                strcat(newstr, filename1);
               // strcat(newstr, filename2);

                //printf("%s\n", newstr);
                //printf("%s\n", filename);
                // FILE *fp1 = fopen(newstr, "w");
                // if (fp1 != NULL){
                //     ssize_t newlen = write(fp1, message1, strlen(message1));
                //     //ssize_t write(fp, message2, strlen(message2));
                //     // if (newLen == 0){
                //     //     fputs("Error reading file", stderr);
                //     // } 
                //     // else{
                //     //     source[++newLen] = '\0'; //Just to be safe add null at end of char array 
                //     // }
                //     fclose(fp);
                // }
                close(new1);
                exit(0); 
            }
            //close(new1);
           // exit(0);
        }
    }
    free(newstr);


    //printf("\nClient Disconnected\n");
    //close(new4);
    int pid;
    do
    {
        pid = wait(NULL);
    }while(pid != -1);
    return 0;
}
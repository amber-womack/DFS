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
//-------------------------------------- encryption() ------------------------------------------------

// encryption
// char *xorencrypt(char *string, char *key)
// {
//     int length = strlen(key);
//     int i = 0;
//     while(i < strlen(string))
//     {
//         string[i] = string[i] ^ key[i % length];
//         i++;
//     }
//     return string;
// }

//-------------------------------------- dfsConf() ------------------------------------------------
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
//-------------------------------------- put() ------------------------------------------------
void put(char *filename1, char *filename2, char *username, char *password, char *reqType, 
            char *messageSize, char *subfolderOnOf, char *subfolder, char *serverFolder, char *filename, int new1){
    printf("put %s\n", username);
    printf("put %s\n", password);
    printf("put %s\n", reqType);
    printf("put %s\n", messageSize);
    
    int msgSize = atoi(messageSize);
    char message1[msgSize];
    char message2[msgSize];

    //READ IN THE FILE 1 AND MESSAGE 1
    read(new1, filename1, 256);
    read(new1, message1, msgSize);
   // printf("THIS IS MESSAGE 1 encrypted  %s\n", message1);
    
    // char * message1;
    // message1 = xorencrypt(message1EN, configcount.passOne); 
    // printf("THIS IS MESSAGE 1   %s\n", message1);
   
    //READ IN FILE 2 AND MESSAGE 2
    read(new1, filename2, 256);
    read(new1, message2, msgSize); 

    // char * message2;
    // message2 = xorencrypt(message2EN, configcount.passOne);
    // printf("THIS IS MESSAGE 2   %s\n", message2);

    //CONCAT THE FILENAMES 1 and 2
    // struct stat st = {0};
    char *newstr1 = calloc(256,1);
    strcat(newstr1,".");
    strcat(newstr1, serverFolder);
    mkdir(newstr1, 0777);
    strcat(newstr1, "/");
    strcat(newstr1,username);
    mkdir(newstr1, 0777);
    printf("subfolder %s\n", subfolderOnOf);
    if(strcmp(subfolderOnOf, "Subfoldering On") == 0) {
        strcat(newstr1, subfolder);
        mkdir(newstr1, 0777);
        strcat(newstr1, "/");
        strcat(newstr1, filename1);
        printf("filename 1 on server = %s\n", newstr1);
    }
    else {
        strcat(newstr1, "/");
        strcat(newstr1, filename1);
        printf("filename 1 on server = %s\n", newstr1);
    }

    char *newstr2 = calloc(256,1);
    strcat(newstr2,".");
    strcat(newstr2, serverFolder);
    mkdir(newstr2, 0777);
    strcat(newstr2, "/");
    strcat(newstr2,username);
    mkdir(newstr2, 0777);
    printf("subfolder %s\n", subfolderOnOf);
    if(strcmp(subfolderOnOf, "Subfoldering On") == 0){
        strcat(newstr2, subfolder);
        mkdir(newstr2, 0777);
        strcat(newstr2, "/");
        strcat(newstr2, filename2);
        printf("filename 1 on server = %s\n", newstr2);
    }
    else {
        strcat(newstr2, "/");
        strcat(newstr2, filename2);
        printf("filename 2 on server = %s\n", newstr2);
    }
    char * source = malloc(MAX_DATA);

    //WRITING THE MESSAGE TO EACH FILE
    FILE *fp1 = fopen(newstr1, "w");
    printf("file to write to = %s\n", newstr1);
    if (fp1 != NULL){
       // ssize_t newlen1 = write(new1, message1, strlen(message1));
        fputs(message1, fp1);
       // printf("%li\n", newlen1); //length of file
        printf("%s\n", message1); //message 1
        if(fp1 == NULL){
           printf("Error opening file!");
           exit(1);
       }
        fclose(fp1);
    }
    printf("placing\n");
    FILE *fp2 = fopen(newstr2, "w");
    printf("file to write to = %s\n", newstr2);
    if (fp2 != NULL){
        fputs(message2, fp2);
        printf("%s\n", message2); //message 2
        if(fp2 == NULL){
           printf("Error opening file!");
           exit(1);
       }
        fclose(fp2);
    }
    printf("you are leaving put\n");

    //free(message2);
    //free(message1);
    free(newstr1);
    free(newstr2);
    free(source);
}
//-------------------------------------- get() ------------------------------------------------


void get(char * messageSize, int new1, char * serverFolder){
}
//-------------------------------------- main() ------------------------------------------------

int main(int argc, char *argv[]){
    struct config configcount;
    configcount = dfsConf();
    char * serverFolder;
    serverFolder = argv[1];

    //struct type server, contains port to which the server is bound locally
    //and client information connecting to us
    struct sockaddr_in server1;
    struct sockaddr_in client1;
   
    int sock1; // server socket
    int new1;  // client socket
    char * portnum = argv[2];
    //printf("%s\n", portnum);

    unsigned int sockaddr_len  = sizeof(struct sockaddr_in); //size of the socket calls //////////////? need multiple?? 

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
       
        //READS HEADER FROM CLIENT
        //PASSWORD HANDLING, REQUEST TYPE, MESSAGE SIZE
			
		//Forking and keeping while open
        if(!fork() ){ 
            close(sock1);
            while(1){
                char *filename1 = calloc(256,sizeof(char));
                char *filename2 = calloc(256,sizeof(char));
                char *username = calloc(256,sizeof(char));
                char *password = calloc(256,sizeof(char));
                char *reqType = calloc(256,sizeof(char));
                char *messageSize = calloc(256,sizeof(char));
                char *subfolderOnOf = calloc(256,sizeof(char));
                char *subfolder = calloc(256,sizeof(char));
                char *filename = calloc(256,sizeof(char));

                // char filename1[256];
                // char filename2[256];
                // char username[256];
                // char password[256];
                // char reqType[256];
                // char messageSize[256];
                // char subfolderOnOf[256];
                // char subfolder[256];
                // char filename[256];

                // read(new1, filename1, 256);                
                read(new1, username, 256);
                read(new1, password, 256); 
                read(new1, reqType, 256);
                read(new1, messageSize, 256);
                read(new1, subfolderOnOf, 256);
                read(new1, subfolder, 256);
                read(new1, filename, 256);

                //CHECK IF AUTHORIZED
                int check = 0;

                printf("username is = %s\n", username);
                printf("password is = %s\n", password);
                printf("filename is = %s\n", filename);
                printf("subfolder is = %s\n", subfolder);
                if(strcmp(username,configcount.userOne) == 0){
                    if(strcmp(password,configcount.passOne) == 0){
                        check = 1;
                    }
                    else{
                        check = 0;
                    }
                }
                //is the user 2 the same? and if so
                else if(strcmp(username,configcount.userTwo) == 0){
                    //check the password is the same
                    if (strcmp(password, configcount.passTwo) == 0){
                        //if both are correct, then its valid user/pass
                        check = 1;
                    }      
                    else{ //its not the right password and invalid
                        check = 0;
                    }
                }
                else{ //its invalid user and pass and send to user
                    check = 0;
                }

                //sending a valid or invalid to user
                if(check == 0){
                    send(new1, "Inval", 5, 0);
                    close(new1);
                }
                else{
                    send(new1, "Valid", 5, 0);
                    if (strcmp(reqType, "GET") == 0){
                        printf("valid GET request\n");
                       // get();
                       get(messageSize, new1, serverFolder);   
                    }
                    else if (strcmp(reqType, "PUT") == 0){
                        printf("valid PUT request\n");
                       // printf("%d\n", subfoldering);
                        put(filename1, filename2, username, password, reqType, 
                            messageSize, subfolderOnOf, subfolder, serverFolder, filename, new1);
                        printf("you have left put\n");
                    }
                    else if (strcmp(reqType, "LIST") == 0){
                        printf("valid LIST request\n");
                    }
                    printf("free vars\n");
                    free(filename1);
                    free(filename2);
                    free(username);
                    free(password);
                    free(reqType);
                    free(messageSize);
                    free(subfolderOnOf);
                    free(subfolder);
                    free(filename);
                    close(new1);
                    exit(0);
                    break;
                } 
            }
        }
    }
    
    //printf("\nClient Disconnected\n");
    //close(new4);
    int pid;
    do{
        pid = wait(NULL);
    }while(pid != -1);
    return 0;

}


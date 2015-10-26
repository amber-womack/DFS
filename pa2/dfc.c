//to compile : gcc -Wall myclient.c -o myclient -lcrypto -lssl
//gcc -Wall myserver.c -o myserver -lcrypto -lssl
//Reference - http://stackoverflow.com/questions/10324611/how-to-calculate-the-md5-hash-of-a-large-file-in-c

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


int dfs1;
int dfs2;
int dfs3;
int dfs4;

struct config {
    char * dfsOne;
    char * dfsTwo;
    char * dfsThree; 
    char * dfsFour;
    char * userName;
    char * password;
};
//-------------------------------------- encryption() ------------------------------------------------

// encryption
char* xorencrypt(char *string, char *key){
    int length = strlen(key);
    int i = 0;
    while(i < strlen(string))
    {
        string[i] = string[i] ^ key[i % length];
        i++;
    }
    return string;
}

//-------------------------------------- dfcConf() ------------------------------------------------
//struct to parse the dfc.conf file
struct config dfcConf(char * conf){  

    struct config configfile;
    char * line;
    int parsecount = 0;
    char * source = malloc(MAX_DATA);

    //open the file, error if not
    FILE *fp = fopen(conf, "r");
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
    //loop through the file, seperating on the : and new lines
    //the first 4 are the portnumbers
     while((line = strsep(&source, "\n")) != NULL){             
        if(parsecount == 0){
            configfile.dfsOne = strchr(line, ':');
            configfile.dfsOne++;
            printf("dfs1 = %s\n", configfile.dfsOne);
        }
        else if(parsecount == 1){
            configfile.dfsTwo = strchr(line, ':');
            configfile.dfsTwo++;
            printf("dfs2 = %s\n", configfile.dfsTwo);
        }
        else if(parsecount == 2){
            configfile.dfsThree = strchr(line, ':');
            configfile.dfsThree++;
            printf("dfs3 = %s\n", configfile.dfsThree);
        }
        else if(parsecount == 3){
            configfile.dfsFour = strchr(line, ':');
            configfile.dfsFour++;
            printf("dfs4 = %s\n", configfile.dfsFour);
        }
        //these are the usernames and passwords
        else if(parsecount == 4){
            configfile.userName = strchr(line, ':');
            configfile.userName++;
            printf("username = %s\n", configfile.userName);
        }
        else if(parsecount == 5){
            configfile.password = strchr(line, ':');
            configfile.password++;
            printf("password = %s\n", configfile.password);
        }
        parsecount++;
    }
    free(source);

    //then return the struct
    return configfile;
}
//-------------------------------------- connectionport() ------------------------------------------------


//COONECT THE PORT AND IP ADDRESS 
int connectport(const char *portnumber){

    struct sockaddr_in remote_server;
    int sock;
    
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR){
        perror("socket");
        exit(-1);
    }

    remote_server.sin_family = AF_INET;
    remote_server.sin_port = htons(atoi(portnumber)); //portnumber
    remote_server.sin_addr.s_addr = inet_addr("127.0.0.1"); //ip address
    bzero(&remote_server.sin_zero, 8);

    if((connect(sock, (struct sockaddr *) &remote_server, sizeof(struct sockaddr_in))) == ERROR){
        perror("connect");
        exit(-1);
    }
    return sock;
}
//-------------------------------------- hash() ------------------------------------------------

int hash(char * filename){

    int bytes;
    unsigned char data[1024];
    //int i = 0;
    int md5hash;
    unsigned char c[MD5_DIGEST_LENGTH];
    //char *filename="1.txt";
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    //md5 hash of a large file, provided the string 
    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename);
        return 0;
    }
    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0){
        MD5_Update (&mdContext, data, bytes);
    }
    MD5_Final (c,&mdContext);
    // while(i < MD5_DIGEST_LENGTH) 
    // {
    //     printf("%02x\n", c[i]);
    //     i++;
    // }
    md5hash = c[MD5_DIGEST_LENGTH - 1] % 4;
    //printf ("\n%d\n", md5hash);
    //printf ("%s\n", filename);
    fclose (inFile);
    return md5hash;
}
//-------------------------------------- put() ------------------------------------------------

//￼PUT command uploads files onto DFS
void put(char * filename, char * requestType, struct config configcount, int subfoldering, char * subfolder){
    FILE *fp_original;
    long int filesize;
   // struct stat st;

    fp_original=fopen(filename, "r");
    if(fp_original == NULL ) {
        // Error, as expected.
        perror( "Error opening file original\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }

    // Find the size of the file (using fseek and ftell). 
    fseek(fp_original, 0, 2);
    filesize = ftell(fp_original);
   ////// char *originalFile = calloc(filesize, 1);

    printf("the size of the file in bytes is : %ld\n", filesize);
    //Then seek back to the beginning
    rewind(fp_original);
    // Determine the size you need to write to each file (size / n)
    //k = size/4;
   // printf("%li\n", k);

    // stat(filename, &st);
    // long int bigsize = st.st_size*2;
    // char *originalFile = calloc(bigsize, 1);
    // fread(originalFile, 1, bigsize, fp_original);
    // originalFile = xorencrypt(originalFile, configcount.password);
    // filesize = strlen(originalFile);

    
   ///// // fread(originalFile, 1, filesize, fp_original);
   ///// // originalFile = xorencrypt(originalFile, configcount.password);
    // printf("encrypt%s\n", originalFile);
    // originalFile = xorencrypt(originalFile, configcount.password);
    // printf("decrypt%s\n", originalFile);

    //filesize = strlen(originalFile);
    // Read one character at a time (use getc) and write to the corresponding file (putc)
    // Use sprintf(fname, "%d.txt", index) to build names like "1.txt"
    // Use fopen to open files and keep a FILE *current to which you write at each step
    
    char c;
    int piecesize;
    int piecesizelast;

    char *piece_name1 = calloc(256,1);
    char *piece_name2 = calloc(256,1);
    char *piece_name3 = calloc(256,1);
    char *piece_name4 = calloc(256,1);

    strcat(piece_name1, ".");
    strcat(piece_name1, filename);
    strcat(piece_name1, ".1");

    strcat(piece_name2, ".");
    strcat(piece_name2, filename);
    strcat(piece_name2, ".2");

    strcat(piece_name3, ".");
    strcat(piece_name3, filename);
    strcat(piece_name3, ".3");

    strcat(piece_name4, ".");
    strcat(piece_name4, filename);
    strcat(piece_name4, ".4");

    FILE * fp_piece1;
    FILE * fp_piece2;
    FILE * fp_piece3;
    FILE * fp_piece4;

    printf("%s\n", piece_name1);
    printf("%s\n", piece_name2);
    printf("%s\n", piece_name3);
    printf("%s\n", piece_name4);

    fp_piece1 = fopen(piece_name1, "w");
    fp_piece2 = fopen(piece_name2, "w");
    fp_piece3 = fopen(piece_name3, "w");
    fp_piece4 = fopen(piece_name4, "w");

    if(fp_piece1 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 1\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }
    if(fp_piece2 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 2\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }
    if(fp_piece3 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 3\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }
    if(fp_piece4 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 4\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }

    //GETTING EACH PIECE OF THE FILE BY SPLITTING
    if(filesize % 4 == 0){
        printf("In even loop\n");

        piecesize = filesize / 4;
        piecesizelast = piecesize;
        int i;
        // piece 1
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece1);
        }

        // piece 2
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece2);
        }

        // piece 3
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece3);
        }

        // piece 4
        for(i=0; i < piecesizelast; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece4);
        }


    }
    else if(filesize % 4 == 1){
        printf("In +1 loop\n");

        piecesize = filesize / 4;
        piecesizelast = piecesize + 1;

        int i;
        
        // piece 1
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece1);
        }

        // piece 2
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece2);
        }

        // piece 3
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece3);
        }

        // piece 4
        for(i=0; i < piecesizelast; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece4);
        }
    }
    else if(filesize % 4 == 2){
        printf("In +2 loop\n");

        piecesize = filesize / 4;
        piecesizelast = piecesize + 2;

        int i;
        
        // piece 1
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece1);
        }

        // piece 2
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece2);
        }

        // piece 3
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece3);
        }

        // piece 4
        for(i=0; i < piecesizelast; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece4);
        }
    }
    else if(filesize % 4 == 3){

        printf("In +3 loop\n");

        piecesize = filesize / 4;
        piecesizelast = piecesize + 3;

        int i;
        
        // piece 1
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece1);
        }

        // piece 2
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece2);
        }

        // piece 3
        for(i=0; i < piecesize; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece3);
        }

        // piece 4
        for(i=0; i < piecesizelast; i++) {
            c = fgetc(fp_original);
            fputc(c, fp_piece4);
        }
    }

    //close
    fclose(fp_piece1);
    fclose(fp_piece2);
    fclose(fp_piece3);
    fclose(fp_piece4);
    fclose(fp_original);
    
    //HAD TO CLOSE AND REOPEN THE FILE
    fp_piece1 = fopen(".1.txt.1", "rb");
    fp_piece2 = fopen(".1.txt.2", "rb");
    fp_piece3 = fopen(".1.txt.3", "rb");
    fp_piece4 = fopen(".1.txt.4", "rb");

    if(fp_piece1 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 1-2\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }
    if(fp_piece2 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 2-2\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }
    if(fp_piece3 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 3-2\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }
    if(fp_piece4 == NULL ) {
        // Error, as expected.
        perror( "Error opening file piece 4-2\n" );
        printf( "Error code opening file: %d\n", errno );
        printf( "Error opening file: %s\n", strerror( errno ) );
        exit(-1);
    }


    char msgSize[15];
    sprintf(msgSize, "%d", piecesize);
    printf("Subfolder in PUT = %s\n", subfolder);
    //SEND THE HEADERS
    // send(dfs1, piece_name1, 256, 0);

    printf("before send server 1\n");
    send(dfs1, configcount.userName, 256, 0);
    send(dfs1, configcount.password, 256, 0);
    send(dfs1, requestType, 256, 0);
    send(dfs1, msgSize, 256, 0);
    if(subfoldering == 1){
        send(dfs1, "Subfoldering On", 256, 0);
    }
    else{
        send(dfs1, "Subfoldering Of", 256, 0);
    }
    send(dfs1, subfolder,256, 0);
    send(dfs1, filename, 256, 0);

    printf("before send server 2\n");
   // send(dfs2, piece_name2, 256, 0);
    send(dfs2, configcount.userName, 256, 0);
    send(dfs2, configcount.password, 256, 0);
    send(dfs2, requestType, 256, 0);
    send(dfs2, msgSize, 256, 0);
    if(subfoldering == 1){
        send(dfs2, "Subfoldering On", 256, 0);
    }
    else{
        send(dfs2, "Subfoldering Of", 256, 0);
    }
    send(dfs2, subfolder,256, 0);
    send(dfs2, filename, 256, 0);


    printf("before send server 3\n");
   // send(dfs3, piece_name3, 256, 0);
    send(dfs3, configcount.userName, 256, 0);
    send(dfs3, configcount.password, 256, 0);
    send(dfs3, requestType, 256, 0);
    send(dfs3, msgSize, 256, 0);
    if(subfoldering == 1){
        send(dfs3, "Subfoldering On", 256, 0);
    }
    else{
        send(dfs3, "Subfoldering Of", 256, 0);
    }
    send(dfs3, subfolder,256, 0);
    send(dfs3, filename, 256, 0);

    printf("before send server 4\n");
   // send(dfs4, piece_name4, 256, 0);
    printf("send 1\n");
    send(dfs4, configcount.userName, 256, 0);
    printf("send 2\n");
    send(dfs4, configcount.password, 256, 0);
    printf("send 3\n");
    send(dfs4, requestType, 256, 0);
    printf("send 4\n");
    send(dfs4, msgSize, 256, 0);
    if(subfoldering == 1){
        printf("send 5\n");
        send(dfs4, "Subfoldering On", 256, 0);
    }
    else{
        printf("send 5\n");
        send(dfs4, "Subfoldering Of", 256, 0);
    }
    printf("send 6\n");
    send(dfs4, subfolder,256, 0);
    printf("send 7\n");
    send(dfs4, filename, 256, 0);

    char *  newstr1 = calloc(5,1);
    char *  newstr2 = calloc(5,1);
    char *  newstr3 = calloc(5,1);
    char *  newstr4 = calloc(5,1);
    printf("before reads");
    //TO ADD THE USERNAMES 
    read(dfs1, newstr1, 5);
    read(dfs2, newstr2, 5);
    read(dfs3, newstr3, 5);
    read(dfs4, newstr4, 5);
    if(strcmp(newstr1,"Inval") == 0){
        printf("Invalid Username/Password. Plese Try Again\n" );
    }
    printf("after reads");
    char bufferpiece1[piecesize];
    char bufferpiece2[piecesize];
    char bufferpiece3[piecesize];
    char bufferpiece4[piecesize];

    //PUT FILENAME INTO HASH
    int serverDest;
    serverDest = hash(filename);
    printf("HSH X = %d\n", serverDest); 

    //WHICH PAIR GOES TO WHICH SERVER BY MD5H
    if(serverDest == 0){
        //piece1
        printf("\nPiece 1, Server 1, 4\n");

        fread(bufferpiece1, 1, piecesize, fp_piece1);
        send(dfs1, piece_name1, 256, 0); //sending filename
        send(dfs1, bufferpiece1, piecesize, 0);
        send(dfs4, piece_name1, 256, 0); //sending filename
        send(dfs4, bufferpiece1, piecesize, 0);
        

        //piece2
        printf("\nPiece 2, Server 1, 2\n");

        fread(bufferpiece2, 1, piecesize, fp_piece2);
        send(dfs1, piece_name2, 256, 0); //sending filename
        send(dfs1, bufferpiece2, piecesize, 0);
        send(dfs2, piece_name2, 256, 0); //sending filename
        send(dfs2, bufferpiece2, piecesize, 0);
        

        // piece 3
        printf("\nPiece 3, Server 2, 3\n");

        fread(bufferpiece3, 1, piecesize, fp_piece3);
        send(dfs2, piece_name3, 256, 0); //sending filename
        send(dfs2, bufferpiece3, piecesize, 0);
        send(dfs3, piece_name3, 256, 0); //sending filename
        send(dfs3, bufferpiece3, piecesize, 0);
        
        // piece4
        printf("\nPiece 4, Server 3, 4\n");

        fread(bufferpiece4, 1, piecesizelast, fp_piece4);
        send(dfs4, piece_name4, 256, 0); //sending filename
        send(dfs4, bufferpiece4, piecesizelast, 0);
        send(dfs3, piece_name4, 256, 0); //sending filename
        send(dfs3, bufferpiece4, piecesizelast, 0);
    }
    else if(serverDest == 1){
        //piece1
        printf("\nPiece 1, Server 1, 2\n");

        fread(bufferpiece1, 1, piecesize, fp_piece1);
        send(dfs1, piece_name1, 256, 0); //sending filename
        send(dfs1, bufferpiece1, piecesize, 0);
        send(dfs2, piece_name1, 256, 0); //sending filename
        send(dfs2, bufferpiece1, piecesize, 0);
        

        //piece2
        printf("\nPiece 2, Server 2, 3\n");

        fread(bufferpiece2, 1, piecesize, fp_piece2);
        send(dfs2, piece_name2, 256, 0); //sending filename
        send(dfs2, bufferpiece2, piecesize, 0);
        send(dfs3, piece_name2, 256, 0); //sending filename
        send(dfs3, bufferpiece2, piecesize, 0);
        
        

        // piece 3
        printf("\nPiece 3, Server 3, 4\n");

        fread(bufferpiece3, 1, piecesize, fp_piece3);
        send(dfs4, piece_name3, 256, 0); //sending filename
        send(dfs4, bufferpiece3, piecesize, 0);
        send(dfs3, piece_name3, 256, 0); //sending filename
        send(dfs3, bufferpiece3, piecesize, 0);
        
        
        // piece4
        printf("\nPiece 4, Server 4, 1\n");

        fread(bufferpiece4, 1, piecesizelast, fp_piece4);
        send(dfs1, piece_name4, 256, 0); //sending filename
        send(dfs1, bufferpiece4, piecesizelast, 0);
        send(dfs4, piece_name4, 256, 0); //sending filename
        send(dfs4, bufferpiece4, piecesizelast, 0);
    }
    else if(serverDest == 2){
       // printf("\nServer Destination 1, piece 1\n");
        //read a size and keep reading it. 
        // ssize_t bytes_sent;
        // do {
        //     bytes_sent = send(dfs1, buffer, piecesize, 0);
        // } while (bytes_sent > 0);
       
        //piece1
        printf("\nPiece 1, Server 2, 3\n");

        fread(bufferpiece1, 1, piecesize, fp_piece1);
        send(dfs2, piece_name1, 256, 0); //sending filename
        send(dfs2, bufferpiece1, piecesize, 0);
        send(dfs3, piece_name1, 256, 0); //sending filename
        send(dfs3, bufferpiece1, piecesize, 0);

        //piece2
        printf("\nPiece 2, Server 3, 4\n");

        fread(bufferpiece2, 1, piecesize, fp_piece2);
        send(dfs4, piece_name2, 256, 0); //sending filename
        send(dfs4, bufferpiece2, piecesize, 0);
        send(dfs3, piece_name2, 256, 0); //sending filename
        send(dfs3, bufferpiece2, piecesize, 0);
        

        // piece 3
        printf("\nPiece 3, Server 1, 4\n");

        fread(bufferpiece3, 1, piecesize, fp_piece3);
        send(dfs1, piece_name3, 256, 0); //sending filename
        send(dfs1, bufferpiece3, piecesize, 0);
        send(dfs4, piece_name3, 256, 0); //sending filename
        send(dfs4, bufferpiece3, piecesize, 0);
        
        // piece4
        printf("\nPiece 4, Server 1, 2\n");

        fread(bufferpiece4, 1, piecesizelast, fp_piece4);
        send(dfs1, piece_name4, 256, 0); //sending filename
        send(dfs1, bufferpiece4, piecesizelast, 0);
        send(dfs2, piece_name4, 256, 0); //sending filename
        send(dfs2, bufferpiece4, piecesizelast, 0);
    }
    //--------------------------------------
        else if(serverDest == 3){
      
        //piece1
        printf("\nPiece 1, Server 3, 4\n");

        fread(bufferpiece1, 1, piecesize, fp_piece1);
        send(dfs4, piece_name1, 256, 0); //sending filename
        send(dfs4, bufferpiece1, piecesize, 0);
        send(dfs3, piece_name1, 256, 0); //sending filename
        send(dfs3, bufferpiece1, piecesize, 0);
        

        //piece2
        printf("\nPiece 2, Server 4, 1\n");

        fread(bufferpiece2, 1, piecesize, fp_piece2);
        send(dfs1, piece_name2, 256, 0); //sending filename
        send(dfs1, bufferpiece2, piecesize, 0);
        send(dfs4, piece_name2, 256, 0); //sending filename
        send(dfs4, bufferpiece2, piecesize, 0);
        

        // piece 3
        printf("\nPiece 3, Server 1, 2\n");

        fread(bufferpiece3, 1, piecesize, fp_piece3);
        send(dfs1, piece_name3, 256, 0); //sending filename
        send(dfs1, bufferpiece3, piecesize, 0);
        send(dfs2, piece_name3, 256, 0); //sending filename
        send(dfs2, bufferpiece3, piecesize, 0);
        
        // piece4
        printf("\nPiece 4, Server 2, 3\n");

        fread(bufferpiece4, 1, piecesizelast, fp_piece4);
        send(dfs2, piece_name4, 256, 0); //sending filename
        send(dfs2, bufferpiece4, piecesizelast, 0);
        send(dfs3, piece_name4, 256, 0); //sending filename
        send(dfs3, bufferpiece4, piecesizelast, 0);
    }
   
    fclose(fp_piece1);
    fclose(fp_piece2);
    fclose(fp_piece3);
    fclose(fp_piece4);
    free(piece_name1);
    free(piece_name2);
    free(piece_name3);
    free(piece_name4);
    free(newstr1);
    free(newstr2);
    free(newstr3);
    free(newstr4);
}
//-------------------------------------- get() ------------------------------------------------

// GET command downloads all available pieces of a file from all available DFS, if
// the file is reconstructable then write the file into your working folder. If the
// file is not reconstructable, then print “File is incomplete.”
void get(char * filename, char * requestType, struct config configcount, int subfoldering, char * subfolder){

    char msgSize[15];
    // sprintf(msgSize, "%d", filename);
    //printf("Subfolder in GET = %s\n", subfolder);

    send(dfs1, configcount.userName, 256, 0);
    send(dfs1, configcount.password, 256, 0);
    send(dfs1, requestType, 256, 0);
    send(dfs1, msgSize, 256, 0);
    if(subfoldering == 1){
        send(dfs1, "Subfoldering On", 256, 0);
    }
    else{
        send(dfs1, "Subfoldering Of", 256, 0);
    }
    send(dfs1, subfolder,256, 0);
    send(dfs1, filename, 256, 0);

   // send(dfs2, piece_name2, 256, 0);
    send(dfs2, configcount.userName, 256, 0);
    send(dfs2, configcount.password, 256, 0);
    send(dfs2, requestType, 256, 0);
    send(dfs2, msgSize, 256, 0);
    if(subfoldering == 1){
        send(dfs2, "Subfoldering On", 256, 0);
    }
    else{
        send(dfs2, "Subfoldering Of", 256, 0);
    }
    send(dfs2, subfolder,256, 0);
    send(dfs2, filename, 256, 0);


   // send(dfs3, piece_name3, 256, 0);
    send(dfs3, configcount.userName, 256, 0);
    send(dfs3, configcount.password, 256, 0);
    send(dfs3, requestType, 256, 0);
    send(dfs3, msgSize, 256, 0);
    if(subfoldering == 1){
        send(dfs3, "Subfoldering On", 256, 0);
    }
    else{
        send(dfs3, "Subfoldering Of", 256, 0);
    }
    send(dfs3, subfolder,256, 0);
    send(dfs3, filename, 256, 0);


   // send(dfs4, piece_name4, 256, 0);
    send(dfs4, configcount.userName, 256, 0);
    send(dfs4, configcount.password, 256, 0);
    send(dfs4, requestType, 256, 0);
    send(dfs4, msgSize, 256, 0);
    if(subfoldering == 1){
        send(dfs4, "Subfoldering On", 256, 0);
    }
    else{
        send(dfs4, "Subfoldering Of", 256, 0);
    }
    send(dfs4, subfolder,256, 0);
    send(dfs4, filename, 256, 0);
}
//-------------------------------------- list() ------------------------------------------------

//￼LIST command inquires what file is stored on DFS servers, and print file
//names stored under Username on DFS servers
void list(){

}

//-------------------------------------- main() ------------------------------------------------
int main(int argc, char *argv[]){
    char * line;
    int parsecount = 0;
    char * source;
    char * piece;
    char * filename = NULL;
    char * request;
    char * subfolder;
    char * buf = malloc(MAX_DATA);
    char * conf;
    int subfoldering = 0;

    //dfc.conf from command line, then to be placed for parsing
    conf = argv[1];
    //call parsing function of dfc.conf
    struct config configcount;
    configcount = dfcConf(conf);
   
   //READ IN THE FILE
    while(fgets(buf, MAX_DATA, stdin)) {
        //connect the portnumber of each server from the config file
        dfs1 = connectport(configcount.dfsOne);
        dfs2 = connectport(configcount.dfsTwo);
        dfs3 = connectport(configcount.dfsThree);
        dfs4 = connectport(configcount.dfsFour);

        buf[strlen(buf) - 1] = '\0';    /* insure line null-terminated  */
       
        source = buf;
        if (subfoldering == 0){
            //parse the GET, PUT, or LIST REQUEST and the filename
            while((line = strsep(&source, "\0")) != NULL){
                while((piece = strsep(&line, " ")) != NULL){
                    if(parsecount == 0){
                        request = piece;
                        printf("Request type = %s\n", request);
                    }
                    else{

                        filename = piece;
                        printf("Filename = %s\n", filename);
                    }
                    parsecount++; 
                }
            }
            parsecount = 0;
        }
        else{ //means MKDIR turned on
            //parse the GET, PUT, or LIST REQUEST and the filename
            while((line = strsep(&source, "\0")) != NULL){
                while((piece = strsep(&line, " ")) != NULL){
                    if(parsecount == 0){
                        request = piece;
                        printf("Request type = %s\n", request);
                    }
                    else if (parsecount == 1){
                        filename = piece;
                        printf("Filename = %s\n", filename);
                    }
                    else{
                        subfolder = piece;
                        printf("Subfolder = %s\n", subfolder);
                    }
                    parsecount++; 
                }
            }
            parsecount = 0;
            subfoldering = 1;
        }
        //check if the request is valid or not, and if the filename is valid or not
        if (strcmp(request, "GET") == 0){
            printf("valid GET request\n");
            if(filename == NULL){
                printf("invalid file for GET\n");
                exit(-1);
            } 
           get(filename, "GET", configcount, subfoldering, subfolder);    
        }
        else if (strcmp(request, "PUT") == 0){
            printf("valid PUT request\n");
            if(filename == NULL){
                printf("invalid file for PUT\n");
                exit(-1);
            }
           // printf("%d\n", subfoldering);
            put(filename, "PUT", configcount, subfoldering, subfolder);
        }
        else if (strcmp(request, "LIST") == 0){
            printf("valid LIST request\n");
        }
        else if (strcmp(request, "MKDIR") == 0){\
            if(subfoldering == 0){
                subfoldering = 1;
                printf("Subfoldering Active\n");
            }
            else{
                subfoldering = 0;
                printf("subfoldering InActive\n");
            }
        }
        else{
            printf("Invalid request sent\n");
        }
        filename = NULL;
    }
    printf("outside while");

    //close all the ports 
    close(dfs1);
    close(dfs2);
    close(dfs3);
    close(dfs4);
    free(buf);
    return 0;
}


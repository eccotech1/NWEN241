
#include<stdio.h>
#include<string.h> //copy string 
#include<stdlib.h> //exit()
#include<sys/socket.h> //socket
#include<netinet/in.h> // socket address struct sockaddr_in 
#include<unistd.h> //read and write




void error(const char *msg){

    printf("Error: %s\n", msg);
    exit(1);
}
void open_file(char* filename, char mode[], int clfd, char* incoming ){
    FILE *f; //file
    f = fopen(filename, "r"); //read file
    char msg[10000]; //message incoming
    char charc; //each char
    if(f == NULL){ //no file found
        ssize_t sCLient = send(clfd, "SERVER 404 Not Found\n", strlen("SERVER 404 Not Found\n"), 0);
        return; //no file name provided
    }
    memset(incoming, '\0', sizeof(incoming)); //clear
    int index = 0; 
    while (!feof(f)){ //not at the end of file
        charc = fgetc(f);
        msg[index] = charc;
        index++;
    }
    msg[index-1] = '\0';
    //strcpy(("SERVER 200 OK\n \n %s \n \n \n", msg), msg);
    ssize_t sClient = send(clfd, "SERVER 200 OK\n\n", strlen("SERVER 200 OK\n\n"),0); //print message
    ssize_t sClient1 = send(clfd, msg, strlen(msg),0);
    ssize_t sClient2= send(clfd, "\n\n\n", strlen("\n\n\n"),0);
    fclose(f);
    
}
void writeToFile(FILE *out, int sockfd, int clientfd, int readingPos, char *incoming, char *putIncoming) 
{   
    int killCommand = 0;           
    int counter = 0;

    while (killCommand == 0) { //check if the put has been successful
        memset(putIncoming, '\0', sizeof(putIncoming)); // clear

        ssize_t receivingPut = recv(clientfd, putIncoming, sizeof(putIncoming), 0); // get message from client
        if(receivingPut <= 0) { //put command error
            error("receiving Put Command");
            close(clientfd);      //boot client          
            exit(0); //close
        }
        
        for (int i = 0; i < sizeof(putIncoming); i++) { //run through the 
            if (putIncoming[i] != '\0') {
                if (putIncoming[0] == '\n') { //go to next char
                    counter++;
                } else {
                    counter = 0;
                }

                fprintf(out, "%c", putIncoming[i]);
                if (counter == 2) { //check if it has 2 empty lines
                    fclose(out);
                    ssize_t sCLient = send(clientfd, "Server 201 Created\n", strlen("Server 201 Created\n"), 0); //send this client
                    printf("\ndone! \n"); //display done in server
                    memset(putIncoming, '\0', sizeof(putIncoming)); //clear memory
                    killCommand = 1; // file has been created, do other stuff
                }
            } 
        }
    }

}
int main(int argc, char *argv[]){
    char outPut[10000];

    if(argc == 1){
        printf("Terminated - no port");
        return -1;
    }

    int tempValue;
    sscanf(argv[1], "%d",&tempValue);

    if(tempValue < 1024){
        printf("Terminated - bad port");
        return -1;
    }

        // Step 1 - create a socket
    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        error("Error creating socket");
    }
    printf("Socket Created\n");


        //Step 2 - bind socket to an address
        //define IP and Port 
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr= INADDR_ANY;
    serveraddr.sin_port=htons(tempValue);

    printf("Address created\n");


        //bind address to socket
    int br;
    br = bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(br<0){
     error("Error Binding");
 }
 printf("Bind successful\n");

        //Step 3 - listen for connection requests
 if(listen(sockfd,SOMAXCONN)<0){
    error("Litening error");
    exit(0);
}
printf("Success listening\n");

while(1){


    struct sockaddr_in clientaddr;
    int clienlen = sizeof(clientaddr);
    char *fname = "";
    int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,(socklen_t*)&clientaddr);

    if(clientfd<0){
        error("Error accepting client");
    }

    printf("Client connection accepted\n");

                            // write to client
    char buffer[100];
    memset(buffer,0,100);
    strncpy(buffer,"HELLO\n",strlen("HELLO\n"));

    int s = write(clientfd,buffer,100);
    if(s<0){
        error("write error");
    }
    printf("Write successful\n");
                            //read from client
    int killClient = 0;
    while(killClient != 1){
        char incoming[100];
        int r = recv(clientfd, incoming, 100, 0);
        if(r <= 0) {
            printf("Error receiving message");
            close(clientfd);
            close(sockfd);
            exit(0);
        }
                            // Do something with receiving message
        printf("Received message: %s", incoming);
        char temp[100];

        if((incoming[0]=='b' || incoming[0]=='B') && (incoming[1] == 'y' || incoming[1]=='Y') && (incoming[2]== 'e' || incoming[2]=='E')){
            printf("Closing client connection\n");
            printf("Listening...\n");
            killClient = 1;
            close(clientfd);

        }
        else if((incoming[0]=='g' || incoming[0]=='G') && (incoming[1] == 'e' || incoming[1]=='E') && (incoming[2]== 't' || incoming[2]=='T')){
            char fname[100]; //file name
            int fFound=0; //file found then exit
            memset(fname,'\0', sizeof(fname)); //clear 
            if(incoming[3] == ' '){ //space on 4 char
                int i = 4;
                while (i < strlen(incoming)){
                    fname[i-4] = incoming[i];
                    i++;
                }
                fname[i-5] ='\0';
                if(strlen(fname) > 1){
                    fFound = 1; //file has been found
                }
            }
            if(fFound == 1){
                open_file(fname, "r", clientfd, incoming); //read file
            }else {
                ssize_t sCLient = send(clientfd, "SERVER 500 Get Error\n", strlen("SERVER 500 Get Error\n"), 0); //get command error
            }

        }
        else if((incoming[0]=='p' || incoming[0]=='P') && (incoming[1] == 'u' || incoming[1]=='U') && (incoming[2]== 't' || incoming[2]=='T')){ 


            char fName[100];
            int fNameFound = 0;
            memset(fName, '\0', sizeof(fName)); //clear
            int index = 0; //char pos 

            if (incoming[3] == ' ') { // after space check fname                  
                int i=4;
                while (incoming[i] != '\0' && incoming[i] != ' ' && incoming[i] != '\n') {                          
                    fName[i-4] = incoming[i];
                    i++;
                    index = i+1; //space char
                }

                fName[i-4] = '\0';
                fNameFound = 1;
            }
            if (fNameFound == 1) {
                printf("Writing to: %s\n", fName); 
                FILE *out = fopen(fName, "w");
                if (out == NULL){
                    ssize_t sCLient = send(clientfd, "SERVER 501 Put Error\n", strlen("SERVER 501 Put Error\n"), 0);
                    return -1;
                }

                for(int i = index; i < sizeof(incoming); i++){
                    if (incoming[i] != '\0') {
                            if (!(i == 0 && incoming[i] == '\n')) { //if empty line then
                                fprintf(out, "%c", incoming[i]); //write content to file starting from the line

                            } 
                        }
                    }
                    writeToFile(out, sockfd, clientfd, index, incoming, outPut); // write to file 
                }else{
                    ssize_t sCLient = send(clientfd, "SERVER 502 Command Error\n", strlen("SERVER 502 Command Error\n"), 0); // else error

                }
            }
        }
    }
    return 0;


}//close(sockfd);

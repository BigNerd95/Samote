#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#include "samote.h"
#include "list.h"
#include "controlProtocol.h"
#include "RCR.h"

#define SERVER_PORT 55000

#define BUFFER_SIZE 4096

#define CONF_FILE "key_codes.conf"


void* rcr_main() {

    int client_sock, client_slen, read_size;
    struct sockaddr_in server, client;
    List* codeList;


    FILE* fp = fopen("/mtd_ram/RCR.log", "a+");

    fputs("RCR launched\n", fp);
    fflush(fp);

    FILE* fpConf = openFileConf(path, CONF_FILE);
    if (fpConf) {
        codeList = setCommandsFile(fpConf);
        //printList(codeList->first, fp);
        fclose(fpConf);
    } else {
        fprintf(fp, "Cannot open %s\n", CONF_FILE);
        fflush(fp);
        fclose(fp);
        return NULL; 
    }
    

    fputs("Starting server...\n", fp);
    fflush(fp);
     
    //Create TCP socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        fputs("Socket Error\n", fp);
        fflush(fp);
        fclose(fp);
        return NULL;
    }

    fputs("Socket created\n", fp);
    fflush(fp);
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);
     
    //Bind
    if (bind(socket_desc, (struct sockaddr*) &server, sizeof(server)) < 0) {
        fputs("Bind failed. Error\n", fp);
        fflush(fp);
        fclose(fp);
        return NULL;
    }

    fprintf(fp, "Binding on port %d\n", SERVER_PORT);
    fflush(fp);
     
    //Listen
    listen(socket_desc, 3); 
     
    //Accept incoming connections
    fputs("Waiting for incoming connections...\n", fp);
    fflush(fp);

    client_slen = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    while ((client_sock = accept(socket_desc, (struct sockaddr*) &client, (socklen_t*) &client_slen)) > 0){

    	fputs("New connection\n", fp);
        fflush(fp);

        pthread_t new_thread;
        struct threadArgs* targs = (struct threadArgs*) malloc(sizeof(struct threadArgs));
        targs->sock = client_sock;
        targs->list = codeList;

        if (pthread_create(&new_thread, NULL, connection_handler, (void*) targs) < 0){
            fputs("Could not create thread\n", fp);
            fflush(fp);
            fclose(fp);
            return NULL;
        }
    }
    

    fclose(fp);
     
    return NULL;
}



void* connection_handler(void* argsp) {
    //Get the socket descriptor
    struct threadArgs* args = argsp;
    int client_socket = args->sock, read_size;
    List* codeList = args->list;
    free(argsp);
    unsigned char* client_message = (unsigned char*) malloc(BUFFER_SIZE);
     
    //Receive a message from client
    while((read_size = recv(client_socket, client_message, BUFFER_SIZE-1, 0)) > 0) {

        client_message[read_size] = '\0';
        processPacket(client_socket, client_message, read_size, codeList);

    }

    free(client_message);
     
    if(read_size == 0) {
        puts("Client disconnected");
    } else if(read_size == -1){
        perror("recv failed");
    }
     
    return NULL;
}


FILE* openFileConf(char* path, char* filename){
    char* filePath = (char*) malloc(sizeof(char) * (strlen(path)+strlen(filename)+1) );
    strcpy(filePath, path);
    strcat(filePath, filename);
    FILE* fp = fopen(filePath, "r");
    free(filePath);
    return fp;
}

List* setCommandsFile(FILE* fp){

    List* codeList = newList();

    if (fp){
        char str[64];
        int code, lineRes;

        str[63] = '\0';
        do {
            lineRes = fscanf(fp, "%63s %i\n", str, &code);
            if (lineRes == 2){
                listAppend(codeList, str, code);
            }
        } while (lineRes != EOF);
    }

    return codeList;
}

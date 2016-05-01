#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#include "samote.h"
#include "SSDP.h"
#include "RCR.h"


void* pInstance = NULL;
int (*SendKey)(unsigned*, int) = NULL;
char* path = NULL;


void* samote_main(void* argsp) {

    FILE* fp = fopen("/mtd_ram/samote.log", "a+");

    fputs("Samote launched\n", fp);
    fflush(fp);

    // get the passed arguments and set them as global variable
    struct ri_args* args = argsp;
    pInstance = args->pInstance;
    SendKey = args->SendKey;
    path = args->path;

    free(argsp);

    fprintf(fp, "pInstance: %p\n", pInstance);
    fprintf(fp, "SendKey: %p\n", SendKey);
    fprintf(fp, "Path: %s\n", path);
    fflush(fp);



    /************************
    Run SSDP server as thread
    ************************/

    pthread_t new_thread1;
    pthread_create(&new_thread1, NULL, ssdp_main, NULL);

    fputs("SSDP launched\n", fp);
    fflush(fp);



    /************************
    Run UPNP server as thread
    ************************/

    // TODO
    // pthread_t new_thread2;
    // pthread_create(&new_thread2, NULL, upnp_main, NULL);



    /************************
    Run RCR server as thread
    ************************/

    pthread_t new_thread3;
    pthread_create(&new_thread3, NULL, rcr_main, NULL);

    fputs("RCR launched\n", fp);
    fflush(fp);
    

    fclose(fp);

    return NULL;

}



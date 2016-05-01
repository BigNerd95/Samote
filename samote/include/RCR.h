#ifndef _RCR_H_
#define _RCR_H_

#include "list.h"

struct threadArgs {
    int sock;
    List* list;
};

void* rcr_main();
void* connection_handler(void* argsp);
FILE* openFileConf(char* path, char* filename);
List* setCommandsFile(FILE* fp);

#endif

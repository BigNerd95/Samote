#ifndef _SAMPROTOCOL_H_
#define _SAMPROTOCOL_H_

#include "list.h"

void processPacket(int client_sock, char* buffer, int size, List* codeList);
int isRCSamsung(char* appName, int appNameLen);
void processCMD(char* payload, List* codeList);
void processKB(char* payload);
void processAuth(char* payload);

#endif
